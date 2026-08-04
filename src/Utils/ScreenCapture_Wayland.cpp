#include "dui/Utils/ScreenCapture_Wayland.h"
#include "dui/Core/GlobalManager.h"

#if defined (DUI_BUILD_FOR_LINUX) || defined (DUI_BUILD_FOR_FREEBSD)
//Linux/FreeBSD OS

#include <cstdlib> // For getenv

namespace ui
{
bool ScreenCapture_Wayland::IsWaylandEnvironment()
{
    // The following environment variables are usually set in a Wayland environment
    const char* waylandDisplay = getenv("WAYLAND_DISPLAY");
    const char* xdgSessionType = getenv("XDG_SESSION_TYPE");
    
    return (waylandDisplay != nullptr && *waylandDisplay != '\0') ||
           (xdgSessionType != nullptr && strcmp(xdgSessionType, "wayland") == 0);
}

std::shared_ptr<IBitmap> ScreenCapture_Wayland::CaptureBitmap(const ui::Window* pWindow)
{
    if (pWindow == nullptr) {
        return nullptr;
    }

    // Get the native window pointer
    const NativeWindow* pNativeWnd = pWindow->NativeWnd();
    if (pNativeWnd == nullptr) {
        return nullptr;
    }
    
    // Detect the Wayland environment early and return failure directly
    if (!IsWaylandEnvironment()) {
        return nullptr;
    }
    
    return nullptr;
}

} // namespace ui

#endif //defined (DUI_BUILD_FOR_LINUX) || defined (DUI_BUILD_FOR_FREEBSD)
