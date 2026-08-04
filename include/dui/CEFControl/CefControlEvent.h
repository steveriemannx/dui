#ifndef UI_CEF_CONTROL_CEF_CONTROL_EVENT_H_
#define UI_CEF_CONTROL_CEF_CONTROL_EVENT_H_

#include "dui/dui_config.h"

#ifdef DUI_BUILD_FOR_CEF

#pragma warning (push)
#pragma warning (disable:4100)
#include "include/cef_client.h"

#include <functional>

/** Cef control event handling interface (available in two forms: callback functions or interface classes; either one can be used)
*  @copyright (c) 2016, NetEase Inc. All rights reserved
*  @author Redrain
*  @date 2016/7/22
*/
namespace ui
{
    //JS-related callback functions
    typedef std::function<void(bool has_error, const std::string& result)> ReportResultFunction;
    typedef std::function<void(const std::string& result)> CallJsFunctionCallback;
    typedef std::function<void(const std::string& params, ReportResultFunction callback)> CppFunction;


    //Callback functions related to the browser control

    //Callback before navigation (called on CEF's UI thread)
    typedef std::function<bool (CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefRequest> request,
                                bool user_gesture,
                                bool is_redirect)> OnBeforeBrowseEvent;

    //Callback before resource loading (called on CEF's IO thread)
    typedef std::function<cef_return_value_t (CefRefPtr<CefBrowser> browser,
                                              CefRefPtr<CefFrame> frame,
                                              CefRefPtr<CefRequest> request,
                                              CefRefPtr<CefCallback> callback)> OnBeforeResourceLoadEvent;

    //Resource redirect callback (called on CEF's IO thread)
    typedef std::function<void (CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefRequest> request,
                                CefRefPtr<CefResponse> response,
                                CefString& new_url)> OnResourceRedirectEvent;

    //Resource response callback (called on CEF's IO thread)
    typedef std::function<bool (CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefRequest> request,
                                CefRefPtr<CefResponse> response)> OnResourceResponseEvent;

    //Resource load complete callback (called on CEF's IO thread)
    typedef std::function<void (CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefRequest> request,
                                CefRefPtr<CefResponse> response,
                                cef_urlrequest_status_t status,
                                int64_t received_content_length)> OnResourceLoadCompleteEvent;

    //Resource protocol execution callback (called on CEF's IO thread)
    typedef std::function<void (CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefRequest> request,
                                bool& allow_os_execution)> OnProtocolExecutionEvent;


    //Page loading state change callback (called on the main process's UI thread)
    typedef std::function<void (CefRefPtr<CefBrowser> browser,
                                bool isLoading,
                                bool canGoBack,
                                bool canGoForward)> OnLoadingStateChangeEvent;

    //Page load start callback (called on the main process's UI thread)
    typedef std::function<void (CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                cef_transition_type_t transition_type)> OnLoadStartEvent;

    //Page load end callback (called on the main process's UI thread)
    typedef std::function<void (CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                int httpStatusCode)> OnLoadEndEvent;

    //Page load error callback (called on the main process's UI thread)
    typedef std::function<void (CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                cef_errorcode_t errorCode,
                                const DString& errorText,
                                const DString& failedUrl)> OnLoadErrorEvent;

    //Browser object creation complete (called on the main process's UI thread)
    typedef std::function<void (CefRefPtr<CefBrowser> browser)> OnAfterCreatedEvent;

    //Browser object is about to close (called on the main process's UI thread)
    typedef std::function<void (CefRefPtr<CefBrowser> browser)> OnBeforeCloseEvent;

    //Context menu is about to pop up (called on CEF's UI thread)
    typedef std::function<void (CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefContextMenuParams> params,
                                CefRefPtr<CefMenuModel> model)> OnBeforeContextMenuEvent;

    //A context menu command was executed (called on CEF's UI thread)
    typedef std::function<bool (CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefContextMenuParams> params,
                                int command_id,
                                cef_event_flags_t event_flags)> OnContextMenuCommandEvent;

