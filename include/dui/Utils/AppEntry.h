#ifndef DUI_APP_ENTRY_H_
#define DUI_APP_ENTRY_H_

#include "dui/dui_config.h"

/** @file AppEntry.h
*   Unified application entry point macros (Qt style: one main.cpp per executable).
*
*   Usage:
*   1. Include this header in the .cpp file that defines the entry point.
*   2. Include the header of the application class (AppClass) BEFORE invoking the macro.
*   3. Invoke the macro EXACTLY ONCE in EXACTLY ONE .cpp file of the executable,
*      at global scope (no trailing semicolon needed):
*         DUI_APP_ENTRY(MyApp)       // MyApp must provide: void Run();
*         DUI_APP_ENTRY_ARGS(MyApp)  // MyApp must provide:
*                                       //   static MyApp& Instance();
*                                       //   int Run(int argc, char** argv);
*
*   Notes:
*   - Never invoke the macro in a header file, and never more than once per
*     executable: each invocation defines the platform entry function
*     (main / WinMain / wWinMain), so a second invocation in another translation
*     unit causes a duplicate-symbol link error.
*   - On Windows, MSVC builds use wWinMain (paired with
*     /ENTRY:wWinMainCRTStartup) and MinGW builds use WinMain (paired with
*     -mwindows). wWinMain has no argc/argv, so DUI_APP_ENTRY_ARGS always
*     passes (0, nullptr) on Windows; CEF applications parse the command line
*     internally via GetCommandLineW, matching the previous behavior.
*   - On macOS, the entry function pre-warms the run loop with
*     CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, false) before running the
*     application, matching the historical main_macos.cpp behavior.
*   - CEF on macOS requires an Objective-C++ entry point (main_macos.mm, with
*     CefScopedLibraryLoader and an NSApplication subclass); do NOT invoke this
*     macro there. In main.cpp, guard the macro invocation with
*     #if !defined(__APPLE__).
*/

//============================================================
// DUI_APP_ENTRY(AppClass)
// AppClass must provide: void Run();
//============================================================
#if defined(DUI_BUILD_FOR_WIN)
    #if defined(DUI_COMPILER_MINGW)
        //MinGW-w64 build
        #define DUI_APP_ENTRY_IMPL(AppClass) \
            int APIENTRY WinMain(_In_ HINSTANCE /*hInstance*/, \
                                 _In_opt_ HINSTANCE /*hPrevInstance*/, \
                                 _In_ LPSTR /*lpCmdLine*/, \
                                 _In_ int /*nCmdShow*/) \
            { \
                AppClass app; \
                app.Run(); \
                return 0; \
            }
    #else
        //MSVC build
        #define DUI_APP_ENTRY_IMPL(AppClass) \
            int APIENTRY wWinMain(_In_ HINSTANCE /*hInstance*/, \
                                  _In_opt_ HINSTANCE /*hPrevInstance*/, \
                                  _In_ LPWSTR /*lpCmdLine*/, \
                                  _In_ int /*nCmdShow*/) \
            { \
                AppClass app; \
                app.Run(); \
                return 0; \
            }
    #endif
#elif defined(DUI_BUILD_FOR_LINUX) || defined(DUI_BUILD_FOR_FREEBSD)
    #define DUI_APP_ENTRY_IMPL(AppClass) \
        int main(int argc, char** argv) \
        { \
            AppClass app; \
            app.Run(); \
            return 0; \
        }
#elif defined(DUI_BUILD_FOR_MACOS)
    #define DUI_APP_ENTRY_IMPL(AppClass) \
        int main(int argc, char** argv) \
        { \
            /*macOS-specific initialization (e.g., activate multithreaded GCD)*/ \
            CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, false); \
            AppClass app; \
            app.Run(); \
            return 0; \
        }
#else
    #error "Unknown Platform!"
#endif

//============================================================
// DUI_APP_ENTRY_ARGS(AppClass)
// AppClass must provide:
//   static AppClass& Instance();
//   int Run(int argc, char** argv);
//============================================================
#if defined(DUI_BUILD_FOR_WIN)
    #if defined(DUI_COMPILER_MINGW)
        #define DUI_APP_ENTRY_ARGS_IMPL(AppClass) \
            int APIENTRY WinMain(_In_ HINSTANCE /*hInstance*/, \
                                 _In_opt_ HINSTANCE /*hPrevInstance*/, \
                                 _In_ LPSTR /*lpCmdLine*/, \
                                 _In_ int /*nCmdShow*/) \
            { \
                AppClass::Instance().Run(0, nullptr); \
                return 0; \
            }
    #else
        #define DUI_APP_ENTRY_ARGS_IMPL(AppClass) \
            int APIENTRY wWinMain(_In_ HINSTANCE /*hInstance*/, \
                                  _In_opt_ HINSTANCE /*hPrevInstance*/, \
                                  _In_ LPWSTR /*lpCmdLine*/, \
                                  _In_ int /*nCmdShow*/) \
            { \
                AppClass::Instance().Run(0, nullptr); \
                return 0; \
            }
    #endif
#elif defined(DUI_BUILD_FOR_LINUX) || defined(DUI_BUILD_FOR_FREEBSD)
    #define DUI_APP_ENTRY_ARGS_IMPL(AppClass) \
        int main(int argc, char** argv) \
        { \
            AppClass::Instance().Run(argc, argv); \
            return 0; \
        }
#elif defined(DUI_BUILD_FOR_MACOS)
    #define DUI_APP_ENTRY_ARGS_IMPL(AppClass) \
        int main(int argc, char** argv) \
        { \
            /*macOS-specific initialization (e.g., activate multithreaded GCD)*/ \
            CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, false); \
            AppClass::Instance().Run(argc, argv); \
            return 0; \
        }
#else
    #error "Unknown Platform!"
#endif

#define DUI_APP_ENTRY(AppClass)         DUI_APP_ENTRY_IMPL(AppClass)
#define DUI_APP_ENTRY_ARGS(AppClass)    DUI_APP_ENTRY_ARGS_IMPL(AppClass)

#endif //DUI_APP_ENTRY_H_
