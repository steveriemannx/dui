#ifndef UI_CEF_CONTROL_CEF_CONTROL_H_
#define UI_CEF_CONTROL_CEF_CONTROL_H_

#include "duilib/Core/Control.h"

#ifdef DUILIB_BUILD_FOR_CEF

#include "duilib/CEFControl/CefControlEvent.h"
#include "duilib/CEFControl/internal/CefBrowserHandlerDelegate.h"
#include <memory>

namespace ui {

class CefJSBridge;
class CefBrowserHandler;
class IBitmap;

/**@brief Wraps the CEF browser object as a duilib control base class
 * @copyright (c) 2016, NetEase Inc. All rights reserved
 * @author Redrain
 * @date 2019/3/20
 */
class DUILIB_API CefControl: public Control, public CefBrowserHandlerDelegate
{
    typedef Control BaseClass;
public:
    explicit CefControl(ui::Window* pWindow);
    virtual ~CefControl() override;

public:
    /** Loads a URL
    * @param [in] url The website URL
    */
    void LoadURL(const CefString& url);

    /** Goes back
    */
    void GoBack();

    /** Goes forward
    */
    void GoForward();

    /** Checks whether it can go back
    * @return Returns true if it can, false otherwise
    */
    bool CanGoBack();

    /** Checks whether it can go forward
    * @return Returns true if it can, false otherwise
    */
    bool CanGoForward();

    /** Refreshes
    */
    void Refresh();

    /** Stops loading
    */
    void StopLoad();

    /** Whether it is loading
    * @return Returns true if loading, otherwise false
    */
    bool IsLoading();

    /** Starts a download task
    * @param[in] url The address of the file to download
    */
    void StartDownload(const CefString& url);

    /** Sets the page zoom level
    * @param[in] zoom_level The zoom value
    */
    void SetZoomLevel(float zoom_level);

    /** Gets the window handle that the browser object belongs to
    * @return The window handle
    */
    CefWindowHandle GetCefWindowHandle() const;

    /** Gets the page URL
    * @return Returns the URL
    */
    CefString GetURL();

    /** Gets the URL in UTF8 format
    * @return Returns the URL
    */
    std::string GetUTF8URL();

    /** Gets the address before the # in the URL
    * @param[in] url The complete address to process
    * @return Returns the extracted address
    */
    CefString GetMainURL(const CefString& url);

    /** Registers a C++ method for the frontend to call
    * @param[in] function_name The method name
    * @param[in] function The method function body
    * @param[in] global_function Whether it is a global method
    * @return Returns true if registration succeeded, false if it may already be registered
    */
    bool RegisterCppFunc(const DString& function_name, CppFunction function, bool global_function = false);

    /** Unregisters a C++ method
    * @param[in] function_name The method name
    */
    void UnRegisterCppFunc(const DString& function_name);

    /** Calls a method already registered by the frontend
    * @param[in] js_function_name The method name provided by the frontend
    * @param[in] params The parameters passed in JSON string format
    * @param[in] callback The callback function invoked after the frontend completes execution
    * @param[in] frame_name The name of the frame whose methods are to be called; the main frame is used by default
    * @return Returns true on a successful call, false on failure; the method may not exist
    */
    bool CallJSFunction(const DString& js_function_name, const DString& params, CallJsFunctionCallback callback, const DString& frame_name = _T(""));

    /** Calls a method already registered by the frontend
    * @param[in] js_function_name The method name provided by the frontend
    * @param[in] params The parameters passed in JSON string format
    * @param[in] callback The callback function invoked after the frontend completes execution
    * @param[in] frame_id The ID of the frame whose methods are to be called; the main frame is used by default
    * @return Returns true on a successful call, false on failure; the method may not exist
    */
    bool CallJSFunction(const DString& js_function_name, const DString& params, ui::CallJsFunctionCallback callback, const CefString& frame_id);

