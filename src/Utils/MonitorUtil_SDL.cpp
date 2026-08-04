#include "dui/Utils/MonitorUtil.h"
#include "dui/Core/WindowBase.h"

#ifdef DUI_BUILD_FOR_SDL
#include "dui/Core/MessageLoop_SDL.h"
#include <SDL3/SDL.h>

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
    float fDisplayScale = 1.0f;
    // Detect and initialize SDL as needed
    MessageLoop_SDL::CheckInitSDL();
    SDL_DisplayID displayID = SDL_GetPrimaryDisplay();
    if (displayID != 0) {
        fDisplayScale = SDL_GetDisplayContentScale(displayID);
    }
    return fDisplayScale;
}

} // namespace ui

#endif //DUI_BUILD_FOR_SDL
