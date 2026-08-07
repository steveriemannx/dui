#ifndef UI_CORE_SDL_MACOS_H_
#define UI_CORE_SDL_MACOS_H_

#if defined(__APPLE__) && defined(__MACH__)

#include "dui/Core/INativeWindow.h"

struct SDL_Window;

namespace ui
{
/** On macOS, get the NSView* interface corresponding to the SDL window
*/
void* GetSDLWindowContentView(SDL_Window* sdlWindow);

// Wrapper for the macOS SetFocus function, functionally similar to Windows' SetFocus(HWND)
// Parameters: window - pointer to the window to set focus to
// Return value: returns YES on success, NO on failure
bool SetFocus_MacOS(void* pNSWindow);

/** Enable/disable the OS-provided window shadow (and matching rounded corners)
 *  by converting the borderless SDL window into a titled document window with
 *  the title bar hidden (FullSizeContentView + titlebarAppearsTransparent).
 *  Returns true on success.
 */
bool ModifyNsWindowShadowType(void* pNSWindow, NativeWindowShadowType nativeShadowType);

/** After exiting fullscreen the WindowServer re-composites the window; toggle
 *  the style mask to force the rounded corners back (macOS quirk).
 */
void RestoreWindowShadowAfterFullscreen(void* pNSWindow, NativeWindowShadowType nativeShadowType);

}

#endif

#endif //UI_CORE_SDL_MACOS_H_
