#ifndef DUILIB_CONFIG_WINDOWS_H_
#define DUILIB_CONFIG_WINDOWS_H_

/** Windows SDK implementation (internally uses Windows API to implement some functionality)
*   Minimum supported system platform: Win7
*/

//When using the dynamic library, the DUILIB_DLL macro must be defined in the application VS project
#if defined(DUILIB_DLL)
    #if defined(DUILIB_EXPORTS)
        #define DUILIB_API __declspec(dllexport)
    #else
        #define DUILIB_API __declspec(dllimport)
    #endif
#else
    #define DUILIB_API
#endif

#ifndef VC_EXTRALEAN
    #define VC_EXTRALEAN
#endif

#ifndef WIN32_LEAN_AND_MEAN            // remove rarely used header files, including 'winsock.h'
    #define WIN32_LEAN_AND_MEAN            // which will conflict with 'winsock2.h'
#endif

#ifndef WINVER
    #define WINVER _WIN32_WINNT_WIN7
#endif

#ifndef _WIN32_WINNT
    #define _WIN32_WINNT _WIN32_WINNT_WIN7
#endif

#ifndef _WIN32_WINDOWS
    #define _WIN32_WINDOWS _WIN32_WINNT_WIN7
#endif

#ifndef _WIN32_IE
    #define _WIN32_IE _WIN32_IE_WIN7
#endif

#ifndef NOMINMAX
    #define NOMINMAX 1
#endif

// The minimum version of the RichEdit control
#define _RICHEDIT_VER 0x0500

// Including SDKDDKVer.h will define the highest available Windows platform version.

// If you are building an application for earlier Windows platforms, include WinSDKVer.h and
// Set the _WIN32_WINNT macro to the platform to support, then include SDKDDKVer.h.
#include <SDKDDKVer.h>

// Necessary Windows platform headers
#include <windows.h>
#include <crtdbg.h>
#include <tchar.h>

///////////////////////////////////////////////////////
// windowsx.h
#define GET_X_LPARAM(lp)    ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)    ((int)(short)HIWORD(lp))

/* Value for rolling one detent */
#define GET_WHEEL_DELTA_WPARAM(wParam)  ((short)HIWORD(wParam))
#define GET_KEYSTATE_WPARAM(wParam)     (LOWORD(wParam))

#endif //DUILIB_CONFIG_WINDOWS_H_
