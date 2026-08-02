#ifndef DUILIB_CONFIG_H_
#define DUILIB_CONFIG_H_

/** Platform detection: Windows, Linux, macOS, FreeBSD
*/
#if defined (_WIN32) || defined (_WIN64)
    //Windows platform
    #define DUILIB_BUILD_FOR_WIN    1
#elif defined(linux) || defined(__linux) || defined(__linux__)
    //Linux platform
    #define DUILIB_BUILD_FOR_LINUX  1
#elif defined(__APPLE__) && defined(__MACH__)
    //macOS platform
    #include <TargetConditionals.h>
    #if TARGET_OS_MAC
        #define DUILIB_BUILD_FOR_MACOS  1
    #else
        #error "Unknown Platform!"
    #endif
#elif defined(__FreeBSD__)
    //FreeBSD platform
    #define DUILIB_BUILD_FOR_FREEBSD    1
#else
    //Unsupported system
    #error "Unknown Platform!"
#endif


//Macro definitions for different platforms
#if defined DUILIB_BUILD_FOR_WIN
    //Whether to use SDL window and mouse/keyboard events (currently only SDL3 is supported)
    #if (DUILIB_SDL)
        //To enable SDL, change SDLEnabled to 1 in msvc\PropertySheets\SDLSettings.props
        #define DUILIB_BUILD_FOR_SDL    1
    #endif

    //WebView2 control feature switch
    #if (DUILIB_WEBVIEW2)
        //To enable WebView2, change WebView2Enabled to 1 in msvc\PropertySheets\WebView2Settings.props
        #define DUILIB_BUILD_FOR_WEBVIEW2   1
    #endif

    //CEF control feature switch
    #ifndef DUILIB_CEF
        //Enabled by default
        #define DUILIB_BUILD_FOR_CEF        1
    #else
        #if (DUILIB_CEF)
            //To enable CEF, change LibCefEnabled to 1 in msvc\PropertySheets\CEFSettings.props
            #define DUILIB_BUILD_FOR_CEF    1
        #endif
    #endif

    /** Whether the RichEdit control drawing optimization option is enabled (Windows version of RichEdit)
    */
    #define DUILIB_RICH_EDIT_DRAW_OPT 1

    /** Whether the libjpeg-turbo library is used to decode JPEG format
    */
    #if (DUILIB_JPEG_TURBO)
        #define DUILIB_IMAGE_SUPPORT_JPEG_TURBO 1
    #endif

    /** Whether the libpag library is used to decode PAG format
    */
    #if (DUILIB_LIB_PAG)
        #define DUILIB_IMAGE_SUPPORT_LIB_PAG 1
    #endif
#else
    // Non-Windows platforms
    #if (DUILIB_WAYLAND)
        //Use Wayland + wlroots window and mouse/keyboard events (replacing SDL)
        #define DUILIB_BUILD_FOR_WAYLAND 1
    #else
        //Whether to use SDL window and mouse/keyboard events (currently only SDL3 is supported)
        #define DUILIB_BUILD_FOR_SDL    1
    #endif
    //Defines whether CEF is supported
    #define DUILIB_BUILD_FOR_CEF    1
#endif

#if defined (__MINGW32__) || defined (__MINGW64__)
    //MinGW-w64 compiler
    #define DUILIB_COMPILER_MINGW 1
#endif

/** 64-bit operating system flag
*/
#if defined(_M_X64) || defined(_M_AMD64) || defined(_WIN64) || defined(__x86_64__) 
    #define DUILIB_BIT_64   1
#endif

/** Unicode or Ansi version (for the Ansi version, the file encoding is UTF-8, so the string encoding is also UTF-8)
*/
#if defined(UNICODE) || defined(_UNICODE)
    #define DUILIB_UNICODE 1
#endif

//Unused variable macro, to avoid compiler warnings
#ifndef UNUSED_VARIABLE
    #define UNUSED_VARIABLE(x) ((void)(x))
#endif

#ifndef ASSERT_UNUSED_VARIABLE
    #ifdef _DEBUG
        #define ASSERT_UNUSED_VARIABLE(expr)  ASSERT(expr)
    #else
        #define ASSERT_UNUSED_VARIABLE(expr)  UNUSED_VARIABLE(expr)
    #endif
#endif

#if defined DUILIB_BUILD_FOR_WIN
    #include "duilib_config_windows.h"

    #ifndef ASSERT
        #define ASSERT(expr)  _ASSERTE(expr)
    #endif

#elif defined DUILIB_BUILD_FOR_LINUX
    #include "duilib_config_linux.h"
    #include <cassert>

    #ifdef _DEBUG
        #define ASSERT(expr)  assert(expr)
    #else
        #define ASSERT(expr)  ((void)(0))
    #endif

#elif defined DUILIB_BUILD_FOR_MACOS
    #include "duilib_config_macos.h"
    #include <cassert>

    #ifdef _DEBUG
        #define ASSERT(expr)  assert(expr)
    #else
        #define ASSERT(expr)  ((void)(0))
    #endif

#elif defined DUILIB_BUILD_FOR_FREEBSD
    #include "duilib_config_freebsd.h"
    #include <cassert>

    #ifdef _DEBUG
        #define ASSERT(expr)  assert(expr)
    #else
        #define ASSERT(expr)  ((void)(0))
    #endif
#else
    #error "Unknown Platform!"
#endif

//String class definition
#include "duilib_string.h"

#endif //DUILIB_CONFIG_H_

