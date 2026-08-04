#ifndef EXAMPLES_BROWSER_BOX_H_
#define EXAMPLES_BROWSER_BOX_H_

// dui
#include "dui/dui.h"

// CEF
#include "dui/dui_cef.h"

/** Tab box
* @copyright (c) 2016, NetEase Inc. All rights reserved
* @author Redrain
* @date 2019/3/20
*/
class BrowserForm;
class BrowserBox : public ui::VBox
{
    typedef ui::VBox BaseClass;
public:
    friend class BrowserForm;

    /** Constructor
    * @param [in] pWindow the associated window
    * @param [in] browserId the unique identifier of the browser box, used to distinguish different tabs
    */
    BrowserBox(ui::Window* pWindow, std::string browserId);
    virtual ~BrowserBox() override {};

    /**
    * Get the id
    * @return string the identifier id
    */
    std::string GetBrowserId() const { return m_browserId; }

    /**
    * Get the browser window pointer that this browser box belongs to
    * @return BrowserForm* the browser window
    */
    BrowserForm* GetBrowserForm() const;

    /**
    * Get the Cef control pointer
    * @return ui::CefControl* the Cef control
    */
    ui::CefControl* GetCefControl();

    /** Get the web page title
    */
    const DString& GetTitle() const;

    /** Initialize the browser box
    * @param [in] url the initial URL
    */
    virtual void InitBrowserBox(const DString& url);

    /** Uninitialize the browser box
    */
    virtual void UninitBrowserBox();

    //////////////////////////////////////////////////////////////////////////
    //Operations related to the window merge feature
public:
    /** Control type
    */
    virtual DString GetType() const override { return _T("BrowserBox"); }

    /**
    * Override the base class virtual function to specify the form this control belongs to
    * @param[in] pWindow the pointer of the window it belongs to
    */
    virtual void SetWindow(ui::Window* pWindow) override;

protected:
    /** Get the focus
    */
    virtual bool OnSetFocus(const ui::EventArgs& msg) override;

protected:

    /** Browser object creation finished (callback thread: UI thread of the main process)
    */
    void OnAfterCreated(CefRefPtr<CefBrowser> browser);

    /** Browser object is about to close (callback thread: UI thread of the main process)
    */
    void OnBeforeClose(CefRefPtr<CefBrowser> browser);

