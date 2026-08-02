#ifndef UI_CORE_WINDOW_DROP_TARGET_WAYLAND_H_
#define UI_CORE_WINDOW_DROP_TARGET_WAYLAND_H_

#include "duilib/duilib_defs.h"

#ifdef DUILIB_BUILD_FOR_WAYLAND

namespace ui {

class NativeWindow_SDL;
class Control;
class ControlDropTarget;

class WindowDropTarget
{
public:
    explicit WindowDropTarget(NativeWindow_SDL* pWindow);
    ~WindowDropTarget();

    void OnDropBegin(void* data);
    void OnDropPosition(int32_t x, int32_t y);
    void OnDropText(const std::string& text);
    void OnDropFile(const std::string& filePath);
    void OnDropComplete();

private:
    NativeWindow_SDL* m_pWindow;
};

} // namespace ui

#endif // DUILIB_BUILD_FOR_WAYLAND

#endif // UI_CORE_WINDOW_DROP_TARGET_WAYLAND_H_
