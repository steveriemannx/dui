#ifndef UI_UTILS_CLIPBOARD_H_
#define UI_UTILS_CLIPBOARD_H_

#include "dui/Core/UiTypes.h"

namespace ui
{
/** Clipboard operations
*/
class DUI_API Clipboard
{
public:
    /** Get the clipboard string (UTF16)
    */
    static bool GetClipboardText(std::wstring& text);

    /** Get the clipboard string (UTF8)
    */
    static bool GetClipboardText(std::string& text);

    /** Set the clipboard data (UTF16)
    */
    static bool SetClipboardText(const std::wstring& text);

    /** Set the clipboard data (UTF8)
    */
    static bool SetClipboardText(const std::string& text);
};

} //namespace ui

#endif // UI_UTILS_CLIPBOARD_H_
