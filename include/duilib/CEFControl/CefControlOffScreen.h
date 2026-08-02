/**@brief Encapsulates the Cef browser object as a duilib control
 * @copyright (c) 2016, NetEase Inc. All rights reserved
 * @author Redrain
 * @date 2016/7/19
 */
#ifndef UI_CEF_CONTROL_CEF_OSR_CONTROL_H_
#define UI_CEF_CONTROL_CEF_OSR_CONTROL_H_

#include "duilib/CEFControl/CefControl.h"

#ifdef DUILIB_BUILD_FOR_CEF

#if defined (DUILIB_BUILD_FOR_WIN) && !defined (DUILIB_BUILD_FOR_SDL)
namespace client
{
    class OsrImeHandlerWin;
}
#endif

namespace ui {

class CefMemoryBlock;

/** duilib CEF control, off-screen rendering mode
*/
class DUILIB_API CefControlOffScreen :public CefControl
{
    typedef CefControl BaseClass;
public:
    explicit CefControlOffScreen(ui::Window* pWindow);
    virtual ~CefControlOffScreen(void) override;

    /// Override the parent class interface to provide customized functionality
    virtual void Init() override;
    virtual void SetPos(UiRect rc) override;
    virtual void Paint(IRender* pRender, const UiRect& rcPaint) override;
    virtual void SetWindow(Window* pWindow) override;

    /** Whether it is a CEF off-screen rendering control
    */
    virtual bool IsCefOSR() const override;

    /** Whether it is a CEF off-screen rendering control that handles IME messages itself
    */
    virtual bool IsCefOsrImeMode() const override;

    /** Save the web page as an image, with the image size the same as the control size
    */
    virtual std::shared_ptr<IBitmap> MakeImageSnapshot() override;

protected:
    /** Set visible state event
    * @param [in] bChanged true means the state has changed, false means the state has not changed
    */
    virtual void OnSetVisible(bool bChanged) override;

    /** Recreate the Browser control
    */
    virtual void ReCreateBrowser() override;
    virtual void OnImeCompositionRangeChanged(CefRefPtr<CefBrowser> browser, const CefRange& selected_range, const std::vector<CefRect>& character_bounds) override;

    /** The focused element has changed (called on the main thread)
    */
    virtual void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefDOMNode::Type type,
                                      bool bText,
                                      bool bEditable,
                                      const CefRect& nodeRect) override;

    /** Set the cursor (only valid in off-screen rendering mode)
    */
    virtual void OnCursorChange(cef_cursor_type_t type) override;

    //Cursor message
    virtual bool OnSetCursor(const EventArgs& msg) override;

    //The mouse capture of the window to which the control belongs was lost
    virtual bool OnCaptureChanged(const EventArgs& msg) override;

    //Mouse message (returns true: the message has been handled; returns false: the message has not been handled and needs to be forwarded to the parent control)
    virtual bool MouseMove(const EventArgs& msg) override;
    virtual bool MouseLeave(const EventArgs& msg) override;
    virtual bool MouseWheel(const EventArgs& msg) override;

    virtual bool ButtonDown(const EventArgs& msg) override;
    virtual bool ButtonUp(const EventArgs& msg) override;
    virtual bool ButtonDoubleClick(const EventArgs& msg) override;
    virtual bool RButtonDown(const EventArgs& msg) override;
    virtual bool RButtonUp(const EventArgs& msg) override;
    virtual bool RButtonDoubleClick(const EventArgs& msg) override;
    virtual bool MButtonDown(const EventArgs& msg) override;
    virtual bool MButtonUp(const EventArgs& msg) override;
    virtual bool MButtonDoubleClick(const EventArgs& msg) override;

    //Focus-related message handling
    virtual bool OnSetFocus(const EventArgs& msg) override;
    virtual bool OnKillFocus(const EventArgs& msg) override;

    //Keyboard message (returns true: the message has been handled; returns false: the message has not been handled and needs to be forwarded to the parent control)
    virtual bool OnChar(const EventArgs& msg) override;
    virtual bool OnKeyDown(const EventArgs& msg) override;
    virtual bool OnKeyUp(const EventArgs& msg) override;

