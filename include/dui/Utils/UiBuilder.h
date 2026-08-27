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
#include "dui/Core/WindowCreateParam.h"

#include <initializer_list>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <functional>

namespace ui
{

/** One XML-style attribute, used with the concise UI builder helpers.
 */
struct UiAttr
{
    DString name;
    DString value;

    UiAttr(const DString& n, const DString& v) : name(n), value(v) {}
    UiAttr(const wchar_t* n, const wchar_t* v) : name(n), value(v) {}
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

/** Typed control lookup. Eliminates the usual dynamic_cast + null-check boilerplate:
 *      if (auto* btn = ui::Find<ui::Button>(this, _T("hello_btn"))) { ... }
 */
template <class T>
T* Find(Window* w, const DString& name)
{
    return dynamic_cast<T*>(w->FindControl(name));
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
inline int RunWindow(const DString& title, std::function<void(WindowImplBase*)> init)
{
    class WindowApp : public FrameworkThread
    {
    public:
        WindowApp(const DString& t, std::function<void(WindowImplBase*)> f)
            : FrameworkThread(_T("App"), kThreadUI), m_title(t), m_init(std::move(f)) {}
        void Run() { RunMessageLoop(); }
    protected:
        void OnInit() override
        {
            FilePath resourcePath = FilePathUtil::GetCurrentModuleDirectory();
            resourcePath += _T("resources\\");

            if (!GlobalManager::Instance().Startup(LocalFilesResParam(resourcePath))) {
                SystemUtil::ShowMessageBox(nullptr, _T("Failed to load resources from the repository."), _T("dui"));
                return;
            }

            WindowImplBase* window = new WindowImplBase();
            window->AttachWindowCreateMsg([this, window](const EventArgs& /*args*/) {
                if (m_init) m_init(window);
                return true;
            });
            if (!window->CreateWnd(nullptr, WindowCreateParam(m_title, true))) {
                SystemUtil::ShowMessageBox(nullptr, _T("Failed to create the window."), _T("dui"));
                delete window;
                return;
            }
            window->PostQuitMsgWhenClosed(true);
            window->ShowWindow(kSW_SHOW_NORMAL);
        }
        void OnCleanup() override
        {
            GlobalManager::Instance().Shutdown();
        }
    private:
        DString m_title;
        std::function<void(WindowImplBase*)> m_init;
    };

    WindowApp app(title, std::move(init));
    app.Run();
    return 0;
}

/** Named options for the XML window entry.  Clearer than three positional parameters:
 *
 *      DUI_XML_APP(ui::XmlWindowOptions()
 *          .Title(_T("basic"))
 *          .SkinFolder(_T("basic"))
 *          .SkinFile(_T("basic.xml")))
 */
struct XmlWindowOptions
{
    DString title;
    DString skinFolder;
    DString skinFile;

    XmlWindowOptions& Title(const DString& value)       { title = value; return *this; }
    XmlWindowOptions& SkinFolder(const DString& value)  { skinFolder = value; return *this; }
    XmlWindowOptions& SkinFile(const DString& value)    { skinFile = value; return *this; }

    // Short aliases
    XmlWindowOptions& Skin(const DString& value)  { return SkinFolder(value); }
    XmlWindowOptions& Xml(const DString& value)   { return SkinFile(value); }
};

/** Startup an XML-defined window directly, without writing a MainForm subclass.
 *  This is the “Qt-like minimal” entry for XML-mode examples:
 *
 *      DUI_XML_APP(_T("Hello"), _T("hello"), _T("hello.xml"))
 */
inline int RunXml(const DString& title, const DString& skinFolder, const DString& skinFile)
{
    class XmlApp : public FrameworkThread
    {
    public:
        XmlApp(const DString& t, const DString& folder, const DString& file)
            : FrameworkThread(_T("App"), kThreadUI), m_title(t), m_folder(folder), m_file(file) {}
        void Run() { RunMessageLoop(); }
    protected:
        void OnInit() override
        {
            FilePath resourcePath = FilePathUtil::GetCurrentModuleDirectory();
            resourcePath += _T("resources\\");

            if (!GlobalManager::Instance().Startup(LocalFilesResParam(resourcePath))) {
                SystemUtil::ShowMessageBox(nullptr, _T("Failed to load resources from the repository."), _T("dui"));
                return;
            }

            WindowImplBase* window = new WindowImplBase();
            window->InitSkin(m_folder, m_file);
            if (!window->CreateWnd(nullptr, WindowCreateParam(m_title, true))) {
                SystemUtil::ShowMessageBox(nullptr, _T("Failed to create the window."), _T("dui"));
                delete window;
                return;
            }
            window->PostQuitMsgWhenClosed(true);
            window->ShowWindow(kSW_SHOW_NORMAL);
        }
        void OnCleanup() override
        {
            GlobalManager::Instance().Shutdown();
        }
    private:
        DString m_title;
        DString m_folder;
        DString m_file;
    };

    XmlApp app(title, skinFolder, skinFile);
    app.Run();
    return 0;
}

inline int RunXml(const XmlWindowOptions& options)
{
    return RunXml(options.title, options.skinFolder, options.skinFile);
}

/** Startup a simple WindowImplBase window and run the UI message loop.
 *  This removes the repetitive App/FrameworkThread boilerplate from examples:
 *
 *      #include "dui/Utils/UiBuilder.h"
 *      DUI_SIMPLE_APP(MainForm, _T("Hello"))
 */
template <class WindowT>
int Run(const DString& title)
{
    class SimpleApp : public FrameworkThread
    {
    public:
        explicit SimpleApp(const DString& t)
            : FrameworkThread(_T("App"), kThreadUI), m_title(t) {}
        void Run() { RunMessageLoop(); }
    protected:
        void OnInit() override
        {
            FilePath resourcePath = FilePathUtil::GetCurrentModuleDirectory();
            resourcePath += _T("resources\\");

            if (!GlobalManager::Instance().Startup(LocalFilesResParam(resourcePath))) {
                SystemUtil::ShowMessageBox(nullptr, _T("Failed to load resources from the repository."), _T("dui"));
                return;
            }

            WindowT* window = new WindowT();
            if (!window->CreateWnd(nullptr, WindowCreateParam(m_title, true))) {
                SystemUtil::ShowMessageBox(nullptr, _T("Failed to create the window."), _T("dui"));
                delete window;
                return;
            }
            window->PostQuitMsgWhenClosed(true);
            window->ShowWindow(kSW_SHOW_NORMAL);
        }
        void OnCleanup() override
        {
            GlobalManager::Instance().Shutdown();
        }
    private:
        DString m_title;
    };

    SimpleApp app(title);
    app.Run();
    return 0;
}


/** Startup a WindowImplBase window with embedded (memory) resources.
 *  Used by *_code / *_gen examples that embed resources in the executable.
 */
template <class WindowT>
int RunMemory(const DString& title, const uint8_t* data, size_t size)
{
    class MemoryApp : public FrameworkThread
    {
    public:
        MemoryApp(const DString& t, const uint8_t* d, size_t s)
            : FrameworkThread(_T("App"), kThreadUI), m_title(t), m_data(d), m_size(s) {}
        void Run() { RunMessageLoop(); }
    protected:
        void OnInit() override
        {
            if (!GlobalManager::Instance().Startup(MemoryResParam(m_data, m_size))) {
                SystemUtil::ShowMessageBox(nullptr, _T("Failed to load embedded resources."), _T("dui"));
                return;
            }

            WindowT* window = new WindowT();
            if (!window->CreateWnd(nullptr, WindowCreateParam(m_title, true))) {
                SystemUtil::ShowMessageBox(nullptr, _T("Failed to create the window."), _T("dui"));
                delete window;
                return;
            }
            window->PostQuitMsgWhenClosed(true);
            window->ShowWindow(kSW_SHOW_NORMAL);
        }
        void OnCleanup() override
        {
            GlobalManager::Instance().Shutdown();
        }
    private:
        DString m_title;
        const uint8_t* m_data;
        size_t m_size;
    };

    MemoryApp app(title, data, size);
    app.Run();
    return 0;
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
