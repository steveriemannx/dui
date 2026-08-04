#ifndef UI_CORE_DPI_MANAGER_H_
#define UI_CORE_DPI_MANAGER_H_

#include "dui/Core/UiTypes.h"
#include "dui/Core/DpiAwareness.h"

#if defined DUI_BUILD_FOR_WIN && defined DUI_BUILD_FOR_SDL
    //Define a macro dedicated to testing (can simulate a high-DPI screen in the Windows environment for functional testing)
    //#define DUI_HDPI_TEST_PIXEL_DENSITY (1.5f)
#endif

namespace ui
{
class WindowBase;

/** The interface of the DPI awareness feature (supports high-DPI screens)
*/
class DUI_API DpiManager
{
public:
    DpiManager();
    ~DpiManager();
    DpiManager(const DpiManager&) = delete;
    DpiManager& operator = (const DpiManager&) = delete;

public:
    /** Initialize the DPI awareness mode and DPI value (this function can only be called once, subsequent calls are ignored)
     *   This function is used to initialize the global DPI manager at process startup
     * @param [in] dpiInitParam The initialization parameter, see the parameter description for details
     */
    void InitDpiAwareness(const DpiInitParam& dpiInitParam);

    /** Get the DPI awareness mode of the process
     * This property is a per-process property, set once after program startup and cannot be changed afterwards
     * @return Returns the DPI awareness mode of the current process
     */
    DpiAwarenessMode GetDpiAwareness() const;

    /** Whether the current configuration supports DPI awareness; when DPI awareness is supported, the same window displayed on different screens follows the DPI of the screen it is on, with clear display
     *  This property is a per-process property, set once after program startup and cannot be changed afterwards
     *  Whether DPI awareness is actually supported requires a combined check: whether (!IsUserDefinedDpi() && IsDpiAware()) is true
     * @return Returns true if DPI awareness is supported, returns false if DPI awareness is not supported
     */
    bool IsDpiAware() const;

public:
    /** Set the DPI scale of the window and initialize the DPI manager
     @param [in] pWindow The interface of the window; if nullptr, read the system-configured DPI value
    */
    void SetDisplayScaleForWindow(const WindowBase* pWindow);

    /** Set the DPI scale of the window and initialize the DPI manager
    */
    void SetDisplayScale(float fDisplayScale, float fPixelDensity);

    /** Compare whether the DPI scale has changed compared with the current value
    */
    bool IsDisplayScaleChanged(float fDisplayScale, float fPixelDensity) const;

    /** Whether it is a user-defined DPI
    * @return true means the DPI is user-defined and DPI awareness is not supported; false means the DPI is system-managed and DPI awareness is supported
    */
    bool IsUserDefinedDpi() const;

    /** Set whether the DPI is user-defined; whether DPI awareness is actually supported requires a combined check: whether (!IsUserDefinedDpi() && IsDpiAware()) is true
    * @param [in] bUserDefinedDpi true means the DPI is user-defined and DPI awareness is not supported; false means the DPI is system-managed and DPI awareness is supported
    */
    void SetUserDefinedDpi(bool bUserDefinedDpi);

public:
    /** Whether the current UI has a DPI scale
    * @return Returns false if the current UI scale percentage is 100, otherwise returns true
    */
    bool IsDisplayScaled() const;

    /** Get the current UI scale percentage factor (for example: 100 means the scale percentage is 100%, no scaling)
    * @return The scale, for example: if the returned scale is 125, it means the UI scale percentage is 125%, corresponding to a DPI value of 120
    */
    uint32_t GetDisplayScaleFactor() const;

    /** Get the current UI scale (for example: 1.0 means the scale percentage is 100%, no scaling)
    */
    float GetDisplayScale() const;

    /** Check whether the current UI scale percentage factor matches the target scale; if not, trigger an assertion error
    */
    bool CheckDisplayScaleFactor(uint32_t nCheckScaleFactor) const;

public:
    /** Whether window pixel density is supported (supported only when using the SDL implementation)
    */
    bool IsPixelDensityEnabled() const;

    /** Whether a valid window pixel density value is present
    */
    bool HasPixelDensity() const;

    /** Get the window pixel density, 1.0f means no scaling
    */
    float GetPixelDensity() const;

public:
    /** Scale an integer according to the UI scale
    */
    void ScaleInt(int32_t& nValue) const;
    void ScaleInt(uint32_t& nValue) const;
    int32_t GetScaleInt(int32_t nValue) const;
    uint32_t GetScaleInt(uint32_t nValue) const;

    int32_t GetScaleInt(int32_t nValue, uint32_t nOldScaleFactor) const;
    uint32_t GetScaleInt(uint32_t nValue, uint32_t nOldScaleFactor) const;

