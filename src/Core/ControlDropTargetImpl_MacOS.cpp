#include "dui/Core/ControlDropTargetImpl_MacOS.h"

#if defined(DUI_BUILD_FOR_MACOS)

#include "dui/Core/Control.h"

namespace ui 
{
ControlDropTargetImpl_MacOS::ControlDropTargetImpl_MacOS(Control* pControl) :
    m_pControl(pControl)
{
}

ControlDropTargetImpl_MacOS::~ControlDropTargetImpl_MacOS()
{
}

int32_t ControlDropTargetImpl_MacOS::OnDropBegin(const UiPoint& /*pt*/)
{
    // TODO: macOS native drag and drop is not implemented yet
    return 0;
}

void ControlDropTargetImpl_MacOS::OnDropPosition(const UiPoint& /*pt*/)
{
    // TODO: macOS native drag and drop is not implemented yet
}

void ControlDropTargetImpl_MacOS::OnDropTexts(const std::vector<DString>& /*textList*/, const UiPoint& /*pt*/)
{
    // TODO: macOS native drag and drop is not implemented yet
}

void ControlDropTargetImpl_MacOS::OnDropFiles(const DString& /*source*/, const std::vector<DString>& /*fileList*/, const UiPoint& /*pt*/)
{
    // TODO: macOS native drag and drop is not implemented yet
}

void ControlDropTargetImpl_MacOS::OnDropLeave()
{
    // TODO: macOS native drag and drop is not implemented yet
}

} // namespace ui

#endif //defined (DUI_BUILD_FOR_MACOS)