    /** Repairs the browser object (if the Browser object has not been created, it will be recreated)
    */
    virtual void RepairBrowser();

    /** Closes all Browser objects
    */
    virtual void CloseAllBrowsers();

    /** Saves the web page as an image, with the image size the same as the control size
    */
    virtual std::shared_ptr<IBitmap> MakeImageSnapshot();

    /** Handles the host window close event
    */
    virtual void OnHostWindowClosed();

public:

    /** Sets the control associated with the developer tools (once set, the developer tools content is displayed in this control; if nullptr, the developer tools are shown in a pop-up window)
    *   This setting only takes effect in off-screen rendering mode; in non-off-screen rendering mode, this parameter is ignored
    */
    void SetDevToolsView(CefControl* pDevToolsView);

    /** Opens the developer tools
    * @return Returns true on success, false on failure
    */
    bool AttachDevTools();

    /** Closes the developer tools
    */
    void DettachDevTools();

    /** Checks whether the developer tools are open
    * @return Returns true if attached, false if not attached
    */
    bool IsAttachedDevTools() const;

    /** Clears the DevTools attachment flag and triggers events as needed
    */
    void ResetDevToolAttachedState();

    /** Sets whether the F12 shortcut key (developer tools) is allowed
    */
    void SetEnableF12(bool bEnableF12);

    /** Whether the F12 shortcut key (developer tools) is allowed
    */
    bool IsEnableF12() const;

    /** Sets whether the F11 shortcut key (page fullscreen/page exit fullscreen) is allowed
    */
    void SetEnableF11(bool bEnableF11);

    /** Whether the F11 shortcut key (page fullscreen/page exit fullscreen) is allowed
    */
    bool IsEnableF11() const;

    /** Sets whether to download the website FavIcon
    * @param [in] bDownload true to download, false not to download
    */
    void SetDownloadFaviconImage(bool bDownload);

    /** Whether to download the website FavIcon
    */
    bool IsDownloadFaviconImage() const;

    /** Sets the initially loaded URL (only effective when called before the control is initialized)
    */
    void SetInitURL(const DString& url);

    /** Gets the initially loaded URL
    */
    DString GetInitURL() const;

    /** Sets whether the initially loaded URL is a local file
    */
    void SetInitUrlIsLocalFile(bool bUrlIsLocalFile);

    /** Gets whether the initially loaded URL is a local file
    */
    bool IsInitUrlIsLocalFile() const;

public:
    /** Binds a callback function to listen for the right-click menu opening (the callback is invoked on the CEF's UI thread)
    * @param [in] callback A callback function; refer to the OnBeforeContextMenuEvent declaration
    */
    void AttachBeforeContextMenu(const OnBeforeContextMenuEvent& callback){ m_pfnBeforeContextMenu = callback; }

    /** Binds a callback function to listen for which right-click menu item is selected (the callback is invoked on the CEF's UI thread)
    * @param [in] callback A callback function; refer to the OnContextMenuCommandEvent declaration
    */
    void AttachContextMenuCommand(const OnContextMenuCommandEvent& callback){ m_pfnContextMenuCommand = callback; }

    /** Binds a callback function to listen for the right-click menu being dismissed (the callback is invoked on the CEF's UI thread)
    * @param [in] callback A callback function; refer to the OnContextMenuDismissedEvent declaration
    */
    void AttachContextMenuDismissed(const OnContextMenuDismissedEvent& callback) { m_pfnContextMenuDismissed = callback; }

    /** Binds a callback function to listen for page title changes (the callback is invoked on the main process's UI thread)
    * @param [in] callback A callback function; refer to the OnTitleChangeEvent declaration
    */
    void AttachTitleChange(const OnTitleChangeEvent& callback){ m_pfnTitleChange = callback; }

    /** Binds a callback function to listen for changes to the frame URL in the page (the callback is invoked on the main process's UI thread)
    * @param [in] callback A callback function; refer to the OnUrlChangeEvent declaration
    */
    void AttachUrlChange(const OnUrlChangeEvent& callback){ m_pfnUrlChange = callback; }

