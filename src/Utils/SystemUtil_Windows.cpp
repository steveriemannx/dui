#include "dui/Utils/SystemUtil.h"

#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)

#include "dui/Utils/StringConvert.h"
#include "dui/Core/Window.h"
#include "dui/Core/Control.h"

#include <shellapi.h>

namespace ui
{

bool SystemUtil::OpenUrl(const DString& url)
{
    if (url.empty()) {
        return false;
    }
    return ::ShellExecuteW(nullptr, L"open", StringConvert::TToWString(url).c_str(), nullptr, nullptr, SW_SHOWDEFAULT) > (HINSTANCE)32;
}

bool SystemUtil::ShowMessageBox(const Window* pWindow, const DString& content, const DString& title)
{
    HWND hWnd = (pWindow != nullptr) ? pWindow->NativeWnd()->GetHWND() : nullptr;
    ::MessageBoxW(hWnd, StringConvert::TToWString(content).c_str(), StringConvert::TToWString(title).c_str(), MB_OK);
    return true;
}

} //namespace ui

#endif //DUI_BUILD_FOR_WIN
