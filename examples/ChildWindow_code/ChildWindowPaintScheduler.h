#ifndef EXAMPLES_CHILD_WINDOW_PAINT_SCHEDULER_H_
#define EXAMPLES_CHILD_WINDOW_PAINT_SCHEDULER_H_

// duilib
#include "duilib/duilib.h"

/** Drawing management interface for the child window (used to determine which child window to draw)
*/
class ChildWindowPaintScheduler
{
public:
    virtual ~ChildWindowPaintScheduler() = default;

    /** Draw this child window immediately
    * @param [in] pChildWindow The child window pointer
    */
    virtual bool PaintChildWindow(ui::ChildWindow* pChildWindow) = 0;

    /** Draw the next child window after this one immediately (draw each child window in sequence)
    * @param [in] pChildWindow The child window pointer
    */
    virtual bool PaintNextChildWindow(ui::ChildWindow* pChildWindow) = 0;
};

#endif // EXAMPLES_CHILD_WINDOW_PAINT_SCHEDULER_H_