    /** Binds a callback function to listen for changes to the main page URL (the callback is invoked on the main process's UI thread)
    * @param [in] callback A callback function; refer to the OnMainUrlChangeEvent declaration
    */
    void AttachMainUrlChange(OnMainUrlChangeEvent callback){ m_pfnMainUrlChange = callback; }

    /** Binds a callback function to listen for changes to the main page FaviconURL (the callback is invoked on the CEF's UI thread)
    * @param [in] callback A callback function; refer to the OnFaviconURLChangeEvent declaration
    */
    void AttachFaviconURLChange(OnFaviconURLChangeEvent callback) { m_pfnFaviconURLChange = callback; }

    /** Binds a callback function to listen for changes to the page fullscreen state (the callback is invoked on the main process's UI thread)
    * @param [in] callback A callback function; refer to the OnFullscreenModeChangeEvent declaration
    */
    void AttachFullscreenModeChange(OnFullscreenModeChangeEvent callback) { m_pfnFullscreenModeChange = callback; }

    /** Binds a callback function to listen for changes to the page status message (the callback is invoked on the main process's UI thread)
    * @param [in] callback A callback function; refer to the OnStatusMessageEvent declaration
    */
    void AttachStatusMessage(OnStatusMessageEvent callback) { m_pfnStatusMessage = callback; }

    /** Binds a callback function to listen for changes to the page loading progress (the callback is invoked on the main process's UI thread)
    * @param [in] callback A callback function; refer to the OnLoadingProgressChangeEvent declaration
    */
    void AttachLoadingProgressChange(OnLoadingProgressChangeEvent callback) { m_pfnLoadingProgressChange = callback; }

    /** Binds a callback function to listen for changes to the page media access status (the callback is invoked on the main process's UI thread)
    * @param [in] callback A callback function; refer to the OnMediaAccessChangeEvent declaration
    */
    void AttachMediaAccessChange(OnMediaAccessChangeEvent callback) { m_pfnMediaAccessChange = callback; }

    /** Binds a callback function to listen for page drag operations (the callback is invoked on the CEF's UI thread)
    * @param [in] callback A callback function; refer to the OnDragEnterEvent declaration
    */
    void AttachDragEnter(OnDragEnterEvent callback) { m_pfnDragEnter = callback; }

    /** Binds a callback function to listen for changes to the page draggable regions (the callback is invoked on the main thread's UI thread)
    * @param [in] callback A callback function; refer to the OnDraggableRegionsChangedEvent declaration
    */
    void AttachDraggableRegionsChanged(OnDraggableRegionsChangedEvent callback) { m_pfnDraggableRegionsChanged = callback; }

    /** Binds a callback function to listen for notifications of a pop-up window opening (the callback is invoked on the CEF's UI thread)
    * @param [in] callback A callback function; refer to the OnBeforePopupEvent declaration
    */
    void AttachBeforePopup(const OnBeforePopupEvent& callback){ m_pfnBeforePopup = callback; }

    /** Binds a callback function to listen for notifications of a pop-up window being aborted (the callback is invoked on the main process's UI thread)
    * @param [in] callback A callback function; refer to the OnBeforePopupAbortedEvent declaration
    */
    void AttachBeforePopupAborted(const OnBeforePopupAbortedEvent& callback) { m_pfnBeforePopupAborted = callback; }

    /** Binds a callback function to listen for page resource loading events (the callback is invoked on the CEF's UI thread)
    * @param [in] callback A callback function; refer to the OnBeforeResourceLoadEvent declaration
    */
    void AttachBeforeResourceLoad(const OnBeforeResourceLoadEvent& callback) { m_pfnBeforeResourceLoad = callback; }

