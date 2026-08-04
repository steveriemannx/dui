#ifndef UI_CORE_CONTROL_DROP_TARGET_IMPL_WAYLAND_H_
#define UI_CORE_CONTROL_DROP_TARGET_IMPL_WAYLAND_H_

#include "dui/dui_defs.h"

#ifdef DUI_BUILD_FOR_WAYLAND

namespace ui {

class ControlDropTargetImpl_Wayland
{
public:
    ControlDropTargetImpl_Wayland() = default;
    ~ControlDropTargetImpl_Wayland() = default;
};

} // namespace ui

#endif // DUI_BUILD_FOR_WAYLAND

#endif // UI_CORE_CONTROL_DROP_TARGET_IMPL_WAYLAND_H_
