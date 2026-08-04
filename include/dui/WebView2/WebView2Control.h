#ifndef UI_WEBVIEW2_WEBVIEW2_CONTROL_H_
#define UI_WEBVIEW2_WEBVIEW2_CONTROL_H_

#include "dui/Core/Control.h"

#if defined (DUI_BUILD_FOR_WIN) && defined (DUI_BUILD_FOR_WEBVIEW2)
#include "ComPtr.h"

#include <combaseapi.h>
#include "third_party/Microsoft.Web.WebView2/build/native/include/WebView2.h"

#include <functional>
#include <memory>

namespace ui {

class Window;
class IBitmap;

/** C++ wrapper class for the WebView2 control
 */
class DUI_API WebView2Control: public Control
{
    typedef Control BaseClass;
public:
    /** Navigation state enum, representing the navigation state of WebView2
     */
    enum class NavigationState
    {
        Started,    /**< Navigation started */
        Completed,  /**< Navigation completed */
        Failed      /**< Navigation failed */
    };

    /** Initialization completed callback type
     * @param result Initialization result (S_OK indicates success)
     */
    using InitializeCompletedCallback = std::function<void(HRESULT result)>;

    /** Web message received callback type
     * @param message Received message content
     */
    using WebMessageReceivedCallback = std::function<void(const DString& url,
                                                          const DString& webMessageAsJson, 
                                                          const DString& webMessageAsString)>;

    /** Navigation state changed callback type
     * @param state The new navigation state
     * @param errorCode Error code (if navigation failed)
     */
    using NavigationStateChangedCallback = std::function<void(NavigationState state, HRESULT errorCode)>;

    /** Document title changed callback type
     * @param title The new document title
     */
    using DocumentTitleChangedCallback = std::function<void(const DString& title)>;

    /** Source URL changed callback type
     * @param uri The new source URL
     */
    using SourceChangedCallback = std::function<void(const DString& uri)>;

    /** New window requested callback type
     * @param sourceUrl URL of the source
     * @param sourceFrame Name of the source frame
     * @param targetUrl The requested URL
     * @param targetFrame Name of the target frame
     * @param bUserInitiated Whether the popup was triggered by the user
     * @return Returns true to allow creating a popup page, but the new page navigates within the current page and no new window is popped up; returns false to block the popup page, and the callback handles the display logic of the new page
     */
    using NewWindowRequestedCallback = std::function<bool(const DString& sourceUrl, const DString& sourceFrame,
                                                          const DString& targetUrl, const DString& targetFrame,
                                                          bool bUserInitiated)>;

    /** Navigation history changed event callback type
     */
    using HistoryChangedCallback = std::function<void()>;

    /** Page zoom factor changed event callback type
    * @param [in] zoomFactor The current zoom factor
    */
    using ZoomFactorChangedCallback = std::function<void(double zoomFactor)>;

    /** Favicon changed event callback type
    * @param [in] nWidth Image width
    * @param [in] nHeight Image height
    * @param [in] imageData Image data of the favicon
    */
    using FavIconChangedCallback = std::function<void(int32_t nWidth, int32_t nHeight, const std::vector<uint8_t>& imageData)>;

public:
    /** Constructor
     */
    explicit WebView2Control(Window* pWindow);

    /** Destructor
     */
    virtual ~WebView2Control() override;

    /** Asynchronously initializes the WebView
     * @param userDataFolder User data folder path (optional)
     * @param callback Initialization completed callback (optional)
     */
    bool InitializeAsync(const DString& userDataFolder = _T(""),
                         InitializeCompletedCallback callback = nullptr);

    /** Checks whether the WebView is initializing
     * @return Whether it is initializing
     */
    bool IsInitializing() const;

    /** Checks whether the WebView has completed initialization
     * @return Whether it is initialized
     */
    bool IsInitialized() const;
    
    /** Navigates to the specified URL (may complete asynchronously)
     * @param url The URL to navigate to
     */
    bool Navigate(const DString& url);
    
    /** Navigates to the previous page
     */
    bool NavigateBack();
    
    /** Navigates to the next page
     */
    bool NavigateForward();
    
    /** Refreshes the current page
     */
    bool Refresh();
    
    /** Stops loading
     * @return HRESULT error code
     */
    bool Stop();
    
    /** Executes a JavaScript script
     * @param script The JavaScript script to execute
     * @param callback Callback invoked after execution completes (optional)
     */
    bool ExecuteScript(const DString& script, std::function<void(const DString& result, HRESULT hr)> callback = nullptr);
    
    /** Posts a web message in JSON format
     * @param json The JSON string to send
     */
    bool PostWebMessageAsJson(const DString& json);
    
    /** Posts a web message as a string
     * @param message The message string to send
     */
    bool PostWebMessageAsString(const DString& message);
    
    /** Sets the User-Agent
     * @param userAgent The User-Agent string to set
     */
    bool SetUserAgent(const DString& userAgent);

    /** Gets the User-Agent
    */
    DString GetUserAgent() const;
    
    /** Sets the zoom factor
     * @param zoomFactor The zoom factor
     */
    bool SetZoomFactor(double zoomFactor);

    /** Gets the zoom factor
    */
    double GetZoomFactor() const;
    
    /** Sets whether JavaScript is enabled
    */
    bool SetScriptEnabled(bool enabled);
    bool IsScriptEnabled() const;
    
    /** Sets whether web messages are enabled
    */
    bool SetWebMessageEnabled(bool enabled);
    bool IsWebMessageEnabled() const;

