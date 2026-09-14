#ifndef DUI_UTILS_UIBUILDER_H_
#define DUI_UTILS_UIBUILDER_H_

#include "dui/dui_defs.h"
#include "dui/Core/Control.h"
#include "dui/Core/Box.h"
#include "dui/Core/Window.h"
#include "dui/Utils/WinImplBase.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/FrameworkThread.h"
#include "dui/Utils/FilePathUtil.h"
#include "dui/Utils/StringConvert.h"
#include "dui/Utils/SystemUtil.h"
#include "dui/Utils/AttributeUtil.h"
#include "dui/Core/WindowCreateParam.h"

#include <initializer_list>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <functional>
#include <span>

namespace ui
{

/** One XML-style attribute, used with the concise UI builder helpers.
 */
struct UiAttr
{
    std::string name;
    std::string value;

    UiAttr(const std::string& n, const std::string& v) : name(n), value(v) {}
    UiAttr(const wchar_t* n, const wchar_t* v)
    {
        name = StringConvert::WStringToT(n ? n : L"");
        value = StringConvert::WStringToT(v ? v : L"");
    }
    UiAttr(const char* n, const char* v)
    {
        name = StringConvert::UTF8ToT(n ? n : "");
        value = StringConvert::UTF8ToT(v ? v : "");
    }
};

inline void ApplyAttrs(Control* c, std::initializer_list<UiAttr> attrs)
{
    for (const auto& a : attrs) {
        c->SetAttribute(a.name, a.value);
    }
}

/** Apply the attributes from an XML Window node after the native window exists. */
inline void ApplyWindowAttrs(Window* window, std::initializer_list<UiAttr> attrs)
{
    if (window == nullptr) {
        return;
    }

    bool hasShadowAttached = false;
    bool shadowAttached = false;
    bool hasWindowSize = false;
    bool hasWindowPosition = false;
    UiPoint windowPosition;
    for (const auto& attr : attrs) {
        const std::string& name = attr.name;
        const std::string& value = attr.value;
        if (name == "render_backend_type") {
            RenderBackendType backend = RenderBackendType::kRaster_BackendType;
            if (StringUtil::IsEqualNoCase(value, "GL") || StringUtil::IsEqualNoCase(value, "GPU")) {
                backend = RenderBackendType::kNativeGL_BackendType;
            }
            else if (StringUtil::IsEqualNoCase(value, "Metal")) {
                backend = RenderBackendType::kMetal_BackendType;
            }
            window->SetRenderBackendType(backend);
        }
        else if (name == "min_size" || name == "mininfo") {
            UiSize size;
            AttributeUtil::ParseSizeValue(value.c_str(), size);
            window->SetWindowMinimumSize(size, true);
        }
        else if (name == "max_size" || name == "maxinfo") {
            UiSize size;
            AttributeUtil::ParseSizeValue(value.c_str(), size);
            window->SetWindowMaximumSize(size, true);
        }
        else if (name == "use_system_caption") {
            window->SetUseSystemCaption(value == "true");
        }
        else if (name == "size_box" || name == "sizebox") {
            UiRect rect;
            AttributeUtil::ParseRectValue(value.c_str(), rect, false);
            window->SetSizeBox(rect, true);
        }
        else if (name == "caption") {
            UiRect rect;
            AttributeUtil::ParseRectValue(value.c_str(), rect);
            window->SetCaptionRect(rect, true);
        }
        else if (name == "snap_layout_menu") {
            window->SetEnableSnapLayoutMenu(value == "true");
        }
        else if (name == "sys_menu") {
            window->SetEnableSysMenu(value == "true");
        }
        else if (name == "sys_menu_rect") {
            UiRect rect;
            AttributeUtil::ParseRectValue(value.c_str(), rect);
            window->SetSysMenuRect(rect, true);
        }
        else if (name == "icon") {
            window->SetWindowIcon(value);
        }
        else if (name == "text") {
            window->SetText(value);
        }
        else if (name == "text_id" || name == "textid") {
            window->SetTextId(value);
        }
        else if (name == "round_corner" || name == "roundcorner") {
            UiSize size;
            AttributeUtil::ParseSizeValue(value.c_str(), size);
            window->SetRoundCorner(size.cx, size.cy, true);
        }
        else if (name == "alpha_fix_corner" || name == "alphafixcorner") {
            UiRect rect;
            AttributeUtil::ParseRectValue(value.c_str(), rect);
            window->SetAlphaFixCorner(rect, true);
        }
        else if (name == "shadow_attached" || name == "shadowattached") {
            hasShadowAttached = true;
            shadowAttached = value == "true";
        }
        else if (name == "shadow_type") {
            Shadow::ShadowType shadowType = Shadow::ShadowType::kShadowCount;
            if (Shadow::GetShadowType(value, shadowType)) {
                window->SetShadowType(shadowType);
            }
        }
        else if (name == "shadow_image" || name == "shadowimage") {
            window->SetShadowImage(value);
        }
        else if (name == "shadow_corner" || name == "shadowcorner") {
            UiPadding padding;
            AttributeUtil::ParsePaddingValue(value.c_str(), padding);
            window->SetShadowCorner(padding);
        }
        else if (name == "shadow_border_round") {
            UiSize size;
            AttributeUtil::ParseSizeValue(value.c_str(), size);
            window->SetShadowBorderRound(size);
        }
        else if (name == "shadow_border_size") {
            window->SetShadowBorderSize(StringUtil::StringToInt32(value));
        }
        else if (name == "shadow_border_color") {
            window->SetShadowBorderColor(value);
        }
        else if (name == "shadow_snap") {
            window->SetEnableShadowSnap(value == "true");
        }
        else if (name == "layered_window" || name == "layeredwindow") {
            if (!window->IsUseSystemCaption()) {
                window->SetLayeredWindow(value == "true", false);
            }
        }
        else if (name == "alpha") {
            window->SetLayeredWindowAlpha(StringUtil::StringToInt32(value));
        }
        else if (name == "opacity") {
            window->SetLayeredWindowOpacity(StringUtil::StringToInt32(value));
        }
        else if (name == "drag_drop") {
            window->SetEnableDragDrop(value == "true");
        }
        else if (name == "size") {
            hasWindowSize = true;
            UiSize size;
            bool scaledCX = false;
            bool scaledCY = false;
            bool percentCX = false;
            bool percentCY = false;
            AttributeUtil::ParseWindowSize(window, value.c_str(), size,
                                           &scaledCX, &scaledCY, &percentCX, &percentCY);
            window->SetWindowSize(size.cx, size.cy);
        }
        else if (name == "position" || name == "pos") {
            hasWindowPosition = true;
            AttributeUtil::ParsePointValue(value.c_str(), windowPosition);
        }
    }

    // Apply this last so the selected shadow type is already established.
    if (hasShadowAttached) {
        window->SetShadowAttached(shadowAttached);
    }
    if (hasWindowPosition) {
        UiRect windowRect = window->GetWindowPos(false);
        window->MoveWindow(windowPosition.x, windowPosition.y,
                           windowRect.Width(), windowRect.Height(), true);
    }
    else if (hasWindowSize) {
        // The size is applied after native creation, so center it again.
        window->CenterWindow();
    }
}

/** Create a control not attached to a parent yet.
 *  This is the base helper for the concise pure-code UI syntax:
 *      auto* root = ui::Create<ui::VBox>(w, {{"bkcolor", "bk_wnd_darkcolor"}});
 */
template <class T, class... Children>
T* Create(Window* w, std::initializer_list<UiAttr> attrs, Children... children)
{
    static_assert(std::is_base_of_v<Control, T>, "ui::Create<T>: T must be a dui control");
    T* c = new T(w);
    ApplyAttrs(c, attrs);
    if constexpr (sizeof...(Children) > 0) {
        static_assert(std::is_base_of_v<Box, T>, "ui::Create<T>: only Box controls can have children");
        (c->AddItem(children), ...);
    }
    return c;
}

template <class T, class... Children>
T* Create(Window* w, Children... children)
{
    return Create<T>(w, {}, children...);
}

/** Create a control and attach it to a parent container.
 *  This is the main helper for concise pure-code UI syntax:
 *      auto* label = ui::Attach<ui::Label>(root, {{"text", "Hello"}});
 */
template <class T>
T* Attach(Box* parent, std::initializer_list<UiAttr> attrs = {})
{
    static_assert(std::is_base_of_v<Control, T>, "ui::Attach<T>: T must be a dui control");
    T* c = Create<T>(parent->GetWindow(), attrs);
    parent->AddItem(c);
    return c;
}

/** Attach an already-created control without creating another instance. */
template <class T>
T* Attach(Box* parent, T* control)
{
    static_assert(std::is_base_of_v<Control, T>, "ui::Attach: T must be a dui control");
    if (parent != nullptr && control != nullptr) {
        parent->AddItem(control);
    }
    return control;
}

/** Attach the root container to a window (uniform with ui::Attach for boxes). */
inline Box* Attach(Window* w, Box* root)
{
    if (w != nullptr && root != nullptr) {
        w->AttachBox(root);
    }
    return root;
}

/** Typed control lookup. Eliminates the usual dynamic_cast + null-check boilerplate:
 *      if (auto* btn = ui::Find<ui::Button>(this, "hello_btn")) { ... }
 */
template <class T>
T* Find(Window* w, const std::string& name)
{
    return dynamic_cast<T*>(w->FindControl(name));
}

template <class T>
T* Find(Window* w, const char* name)
{
    return dynamic_cast<T*>(w->FindControl(StringConvert::UTF8ToT(name ? name : "")));
}

/** Bind a click handler to a named control when it exists. */
inline void OnClick(Window* w, const std::string& name, std::function<bool(const EventArgs&)> handler)
{
    if (w == nullptr) {
        return;
    }
    if (Control* control = w->FindControl(name)) {
        control->AttachClick(std::move(handler));
    }
}

/** Convenience binder: attach the common click handler. */
inline void OnClick(Control* c, std::function<bool(const EventArgs&)> handler)
{
    c->AttachClick(std::move(handler));
}

/** Startup a plain window and run an initializer callback.
 *  This is the “Qt-like minimal” entry for pure-code windows:
 *  no MainForm subclass is required.
 */
inline int RunWindow(const std::string& title, std::function<void(WindowImplBase*)> init)
{
    class WindowApp : public FrameworkThread
    {
    public:
        WindowApp(const std::string& t, std::function<void(WindowImplBase*)> f)
            : FrameworkThread("App", kThreadUI), m_title(t), m_init(std::move(f)) {}
        void Run() { RunMessageLoop(); }
    protected:
        void OnInit() override
        {
            FilePath resourcePath = FilePathUtil::GetCurrentModuleDirectory();
            resourcePath += "resources\\";

            if (!GlobalManager::Instance().Startup(LocalFilesResParam(resourcePath))) {
                SystemUtil::ShowMessageBox(nullptr, "Failed to load resources from the repository.", "dui");
                return;
            }

            WindowImplBase* window = new WindowImplBase();
            window->AttachWindowCreateMsg([this, window](const EventArgs& /*args*/) {
                if (m_init) m_init(window);
                return true;
            });
            if (!window->CreateWnd(nullptr, WindowCreateParam(m_title, true))) {
                SystemUtil::ShowMessageBox(nullptr, "Failed to create the window.", "dui");
                delete window;
                return;
            }
            window->PostQuitMsgWhenClosed(true);
            window->InvalidateAll();
            window->UpdateWindow();
            window->ShowWindow(kSW_SHOW_NORMAL);
        }
        void OnCleanup() override
        {
            GlobalManager::Instance().Shutdown();
        }
    private:
        std::string m_title;
        std::string m_folder;
        std::string m_file;
        std::function<void(WindowImplBase*)> m_init;
    };

    WindowApp app(title, std::move(init));
    app.Run();
    return 0;
}

/** Named options for the XML window entry.  Clearer than three positional parameters:
 *
 *      DUI_XML_APP(ui::XmlWindowOptions()
 *          .Title("basic")
 *          .SkinFolder("basic")
 *          .SkinFile("basic.xml"))
 */
struct XmlWindowOptions
{
    std::string title;
    std::string skinFolder;
    std::string skinFile;

