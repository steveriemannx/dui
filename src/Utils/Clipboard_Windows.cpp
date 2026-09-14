#include "dui/Utils/Clipboard.h"
#include "dui/Utils/StringConvert.h"

#if defined (DUI_BUILD_FOR_WIN)

namespace ui
{
bool Clipboard::GetClipboardText(std::wstring& text)
{
    text.clear();
    BOOL ret = ::OpenClipboard(nullptr);
    if (ret) {
        if (::IsClipboardFormatAvailable(CF_UNICODETEXT)) {
            HANDLE h = ::GetClipboardData(CF_UNICODETEXT);
            if (h != INVALID_HANDLE_VALUE) {
                wchar_t* buf = (wchar_t*)::GlobalLock(h);
                if (buf != nullptr) {
                    std::wstring str(buf, GlobalSize(h) / sizeof(wchar_t));
                    text = str;
                    ::GlobalUnlock(h);
                }
            }
        }
        else if (::IsClipboardFormatAvailable(CF_TEXT)) {
            HANDLE h = ::GetClipboardData(CF_TEXT);
            if (h != INVALID_HANDLE_VALUE) {
                char* buf = (char*)::GlobalLock(h);
                if (buf != nullptr) {
                    std::string str(buf, GlobalSize(h));
                    text = StringConvert::MBCSToUnicode(str);
                    ::GlobalUnlock(h);
                }
            }
        }
        ::CloseClipboard();
    }
    return ret != FALSE;
}

bool Clipboard::GetClipboardText(std::string& text)
{
    std::wstring textW;
    bool bRet = GetClipboardText(textW);
    text = StringConvert::WStringToUTF8(textW);
    return bRet;
}

bool Clipboard::SetClipboardText(const std::wstring& text)
{
    if (!::OpenClipboard(nullptr)) {
        return false;
    }

    if (!::EmptyClipboard()) {
        ::CloseClipboard();
        return false;
    }

    size_t len = text.size();
    HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE, (len + 1) * sizeof(wchar_t));
    if (hMem == nullptr) {
        ::CloseClipboard();
        return false;
    }

    wchar_t* lpStr = (wchar_t*)::GlobalLock(hMem);
    ::memcpy(lpStr, text.c_str(), len * sizeof(wchar_t));
    lpStr[len] = wchar_t(0);
    ::GlobalUnlock(hMem);
    ::SetClipboardData(CF_UNICODETEXT, hMem);
    ::CloseClipboard();
    return true;
}

bool Clipboard::SetClipboardText(const std::string& text)
{
    return SetClipboardText(StringConvert::UTF8ToWString(text));
}

} //namespace ui

#endif //DUI_BUILD_FOR_WIN
