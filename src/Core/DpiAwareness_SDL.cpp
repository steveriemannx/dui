#include "dui/Core/DpiAwareness.h"

//Non-Windows platforms only; the various DpiAwareness modes are not supported, non-Windows platforms have no such concept
#if defined (DUI_BUILD_FOR_SDL) && !defined (DUI_BUILD_FOR_WIN)

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

}

#endif //DUI_BUILD_FOR_SDL
