#ifndef EXAMPLES_CHILD_WINDOW_PAINT_H_
#define EXAMPLES_CHILD_WINDOW_PAINT_H_

// duilib
#include "duilib/duilib.h"

/** Drawing implementation of the child window
*/
class ChildWindowPaint
{
public:
    explicit ChildWindowPaint(ui::ChildWindow* pChildWindow);
    ~ChildWindowPaint();

public:
    /** Window drawing (SDL_EVENT_WINDOW_EXPOSED/WM_PAINT)
     * @param [in] rcPaint The rectangular area that needs to be updated for this drawing
     * @param [in] nativeMsg The raw message content received from the system
     *             SDL implementation: nativeMsg.uMsg is SDL_EVENT_WINDOW_EXPOSED, and nativeMsg.wParam is an SDL_Window* pointer
     *             Windows implementation: nativeMsg.uMsg is WM_PAINT, and nativeMsg.wParam is the HWND handle of the window
     * @param [in] bPaintFps Whether it is currently in the dynamic drawing state (to demonstrate the child window drawing feature)
     */
    void PaintChildWindow(const ui::UiRect& rcPaint, const ui::NativeMsg& nativeMsg, bool bPaintFps);

private:
    // The associated child window
    ui::ChildWindow* m_pChildWindow;

    // Internal implementation class
    struct TImpl;
    TImpl* m_impl;
};

#endif // EXAMPLES_CHILD_WINDOW_PAINT_H_
