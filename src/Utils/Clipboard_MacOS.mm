#include "dui/Utils/Clipboard.h"
#include "dui/Utils/StringConvert.h"

#if defined(DUI_BUILD_FOR_MACOS)

#import <Cocoa/Cocoa.h>

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
    NSPasteboard* pboard = [NSPasteboard generalPasteboard];
    if (pboard == nil) {
        return false;
    }
    NSPasteboardType availableType = [pboard availableTypeFromArray:@[NSPasteboardTypeString]];
    if (availableType == nil) {
        return false;
    }
    NSString* nsText = [pboard stringForType:NSPasteboardTypeString];
    if (nsText == nil) {
        return false;
    }
    const char* utf8 = [nsText UTF8String];
    if (utf8 == nullptr) {
        return false;
    }
    text = utf8;
    return true;
}

bool Clipboard::SetClipboardText(const std::wstring& text)
{
    return SetClipboardText(StringConvert::WStringToUTF8(text));
}

bool Clipboard::SetClipboardText(const std::string& text)
{
    NSString* nsText = [[NSString alloc] initWithBytes:(text.empty() ? "" : text.data())
                                                length:text.size()
                                              encoding:NSUTF8StringEncoding];
    if (nsText == nil) {
        return false;
    }
    NSPasteboard* pboard = [NSPasteboard generalPasteboard];
    if (pboard == nil) {
        return false;
    }
    [pboard clearContents];
    return [pboard writeObjects:@[nsText]] ? true : false;
}

} //namespace ui

#endif //DUI_BUILD_FOR_MACOS
