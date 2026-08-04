#ifndef UI_CEF_CONTROL_CEF_WINDOW_UTILS_H_
#define UI_CEF_CONTROL_CEF_WINDOW_UTILS_H_

#include "CefControl.h"

#ifdef DUI_BUILD_FOR_CEF

#include <vector>

namespace ui
{
/** Set the position of the window associated with CEF (the position is the same as that of pCefControl)
*/
void SetCefWindowPos(CefWindowHandle cefWindow, CefControl* pCefControl);

/** Set the visibility of the window associated with CEF (the visibility is the same as that of pCefControl)
*/
void SetCefWindowVisible(CefWindowHandle cefWindow, CefControl* pCefControl);

/** Set the parent window of the window associated with CEF (the parent window is the window associated with pCefControl)
*/
void SetCefWindowParent(CefWindowHandle cefWindow, CefControl* pCefControl);

/** Capture a screenshot of the CEF window as bitmap data
*/
bool CaptureCefWindowBitmap(CefWindowHandle cefWindow, std::vector<uint8_t>& bitmap, int32_t& width, int32_t& height);

/** Set the cursor
*/
void SetCefWindowCursor(CefWindowHandle cefWindow, CefCursorHandle cursor);

/** Remove the parent-child relationship between the CEF child window and its parent window
*/
void RemoveCefWindowFromParent(CefWindowHandle cefWindow);

} //namespace ui

#endif //DUI_BUILD_FOR_CEF
#endif //UI_CEF_CONTROL_CEF_WINDOW_UTILS_H_