    XmlWindowOptions& Title(const std::string& value)       { title = value; return *this; }
    XmlWindowOptions& SkinFolder(const std::string& value)  { skinFolder = value; return *this; }
    XmlWindowOptions& SkinFile(const std::string& value)    { skinFile = value; return *this; }

    // Short aliases
    XmlWindowOptions& Skin(const std::string& value)  { return SkinFolder(value); }
    XmlWindowOptions& Xml(const std::string& value)   { return SkinFile(value); }
};

/** Startup an XML-defined window directly, without writing a MainForm subclass.
 *  This is the “Qt-like minimal” entry for XML-mode examples:
 *
 *      DUI_XML_APP("Hello", "hello", "hello.xml")
 */
inline int RunXml(const std::string& title, const std::string& skinFolder, const std::string& skinFile)
{
    class XmlApp : public FrameworkThread
    {
    public:
        XmlApp(const std::string& t, const std::string& folder, const std::string& file)
            : FrameworkThread("App", kThreadUI), m_title(t), m_folder(folder), m_file(file) {}
        void Run() { RunMessageLoop(); }
    protected:
        void OnInit() override
        {
            FilePath resourcePath = FilePathUtil::GetCurrentModuleDirectory();
            resourcePath += "resources\\";

            if (!GlobalManager::Instance().Startup(LocalFilesResParam(resourcePath))) {
                SystemUtil::ShowMessageBox(nullptr, "Failed to load resources from the repository.", "dui");
                return;
            }

            WindowImplBase* window = new WindowImplBase();
            window->InitSkin(m_folder, m_file);
            if (!window->CreateWnd(nullptr, WindowCreateParam(m_title, true))) {
                SystemUtil::ShowMessageBox(nullptr, "Failed to create the window.", "dui");
                delete window;
                return;
            }
            window->PostQuitMsgWhenClosed(true);
            window->InvalidateAll();
            window->UpdateWindow();
            window->ShowWindow(kSW_SHOW_NORMAL);
        }
        void OnCleanup() override
        {
            GlobalManager::Instance().Shutdown();
        }
    private:
        std::string m_title;
        std::string m_folder;
        std::string m_file;
    };

    XmlApp app(title, skinFolder, skinFile);
    app.Run();
    return 0;
}

inline int RunXml(const XmlWindowOptions& options)
{
    return RunXml(options.title, options.skinFolder, options.skinFile);
}

/** Startup a custom WindowImplBase subclass from an XML layout. */
template <class WindowT>
int RunXml(const std::string& title)
{
    static_assert(std::is_base_of_v<WindowImplBase, WindowT>,
                  "ui::RunXml<WindowT>: WindowT must derive from WindowImplBase");

    class XmlWindowApp : public FrameworkThread
    {
    public:
        explicit XmlWindowApp(const std::string& t)
            : FrameworkThread("App", kThreadUI), m_title(t) {}

        void Run() { RunMessageLoop(); }

    protected:
        void OnInit() override
        {
            FilePath resourcePath = FilePathUtil::GetCurrentModuleDirectory();
            resourcePath += "resources\\";
            if (!GlobalManager::Instance().Startup(LocalFilesResParam(resourcePath))) {
                SystemUtil::ShowMessageBox(nullptr, "Failed to load resources from the repository.", "dui");
                return;
            }

            WindowT* window = new WindowT();
            if (!window->CreateWnd(nullptr, WindowCreateParam(m_title, true))) {
                SystemUtil::ShowMessageBox(nullptr, "Failed to create the window.", "dui");
                delete window;
                return;
            }
            window->PostQuitMsgWhenClosed(true);
            window->InvalidateAll();
            window->UpdateWindow();
            window->ShowWindow(kSW_SHOW_NORMAL);
        }

        void OnCleanup() override
        {
            GlobalManager::Instance().Shutdown();
        }

    private:
        std::string m_title;
    };

    XmlWindowApp app(title);
    app.Run();
    return 0;
}

/** Startup a simple WindowImplBase window and run the UI message loop.
 *  This removes the repetitive App/FrameworkThread boilerplate from examples:
 *
 *      #include "dui/Utils/UiBuilder.h"
 *      DUI_SIMPLE_APP(MainForm, "Hello")
 */
template <class WindowT>
int Run(const std::string& title, const std::function<void(WindowT*)>& idleCallback = nullptr)
{
    class SimpleApp : public FrameworkThread
    {
    public:
        SimpleApp(const std::string& t, const std::function<void(WindowT*)>& callback)
            : FrameworkThread("App", kThreadUI), m_title(t), m_idleCallback(callback) {}
        void Run() { RunMessageLoop(m_idleCallback != nullptr); }
    protected:
        void OnInit() override
        {
            FilePath resourcePath = FilePathUtil::GetCurrentModuleDirectory();
            resourcePath += "resources\\";

            LocalFilesResParam resParam(resourcePath);

            if (!GlobalManager::Instance().Startup(resParam)) {
                SystemUtil::ShowMessageBox(nullptr, "Failed to load resources from the repository.", "dui");
                return;
            }

            WindowT* window = new WindowT();
            m_window = window;
            WindowCreateParam createParam(m_title, true);
#if defined(DUI_BUILD_FOR_WIN)
            // Code-only windows configure their custom caption in OnInitWindow,
            // which runs after native creation. Start without the system frame.
            createParam.m_dwStyle = kWS_POPUP;
#endif
            if (!window->CreateWnd(nullptr, createParam)) {
                SystemUtil::ShowMessageBox(nullptr, "Failed to create the window.", "dui");
                delete window;
                m_window = nullptr;
                return;
            }
            window->PostQuitMsgWhenClosed(true);
            window->InvalidateAll();
            window->UpdateWindow();
            window->ShowWindow(kSW_SHOW_NORMAL);
        }
        void OnCleanup() override
        {
            GlobalManager::Instance().Shutdown();
        }
        void OnMessageLoopIdle() override
        {
            if (m_window != nullptr && m_idleCallback != nullptr) {
                m_idleCallback(m_window);
            }
        }
    private:
        std::string m_title;
        WindowT* m_window = nullptr;
        std::function<void(WindowT*)> m_idleCallback;
    };

    SimpleApp app(title, idleCallback);
    app.Run();
    return 0;
}

/** Startup a WindowImplBase window with embedded resources, accessed directly from memory.
 *  Used by *_code / *_gen examples that embed resources in the executable.
 */
template <class WindowT>
int RunMemory(const std::string& title, std::span<const uint8_t> resources,
              const std::function<void(WindowT*)>& idleCallback = nullptr)
{
    class MemoryApp : public FrameworkThread
    {
    public:
        MemoryApp(const std::string& t, std::span<const uint8_t> resources,
                  const std::function<void(WindowT*)>& callback)
            : FrameworkThread("App", kThreadUI), m_title(t), m_resources(resources),
              m_idleCallback(callback) {}
        void Run() { RunMessageLoop(m_idleCallback != nullptr); }
    protected:
        void OnInit() override
        {
            if (!GlobalManager::Instance().Startup(MemoryResParam(m_resources))) {
                SystemUtil::ShowMessageBox(nullptr, "Failed to load embedded resources.", "dui");
                return;
            }

            WindowT* window = new WindowT();
            m_window = window;
            WindowCreateParam createParam(m_title, true);
#if defined(DUI_BUILD_FOR_WIN)
            // Code-only windows configure their custom caption in OnInitWindow,
            // which runs after native creation. Start without the system frame.
            createParam.m_dwStyle = kWS_POPUP;
#endif
            if (!window->CreateWnd(nullptr, createParam)) {
                SystemUtil::ShowMessageBox(nullptr, "Failed to create the window.", "dui");
                delete window;
                m_window = nullptr;
                return;
            }
            window->PostQuitMsgWhenClosed(true);
            // Render the initial frame while the window is still hidden so
            // showing it never exposes the native default background.
            window->InvalidateAll();
            window->UpdateWindow();
            window->ShowWindow(kSW_SHOW_NORMAL);
        }
        void OnCleanup() override
        {
            GlobalManager::Instance().Shutdown();
        }
        void OnMessageLoopIdle() override
        {
            if (m_window != nullptr && m_idleCallback != nullptr) {
                m_idleCallback(m_window);
            }
        }
    private:
        std::string m_title;
        std::span<const uint8_t> m_resources;
        WindowT* m_window = nullptr;
        std::function<void(WindowT*)> m_idleCallback;
    };

    MemoryApp app(title, resources, idleCallback);
    app.Run();
    return 0;
}

/** Narrow-string overload of RunMemory (UTF-8 title). */
template <class WindowT>
int RunMemory(const char* title, std::span<const uint8_t> resources,
              const std::function<void(WindowT*)>& idleCallback = nullptr)
{
    return RunMemory<WindowT>(StringConvert::UTF8ToT(title ? title : ""), resources, idleCallback);
}

} // namespace ui

