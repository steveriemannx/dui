#ifndef UI_CORE_DRAG_WINDOW_FILTER_H_
#define UI_CORE_DRAG_WINDOW_FILTER_H_

#include "duilib/Core/Window.h"

#ifdef DUILIB_BUILD_FOR_SDL

namespace ui
{
/** Window message filter interface, used to forward mouse click messages
*/
class DUILIB_API DragWindowFilter : public IUIMessageFilter
{
public:
    DragWindowFilter(Window* pOwner, Window* pWindow);

    /**   The message handling function, with a higher processing priority than the message handling function of the Window class
    * @param [in] uMsg The message content
    * @param [in] wParam The additional message parameter
    * @param [in] lParam The additional message parameter
    * @param[out] bHandled Returning false continues dispatching the message, returning true means the message is no longer dispatched
    * @return Returns the message handling result
    */
    virtual LRESULT FilterMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) override;

private:
    /** The current window
    */
    Window* m_pWindow;

    /** The parent window
    */
    Window* m_pOwner;
};
}

#endif //DUILIB_BUILD_FOR_SDL

#endif // UI_CORE_DRAG_WINDOW_FILTER_H_
