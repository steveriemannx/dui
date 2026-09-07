#include "dui/Core/WindowDropTarget_MacOS.h"
#include "dui/Core/ControlDropTarget.h"

#if defined (DUI_BUILD_FOR_MACOS)

namespace ui 
{

WindowDropTarget::WindowDropTarget(NativeWindow* pNativeWindow):
    m_pHoverDropTarget(nullptr),
    m_pNativeWindow(pNativeWindow)
{
}

WindowDropTarget::~WindowDropTarget()
{
}

void WindowDropTarget::OnDropBegin()
{
    // TODO: macOS native drag and drop is not implemented yet
}

void WindowDropTarget::OnDropPosition(const UiPoint& /*pt*/)
{
    // TODO: macOS native drag and drop is not implemented yet
}

void WindowDropTarget::OnDropText(const DStringA& /*utf8Text*/)
{
    // TODO: macOS native drag and drop is not implemented yet
}

void WindowDropTarget::OnDropFile(const DStringA& /*utf8Source*/, const DStringA& /*utf8File*/)
{
    // TODO: macOS native drag and drop is not implemented yet
}

void WindowDropTarget::OnDropComplete()
{
    // TODO: macOS native drag and drop is not implemented yet
}

ControlPtrT<ControlDropTarget_Wayland> WindowDropTarget::GetControlDropTarget(const UiPoint& /*clientPt*/) const
{
    return ControlPtrT<ControlDropTarget_Wayland>();
}

void WindowDropTarget::ClearDropStatus()
{
    m_dropPt.SetXY(0, 0);
    m_textList.clear();
    m_fileList.clear();
    m_fileSource.clear();

    if (m_pHoverDropTarget != nullptr) {
        m_pHoverDropTarget->OnDropLeave();
        m_pHoverDropTarget = nullptr;
    }
}

} // namespace ui

#endif //defined (DUI_BUILD_FOR_MACOS)
