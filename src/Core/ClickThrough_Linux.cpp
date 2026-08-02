#include "duilib/Core/ClickThrough.h"
#include "duilib/Core/Window.h"

#if (defined (DUILIB_BUILD_FOR_LINUX) || defined (DUILIB_BUILD_FOR_FREEBSD)) && !defined(DUILIB_BUILD_FOR_WAYLAND)

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h> 
#include <cstring>
#include <unistd.h>
#include <iostream>

namespace ui
{
ClickThrough::ClickThrough()
{
}

ClickThrough::~ClickThrough()
{
}

// Get the process ID corresponding to the window
static pid_t GetWindowPID(Display* display, ::Window window)
{
    if (!display) {
        return 0;
    }

    // Get the _NET_WM_PID atom
    Atom netWmPid = XInternAtom(display, "_NET_WM_PID", False);
    if (netWmPid == None) {
        return -1;
    }
        
    // Query the window properties
    Atom actualType;
    int format;
    unsigned long numItems, bytesAfter;
    unsigned char* prop = nullptr;
    
    // Request the PID property
    Status status = XGetWindowProperty(
        display, window, netWmPid, 0, 1, False, XA_CARDINAL,
        &actualType, &format, &numItems, &bytesAfter, &prop
    );
    
    if (status != Success || prop == nullptr || numItems == 0) {
        if (prop) {
            XFree(prop);
        }
        return -1;
    }
    
    // Get the PID value
    pid_t pid = static_cast<pid_t>(*reinterpret_cast<unsigned long*>(prop));
    XFree(prop);
    
    return pid;
}

static std::string GetWindowTitle(Display* display, ::Window window) 
{
    if (!display) {
        return "";
    }
    
    std::string title;
    
    // Try to get the UTF-8 encoded window title (_NET_WM_NAME)
    Atom utf8Atom = XInternAtom(display, "_NET_WM_NAME", False);
    Atom actualType;
    int format;
    unsigned long numItems, bytesAfter;
    unsigned char* prop = nullptr;
    
    if (XGetWindowProperty(display, window, utf8Atom, 0, 1024, False, 
                          XA_STRING, &actualType, &format, &numItems, 
                          &bytesAfter, &prop) == Success && prop) {
        title = reinterpret_cast<char*>(prop);
        XFree(prop);
        prop = nullptr;
    }
    
    // If the UTF-8 title was not obtained, try to get the traditional WM_NAME
    if (title.empty()) {
        Atom wmNameAtom = XInternAtom(display, "WM_NAME", False);
        if (XGetWindowProperty(display, window, wmNameAtom, 0, 1024, False, 
                              AnyPropertyType, &actualType, &format, &numItems, 
                              &bytesAfter, &prop) == Success && prop) {
            // Check whether the type is a string or UTF8_STRING
            if (actualType == XA_STRING || actualType == utf8Atom) {
                title = reinterpret_cast<char*>(prop);
            }
            XFree(prop);
            prop = nullptr;
        }
    }

    if (title.empty()) {
        char *name = NULL;
        XFetchName(display, window, &name);  // Get the window title
        if (name != nullptr) {
            title = name;
        }        
        XFree(name);
    }
    return title;
}

// Recursively get the window and all its child windows
static void GetAllChildWindows(Display* display, ::Window window, std::vector<::Window>& windows) 
{
    if ((display == nullptr) || (window == None)) {
        return;
    }
    ::Window root = None;
    ::Window parent = None;
    ::Window* children = nullptr;
    unsigned int nchildren =0 ;    
    if (XQueryTree(display, window, &root, &parent, &children, &nchildren)) {
        for (unsigned int i = 0; i < nchildren; i++) {
            windows.push_back(children[i]);
            GetAllChildWindows(display, children[i], windows);
        }
        XFree(children);
    }
}

// Get all windows on the desktop, sorted by Z order
static void GetDisplayOrderedWindows(Display* display, std::vector<::Window>& windows)
{
    windows.clear();
    if (display == nullptr) {
        return;
    }
    ::Window root = DefaultRootWindow(display);
    
    // Get the Z-order window list (EWMH method)
    Atom stack_atom = XInternAtom(display, "_NET_CLIENT_LIST_STACKING", False);
    Atom type;
    int format = 0;
    unsigned long nitems = 0;
    unsigned long bytes_after = 0;
    ::Window* stack_windows = nullptr;    
    if(::XGetWindowProperty(display, root, stack_atom, 0, ~0L, False,
                            XA_WINDOW, &type, &format, &nitems, &bytes_after,
                            (unsigned char**)&stack_windows) == Success) {
        
        // The window Z order from top to bottom
        if (stack_windows != nullptr) {
            for(int i = (int)nitems-1; i >=0; i--) {
                windows.push_back(stack_windows[i]);
            }
            XFree(stack_windows);
        }
        if (!windows.empty()) {
            std::reverse(windows.begin(), windows.end());
        }
    }
    else {
        // Fall back to the XQueryTree method
        ::Window root_return = None;
        ::Window parent_return = None;
        ::Window* children = nullptr;
        unsigned int nchildren = 0;
        if (XQueryTree(display, root, &root_return, &parent_return, &children, &nchildren)) {
            // The window Z order from bottom to top
            if (children != nullptr) {
                for(int i = (int)nchildren-1; i >=0; i--) {
                    windows.push_back(children[i]);
                }
                XFree(children);
            }
        }
    }
}

static ::Window FindWindowBelow(Display* display, ::Window window, int rootX, int rootY) 
{
    // Get the current window and all its child windows
    std::vector<::Window> allWindows;
    allWindows.push_back(window);
    GetAllChildWindows(display, window, allWindows);

    // Get the child window list of the root window (already sorted by Z order)
    std::vector<::Window> allDisplayWindows;
    GetDisplayOrderedWindows(display, allDisplayWindows);

    //TEST
    if(0) {
        std::cout << "allDisplayWindows: " << allDisplayWindows.size() << std::endl;
        for (::Window windowNum : allDisplayWindows) {
            XWindowAttributes attrs;
            if (XGetWindowAttributes(display, windowNum, &attrs)) {
                std::string windowText = GetWindowTitle(display, windowNum);
                pid_t targetPid = GetWindowPID(display, windowNum);    
                std::cout << "Window ID: " << windowNum 
                            << " | Name: " << (!windowText.empty() ? windowText.c_str() : "N/A") 
                            << " | PID: " << targetPid ;

                std::cout << " | 坐标: (" << attrs.x << ", " << attrs.y << ")"
                        << " | 尺寸: " << attrs.width << "x" << attrs.height
                        << " | 可视状态: " << (attrs.map_state == IsUnmapped ? "未映射" : 
                                            attrs.map_state == IsUnviewable ? "不可见" : "可见")
                        << std::endl;
            }
        }
    }
    //TEST

    // Find the position of the target window in the Z order
    int targetIndex = -1;
    for (size_t i = 0; i < allDisplayWindows.size(); ++i) {
        for (const auto& w : allWindows) {
            if (allDisplayWindows[i] == w) {
                targetIndex = (int)i;
                break;
            }
        }
        if (targetIndex != -1) {
            break;
        }
    }

    ::Window result = None;
    if (targetIndex != -1) {
        // Check the windows below the target window in the Z order
        ::Window root = DefaultRootWindow(display);
        for (int i = targetIndex - 1; i >= 0; i--) {
            ::Window candidate = allDisplayWindows[i];
            XWindowAttributes attrs;
            if (XGetWindowAttributes(display, candidate, &attrs) && (attrs.map_state == IsViewable)) {                
                // Coordinate conversion and hit test
                int x = 0;
                int y = 0;
                ::Window dummy = None;
                if (XTranslateCoordinates(display, candidate, root, 0, 0, &x, &y, &dummy)) {                
                    if (rootX >= (x - attrs.border_width) && 
                        rootX < (x + attrs.width + attrs.border_width) &&
                        rootY >= (y - attrs.border_width) && 
                        rootY < (y + attrs.height + attrs.border_width)) {
                        result = candidate;
                        break;
                    }
                }
            }
        }
    }
    return result;
}

/** Activate the specified window (raise to front and get focus)
 * @param window  The target window ID
 * @param force   Whether to force activation (ignore the focus policy)
 * @return        1 on success, 0 on failure
 */
static int ActivateX11Window(::Window window, int force) 
{
    if (window == None) {
        return 0;
    }

    Display* display = ::XOpenDisplay(nullptr);
    if (!display) {
        return 0;
    }

    // RAII resource management
    struct DisplayCloser {
        Display* d;
        ~DisplayCloser() { if(d) ::XCloseDisplay(d); }
    } closer{display};

    // Get the current window properties
    ::XWindowAttributes currentAttrs;
    if (!::XGetWindowAttributes(display, window, &currentAttrs)) {
        return 0;
    }

    // Get the root window and the necessary atoms
    ::Window root = RootWindow(display, DefaultScreen(display));
    Atom wm_state = XInternAtom(display, "_NET_WM_STATE", False);
    Atom wm_active = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);
    Atom wm_protocols = XInternAtom(display, "WM_PROTOCOLS", False);
    Atom wm_take_focus = XInternAtom(display, "WM_TAKE_FOCUS", False);
    
