#ifndef EXAMPLES_BROWSER_BOX_WINDOWS_H_
#define EXAMPLES_BROWSER_BOX_WINDOWS_H_

#include "browser/BrowserBox.h"

/**  Windows platform implementation of the tab box
*/
#if defined (DUILIB_BUILD_FOR_WIN) && !defined (DUILIB_BUILD_FOR_SDL)
class TaskbarTabItem;

class BrowserBox_Windows: public BrowserBox
{
    typedef BrowserBox BaseClass;
public:
    /** Constructor
    * @param [in] pWindow the associated window
    * @param [in] id the unique identifier of the browser box, used to distinguish different tabs
    */
    BrowserBox_Windows(ui::Window* pWindow, std::string id);
    virtual ~BrowserBox_Windows() override {};

public:
    /** Get the Tab pointer bound to this browser box
    * @return TaskbarItem*    the Tab pointer
    */
    TaskbarTabItem* GetTaskbarItem() const;

    /** Control type
    */
    virtual DString GetType() const override { return _T("BrowserBox_Windows"); }

    /** Initialize the browser box
    * @param [in] url the initial URL
    */
    virtual void InitBrowserBox(const DString& url);

    /** Uninitialize the browser box
    */
    virtual void UninitBrowserBox();

    /** Override the base class virtual function, in order to redraw the thumbnail shown in the taskbar
    */
    virtual void Invalidate() override;

    /** Override the base class virtual function, in order to redraw the thumbnail shown in the taskbar
    * @param [in] rc the position of the control to be set
    */
    virtual void SetPos(ui::UiRect rc) override;

    /** Title changed (callback thread: UI thread of the main process)
    */
    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const DString& title) override;

     /** Website icon download finished event (callback thread: UI thread of the main process)
    */
    virtual void OnDownloadFavIconFinished(CefRefPtr<CefBrowser> browser,
                                           const CefString& image_url,
                                           int http_status_code,
                                           CefRefPtr<CefImage> image) override;

private:
    /** Convert a CEF image to an icon handle
    */
    HICON ConvertCefImageToHICON(CefImage& cefImage) const;

private:
    // Taskbar thumbnail management
    TaskbarTabItem* m_pTaskBarItem;
};

#endif //defined (DUILIB_BUILD_FOR_WIN) && !defined (DUILIB_BUILD_FOR_SDL)

#endif //EXAMPLES_BROWSER_BOX_WINDOWS_H_
