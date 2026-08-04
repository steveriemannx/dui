#ifndef UI_CONTROL_CHILD_WINDOW_H_
#define UI_CONTROL_CHILD_WINDOW_H_

#include "dui/Core/Box.h"
#include "dui/Control/ChildWindowEvents.h"

namespace ui
{
class ChildWindowImpl;

/** Child window control; the control itself is an operating system child window. The UI library internally handles the creation and destruction of the child window, but the UI library does not perform the drawing of the child window
 *  The application layer is responsible for the drawing of the child window
 *  Implementation of the child window: on the Windows platform it is a system native child window (with the WS_CHILD attribute); on other platforms it is a SDL popup window, not a native child window, because SDL does not support native child windows
 */
class DUI_API ChildWindow : public Box
{
    typedef Box BaseClass;
public:
    explicit ChildWindow(Window* pWindow);
    virtual ~ChildWindow() override;
    ChildWindow(const ChildWindow&) = delete;
    ChildWindow& operator=(const ChildWindow&) = delete;

    /** Get the control type
    */
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;
    virtual void SetWindow(Window* pWindow) override;
    virtual void SetPos(UiRect rc) override;
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

public:
    /** Create the child window
    * @param [in] pChildWindowEvents The message callback interface of the child window
    */
    bool CreateChildWindow(ChildWindowEvents* pChildWindowEvents);

    /** Close the child window (synchronously)
    */
    void CloseChildWindow();

    /** Set the message callback interface of the child window
    * @param [in] pChildWindowEvents The message callback interface of the child window
    */
    void SetChildWindowEvents(ChildWindowEvents* pChildWindowEvents);

    /** Get the margin of the child window
     */
    UiMargin GetChildWindowMargin() const;

    /** Set the margin of the child window
     * @param [in] rcMargin The margin information of the control
     * @param [in] bNeedDpiScale Whether the margin should adapt to the DPI; false means no DPI adaptation
     */
    void SetChildWindowMargin(UiMargin rcMargin, bool bNeedDpiScale);

    /** Invalidate a rectangle range
    * @param [in] rect The invalidation range, in client area coordinates
    */
    void InvalidateChildWindowRect(const UiRect& rect);

    /** Invalidate the entire window
    */
    void InvalidateChildWindow();

    /** Update the window and perform the redraw
    */
    void UpdateChildWindow() const;

    /** Get the width and height of the child window
    */
    void GetChildWindowRect(UiRect& rect) const;

    /** Set whether the child window is a layered window (a layered window has the WS_EX_LAYERED attribute; child windows are only supported on Windows 8 and later platforms)
     *  Invalid when using SDL; SDL does not support dynamic modification, this attribute can only be applied at creation time
     */
    void SetChildWindowLayered(bool bWindowLayered);

protected:
    /** Set visible state event
    * @param [in] bChanged true means the state has changed, false means the state has not changed
    */
    virtual void OnSetVisible(bool bChanged) override;

    /** Set enabled state event
    * @param [in] bChanged true means the state has changed, false means the state has not changed
    */
    virtual void OnSetEnabled(bool bChanged) override;

private:
    /** Adjust the position of the child window
    */
    void AdjustChildWindowPos();

    /** Register the callback events the child window depends on
    */
    void RegisterWindowCallbacks(Window* pWindow);

    /** Unregister the callback events the child window depends on
    */
    void UnregisterWindowCallbacks(Window* pWindow);

private:
    //Internal implementation of the child window
    std::unique_ptr<ChildWindowImpl> m_pChildWnd;

    //The margin of the child window
    UiMargin m_childWindowMargin;

    //The ID of the callback function, used to remove the callback function
    EventCallbackID m_callbackID;
};

}//namespace ui

#endif //UI_CONTROL_CHILD_WINDOW_H_
