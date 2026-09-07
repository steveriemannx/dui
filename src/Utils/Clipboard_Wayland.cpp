#include "dui/Utils/Clipboard.h"
#include "dui/Utils/StringConvert.h"

#if defined(DUI_BUILD_FOR_WAYLAND)

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
    return true;
}

bool Clipboard::SetClipboardText(const DStringW& text)
{
    return SetClipboardText(StringConvert::WStringToUTF8(text));
}

bool Clipboard::SetClipboardText(const DStringA& text)
{
    (void)text;
    return false;
}

} //namespace ui

#endif // DUI_BUILD_FOR_WAYLAND