    float GetScaleFloat(int32_t nValue) const;
    float GetScaleFloat(uint32_t nValue) const;
    float GetScaleFloat(float fValue) const;

    float GetScaleFloat(float fValue, uint32_t nOldScaleFactor) const;

    /** Scale a UiSize according to the UI scale
    */
    void ScaleSize(UiSize& size) const;
    UiSize GetScaleSize(UiSize size) const;

    UiSize GetScaleSize(UiSize size, uint32_t nOldScaleFactor) const;

    /** Scale a UiPoint according to the UI scale
    */
    void ScalePoint(UiPoint& point) const;

    UiPoint GetScalePoint(UiPoint point, uint32_t nOldScaleFactor) const;

    /** Scale a UiRect according to the UI scale
    */
    void ScaleRect(UiRect& rect) const;
    void ScalePadding(UiPadding& padding) const;
    void ScaleMargin(UiMargin& margin) const;

    UiRect GetScaleRect(UiRect rect, uint32_t nOldScaleFactor) const;
    UiPadding GetScalePadding(UiPadding padding, uint32_t nOldScaleFactor) const;
    UiMargin GetScaleMargin(UiMargin margin, uint32_t nOldScaleFactor) const;

public:
    /// Related API interfaces for high-DPI screen support

    /** Scale window size related values according to the UI scale
    */
    void ScaleWindowSize(int32_t& windowSize) const;
    void UnscaleWindowSize(int32_t& windowSize) const;

    int32_t GetScaleWindowSize(int32_t windowSize) const;
    int32_t GetUnscaleWindowSize(int32_t windowSize) const;

    int32_t GetScaleWindowSize(int32_t windowSize, uint32_t nOldScaleFactor) const;

    void ScaleWindowSize(UiSize& windowSize) const;
    void UnscaleWindowSize(UiSize& windowSize) const;

    UiSize GetScaleWindowSize(UiSize windowSize) const;
    UiSize GetUnscaleWindowSize(UiSize windowSize) const;

    //Convert client area coordinates to screen coordinates (size only)
    void ClientSizeToWindowSize(int32_t& pt) const;
    void ClientSizeToWindowSize(UiPoint& pt) const;
    void ClientSizeToWindowSize(UiSize& size) const;
    void ClientSizeToWindowSize(UiRect& rc) const;
    void ClientSizeToWindowSize(UiPadding& padding) const;
    void ClientSizeToWindowSize(UiMargin& margin) const;

    //Convert screen coordinates to client area coordinates (size only)
    void WindowSizeToClientSize(int32_t& pt) const;
    void WindowSizeToClientSize(UiPoint& pt) const;
    void WindowSizeToClientSize(UiSize& size) const;
    void WindowSizeToClientSize(UiRect& rc) const;
    void WindowSizeToClientSize(UiPadding& padding) const;
    void WindowSizeToClientSize(UiMargin& margin) const;

public:
    /** Restore values that have been DPI scaled to their original values (i.e. restore to the original values under a scale of 1.0f)
    */
    void UnscaleInt(int32_t& nValue) const;
    void UnscaleInt(uint32_t& nValue) const;
    int32_t GetUnscaleInt(int32_t nValue) const;
    uint32_t GetUnscaleInt(uint32_t nValue) const;

public:
    /** MulDiv function wrapper
    * @return Returns -1 if nDenominator is 0; otherwise returns the result of nNumber * nNumerator / nDenominator, ensuring the intermediate result does not overflow and the result is rounded
    */
    static int32_t MulDiv(int32_t nNumber, int32_t nNumerator, int32_t nDenominator);
    static uint32_t MulDiv(uint32_t nNumber, uint32_t nNumerator, uint32_t nDenominator);

private:
    /** Whether it has been initialized
    */
    bool m_bDpiInited;

    /** Whether the DPI value is user-defined; if user-defined, DPI awareness is no longer supported
    */
    bool m_bUserDefinedDpi;

    /** Whether UI pixel density is supported (this value is only valid when SDL is enabled, and only takes effect in system environments that support high-DPI screens)
     *  In macOS/Wayland desktop environments: true means high-DPI screens are supported, false means high-DPI screens are not supported
     *  In Windows/X11 desktop environments: this parameter is invalid
     */
    bool m_bEnablePixelDensity;

    /** The DPI awareness mode of the current process
    */
    DpiAwarenessMode m_dpiAwarenessMode;

private:
    /** The DPI scale factor, 100 means no scaling
    */
    uint32_t m_nScaleFactor;

    /** The window pixel density, 1.0f means no scaling
    */
    float m_fPixelDensity;
};
}
#endif //UI_CORE_DPI_MANAGER_H_
