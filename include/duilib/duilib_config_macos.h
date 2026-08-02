#ifndef DUILIB_CONFIG_MACOS_H_
#define DUILIB_CONFIG_MACOS_H_

#include <cstddef>
#include <cstdint>
#include <climits>
#include <TargetConditionals.h>

// macOS platform export macro definitions
#define DUILIB_API

// Basic type definitions (kept consistent with Linux)
typedef unsigned int        UINT;
typedef unsigned long long  WPARAM;
typedef long long           LPARAM;
typedef long long           LRESULT;

// Byte manipulation macros (compatible with Windows/Linux)
#define MAKEWORD(a, b)      ((uint16_t)(((uint8_t)(((size_t)(a)) & 0xff)) | ((uint16_t)((uint8_t)(((size_t)(b)) & 0xff))) << 8))
#define MAKELONG(a, b)      ((uint32_t)(((uint16_t)(((size_t)(a)) & 0xffff)) | ((uint32_t)((uint16_t)(((size_t)(b)) & 0xffff))) << 16))
#define LOWORD(l)           ((uint16_t)(((size_t)(l)) & 0xffff))
#define HIWORD(l)           ((uint16_t)((((size_t)(l)) >> 16) & 0xffff))
#define LOBYTE(w)           ((uint8_t)(((size_t)(w)) & 0xff))
#define HIBYTE(w)           ((uint8_t)((((size_t)(w)) >> 8) & 0xff))

// Success status codes (compatible with Windows)
#define S_OK            ((long)0L)
#define S_FALSE         ((long)1L)

// Callback function modifiers
#define CALLBACK

// macOS-specific parameter parsing macros (mimicking Windows message parameter parsing)
#define GET_X_LPARAM(lp)    ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)    ((int)(short)HIWORD(lp))
#define GET_WHEEL_DELTA_WPARAM(wParam)  ((short)HIWORD(wParam))
#define GET_KEYSTATE_WPARAM(wParam)     (LOWORD(wParam))

// macOS system headers
#include <CoreFoundation/CoreFoundation.h>
#include <Carbon/Carbon.h>  // Used for keyboard event handling

// Resource path definitions (macOS uses the bundle structure)
#ifdef __OBJC__
    #import <Foundation/Foundation.h>
    #define DUILIB_RESOURCE_PATH [[NSBundle mainBundle] resourcePath].UTF8String
#else
    #define DUILIB_RESOURCE_PATH ""
#endif

#endif // DUILIB_CONFIG_MACOS_H_