    //Context menu dismissed (called on CEF's UI thread)
    typedef std::function<void (CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame)> OnContextMenuDismissedEvent;

    //Title change (called on the main process's UI thread)
    typedef std::function<void (CefRefPtr<CefBrowser> browser,
                                const DString& title)> OnTitleChangeEvent;

    //URL change (called on the main process's UI thread)
    typedef std::function<void (CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                const DString& url)> OnUrlChangeEvent;

    //Main frame URL change (called on the main process's UI thread)
    typedef std::function<void (const DString& oldUrl,
                                const DString& newUrl)> OnMainUrlChangeEvent;

    //Favicon URL change (called on the main process's UI thread)
    typedef std::function<void (CefRefPtr<CefBrowser> browser,
                                const std::vector<CefString>& icon_urls)> OnFaviconURLChangeEvent;

    //Fullscreen state change (called on the main process's UI thread)
    typedef std::function<void (CefRefPtr<CefBrowser> browser,
                                bool bFullscreen)> OnFullscreenModeChangeEvent;

    //Status message change (called on the main process's UI thread)
    typedef std::function<void (CefRefPtr<CefBrowser> browser,
                                const DString& value)> OnStatusMessageEvent;

    //Loading progress change (called on the main process's UI thread)
    typedef std::function<void (CefRefPtr<CefBrowser> browser,
                                double progress)> OnLoadingProgressChangeEvent;

    //Media access change (called on the main process's UI thread)
    typedef std::function<void (CefRefPtr<CefBrowser> browser,
                                bool has_video_access,
                                bool has_audio_access)> OnMediaAccessChangeEvent;

    //A hyperlink was clicked and a new window is about to pop up (called on CEF's UI thread)
    //Since some compilers' std::placeholders supports at most 10 placeholders, limit the parameters to at most 10
    struct BeforePopupEventParam
    {
        CefString target_frame_name;
        CefLifeSpanHandler::WindowOpenDisposition target_disposition;
        bool user_gesture;
        CefPopupFeatures popupFeatures;
    };
    typedef std::function<bool (CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                int popup_id,
                                const CefString& target_url,
                                const BeforePopupEventParam& param,
                                CefWindowInfo& windowInfo,
                                CefRefPtr<CefClient>& client,
                                CefBrowserSettings& settings,
                                CefRefPtr<CefDictionaryValue>& extra_info,
                                bool* no_javascript_access)> OnBeforePopupEvent;

    //Notification that opening the new popup window failed (called on the main process's UI thread)
    typedef std::function<void (CefRefPtr<CefBrowser> browser,
                                int popup_id)> OnBeforePopupAbortedEvent;

    //The dev tools' visibility state changed (called on the main process's UI thread)
    typedef std::function<void (bool bVisible, bool bPopup)> OnDevToolAttachedStateChangeEvent;

    //Whether the file can be downloaded (called on CEF's UI thread)
    typedef std::function<bool (CefRefPtr<CefBrowser> browser,
                                const CefString& url,
                                const CefString& request_method)> OnCanDownloadEvent;

    //Callback for the before-download event; the return value is ignored in CEF 109 (called on CEF's UI thread)
    typedef std::function<bool (CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefDownloadItem> download_item,
                                const CefString& suggested_name,
                                CefRefPtr<CefBeforeDownloadCallback> callback)> OnBeforeDownloadEvent;

    //Callback for the download information update event (called on CEF's UI thread)
    typedef std::function<void (CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefDownloadItem> download_item,
                                CefRefPtr<CefDownloadItemCallback> callback)> OnDownloadUpdatedEvent;

    //Callback for the open file/save file/select folder dialog (called on CEF's UI thread)
    typedef std::function<bool (CefRefPtr<CefBrowser> browser,
                                cef_file_dialog_mode_t mode,
                                const CefString& title,
                                const CefString& default_file_path,
                                const std::vector<CefString>& accept_filters,
                                const std::vector<CefString>& accept_extensions,
                                const std::vector<CefString>& accept_descriptions,
                                CefRefPtr<CefFileDialogCallback> callback)> OnFileDialogEvent;