    /** Binds a callback function to listen for page resource loading redirect events (the callback is invoked on the CEF's UI thread)
    * @param [in] callback A callback function; refer to the OnResourceRedirectEvent declaration
    */
    void AttachResourceRedirect(const OnResourceRedirectEvent& callback) { m_pfnResourceRedirect = callback; }

    /** Binds a callback function to listen for page resource loading response events (the callback is invoked on the CEF's UI thread)
    * @param [in] callback A callback function; refer to the OnResourceResponseEvent declaration
    */
    void AttachResourceResponse(const OnResourceResponseEvent& callback) { m_pfnResourceResponse = callback; }

    /** Binds a callback function to listen for page resource loading complete events (the callback is invoked on the CEF's UI thread)
    * @param [in] callback A callback function; refer to the OnResourceLoadCompleteEvent declaration
    */
    void AttachResourceLoadComplete(const OnResourceLoadCompleteEvent& callback) { m_pfnResourceLoadComplete = callback; }

    /** Binds a callback function to listen for an unknown Protocol (the callback is invoked on the CEF's UI thread)
    * @param[in] callback A callback function; refer to the OnProtocolExecutionEvent declaration
    */
    void AttachProtocolExecution(const OnProtocolExecutionEvent& callback) { m_pfnProtocolExecution = callback; }

    /** Binds a callback function to listen for changes to the page loading state (the callback is invoked on the main process's UI thread)
    * @param [in] callback A callback function; refer to the OnLoadingStateChangeEvent declaration
    */
    void AttachLoadingStateChange(const OnLoadingStateChangeEvent& callback){ m_pfnLoadingStateChange = callback; }

    /** Binds a callback function to listen for notifications that the page has started loading (the callback is invoked on the main process's UI thread)
    * @param [in] callback A callback function; refer to the OnLoadStartEvent declaration
    */
    void AttachLoadStart(const OnLoadStartEvent& callback){ m_pfnLoadStart = callback; }

    /** Binds a callback function to listen for notifications that the page has finished loading (the callback is invoked on the main process's UI thread)
    * @param [in] callback A callback function; refer to the OnLoadEndEvent declaration
    */
    void AttachLoadEnd(const OnLoadEndEvent& callback){ m_pfnLoadEnd = callback; }

    /** Binds a callback function to listen for page loading error notifications (the callback is invoked on the main process's UI thread)
    * @param [in] callback A callback function; refer to the OnLoadErrorEvent declaration
    */
    void AttachLoadError(const OnLoadErrorEvent& callback){ m_pfnLoadError = callback; }

    /** Binds a callback function to listen for developer tools state change notifications (the callback is invoked on the main process's UI thread)
    * @param [in] callback A callback function; refer to the OnDevToolAttachedStateChangeEvent declaration
    */
    void AttachDevToolAttachedStateChange(const OnDevToolAttachedStateChangeEvent& callback){ m_pfnDevToolVisibleChange = callback; };

    /** Binds a callback function to listen for notifications that a new browser instance has been created (the callback is invoked on the main process's UI thread)
    * @param [in] callback A callback function; refer to the OnAfterCreatedEvent declaration
    */
    void AttachAfterCreated(const OnAfterCreatedEvent& callback){ m_pfnAfterCreated = callback; }

    /** Binds a callback function to listen for notifications before a browser instance closes (the callback is invoked on the main process's UI thread)
    * @param [in] callback A callback function; refer to the OnBeforeCloseEvent declaration
    */
    void AttachBeforeClose(const OnBeforeCloseEvent& callback) { m_pfnBeforeClose = callback; }

    /** Binds a callback function to listen for browser instance loading notifications (the callback is invoked on the CEF's UI thread)
    * @param [in] callback A callback function; refer to the OnBeforeBrowseEvent declaration
    */
    void AttachBeforeBrowse(const OnBeforeBrowseEvent& callback) { m_pfnBeforeBrowse = callback; }

