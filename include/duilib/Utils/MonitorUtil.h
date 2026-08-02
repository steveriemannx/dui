#ifndef UI_UTILS_MONITOR_UTIL_H_
#define UI_UTILS_MONITOR_UTIL_H_

#include "duilib/Core/UiTypes.h"

namespace ui
{
class WindowBase;

/** Display scale helper class for monitors
*/
class DUILIB_API MonitorUtil
{
public:
    /** Get the DPI scaling factor of the monitor where the window is located; if pWindowBase is nullptr, get the DPI scaling factor of the primary monitor
    * @param [in] pWindowBase The window
    * @param [out] fWindowPixelDensity Returns the pixel density value of the window
    * @return Returns the DPI scaling factor of the window; 1.0f means a display scale of 100%
    */
    static float GetWindowDisplayScale(const WindowBase* pWindowBase, float& fWindowPixelDensity);

    /** Get the DPI scaling factor of the primary monitor
    * @return Returns the DPI scaling factor of the primary monitor; 1.0f means a display scale of 100%
    */
    static float GetPrimaryMonitorDisplayScale();

};

} // namespace ui

#endif // UI_UTILS_MONITOR_UTIL_H_
