#include "duilib/Utils/ScreenCapture.h"
#include "duilib/Utils/ScreenCapture_Wayland.h"
#include "duilib/Core/GlobalManager.h"

#if !defined(DUILIB_BUILD_FOR_WAYLAND)
#include "duilib/Utils/ScreenCapture_X11.h"
#endif

#if defined (DUILIB_BUILD_FOR_LINUX) || defined (DUILIB_BUILD_FOR_FREEBSD)
//Linux/FreeBSD OS

namespace ui
{
std::shared_ptr<IBitmap> ScreenCapture::CaptureBitmap(const ui::Window* pWindow)
{
#if defined(DUILIB_BUILD_FOR_WAYLAND)
    // Wayland-only build
    return ScreenCapture_Wayland::CaptureBitmap(pWindow);
#else
    if (ScreenCapture_Wayland::IsWaylandEnvironment()) {
        // Wayland environment
        return ScreenCapture_Wayland::CaptureBitmap(pWindow);
    }
    else {
        // X11 environment
        return ScreenCapture_X11::CaptureBitmap(pWindow);
    }
#endif
}

} // namespace ui

#endif //defined (DUILIB_BUILD_FOR_LINUX) || defined (DUILIB_BUILD_FOR_FREEBSD)