    // Check whether the window manager supports the _NET_ACTIVE_WINDOW protocol
    Atom actual_type = None;
    int format = 0;
    unsigned long num_items = 0;
    unsigned long bytes_after = 0;
    unsigned char *prop = nullptr;
    int supports_netwm = 0;
    
    if (wm_active && XGetWindowProperty(display, root, 
        XInternAtom(display, "_NET_SUPPORTED", False), 0, 1024, False, 
                    XA_ATOM, &actual_type, &format, &num_items, &bytes_after, &prop) == Success) {
        
        Atom *supported = (Atom*)prop;
        for (unsigned long i = 0; i < num_items; i++) {
            if (supported[i] == wm_active) {
                supports_netwm = 1;
                break;
            }
        }
        if (prop) {
            XFree(prop);
            prop = nullptr;
        }
    }
    
    // 1. Use the _NET_ACTIVE_WINDOW protocol (preferred method)
    if (supports_netwm) {
        XClientMessageEvent ev;
        memset(&ev, 0, sizeof(ev));
        ev.type = ClientMessage;
        ev.window = root;
        ev.message_type = wm_active;
        ev.format = 32;
        ev.data.l[0] = 2;  // _NET_ACTIVE_WINDOW_REMOVE
        ev.data.l[1] = (long)window;
        ev.data.l[2] = CurrentTime;
        ev.data.l[3] = 0;  // source indication: application
        ev.data.l[4] = 0;
        
        XSendEvent(display, root, False, 
                  SubstructureRedirectMask | SubstructureNotifyMask,
                  (XEvent*)&ev);
    }
    