    //IME-related message handling
    virtual bool OnImeSetContext(const EventArgs& msg) override;
    virtual bool OnImeStartComposition(const EventArgs& msg) override;
    virtual bool OnImeComposition(const EventArgs& msg) override;
    virtual bool OnImeEndComposition(const EventArgs& msg) override;

    /** The page has gained focus
    */
    virtual void OnGotFocus() override;

private:
    void SendButtonDownEvent(const EventArgs& msg);
    void SendButtonUpEvent(const EventArgs& msg);
    void SendButtonDoubleClickEvent(const EventArgs& msg);

    /** @brief Convert a normal mouse message to a CEF-recognizable mouse message
     * @param [in] msg The message
     * @return Returns the converted result
     */
    int32_t GetCefMouseModifiers(const EventArgs& msg) const;

#if defined (DUILIB_BUILD_FOR_SDL) || defined (DUILIB_BUILD_FOR_WAYLAND)
    /** Forward keyboard-related messages to BrowserHost
    */
    void SendKeyEvent(const EventArgs& msg, cef_key_event_type_t type);
#endif

protected:

#if defined (DUILIB_BUILD_FOR_WIN) && !defined (DUILIB_BUILD_FOR_SDL)
    /** Forward keyboard-related messages to BrowserHost
     * @param[in] uMsg The message
     * @param[in] wParam Additional parameters of the message
     * @param[in] lParam Additional parameters of the message
     * @param[out] bHandled Whether to continue passing the message
     * @return Returns the message processing result
     */
    LRESULT SendKeyEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

    /** IME-related message handling
    */
    void OnIMEStartComposition();
    void OnIMESetContext(UINT message, WPARAM wParam, LPARAM lParam);
    void OnIMEComposition(UINT message, WPARAM wParam, LPARAM lParam);
    void OnIMECancelCompositionEvent();

private:
    /** IME message handling to support IME input operations
    */
    std::unique_ptr<client::OsrImeHandlerWin> m_imeHandler;

#endif

private:
    /** CefRenderHandler interface, called on a non-UI thread
    *   When the browser rendering data changes, this interface is triggered; at that time, the rendering data is saved to an in-memory DC
        and the window is notified to refresh the control. In the control's Paint function, the bitmap in the in-memory DC is drawn to the window
        thus drawing the off-screen rendering data onto the window
    */
    virtual void OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type,
                         const CefRenderHandler::RectList& dirtyRects, const void* buffer,
                         int width, int height) override;//CefRenderHandler interface
    virtual void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) override;//CefRenderHandler interface
    virtual void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) override;//CefRenderHandler interface

    /** Convert client area coordinates to control coordinates
    */
    virtual void ClientToControl(UiPoint& pt) override;

    /** Handle DPI adaptation (off-screen rendering mode differs from normal mode)
    */
    void AdaptDpiScale(CefMouseEvent& mouse_event);

    /** Handle the focused element change event
    */
    void OnFocusedNodeChanged(bool bEditable, const CefRect& nodeRect);

private:
    // In-memory data for page rendering, saving the cef off-screen rendering data to the cache
    std::unique_ptr<CefMemoryBlock> m_pCefMemData;

    // In-memory data for the page popup window rendering, saving the off-screen rendering data of the cef popup window to the cache
    std::unique_ptr<CefMemoryBlock> m_pCefPopupMemData;

    // When a control like a combo box pops up on the web page, record the popup position
    CefRect m_rectPopup;

private:
    //Properties of the focused element
    CefRect m_focusNodeRect;
    bool m_bHasFocusNode;
    bool m_bFocusNodeEditable;    

    /** Whether it is currently inside the OnGotFocus callback function
    */
    bool m_bInGotFocusEvent;
};

} //namespace ui

#endif //DUILIB_BUILD_FOR_CEF
#endif //UI_CEF_CONTROL_CEF_OSR_CONTROL_H_
