#ifndef UI_UTILS_SCREEN_CAPTURE_H_
#define UI_UTILS_SCREEN_CAPTURE_H_

#include "duilib/Render/IRender.h"
#include "duilib/Core/Window.h"
#include <memory>

namespace ui
{
/** Screen capture
*/
class DUILIB_API ScreenCapture
{
public:
    /** Capture a screenshot of the screen where the window is located
    * @param [in] pWindow The window
    */
    static std::shared_ptr<IBitmap> CaptureBitmap(const Window* pWindow);
};

} // namespace ui

#endif // UI_UTILS_SCREEN_CAPTURE_H_
