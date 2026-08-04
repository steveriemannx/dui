#ifndef UI_CORE_CLICK_THROUGH_H_
#define UI_CORE_CLICK_THROUGH_H_

#include "dui/Core/UiPoint.h"

namespace ui
{
class Window;

/** Implementation wrapper of the window click-through feature
*/
class DUI_API ClickThrough
{
public:
    ClickThrough();
    ~ClickThrough();

public:
    /** Perform the click-through operation at the mouse position, activating the window behind the current window at that position
    * @param [in] pWindow The current window
    * @param [in] ptMouse The mouse click point (screen coordinates)
    */
    bool ClickThroughWindow(Window* pWindow, const UiPoint& ptMouse);
};

}//namespace ui

#endif //UI_CORE_CLICK_THROUGH_H_
