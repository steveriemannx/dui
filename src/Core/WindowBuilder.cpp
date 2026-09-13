#include "dui/Core/WindowBuilder.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/Window.h"
#include "dui/Core/Box.h"
#include "dui/Core/Control.h"
#include "dui/Core/ControlDragable.h"
#include "dui/Core/ControlMovable.h"
#include "dui/Core/ControlResizable.h"
#include "dui/Core/ScrollBar.h"
#include "dui/Core/WindowCreateAttributes.h"

#include "dui/Control/TreeView.h"
#include "dui/Control/DirectoryTree.h"
#include "dui/Control/Combo.h"
#include "dui/Control/ComboButton.h"
#include "dui/Control/FilterCombo.h"
#include "dui/Control/CheckCombo.h"
#include "dui/Control/Slider.h"
#include "dui/Control/Progress.h"
#include "dui/Control/CircleProgress.h"
#include "dui/Control/RichEdit.h"
#include "dui/Control/RichText.h"
#include "dui/Control/DateTime.h"
#include "dui/Control/Split.h"
#include "dui/Control/GroupBox.h"

#include "dui/Control/ColorControl.h"
#include "dui/Control/ColorSlider.h"
#include "dui/Control/ColorPickerRegular.h"
#include "dui/Control/ColorPickerStatard.h"
#include "dui/Control/ColorPickerStatardGray.h"
#include "dui/Control/ColorPickerCustom.h"
#include "dui/Control/Line.h"
#include "dui/Control/IPAddress.h"
#include "dui/Control/HotKey.h"
#include "dui/Control/HyperLink.h"
#include "dui/Control/ListCtrl.h"
#include "dui/Control/PropertyGrid.h"
#include "dui/Control/TabCtrl.h"
#include "dui/Control/IconControl.h"
#include "dui/Control/BitmapControl.h"
#include "dui/Control/AddressBar.h"
#include "dui/Control/MenuBar.h"
#include "dui/Control/ChildWindow.h"

#include "dui/Box/HBox.h"
#include "dui/Box/VBox.h"
#include "dui/Box/XmlBox.h"
#include "dui/Box/TabBox.h"
#include "dui/Box/GridBox.h"
#include "dui/Box/TileBox.h"
#include "dui/Box/ScrollBox.h"
#include "dui/Box/ListBox.h"
#include "dui/Box/VirtualListBox.h"

#include "dui/Utils/StringUtil.h"
#include "dui/Utils/StringConvert.h"
#include "dui/Utils/AttributeUtil.h"
#include "dui/Utils/FilePathUtil.h"

#include "third_party/xml/pugixml.hpp"
#include <set>