#if defined(DUI_BUILD_FOR_WIN)
    #if defined(DUI_COMPILER_MINGW)
        #define DUI_SIMPLE_APP(WindowClass, title) \
            int APIENTRY WinMain(_In_ HINSTANCE /*hInstance*/, \
                                 _In_opt_ HINSTANCE /*hPrevInstance*/, \
                                 _In_ LPSTR /*lpCmdLine*/, \
                                 _In_ int /*nCmdShow*/) \
            { \
                return ui::Run<WindowClass>(title); \
            }
    #else
        #define DUI_SIMPLE_APP(WindowClass, title) \
            int APIENTRY wWinMain(_In_ HINSTANCE /*hInstance*/, \
                                  _In_opt_ HINSTANCE /*hPrevInstance*/, \
                                  _In_ LPWSTR /*lpCmdLine*/, \
                                  _In_ int /*nCmdShow*/) \
            { \
                return ui::Run<WindowClass>(title); \
            }
    #endif
#elif defined(DUI_BUILD_FOR_LINUX) || defined(DUI_BUILD_FOR_FREEBSD)
    #define DUI_SIMPLE_APP(WindowClass, title) \
        int main(int argc, char** argv) \
        { \
            (void)argc; (void)argv; \
            return ui::Run<WindowClass>(title); \
        }