    /** Context menu is about to show (callback thread: CEF's UI thread)
    */
    void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             CefRefPtr<CefContextMenuParams> params,
                             CefRefPtr<CefMenuModel> model);

    /** A menu command was executed (callback thread: CEF's UI thread)
    */
    bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame,
                              CefRefPtr<CefContextMenuParams> params,
                              int command_id,
                              cef_event_flags_t event_flags);
    
    /** Context menu dismissed (callback thread: CEF's UI thread)
    */
    void OnContextMenuDismissed(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame);

    /** Title changed (callback thread: UI thread of the main process)
    */
    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const DString& title);
    
    /** URL changed (callback thread: UI thread of the main process)
    */
    void OnUrlChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const DString& url);
    
    /** URL change of the main frame (callback thread: UI thread of the main process)
    */
    void OnMainUrlChange(const DString& oldUrl, const DString& newUrl);
    
    /** Favicon URL change (callback thread: UI thread of the main process)
    */
    void OnFaviconURLChange(CefRefPtr<CefBrowser> browser, const std::vector<CefString>& icon_urls);
        
    /** Fullscreen state change (callback thread: UI thread of the main process)
    */
    void OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool bFullscreen);
    
    /** Status message change (callback thread: UI thread of the main process)
    */
    void OnStatusMessage(CefRefPtr<CefBrowser> browser, const DString& value);
    
    /** Loading progress change (callback thread: UI thread of the main process)
    */
    void OnLoadingProgressChange(CefRefPtr<CefBrowser> browser, double progress);
    
    /** Media access change (callback thread: UI thread of the main process)
    */
    void OnMediaAccessChange(CefRefPtr<CefBrowser> browser, bool has_video_access, bool has_audio_access);

    /** CefDragHandler interface event: drag started (callback thread: CEF's UI thread)
    */
    bool OnDragEnter(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDragData> dragData, CefDragHandler::DragOperationsMask mask);

    /** CefDragHandler interface event: draggable regions changed (callback thread: UI thread of the main process)
    */
    void OnDraggableRegionsChanged(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const std::vector<CefDraggableRegion>& regions);

    /** A link was clicked and a new window is about to pop up (callback thread: CEF's UI thread)
    */
    bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
                       CefRefPtr<CefFrame> frame,
                       int popup_id,
                       const CefString& target_url,
                       const ui::BeforePopupEventParam& param,
                       CefWindowInfo& windowInfo,
                       CefRefPtr<CefClient>& client,
                       CefBrowserSettings& settings,
                       CefRefPtr<CefDictionaryValue>& extra_info,
                       bool* no_javascript_access);

    /** Notification that opening the new popup window failed (callback thread: UI thread of the main process)
    */
    void OnBeforePopupAborted(CefRefPtr<CefBrowser> browser, int popup_id);

    /** Callback before navigation (callback thread: CEF's UI thread)
    */
    bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefFrame> frame,
                        CefRefPtr<CefRequest> request,
                        bool user_gesture,
                        bool is_redirect);

    /** Callback before resource loading (callback thread: CEF's IO thread)
    */
    cef_return_value_t OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
                                            CefRefPtr<CefFrame> frame,
                                            CefRefPtr<CefRequest> request,
                                            CefRefPtr<CefCallback> callback);

    /** Resource redirect callback (callback thread: CEF's IO thread)
    */
    void OnResourceRedirect(CefRefPtr<CefBrowser> browser,
                            CefRefPtr<CefFrame> frame,
                            CefRefPtr<CefRequest> request,
                            CefRefPtr<CefResponse> response,
                            CefString& new_url);
    
    /** Resource response received callback (callback thread: CEF's IO thread)
    */
    bool OnResourceResponse(CefRefPtr<CefBrowser> browser,
                            CefRefPtr<CefFrame> frame,
                            CefRefPtr<CefRequest> request,
                            CefRefPtr<CefResponse> response);

    /** Resource load complete callback (callback thread: CEF's IO thread)
    */
    void OnResourceLoadComplete(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefRequest> request,
                                CefRefPtr<CefResponse> response,
                                cef_urlrequest_status_t status,
                                int64_t received_content_length);

    /** Resource protocol execution callback (callback thread: CEF's IO thread)
    */
    void OnProtocolExecution(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             CefRefPtr<CefRequest> request,
                             bool& allow_os_execution);

    /** Callback for the page loading state change (callback thread: UI thread of the main process)
    */
    void OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward);
    
    /** Callback for the page starting to load (callback thread: UI thread of the main process)
    */
    void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, cef_transition_type_t transition_type);
    
    /** Callback for the page load finishing (callback thread: UI thread of the main process)
    */
    void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode);
    
    /** Callback for a page load error (callback thread: UI thread of the main process)
    */
    void OnLoadError(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefFrame> frame,
                     cef_errorcode_t errorCode,
                     const DString& errorText,
                     const DString& failedUrl);

    /** The visibility of the developer tools changed (callback thread: UI thread of the main process)
    */
    void OnDevToolAttachedStateChange(bool bVisible);

    /** Whether a file can be downloaded (callback thread: CEF's UI thread)
    */
    bool OnCanDownload(CefRefPtr<CefBrowser> browser,
                       const CefString& url,
                       const CefString& request_method);

    /** Callback for the before-download event; CEF 109 ignores the return value (callback thread: CEF's UI thread)
    */
    bool OnBeforeDownload(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefDownloadItem> download_item,
                          const CefString& suggested_name,
                          CefRefPtr<CefBeforeDownloadCallback> callback);

    /** Callback for the download information update event (callback thread: CEF's UI thread)
    */
    void OnDownloadUpdated(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefDownloadItem> download_item,
                           CefRefPtr<CefDownloadItemCallback> callback);

    /** Callback for the open file/save file/select folder dialog (callback thread: CEF's UI thread)
    */
    bool OnFileDialog(CefRefPtr<CefBrowser> browser,
                      cef_file_dialog_mode_t mode,
                      const CefString& title,
                      const CefString& default_file_path,
                      const std::vector<CefString>& accept_filters,
                      const std::vector<CefString>& accept_extensions,
                      const std::vector<CefString>& accept_descriptions,
                      CefRefPtr<CefFileDialogCallback> callback);

    /** Callback for the main frame document load finishing (callback thread: UI thread of the main process)
    */
    void OnDocumentAvailableInMainFrame(CefRefPtr<CefBrowser> browser);

    /** Website icon download finished event (callback thread: UI thread of the main process)
    */
    virtual void OnDownloadFavIconFinished(CefRefPtr<CefBrowser> browser,
                                           const CefString& image_url,
                                           int http_status_code,
                                           CefRefPtr<CefImage> image);

private:
    ui::CefControl* m_pCefControl;
    BrowserForm* m_pBrowserForm;
    std::string m_browserId;
    DString m_url;
    DString m_title;
};

#endif //EXAMPLES_BROWSER_BOX_H_