    /** Binds a callback function to listen for notifications of whether a download task can be executed (the callback is invoked on the CEF's UI thread)
    * @param [in] callback A callback function; refer to the OnCanDownloadEvent declaration
    */
    void AttachCanDownload(const OnCanDownloadEvent& callback) { m_pfnCanDownload = callback; }

    /** Binds a callback function to listen for notifications before a download task starts (the callback is invoked on the CEF's UI thread)
    * @param [in] callback A callback function; refer to the OnBeforeDownloadEvent declaration
    */
    void AttachBeforeDownload(const OnBeforeDownloadEvent& callback) { m_pfnBeforeDownload = callback; }

    /** Binds a callback function to listen for task state change notifications during download (the callback is invoked on the CEF's UI thread)
    * @param[in] callback A callback function; refer to the OnDownloadUpdatedEvent declaration
    */
    void AttachDownloadUpdated(const OnDownloadUpdatedEvent& callback) { m_pfnDownloadUpdated = callback; }

    /** Binds a callback function to listen for notifications of opening a file from a dialog (the callback is invoked on the CEF's UI thread)
    * @param[in] callback A callback function; refer to the OnFileDialogEvent declaration
    */
    void AttachFileDialog(const OnFileDialogEvent& callback) { m_pfnFileDialog = callback; }

    /** Binds a callback function to listen for notifications that the document in the main frame has finished loading (the callback is invoked on the main thread's UI thread)
    * @param[in] callback A callback function; refer to the OnDocumentAvailableInMainFrameEvent declaration
    */
    void AttachDocumentAvailableInMainFrame(const OnDocumentAvailableInMainFrameEvent& callback) { m_pfnDocumentAvailableInMainFrame = callback; }

    /** Binds a callback function to listen for notifications that the website icon download has completed (the callback is invoked on the main thread's UI thread)
    * @param[in] callback A callback function; refer to the OnDownloadFavIconFinishedEvent declaration
    */
    void AttachDownloadFavIconFinished(const OnDownloadFavIconFinishedEvent& callback) { m_pfnDownloadFavIconFinished = callback; }

public:
    /** Sets the callback interface for CEF control events
        (1) If this callback interface is set, the above AttachXXX event callback functions need not be used; either one can be used, but the two are mutually exclusive, with AttachXXX having higher priority)
        (2) For the callback functions in the interface, if the corresponding AttachXXX event callback function is also set, then AttachXXX has higher priority and the function corresponding to pCefControlEventHandler will not be called
    * @param [in] pCefControlEventHandler The callback interface for CEF control events
    */
    void SetCefEventHandler(CefControlEvent* pCefControlEventHandler);

    /** Gets the callback interface for CEF control events
    */
    CefControlEvent* GetCefEventHandler() const;

    /** Gets the CEF Browser object
    */
    CefRefPtr<CefBrowser> GetCefBrowser();

    /** Gets the CEF BrowserHost object
    */
    CefRefPtr<CefBrowserHost> GetCefBrowserHost();

    /** Re-downloads the website FavIcon
    */
    bool ReDownloadFavIcon();

public:
    /** Gets the control type
    */
    virtual DString GetType() const override;

    /** Sets the specified control attribute
     */
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;

protected:
    /** Re-creates the Browser object
    */
    virtual void ReCreateBrowser() = 0;

    /** Closes all Browser objects
    */
    void DoCloseAllBrowsers(bool bForceClose);

