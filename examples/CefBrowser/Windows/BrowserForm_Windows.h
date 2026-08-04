#ifndef EXAMPLES_BROWSER_FORM_WINDOWS_H_
#define EXAMPLES_BROWSER_FORM_WINDOWS_H_

// dui
#include "dui/dui.h"

#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)

#include "Browser/BrowserForm.h"
#include "Windows/taskbar/TaskbarManager.h"

class BrowserBox;

/** Off-screen mode Cef multi-tab browser window (Windows implementation part, with the taskbar thumbnail feature added)
*/
class BrowserForm_Windows: public BrowserForm, public TaskbarManager::ITaskbarDelegate
{
    typedef BrowserForm BaseClass;
public:
    BrowserForm_Windows();
    virtual ~BrowserForm_Windows() override;

    /** Create a BrowserBox object
    * @param [in] pWindow the associated window
    * @param [in] id the unique identifier of the browser box, used to distinguish different tabs
    */
    virtual BrowserBox* CreateBrowserBox(ui::Window* pWindow, std::string id) override;

    /** Intercept and handle the underlying form messages
    * @param[in] uMsg the message type
    * @param[in] wParam additional parameter
    * @param[in] lParam additional parameter
    * @param[out] bHandled whether the message was handled; if handled, the message is not passed on
    * @return LRESULT the processing result
    */
    virtual LRESULT OnWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) override;   

public:
    /** Get the form handle
    * @return HWND    the form handle
    */
    virtual HWND GetHandle() const override { return this->NativeWnd()->GetHWND(); };

    /** Get the render interface
    * @return IRender*    the render interface
    */
    virtual ui::IRender* GetTaskbarRender() const override { return this->GetRender(); };

    /** Close a taskbar item
    * @param[in] id the taskbar item id
    * @return void    no return value
    */
    virtual void CloseTaskbarItem(const std::string &id) override { CloseBox(id); }

    /** Activate and switch to a taskbar item
    * @param[in] id the taskbar item id
    * @return void no return value
    */
    virtual void SetActiveTaskbarItem(const std::string &id) override { SetActiveBox(id); }

private:
    /** A new tab was created
    * @param [in] pTabItem the interface of the tab page
    * @param [in] pBrowserBox the interface of the web page box
    */
    virtual void OnCreateNewTabPage(ui::TabCtrlItem* pTabItem, BrowserBox* pBrowserBox) override;

    /** A tab was closed
    * @param [in] pBrowserBox the interface of the web page box
    */
    virtual void OnCloseTabPage(BrowserBox* pBrowserBox) override;

private:
    // Taskbar thumbnail manager
    TaskbarManager m_taskbarManager;
};

#endif //defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
#endif //EXAMPLES_BROWSER_FORM_WINDOWS_H_
