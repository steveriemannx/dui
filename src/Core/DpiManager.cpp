#include "dui/Core/DpiManager.h"
#include "dui/Utils/MonitorUtil.h"
#include "dui/Core/GlobalManager.h"
#include <cmath>

namespace ui
{
DpiManager::DpiManager():
    m_bDpiInited(false),
    m_bUserDefinedDpi(false),
    m_bEnablePixelDensity(true),
    m_dpiAwarenessMode(DpiAwarenessMode::kFromManifest),
    m_nScaleFactor(100),
    m_fPixelDensity(1.0f)
{
#ifdef DUI_BUILD_FOR_WIN
    //Windows systems: this option is not supported
    m_bEnablePixelDensity = false;
#endif
}

DpiManager::~DpiManager()
{
}

void DpiManager::InitDpiAwareness(const DpiInitParam& dpiInitParam)
{
    //If already initialized, do not initialize again
    ASSERT(!m_bDpiInited);
    if (m_bDpiInited) {
        return;
    }
    m_bDpiInited = true;

    //Initialize the DPI awareness mode
    DpiAwareness dpiAwareness;
    dpiAwareness.InitDpiAwareness(dpiInitParam.m_dpiAwarenessMode);
    m_dpiAwarenessMode = dpiAwareness.GetDpiAwareness();    
    m_fPixelDensity = 1.0f;

#ifdef DUI_BUILD_FOR_WIN
    //Windows systems: this option is not supported
    m_bEnablePixelDensity = false;
#else
    m_bEnablePixelDensity = dpiInitParam.m_bEnablePixelDensity;
#endif

    float fDisplayScale = dpiInitParam.m_fDisplayScale;
    if (fDisplayScale < DUI_DISPLAY_SCALE_MIN) {
        fDisplayScale = 1.0f;
    }

    //Initialize the DPI scale
    if (IsFloatEqual(fDisplayScale, 1.0f)) {
        //Default value
        m_bUserDefinedDpi = false;

        //Read the default DPI value from the system configuration (read according to the primary monitor configuration)
        SetDisplayScaleForWindow(nullptr);
    }
    else {

        //Set a custom DPI scale externally
        m_bUserDefinedDpi = true;

        //Set the display scale value
        SetDisplayScale(fDisplayScale, 1.0f);
    }
}

bool DpiManager::IsUserDefinedDpi() const
{
    return m_bUserDefinedDpi;
}

void DpiManager::SetUserDefinedDpi(bool bUserDefinedDpi)
{
    ASSERT(m_bDpiInited);
    m_bUserDefinedDpi = bUserDefinedDpi;
}

DpiAwarenessMode DpiManager::GetDpiAwareness() const
{
    DpiAwarenessMode dpiAwarenessMode = m_dpiAwarenessMode;
    if (!m_bDpiInited) {
        DpiAwareness dpiAwareness;
        dpiAwarenessMode = dpiAwareness.GetDpiAwareness();
    }
    return dpiAwarenessMode;
}

bool DpiManager::IsDpiAware() const
{
    DpiAwarenessMode dpiAwarenessMode = GetDpiAwareness();
    if ((dpiAwarenessMode == DpiAwarenessMode::kPerMonitorDpiAware) ||
        (dpiAwarenessMode == DpiAwarenessMode::kPerMonitorDpiAware_V2)) {
        return true;
    }
    else {
        return false;
    }
}

void DpiManager::SetDisplayScaleForWindow(const WindowBase* pWindow)
{
    //Read the DPI display scale of the window and the window pixel density value
    float fWindowPixelDensity = 1.0f;
    float fWindowDisplayScale = 1.0f;
    if (pWindow != nullptr) {
        fWindowDisplayScale = MonitorUtil::GetWindowDisplayScale(pWindow, fWindowPixelDensity);
    }
    else {
        fWindowDisplayScale = MonitorUtil::GetPrimaryMonitorDisplayScale();
    }
    SetDisplayScale(fWindowDisplayScale, fWindowPixelDensity);
}

void DpiManager::SetDisplayScale(float fDisplayScale, float fPixelDensity)
{
    ASSERT(GlobalManager::Instance().Dpi().m_bDpiInited);
    if (!m_bDpiInited && GlobalManager::Instance().Dpi().m_bDpiInited) {
        //Initialize the DPI state variables from the global object to keep the state consistent across the whole system
        m_bDpiInited = true;
        m_dpiAwarenessMode = GlobalManager::Instance().Dpi().m_dpiAwarenessMode;
        m_bUserDefinedDpi = GlobalManager::Instance().Dpi().m_bUserDefinedDpi;
        m_bEnablePixelDensity = GlobalManager::Instance().Dpi().m_bEnablePixelDensity;
    }
    if ((fPixelDensity < 1.0f) || !IsPixelDensityEnabled()) {
        //Invalid UI pixel density, or UI pixel density is not supported
        fPixelDensity = 1.0f;
    }
#ifdef DUI_HDPI_TEST_PIXEL_DENSITY
    //TEST ONLY
    fPixelDensity = DUI_HDPI_TEST_PIXEL_DENSITY;
    m_bEnablePixelDensity = true;

    //Simulation, test environment
    m_fPixelDensity = fPixelDensity;
    m_nScaleFactor = (uint32_t)(fDisplayScale * fPixelDensity * 100 + 0.5f); 
#else
    //Real environment
    m_fPixelDensity = fPixelDensity;
    m_nScaleFactor = (uint32_t)(fDisplayScale * 100 + 0.5f); //Save the integer value scaled by 100, keeping 2 significant digits of precision
#endif
}

bool DpiManager::IsDisplayScaleChanged(float fDisplayScale, float fPixelDensity) const
{
    if (fPixelDensity < 1.0f) {
        //Invalid UI pixel density
        fPixelDensity = 1.0f;
    }
    if (!IsPixelDensityEnabled()) {
        //UI pixel density is not supported
        fPixelDensity = 1.0f;
    }
    if (IsPixelDensityEnabled() && !IsFloatEqual(fPixelDensity, m_fPixelDensity)) {
        //The pixel density has changed
        return true;
    }
    uint32_t nScaleFactor = (uint32_t)(fDisplayScale * fPixelDensity * 100 + 0.5f); //Save the integer value scaled by 100, keeping 2 significant digits of precision
    return nScaleFactor != m_nScaleFactor;
}

uint32_t DpiManager::GetDisplayScaleFactor() const
{
    return m_nScaleFactor;
}

float DpiManager::GetDisplayScale() const
{
    return (float)m_nScaleFactor / 100.0f;
}

bool DpiManager::CheckDisplayScaleFactor(uint32_t nCheckScaleFactor) const
{
    ASSERT(nCheckScaleFactor == m_nScaleFactor);
    return (nCheckScaleFactor == m_nScaleFactor);
}

bool DpiManager::IsDisplayScaled() const
{
    return m_nScaleFactor != 100;
}

void DpiManager::ScaleInt(int32_t& nValue) const
{
    if (m_nScaleFactor != 100) {
        nValue = MulDiv(nValue, (int32_t)m_nScaleFactor, 100);
    }    
}

void DpiManager::ScaleInt(uint32_t& nValue) const
{
    if (m_nScaleFactor != 100) {
        nValue = MulDiv(nValue, m_nScaleFactor, 100u);
    }
}

int32_t DpiManager::GetScaleInt(int32_t nValue) const
{
    if (m_nScaleFactor == 100) {
        return nValue;
    }
    nValue = MulDiv(nValue, (int32_t)m_nScaleFactor, 100);
    return nValue;
}

uint32_t DpiManager::GetScaleInt(uint32_t nValue) const
{
    if (m_nScaleFactor == 100) {
        return nValue;
    }
    nValue = (uint32_t)MulDiv(nValue, m_nScaleFactor, 100u);
    return nValue;
}

void DpiManager::UnscaleInt(int32_t& nValue) const
{
    if (m_nScaleFactor != 100) {
        nValue = MulDiv(nValue, 100, (int32_t)m_nScaleFactor);
    }
}

void DpiManager::UnscaleInt(uint32_t& nValue) const
{
    if (m_nScaleFactor != 100) {
        nValue = MulDiv(nValue, 100u, m_nScaleFactor);
    }
}

int32_t DpiManager::GetUnscaleInt(int32_t nValue) const
{
    if (m_nScaleFactor != 100) {
        nValue = MulDiv(nValue, 100, (int32_t)m_nScaleFactor);
    }
    return nValue;
}

uint32_t DpiManager::GetUnscaleInt(uint32_t nValue) const
{
    if (m_nScaleFactor != 100) {
        nValue = MulDiv(nValue, 100u, m_nScaleFactor);
    }
    return nValue;
}

float DpiManager::GetScaleFloat(int32_t nValue) const
{
    return std::round((nValue * m_nScaleFactor) / 100.0f);
}

float DpiManager::GetScaleFloat(uint32_t nValue) const
{
    return std::round((nValue * m_nScaleFactor) / 100.0f);
}

float DpiManager::GetScaleFloat(float fValue) const
{
    return std::round((fValue * m_nScaleFactor) / 100.0f);
}

float DpiManager::GetScaleFloat(float fValue, uint32_t nOldScaleFactor) const
{
    if ((nOldScaleFactor == 0) || (m_nScaleFactor == 0)) {
        return fValue;
    }
    return std::round((fValue * m_nScaleFactor) / (float)nOldScaleFactor);
}

int32_t DpiManager::GetScaleInt(int32_t nValue, uint32_t nOldScaleFactor) const
{
    if ((nOldScaleFactor == 0) || (m_nScaleFactor == 0)) {
        return nValue;
    }
    nValue = (uint32_t)MulDiv(nValue, (int32_t)m_nScaleFactor, (int32_t)nOldScaleFactor);
    return nValue;
}

uint32_t DpiManager::GetScaleInt(uint32_t nValue, uint32_t nOldScaleFactor) const
{
    if ((nOldScaleFactor == 0) || (m_nScaleFactor == 0)) {
        return nValue;
    }
    nValue = (uint32_t)MulDiv(nValue, m_nScaleFactor, nOldScaleFactor);
    return nValue;
}

void DpiManager::ScaleSize(UiSize& size) const
{
    if (m_nScaleFactor == 100) {
        return;
    }
    size.cx = MulDiv(size.cx, (int32_t)m_nScaleFactor, 100);
    size.cy = MulDiv(size.cy, (int32_t)m_nScaleFactor, 100);
}

UiSize DpiManager::GetScaleSize(UiSize size) const
{
    ScaleSize(size);
    return size;
}

UiSize DpiManager::GetScaleSize(UiSize size, uint32_t nOldScaleFactor) const
{
    if ((m_nScaleFactor == 0) || (nOldScaleFactor == 0) ){
        return size;
    }
    size.cx = MulDiv(size.cx, (int32_t)m_nScaleFactor, (int32_t)nOldScaleFactor);
    size.cy = MulDiv(size.cy, (int32_t)m_nScaleFactor, (int32_t)nOldScaleFactor);
    return size;
}

void DpiManager::ScalePoint(UiPoint &point) const
{
    if (m_nScaleFactor == 100) {
        return;
    }
    point.x = MulDiv(point.x, (int32_t)m_nScaleFactor, 100);
    point.y = MulDiv(point.y, (int32_t)m_nScaleFactor, 100);
}

UiPoint DpiManager::GetScalePoint(UiPoint point, uint32_t nOldScaleFactor) const
{
    if ((m_nScaleFactor == 0) || (nOldScaleFactor == 0)) {
        return point;
    }
    point.x = MulDiv(point.x, (int32_t)m_nScaleFactor, (int32_t)nOldScaleFactor);
    point.y = MulDiv(point.y, (int32_t)m_nScaleFactor, (int32_t)nOldScaleFactor);
    return point;
}

void DpiManager::ScaleRect(UiRect &rect) const
{
    if (m_nScaleFactor == 100) {
        return;
    }
    int32_t width = MulDiv(rect.right - rect.left, (int32_t)m_nScaleFactor, 100);
    int32_t height = MulDiv(rect.bottom - rect.top, (int32_t)m_nScaleFactor, 100);
    rect.left = MulDiv(rect.left, (int32_t)m_nScaleFactor, 100);
    rect.top = MulDiv(rect.top, (int32_t)m_nScaleFactor, 100);
    rect.right = rect.left + width;
    rect.bottom = rect.top + height;
}

UiRect DpiManager::GetScaleRect(UiRect rect, uint32_t nOldScaleFactor) const
{
    if ((m_nScaleFactor == 0) || (nOldScaleFactor == 0)) {
        return rect;
    }
    int32_t width = MulDiv(rect.right - rect.left, (int32_t)m_nScaleFactor, (int32_t)nOldScaleFactor);
    int32_t height = MulDiv(rect.bottom - rect.top, (int32_t)m_nScaleFactor, (int32_t)nOldScaleFactor);
    rect.left = MulDiv(rect.left, (int32_t)m_nScaleFactor, (int32_t)nOldScaleFactor);
    rect.top = MulDiv(rect.top, (int32_t)m_nScaleFactor, (int32_t)nOldScaleFactor);
    rect.right = rect.left + width;
    rect.bottom = rect.top + height;
    return rect;
}

void DpiManager::ScalePadding(UiPadding& padding) const
{
    if (m_nScaleFactor == 100) {
        return;
    }
    padding.left = MulDiv(padding.left, (int32_t)m_nScaleFactor, 100);
    padding.top = MulDiv(padding.top, (int32_t)m_nScaleFactor, 100);
    padding.right = MulDiv(padding.right, (int32_t)m_nScaleFactor, 100);
    padding.bottom = MulDiv(padding.bottom, (int32_t)m_nScaleFactor, 100);
}

UiPadding DpiManager::GetScalePadding(UiPadding padding, uint32_t nOldScaleFactor) const
{
    if ((m_nScaleFactor == 0) || (nOldScaleFactor == 0)) {
        return padding;
    }
    padding.left = MulDiv(padding.left, (int32_t)m_nScaleFactor, (int32_t)nOldScaleFactor);
    padding.top = MulDiv(padding.top, (int32_t)m_nScaleFactor, (int32_t)nOldScaleFactor);
    padding.right = MulDiv(padding.right, (int32_t)m_nScaleFactor, (int32_t)nOldScaleFactor);
    padding.bottom = MulDiv(padding.bottom, (int32_t)m_nScaleFactor, (int32_t)nOldScaleFactor);
    return padding;
}

void DpiManager::ScaleMargin(UiMargin& margin) const
{
    if (m_nScaleFactor == 100) {
        return;
    }
    margin.left = MulDiv(margin.left, (int32_t)m_nScaleFactor, 100);
    margin.top = MulDiv(margin.top, (int32_t)m_nScaleFactor, 100);
    margin.right = MulDiv(margin.right, (int32_t)m_nScaleFactor, 100);
    margin.bottom = MulDiv(margin.bottom, (int32_t)m_nScaleFactor, 100);
}

UiMargin DpiManager::GetScaleMargin(UiMargin margin, uint32_t nOldScaleFactor) const
{
    if ((m_nScaleFactor == 0) || (nOldScaleFactor == 0)) {
        return margin;
    }
    margin.left = MulDiv(margin.left, (int32_t)m_nScaleFactor, (int32_t)nOldScaleFactor);
    margin.top = MulDiv(margin.top, (int32_t)m_nScaleFactor, (int32_t)nOldScaleFactor);
    margin.right = MulDiv(margin.right, (int32_t)m_nScaleFactor, (int32_t)nOldScaleFactor);
    margin.bottom = MulDiv(margin.bottom, (int32_t)m_nScaleFactor, (int32_t)nOldScaleFactor);
    return margin;
}

bool DpiManager::IsPixelDensityEnabled() const
{
    return m_bEnablePixelDensity;
}

bool DpiManager::HasPixelDensity() const
{
    if (!IsPixelDensityEnabled()) {
        return false;
    }
    return m_fPixelDensity > 1.00001f;
}

float DpiManager::GetPixelDensity() const
{
    return m_fPixelDensity;
}

void DpiManager::ScaleWindowSize(int32_t& windowSize) const
{
    ScaleInt(windowSize);
    if (HasPixelDensity()) {
        windowSize = (int32_t)std::round(windowSize / GetPixelDensity());
    }
}

void DpiManager::UnscaleWindowSize(int32_t& windowSize) const
{
    if (HasPixelDensity()) {
        windowSize = (int32_t)std::round(windowSize * GetPixelDensity());
    }
    UnscaleInt(windowSize);
}

int32_t DpiManager::GetScaleWindowSize(int32_t windowSize) const
{
    ScaleWindowSize(windowSize);
    return windowSize;
}

int32_t DpiManager::GetUnscaleWindowSize(int32_t windowSize) const
{
    UnscaleWindowSize(windowSize);
    return windowSize;
}

int32_t DpiManager::GetScaleWindowSize(int32_t windowSize, uint32_t nOldScaleFactor) const
{
    windowSize = GetScaleInt(windowSize, nOldScaleFactor);
    if (HasPixelDensity()) {
        windowSize = (int32_t)std::round(windowSize / GetPixelDensity());
    }
    return windowSize;
}

void DpiManager::ScaleWindowSize(UiSize& windowSize) const
{
    ScaleWindowSize(windowSize.cx);
    ScaleWindowSize(windowSize.cy);
}

void DpiManager::UnscaleWindowSize(UiSize& windowSize) const
{
    UnscaleWindowSize(windowSize.cx);
    UnscaleWindowSize(windowSize.cy);
}

UiSize DpiManager::GetScaleWindowSize(UiSize windowSize) const
{
    ScaleWindowSize(windowSize);
    return windowSize;
}

UiSize DpiManager::GetUnscaleWindowSize(UiSize windowSize) const
{
    UnscaleWindowSize(windowSize);
    return windowSize;
}

void DpiManager::ClientSizeToWindowSize(int32_t& pt) const
{
    if (HasPixelDensity()) {
        pt = (int32_t)std::round(pt / GetPixelDensity());
    }
}

void DpiManager::ClientSizeToWindowSize(UiPoint& pt) const
{
    ClientSizeToWindowSize(pt.x);
    ClientSizeToWindowSize(pt.y);
}

void DpiManager::ClientSizeToWindowSize(UiSize& size) const
{
    ClientSizeToWindowSize(size.cx);
    ClientSizeToWindowSize(size.cy);
}

void DpiManager::ClientSizeToWindowSize(UiRect& rc) const
{
    ClientSizeToWindowSize(rc.left);
    ClientSizeToWindowSize(rc.top);
    ClientSizeToWindowSize(rc.right);
    ClientSizeToWindowSize(rc.bottom);
}

void DpiManager::ClientSizeToWindowSize(UiPadding& padding) const
{
    ClientSizeToWindowSize(padding.left);
    ClientSizeToWindowSize(padding.top);
    ClientSizeToWindowSize(padding.right);
    ClientSizeToWindowSize(padding.bottom);
}

void DpiManager::ClientSizeToWindowSize(UiMargin& margin) const
{
    ClientSizeToWindowSize(margin.left);
    ClientSizeToWindowSize(margin.top);
    ClientSizeToWindowSize(margin.right);
    ClientSizeToWindowSize(margin.bottom);
}

void DpiManager::WindowSizeToClientSize(int32_t& pt) const
{
    if (HasPixelDensity()) {
        pt = (int32_t)std::round(pt * GetPixelDensity());
    }
}

void DpiManager::WindowSizeToClientSize(UiPoint& pt) const
{
    WindowSizeToClientSize(pt.x);
    WindowSizeToClientSize(pt.y);
}

void DpiManager::WindowSizeToClientSize(UiSize& size) const
{
    WindowSizeToClientSize(size.cx);
    WindowSizeToClientSize(size.cy);
}

void DpiManager::WindowSizeToClientSize(UiRect& rc) const
{
    WindowSizeToClientSize(rc.left);
    WindowSizeToClientSize(rc.right);
    WindowSizeToClientSize(rc.top);
    WindowSizeToClientSize(rc.bottom);
}

void DpiManager::WindowSizeToClientSize(UiPadding& padding) const
{
    WindowSizeToClientSize(padding.left);
    WindowSizeToClientSize(padding.right);
    WindowSizeToClientSize(padding.top);
    WindowSizeToClientSize(padding.bottom);
}

void DpiManager::WindowSizeToClientSize(UiMargin& margin) const
{
    WindowSizeToClientSize(margin.left);
    WindowSizeToClientSize(margin.right);
    WindowSizeToClientSize(margin.top);
    WindowSizeToClientSize(margin.bottom);
}

int32_t DpiManager::MulDiv(int32_t nNumber, int32_t nNumerator, int32_t nDenominator)
{
    if (nDenominator == 0) {
        return -1;
    }
    int64_t v = (int64_t)nNumber * (int64_t)nNumerator;
    return static_cast<int32_t>(std::round((double)v / nDenominator));
}

uint32_t DpiManager::MulDiv(uint32_t nNumber, uint32_t nNumerator, uint32_t nDenominator)
{
    if (nDenominator == 0) {
        return nNumber;
    }
    uint64_t v = (uint64_t)nNumber * (uint64_t)nNumerator;
    return static_cast<uint32_t>(std::round((double)v / nDenominator));
}

}