    /** Gets the drop target interface
    * @return Returns the drop target interface; if nullptr is returned, drag-and-drop is not supported
    */
    virtual ControlDropTarget_Windows* GetControlDropTarget() override;

protected:
    /** CefRenderHandler interface, called on a non-UI thread
    *   When the browser rendering data changes, this interface is triggered; the rendering data is then saved to the memory DC,
        and the window is notified to refresh the control. In the control's Paint function, the bitmap in the memory DC is drawn onto the window,
        thereby drawing the off-screen rendering data onto the window
    */
    virtual void OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type,
                         const CefRenderHandler::RectList& dirtyRects, const void* buffer,
                         int width, int height) override;//CefRenderHandler interface
    virtual void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) override;//CefRenderHandler interface
    virtual void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) override;//CefRenderHandler interface
    virtual void OnImeCompositionRangeChanged(CefRefPtr<CefBrowser> browser, const CefRange& selected_range, const std::vector<CefRect>& character_bounds) override;

    //CefContextMenuHandler interface, called on a non-UI thread
    virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) override;

    //CefContextMenuHandler interface, called on a non-UI thread
    virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params,
                                      int command_id, CefContextMenuHandler::EventFlags event_flags) override;
    //CefContextMenuHandler interface, called on a non-UI thread
    virtual void OnContextMenuDismissed(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) override;

    //CefDisplayHandler interface
    virtual void OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) override;
    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) override;
    virtual void OnFaviconURLChange(CefRefPtr<CefBrowser> browser, const std::vector<CefString>& icon_urls) override;
    virtual void OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool fullscreen) override;
    virtual void OnStatusMessage(CefRefPtr<CefBrowser> browser, const DString& value) override;
    virtual void OnLoadingProgressChange(CefRefPtr<CefBrowser> browser, double progress) override;
    virtual void OnMediaAccessChange(CefRefPtr<CefBrowser> browser, bool has_video_access, bool has_audio_access) override;

    //Implementation of the CefDragHandler interface
    virtual bool OnDragEnter(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDragData> dragData, CefDragHandler::DragOperationsMask mask) override;
    virtual void OnDraggableRegionsChanged(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const std::vector<CefDraggableRegion>& regions) override;

    //CefLoadHandler interface
    virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward) override;
    virtual void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, cef_transition_type_t transition_type) override;
    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) override;
    virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                             CefLoadHandler::ErrorCode errorCode,
                             const CefString& errorText,
                             const CefString& failedUrl) override;

    //CefLifeSpanHandler interface, called on a non-UI thread
    virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefFrame> frame,
                               int popup_id,//only exists in CEF 133 and later versions
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
    virtual void OnBeforePopupAborted(CefRefPtr<CefBrowser> browser, int popup_id) override;
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

    //CefRequestHandler interface, called on a non-UI thread
    virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool user_gesture, bool is_redirect) override;

    //CefResourceRequestHandler interface, called on a non-UI thread
    virtual cef_return_value_t OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
                                                    CefRefPtr<CefFrame> frame,
                                                    CefRefPtr<CefRequest> request,
                                                    CefRefPtr<CefCallback> callback) override;
    virtual void OnResourceRedirect(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefRequest> request,
                                    CefRefPtr<CefResponse> response,
                                    CefString& new_url) override;
    virtual bool OnResourceResponse(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefRequest> request,
                                    CefRefPtr<CefResponse> response) override;
    virtual void OnResourceLoadComplete(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame,
                                        CefRefPtr<CefRequest> request,
                                        CefRefPtr<CefResponse> response,
                                        cef_urlrequest_status_t status,
                                        int64_t received_content_length) override;
    virtual void OnProtocolExecution(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     CefRefPtr<CefRequest> request,
                                     bool& allow_os_execution) override;

    //CefRequestHandler interface
    virtual void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
                                           CefRequestHandler::TerminationStatus status,
                                           int error_code,
                                           CefString error_string) override;
    virtual void OnDocumentAvailableInMainFrame(CefRefPtr<CefBrowser> browser) override;

    //CefDownloadHandler interface, file download related
    virtual bool OnCanDownload(CefRefPtr<CefBrowser> browser,
                               const CefString& url,
                               const CefString& request_method) override;
    virtual bool OnBeforeDownload(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefDownloadItem> download_item,
                                  const CefString& suggested_name,
                                  CefRefPtr<CefBeforeDownloadCallback> callback) override;
    virtual void OnDownloadUpdated(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefDownloadItem> download_item,
                                   CefRefPtr<CefDownloadItemCallback> callback) override;

    //CefDialogHandler interface, open file Dialog
    virtual bool OnFileDialog(CefRefPtr<CefBrowser> browser,
                              CefBrowserHost::FileDialogMode mode,
                              const CefString& title,
                              const CefString& default_file_path,
                              const std::vector<CefString>& accept_filters,
                              const std::vector<CefString>& accept_extensions,
                              const std::vector<CefString>& accept_descriptions,
                              CefRefPtr<CefFileDialogCallback> callback) override;

    //CefKeyboardHandler interface
    virtual bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                               const CefKeyEvent& event,
                               CefEventHandle os_event,
                               bool* is_keyboard_shortcut) override;
    virtual bool OnKeyEvent(CefRefPtr<CefBrowser> browser,
                            const CefKeyEvent& event,
                            CefEventHandle os_event) override;

