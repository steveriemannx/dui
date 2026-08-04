#include "dui/Core/DpiAwareness.h"

#ifdef DUI_BUILD_FOR_WAYLAND

namespace ui
{
DpiAwareness::DpiAwareness():
    m_dpiAwarenessMode(DpiAwarenessMode::kPerMonitorDpiAware_V2)
{
}

DpiAwareness::~DpiAwareness()
{
}

bool DpiAwareness::InitDpiAwareness(DpiAwarenessMode dpiAwarenessMode)
{
    switch (dpiAwarenessMode) {
    case DpiAwarenessMode::kFromManifest:
    case DpiAwarenessMode::kPerMonitorDpiAware:
    case DpiAwarenessMode::kPerMonitorDpiAware_V2:
        m_dpiAwarenessMode = DpiAwarenessMode::kPerMonitorDpiAware_V2;
        break;
    default:
        m_dpiAwarenessMode = DpiAwarenessMode::kDpiUnaware;
    }
    return true;
}

DpiAwarenessMode DpiAwareness::SetDpiAwareness(DpiAwarenessMode /*dpiAwarenessMode*/)
{
    return m_dpiAwarenessMode;
}

DpiAwarenessMode DpiAwareness::GetDpiAwareness() const
{
    return m_dpiAwarenessMode;
}

} // namespace ui

#endif // DUI_BUILD_FOR_WAYLAND
