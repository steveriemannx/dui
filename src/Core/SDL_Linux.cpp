#include "dui/Core/SDL_Linux.h"

#if (defined (DUI_BUILD_FOR_LINUX) || defined (DUI_BUILD_FOR_FREEBSD)) && !defined(DUI_BUILD_FOR_WAYLAND)

#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace ui
{
/** Wrapper for the Linux SetFocus function, functionally similar to Windows' SetFocus(HWND)
*/
bool SetFocus_Linux(uint64_t x11WindowNumber)
{
    // Get the display connection
    Display* display = XOpenDisplay(NULL);
    if (!display) {
        return false; // Failed to open the display
    }
    // RAII resource management
    struct DisplayCloser {
        Display* d;
        ~DisplayCloser() { if (d) ::XCloseDisplay(d); }
    } closer{ display };

    // Get the current window handle
    ::Window hWnd = x11WindowNumber;
    if (hWnd == None) {
        return false;
    }

    // Get the current focus window
    ::Window focusWindow = None;
    int revertTo = 0;
    XGetInputFocus(display, &focusWindow, &revertTo);

    // If the current focus is not on the target window, set the focus
    if (focusWindow != hWnd) {
        // Set the focus to the target window; the last parameter is the timestamp, CurrentTime means immediately
        XSetInputFocus(display, hWnd, RevertToParent, CurrentTime);
        // Flush the display to ensure the operation takes effect
        XFlush(display);
    }
    return true;
}

}

#endif //DUI_BUILD_FOR_LINUX
