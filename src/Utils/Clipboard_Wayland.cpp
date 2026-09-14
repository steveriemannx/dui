#include "dui/Utils/Clipboard.h"
#include "dui/Utils/StringConvert.h"

#if defined(DUI_BUILD_FOR_WAYLAND)

namespace ui
{
bool Clipboard::GetClipboardText(std::wstring& text)
{
    std::string textA;
    bool bRet = GetClipboardText(textA);
    text = StringConvert::UTF8ToWString(textA);
    return bRet;
}

bool Clipboard::GetClipboardText(std::string& text)
{
    text.clear();
    return true;
}

bool Clipboard::SetClipboardText(const std::wstring& text)
{
    return SetClipboardText(StringConvert::WStringToUTF8(text));
}

bool Clipboard::SetClipboardText(const std::string& text)
{
    (void)text;
    return false;
}

} //namespace ui

#endif // DUI_BUILD_FOR_WAYLAND