    //Callback for the main frame's document load complete (called on the main process's UI thread)
    typedef std::function<void (CefRefPtr<CefBrowser> browser)> OnDocumentAvailableInMainFrameEvent;

    //Website icon download finished event (called on the main process's UI thread)
    typedef std::function<void (CefRefPtr<CefBrowser> browser,
                                const CefString& image_url,
                                int http_status_code,
                                CefRefPtr<CefImage> image)> OnDownloadFavIconFinishedEvent;

    //CefDragHandler interface
    //Drag operation (called on CEF's UI thread)
    typedef std::function<bool (CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefDragData> dragData,
                                CefDragHandler::DragOperationsMask mask)> OnDragEnterEvent;
    //Draggable regions changed (called on the main process's UI thread)
    typedef std::function<void (CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                const std::vector<CefDraggableRegion>& regions)> OnDraggableRegionsChangedEvent;

}

namespace ui
{

/** Cef control event handling interface
*/
class DUI_API CefControlEvent
{
public:
    /** Browser object creation complete (called on the main process's UI thread)
    */
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) {}

    /** Browser object is about to close (called on the main process's UI thread)
    */
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) {}

    /** Context menu is about to pop up (called on CEF's UI thread)
    */
    virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     CefRefPtr<CefContextMenuParams> params,
                                     CefRefPtr<CefMenuModel> model) {}

    /** A context menu command was executed (called on CEF's UI thread)
    */
    virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefRefPtr<CefContextMenuParams> params,
                                      int command_id,
                                      cef_event_flags_t event_flags)  { return false; }
    
    /** Context menu dismissed (called on CEF's UI thread)
    */
    virtual void OnContextMenuDismissed(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) {}

    /** Title change (called on the main process's UI thread)
    */
    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const DString& title) {}
    
    /** URL change (called on the main process's UI thread)
    */
    virtual void OnUrlChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const DString& url) {}
    
    /** Main frame URL change (called on the main process's UI thread)
    */
    virtual void OnMainUrlChange(const DString& oldUrl, const DString& newUrl) {}
    
    /** Favicon URL change (called on the main process's UI thread)
    */
    virtual void OnFaviconURLChange(CefRefPtr<CefBrowser> browser, const std::vector<CefString>& icon_urls) {}
        
    /** Fullscreen state change (called on the main process's UI thread)
    */
    virtual void OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool bFullscreen) {}
    
    /** Status message change (called on the main process's UI thread)
    */
    virtual void OnStatusMessage(CefRefPtr<CefBrowser> browser, const DString& value) {}
    
    /** Loading progress change (called on the main process's UI thread)
    */
    virtual void OnLoadingProgressChange(CefRefPtr<CefBrowser> browser, double progress) {}
    
    /** Media access change (called on the main process's UI thread)
    */
    virtual void OnMediaAccessChange(CefRefPtr<CefBrowser> browser, bool has_video_access, bool has_audio_access) {}

    /** A hyperlink was clicked and a new window is about to pop up (called on CEF's UI thread)
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
                               bool* no_javascript_access) { return true; }

    /** Notification that opening the new popup window failed (called on the main process's UI thread)
    */
    virtual void OnBeforePopupAborted(CefRefPtr<CefBrowser> browser, int popup_id) {}

    /** Callback before navigation (called on CEF's UI thread)
    */
    virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefRequest> request,
                                bool user_gesture,
                                bool is_redirect) { return false; }

    /** Callback before resource loading (called on CEF's IO thread)
    */
    virtual cef_return_value_t OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
                                                    CefRefPtr<CefFrame> frame,
                                                    CefRefPtr<CefRequest> request,
                                                    CefRefPtr<CefCallback> callback) { return RV_CONTINUE; }

    /** Resource redirect callback (called on CEF's IO thread)
    */
    virtual void OnResourceRedirect(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefRequest> request,
                                    CefRefPtr<CefResponse> response,
                                    CefString& new_url) {}
    
    /** Resource response callback (called on CEF's IO thread)
    */
    virtual bool OnResourceResponse(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefRequest> request,
                                    CefRefPtr<CefResponse> response) { return false; }

    /** Resource load complete callback (called on CEF's IO thread)
    */
    virtual void OnResourceLoadComplete(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame,
                                        CefRefPtr<CefRequest> request,
                                        CefRefPtr<CefResponse> response,
                                        cef_urlrequest_status_t status,
                                        int64_t received_content_length) {}

    /** Resource protocol execution callback (called on CEF's IO thread)
    */
    virtual void OnProtocolExecution(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     CefRefPtr<CefRequest> request,
                                     bool& allow_os_execution) {}

    /** Page loading state change callback (called on the main process's UI thread)
    */
    virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward) {}
    
    /** Page load start callback (called on the main process's UI thread)
    */
    virtual void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, cef_transition_type_t transition_type) {}
    
    /** Page load end callback (called on the main process's UI thread)
    */
    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) {}
    
    /** Page load error callback (called on the main process's UI thread)
    */
    virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             cef_errorcode_t errorCode,
                             const DString& errorText,
                             const DString& failedUrl) {}

    /** The dev tools' visibility state changed (called on the main process's UI thread)
    */
    virtual void OnDevToolAttachedStateChange(bool bVisible) {}

    /** Whether the file can be downloaded (called on CEF's UI thread)
    */
    virtual bool OnCanDownload(CefRefPtr<CefBrowser> browser,
                               const CefString& url,
                               const CefString& request_method) { return true; }

    /** Callback for the before-download event; the return value is ignored in CEF 109 (called on CEF's UI thread)
    */
    virtual bool OnBeforeDownload(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefDownloadItem> download_item,
                                  const CefString& suggested_name,
                                  CefRefPtr<CefBeforeDownloadCallback> callback) { return true; }

    /** Callback for the download information update event (called on CEF's UI thread)
    */
    virtual void OnDownloadUpdated(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefDownloadItem> download_item,
                                   CefRefPtr<CefDownloadItemCallback> callback) {}

    /** Callback for the open file/save file/select folder dialog (called on CEF's UI thread)
    */
    virtual bool OnFileDialog(CefRefPtr<CefBrowser> browser,
                              cef_file_dialog_mode_t mode,
                              const CefString& title,
                              const CefString& default_file_path,
                              const std::vector<CefString>& accept_filters,
                              const std::vector<CefString>& accept_extensions,
                              const std::vector<CefString>& accept_descriptions,
                              CefRefPtr<CefFileDialogCallback> callback) { return false; }

    /** Callback for the main frame's document load complete (called on the main process's UI thread)
    */
    virtual void OnDocumentAvailableInMainFrame(CefRefPtr<CefBrowser> browser) {}

    /** Website icon download finished event (called on the main process's UI thread)
    */
    virtual void OnDownloadFavIconFinished(CefRefPtr<CefBrowser> browser,
                                           const CefString& image_url,
                                           int http_status_code,
                                           CefRefPtr<CefImage> image) {}

    //CefDragHandler interface
    //Drag operation (called on CEF's UI thread)
    virtual bool OnDragEnter(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDragData> dragData, CefDragHandler::DragOperationsMask mask) { return false; };

    //Draggable regions changed (called on the main process's UI thread)
    virtual void OnDraggableRegionsChanged(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const std::vector<CefDraggableRegion>& regions) {};

public:
    virtual ~CefControlEvent() = default;
};

}//namespace ui

#pragma warning (pop)

#endif //DUI_BUILD_FOR_CEF
#endif //UI_CEF_CONTROL_CEF_CONTROL_EVENT_H_
