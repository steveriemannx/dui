#ifndef UI_UTILS_SYSTEM_UTIL_H_
#define UI_UTILS_SYSTEM_UTIL_H_

#include "duilib/Core/UiTypes.h"

namespace ui
{
class Window;

/** System-related helper operations
*/
class DUILIB_API SystemUtil
{
public:
    /** Open a URL
    * @param [in] url The URL to open
    */
    static bool OpenUrl(const DString& url);

    /** Show a simple MessageBox
    * @param [in] pWindow The parent window
    * @param [in] content The content to display
    * @param [in] title The title
    */
    static bool ShowMessageBox(const Window* pWindow, const DString& content, const DString& title);
};

} //namespace ui

#endif // UI_UTILS_SYSTEM_UTIL_H_
