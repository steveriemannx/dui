/** @brief Implements the CefClient interface, handles events and messages issued by the Cef browser object, and exchanges data with the upper-level control
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2016/7/19
*/
#ifndef UI_CEF_CONTROL_BROWSER_HANDLER_DELEGATE_H_
#define UI_CEF_CONTROL_BROWSER_HANDLER_DELEGATE_H_

#include "dui/Core/Callback.h"

#ifdef DUI_BUILD_FOR_CEF

#pragma warning (push)
#pragma warning (disable:4100)
    #include "include/cef_client.h"
    #include "include/cef_browser.h"
    #include "include/cef_version.h"
#pragma warning (pop)

namespace ui
{
enum class CefCallbackID;

/** The message delegate interface of CefBrowserHandler; the CefBrowserHandler class handles most events of the Cef browser object
*  Some event interfaces that need to interact with the upper layer are encapsulated in this class, and CefBrowserHandler passes these events to the delegate interface
*  This interface can be extended according to requirements
*/
class DUI_API CefBrowserHandlerDelegate : public virtual SupportWeakCallback
{
public:
    /** The CefRenderHandler interface, called on a non-UI thread
    *   When the browser rendering data changes, this interface is triggered; at this time, save the rendering data to the memory DC
        and notify the window to refresh the control; in the control's Paint function, draw the bitmap of the memory DC onto the window
        In this way, the off-screen rendered data is drawn onto the window
    */
    virtual void OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type,
                         const CefRenderHandler::RectList& dirtyRects, const void* buffer,
                         int width, int height) = 0;//The CefRenderHandler interface
    virtual void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) = 0;//The CefRenderHandler interface
    virtual void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) = 0;//The CefRenderHandler interface
    virtual void OnImeCompositionRangeChanged(CefRefPtr<CefBrowser> browser, const CefRange& selected_range, const std::vector<CefRect>& character_bounds) = 0;

    //The CefContextMenuHandler interface, called on a non-UI thread
    virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) = 0;

    //The CefContextMenuHandler interface, called on a non-UI thread
    virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params,
                                      int command_id, CefContextMenuHandler::EventFlags event_flags) = 0;
    //The CefContextMenuHandler interface, called on a non-UI thread
    virtual void OnContextMenuDismissed(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) = 0;

    //The CefDisplayHandler interface
    virtual void OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) = 0;
    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) = 0;
    virtual void OnFaviconURLChange(CefRefPtr<CefBrowser> browser, const std::vector<CefString>& icon_urls) = 0;
    virtual void OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool fullscreen) = 0;
    virtual void OnStatusMessage(CefRefPtr<CefBrowser> browser, const DString& value) = 0;
    virtual void OnLoadingProgressChange(CefRefPtr<CefBrowser> browser, double progress) = 0;
    virtual void OnMediaAccessChange(CefRefPtr<CefBrowser> browser, bool has_video_access, bool has_audio_access) = 0;

    //The CefLoadHandler interface
    virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward) = 0;
    virtual void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, cef_transition_type_t transition_type) = 0;
    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) = 0;
    virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                             CefLoadHandler::ErrorCode errorCode,
                             const CefString& errorText,
                             const CefString& failedUrl) = 0;

    //The CefLifeSpanHandler interface, called on a non-UI thread
    virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefFrame> frame,
                               int popup_id,//Only exists in CEF 133 and above
                               const CefString& target_url,
                               const CefString& target_frame_name,
                               CefLifeSpanHandler::WindowOpenDisposition target_disposition,
                               bool user_gesture,
                               const CefPopupFeatures& popupFeatures,
                               CefWindowInfo& windowInfo,
                               CefRefPtr<CefClient>& client,
                               CefBrowserSettings& settings,
                               CefRefPtr<CefDictionaryValue>& extra_info,
                               bool* no_javascript_access) = 0;
    virtual void OnBeforePopupAborted(CefRefPtr<CefBrowser> browser, int popup_id) = 0;//Only exists in CEF 133 and above
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) = 0;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) = 0;

    //The CefRequestHandler interface, called on a non-UI thread
    virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool user_gesture, bool is_redirect) = 0;

    //The CefResourceRequestHandler interface, called on a non-UI thread
    virtual cef_return_value_t OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
                                                    CefRefPtr<CefFrame> frame,
                                                    CefRefPtr<CefRequest> request,
                                                    CefRefPtr<CefCallback> callback) = 0;
    virtual void OnResourceRedirect(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefRequest> request,
                                    CefRefPtr<CefResponse> response,
                                    CefString& new_url) = 0;
    virtual bool OnResourceResponse(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefRequest> request,
                                    CefRefPtr<CefResponse> response) = 0;
    virtual void OnResourceLoadComplete(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame,
                                        CefRefPtr<CefRequest> request,
                                        CefRefPtr<CefResponse> response,
                                        cef_urlrequest_status_t status,
                                        int64_t received_content_length) = 0;
    virtual void OnProtocolExecution(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     CefRefPtr<CefRequest> request,
                                     bool& allow_os_execution) = 0;

    //CefRequestHandler interface
    virtual void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
                                           CefRequestHandler::TerminationStatus status,
                                           int error_code,
                                           CefString error_string) = 0;
    virtual void OnDocumentAvailableInMainFrame(CefRefPtr<CefBrowser> browser) = 0;

    //CefDownloadHandler interface, related to file downloads
    virtual bool OnCanDownload(CefRefPtr<CefBrowser> browser,
                               const CefString& url,
                               const CefString& request_method) = 0;
    virtual bool OnBeforeDownload(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefDownloadItem> download_item,
                                  const CefString& suggested_name,
                                  CefRefPtr<CefBeforeDownloadCallback> callback) = 0;
    virtual void OnDownloadUpdated(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefDownloadItem> download_item,
                                   CefRefPtr<CefDownloadItemCallback> callback) = 0;

    //CefDialogHandler interface, opens the file dialog
    virtual bool OnFileDialog(CefRefPtr<CefBrowser> browser,
                              CefBrowserHost::FileDialogMode mode,
                              const CefString& title,
                              const CefString& default_file_path,
                              const std::vector<CefString>& accept_filters,
                              const std::vector<CefString>& accept_extensions,
                              const std::vector<CefString>& accept_descriptions,
                              CefRefPtr<CefFileDialogCallback> callback)  = 0;

    //CefKeyboardHandler interface
    virtual bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                               const CefKeyEvent& event,
                               CefEventHandle os_event,
                               bool* is_keyboard_shortcut) = 0;
    virtual bool OnKeyEvent(CefRefPtr<CefBrowser> browser,
                            const CefKeyEvent& event,
                            CefEventHandle os_event) = 0 ;

    //CefDragHandler interface
    virtual bool OnDragEnter(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDragData> dragData, CefDragHandler::DragOperationsMask mask) = 0;
    virtual void OnDraggableRegionsChanged(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const std::vector<CefDraggableRegion>& regions) = 0;

