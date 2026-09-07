#include "dui/Core/ClickThrough.h"
#include "dui/Core/Window.h"

namespace ui
{
ClickThrough::ClickThrough()
{
}

ClickThrough::~ClickThrough()
{
}

bool ClickThrough::ClickThroughWindow(Window* /*pWindow*/, const UiPoint& /*ptMouse*/)
{
    // SDL-only: click-through not supported in cross-platform SDL backend
    // Always return false (no window behind activation)
    return false;
}

} // namespace ui
