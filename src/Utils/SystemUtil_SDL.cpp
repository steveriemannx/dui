#include "duilib/Utils/SystemUtil.h"
#include "duilib/Utils/StringConvert.h"
#include "duilib/Core/Window.h"
#include "duilib/Core/Control.h"

#if defined(DUILIB_BUILD_FOR_SDL)
#include <SDL3/SDL.h>
#endif

namespace ui
{
bool SystemUtil::OpenUrl(const DString& url)
{
    if (url.empty()) {
        return false;
    }
#if defined(DUILIB_BUILD_FOR_SDL)
    return SDL_OpenURL(StringConvert::TToUTF8(url).c_str());
#else
    (void)url;
    return false;
#endif
}

bool SystemUtil::ShowMessageBox(const Window* pWindow, const DString& content, const DString& title)
{
#if defined(DUILIB_BUILD_FOR_SDL)
    SDL_Window* sdlWindow = (SDL_Window*)((pWindow != nullptr) ? pWindow->NativeWnd()->GetWindowHandle() : nullptr);
    SDL_MessageBoxFlags flags = SDL_MESSAGEBOX_INFORMATION;
    return SDL_ShowSimpleMessageBox(flags, StringConvert::TToUTF8(title).c_str(), StringConvert::TToUTF8(content).c_str(), sdlWindow);
#else
    (void)pWindow; (void)content; (void)title;
    return false;
#endif
}

} //namespace ui

