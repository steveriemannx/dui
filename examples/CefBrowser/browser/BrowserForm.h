#ifndef EXAMPLES_BROWSER_FORM_H_
#define EXAMPLES_BROWSER_FORM_H_

// dui
#include "dui/dui.h"

// CEF
#include "dui/dui_cef.h"

class BrowserBox;

namespace ui {
    class TabCtrlItem;
}

/** Off-screen mode Cef multi-tab browser window
* @copyright (c) 2016, NetEase Inc. All rights reserved
* @author Redrain
* @date 2019/3/20
*/
class BrowserForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
    friend class DragDropManager;
public:
    BrowserForm();
    virtual ~BrowserForm() override;
    
    //Override virtual functions
    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;   

    /** Key pressed (WM_KEYDOWN or WM_SYSKEYDOWN)
    * @param [in] vkCode virtual key code
    * @param [in] modifierKey modifier key flags, valid values: ModifierKey::kFirstPress, ModifierKey::kAlt
    * @param [in] nativeMsg the raw message content received from the system
    * @param [out] bHandled whether the message has been handled; returning true means the message was processed successfully and need not be passed to the window procedure; returning false means the message will continue to be passed to the window procedure for processing
    * @return the message processing result; if the application handles this message, it should return zero
    */
    virtual LRESULT OnKeyDownMsg(ui::VirtualKeyCode vkCode, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) override;

    /** Window close message (WM_CLOSE)
    * @param [in] wParam the wParam parameter of the message
    * @param [in] nativeMsg the raw message content received from the system
    * @param [out] bHandled whether the message has been handled; returning true means the message was processed successfully and need not be passed to the window procedure; returning false means the message will continue to be passed to the window procedure for processing
    * @return the message processing result; if the application handles this message, it should return zero
    */
    virtual LRESULT OnWindowCloseMsg(uint32_t wParam, const ui::NativeMsg& nativeMsg, bool& bHandled) override;

    /** Called after the window is created, so that subclasses can do some initialization work
    */
    virtual void OnInitWindow() override;

    /** Called when the window is about to be closed, so that subclasses can do some cleanup work
    */
    virtual void OnPreCloseWindow() override;

    /** Called when the window has been closed, so that subclasses can do some cleanup work
    */
    virtual void OnCloseWindow() override;

public:
    /** Create a BrowserBox object
    * @param [in] pWindow the associated window
    * @param [in] browserId the unique identifier of the browser box, used to distinguish different tabs
    */
    virtual BrowserBox* CreateBrowserBox(ui::Window* pWindow, std::string browserId);

    /** Create a new browser box in this window
    * @param[in] browserId the browser id
    * @param[in] url the initial URL
    * @return BrowserBox* the browser box
    */
    BrowserBox* CreateBox(const std::string& browserId, DString url);

    /** Close a browser box in this window
     * @param[in] browserId the browser id
     */
    bool CloseBox(const std::string& browserId);

    /** Attach a browser box from another window to this window
    * @param[in] pBrowserBox the browser box
    * @return bool true success, false failure
    */
    bool AttachBox(BrowserBox* pBrowserBox);

    /** Detach a browser box in this window from the window
    * @param[in] pBrowserBox the browser box
    * @return bool true success, false failure
    */
    bool DetachBox(BrowserBox* pBrowserBox);

    /** Get the currently displayed browser box
    * @return BrowserBox* the browser box
    */
    BrowserBox* GetSelectedBox();

    /** Activate and switch to a browser box (and activate the browser window at the same time)
    * @param[in] browserId the browser id
    * @return void no return value
    */
    void SetActiveBox(const std::string& browserId);

    /** Determine whether the browser box is active (and whether the browser window is activated at the same time)
    * @param[in] pBrowserBox the browser box
    * @return bool true yes, false no
    */
    bool IsActiveBox(const BrowserBox* pBrowserBox);

    /** Determine whether the browser box is active (and whether the browser window is activated at the same time)
    * @param[in] browserId the browser id
    * @return bool true yes, false no
    */
    bool IsActiveBox(const DString& browserId);

    /** Get the total number of browser boxes in this window
    * @return the number of browser boxes
    */
    int32_t GetBoxCount() const;

    /** The FavIcon of the website has been downloaded
    */
    void NotifyFavicon(const BrowserBox* pBrowserBox, CefRefPtr<CefImage> image);

    /** Set the title of the tab control corresponding to a browser
    * @param[in] browserId the browser id
    * @param[in] name the title
    */
    void SetTabItemName(const DString& browserId, const DString& name);

    /** Set the URL of the tab control corresponding to a browser
    * @param [in] browserId the browser id
    * @param [in] url the URL
    */
    void SetURL(const std::string& browserId, const DString& url);

    /** The loading state of the Browser changed, update the interface
    */
    void OnLoadingStateChange(BrowserBox* pBrowserBox);

public:
    /** Open a link in a new tab/new window
    */
    void OpenLinkUrl(const DString& url, bool bInNewWindow);

