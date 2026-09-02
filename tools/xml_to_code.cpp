/** xml_to_code - convert XML layout files to pure C++ initialization code
 *  Usage: xml_to_code [-g global.xml] [-r resource_folder ...] <output.cpp> <function_name> <input_xml_files...>
 *
 *  Parses XML at build time (via pugixml linked into this tool) and generates
 *  C++ code that creates the same UI at runtime via direct API calls -
 *  zero XML parsing needed at runtime.
 *
 *  The generated function signature is:
 *    void <function_name>(ui::Window* pWindow)
 *
 *  Build: clang++ -std=c++17 -O2 xml_to_code.cpp -lstdc++ -o xml_to_code
 *         (pugixml is header-only, include path must point to third_party/xml)
 *
 *  Supported XML features:
 *    - All control class names that WindowBuilder::CreateControlByClass supports
 *    - <Include src="x.xml" count="n"/>: inlined at generation time
 *    - <RichText>...<b>/<font>/<i>/<s>/<u>...</RichText>: rich text content
 *    - Unknown class names produce a WARNING (not silently skipped)
 *    - Event attributes (on_click/on_select/on_change) and <Event> tags are
 *      skipped; events must be wired manually after the generated Init call
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <cstring>
#include <sys/stat.h>

// ---- Minimal pugixml (header-only, included from dui) ----
#include "pugixml.hpp"

static std::string attr(const pugi::xml_node& node, const char* name,
                         const std::string& def = "") {
    const char* v = node.attribute(name).value();
    return v[0] ? v : def;
}

static std::string escapeCStr(const std::string& s) {
    std::string out;
    for (char ch : s) {
        switch (ch) {
        case '\\': out += "\\\\"; break;
        case '"':  out += "\\\""; break;
        case '\n': out += "\\n";  break;
        case '\r': break;
        case '\t': out += "\\t";  break;
        default:   out += ch;     break;
        }
    }
    return out;
}

// Escape text content for re-embedding in XML
static std::string escapeXml(const std::string& s) {
    std::string out;
    for (char ch : s) {
        switch (ch) {
        case '&':  out += "&amp;";  break;
        case '<':  out += "&lt;";   break;
        case '>':  out += "&gt;";   break;
        default:   out += ch;       break;
        }
    }
    return out;
}

static std::string nodeName(const pugi::xml_node& node) {
    return node.name();
}

// Serialize the children of a node (used for <RichText> content)
static std::string serializeChildren(const pugi::xml_node& node) {
    struct BufWriter : pugi::xml_writer {
        std::string data;
        virtual void write(const void* buffer, size_t size) override {
            data.append(static_cast<const char*>(buffer), size);
        }
    };
    std::string out;
    for (auto child : node.children()) {
        if (child.type() == pugi::node_pcdata) {
            out += escapeXml(child.value());
        }
        else if (child.type() == pugi::node_cdata) {
            out += child.value();   // CDATA content is used verbatim
        }
        else if (child.type() == pugi::node_element) {
            BufWriter w;
            child.print(w, "", pugi::format_raw);
            out += w.data;
        }
    }
    return out;
}

// Expand <Include src="..." count="n"/> nodes in-place before generation
static void expandIncludes(pugi::xml_node node, const std::string& currentDir, int depth) {
    if (depth > 16) return;
    for (auto child = node.first_child(); child; ) {
        pugi::xml_node current = child;
        child = child.next_sibling();   // advance before mutating the tree

        if (current.type() != pugi::node_element) continue;
        if (std::string(current.name()) != "Include") {
            expandIncludes(current, currentDir, depth + 1);
            continue;
        }

        std::string src = attr(current, "src");
        if (src.empty()) {
            std::cerr << "WARNING: <Include> without src attribute, skipped" << std::endl;
            current.parent().remove_child(current);
            continue;
        }
        int nCount = current.attribute("count").as_int();
        if (nCount <= 0) nCount = 1;

        std::string targetPath = src;
        if ((src[0] != '/') && !currentDir.empty()) {
            targetPath = currentDir + "/" + src;
        }
        pugi::xml_document incDoc;
        pugi::xml_parse_result result = incDoc.load_file(targetPath.c_str());
        if (!result) {
            std::cerr << "WARNING: <Include src=\"" << src << "\"> not found (" << targetPath
                      << "), skipped" << std::endl;
            current.parent().remove_child(current);
            continue;
        }

        std::string incDir = targetPath;
        auto slash = incDir.rfind('/');
        if (slash != std::string::npos) incDir = incDir.substr(0, slash);

        // Inline the element children of the included file's root
        std::vector<pugi::xml_node> nodesToCopy;
        for (auto incChild : incDoc.document_element().children()) {
            if (incChild.type() == pugi::node_element) {
                nodesToCopy.push_back(incChild);
            }
        }
        for (int i = 0; i < nCount; ++i) {
            pugi::xml_node insertionPoint = current;
            for (const auto& incChild : nodesToCopy) {
                pugi::xml_node inserted = current.parent().insert_copy_after(incChild, insertionPoint);
                expandIncludes(inserted, incDir, depth + 1);
                insertionPoint = inserted;
            }
        }
        current.parent().remove_child(current);
    }
}

// Map XML class names to C++ control class names.
// Source of truth: WindowBuilder::CreateControlByClass (src/Core/WindowBuilder.cpp)
static std::string cppClass(const std::string& xmlClass) {
    // Structural nodes that are never turned into controls
    if (xmlClass == "Window" || xmlClass == "Global") return "";
    if (xmlClass == "Event" || xmlClass == "BubbledEvent") return "";

    static const std::map<std::string, std::string> kClassMap = {
        // Box containers
        {"Box", "ui::Box"}, {"HBox", "ui::HBox"}, {"VBox", "ui::VBox"},
        {"HFlowBox", "ui::HFlowBox"}, {"VFlowBox", "ui::VFlowBox"},
        {"XmlBox", "ui::XmlBox"}, {"VTileBox", "ui::VTileBox"}, {"HTileBox", "ui::HTileBox"},
        {"TabBox", "ui::TabBox"}, {"GridBox", "ui::GridBox"}, {"GridScrollBox", "ui::GridScrollBox"},
        {"ScrollBox", "ui::ScrollBox"}, {"HScrollBox", "ui::HScrollBox"}, {"VScrollBox", "ui::VScrollBox"},
        {"HFlowScrollBox", "ui::HFlowScrollBox"}, {"VFlowScrollBox", "ui::VFlowScrollBox"},
        {"HTileScrollBox", "ui::HTileScrollBox"}, {"VTileScrollBox", "ui::VTileScrollBox"},
        {"SplitBox", "ui::SplitBox"},
        {"BoxDragable", "ui::BoxDragable"}, {"HBoxDragable", "ui::HBoxDragable"}, {"VBoxDragable", "ui::VBoxDragable"},
        {"BoxMovable", "ui::BoxMovable"}, {"HBoxMovable", "ui::HBoxMovable"}, {"VBoxMovable", "ui::VBoxMovable"},
        {"BoxResizable", "ui::BoxResizable"}, {"HBoxResizable", "ui::HBoxResizable"}, {"VBoxResizable", "ui::VBoxResizable"},
        {"GroupBox", "ui::GroupBox"}, {"GroupHBox", "ui::GroupHBox"}, {"GroupVBox", "ui::GroupVBox"},

        // List / virtual list
        {"ListBox", "ui::ListBox"}, {"ListBoxItem", "ui::ListBoxItem"},
        {"HListBox", "ui::HListBox"}, {"VListBox", "ui::VListBox"},
        {"HTileListBox", "ui::HTileListBox"}, {"VTileListBox", "ui::VTileListBox"},
        {"ListCtrl", "ui::ListCtrl"}, {"PropertyGrid", "ui::PropertyGrid"},
        // Virtual*ListBox variants need Layout* in constructor
        {"VirtualVListBox", "virtual_v"},
        {"VirtualHListBox", "virtual_h"},
        {"VirtualVTileListBox", "virtual_vtile"},
        {"VirtualHTileListBox", "virtual_htile"},

        // Basic controls
        {"Control", "ui::Control"},
        {"ControlDragable", "ui::ControlDragable"},
        {"ControlMovable", "ui::ControlMovable"},
        {"ControlResizable", "ui::ControlResizable"},
        {"ScrollBar", "ui::ScrollBar"},
        {"Label", "ui::Label"}, {"LabelBox", "ui::LabelBox"},
        {"LabelHBox", "ui::LabelHBox"}, {"LabelVBox", "ui::LabelVBox"},
        {"Button", "ui::Button"}, {"ButtonBox", "ui::ButtonBox"},
        {"ButtonHBox", "ui::ButtonHBox"}, {"ButtonVBox", "ui::ButtonVBox"},
        {"Option", "ui::Option"}, {"OptionBox", "ui::OptionBox"},
        {"CheckBox", "ui::CheckBox"}, {"CheckBoxBox", "ui::CheckBoxBox"},
        {"CheckBoxHBox", "ui::CheckBoxHBox"}, {"CheckBoxVBox", "ui::CheckBoxVBox"},
        {"TreeView", "ui::TreeView"}, {"DirectoryTree", "ui::DirectoryTree"},
        {"TreeNode", "ui::TreeNode"},
        {"Combo", "ui::Combo"}, {"ComboButton", "ui::ComboButton"},
        {"FilterCombo", "ui::FilterCombo"}, {"CheckCombo", "ui::CheckCombo"},
        {"Slider", "ui::Slider"}, {"Progress", "ui::Progress"}, {"CircleProgress", "ui::CircleProgress"},
        {"RichText", "ui::RichText"}, {"RichTextBox", "ui::RichTextBox"},
        {"RichTextHBox", "ui::RichTextHBox"}, {"RichTextVBox", "ui::RichTextVBox"},
        {"RichEdit", "ui::RichEdit"}, {"DateTime", "ui::DateTime"},
        {"ColorControl", "ui::ColorControl"}, {"ColorSlider", "ui::ColorSlider"},
        {"ColorPickerRegular", "ui::ColorPickerRegular"},
        {"ColorPickerStatard", "ui::ColorPickerStatard"},
        {"ColorPickerStatardGray", "ui::ColorPickerStatardGray"},
        {"ColorPickerCustom", "ui::ColorPickerCustom"},
        {"Line", "ui::Line"}, {"IPAddress", "ui::IPAddress"}, {"HotKey", "ui::HotKey"},
        {"HyperLink", "ui::HyperLink"},
        {"TabCtrl", "ui::TabCtrl"}, {"TabCtrlItem", "ui::TabCtrlItem"},
        {"IconControl", "ui::IconControl"}, {"BitmapControl", "ui::BitmapControl"},
        {"AddressBar", "ui::AddressBar"}, {"MenuBar", "ui::MenuBar"},
        {"ChildWindow", "ui::ChildWindow"},
        {"Split", "ui::Split"},
    };
    auto it = kClassMap.find(xmlClass);
    if (it != kClassMap.end()) return it->second;
    return ""; // Unknown - skip (caller emits a WARNING)
}

// Track which classes are used by generated controls
static std::set<std::string> s_usedClasses;
static void trackClass(const std::string& cls) { if (!cls.empty()) s_usedClasses.insert(cls); }

// Per-file control variable counter (reset for each XML file so that
// AttachBox(p0) always refers to the first control of that file)
static int g_varId = 0;

static void genAttrs(std::ostream& out, const std::string& var,
                     const pugi::xml_node& node) {
    for (const auto& a : node.attributes()) {
        std::string name = a.name();
        std::string value = a.value();
        if (name.empty() || value.empty()) continue;
        if (name == "on_click" || name == "on_select" || name == "on_change") continue;
        if (name == "class") trackClass(value);  // Track class usage for image embedding
        out << "        " << var << "->SetAttribute(" << "DUI_T(\"" << escapeCStr(name) << "\")"
            << ", " << "DUI_T(\"" << escapeCStr(value) << "\")" << ");\n";
    }
}

// Format XML attributes as a ui::Create/ui::Attach initializer list.
static std::string genAttrList(const pugi::xml_node& node) {
    std::string s;
    for (const auto& a : node.attributes()) {
        std::string name = a.name();
        std::string value = a.value();
        if (name.empty() || value.empty()) continue;
        if (name == "on_click" || name == "on_select" || name == "on_change") continue;
        if (name == "class") trackClass(value);  // Track class usage for image embedding
        if (!s.empty()) s += ", ";
        s += std::string("{") + "DUI_T(\"" + escapeCStr(name) + "\")" + ", " + "DUI_T(\"" + escapeCStr(value) + "\")" + "}";
    }
    return s;
}

static std::vector<int> parseIntList(const std::string& value) {
    std::vector<int> values;
    std::stringstream stream(value);
    std::string part;
    while (std::getline(stream, part, ',')) {
        try {
            size_t parsed = 0;
            int valueInt = std::stoi(part, &parsed);
            if (parsed != part.size()) {
                return {};
            }
            values.push_back(valueInt);
        }
        catch (...) {
            return {};
        }
    }
    return values;
}

// Map shadow_type XML attribute value to the C++ enum suffix (e.g. "system_default" -> "SystemDefault")
static std::string shadowTypeCppName(const std::string& xmlValue) {
    static const std::map<std::string, std::string> types = {
        {"default", "Default"}, {"system_default", "SystemDefault"},
        {"small", "Small"}, {"small_round", "SmallRound"},
        {"big", "Big"}, {"big_round", "BigRound"},
        {"system_round", "SystemRound"},
        {"system_small_round", "SystemSmallRound"},
        {"system_not_round", "SystemDoNotRound"},
    };
    auto it = types.find(xmlValue);
    return (it != types.end()) ? it->second : "Default";
}

// Generate readable setter calls for the root Window attributes.
static void genWindowAttrs(std::ostream& out, const pugi::xml_node& root,
                           const std::string& resourceFolder) {
    out << "    auto& w = *pWindow;\n";
    bool hasShadowAttached = false;
    bool shadowAttached = false;
    bool hasPosition = false;
    std::vector<int> position;
    bool hasSize = false;
    bool hasShadowType = false;
    std::string shadowTypeValue;

    for (const auto& a : root.attributes()) {
        const std::string name = a.name();
        const std::string value = a.value();
        if (name == "shadow_attached" || name == "shadowattached") {
            hasShadowAttached = true;
            shadowAttached = value == "true";
        }
        else if (name == "position" || name == "pos") {
            hasPosition = true;
            position = parseIntList(value);
        }
        else if (name == "size") {
            hasSize = true;
            std::vector<int> size = parseIntList(value);
            if (size.size() == 2) {
                out << "    w.SetWindowSize(" << size[0] << ", " << size[1] << ");\n";
            }
            else {
                out << "    { ui::UiSize size; bool scaledCX = false; bool scaledCY = false;\n"
                    << "      bool percentCX = false; bool percentCY = false;\n"
                    << "      ui::AttributeUtil::ParseWindowSize(pWindow, " << "DUI_T(\"" << escapeCStr(value) << "\")"
                    << ", size, &scaledCX, &scaledCY, &percentCX, &percentCY);\n"
                     << "      w.SetWindowSize(size.cx, size.cy); }\n";
            }
        }
        else if (name == "min_size" || name == "mininfo" || name == "max_size" || name == "maxinfo") {
            std::vector<int> size = parseIntList(value);
            if (size.size() == 2) {
                const bool isMin = name == "min_size" || name == "mininfo";
                out << "    w.SetWindow" << (isMin ? "Minimum" : "Maximum")
                    << "Size(ui::UiSize(" << size[0] << ", " << size[1] << "), true);\n";
            }
        }
        else if (name == "use_system_caption") {
            out << "    w.SetUseSystemCaption(" << (value == "true" ? "true" : "false") << ");\n";
        }
        else if (name == "show_caption_title") {
            out << "    w.SetShowCaptionTitle(" << (value == "true" ? "true" : "false") << ");\n";
        }
        else if (name == "caption_title_style") {
            out << "    w.SetCaptionTitleStyle(DUI_T(\"" << escapeCStr(value) << "\"));\n";
        }
        else if (name == "size_box" || name == "sizebox" || name == "caption" || name == "sys_menu_rect") {
            std::vector<int> rect = parseIntList(value);
            if (rect.size() == 4) {
                std::string setter;
                if (name == "size_box" || name == "sizebox") setter = "SetSizeBox";
                else if (name == "caption") setter = "SetCaptionRect";
                else setter = "SetSysMenuRect";
                out << "    w." << setter << "(ui::UiRect(" << rect[0] << ", " << rect[1]
                    << ", " << rect[2] << ", " << rect[3] << "), true);\n";
            }
        }
        else if (name == "snap_layout_menu" || name == "sys_menu" || name == "shadow_snap" || name == "drag_drop") {
            std::string setter;
            if (name == "snap_layout_menu") setter = "SetEnableSnapLayoutMenu";
            else if (name == "sys_menu") setter = "SetEnableSysMenu";
            else if (name == "shadow_snap") setter = "SetEnableShadowSnap";
            else setter = "SetEnableDragDrop";
            out << "    w." << setter << "(" << (value == "true" ? "true" : "false") << ");\n";
        }
        else if (name == "icon" || name == "text" || name == "text_id" || name == "textid") {
            const char* setter = name == "icon" ? "SetWindowIcon" :
                                 (name == "text" ? "SetText" : "SetTextId");
            out << "    w." << setter << "(" << "DUI_T(\"" << escapeCStr(value) << "\")" << ");\n";
        }
        else if (name == "round_corner" || name == "roundcorner") {
            std::vector<int> size = parseIntList(value);
            if (size.size() == 2) {
                out << "    w.SetRoundCorner(" << size[0] << ", " << size[1] << ", true);\n";
            }
        }
        else if (name == "shadow_type") {
            hasShadowType = true;
            shadowTypeValue = value;
            static const std::map<std::string, std::string> types = {
                {"default", "kShadowDefault"}, {"system_default", "kShadowSystemDefault"},
                {"small", "kShadowSmall"}, {"small_round", "kShadowSmallRound"},
                {"big", "kShadowBig"}, {"big_round", "kShadowBigRound"},
                {"system_round", "kShadowSystemRound"},
                {"system_small_round", "kShadowSystemSmallRound"},
                {"system_not_round", "kShadowSystemDoNotRound"},
            };
            auto it = types.find(value);
            if (it != types.end()) {
                out << "    w.SetShadowType(ui::Shadow::ShadowType::" << it->second << ");\n";
            }
        }
        else if (name == "shadow_image" || name == "shadow_border_color") {
            out << "    w." << (name == "shadow_image" ? "SetShadowImage" : "SetShadowBorderColor")
                << "(" << "DUI_T(\"" << escapeCStr(value) << "\")" << ");\n";
        }
        else if (name == "shadow_corner") {
            std::vector<int> padding = parseIntList(value);
            if (padding.size() == 4) {
                out << "    w.SetShadowCorner(ui::UiPadding(" << padding[0] << ", " << padding[1]
                    << ", " << padding[2] << ", " << padding[3] << "));\n";
            }
        }
        else if (name == "shadow_border_round") {
            std::vector<int> size = parseIntList(value);
            if (size.size() == 2) {
                out << "    w.SetShadowBorderRound(ui::UiSize(" << size[0] << ", " << size[1] << "));\n";
            }
        }
        else if (name == "shadow_border_size" || name == "alpha" || name == "opacity") {
            const char* setter = name == "shadow_border_size" ? "SetShadowBorderSize" :
                                 (name == "alpha" ? "SetLayeredWindowAlpha" : "SetLayeredWindowOpacity");
            out << "    w." << setter << "(" << value << ");\n";
        }
        else if (name == "layered_window" || name == "layeredwindow") {
            out << "    w.SetLayeredWindow(" << (value == "true" ? "true" : "false") << ", false);\n";
        }
        else if (name == "render_backend_type") {
            std::string backend = "kRaster_BackendType";
            if (value == "GL" || value == "GPU") backend = "kNativeGL_BackendType";
            else if (value == "Metal") backend = "kMetal_BackendType";
            out << "    w.SetRenderBackendType(ui::RenderBackendType::" << backend << ");\n";
        }
    }

    if (hasShadowAttached) {
        out << "    w.SetShadowAttached(" << (shadowAttached ? "true" : "false") << ");\n";
    }

    // System shadow type post-processing: mirrors WindowBuilder::ParseWindowAttributes
    // behavior. System shadow types require a non-layered window (OS draws the shadow),
    // so we must override layered_window to false after all individual setters have run.
    if (hasShadowType) {
        static const std::set<std::string> kSystemTypes = {
            "system_default", "system_round",
            "system_small_round", "system_not_round"
        };
        if (kSystemTypes.count(shadowTypeValue)) {
            std::string cppName = shadowTypeCppName(shadowTypeValue);
            out << "    // System shadow type: normalize and force non-layered window\n";
            out << "    { ui::Shadow::ShadowType supportedType =\n";
            out << "          ui::Shadow::GetSupportedShadowType(pWindow, ui::Shadow::ShadowType::kShadow"
                << cppName << ");\n";
            out << "      if (supportedType != ui::Shadow::ShadowType::kShadow" << cppName << ") {\n";
            out << "          w.SetShadowType(supportedType);\n";
            out << "      }\n";
            out << "      if (ui::Shadow::IsSystemShadowType(supportedType)) {\n";
            out << "          w.SetLayeredWindow(false, false);\n";
            out << "      }\n";
            out << "    }\n";
        }
    }

    if (hasPosition && position.size() == 2) {
        out << "    { ui::UiRect windowRect = pWindow->GetWindowPos(false);\n"
            << "      w.MoveWindow(" << position[0] << ", " << position[1]
            << ", windowRect.Width(), windowRect.Height(), true); }\n";
    }
    else if (hasSize) {
        out << "    w.CenterWindow();\n";
    }
}

// Generate a node as a sequential (non-nested) statement: create the control
// with ui::Create<T>(pWindow, {attrs}), recurse into children, then attach it
// to its parent with ui::Attach(parent, var). This mirrors the handwritten
// code style (see examples/hello_code), and avoids deeply nested expressions.
static void genNode(std::ostream& out, const pugi::xml_node& node,
                    const std::string& parentVar, const std::string& parentTag, int depth) {
    std::string tag = nodeName(node);
    if (tag.empty()) return;

    // ---- Window-level shared resource nodes (classes, fonts, colors) ----
    if (tag == "Class") {
        std::string clsName = attr(node, "name");
        std::string attrs;
        for (const auto& a : node.attributes()) {
            std::string name = a.name();
            if (name == "name") continue;
            attrs += " " + name + "=\"" + a.value() + "\"";
        }
        out << "    pWindow->AddClass(" << "DUI_T(\"" << escapeCStr(clsName) << "\")" << ", "
            << "DUI_T(\"" << escapeCStr(attrs) << "\")" << ");\n";
        return;
    }
    if (tag == "TextColor") {
        std::string colorName = attr(node, "name");
        std::string colorValue = attr(node, "value");
        out << "    pWindow->AddTextColor(" << "DUI_T(\"" << escapeCStr(colorName) << "\")" << ", "
            << "DUI_T(\"" << escapeCStr(colorValue) << "\")" << ");\n";
        return;
    }
    if (tag == "Font") {
        // <Font id="..." name="..." size="12" bold="true" italic="true" default="true"/>
        out << "    { ui::UiFont f; f.m_fontName = " << "DUI_T(\"" << escapeCStr(attr(node, "name")) << "\")" << ";\n";
        int fontSize = node.attribute("size").as_int(12);
        out << "      f.m_fontSize = " << fontSize << ";";
        if (attr(node, "bold") == "true")     out << " f.m_bBold = true;";
        if (attr(node, "underline") == "true") out << " f.m_bUnderline = true;";
        if (attr(node, "italic") == "true")   out << " f.m_bItalic = true;";
        bool isDefault = (attr(node, "default") == "true");
        out << "\n      ui::GlobalManager::Instance().Font().AddFont("
            << "DUI_T(\"" << escapeCStr(attr(node, "id")) << "\")" << ", f, " << (isDefault ? "true" : "false") << "); }\n";
        return;
    }
    if (tag == "DefaultFontFamilyNames") {
        out << "    ui::GlobalManager::Instance().Font().SetDefaultFontFamilyNames("
            << "DUI_T(\"" << escapeCStr(attr(node, "value")) << "\")" << ");\n";
        return;
    }

    std::string cls = cppClass(tag);
    if (cls.empty()) {
        // Structural / unknown nodes: recurse into children.
        // Unknown class names get a WARNING so nothing is silently dropped.
        static const std::set<std::string> kStructural = {"Window", "Global", "Event", "BubbledEvent"};
        if (kStructural.count(tag) == 0) {
            std::cerr << "WARNING: unknown XML class \"" << tag << "\" skipped (no C++ mapping)" << std::endl;
        }
        for (auto child : node.children()) {
            if (child.type() == pugi::node_element) {
                genNode(out, child, parentVar, parentTag, depth);
            }
        }
        return;
    }

    std::string var = "p" + std::to_string(g_varId++);

    // Handle Virtual*ListBox variants (need Layout* in constructor)
    if (cls == "virtual_vtile") {
        out << "    ui::VirtualListBox* " << var
            << " = new ui::VirtualListBox(pWindow, new ui::VirtualVTileLayout);\n";
        genAttrs(out, var, node);
    } else if (cls == "virtual_htile") {
        out << "    ui::VirtualListBox* " << var
            << " = new ui::VirtualListBox(pWindow, new ui::VirtualHTileLayout);\n";
        genAttrs(out, var, node);
    } else if (cls == "virtual_v") {
        out << "    ui::VirtualListBox* " << var
            << " = new ui::VirtualListBox(pWindow, new ui::VirtualVLayout);\n";
        genAttrs(out, var, node);
    } else if (cls == "virtual_h") {
        out << "    ui::VirtualListBox* " << var
            << " = new ui::VirtualListBox(pWindow, new ui::VirtualHLayout);\n";
        genAttrs(out, var, node);
    } else {
        std::string attrs = genAttrList(node);
        out << "    auto* " << var << " = ui::Create<" << cls
            << ">(pWindow, {" << attrs << "});\n";
    }

    if (tag == "RichText") {
        // <RichText> content: inline XML parsed at runtime via the public
        // static helper (b/i/font/s/u markup and text slices)
        std::string inner = serializeChildren(node);
        if (!inner.empty()) {
            out << "    ui::WindowBuilder::ParseRichTextXmlText(DUI_T(\"<RichText>"
                << escapeCStr(inner) << "</RichText>\"), " << var << ");\n";
        }
    }
    else {
        // Process children
        for (auto child : node.children()) {
            if (child.type() == pugi::node_element) {
                genNode(out, child, var, tag, depth + 1);
            }
        }
    }

    // Attach to parent. The root control (parentVar empty) is attached by the
    // caller via ui::Attach(pWindow, p0); see the generator's main loop.
    // TreeNode / Combo nodes are added through their dedicated APIs instead.
    if (!parentVar.empty()) {
        if (tag == "TreeNode") {
            if (parentTag == "TreeView") {
                out << "    " << parentVar << "->GetRootNode()->AddChildNode(" << var << ");\n";
            }
            else if (parentTag == "TreeNode") {
                out << "    " << parentVar << "->AddChildNode(" << var << ");\n";
            }
            else if (parentTag == "Combo") {
                out << "    " << parentVar << "->GetTreeView()->GetRootNode()->AddChildNode(" << var << ");\n";
            }
            else {
                out << "    " << parentVar << "->AddItem(" << var << ");\n";
            }
        }
        else {
            out << "    ui::Attach(" << parentVar << ", " << var << ");\n";
        }
    }
    out << "\n";
}

static std::string stem(const std::string& path) {
    auto pos = path.rfind('/');
    std::string name = (pos != std::string::npos) ? path.substr(pos + 1) : path;
    auto dot = name.rfind('.');
    if (dot != std::string::npos) name = name.substr(0, dot);
    // Capitalize first letter: main -> Main, item -> Item
    if (!name.empty()) name[0] = toupper(name[0]);
    return name;
}

// ---- Base64 encode / image embedding support ----
static const char kBase64Tbl[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static std::string readBinaryFile(const std::string& path, bool& ok) {
    std::ifstream f(path, std::ios::binary);
    ok = f.good();
    if (!ok) return "";
    return std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
}

static std::string base64Encode(const std::string& data) {
    std::string out;
    out.reserve((data.size() + 2) / 3 * 4);
    for (size_t i = 0; i < data.size(); i += 3) {
        unsigned n = (unsigned char)data[i] << 16;
        if (i + 1 < data.size()) n |= (unsigned char)data[i + 1] << 8;
        if (i + 2 < data.size()) n |= (unsigned char)data[i + 2];
        out += kBase64Tbl[(n >> 18) & 63];
        out += kBase64Tbl[(n >> 12) & 63];
        out += (i + 1 < data.size()) ? kBase64Tbl[(n >> 6) & 63] : '=';
        out += (i + 2 < data.size()) ? kBase64Tbl[n & 63] : '=';
    }
    return out;
}

static std::string imageVarName(const std::string& path) {
    std::string n;
    auto p = path.rfind('/'); if (p != std::string::npos) n = path.substr(p+1);
    else n = path;
    for (auto& c : n) if (c == '.' || c == '-') c = '_';
    return n;
}

// Collect class→image mapping from global.xml
static void parseGlobalClasses(const std::string& globalPath,
    std::map<std::string, std::string>& classImages,
    std::map<std::string, std::string>& classAttrs) {
    pugi::xml_document doc;
    if (!doc.load_file(globalPath.c_str())) return;
    for (auto node : doc.document_element().children()) {
        if (std::string(node.name()) != "Class") continue;
        std::string name = attr(node, "name");
        if (name.empty()) continue;
        std::string img = attr(node, "normal_image");
        if (!img.empty()) {
            // Extract file='xxx' from image string
            auto fp = img.find("file='");
            if (fp != std::string::npos) {
                auto fe = img.find('\'', fp + 6);
                if (fe != std::string::npos) {
                    classImages[name] = img.substr(fp + 6, fe - fp - 6);
                }
            }
        }
        // Collect all attributes for AddClass
        std::string attrs;
        for (auto& a : node.attributes()) {
            std::string an = a.name();
            if (an == "name") continue;
            attrs += " " + an + "=\"" + a.value() + "\"";
        }
        if (!attrs.empty()) classAttrs[name] = attrs;
    }
}

int main(int argc, char** argv) {
    std::string outputPath, baseName, globalPath;
    std::vector<std::string> xmlFiles;
    std::vector<std::string> resourceFolders;
    bool autoEmbed = false;

    for (int i = 1; i < argc; i++) {
        std::string a = argv[i];
        if (a == "-g" && i + 1 < argc) { globalPath = argv[++i]; autoEmbed = true; }
        else if (a == "-r" && i + 1 < argc) { resourceFolders.push_back(argv[++i]); }
        else if (outputPath.empty()) outputPath = a;
        else if (baseName.empty()) baseName = a;
        else xmlFiles.push_back(a);
    }

    if (outputPath.empty() || baseName.empty() || xmlFiles.empty()) {
        std::cerr << "Usage: xml_to_code [-g global.xml] <output.inc> <base_func_name> <xml1> [xml2...]"
                  << std::endl;
        std::cerr << "  -g global.xml : auto-embed images referenced by classes" << std::endl;
        std::cerr << "  -r folder     : resource folder for the corresponding XML file (repeatable)" << std::endl;
        return 1;
    }

    // Parse global.xml for class→image mappings
    std::map<std::string, std::string> classImages, classAttrs;
    if (!globalPath.empty()) parseGlobalClasses(globalPath, classImages, classAttrs);

    std::ofstream out(outputPath);
    if (!out) { std::cerr << "Cannot open output: " << outputPath << std::endl; return 1; }

    out << "///////////////////////////////////////////////////////////////////////////\n";
    out << "//   AUTO-GENERATED by xml_to_code - DO NOT EDIT BY HAND\n";
    out << "//\n";
    out << "//   Source XML files:\n";
    for (const auto& x : xmlFiles) out << "//     " << x << "\n";
    out << "//\n";
    out << "//   Each XML file produces one function:\n";
    out << "//     " << baseName << " + FileStem\n";
    for (const auto& x : xmlFiles) {
        out << "//     " << baseName << stem(x) << "(ui::Window* pWindow)\n";
    }
    out << "//\n";
     out << "//   Functions ending in 'Window' call ui::Attach(pWindow, p0) automatically.\n";
    out << "//   Other functions (templates, items) do NOT attach the root - the\n";
    out << "//   caller must add the root control to a parent container.\n";
    out << "///////////////////////////////////////////////////////////////////////////\n\n";
    out << "#include \"dui/dui.h\"\n";
    out << "#include \"dui/Utils/UiBuilder.h\"\n\n";

    bool hasWindowFunc = false;
    for (size_t xmlIndex = 0; xmlIndex < xmlFiles.size(); ++xmlIndex) {
        const auto& xmlFile = xmlFiles[xmlIndex];
        g_varId = 0;    // variable numbering starts at 0 per XML file, ensuring AttachBox(p0) is correct
        pugi::xml_document doc;
        if (!doc.load_file(xmlFile.c_str())) {
            std::cerr << "Failed to parse: " << xmlFile << std::endl;
            return 1;
        }

        // Expand <Include src="..." count="n"/> nodes in-place
        std::string dir = xmlFile;
        auto slash = dir.rfind('/');
        if (slash != std::string::npos) dir = dir.substr(0, slash);
        expandIncludes(doc.document_element(), dir, 0);

        pugi::xml_node root = doc.document_element();
        if (!root) continue;

        std::string funcName = baseName + stem(xmlFile);
        std::string rootTag = nodeName(root);

        out << "void " << funcName << "(ui::Window* pWindow) {\n";

        if (rootTag == "Window" || rootTag == "Global") {
            if (rootTag == "Window") {
                std::string resourceFolder;
                const std::string marker = "/themes/";
                std::string normalizedXmlFile = xmlFile;
                for (char& ch : normalizedXmlFile) {
                    if (ch == '\\') ch = '/';
                }
                size_t themeStart = normalizedXmlFile.find(marker);
                if (themeStart != std::string::npos) {
                    size_t folderStart = normalizedXmlFile.find('/', themeStart + marker.size());
                    if (folderStart != std::string::npos) {
                        folderStart++;
                        size_t folderEnd = normalizedXmlFile.find('/', folderStart);
                        if (folderEnd != std::string::npos) {
                            resourceFolder = normalizedXmlFile.substr(folderStart, folderEnd - folderStart);
                        }
                    }
                }
                if (xmlIndex < resourceFolders.size()) {
                    resourceFolder = resourceFolders[xmlIndex];
                }
                genWindowAttrs(out, root, resourceFolder);
            }
            for (auto child : root.children()) {
                if (child.type() == pugi::node_element) {
                    genNode(out, child, "", "", 1);
                }
            }
        } else {
            genNode(out, root, "", "", 1);
        }

        // AttachBox for Window-rooted files; skip for templates (ListBoxItem etc.)
        bool isTemplate = false;
        for (auto child : root.children()) {
            if (child.type() == pugi::node_element) {
                std::string tag = nodeName(child);
                if (tag == "ListBoxItem" || tag == "BoxItem" || tag == "TreeNode")
                    isTemplate = true;
                break;
            }
        }
        if (rootTag == "Window" && !isTemplate) {
            out << "    ui::Attach(pWindow, p0);\n";
        }
        out << "}\n\n";
    }

    // ---- Auto-embed images referenced by used classes ----
    if (autoEmbed && !s_usedClasses.empty()) {
        out << "// ============================================================\n";
        out << "// Auto-embedded images (only those referenced by generated UI)\n";
        out << "// ============================================================\n\n";
        out << "#include <sys/mman.h>\n#include <unistd.h>\n";
        out << "#include <cstring>\n#include <cstdio>\n#include <cstdlib>\n\n";

        // Base64 decoder
        out << "static const signed char kDecTbl[256] = {\n";
        for (int i = 0; i < 256; i++) {
            if (i % 16 == 0) out << "    ";
            int v = -1;
            if (i >= 'A' && i <= 'Z') v = i - 'A';
            else if (i >= 'a' && i <= 'z') v = i - 'a' + 26;
            else if (i >= '0' && i <= '9') v = i - '0' + 52;
            else if (i == '+') v = 62;
            else if (i == '/') v = 63;
            out << v << (i < 255 ? "," : "");
            if (i % 16 == 15) out << "\n";
        }
        out << "};\n\n";
        out << "inline DString ImgToMemFd(const char* b64, const char* tag) {\n";
        out << "#if defined(__linux__)\n";
        out << "    // Linux: anonymous memory file, readable via /proc/self/fd\n";
        out << "    int fd=memfd_create(tag,MFD_CLOEXEC); if(fd<0)return \"\";\n";
        out << "#else\n";
        out << "    // macOS / FreeBSD: memfd_create and /proc/self/fd are Linux-only;\n";
        out << "    // fall back to an anonymous temporary file\n";
        out << "    char tmpl[]=\"/tmp/dui_embedded_XXXXXX\";\n";
        out << "    int fd=mkstemp(tmpl); if(fd<0)return \"\";\n";
        out << "#endif\n";
        out << "    unsigned char buf[8192];\n";
        out << "    const unsigned char* s=(const unsigned char*)b64;\n";
        out << "    size_t di=0;int val=0,vb=-8;\n";
        out << "    while(*s&&di<sizeof(buf)){\n";
        out << "        int c=kDecTbl[*s++];if(c==-1)continue;\n";
        out << "        val=(val<<6)|c;vb+=6;\n";
        out << "        if(vb>=0){buf[di++]=(unsigned char)((val>>vb)&0xFF);vb-=8;}\n";
        out << "    }\n";
        out << "    if(write(fd,buf,di)!=(ssize_t)di){close(fd);return \"\";}\n";
        out << "#if defined(__linux__)\n";
        out << "    char tmp[32];snprintf(tmp,sizeof(tmp),\"%d\",fd);\n";
        out << "    DString p=\"/proc/self/fd/\";\n";
        out << "    for(char* x=tmp;*x;x++)p+=(DString::value_type)(unsigned char)*x;\n";
        out << "#else\n";
        out << "    DString p;\n";
        out << "    for(char* x=tmpl;*x;x++)p+=(DString::value_type)(unsigned char)*x;\n";
        out << "#endif\n";
        out << "    return p;\n}\n\n";

        // Embed each used image
        std::set<std::string> embeddedImages;
        for (const auto& cls : s_usedClasses) {
            auto it = classImages.find(cls);
            if (it == classImages.end()) continue;
            std::string imgPath = it->second;
            if (embeddedImages.count(imgPath)) continue;
            embeddedImages.insert(imgPath);

            // Try to find the image file relative to the global.xml directory
            std::string globalDir = globalPath.substr(0, globalPath.rfind('/'));
            std::string fullPath = globalDir + "/" + imgPath;
            bool ok = false;
            std::string data = readBinaryFile(fullPath, ok);
            if (!ok) {
                // Try alternative paths
                fullPath = globalDir + "/../" + imgPath;
                data = readBinaryFile(fullPath, ok);
            }
            if (!ok) continue;

            std::string var = "kImg_" + imageVarName(imgPath);
            out << "// " << imgPath << " (" << data.size() << " bytes)\n";
            out << "static const char " << var << "[] =\n";
            std::string b64 = base64Encode(data);
            for (size_t j = 0; j < b64.size(); j += 72) {
                out << "    \"" << b64.substr(j, 72) << "\""
                    << (j + 72 >= b64.size() ? ";\n" : "\n");
            }
            out << "inline DString GetPath_" << imageVarName(imgPath) << "() {\n";
            out << "    static DString p;\n";
            out << "    if(p.empty()) p=ImgToMemFd(" << var << ",\"" << imageVarName(imgPath) << "\");\n";
            out << "    return p;\n}\n\n";
        }

        // Generate class registrations with embedded paths
        if (!embeddedImages.empty()) {
            out << "// Class registrations using embedded images\n";
            out << "inline void RegisterEmbeddedClasses(ui::Window* pWindow) {\n";
            out << "    DString attrs;\n";
            for (const auto& cls : s_usedClasses) {
                auto it = classImages.find(cls);
                auto ai = classAttrs.find(cls);
                if (it == classImages.end() || ai == classAttrs.end()) continue;
                std::string imgVar = imageVarName(it->second);
                // Build the full attribute string using a local DString
                // Replace file='...' with file='[embedded]'
                std::string origAttr = ai->second;
                auto fp = origAttr.find("file='");
                if (fp != std::string::npos) {
                    auto fe = origAttr.find('\'', fp + 6);
                    if (fe != std::string::npos) {
                        std::string before = origAttr.substr(0, fp + 6);
                        std::string after = origAttr.substr(fe);
                        out << "    attrs = \"" << escapeCStr(before) << "\";\n";
                        out << "    attrs += GetPath_" << imgVar << "();\n";
                        out << "    attrs += \"" << escapeCStr(after) << "\";\n";
                        out << "    pWindow->AddClass(\"" << escapeCStr(cls) << "\", attrs);\n";
                        continue;
                    }
                }
                out << "    pWindow->AddClass(\"" << escapeCStr(cls) << "\", \""
                    << escapeCStr(origAttr) << "\");\n";
            }
            out << "}\n\n";
        }
    }

    std::cout << "Generated " << outputPath;
    if (autoEmbed) std::cout << " with " << s_usedClasses.size() << " class(es)";
    std::cout << " from " << xmlFiles.size() << " XML file(s)" << std::endl;
    return 0;
}