    // 2. Send the WM_TAKE_FOCUS protocol message
    if (wm_protocols && wm_take_focus) {
        XClientMessageEvent ev;
        memset(&ev, 0, sizeof(ev));
        ev.type = ClientMessage;
        ev.window = window;
        ev.message_type = wm_protocols;
        ev.format = 32;
        ev.data.l[0] = wm_take_focus;
        ev.data.l[1] = CurrentTime;
        
        XSendEvent(display, window, False, 0, (XEvent*)&ev);
    }
    
    // 3. Use the Xlib low-level functions to raise the window
    XMapRaised(display, window);
    XRaiseWindow(display, window);
    
    // 4. Set the input focus
    if (force) {
        XSetInputFocus(display, window, RevertToParent, CurrentTime);
    } else {
        XSetInputFocus(display, window, RevertToPointerRoot, CurrentTime);
    }
    
    // 5. Handle possible race conditions (retry)
    for (int i = 0; i < 3; i++) {
        XFlush(display);
        usleep(10000);  // Wait 10 milliseconds
        XMapRaised(display, window);
    }
    
    // Synchronize the requests to ensure all operations are completed
    XSync(display, False);
    
    // Verify whether the window is visible (simple check)
    XWindowAttributes attrs;
    if (XGetWindowAttributes(display, window, &attrs) && 
        (attrs.map_state == IsViewable || attrs.map_state == IsUnmapped)) {
        //std::cout << "ActivateX11Window: OK" << std::endl;
        return 1;
    }
    
    return 0;
}

bool ClickThrough::ClickThroughWindow(Window* pWindow, const UiPoint& ptMouse)
{
    // Parameter validation
    if (!pWindow || !pWindow->NativeWnd()) {
        return false;
    }

    const char* sessionType = std::getenv("XDG_SESSION_TYPE");
    // Only X11 desktop environments are supported
    if (sessionType == nullptr) {        
        return false;
    }
    if ((std::string(sessionType) != "x11") && (std::string(sessionType) != "X11")) {        
        return false;
    }

    Display* display = ::XOpenDisplay(nullptr);
    if (!display) {
        return false;
    }

    // RAII resource management
    struct DisplayCloser {
        Display* d;
        ~DisplayCloser() { if(d) ::XCloseDisplay(d); }
    } closer{display};

    // Get the current window properties
    ::Window x11Window = pWindow->NativeWnd()->GetX11WindowNumber();
    ::XWindowAttributes currentAttrs;
    if (!::XGetWindowAttributes(display, x11Window, &currentAttrs)) {
        return false;
    }

    // Temporarily disable the events of the current window
    ::XSetWindowAttributes newAttrs;
    newAttrs.event_mask = currentAttrs.all_event_masks & ~(ButtonPressMask|ButtonReleaseMask);
    ::XChangeWindowAttributes(display, x11Window, CWEventMask, &newAttrs);
    
    // Convert the mouse coordinates to root window coordinates
    int rootX = 0;
    int rootY = 0;
    ::Window root = DefaultRootWindow(display);
    UiPoint ptClient = ptMouse;
    pWindow->ScreenToClient(ptClient);
    ::Window dummy = None;
    XTranslateCoordinates(display, x11Window, root, ptClient.x, ptClient.y, &rootX, &rootY, &dummy);

    // Recursively find the visible window under the mouse position
    ::Window targetWindow = FindWindowBelow(display, x11Window, rootX, rootY);

    // Restore the window events
    ::XSetWindowAttributes restoreAttrs;
    restoreAttrs.event_mask = currentAttrs.all_event_masks;
    ::XChangeWindowAttributes(display, x11Window, CWEventMask, &restoreAttrs);

    // Activate the target window
    if ((targetWindow != None) && (targetWindow != x11Window)) {
        int nRet = ActivateX11Window(targetWindow, 1) ;
        return nRet != 0;
    }
    return false;
}

}//namespace ui

#endif //(DUILIB_BUILD_FOR_LINUX || DUILIB_BUILD_FOR_FREEBSD) && !DUILIB_BUILD_FOR_WAYLAND

// Wayland stubs (no X11 available)
#if defined(DUILIB_BUILD_FOR_WAYLAND)
namespace ui {

ClickThrough::ClickThrough() {}
ClickThrough::~ClickThrough() {}
bool ClickThrough::ClickThroughWindow(Window* pWindow, const UiPoint& ptMouse) {
    (void)pWindow; (void)ptMouse;
    return false;
}

} // namespace ui
#endif