protected:
    /** Converts client-area coordinates to control coordinates
    */
    virtual void ClientToControl(UiPoint& pt) override;

    /** Updates the position of the CEF control window (child window mode)
    */
    virtual void UpdateCefWindowPos() override;

    /** Executes a C++ function (JS-C++ interaction)
    */
    virtual bool OnExecuteCppFunc(const CefString& function_name, const CefString& params, int js_callback_id, CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) override;

    /** Executes a C++ callback function (JS-C++ interaction)
    */
    virtual bool OnExecuteCppCallbackFunc(int cpp_callback_id, const CefString& json_string) override;

    /** Checks whether the following callback function exists (to reduce messages sent to the UI thread and improve efficiency)
    * @param [in] nCallbackID The ID of the callback function; see the enum type definition
    */
    virtual bool IsCallbackExists(CefCallbackID nCallbackID) override;

    /** The focused element has changed (called on the main thread)
    */
    virtual void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefDOMNode::Type type,
                                      bool bText,
                                      bool bEditable,
                                      const CefRect& nodeRect) override;

    /** Sets the cursor (only valid in off-screen rendering mode)
    */
    virtual void OnCursorChange(cef_cursor_type_t type) override;

    /** Gets the associated CEF control interface
    */
    virtual Control* GetCefControl() override;

    /// Method that will be executed when the image download has completed.
    /// |image_url| is the URL that was downloaded and |http_status_code| is the
    /// resulting HTTP status code. |image| is the resulting image, possibly at
    /// multiple scale factors, or empty if the download failed.
    ///
    /*--cef(optional_param=image)--*/
    friend class CefControlDownloadImageCallback;
    virtual void OnDownloadImageFinished(const CefString& image_url,
                                         int http_status_code,
                                         CefRefPtr<CefImage> image);

protected:
    /** Sets the developer tools attachment flag
    * @param [in] bAttachedDevTools Attach/detach the developer tools
    * @param [in] bPopup Whether the developer tools are in pop-up window mode
    */
    void SetAttachedDevTools(bool bAttachedDevTools, bool bPopup);

    /** The visibility of the developer tools has changed; triggers the interface callback (ensures the callback runs on the main process's UI thread)
    */
    void OnDevToolVisibleChanged(bool bAttachedDevTools, bool bPopup);

protected:
    /** The CEF browser object associated with the control
    */
    CefRefPtr<CefBrowserHandler> m_pBrowserHandler;

    /** JS functionality management interface
    */
    std::shared_ptr<CefJSBridge> m_jsBridge;