#elif defined(DUI_BUILD_FOR_MACOS)
    #define DUI_SIMPLE_APP(WindowClass, title) \
        int main(int argc, char** argv) \
        { \
            CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, false); \
            (void)argc; (void)argv; \
            return ui::Run<WindowClass>(title); \
        }
#else
    #error "Unknown Platform!"
#endif

#if defined(DUI_BUILD_FOR_WIN)
    #if defined(DUI_COMPILER_MINGW)
        #define DUI_XML_APP(...) \
            int APIENTRY WinMain(_In_ HINSTANCE /*hInstance*/, \
                                 _In_opt_ HINSTANCE /*hPrevInstance*/, \
                                 _In_ LPSTR /*lpCmdLine*/, \
                                 _In_ int /*nCmdShow*/) \
            { \
                return ui::RunXml(__VA_ARGS__); \
            }
    #else
        #define DUI_XML_APP(...) \
            int APIENTRY wWinMain(_In_ HINSTANCE /*hInstance*/, \
                                  _In_opt_ HINSTANCE /*hPrevInstance*/, \
                                  _In_ LPWSTR /*lpCmdLine*/, \
                                  _In_ int /*nCmdShow*/) \
            { \
                return ui::RunXml(__VA_ARGS__); \
            }
    #endif
#elif defined(DUI_BUILD_FOR_LINUX) || defined(DUI_BUILD_FOR_FREEBSD)
    #define DUI_XML_APP(...) \
        int main(int argc, char** argv) \
        { \
            (void)argc; (void)argv; \
            return ui::RunXml(__VA_ARGS__); \
        }
