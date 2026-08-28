#include "dui/Utils/MonitorUtil.h"
#include "dui/Core/WindowBase.h"
#include "dui/Core/NativeWindow_MacOS.h"

#if defined(DUI_BUILD_FOR_MACOS)

#import <Cocoa/Cocoa.h>

namespace ui
{

float MonitorUtil::GetWindowDisplayScale(const WindowBase* pWindowBase, float& fWindowPixelDensity)
{   
    if ((pWindowBase != nullptr) && pWindowBase->IsWindow()) {
        fWindowPixelDensity = pWindowBase->NativeWnd()->GetWindowPixelDensity();
        return pWindowBase->NativeWnd()->GetWindowDisplayScale();
    }
    else {
        fWindowPixelDensity = 1.0f;
        return GetPrimaryMonitorDisplayScale();
    }
}

float MonitorUtil::GetPrimaryMonitorDisplayScale()
{
    NSScreen* screen = [NSScreen mainScreen];
    if (screen == nil) {
        return 1.0f;
    }
    return (float)[screen backingScaleFactor];
}

} // namespace ui

#endif //DUI_BUILD_FOR_MACOS
