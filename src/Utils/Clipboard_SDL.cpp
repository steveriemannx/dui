#include "dui/Utils/Clipboard.h"
#include "dui/Utils/StringConvert.h"

#if defined(DUI_BUILD_FOR_SDL) || defined(DUI_BUILD_FOR_WAYLAND)
#if defined(DUI_BUILD_FOR_SDL)
#include <SDL3/SDL.h>
#endif

namespace ui
{
bool Clipboard::GetClipboardText(DStringW& text)
{
    DStringA textA;
    bool bRet = GetClipboardText(textA);
    text = StringConvert::UTF8ToWString(textA);
    return bRet;
}

bool Clipboard::GetClipboardText(DStringA& text)
{
    text.clear();
    #if defined(DUI_BUILD_FOR_SDL)
    if (SDL_HasClipboardText()) {
        char* szTemp = SDL_GetClipboardText();
        if (szTemp != nullptr) {
            text = szTemp;
            SDL_free(szTemp);
            szTemp = nullptr;
        }
    }
#endif
    return true;
}

bool Clipboard::SetClipboardText(const DStringW& text)
{
    return SetClipboardText(StringConvert::WStringToUTF8(text));
}

bool Clipboard::SetClipboardText(const DStringA& text)
{
    #if defined(DUI_BUILD_FOR_SDL)
    return SDL_SetClipboardText(text.c_str());
#else
    (void)text;
    return false;
#endif
}

} //namespace ui

#endif //DUI_BUILD_FOR_SDL
