#ifndef UI_CORE_DRAG_WINDOW_FILTER_WAYLAND_H_
#define UI_CORE_DRAG_WINDOW_FILTER_WAYLAND_H_

#include "dui/Core/Window.h"

#ifdef DUI_BUILD_FOR_WAYLAND

namespace ui
{
/** Message filter owned by a Wayland drag window.
 * Wayland delivers pointer events through the compositor, so there is no
 * native event to forward from the temporary drag window here.
 */
class DUI_API DragWindowFilter : public IUIMessageFilter
{
public:
    DragWindowFilter(Window* /*pOwner*/, Window* /*pWindow*/) {}

    LRESULT FilterMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, bool& bHandled) override
    {
        bHandled = false;
        return 0;
    }
};
}

#endif // DUI_BUILD_FOR_WAYLAND

#endif // UI_CORE_DRAG_WINDOW_FILTER_WAYLAND_H_