protected:
    /** Click event
    */
    bool OnClicked(const ui::EventArgs& arg);

    /** Return key event
    */
    bool OnReturn(const ui::EventArgs& arg);

    /** Handle the selection message of the tab control
    * @param [in] param the related information of the message
    * @return bool true to continue passing the control message, false to stop passing the control message
    */
    bool OnTabItemSelected(const ui::EventArgs& param);

    /** Handle the click message of the close button of the tab control
    * @param [in] param the related information of the message
    * @param [in] browserId the browser id corresponding to the list item
    * @return bool true to continue passing the control message, false to stop passing the control message
    */
    bool OnTabItemClose(const ui::EventArgs& param, const std::string& browserId);

    /** Find a browser box in this window
    * @param [in] browserId the browser id
    * @return BrowserBox* the browser box
    */
    BrowserBox* FindBox(const DString& browserId);

    /** Find the tab control in this window
    * @param [in] browserId the browser id
    * @return BrowserBox* the browser box
    */
    ui::TabCtrlItem* FindTabItem(const DString& browserId);

    /** Switch a browser box to the visible state
    * @param [in] browserId the browser id
    * @return bool true success, false failure
    */
    bool ChangeToBox(const DString& browserId);

    /** A new tab was created
    * @param [in] pTabItem the interface of the tab page
    * @param [in] pBrowserBox the interface of the web page box
    */
    virtual void OnCreateNewTabPage(ui::TabCtrlItem* pTabItem, BrowserBox* pBrowserBox);

    /** A tab was closed
    * @param [in] pBrowserBox the interface of the web page box
    */
    virtual void OnCloseTabPage(BrowserBox* pBrowserBox);

protected:
    /** Before performing the drag operation, if the dragged browser box belongs to this window, notify this window
    * @param [in] browserId the browser id
    */
    bool OnBeforeDragBoxCallback(const DString& browserId);

    /** After performing the drag operation, if the dragged browser box belongs to this window, notify this window of the operation result
    * @param [in] bDropSucceed whether the browser box was dragged out
    */
    void OnAfterDragBoxCallback(bool bDropSucceed);

    /** Determine whether to drag the browser box
    * @param[in] param handles the event sent by the session merge list items on the left of the browser window
    * @return bool return value true: continue passing the control message, false: stop passing the control message
    */
    bool OnProcessTabItemDrag(const ui::EventArgs& param);

    /** Generate a bitmap of the page
    */
    std::shared_ptr<ui::IBitmap> GenerateWebPageBitmap(ui::CefControl* pCefControl);

    /** Mouse move message (WM_MOUSEMOVE)
    * @param [in] pt the mouse position, in client area coordinates
    * @param [in] modifierKey modifier key flags, valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] bFromNC true means this is an NC message (WM_NCMOUSEMOVE), false means it is a WM_MOUSEMOVE message
    * @param [in] nativeMsg the raw message content received from the system
    * @param [out] bHandled whether the message has been handled; returning true means the message was processed successfully and need not be passed to the window procedure; returning false means the message will continue to be passed to the window procedure for processing
    * @return the message processing result; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseMoveMsg(const ui::UiPoint& pt, uint32_t modifierKey, bool bFromNC, const ui::NativeMsg& nativeMsg, bool& bHandled) override;

    /** Mouse left button up message (WM_LBUTTONUP)
    * @param [in] pt the mouse position, in client area coordinates
    * @param [in] modifierKey modifier key flags, valid values: ModifierKey::kControl, ModifierKey::kShift
    * @param [in] nativeMsg the raw message content received from the system
    * @param [out] bHandled whether the message has been handled; returning true means the message was processed successfully and need not be passed to the window procedure; returning false means the message will continue to be passed to the window procedure for processing
    * @return the message processing result; if the application handles this message, it should return zero
    */
    virtual LRESULT OnMouseLButtonUpMsg(const ui::UiPoint& pt, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) override;

    /** Window lost mouse capture (WM_CAPTURECHANGED)
    * @param [in] nativeMsg the raw message content received from the system
    * @param [out] bHandled whether the message has been handled; returning true means the message was processed successfully and need not be passed to the window procedure; returning false means the message will continue to be passed to the window procedure for processing
    * @return the message processing result; if the application handles this message, it should return zero
    */
    virtual LRESULT OnCaptureChangedMsg(const ui::NativeMsg& nativeMsg, bool& bHandled) override;

private:
    /** Show or hide the developer tools
    */
    void SwitchShowDevTools();

    /** Display the current page in fullscreen
    */
    void ShowCurrentPageFullscreen();

protected:
    /** Address bar control (used to display and input the URL)
    */
    ui::RichEdit* m_pEditUrl;

    /** Tab bar
    */
    ui::TabCtrl* m_pTabCtrl;

    /** BrowserBox manager
    */
    ui::TabBox* m_pBorwserBoxTab;

    /** The currently active BrowserBox interface
    */
    BrowserBox* m_pActiveBrowserBox;

private:
    //Handle the browser box drag event
    /** Whether the mouse left button is pressed
    */
    bool m_bButtonDown;

    /** Whether it is currently in the drag state
    */
    bool m_bDragState;

    /** The coordinate point when the mouse was pressed
    */
    ui::UiPoint m_oldDragPoint;

    /** The Browser ID being dragged
    */
    DString m_dragingBrowserId;
};

#endif //EXAMPLES_BROWSER_FORM_H_