#elif defined(DUI_BUILD_FOR_MACOS)
    #define DUI_XML_APP(...) \
        int main(int argc, char** argv) \
        { \
            CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, false); \
            (void)argc; (void)argv; \
            return ui::RunXml(__VA_ARGS__); \
        }
#else
    #error "Unknown Platform!"
#endif


#if defined(DUI_BUILD_FOR_WIN)
    #if defined(DUI_COMPILER_MINGW)
        #define DUI_WINDOW_APP(title, ...) \
            int APIENTRY WinMain(_In_ HINSTANCE /*hInstance*/, \
                                 _In_opt_ HINSTANCE /*hPrevInstance*/, \
                                 _In_ LPSTR /*lpCmdLine*/, \
                                 _In_ int /*nCmdShow*/) \
            { \
                return ui::RunWindow((title), [](ui::WindowImplBase* w) { __VA_ARGS__ }); \
            }
    #else
        #define DUI_WINDOW_APP(title, ...) \
            int APIENTRY wWinMain(_In_ HINSTANCE /*hInstance*/, \
                                  _In_opt_ HINSTANCE /*hPrevInstance*/, \
                                  _In_ LPWSTR /*lpCmdLine*/, \
                                  _In_ int /*nCmdShow*/) \
            { \
                return ui::RunWindow((title), [](ui::WindowImplBase* w) { __VA_ARGS__ }); \
            }
    #endif
#elif defined(DUI_BUILD_FOR_LINUX) || defined(DUI_BUILD_FOR_FREEBSD)
    #define DUI_WINDOW_APP(title, ...) \
        int main(int argc, char** argv) \
        { \
            (void)argc; (void)argv; \
            return ui::RunWindow((title), [](ui::WindowImplBase* w) { __VA_ARGS__ }); \
        }
#elif defined(DUI_BUILD_FOR_MACOS)
    #define DUI_WINDOW_APP(title, ...) \
        int main(int argc, char** argv) \
        { \
            CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, false); \
            (void)argc; (void)argv; \
            return ui::RunWindow((title), [](ui::WindowImplBase* w) { __VA_ARGS__ }); \
        }
#else
    #error "Unknown Platform!"
#endif


#endif // DUI_UTILS_UIBUILDER_H_
