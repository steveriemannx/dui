#ifndef UI_CORE_SDL_MACOS_H_
#define UI_CORE_SDL_MACOS_H_

#if defined(__APPLE__) && defined(__MACH__)

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

}

#endif

#endif //UI_CORE_SDL_MACOS_H_
