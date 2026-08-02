#ifndef DUILIB_APP_ENTRY_H_
#define DUILIB_APP_ENTRY_H_

#include "duilib/duilib_config.h"

/** @file AppEntry.h
*   Unified application entry point macros (Qt style: one main.cpp per executable).
*
*   Usage:
*   1. Include this header in the .cpp file that defines the entry point.
*   2. Include the header of the application class (AppClass) BEFORE invoking the macro.
*   3. Invoke the macro EXACTLY ONCE in EXACTLY ONE .cpp file of the executable,
*      at global scope (no trailing semicolon needed):
*         DUILIB_APP_ENTRY(MyApp)       // MyApp must provide: void Run();
*         DUILIB_APP_ENTRY_ARGS(MyApp)  // MyApp must provide:
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
*     -mwindows). wWinMain has no argc/argv, so DUILIB_APP_ENTRY_ARGS always
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
// DUILIB_APP_ENTRY(AppClass)
// AppClass must provide: void Run();
//============================================================
#if defined(DUILIB_BUILD_FOR_WIN)
    #if defined(DUILIB_COMPILER_MINGW)
        //MinGW-w64 build
        #define DUILIB_APP_ENTRY_IMPL(AppClass) \
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
        #define DUILIB_APP_ENTRY_IMPL(AppClass) \
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
#elif defined(DUILIB_BUILD_FOR_LINUX) || defined(DUILIB_BUILD_FOR_FREEBSD)
    #define DUILIB_APP_ENTRY_IMPL(AppClass) \
        int main(int argc, char** argv) \
        { \
            AppClass app; \
            app.Run(); \
            return 0; \
        }
#elif defined(DUILIB_BUILD_FOR_MACOS)
    #define DUILIB_APP_ENTRY_IMPL(AppClass) \
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
// DUILIB_APP_ENTRY_ARGS(AppClass)
// AppClass must provide:
//   static AppClass& Instance();
//   int Run(int argc, char** argv);
//============================================================
#if defined(DUILIB_BUILD_FOR_WIN)
    #if defined(DUILIB_COMPILER_MINGW)
        #define DUILIB_APP_ENTRY_ARGS_IMPL(AppClass) \
            int APIENTRY WinMain(_In_ HINSTANCE /*hInstance*/, \
                                 _In_opt_ HINSTANCE /*hPrevInstance*/, \
                                 _In_ LPSTR /*lpCmdLine*/, \
                                 _In_ int /*nCmdShow*/) \
            { \
                AppClass::Instance().Run(0, nullptr); \
                return 0; \
            }
    #else
        #define DUILIB_APP_ENTRY_ARGS_IMPL(AppClass) \
            int APIENTRY wWinMain(_In_ HINSTANCE /*hInstance*/, \
                                  _In_opt_ HINSTANCE /*hPrevInstance*/, \
                                  _In_ LPWSTR /*lpCmdLine*/, \
                                  _In_ int /*nCmdShow*/) \
            { \
                AppClass::Instance().Run(0, nullptr); \
                return 0; \
            }
    #endif
#elif defined(DUILIB_BUILD_FOR_LINUX) || defined(DUILIB_BUILD_FOR_FREEBSD)
    #define DUILIB_APP_ENTRY_ARGS_IMPL(AppClass) \
        int main(int argc, char** argv) \
        { \
            AppClass::Instance().Run(argc, argv); \
            return 0; \
        }
#elif defined(DUILIB_BUILD_FOR_MACOS)
    #define DUILIB_APP_ENTRY_ARGS_IMPL(AppClass) \
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

#define DUILIB_APP_ENTRY(AppClass)         DUILIB_APP_ENTRY_IMPL(AppClass)
#define DUILIB_APP_ENTRY_ARGS(AppClass)    DUILIB_APP_ENTRY_ARGS_IMPL(AppClass)

#endif //DUILIB_APP_ENTRY_H_
