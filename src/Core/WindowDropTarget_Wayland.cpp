#include "duilib/Core/WindowDropTarget_Wayland.h"

#ifdef DUILIB_BUILD_FOR_WAYLAND

namespace ui {

WindowDropTarget::WindowDropTarget(NativeWindow_SDL* pWindow):
    m_pWindow(pWindow)
{
}

WindowDropTarget::~WindowDropTarget()
{
}

void WindowDropTarget::OnDropBegin(void* data)
{
    (void)data;
}

void WindowDropTarget::OnDropPosition(int32_t x, int32_t y)
{
    (void)x; (void)y;
}

void WindowDropTarget::OnDropText(const std::string& text)
{
    (void)text;
}

void WindowDropTarget::OnDropFile(const std::string& filePath)
{
    (void)filePath;
}

void WindowDropTarget::OnDropComplete()
{
}

} // namespace ui

#endif // DUILIB_BUILD_FOR_WAYLAND