public:
    /** Convert client area coordinates to control coordinates
    */
    virtual void ClientToControl(UiPoint& pt) = 0;

    /** Update the position of the CEF control window (child window mode)
    */
    virtual void UpdateCefWindowPos() = 0;

    /** Execute a C++ function (the JS and C++ interaction functionality)
    */
    virtual bool OnExecuteCppFunc(const CefString& function_name, const CefString& params, int js_callback_id, CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) = 0;

    /** Execute a C++ callback function (the JS and C++ interaction functionality)
    */
    virtual bool OnExecuteCppCallbackFunc(int cpp_callback_id, const CefString& json_string) = 0;

    /** Check whether the following callback function exists (to reduce messages sent to the UI thread, improving efficiency)
    * @param [in] nCallbackID The ID of the callback function; see the enum type definition
    */
    virtual bool IsCallbackExists(CefCallbackID nCallbackID) = 0;

    /** The focused element has changed (called on the main thread)
    */
    virtual void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefDOMNode::Type type,
                                      bool bText,
                                      bool bEditable,
                                      const CefRect& nodeRect) = 0;

    /** Set the cursor (only valid in off-screen rendering mode, and only when using SDL)
    */
    virtual void OnCursorChange(cef_cursor_type_t type) = 0;

    /** The page has gained focus
    */
    virtual void OnGotFocus() = 0;

    /** Get the interface of the associated CEF control
    */
    virtual Control* GetCefControl() = 0;
};

/** The ID of the callback function
*/
enum class CefCallbackID
{
    OnPopupShow,
    OnPopupSize,
    OnImeCompositionRangeChanged,
    OnBeforeContextMenu,
    OnContextMenuCommand,
    OnContextMenuDismissed,
    OnAddressChange,
    OnTitleChange,
    OnFaviconURLChange,
    OnFullscreenModeChange,
    OnStatusMessage,
    OnLoadingProgressChange,
    OnMediaAccessChange,
    OnLoadingStateChange,
    OnLoadStart,
    OnLoadEnd,
    OnLoadError,
    OnBeforePopup,
    OnBeforePopupAborted,
    OnAfterCreated,
    OnBeforeClose,
    OnBeforeBrowse,
    OnBeforeResourceLoad,
    OnResourceRedirect,
    OnResourceResponse,
    OnResourceLoadComplete,
    OnProtocolExecution,
    OnRenderProcessTerminated,
    OnDocumentAvailableInMainFrame,
    OnCanDownload,
    OnBeforeDownload,
    OnDownloadUpdated,
    OnFileDialog,
    OnPreKeyEvent,
    OnKeyEvent,
    OnDragEnter,
    OnDraggableRegionsChanged
};

}//namespace ui

#endif //DUI_BUILD_FOR_CEF

#endif //UI_CEF_CONTROL_BROWSER_HANDLER_DELEGATE_H_
