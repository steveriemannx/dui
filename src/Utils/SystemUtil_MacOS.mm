#include "dui/Utils/SystemUtil.h"
#include "dui/Utils/StringConvert.h"
#include "dui/Core/Window.h"
#include "dui/Core/Control.h"

#if defined(DUI_BUILD_FOR_MACOS)

#import <Cocoa/Cocoa.h>

namespace ui
{
bool SystemUtil::OpenUrl(const DString& url)
{
    if (url.empty()) {
        return false;
    }
    std::string utf8 = StringConvert::TToUTF8(url);
    NSString* nsUrl = [[NSString alloc] initWithBytes:(utf8.empty() ? "" : utf8.data())
                                               length:utf8.size()
                                             encoding:NSUTF8StringEncoding];
    if (nsUrl == nil) {
        return false;
    }
    NSURL* parsedUrl = [NSURL URLWithString:nsUrl];
    if (parsedUrl == nil) {
        return false;
    }
    // The example/demo URLs are often written without a scheme (e.g.
    // "www.baidu.com"); NSWorkspace needs a real URL.
    if ([parsedUrl scheme] == nil) {
        NSString* prefixed = [@"http://" stringByAppendingString:nsUrl];
        parsedUrl = [NSURL URLWithString:prefixed];
        if (parsedUrl == nil) {
            return false;
        }
    }
    return [[NSWorkspace sharedWorkspace] openURL:parsedUrl] ? true : false;
}

bool SystemUtil::ShowMessageBox(const Window* pWindow, const DString& content, const DString& title)
{
    NSString* nsContent = [NSString stringWithUTF8String:StringConvert::TToUTF8(content).c_str()];
    NSString* nsTitle = [NSString stringWithUTF8String:StringConvert::TToUTF8(title).c_str()];

    NSAlert* alert = [[NSAlert alloc] init];
    [alert setMessageText:nsTitle];
    [alert setInformativeText:nsContent];
    [alert addButtonWithTitle:@"OK"];

    [alert runModal];

    //After the modal alert closes, make sure the owning dui window is
    //foreground/key again so the main window keeps focus.
    if (pWindow != nullptr) {
        const_cast<Window*>(pWindow)->SetWindowForeground();
    }
    return true;
}

} //namespace ui

#endif //DUI_BUILD_FOR_MACOS