private:

    /** The initially loaded URL
    */
    UiString m_initUrl;

    /** Whether the initially loaded URL is a local file
    */
    bool m_bUrlIsLocalFile;

    /** The URL of the current main frame
    */
    UiString m_url;

    // Saves the thread that the code receiving JS calls to CPP functions belongs to; when a JS callback is triggered later, the callback is dispatched to that thread
    int32_t m_jsCallbackThreadId = -1;

    OnBeforeContextMenuEvent        m_pfnBeforeContextMenu = nullptr;
    OnContextMenuCommandEvent       m_pfnContextMenuCommand = nullptr;
    OnContextMenuDismissedEvent     m_pfnContextMenuDismissed = nullptr;
    OnTitleChangeEvent              m_pfnTitleChange = nullptr;
    OnBeforeResourceLoadEvent       m_pfnBeforeResourceLoad = nullptr;
    OnResourceRedirectEvent         m_pfnResourceRedirect = nullptr;
    OnResourceResponseEvent         m_pfnResourceResponse = nullptr;
    OnResourceLoadCompleteEvent     m_pfnResourceLoadComplete = nullptr;
    OnProtocolExecutionEvent        m_pfnProtocolExecution = nullptr;
    OnUrlChangeEvent                m_pfnUrlChange = nullptr;
    OnMainUrlChangeEvent            m_pfnMainUrlChange = nullptr;
    OnFaviconURLChangeEvent         m_pfnFaviconURLChange = nullptr;
    OnFullscreenModeChangeEvent     m_pfnFullscreenModeChange = nullptr;
    OnStatusMessageEvent            m_pfnStatusMessage = nullptr;
    OnLoadingProgressChangeEvent    m_pfnLoadingProgressChange = nullptr;
    OnMediaAccessChangeEvent        m_pfnMediaAccessChange = nullptr;
    OnBeforePopupEvent              m_pfnBeforePopup = nullptr;
    OnBeforePopupAbortedEvent       m_pfnBeforePopupAborted = nullptr;
    OnLoadingStateChangeEvent       m_pfnLoadingStateChange = nullptr;
    OnLoadStartEvent                m_pfnLoadStart = nullptr;
    OnLoadEndEvent                  m_pfnLoadEnd = nullptr;
    OnLoadErrorEvent                m_pfnLoadError = nullptr;
    OnAfterCreatedEvent             m_pfnAfterCreated = nullptr;
    OnBeforeCloseEvent              m_pfnBeforeClose = nullptr;
    OnBeforeBrowseEvent             m_pfnBeforeBrowse = nullptr;
    OnCanDownloadEvent              m_pfnCanDownload = nullptr;
    OnBeforeDownloadEvent           m_pfnBeforeDownload = nullptr;
    OnDownloadUpdatedEvent          m_pfnDownloadUpdated = nullptr;
    OnFileDialogEvent               m_pfnFileDialog = nullptr;
    OnDevToolAttachedStateChangeEvent   m_pfnDevToolVisibleChange = nullptr;
    OnDocumentAvailableInMainFrameEvent m_pfnDocumentAvailableInMainFrame = nullptr;
    OnDownloadFavIconFinishedEvent      m_pfnDownloadFavIconFinished = nullptr;
    OnDragEnterEvent                    m_pfnDragEnter = nullptr;
    OnDraggableRegionsChangedEvent      m_pfnDraggableRegionsChanged = nullptr;

    /** The event callback interface for CEF controls
    */
    CefControlEvent* m_pCefControlEventHandler = nullptr;

private:
    //Whether the developer tools are displayed
    bool m_bAttachedDevTools;

    //Whether the developer tools are in pop-up window mode
    bool m_bDevToolsPopup;

    //The control associated with the developer tools
    CefControl* m_pDevToolsView;

    //The lifecycle of the control associated with the developer tools
    std::weak_ptr<WeakFlag> m_pDevToolsViewFlag;

    //Whether the F12 shortcut key (developer tools) is allowed
    bool m_bEnableF12;

    //Whether the F11 shortcut key (page fullscreen/page exit fullscreen) is allowed
    bool m_bEnableF11;

    //Whether to download the website FavIcon
    bool m_bDownloadFaviconImage;

    //The URL of the website FavIcon
    CefString m_favIconUrl;
};
} //namespace ui

#endif //DUILIB_BUILD_FOR_CEF
#endif //UI_CEF_CONTROL_CEF_CONTROL_H_