    /** Whether default script dialogs are enabled
    */
    bool SetAreDefaultScriptDialogsEnabled(bool enabled);
    bool AreDefaultScriptDialogsEnabled() const;

    /** Whether the default context menu is allowed
    */
    bool SetAreDefaultContextMenusEnabled(bool enabled);
    bool AreDefaultContextMenusEnabled() const;

    /** Whether zoom control is disabled
    */
    bool SetZoomControlEnabled(bool enabled);
    bool IsZoomControlEnabled() const;
    
    /** Sets the web message received callback
     * @param callback The callback function
     */
    bool SetWebMessageReceivedCallback(WebMessageReceivedCallback callback);
    
    /** Sets the navigation state changed callback
     * @param callback The callback function
     */
    bool SetNavigationStateChangedCallback(NavigationStateChangedCallback callback);
    
    /** Sets the document title changed callback
     * @param callback The callback function
     */
    bool SetDocumentTitleChangedCallback(DocumentTitleChangedCallback callback);
    
    /** Sets the source URL changed callback
     * @param callback The callback function
     */
    bool SetSourceChangedCallback(SourceChangedCallback callback);   
  
    /** Sets the new window requested callback
     * @param callback The callback function
     */
    bool SetNewWindowRequestedCallback(NewWindowRequestedCallback callback);

    /** Sets the navigation history changed callback
     * @param callback The callback function
     */
    bool SetHistoryChangedCallback(HistoryChangedCallback callback);

    /** Sets the page zoom factor changed event callback
     */
    bool SetZoomFactorChangedCallback(ZoomFactorChangedCallback callback);

    /** Sets the favicon changed event callback type
    */
    void SetFavIconChangedCallback(FavIconChangedCallback callback);
       
    /** Captures a preview image of the current page (asynchronous), saved as PNG or JPG
     * @param filePath File path to save the preview image; the format is determined automatically by the extension of the saved image file name
     * @param callback Operation completed callback (optional)
     */
    bool CapturePreview(const DString& filePath,
                        std::function<void(const DString& filePath, HRESULT hr)> callback = nullptr);
    
    /** Gets the current URL
     * @return The current URL
     */
    DString GetUrl() const;
    
    /** Gets the current document title
     * @return The current document title
     */
    DString GetTitle() const;

    /** Whether navigation is in progress
    */
    bool IsNavigating() const;
    
    /** Checks whether it can navigate to the previous page
     * @return Whether it can navigate to the previous page
     */
    bool CanGoBack() const;
    
    /** Checks whether it can navigate to the next page
     * @return Whether it can navigate to the next page
     */
    bool CanGoForward() const;

    /** Gets the error code of the last operation
    */
    HRESULT GetLastErrorCode() const;

public:
    /** Sets whether developer tools are enabled
    */
    void SetAreDevToolsEnabled(bool bAreDevToolsEnabled);

    /** Gets whether developer tools are enabled
    */
    bool AreDevToolsEnabled() const;

    /** Opens the developer tools
    */
    bool OpenDevToolsWindow();

public:
    /** Gets the ICoreWebView2Environment interface
    */
    ui::ComPtr<ICoreWebView2Environment> GetWebView2Environment() const;

    /** Gets the ICoreWebView2Controller interface
    */
    ui::ComPtr<ICoreWebView2Controller> GetWebView2Controller() const;

    /** Gets the ICoreWebView2 interface
    */
    ui::ComPtr<ICoreWebView2> GetWebView2() const;

public:
    // Properties related to the control type
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;
    virtual void OnInit() override;
    virtual void SetPos(UiRect rc) override;
    virtual bool OnSetFocus(const EventArgs& msg) override;
    virtual bool OnKillFocus(const EventArgs& msg) override;
    virtual void SetWindow(Window* pWindow) override;

    /** Sets whether the F12 shortcut key is allowed (developer tools)
    */
    void SetEnableF12(bool bEnableF12);

    /** Whether the F12 shortcut key is allowed (developer tools)
    */
    bool IsEnableF12() const;

    /** Sets whether the F11 shortcut key is allowed (enter fullscreen page / exit fullscreen page)
    */
    void SetEnableF11(bool bEnableF11);

    /** Whether the F11 shortcut key is allowed (enter fullscreen page / exit fullscreen page)
    */
    bool IsEnableF11() const;

    /** Sets the URL to load initially (only effective when called before the control is initialized)
    */
    void SetInitURL(const DString& url);

    /** Gets the URL to load initially
    */
    DString GetInitURL() const;

    /** Sets whether the initially loaded URL is a local file
    */
    void SetInitUrlIsLocalFile(bool bUrlIsLocalFile);

    /** Gets whether the initially loaded URL is a local file
    */
    bool IsInitUrlIsLocalFile() const;

    /** Downloads the favicon (prerequisite: SetFavIconChangedCallback must have been called to set the callback)
    */
    bool DownloadFavIconImage();

    /** Saves the web page as an image, with the same size as the control
    */
    std::shared_ptr<IBitmap> MakeImageSnapshot();

protected:
    /** Sets the visible state event
    * @param [in] bChanged true means the state has changed, false means the state has not changed
    */
    virtual void OnSetVisible(bool bChanged) override;

private:
    // PIMPL implementation
    class Impl;
    std::unique_ptr<Impl> m_pImpl;

    /** URL loaded at initialization
    */
    UiString m_initUrl;

    /** Whether the initially loaded URL is a local file
    */
    bool m_bUrlIsLocalFile;
};

} //namespace ui

#endif //DUI_BUILD_FOR_WEBVIEW2

#endif //UI_WEBVIEW2_WEBVIEW2_CONTROL_H_
