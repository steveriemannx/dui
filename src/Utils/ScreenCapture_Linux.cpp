#include "dui/Utils/ScreenCapture.h"
#include "dui/Utils/ScreenCapture_Wayland.h"
#include "dui/Core/GlobalManager.h"

#if !defined(DUI_BUILD_FOR_WAYLAND)
#include "dui/Utils/ScreenCapture_X11.h"
#endif

#if defined (DUI_BUILD_FOR_LINUX) || defined (DUI_BUILD_FOR_FREEBSD)
//Linux/FreeBSD OS

namespace ui
{
std::shared_ptr<IBitmap> ScreenCapture::CaptureBitmap(const ui::Window* pWindow)
{
    //Which backend to use is decided at build time, not at run time: the CMake
    //configuration picks one of the two source sets, so the other is not even
    //linked in. This used to fall off the end without returning when built for
    //Wayland, which is undefined behaviour.
#if defined(DUI_BUILD_FOR_WAYLAND)
    return ScreenCapture_Wayland::CaptureBitmap(pWindow);
#else
    return ScreenCapture_X11::CaptureBitmap(pWindow);
#endif
}

} // namespace ui

#endif //defined (DUI_BUILD_FOR_LINUX) || defined (DUI_BUILD_FOR_FREEBSD)
