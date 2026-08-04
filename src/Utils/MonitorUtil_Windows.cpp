#include "dui/Utils/MonitorUtil.h"
#include "dui/Core/WindowBase.h"
#include "dui/Core/DpiManager.h"

#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
#include "dui/Utils/ApiWrapper_Windows.h"
#include "dui/dui_config_windows.h"
#include <VersionHelpers.h>

namespace ui
{
float MonitorUtil::GetWindowDisplayScale(const WindowBase* pWindowBase, float& fWindowPixelDensity)
{
    fWindowPixelDensity = 1.0f;
    // Read the DPI value of the window
    uint32_t uDPI = 0;
    HWND hWnd = nullptr;
    if ((pWindowBase != nullptr) && pWindowBase->IsWindow()) {
        hWnd = pWindowBase->NativeWnd()->GetHWND();
    }    
    if ((uDPI == 0) && (hWnd != nullptr) && ::IsWindows8OrGreater()) {
        HMONITOR hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        if (hMonitor != nullptr) {
            uint32_t dpix = 96;
            uint32_t dpiy = 96;
            if (GetDpiForMonitorWrapper(hMonitor, MDT_EFFECTIVE_DPI, &dpix, &dpiy)) {
                uDPI = dpix;
            }
        }
    }
    if ((uDPI == 0) && (hWnd != nullptr)) {
        if (::IsWindows10OrGreater()) {
            if (!GetDpiForWindowWrapper(hWnd, uDPI)) {
                uDPI = 0;
            }
        }
    }
    if ((uDPI == 0) && (hWnd != nullptr)) {
        HDC hDC = ::GetDC(hWnd);
        if (hDC != nullptr) {
            uDPI = (uint32_t)::GetDeviceCaps(hDC, LOGPIXELSX);
            ::ReleaseDC(hWnd, hDC);
        }
    }
    if (uDPI != 0) {
        // Convert DPI to a display scale
        return (float)DpiManager::MulDiv(uDPI, 100u, 96u) / 100.0f;
    }
    return GetPrimaryMonitorDisplayScale();
}

float MonitorUtil::GetPrimaryMonitorDisplayScale()
{
    bool bOk = false;
    uint32_t uDPI = 96;
    // Prefer getting it from the desktop window
    HWND hDesktopWnd = ::GetDesktopWindow();
    if (!bOk && (hDesktopWnd != nullptr) && ::IsWindows8OrGreater()) {
        HMONITOR hMonitor = ::MonitorFromWindow(hDesktopWnd, MONITOR_DEFAULTTOPRIMARY);
        if (hMonitor != nullptr) {
            uint32_t dpix = 96;
            uint32_t dpiy = 96;
            if (GetDpiForMonitorWrapper(hMonitor, MDT_EFFECTIVE_DPI, &dpix, &dpiy)) {
                uDPI = dpix;
                bOk = true;
            }
        }
    }

    if (!bOk && ::IsWindows10OrGreater()) {
        if (GetDpiForSystemWrapper(uDPI)) {// After the process starts, if the system DPI is changed, this API still returns the old value
            bOk = true;
        }
    }
    if (!bOk && ::IsWindows8OrGreater()) {
        POINT pt = { 1, 1 };
        HMONITOR hMonitor = ::MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
        if (hMonitor != nullptr) {
            uint32_t dpix = 96;
            uint32_t dpiy = 96;
            if (GetDpiForMonitorWrapper(hMonitor, MDT_EFFECTIVE_DPI, &dpix, &dpiy)) {
                uDPI = dpix;
                bOk = true;
            }
        }
    }
    if (!bOk) {
        HDC desktopDc = ::GetDC(nullptr);
        uDPI = (uint32_t)::GetDeviceCaps(desktopDc, LOGPIXELSX);
        ::ReleaseDC(nullptr, desktopDc);
    }
    if (uDPI == 0) {
        uDPI = 96;
    }
    // Convert DPI to a display scale
    return (float)DpiManager::MulDiv(uDPI, 100u, 96u) / 100.0f;
}

} // namespace ui

#endif //DUI_BUILD_FOR_WIN
