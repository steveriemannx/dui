#include "dui/Utils/SystemUtil.h"
#include "dui/Utils/StringConvert.h"
#include "dui/Core/Window.h"
#include "dui/Core/Control.h"

// This file provides the native Wayland fallback; Windows has its own
// implementation.
#if defined(DUI_BUILD_FOR_WAYLAND)

namespace ui
{
bool SystemUtil::OpenUrl(const std::string& url)
{
    if (url.empty()) {
        return false;
    }
    (void)url;
    return false;
}

bool SystemUtil::ShowMessageBox(const Window* pWindow, const std::string& content, const std::string& title)
{
    (void)pWindow; (void)content; (void)title;
    return false;
}

} //namespace ui

#endif // DUI_BUILD_FOR_WAYLAND
