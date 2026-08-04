#ifndef UI_CORE_DPI_AWARENESS_H_
#define UI_CORE_DPI_AWARENESS_H_

#include "dui/Core/UiTypes.h"

namespace ui
{
/** Definition of the DPI awareness modes (referring to the DPI awareness feature definition on the Windows platform; the feature definition is also compatible with other platforms, where ultimately there are only two results: DPI awareness supported or not supported)
 *  For Linux/macOS/FreeBSD platforms:
 *  1. Setting to kDpiUnaware/kSystemDpiAware has the same result: DPI awareness is not supported, and at high DPI the UI does not scale proportionally with DPI, causing blurriness
 *  2. Setting to kFromManifest/kPerMonitorDpiAware/kPerMonitorDpiAware_V2 has the same result: DPI awareness is supported, and at high DPI the UI scales proportionally with clear display
 */
enum class DUI_API DpiAwarenessMode
{
    /** Set different DPI awareness modes according to the platform:
    *   Windows platform: read from the manifest configuration of the executable exe file, no code setting required; if the exe's manifest is not configured, DPI awareness is not supported
    *   Linux/macOS/FreeBSD platforms: DPI awareness is treated as supported by default, and will be set to kPerMonitorDpiAware_V2
    */
    kFromManifest = -1,

    /** Not aware
     *  Application DPI view: all monitors are 96 DPI, the UI display scale is 100%
     *  Behavior when the screen DPI changes: bitmap stretching (blurry)
     */
    kDpiUnaware = 0,

    /** System aware
     *  Application DPI view: all monitors are 96 DPI, the UI display scale is 100%
     *  Behavior when the screen DPI changes: bitmap stretching (blurry)
     */
    kSystemDpiAware = 1,

    /** Per-monitor aware
     *  Windows platform: introduced Windows version: 8.1
     *  Application DPI view: the DPI of the monitor where the application window is mainly located
     *  Behavior when the screen DPI changes: the UI scales proportionally with the screen DPI, with clear display
     */
    kPerMonitorDpiAware = 2,

    /** Per-monitor V2 aware
     *  Windows platform: introduced Windows version: Windows 10 Creators Update (1703)
     *  Application DPI view: the DPI of the monitor where the application window is mainly located
     *  Behavior when the screen DPI changes: the UI scales proportionally with the screen DPI, with clear display
     */
    kPerMonitorDpiAware_V2 = 3
};

/** The minimum value of the UI display scale
*/
#define DUI_DISPLAY_SCALE_MIN    (0.60f)

/** The maximum value of the UI display scale
*/
#define DUI_DISPLAY_SCALE_MAX    (5.00f)

/** Initialization parameters of the DPI awareness feature
*/
class DUI_API DpiInitParam
{
public:
    /** The DPI awareness mode set by the parameter
    */
    DpiAwarenessMode m_dpiAwarenessMode = DpiAwarenessMode::kPerMonitorDpiAware_V2;

    /** UI display scale: 1.0f means the original value with a display scale of 100%, 2.0f means enlarged to 200% of the original
    */
    float m_fDisplayScale = 1.0f;

    /** Whether UI pixel density is supported (this value is only valid when SDL is enabled, and only takes effect in system environments that support high-DPI screens)
     *  In macOS/Wayland desktop environments: true means high-DPI screens are supported, false means high-DPI screens are not supported
     *  In Windows/X11 desktop environments: this parameter is invalid and will be ignored
     */
    bool m_bEnablePixelDensity = true;
};

/** The interface of the DPI awareness feature
*/
class DUI_API DpiAwareness
{
public:
    DpiAwareness();
    ~DpiAwareness();
    DpiAwareness(const DpiAwareness&) = delete;
    DpiAwareness& operator = (const DpiAwareness&) = delete;

public:
    /** Initialize the DPI awareness mode and DPI value (this function can only be called once, subsequent calls are ignored)
    * @param [in] initParam The initialization parameter, see the parameter description for details
    */
    bool InitDpiAwareness(DpiAwarenessMode dpiAwarenessMode);

    /** Get the DPI awareness mode of the current process
    *   Note: the return value of this function may differ from the value passed to InitDpiAwareness
    */
    DpiAwarenessMode GetDpiAwareness() const;

public:
    /** Set the DPI awareness mode of the process
       (The DPI awareness mode can only be set once and cannot be modified after a successful set; if the awareness mode is set in the executable's manifest configuration, it also cannot be changed)
    * @param [in] dpiAwarenessMode The expected DPI awareness mode; the overall strategy is to degrade the setting step by step
    *             In order: kPerMonitorDpiAware_V2 -> kPerMonitorDpiAware -> kSystemDpiAware
    * @return Returns the actual DPI awareness mode of the process
    */
    DpiAwarenessMode SetDpiAwareness(DpiAwarenessMode dpiAwarenessMode);

private:
    /** The value set externally
    */
    DpiAwarenessMode m_dpiAwarenessMode;
};
}
#endif //UI_CORE_DPI_AWARENESS_H_
