#include "dui/Core/DragWindowFilter_MacOS.h"
#include "dui/Core/Control.h"

#if defined(DUI_BUILD_FOR_MACOS)

namespace ui
{
DragWindowFilter::DragWindowFilter(Window* pOwner, Window* pWindow) :
    m_pOwner(pOwner),
    m_pWindow(pWindow)
{
}

LRESULT DragWindowFilter::FilterMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, bool& /*bHandled*/)
{
    // TODO: macOS native drag window message forwarding is not implemented yet.
    // The SDL implementation translates mouse events and pushes them onto the SDL
    // event queue; on macOS the events arrive through the AppKit event loop
    // (MessageLoop_MacOS), so forwarding is handled differently.
    return 0;
}

}

#endif //defined (DUI_BUILD_FOR_MACOS)
