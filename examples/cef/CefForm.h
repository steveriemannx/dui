#ifndef EXAMPLES_CEF_FORM_H_
#define EXAMPLES_CEF_FORM_H_

// duilib
#include "duilib/duilib.h"

// CEF
#include "duilib/duilib_cef.h"

// Controls whether to use off-screen rendering:
// When true, the default window shadow scheme is used, and the window has the WS_EX_LAYERED attribute in off-screen rendering mode;
// When false, because the real window mode does not support windows with the WS_EX_LAYERED attribute, the external window shadow scheme is used;
const bool kEnableOffScreenRendering = false;

/** Window of the CEF control
*/
#if defined (DUILIB_BUILD_FOR_WIN)
class CefForm: public std::conditional<kEnableOffScreenRendering, ui::WindowImplBase, ui::ShadowWnd>::type,
               public ui::CefControlEvent
#else
class CefForm : public ui::WindowImplBase,
                public ui::CefControlEvent
#endif
{
    typedef WindowImplBase BaseClass;
public:
    CefForm();
    virtual ~CefForm() override;

    /** Resource related interfaces
     * The GetSkinFolder interface sets the skin resource path of the window to be drawn
     * The GetSkinFile interface sets the xml description file of the window to be drawn
     */
    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;

protected:
    /** Called after the window is created, so that subclasses can do some initialization work
    */
    virtual void OnInitWindow() override;

    /** Called when the window is about to be closed, so that subclasses can do some cleanup work
    */
    virtual void OnPreCloseWindow() override;

    /** Called when the window has been closed, so that subclasses can do some cleanup work
    */
    virtual void OnCloseWindow() override;

    /** Key pressed (WM_KEYDOWN or WM_SYSKEYDOWN)
    * @param [in] vkCode virtual key code
    * @param [in] modifierKey modifier key flags, valid values: ModifierKey::kFirstPress, ModifierKey::kAlt
    * @param [in] nativeMsg the raw message content received from the system
    * @param [out] bHandled whether the message has been handled; returning true means the message was processed successfully and need not be passed to the window procedure; returning false means the message will continue to be passed to the window procedure for processing
    * @return the message processing result; if the application handles this message, it should return zero
    */
    virtual LRESULT OnKeyDownMsg(ui::VirtualKeyCode vkCode, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) override;

private:
    bool OnClicked(const ui::EventArgs& msg);
    bool OnNavigate(const ui::EventArgs& msg);

    /** Callback function for the main process singleton
    */
    void OnAlreadyRunningAppRelaunch(const std::vector<DString>& argumentList);

private:
    /** Browser object creation finished (callback thread: UI thread of the main process)
    */
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;

    /** Browser object is about to close (callback thread: UI thread of the main process)
    */
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

    /** Context menu is about to show (callback thread: CEF's UI thread)
    */
    virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     CefRefPtr<CefContextMenuParams> params,
                                     CefRefPtr<CefMenuModel> model) override;

    /** A menu command was executed (callback thread: CEF's UI thread)
    */
    virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefRefPtr<CefContextMenuParams> params,
                                      int command_id,
                                      cef_event_flags_t event_flags) override;
    
    /** Context menu dismissed (callback thread: CEF's UI thread)
    */
    virtual void OnContextMenuDismissed(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) override;

    /** Title changed (callback thread: UI thread of the main process)
    */
    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const DString& title) override;
    
    /** URL changed (callback thread: UI thread of the main process)
    */
    virtual void OnUrlChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const DString& url) override;
    
    /** URL change of the main frame (callback thread: UI thread of the main process)
    */
    virtual void OnMainUrlChange(const DString& oldUrl, const DString& newUrl) override;
    
    /** Favicon URL change (callback thread: UI thread of the main process)
    */
    virtual void OnFaviconURLChange(CefRefPtr<CefBrowser> browser, const std::vector<CefString>& icon_urls) override;
        
    /** Fullscreen state change (callback thread: UI thread of the main process)
    */
    virtual void OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool bFullscreen) override;
    
    /** Status message change (callback thread: UI thread of the main process)
    */
    virtual void OnStatusMessage(CefRefPtr<CefBrowser> browser, const DString& value) override;
    
    /** Loading progress change (callback thread: UI thread of the main process)
    */
    virtual void OnLoadingProgressChange(CefRefPtr<CefBrowser> browser, double progress) override;
    
    /** Media access change (callback thread: UI thread of the main process)
    */
    virtual void OnMediaAccessChange(CefRefPtr<CefBrowser> browser, bool has_video_access, bool has_audio_access) override;

    /** CefDragHandler interface event: drag started (callback thread: CEF's UI thread)
    */
    virtual bool OnDragEnter(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDragData> dragData, CefDragHandler::DragOperationsMask mask) override;

    /** CefDragHandler interface event: draggable regions changed (callback thread: UI thread of the main process)
    */
    virtual void OnDraggableRegionsChanged(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const std::vector<CefDraggableRegion>& regions) override;


    /** A link was clicked and a new window is about to pop up (callback thread: CEF's UI thread)
    */
    virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefFrame> frame,
                               int popup_id,
                               const CefString& target_url,
                               const CefString& target_frame_name,
                               CefLifeSpanHandler::WindowOpenDisposition target_disposition,
                               bool user_gesture,
                               const CefPopupFeatures& popupFeatures,
                               CefWindowInfo& windowInfo,
                               CefRefPtr<CefClient>& client,
                               CefBrowserSettings& settings,
                               CefRefPtr<CefDictionaryValue>& extra_info,
                               bool* no_javascript_access) override;

    /** Notification that opening the new popup window failed (callback thread: UI thread of the main process)
    */
    virtual void OnBeforePopupAborted(CefRefPtr<CefBrowser> browser, int popup_id) override;

    /** Callback before navigation (callback thread: CEF's UI thread)
    */
    virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefRequest> request,
                                bool user_gesture,
                                bool is_redirect) override;

    /** Callback before resource loading (callback thread: CEF's IO thread)
    */
    virtual cef_return_value_t OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
                                                    CefRefPtr<CefFrame> frame,
                                                    CefRefPtr<CefRequest> request,
                                                    CefRefPtr<CefCallback> callback) override;

    /** Resource redirect callback (callback thread: CEF's IO thread)
    */
    virtual void OnResourceRedirect(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefRequest> request,
                                    CefRefPtr<CefResponse> response,
                                    CefString& new_url) override;
    
    /** Resource response received callback (callback thread: CEF's IO thread)
    */
    virtual bool OnResourceResponse(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefRequest> request,
                                    CefRefPtr<CefResponse> response) override;

    /** Resource load complete callback (callback thread: CEF's IO thread)
    */
    virtual void OnResourceLoadComplete(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame,
                                        CefRefPtr<CefRequest> request,
                                        CefRefPtr<CefResponse> response,
                                        cef_urlrequest_status_t status,
                                        int64_t received_content_length) override;

    /** Resource protocol execution callback (callback thread: CEF's IO thread)
    */
    virtual void OnProtocolExecution(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     CefRefPtr<CefRequest> request,
                                     bool& allow_os_execution) override;

    /** Callback for the page loading state change (callback thread: UI thread of the main process)
    */
    virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward) override;
    
    /** Callback for the page starting to load (callback thread: UI thread of the main process)
    */
    virtual void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, cef_transition_type_t transition_type) override;
    
    /** Callback for the page load finishing (callback thread: UI thread of the main process)
    */
    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) override;
    
    /** Callback for a page load error (callback thread: UI thread of the main process)
    */
    virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             cef_errorcode_t errorCode,
                             const DString& errorText, const DString& failedUrl) override;

    /** The visibility of the developer tools changed (callback thread: UI thread of the main process)
    */
    virtual void OnDevToolAttachedStateChange(bool bVisible) override;

    /** Whether a file can be downloaded (callback thread: CEF's UI thread)
    */
    virtual bool OnCanDownload(CefRefPtr<CefBrowser> browser,
                               const CefString& url,
                               const CefString& request_method) override;

    /** Callback for the before-download event; CEF 109 ignores the return value (callback thread: CEF's UI thread)
    */
    virtual bool OnBeforeDownload(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefDownloadItem> download_item,
                                  const CefString& suggested_name,
                                  CefRefPtr<CefBeforeDownloadCallback> callback) override;

    /** Callback for the download information update event (callback thread: CEF's UI thread)
    */
    virtual void OnDownloadUpdated(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefDownloadItem> download_item,
                                   CefRefPtr<CefDownloadItemCallback> callback) override;

    /** Callback for the open file/save file/select folder dialog (callback thread: CEF's UI thread)
    */
    virtual bool OnFileDialog(CefRefPtr<CefBrowser> browser,
                              cef_file_dialog_mode_t mode,
                              const CefString& title,
                              const CefString& default_file_path,
                              const std::vector<CefString>& accept_filters,
                              const std::vector<CefString>& accept_extensions,
                              const std::vector<CefString>& accept_descriptions,
                              CefRefPtr<CefFileDialogCallback> callback) override;

    /** Callback for the main frame document load finishing (callback thread: UI thread of the main process)
    */
    virtual void OnDocumentAvailableInMainFrame(CefRefPtr<CefBrowser> browser) override;

    /** Website icon download finished event (callback thread: UI thread of the main process)
    */
    virtual void OnDownloadFavIconFinished(CefRefPtr<CefBrowser> browser,
                                           const CefString& image_url,
                                           int http_status_code,
                                           CefRefPtr<CefImage> image) override;

private:
    /** The state of the developer tools changed
    */
    void OnDevToolVisibleStateChanged(bool bVisible, bool bPopup);

    /** Business logic for the file drop operation
    */
    void OnDropFiles(const DString& jsonDropFileList);

    /** Show or hide the developer tools
    */
    void SwitchShowDevTools();

private:
    ui::CefControl* m_pCefControl;
    ui::CefControl* m_pCefControlDev;
    ui::Button* m_pDevToolBtn;
    ui::RichEdit* m_pEditUrl;

    /** File drop operation: records the file list
    */
    std::vector<CefString> m_dropFileList;
};

#endif //EXAMPLES_CEF_FORM_H_
