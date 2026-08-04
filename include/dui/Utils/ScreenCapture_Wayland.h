#ifndef UI_UTILS_SCREEN_CAPTURE_WAYLAND_H_
#define UI_UTILS_SCREEN_CAPTURE_WAYLAND_H_

#include "dui/Render/IRender.h"
#include "dui/Core/Window.h"
#include <memory>

namespace ui
{
/** Screen capture (Linux Wayland implementation)
*/
class DUI_API ScreenCapture_Wayland
{
public:
    /** Capture a screenshot of the screen where the window is located
    * @param [in] pWindow The window
    */
    static std::shared_ptr<IBitmap> CaptureBitmap(const Window* pWindow);
        
    /** Check whether the current environment is a Wayland desktop environment
    */
    static bool IsWaylandEnvironment();
};

} // namespace ui

#endif // UI_UTILS_SCREEN_CAPTURE_WAYLAND_H_