namespace ui 
{

WindowBuilder::WindowBuilder()
{
    m_xml = std::make_unique<pugi::xml_document>();
}

WindowBuilder::~WindowBuilder()
{
}


Control* WindowBuilder::CreateControlByClass(const std::string& strControlClass, Window* pWindow)
{
    typedef std::function<Control* (Window* pWindow)> CreateControlFunction;
    static std::map<std::string, CreateControlFunction> createControlMap =
    {
        {DUI_CTR_BOX,  [](Window* pWindow) { return new Box(pWindow); }},
        {DUI_CTR_HBOX, [](Window* pWindow) { return new HBox(pWindow); }},
        {DUI_CTR_VBOX, [](Window* pWindow) { return new VBox(pWindow); }},
        {DUI_CTR_HFLOWBOX, [](Window* pWindow) { return new HFlowBox(pWindow); }},
        {DUI_CTR_VFLOWBOX, [](Window* pWindow) { return new VFlowBox(pWindow); }},
        {DUI_CTR_XMLBOX,  [](Window* pWindow) { return new XmlBox(pWindow); }},
        {DUI_CTR_VTILE_BOX, [](Window* pWindow) { return new VTileBox(pWindow); }},
        {DUI_CTR_HTILE_BOX, [](Window* pWindow) { return new HTileBox(pWindow); }},
        {DUI_CTR_TABBOX, [](Window* pWindow) { return new TabBox(pWindow); }},
        {DUI_CTR_GRIDBOX, [](Window* pWindow) { return new GridBox(pWindow); }},
        {DUI_CTR_GRID_SCROLLBOX, [](Window* pWindow) { return new GridScrollBox(pWindow); }},

        {DUI_CTR_SCROLLBOX, [](Window* pWindow) { return new ScrollBox(pWindow); }},
        {DUI_CTR_HSCROLLBOX, [](Window* pWindow) { return new HScrollBox(pWindow); }},
        {DUI_CTR_VSCROLLBOX, [](Window* pWindow) { return new VScrollBox(pWindow); }},
        {DUI_CTR_HFLOW_SCROLLBOX, [](Window* pWindow) { return new HFlowScrollBox(pWindow); }},
        {DUI_CTR_VFLOW_SCROLLBOX, [](Window* pWindow) { return new VFlowScrollBox(pWindow); }},
        {DUI_CTR_HTILE_SCROLLBOX, [](Window* pWindow) { return new HTileScrollBox(pWindow); }},
        {DUI_CTR_VTILE_SCROLLBOX, [](Window* pWindow) { return new VTileScrollBox(pWindow); }},

        {DUI_CTR_LISTBOX_ITEM, [](Window* pWindow) { return new ListBoxItem(pWindow); }},
        {DUI_CTR_HLISTBOX, [](Window* pWindow) { return new HListBox(pWindow); }},
        {DUI_CTR_VLISTBOX, [](Window* pWindow) { return new VListBox(pWindow); }},
        {DUI_CTR_HTILE_LISTBOX, [](Window* pWindow) { return new HTileListBox(pWindow); }},
        {DUI_CTR_VTILE_LISTBOX, [](Window* pWindow) { return new VTileListBox(pWindow); }},
        {DUI_CTR_LISTCTRL, [](Window* pWindow) { return new ListCtrl(pWindow); }},
        {DUI_CTR_PROPERTY_GRID, [](Window* pWindow) { return new PropertyGrid(pWindow); }},

        {DUI_CTR_VIRTUAL_HTILE_LISTBOX, [](Window * pWindow) { return new VirtualHTileListBox(pWindow); }},
        {DUI_CTR_VIRTUAL_VTILE_LISTBOX, [](Window* pWindow) { return new VirtualVTileListBox(pWindow); }},
        {DUI_CTR_VIRTUAL_VLISTBOX, [](Window* pWindow) { return new VirtualVListBox(pWindow); }},
        {DUI_CTR_VIRTUAL_HLISTBOX, [](Window* pWindow) { return new VirtualHListBox(pWindow); }},

        {DUI_CTR_CONTROL, [](Window* pWindow) { return new Control(pWindow); }},
        {DUI_CTR_CONTROL_DRAGABLE, [](Window* pWindow) { return new ControlDragable(pWindow); }},
        {DUI_CTR_CONTROL_MOVABLE, [](Window* pWindow) { return new ControlMovable(pWindow); }},
        {DUI_CTR_CONTROL_RESIZABLE, [](Window* pWindow) { return new ControlResizable(pWindow); }},
        {DUI_CTR_SCROLLBAR, [](Window* pWindow) { return new ScrollBar(pWindow); }},
        {DUI_CTR_LABEL, [](Window* pWindow) { return new Label(pWindow); }},
        {DUI_CTR_LABELBOX, [](Window* pWindow) { return new LabelBox(pWindow); }},
        {DUI_CTR_LABELHBOX, [](Window* pWindow) { return new LabelHBox(pWindow); } },
        {DUI_CTR_LABELVBOX, [](Window* pWindow) { return new LabelVBox(pWindow); }},
        {DUI_CTR_BUTTON, [](Window* pWindow) { return new Button(pWindow); }},
        {DUI_CTR_BUTTONBOX, [](Window* pWindow) { return new ButtonBox(pWindow); }},
        {DUI_CTR_BUTTONHBOX, [](Window* pWindow) { return new ButtonHBox(pWindow); }},
        {DUI_CTR_BUTTONVBOX, [](Window* pWindow) { return new ButtonVBox(pWindow); }},
        {DUI_CTR_OPTION, [](Window* pWindow) { return new Option(pWindow); }},
        {DUI_CTR_OPTIONBOX, [](Window* pWindow) { return new OptionBox(pWindow); }},
        {DUI_CTR_CHECKBOX, [](Window* pWindow) { return new CheckBox(pWindow); }},
        {DUI_CTR_CHECKBOXBOX, [](Window* pWindow) { return new CheckBoxBox(pWindow); }},
        {DUI_CTR_CHECKBOXHBOX, [](Window* pWindow) { return new CheckBoxHBox(pWindow); }},
        {DUI_CTR_CHECKBOXVBOX, [](Window* pWindow) { return new CheckBoxVBox(pWindow); }},
        {DUI_CTR_TREEVIEW, [](Window* pWindow) { return new TreeView(pWindow); }},
        {DUI_CTR_DIRECTORY_TREE, [](Window* pWindow) { return new DirectoryTree(pWindow); }},
        {DUI_CTR_TREENODE, [](Window* pWindow) { return new TreeNode(pWindow); }},
        {DUI_CTR_COMBO, [](Window* pWindow) { return new Combo(pWindow); }},
        {DUI_CTR_COMBO_BUTTON, [](Window* pWindow) { return new ComboButton(pWindow); }},
        {DUI_CTR_FILTER_COMBO, [](Window* pWindow) { return new FilterCombo(pWindow); }},
        {DUI_CTR_CHECK_COMBO, [](Window* pWindow) { return new CheckCombo(pWindow); }},
        {DUI_CTR_SLIDER, [](Window* pWindow) { return new Slider(pWindow); }},
        {DUI_CTR_PROGRESS, [](Window* pWindow) { return new Progress(pWindow); }},
        {DUI_CTR_CIRCLEPROGRESS, [](Window* pWindow) { return new CircleProgress(pWindow); }},
        {DUI_CTR_RICHTEXT, [](Window* pWindow) { return new RichText(pWindow); }},
        {DUI_CTR_RICHTEXT_BOX, [](Window* pWindow) { return new RichTextBox(pWindow); }},
        {DUI_CTR_RICHTEXT_HBOX, [](Window* pWindow) { return new RichTextHBox(pWindow); }},
        {DUI_CTR_RICHTEXT_VBOX, [](Window* pWindow) { return new RichTextVBox(pWindow); }},
        {DUI_CTR_RICHEDIT, [](Window* pWindow) { return new RichEdit(pWindow); }},
        {DUI_CTR_DATETIME, [](Window* pWindow) { return new DateTime(pWindow); }},
        {DUI_CTR_COLOR_CONTROL, [](Window* pWindow) { return new ColorControl(pWindow); }},
        {DUI_CTR_COLOR_SLIDER, [](Window* pWindow) { return new ColorSlider(pWindow); }},
        {DUI_CTR_COLOR_PICKER_REGULAR, [](Window* pWindow) { return new ColorPickerRegular(pWindow); }},
        {DUI_CTR_COLOR_PICKER_STANDARD, [](Window* pWindow) { return new ColorPickerStatard(pWindow); }},
        {DUI_CTR_COLOR_PICKER_STANDARD_GRAY, [](Window* pWindow) { return new ColorPickerStatardGray(pWindow); }},
        {DUI_CTR_COLOR_PICKER_CUSTOM, [](Window* pWindow) { return new ColorPickerCustom(pWindow); }},
        {DUI_CTR_LINE, [](Window* pWindow) { return new Line(pWindow); }},
        {DUI_CTR_IPADDRESS, [](Window* pWindow) { return new IPAddress(pWindow); }},
        {DUI_CTR_HOTKEY, [](Window* pWindow) { return new HotKey(pWindow); }},
        {DUI_CTR_HYPER_LINK, [](Window* pWindow) { return new HyperLink(pWindow); }},
        {DUI_CTR_TAB_CTRL, [](Window* pWindow) { return new TabCtrl(pWindow); }},
        {DUI_CTR_TAB_CTRL_ITEM, [](Window* pWindow) { return new TabCtrlItem(pWindow); }},
        {DUI_CTR_ICON_CONTROL, [](Window* pWindow) { return new IconControl(pWindow); }},
        {DUI_CTR_BITMAP_CONTROL, [](Window* pWindow) { return new BitmapControl(pWindow); }},
        {DUI_CTR_ADDRESS_BAR, [](Window* pWindow) { return new AddressBar(pWindow); }},
        {DUI_CTR_MENU_BAR, [](Window* pWindow) { return new MenuBar(pWindow); }},
        {DUI_CTR_CHILD_WINDOW, [](Window* pWindow) { return new ChildWindow(pWindow); }},

        {DUI_CTR_SPLIT, [](Window* pWindow) { return new Split(pWindow); }},
        {DUI_CTR_SPLITBOX, [](Window* pWindow) { return new SplitBox(pWindow); }},
        {DUI_CTR_GROUP_BOX, [](Window* pWindow) { return new GroupBox(pWindow); }},
        {DUI_CTR_GROUP_HBOX, [](Window* pWindow) { return new GroupHBox(pWindow); }},
        {DUI_CTR_GROUP_VBOX, [](Window* pWindow) { return new GroupVBox(pWindow); }},

        {DUI_CTR_BOX_DRAGABLE, [](Window* pWindow) { return new BoxDragable(pWindow); }},
        {DUI_CTR_HBOX_DRAGABLE, [](Window* pWindow) { return new HBoxDragable(pWindow); }},
        {DUI_CTR_VBOX_DRAGABLE, [](Window* pWindow) { return new VBoxDragable(pWindow); }},

        {DUI_CTR_BOX_MOVABLE, [](Window* pWindow) { return new BoxMovable(pWindow); }},
        {DUI_CTR_HBOX_MOVABLE, [](Window* pWindow) { return new HBoxMovable(pWindow); }},
        {DUI_CTR_VBOX_MOVABLE, [](Window* pWindow) { return new VBoxMovable(pWindow); }},

        {DUI_CTR_BOX_RESIZABLE, [](Window* pWindow) { return new BoxResizable(pWindow); }},
        {DUI_CTR_HBOX_RESIZABLE, [](Window* pWindow) { return new HBoxResizable(pWindow); }},
        {DUI_CTR_VBOX_RESIZABLE, [](Window* pWindow) { return new VBoxResizable(pWindow); }},
    };
    Control* pControl = nullptr;
    auto iter = createControlMap.find(strControlClass);
    if (iter != createControlMap.end()) {
        pControl = iter->second(pWindow);
    }
    return pControl;
}

bool WindowBuilder::IsXmlFileExists(const FilePath& xmlFilePath) const
{
    if (xmlFilePath.IsEmpty()) {
        return false;
    }
    bool bExists = false;
    const FilePath themeDefaultPath = GlobalManager::Instance().GetThemeDefaultPath();
    if (GlobalManager::Instance().MemoryResources().IsOpen()) {
        FilePath sFile = FilePathUtil::JoinFilePath(GlobalManager::Instance().GetResourcePath(), xmlFilePath);
        bool bResExist = GlobalManager::Instance().MemoryResources().IsDataExist(sFile);
        if (!bResExist && !themeDefaultPath.IsEmpty()) {
            //Overlay theme: fall back to the default embedded resources
            bResExist = GlobalManager::Instance().MemoryResources().IsDataExist(
                FilePathUtil::JoinFilePath(themeDefaultPath, xmlFilePath));
        }
        bExists = bResExist;
    }
    else {
        if (xmlFilePath.IsAbsolutePath()) {
            bExists = xmlFilePath.IsExistsFile();
        }
        else {
            FilePath xmlFullPath = FilePathUtil::JoinFilePath(GlobalManager::Instance().GetResourcePath(), xmlFilePath);
            bExists = xmlFullPath.IsExistsFile();
            if (!bExists && !themeDefaultPath.IsEmpty()) {
                //Overlay theme: fall back to the default theme root
                bExists = FilePathUtil::JoinFilePath(themeDefaultPath, xmlFilePath).IsExistsFile();
            }
        }
    }
    return bExists;
}

bool WindowBuilder::ParseXmlData(const std::string& xmlFileData, const FilePath& xmlFilePath)
{
    ASSERT(!xmlFileData.empty() && "xml parameter is empty!");
    if (xmlFileData.empty()) {
        return false;
    }
    bool isLoaded = false;
    //If the string starts with '<', it is treated as an XML string; otherwise it is treated as an XML file
    //If embedded resources are used, read directly from memory
    if (xmlFileData.front() == '<') {
        pugi::xml_encoding encoding = pugi::xml_encoding::encoding_utf8;
        pugi::xml_parse_result result = m_xml->load_buffer(xmlFileData.c_str(),
                                                           xmlFileData.size() * sizeof(std::string::value_type),
                                                           pugi::parse_default, encoding);
        isLoaded = result.status == pugi::status_ok;
    }
    if (!isLoaded) {
        ASSERT(!"WindowBuilder::ParseXmlData load xmlFileData failed!");
        return false;
    }
    m_xmlFilePath = xmlFilePath;
    return true;
}

bool WindowBuilder::ParseXmlData(const std::vector<unsigned char>& xmlFileData, const FilePath& xmlFilePath)
{
    ASSERT(!xmlFileData.empty() && "xml parameter is empty!");
    if (xmlFileData.empty()) {
        return false;
    }
    pugi::xml_encoding encoding = pugi::xml_encoding::encoding_auto;
    pugi::xml_parse_result result = m_xml->load_buffer(xmlFileData.data(),
                                                       xmlFileData.size(),
                                                       pugi::parse_default, encoding);
    bool isLoaded = result.status == pugi::status_ok;
    if (!isLoaded) {
        ASSERT(!"WindowBuilder::ParseXmlData load xmlFileData failed!");
        return false;
    }
    m_xmlFilePath = xmlFilePath;
    return true;
}

bool WindowBuilder::ParseXmlFile(const FilePath& xmlFilePath, const FilePath& windowResPath)
{
    ASSERT(!xmlFilePath.IsEmpty() && "xmlFilePath parameter is empty!");
    if (xmlFilePath.IsEmpty()) {
        return false;
    }
    bool isLoaded = false;
    //Overlay theme roots to search in order: the active theme root, then the default theme root
    std::vector<FilePath> resRoots;
    resRoots.push_back(GlobalManager::Instance().GetResourcePath());
    const FilePath themeDefaultPath = GlobalManager::Instance().GetThemeDefaultPath();
    if (!themeDefaultPath.IsEmpty() && themeDefaultPath != resRoots[0]) {
        resRoots.push_back(themeDefaultPath);
    }
    if (GlobalManager::Instance().MemoryResources().IsOpen()) {
        std::vector<unsigned char> file_data;
        for (const FilePath& root : resRoots) {
            FilePath sFile = FilePathUtil::JoinFilePath(root, xmlFilePath);
        if (!windowResPath.IsEmpty() && !GlobalManager::Instance().MemoryResources().IsDataExist(sFile)) {
                //Searches in the window directory
                sFile = FilePathUtil::JoinFilePath(root, windowResPath);
                sFile = FilePathUtil::JoinFilePath(sFile, xmlFilePath);
            }
            if (GlobalManager::Instance().MemoryResources().GetData(sFile, file_data)) {
                pugi::xml_parse_result result = m_xml->load_buffer(file_data.data(), file_data.size());
                if (result.status != pugi::status_ok) {
                    ASSERT(!"WindowBuilder::ParseXmlFile load xml from memory data failed!");
                    return false;
                }
                isLoaded = true;
                break;
            }
        }
    }
    else {
        FilePath xmlFileFullPath;
        if (xmlFilePath.IsRelativePath()) {
            for (const FilePath& root : resRoots) {
                xmlFileFullPath = FilePathUtil::JoinFilePath(root, xmlFilePath);
                if (!windowResPath.IsEmpty() && !xmlFileFullPath.IsExistsFile()) {
                    //Look it up in the window directory
                    xmlFileFullPath = FilePathUtil::JoinFilePath(root, windowResPath);
                    xmlFileFullPath = FilePathUtil::JoinFilePath(xmlFileFullPath, xmlFilePath);
                }
                if (xmlFileFullPath.IsExistsFile()) {
                    break;
                }
            }
        }
        else {
            xmlFileFullPath = xmlFilePath;
        }
        pugi::xml_parse_result result = m_xml->load_file(xmlFileFullPath.NativePathA().c_str());
        if (result.status != pugi::status_ok) {
            ASSERT(!"WindowBuilder::ParseXmlFile load xml file failed!");
            return false;
        }
        isLoaded = true;
    }
    if (!isLoaded) {
        ASSERT(!"WindowBuilder::ParseXmlFile load xmlFilePath failed!");
        return false;
    }
    m_xmlFilePath = xmlFilePath;
    return true;
}

Control* WindowBuilder::CreateControls(Window* pWindow, CreateControlCallback pCallback, Box* pParent, Box* pUserDefinedBox)
{
    //Validate the window: it must exist, otherwise features such as DPI adaptation will fail, resulting in incorrect layout
    ASSERT(pWindow != nullptr);
    if (pWindow == nullptr) {
        return nullptr;
    }
    if ((pParent != nullptr) && (pParent->GetWindow() == nullptr)) {
        pParent->SetWindow(pWindow);
    }
    if ((pUserDefinedBox != nullptr) && (pUserDefinedBox->GetWindow() == nullptr)) {
        pUserDefinedBox->SetWindow(pWindow);
    }

    m_createControlCallback = pCallback;
    pugi::xml_node root = m_xml->root().first_child();
    ASSERT(!root.empty());
    if (root.empty()) {
        return nullptr;
    }

    if( pWindow != nullptr) {
        std::string strClass;
        std::string strName;
        std::string strValue;
        strClass = root.name();
        if( strClass == "Window" ) {
            if (!pWindow->IsWindowAttributesApplied()) {
                //The window attributes are set only once, to avoid the XML files included in the XML (Include tag) setting the window attributes again and causing confusion
                ParseWindowAttributes(pWindow, root);
                pWindow->SetWindowAttributesApplied(true);
            }            
            ParseWindowShareAttributes(pWindow, root);
        }
        else if( strClass == "Global" ) {
            ParseGlobalAttributes(root);
        }
    }

    for (pugi::xml_node node : root.children()) {
        std::string strClass = node.name();
        if ( (strClass == "Image")          ||
             (strClass == "FontResource")   ||
             (strClass == "Font")           ||
             (strClass == "Class")          ||
             (strClass == "TextColor") ) {
            //Ignore these attributes

        }
        else {
            if (pUserDefinedBox == nullptr) {
                return ParseXmlNodeChildren(root, pParent, pWindow);
            }
            else {
                ParseXmlNodeChildren(node, pUserDefinedBox, pWindow);
                int i = 0;
                for (pugi::xml_attribute attr : node.attributes()) {
                    if (StringUtil::StringCompare(attr.name(), "class") == 0) {
                        //The class attribute must be the first attribute
                        ASSERT_UNUSED_VARIABLE(i == 0);
                    }
                    ++i;
                    pUserDefinedBox->SetAttribute(attr.name(), attr.value());
                }
                return pUserDefinedBox;
            }
        }
    }
    return nullptr;
}

bool WindowBuilder::ParseWindowCreateAttributes(WindowCreateAttributes& createAttributes)
{
    pugi::xml_node root = m_xml->root().first_child();
    ASSERT(!root.empty());
    if (root.empty()) {
        return false;
    }
    std::string strClass = root.name();
    ASSERT(strClass == "Window");
    if (strClass != "Window") {
        return false;
    }

    UiSize szMinSize;
    UiSize szMaxSize;
    bool bScaledCX = false;
    bool bScaledCY = false;
    bool bPercentCX = false;
    bool bPercentCY = false;

    //Whether the size configured for the window includes the shadow
    bool bSizeContainShadow = false;

    //Shadow related parameters
    bool bShadowAttached = false;
    bool bHasShadowAttached = false;
    Shadow::ShadowType nShadowType = Shadow::ShadowType::kShadowDrawDefault;
    UiPadding rcShadowCorner;

    RenderBackendType backendType = RenderBackendType::kRaster_BackendType;
    std::string strName;
    std::string strValue;
    for (pugi::xml_attribute attr : root.attributes()) {
        strName = attr.name();
        strValue = attr.value();
        if (strName == "render_backend_type") {            
            if (StringUtil::IsEqualNoCase(strValue, "GL") || StringUtil::IsEqualNoCase(strValue, "GPU")) {
                backendType = RenderBackendType::kNativeGL_BackendType;
            }
            else if (StringUtil::IsEqualNoCase(strValue, "CPU")) {
                backendType = RenderBackendType::kRaster_BackendType;
            }
            else if (StringUtil::IsEqualNoCase(strValue, "Metal")) {
                backendType = RenderBackendType::kMetal_BackendType;
            }
        }
        else if (strName == "use_system_caption") {
            createAttributes.m_bUseSystemCaption = (strValue == "true");
            createAttributes.m_bUseSystemCaptionDefined = true;
        }
        else if ((strName == "size_box") || (strName == "sizebox")) {
            AttributeUtil::ParseRectValue(strValue.c_str(), createAttributes.m_rcSizeBox);
            createAttributes.m_bSizeBoxDefined = true;
        }
        else if (strName == "caption") {
            AttributeUtil::ParseRectValue(strValue.c_str(), createAttributes.m_rcCaption);
            createAttributes.m_bCaptionDefined = true;
        }
        else if ((strName == "shadow_attached") || (strName == "shadowattached")) {
            createAttributes.m_bShadowAttached = (strValue == "true");
            createAttributes.m_bShadowAttachedDefined = true;
        }
        else if ((strName == "layered_window") || (strName == "layeredwindow")) {
            createAttributes.m_bIsLayeredWindow = (strValue == "true");
            createAttributes.m_bIsLayeredWindowDefined = true;
        }
        else if (strName == "alpha") {
            //Set the transparency of the window (0 - 255); only valid when a layered window is used, passed as a parameter in the UpdateLayeredWindow function
            int32_t nAlpha = StringUtil::StringToInt32(strValue);
            ASSERT(nAlpha >= 0 && nAlpha <= 255);
            if ((nAlpha >= 0) && (nAlpha <= 255)) {
                createAttributes.m_nLayeredWindowAlpha = (uint8_t)nAlpha;
                createAttributes.m_bLayeredWindowAlphaDefined = true;
            }
        }
        else if (strName == "opacity") {
            //Set the opacity of the window (0 - 255); this value is used as a parameter (bAlpha) in the SetLayeredWindowAttributes function
            const int32_t nAlpha = StringUtil::StringToInt32(strValue);
            ASSERT(nAlpha >= 0 && nAlpha <= 255);
            if ((nAlpha >= 0) && (nAlpha <= 255)) {
                createAttributes.m_nLayeredWindowOpacity = (uint8_t)nAlpha;
                createAttributes.m_bLayeredWindowOpacityDefined = true;
            }
        }
        else if (strName == "size") {
            AttributeUtil::ParseWindowSize(nullptr, strValue.c_str(), createAttributes.m_szInitSize, &bScaledCX, &bScaledCY, &bPercentCX, &bPercentCY);
            createAttributes.m_bInitSizeDefined = true;
        }
        else if (strName == "size_contain_shadow") {
            //Whether the size configured for the window includes the shadow
            bSizeContainShadow = (strValue == "true");
        }
        else if ((strName == "min_size") || (strName == "mininfo")) {
            AttributeUtil::ParseSizeValue(strValue.c_str(), szMinSize);
        }
        else if ((strName == "max_size") || (strName == "maxinfo")) {
            AttributeUtil::ParseSizeValue(strValue.c_str(), szMaxSize);
        }
        else if (strName == "native_render_name") {
            //The expected native backend Render name
            createAttributes.m_nativeRenderName = strValue;
        }
        else if ((strName == "shadow_attached") || (strName == "shadowattached")) {
            //Set whether window shadow is supported (there are two shadow implementations: layered windows and normal windows)
            bShadowAttached = (strValue == "true");
            bHasShadowAttached = true;
        }
        else if (strName == "shadow_type") {
            //Set the shadow type
            Shadow::GetShadowType(strValue, nShadowType);
        }
        else if ((strName == "shadow_corner") || (strName == "shadowcorner")) {
            //Set the nine-grid properties of the window shadow            
            AttributeUtil::ParsePaddingValue(strValue.c_str(), rcShadowCorner);
        }
    }

    //Evaluate the nine-grid properties of the shadow
    if (bHasShadowAttached && !bShadowAttached) {
        rcShadowCorner.Clear();
    }
    else if (rcShadowCorner.IsEmpty()){
        UiSize szBorderRound;
        std::string shadowImage;
        Shadow::GetShadowParam(nShadowType, szBorderRound, rcShadowCorner, shadowImage);
    }
    createAttributes.m_rcShadowCorner = rcShadowCorner;

    if (createAttributes.m_bInitSizeDefined) {
        int32_t cx = createAttributes.m_szInitSize.cx;
        int32_t cy = createAttributes.m_szInitSize.cy;
        UiSize minSize = szMinSize;
        UiSize maxSize = szMaxSize;
        //Window sizes are physical pixels on every platform (see ParseWindowSize)
        if ((minSize.cx > 0) && (cx < minSize.cx)) {
            cx = minSize.cx;
        }
        if ((maxSize.cx > 0) && (cx > maxSize.cx)) {
            cx = maxSize.cx;
        }
        if ((minSize.cy > 0) && (cy < minSize.cy)) {
            cy = minSize.cy;
        }
        if ((maxSize.cy > 0) && (cy > maxSize.cy)) {
            cy = maxSize.cy;
        }
        if (!bSizeContainShadow) {
            if (!bPercentCX) {
                GlobalManager::Instance().Dpi().ScaleWindowSize(rcShadowCorner.left);
                GlobalManager::Instance().Dpi().ScaleWindowSize(rcShadowCorner.right);
                cx += rcShadowCorner.left + rcShadowCorner.right;
            }
            if (!bPercentCY) {
                GlobalManager::Instance().Dpi().ScaleWindowSize(rcShadowCorner.top);
                GlobalManager::Instance().Dpi().ScaleWindowSize(rcShadowCorner.bottom);
                cy += rcShadowCorner.top + rcShadowCorner.bottom;
            }
        }
        AttributeUtil::ValidateWindowSize(nullptr, cx, cy);
        createAttributes.m_szInitSize.cx = cx;
        createAttributes.m_szInitSize.cy = cy;
    }
#if defined (DUI_BUILD_FOR_WIN)
    if (backendType == RenderBackendType::kNativeGL_BackendType) {
        //When using OpenGL, layered windows cannot be used
        if (!createAttributes.m_bLayeredWindowOpacityDefined || (createAttributes.m_nLayeredWindowOpacity == 255)) {
            if (createAttributes.m_bIsLayeredWindowDefined) {
                createAttributes.m_bIsLayeredWindow = false;
            }
        }
    }
#else
    UNUSED_VARIABLE(backendType);
#endif
    return true;
}

void WindowBuilder::ParseWindowAttributes(Window* pWindow, const pugi::xml_node& root) const
{
    ASSERT((pWindow != nullptr) && pWindow->IsWindow());
    if ((pWindow == nullptr) || !pWindow->IsWindow()) {
        return;
    }

    std::set<std::string> knownNames;//Supported attribute names
    std::string strName;
    std::string strValue;

    bool bInitRenderBackendType = false;
    //First set the "render_backend_type" attribute
    for (pugi::xml_attribute attr : root.attributes()) {
        strName = attr.name();
        strValue = attr.value();
        if (strName == "render_backend_type") {
            knownNames.insert(strName);
            RenderBackendType backendType = RenderBackendType::kRaster_BackendType;
            if (StringUtil::IsEqualNoCase(strValue, "GL") || StringUtil::IsEqualNoCase(strValue, "GPU")) {
                backendType = RenderBackendType::kNativeGL_BackendType;
            }
            else if (StringUtil::IsEqualNoCase(strValue, "CPU")) {
                backendType = RenderBackendType::kRaster_BackendType;
            }
            else if (StringUtil::IsEqualNoCase(strValue, "Metal")) {
                backendType = RenderBackendType::kMetal_BackendType;
            }
            else {
                ASSERT(0);
            }
            pWindow->SetRenderBackendType(backendType);
            bInitRenderBackendType = true;
            break;
        }
    }
    if (!bInitRenderBackendType) {
        //First initialize the Render backend drawing method; this call creates the Render
#if defined(DUI_BUILD_FOR_MACOS)
        //macOS native: GPU (GL) is the default for smooth rendering
        pWindow->SetRenderBackendType(RenderBackendType::kNativeGL_BackendType);
#else
        pWindow->SetRenderBackendType(RenderBackendType::kRaster_BackendType);
#endif
    }
     
    //First process min_size/max_size/use_system_caption, because other attributes depend on these attributes
    for (pugi::xml_attribute attr : root.attributes()) {
        strName = attr.name();
        strValue = attr.value();
        if ((strName == "min_size") || (strName == "mininfo")) {
            knownNames.insert(strName);
            UiSize size;
            AttributeUtil::ParseSizeValue(strValue.c_str(), size);
            //Window sizes/min sizes are physical pixels: no DPI scaling
            pWindow->SetWindowMinimumSize(size, false);
        }
        else if ((strName == "max_size") || (strName == "maxinfo")) {
            knownNames.insert(strName);
            UiSize size;
            AttributeUtil::ParseSizeValue(strValue.c_str(), size);
            //Window sizes/max sizes are physical pixels: no DPI scaling
            pWindow->SetWindowMaximumSize(size, false);
        }
        else if (strName == "use_system_caption") {
            knownNames.insert(strName);
            pWindow->SetUseSystemCaption(strValue == "true");
        }
        else if (strName == "show_caption_title") {
            knownNames.insert(strName);
            pWindow->SetShowCaptionTitle(strValue == "true");
        }
        else if (strName == "caption_title_style") {
            knownNames.insert(strName);
            pWindow->SetCaptionTitleStyle(strValue);
        }
    }
    //Whether the size configured for the window includes the shadow
    bool bSizeContainShadow = false;

    //Whether the window shadow is enabled
    bool bShadowAttached = false;
    bool bHasShadowAttached = false;
    Shadow::ShadowType nShadowType = Shadow::ShadowType::kShadowCount;

    //Note: if use_system_caption is true, the layered window is disabled (because these two attributes are mutually exclusive)
    for (pugi::xml_attribute attr : root.attributes()) {
        strName = attr.name();
        strValue = attr.value();
        if ((strName == "size_box") || (strName == "sizebox")) {
            knownNames.insert(strName);
            UiRect rcSizeBox;
            AttributeUtil::ParseRectValue(strValue.c_str(), rcSizeBox, false);
            pWindow->SetSizeBox(rcSizeBox, true);
        }
        else if (strName == "caption") {
            knownNames.insert(strName);
            UiRect rcCaption;
            AttributeUtil::ParseRectValue(strValue.c_str(), rcCaption);
            pWindow->SetCaptionRect(rcCaption, true);
        }
        else if (strName == "snap_layout_menu") {
            knownNames.insert(strName);
            pWindow->SetEnableSnapLayoutMenu(strValue == "true");
        }
        else if (strName == "sys_menu") {
            knownNames.insert(strName);
            pWindow->SetEnableSysMenu(strValue == "true");
        }
        else if (strName == "sys_menu_rect") {
            knownNames.insert(strName);
            UiRect rcSysMenuRect;
            AttributeUtil::ParseRectValue(strValue.c_str(), rcSysMenuRect);
            pWindow->SetSysMenuRect(rcSysMenuRect, true);
        }
        else if (strName == "icon") {
            knownNames.insert(strName);
            if (!strValue.empty()) {
                //Set the window icon
                pWindow->SetWindowIcon(strValue);
            }
        }
        else if (strName == "text") {
            knownNames.insert(strName);
            pWindow->SetText(strValue);
        }
        else if ((strName == "text_id") || (strName == "textid")) {
            knownNames.insert(strName);
            pWindow->SetTextId(strValue);
        }
        else if (strName == "round_corner" || strName == "roundcorner") {
            knownNames.insert(strName);
            UiSize size;
            AttributeUtil::ParseSizeValue(strValue.c_str(), size);
            pWindow->SetRoundCorner(size.cx, size.cy, true);
        }
        else if (strName == "alpha_fix_corner" || strName == "alphafixcorner") {
            knownNames.insert(strName);
            UiRect rc;
            AttributeUtil::ParseRectValue(strValue.c_str(), rc);
            pWindow->SetAlphaFixCorner(rc, true);
        }
        else if (strName == "size_contain_shadow") {
            knownNames.insert(strName);
            //Whether the size configured for the window includes the shadow
            bSizeContainShadow = (strValue == "true");
        }
        else if ((strName == "shadow_attached") || (strName == "shadowattached")) {
            knownNames.insert(strName);
            //Set whether window shadow is supported (there are two shadow implementations: layered windows and normal windows)
            bShadowAttached = (strValue == "true");
            bHasShadowAttached = true;            
        }
        else if (strName == "shadow_type") {
            knownNames.insert(strName);
            //Set the shadow type
            Shadow::GetShadowType(strValue, nShadowType);
            if ((nShadowType >= Shadow::ShadowType::kShadowFirst) &&
                (nShadowType < Shadow::ShadowType::kShadowCount)) {
                pWindow->SetShadowType((Shadow::ShadowType)nShadowType);
            }
        }
        else if ((strName == "shadow_image") || (strName == "shadowimage")) {
            knownNames.insert(strName);
            //Set the shadow image
            pWindow->SetShadowImage(strValue);
        }
        else if ((strName == "shadow_corner") || (strName == "shadowcorner")) {
            knownNames.insert(strName);
            //Set the nine-grid properties of the window shadow
            UiPadding padding;
            AttributeUtil::ParsePaddingValue(strValue.c_str(), padding);
            pWindow->SetShadowCorner(padding);
        }
        else if (strName == "shadow_border_round") {
            knownNames.insert(strName);
            //Set the corner radius of the window shadow
            UiSize szBorderRound;
            AttributeUtil::ParseSizeValue(strValue.c_str(), szBorderRound);
            pWindow->SetShadowBorderRound(szBorderRound);
        }
        else if (strName == "shadow_border_size") {
            knownNames.insert(strName);
            //Set the border size of the window shadow
            pWindow->SetShadowBorderSize(StringUtil::StringToInt32(strValue));
        }
        else if (strName == "shadow_border_color") {
            knownNames.insert(strName);
            //Set the border color of the window shadow
            pWindow->SetShadowBorderColor(strValue);
        }
        else if (strName == "shadow_snap") {
            knownNames.insert(strName);
            //Set whether the shadow supports window snap operations
            pWindow->SetEnableShadowSnap(strValue == "true");
        }
        else if ((strName == "layered_window") || (strName == "layeredwindow")) {
            knownNames.insert(strName);
            //Set whether the layered window attribute is set (layered window or normal window)
            if (!pWindow->IsUseSystemCaption()) {
                pWindow->SetLayeredWindow(strValue == "true", false);
            }
        }
        else if (strName == "alpha") {
            knownNames.insert(strName);
            //Set the transparency of the window (0 - 255); only valid when a layered window is used, passed as a parameter in the UpdateLayeredWindow function
            int32_t nAlpha = StringUtil::StringToInt32(strValue);
            ASSERT(nAlpha >= 0 && nAlpha <= 255);
            if ((nAlpha >= 0) && (nAlpha <= 255)) {
                pWindow->SetLayeredWindowAlpha(nAlpha);
            }
        }
        else if (strName == "drag_drop") {
            knownNames.insert(strName);
            pWindow->SetEnableDragDrop(strValue == "true");
        }
        else if ((strName == "position") || (strName == "pos")) {
            knownNames.insert(strName);
            UiPoint position;
            AttributeUtil::ParsePointValue(strValue.c_str(), position);
            UiRect windowRect = pWindow->GetWindowPos(false);
            pWindow->MoveWindow(position.x, position.y,
                                 windowRect.Width(), windowRect.Height(), true);
        }
    }

    if (bHasShadowAttached) {
        //Set it afterwards, to avoid being affected by "shadow_type"
        pWindow->SetShadowAttached(bShadowAttached);
    }

    //System shadow types: normalize the type for this platform and force the
    //window to be non-layered (OS shadows need a normal window).
    if ((nShadowType >= Shadow::ShadowType::kShadowFirst) &&
        (nShadowType < Shadow::ShadowType::kShadowCount)) {
        Shadow::ShadowType supportedType =
            Shadow::GetSupportedShadowType(pWindow, nShadowType);
        if (supportedType != nShadowType) {
            pWindow->SetShadowType(supportedType);
        }
        if (Shadow::IsSystemShadowType(supportedType)) {
            pWindow->SetLayeredWindow(false, false);
        }
    }

    bool bScaledCX = false;
    bool bScaledCY = false;
    bool bPercentCX = false;
    bool bPercentCY = false;

    //Finally set the initial size of the window, because the initial size is related to whether the shadow is attached
    bool bLayeredWindowOpacityDefined = false;
    for (pugi::xml_attribute attr : root.attributes()) {
        strName = attr.name();
        strValue = attr.value();
        if (strName == "size") {
            knownNames.insert(strName);
            UiSize windowSize;
            AttributeUtil::ParseWindowSize(pWindow, strValue.c_str(), windowSize, &bScaledCX, &bScaledCY, &bPercentCX, &bPercentCY);
            int32_t cx = windowSize.cx;
            int32_t cy = windowSize.cy;
            UiSize minSize = pWindow->GetWindowMinimumSize();
            UiSize maxSize = pWindow->GetWindowMaximumSize();
            if ((minSize.cx > 0) && (cx < minSize.cx)) {
                cx = minSize.cx;
            }
            if ((maxSize.cx > 0) && (cx > maxSize.cx)) {
                cx = maxSize.cx;
            }
            if ((minSize.cy > 0) && (cy < minSize.cy)) {
                cy = minSize.cy;
            }
            if ((maxSize.cy > 0) && (cy > maxSize.cy)) {
                cy = maxSize.cy;
            }

            if (!bSizeContainShadow) {
                //The window size specified in the XML configuration; if a fixed value is set, it does not include the shadow area
                UiPadding rcShadowCorner = pWindow->GetShadowCorner();                
                if (!bPercentCX && pWindow->IsShadowAttached() && !pWindow->IsWindowMaximized()) {
                    pWindow->Dpi().ScaleWindowSize(rcShadowCorner.left);
                    pWindow->Dpi().ScaleWindowSize(rcShadowCorner.right);
                    cx += rcShadowCorner.left + rcShadowCorner.right;
                }
                if (!bPercentCY && pWindow->IsShadowAttached() && !pWindow->IsWindowMaximized()) {
                    pWindow->Dpi().ScaleWindowSize(rcShadowCorner.top);
                    pWindow->Dpi().ScaleWindowSize(rcShadowCorner.bottom);
                    cy += rcShadowCorner.top + rcShadowCorner.bottom;
                }
            }
            AttributeUtil::ValidateWindowSize(pWindow, cx, cy);
            pWindow->SetInitSize(cx, cy);
        }
        else if (strName == "opacity") {
            knownNames.insert(strName);
            //Set the opacity of the window (0 - 255); this value is used as a parameter (bAlpha) in the SetLayeredWindowAttributes function
            const int32_t nAlpha = StringUtil::StringToInt32(strValue);
            ASSERT(nAlpha >= 0 && nAlpha <= 255);
            if ((nAlpha >= 0) && (nAlpha <= 255)) {
                pWindow->SetLayeredWindowOpacity(nAlpha);
                bLayeredWindowOpacityDefined = true;
            }
        }
    }

#if defined (DUI_BUILD_FOR_WIN)
    if (pWindow->GetRenderBackendType() == RenderBackendType::kNativeGL_BackendType) {
        //When using OpenGL, layered windows cannot be used
        if (!bLayeredWindowOpacityDefined || (pWindow->GetLayeredWindowOpacity() == 255)) {
            pWindow->SetLayeredWindow(false, false);
        }
        if (pWindow->IsShadowAttached() && !pWindow->IsUseSystemCaption()) {
            //If a shadow is used, automatically switch to using the system title bar to avoid display issues
            pWindow->SetUseSystemCaption(true);
        }
    }
#else
    UNUSED_VARIABLE(bLayeredWindowOpacityDefined);
#endif

#ifdef _DEBUG
    //Check for unsupported attributes and warn, to reduce configuration errors
    std::vector<std::string> unknownNames;
    for (pugi::xml_attribute attr : root.attributes()) {
        strName = attr.name();
        if (knownNames.find(strName) == knownNames.end()) {
            unknownNames.push_back(strName);
        }
    }
    ASSERT_UNUSED_VARIABLE(unknownNames.empty() && "Found unknown window attributes in xml!");
#endif
}

void WindowBuilder::ParseWindowShareAttributes(Window* pWindow, const pugi::xml_node& root)
{
    ASSERT((pWindow != nullptr) && pWindow->IsWindow());
    if ((pWindow == nullptr) || !pWindow->IsWindow()) {
        return;
    }

    std::string strName;
    std::string strValue;
    std::string strClass;

    //Parse the shared resources under this window
    for (pugi::xml_node node : root.children()) {
        strClass = node.name();
        if (strClass == "Class") {
            std::string strClassName;
            std::string strAttribute;
            for (pugi::xml_attribute attr : node.attributes()) {
                strName = attr.name();
                strValue = attr.value();
                if (strName == "name") {
                    strClassName = strValue;
                }
                else {
                    strAttribute.append(StringUtil::Printf(" %s=\"%s\"", strName.c_str(), strValue.c_str()));
                }
            }
            if (!strClassName.empty()) {
                //A Class in the window cannot have the same name as a global one, otherwise the Class is invalid
                ASSERT(GlobalManager::Instance().GetClassAttributes(strClassName).empty()); 
                StringUtil::TrimLeft(strAttribute);
                pWindow->AddClass(strClassName, strAttribute);
                m_windowClassList.push_back(strClassName);
            }
        }
        else if (strClass == "TextColor") {
            std::string strColorName;
            std::string strColor;
            for (pugi::xml_attribute attr : node.attributes()) {
                strName = attr.name();
                strValue = attr.value();
                if (strName == "name") {
                    strColorName = strValue;
                }
                else if (strName == "value") {
                    strColor = strValue;
                }
            }
            if (!strColorName.empty()) {
                pWindow->AddTextColor(strColorName, strColor);
                m_windowTextColorList.push_back(strColorName);
            }
        }
        else if (strClass == "Font") {
            //Under the Window node, fonts are allowed to be defined
            ParseFontXmlNode(node);
        }
    }
}

void WindowBuilder::ParseGlobalAttributes(const pugi::xml_node& root)
{
    std::string strClass;
    std::string strName;
    std::string strValue;
    for (pugi::xml_node node : root.children()) {
        strClass = node.name();
        if (strClass == "DefaultFontFamilyNames") {
            std::string defaultFontFamilyNames;
            for (pugi::xml_attribute attr : node.attributes()) {
                strName = attr.name();
                strValue = attr.value();
                if (strName == "value") {
                    defaultFontFamilyNames = strValue;
                    break;
                }
            }
            if (!defaultFontFamilyNames.empty()) {
                GlobalManager::Instance().Font().SetDefaultFontFamilyNames(defaultFontFamilyNames);
            }
        }
        else if (strClass == "FontFile") {
            //Font file
            std::string strFontFile;
            std::string strFontDesc;
            for (pugi::xml_attribute attr : node.attributes()) {
                strName = attr.name();
                strValue = attr.value();
                if (strName == "file") {
                    strFontFile = strValue;
                }
                else if (strName == "desc") {
                    strFontDesc = strValue;
                }
            }
            if (!strFontFile.empty()) {
                GlobalManager::Instance().Font().AddFontFile(strFontFile, strFontDesc);
            }
        }
        else if (strClass == "Font") {
            ParseFontXmlNode(node);
        }
        else if (strClass == "Class") {
            std::string strClassName;
            std::string strAttribute;
            for (pugi::xml_attribute attr : node.attributes()) {
                strName = attr.name();
                strValue = attr.value();
                if (strName == "name") {
                    strClassName = strValue;
                }
                else {
                    strAttribute.append(StringUtil::Printf(" %s=\"%s\"",
                        strName.c_str(), strValue.c_str()));
                }
            }
            if (!strClassName.empty()) {
                StringUtil::TrimLeft(strAttribute);
                GlobalManager::Instance().AddClass(strClassName, strAttribute);
            }
        }
        else if (strClass == "TextColor") {
            std::string colorName = node.attribute("name").as_string();
            std::string colorValue = node.attribute("value").as_string();
            if (!colorName.empty() && !colorValue.empty()) {
                ColorManager& colorManager = GlobalManager::Instance().Color();
                colorManager.AddColor(colorName, colorValue);
                if (colorName == "default_font_color") {
                    colorManager.SetDefaultTextColor(colorName);
                }
                else if (colorName == "disabled_font_color") {
                    colorManager.SetDefaultDisabledTextColor(colorName);
                }
            }
        }
    }
}

void WindowBuilder::ParseFontXmlNode(const pugi::xml_node& xmlNode)
{
    std::string strName;
    std::string strValue;

    std::string strFontId;
    std::string strFontName;
    int size = 12;
    bool bold = false;
    bool underline = false;
    bool strikeout = false;
    bool italic = false;
    bool isDefault = false;
    for (pugi::xml_attribute attr : xmlNode.attributes()) {
        strName = attr.name();
        strValue = attr.value();
        if (strName == "id")
        {
            strFontId = strValue;
        }
        else if (strName == "name") {
            strFontName = strValue;
        }
        else if (strName == "size") {
            size = StringUtil::StringToInt32(strValue);
        }
        else if (strName == "bold") {
            bold = (strValue == "true");
        }
        else if (strName == "underline") {
            underline = (strValue == "true");
        }
        else if (strName == "strikeout") {
            strikeout = (strValue == "true");
        }
        else if (strName == "italic") {
            italic = (strValue == "true");
        }
        else if (strName == "default") {
            isDefault = (strValue == "true");
        }
    }
    if (!strFontName.empty() && !strFontId.empty()) {
        UiFont fontInfo;
        fontInfo.m_fontName = strFontName;
        fontInfo.m_fontSize = size;
        fontInfo.m_bBold = bold;
        fontInfo.m_bItalic = italic;
        fontInfo.m_bUnderline = underline;
        fontInfo.m_bStrikeOut = strikeout;
        if (GlobalManager::Instance().Font().AddFont(strFontId, fontInfo, isDefault)) {
            m_globalFontIdList.push_back(strFontId);
        }
    }
}

Control* WindowBuilder::ParseXmlNodeChildren(const pugi::xml_node& xmlNode, Control* pParent, Window* pWindow)
{
    if (xmlNode.empty()) {
        return nullptr;
    }
    Control* pReturn = nullptr;
    for (pugi::xml_node node : xmlNode.children()) {
        std::string strClass = node.name();
        if( (strClass == "DefaultFontFamilyNames") ||
            (strClass == "Font") ||
            (strClass == "FontFile")  ||
            (strClass == "Class") || 
            (strClass == "TextColor") ) {
                continue;
        }

        Control* pControl = nullptr;
        if (strClass == "Include") {
            if (node.attributes().empty()) {
                continue;
            }
            pugi::xml_attribute countAttr = node.attribute("count");
            int nCount = countAttr.as_int();
            if (nCount <= 0) {
                //The default value is set to 1; the count attribute parameter is optional
                nCount = 1;
            }
            pugi::xml_attribute sourceAttr = node.attribute("src");
            std::string sourceValue = sourceAttr.as_string();
            if (sourceValue.empty()) {
                sourceAttr = node.attribute("source");
                sourceValue = sourceAttr.as_string();                
            }
            FilePath sourceXmlFilePath(sourceValue);
            if (!sourceValue.empty()) {
                StringUtil::ReplaceAll("/", m_xmlFilePath.GetPathSeparatorStr(), sourceValue);
                StringUtil::ReplaceAll("\\", m_xmlFilePath.GetPathSeparatorStr(), sourceValue);
                if (!m_xmlFilePath.IsEmpty()) {
                    //First try to load it from the same directory as the original XML file
                    std::string xmlFilePath = m_xmlFilePath.NativePath();
                    size_t pos = xmlFilePath.find_last_of("\\/");
                    if (pos != std::string::npos) {
                        FilePath srcFilePath(xmlFilePath.substr(0, pos));
                        srcFilePath.JoinFilePath(FilePath(sourceValue));
                        if (IsXmlFileExists(srcFilePath)) {
                            sourceXmlFilePath = srcFilePath;
                        }
                    }
                }
            }
            ASSERT(!sourceXmlFilePath.IsEmpty());
            if (sourceXmlFilePath.IsEmpty()) {
                continue;
            }
            for ( int i = 0; i < nCount; i++ ) {
                WindowBuilder builder;
                FilePath windowResPath = (pWindow != nullptr) ? pWindow->GetResourcePath() : FilePath();
                if (builder.ParseXmlFile(sourceXmlFilePath, windowResPath)) {
                    pControl = builder.CreateControls(pWindow, m_createControlCallback, ToBox(pParent), nullptr);
                }
                else {
                    pControl = nullptr;
                }                
            }
            continue;
        }
        else {
            pControl = CreateControlByClass(strClass, pWindow);
            if (pControl == nullptr) {
                if ((strClass == "Event") || 
                    (strClass == "BubbledEvent")) {
                    bool bBubbled = (strClass == "BubbledEvent");
                    AttachXmlEvent(bBubbled, node, pParent);
                    continue;
                }
            }

            // User-supplied control factory
            if( pControl == nullptr) {
                pControl = GlobalManager::Instance().CreateControl(strClass);
                if (pControl != nullptr) {
                    pControl->SetWindow(pWindow);
                }
            }

            if( pControl == nullptr && m_createControlCallback ) {
                pControl = m_createControlCallback(strClass);
                if (pControl != nullptr) {
                    pControl->SetWindow(pWindow);
                }
            }
        }

        if(pControl == nullptr) {
            std::string nodeName = strClass;
            ASSERT(!"Found unknown node name, can't create control!");
            continue;
        }

        // TreeView related nodes must be added first and parsed later
        if (strClass == DUI_CTR_TREENODE) {
            bool bAdded = false;
            TreeNode* pNode = dynamic_cast<TreeNode*>(pControl);
            ASSERT(pNode != nullptr);
            TreeView* pTreeView = dynamic_cast<TreeView*>(pParent);
            if (pTreeView != nullptr) {
                //A first-level child node
                pTreeView->GetRootNode()->AddChildNode(pNode);
                bAdded = true;
            }
            else {
                //A multi-level child node
                TreeNode* pTreeNode = dynamic_cast<TreeNode*>(pParent);
                if (pTreeNode != nullptr) {
                    pTreeNode->AddChildNode(pNode);
                    bAdded = true;
                }
            }
            if (!bAdded) {
                //Try the Combo control
                Combo* pCombo = dynamic_cast<Combo*>(pParent);
                if (pCombo != nullptr) {
                    pCombo->GetTreeView()->GetRootNode()->AddChildNode(pNode);
                    bAdded = true;
                }
            }
            ASSERT(bAdded);
        }

        pControl->SetWindow(pWindow);
        
        // Process attributes — class must be handled first because other
        // attributes may depend on the control type (e.g. a Button's "icon"
        // attribute is only valid after the class has been set to Button).
        if (!node.attributes().empty()) {
            // Pass 1: process the "class" attribute first regardless of its position
            pugi::xml_attribute classAttr = node.attribute("class");
            if (!classAttr.empty()) {
                pControl->SetAttribute(classAttr.name(), classAttr.value());
            }
            // Pass 2: process all other attributes
            for (pugi::xml_attribute attr : node.attributes()) {
                if (StringUtil::StringCompare(attr.name(), "class") != 0) {
                    pControl->SetAttribute(attr.name(), attr.value());
                }
            }
        }

        if (strClass == DUI_CTR_RICHTEXT) {
            //The node is: <RichText></RichText>; parse its child nodes as the RichText content
            ParseRichTextXmlNode(node, pControl);
        }
        else {
            // Add children
            if (!node.children().empty()) {
                //Recursively process all child nodes of this node and continue adding
                ParseXmlNodeChildren(node, pControl, pWindow);
            }
        }

        // Attach to parent
        // Because some attributes are related to the parent window, such as selected, it must be added to the parent window first
        if (pParent != nullptr && strClass != DUI_CTR_TREENODE) {
            Box* pContainer = dynamic_cast<Box*>(pParent);
            ASSERT(pContainer != nullptr);
            if (pContainer == nullptr) {
                return nullptr;
            }
            if( !pContainer->AddItem(pControl) ) {
                ASSERT(0);
                delete pControl;
                continue;
            }
        }
        
        // Return first item
        if (pReturn == nullptr) {
            pReturn = pControl;
        }
    }
    return pReturn;
}

bool WindowBuilder::ParseRichTextXmlText(const std::string& xmlText, Control* pControl)
{
    pugi::xml_encoding encoding = pugi::xml_encoding::encoding_utf8;
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_buffer(xmlText.c_str(),
                                                    xmlText.size() * sizeof(std::string::value_type),
                                                    pugi::parse_default,
                                                    encoding);
    if (result.status != pugi::status_ok) {
        ASSERT(!"WindowBuilder::ParseRichTextXmlText load xml text failed!");
        return false;
    }
    pugi::xml_node root = doc.root();
    std::string rootName = root.name();
    std::string rootValue = root.value();
    if (rootName.empty() && rootValue.empty()) {
        root = doc.root().first_child();
    }
    rootName = root.name();
    ASSERT(rootName == DUI_CTR_RICHTEXT);
    if (rootName != DUI_CTR_RICHTEXT) {
        return false;
    }
    return ParseRichTextXmlNode(root, pControl, nullptr);
}

bool WindowBuilder::ParseRichTextXmlNode(const pugi::xml_node& xmlNode, Control* pControl, RichTextSlice* pTextSlice)
{
    ASSERT(pControl != nullptr);
    if (pControl == nullptr) {
        return false;
    }
    //Get the implementation interface
    RichTextImpl* pRichTextImpl = nullptr;
    RichText* pRichText = dynamic_cast<RichText*>(pControl);
    if (pRichText != nullptr) {
        pRichTextImpl = pRichText->GetRichTextImpl();
    }
    if (pRichTextImpl == nullptr) {
        RichTextBox* pRichTextBox = dynamic_cast<RichTextBox*>(pControl);
        if (pRichTextBox != nullptr) {
            pRichTextImpl = pRichTextBox->GetRichTextImpl();
        }
    }
    if (pRichTextImpl == nullptr) {
        RichTextHBox* pRichTextBox = dynamic_cast<RichTextHBox*>(pControl);
        if (pRichTextBox != nullptr) {
            pRichTextImpl = pRichTextBox->GetRichTextImpl();
        }
    }
    if (pRichTextImpl == nullptr) {
        RichTextVBox* pRichTextBox = dynamic_cast<RichTextVBox*>(pControl);
        if (pRichTextBox != nullptr) {
            pRichTextImpl = pRichTextBox->GetRichTextImpl();
        }
    }
    ASSERT(pRichTextImpl != nullptr);
    return ParseRichTextXmlNode(xmlNode, pRichTextImpl, pTextSlice);
}

bool WindowBuilder::ParseRichTextXmlNode(const pugi::xml_node& xmlNode, RichTextImpl* pRichTextImpl, RichTextSlice* pTextSlice)
{
    ASSERT(pRichTextImpl != nullptr);
    if (pRichTextImpl == nullptr) {
        return false;
    }

    std::string nodeName;
    for (pugi::xml_node node : xmlNode.children()) {
        RichTextSlice textSlice;
        textSlice.m_nodeName = node.name();
        nodeName = textSlice.m_nodeName.c_str();

        bool bParseChildren = true;
        if (nodeName.empty()) {            
            //No node name; only read the text content, no need to recursively traverse child nodes
            textSlice.m_text = StringConvert::UTF8ToWString(pRichTextImpl->TrimText(node.value()));
            bParseChildren = false;
        }        
        else if (nodeName == "a") {
            textSlice.m_text = StringConvert::UTF8ToWString(pRichTextImpl->TrimText(node.first_child().value()));
            textSlice.m_linkUrl = StringUtil::Trim(node.attribute("href").as_string());
            //Hyperlink node, no need to recursively traverse child nodes
            bParseChildren = false;
        }
        else if (nodeName == "b") {
            //Bold text
            textSlice.m_fontInfo.m_bBold = true;
        }
        else if (nodeName == "i") {
            //Italic text
            textSlice.m_fontInfo.m_bItalic = true;
        }
        else if ((nodeName == "del") || (nodeName == "s") || (nodeName == "strike")) {
            //Strikethrough text
            textSlice.m_fontInfo.m_bStrikeOut = true;
        }
        else if ( (nodeName == "ins") || (nodeName == "u") ){
            //Underline
            textSlice.m_fontInfo.m_bUnderline = true;
        }
        else if (nodeName == "bgcolor") {
            //Background color
            textSlice.m_bgColor = StringUtil::Trim(node.attribute("color").as_string());
        }
        else if (nodeName == "font") {
            //Font settings: text color
            textSlice.m_textColor = node.attribute("color").as_string();
            textSlice.m_fontInfo.m_fontName = node.attribute("face").as_string();
            //The font size does not need DPI scaling; it is scaled according to the current DPI when drawing
            textSlice.m_fontInfo.m_fontSize = node.attribute("size").as_int();            
        }
        else if (nodeName == "br") {
            textSlice.m_text = L"\n";
            //Line break node, no need to recursively traverse child nodes
            bParseChildren = false;
        }
        else {
            //Ignore unknown nodes
            ASSERT(!"Found unknown xml node name!");
            continue;
        }
        if (bParseChildren) {
            //Recursively process child nodes
            ParseRichTextXmlNode(node, pRichTextImpl, &textSlice);
        }
        //Add the child node to the Control or the parent node (note: after std::move, the textSlice object is invalid)
        if (pTextSlice != nullptr) {
            pTextSlice->m_children.emplace_back(std::move(textSlice));
        }
        else {
            pRichTextImpl->AppendTextSlice(std::move(textSlice));
        }
    }
    return true;
}

void WindowBuilder::AttachXmlEvent(bool bBubbled, const pugi::xml_node& node, Control* pParent)
{
    ASSERT(pParent != nullptr);
    if (pParent == nullptr) {
        return;
    }
    std::string strType;
    std::string strReceiver;
    std::string strApplyAttribute;
    std::string strName;
    std::string strValue;
    int i = 0;
    for (pugi::xml_attribute attr : node.attributes()) {
        strName = attr.name();
        strValue = attr.value();
        ASSERT_UNUSED_VARIABLE(i != 0 || strName == "type");
        ASSERT_UNUSED_VARIABLE(i != 1 || strName == "receiver");
        ASSERT_UNUSED_VARIABLE(i != 2 || ((strName == "applyattribute") || (strName == "apply_attribute")));
        ++i;
        if (strName == "type" ) {
            strType = strValue;
        }
        else if (strName == "receiver" ) {
            strReceiver = strValue;
        }
        else if ((strName == "apply_attribute") || (strName == "applyattribute")) {
            strApplyAttribute = strValue;
        }
    }

    auto typeList = StringUtil::Split(strType, " ");
    auto receiverList = StringUtil::Split(strReceiver, " ");
    for (auto itType = typeList.begin(); itType != typeList.end(); itType++) {
        if (receiverList.empty()) {
            receiverList.push_back("");
        }
        for (auto itReceiver = receiverList.begin(); itReceiver != receiverList.end(); itReceiver++) {
            EventType eventType = EventUtils::StringToEventType(*itType);
            ASSERT(eventType != EventType::kEventNone);//If an assertion occurs, the message name configured in the XML is incorrect
            if (eventType == EventType::kEventNone) {
                continue;
            }
            auto callback = UiBind(&Control::OnApplyAttributeList, pParent, *itReceiver, strApplyAttribute, std::placeholders::_1);
            if (!bBubbled) {
                pParent->AttachXmlEvent(eventType, callback, 0);
            }
            else {
                pParent->AttachXmlBubbledEvent(eventType, callback, 0);
            }
        }
    }
}

Box* WindowBuilder::ToBox(Control* pControl) const
{
    if (pControl == nullptr) {
        return nullptr;
    }
    Box* pBox = dynamic_cast<Box*>(pControl);
    ASSERT(pBox != nullptr);
    return pBox;
}

bool WindowBuilder::ParseWindowAttributes(std::map<std::string, std::string>& windowAttributes) const
{
    if (m_xml == nullptr) {
        return false;
    }
    pugi::xml_node root = m_xml->root().first_child();
    ASSERT(!root.empty());
    if (root.empty()) {
        return false;
    }

    std::string strClass = root.name();
    if (strClass == "Window") {
        for (pugi::xml_attribute attr : root.attributes()) {
            windowAttributes[attr.name()] = attr.value();            
        }
        return true;
    }
    return false;
}

const std::vector<std::string>& WindowBuilder::GetWindowClassList() const
{
    return m_windowClassList;
}

const std::vector<std::string>& WindowBuilder::GetWindowTextColorList() const
{
    return m_windowTextColorList;
}

const std::vector<std::string>& WindowBuilder::GetGlobalFontIdList() const
{
    return m_globalFontIdList;
}

} // namespace ui
