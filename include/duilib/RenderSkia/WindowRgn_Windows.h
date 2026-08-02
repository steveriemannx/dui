#ifndef UI_RENDER_SKIA_WINDOW_RGN_WINDOWS_H_
#define UI_RENDER_SKIA_WINDOW_RGN_WINDOWS_H_

#include "duilib/Core/UiTypes.h"

#ifdef DUILIB_BUILD_FOR_WIN

namespace ui 
{
class WindowRgn
{
public:
    /** Set the window shape to a rounded rectangle
    * @param [in] hWnd The associated window handle
    * @param [in] rcWnd The area for which to set the RGN; coordinates are screen coordinates
    * @param [in] rx The corner radius width; must not be 0
    * @param [in] ry The corner radius height; must not be 0
    * @param [in] bRedraw Whether to redraw
    */
    static bool SetWindowRoundRectRgn(HWND hWnd, const UiRect& rcWnd, float rx, float ry, bool bRedraw);

    /** Set the window shape to a rectangle
    * @param [in] hWnd The associated window handle
    * @param [in] rcWnd The area for which to set the RGN; coordinates are screen coordinates
    * @param [in] bRedraw Whether to redraw
    */
    static bool SetWindowRectRgn(HWND hWnd, const UiRect& rcWnd, bool bRedraw);

    /** Clear the window shape setting and restore the system default shape
    * @param [in] hWnd The associated window handle
    * @param [in] bRedraw Whether to redraw
    */
    static void ClearWindowRgn(HWND hWnd, bool bRedraw);
};

} //namespace ui

#endif //DUILIB_BUILD_FOR_WIN

#endif //UI_RENDER_SKIA_WINDOW_RGN_WINDOWS_H_
