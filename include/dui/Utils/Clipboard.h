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
    static bool GetClipboardText(DStringW& text);

    /** Get the clipboard string (UTF8)
    */
    static bool GetClipboardText(DStringA& text);

    /** Set the clipboard data (UTF16)
    */
    static bool SetClipboardText(const DStringW& text);

    /** Set the clipboard data (UTF8)
    */
    static bool SetClipboardText(const DStringA& text);
};

} //namespace ui

#endif // UI_UTILS_CLIPBOARD_H_
