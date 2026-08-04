#ifndef UI_WEBVIEW2_WEBVIEW2_CONTROL_IMPL_H_
#define UI_WEBVIEW2_WEBVIEW2_CONTROL_IMPL_H_

#include "WebView2Control.h"

#if defined (DUI_BUILD_FOR_WIN) && defined (DUI_BUILD_FOR_WEBVIEW2)

#ifdef DUI_COMPILER_MINGW
    //Use "WebView2Loader.dll"
    #define DUI_USE_WEBVIEW2_LOADER_DLL 1
#endif

namespace ui {

class Control;

/** Implementation class of WebView2Control (PIMPL pattern)
    Contains the concrete implementation details of the WebView2 wrapper class, using a purely asynchronous initialization approach
 */
class WebView2Control::Impl
{
public:
    explicit Impl(Control* pControl);
    ~Impl();

    // Initialization and lifecycle management
    HRESULT InitializeAsync(const DString& userDataFolder, InitializeCompletedCallback callback);
    void Cleanup();

    // Navigation control
    HRESULT Navigate(const DString& url);
    HRESULT NavigateBack();
    HRESULT NavigateForward();
    HRESULT Refresh();
    HRESULT Stop();

    // JavaScript interaction
    HRESULT ExecuteScript(const DString& script, std::function<void(const DString& result, HRESULT hr)> callback);
    HRESULT PostWebMessageAsJson(const DString& json);
    HRESULT PostWebMessageAsString(const DString& message);

    // Settings related
    HRESULT SetUserAgent(const DString& userAgent);
    DString GetUserAgent() const;

    HRESULT SetZoomFactor(double zoomFactor);
    double GetZoomFactor() const;

    /** Sets whether JavaScript is enabled
    */
    HRESULT SetScriptEnabled(bool enabled);
    bool IsScriptEnabled() const;

    /** Sets whether web messages are enabled
    */
    HRESULT SetWebMessageEnabled(bool enabled);
    bool IsWebMessageEnabled() const;

    /** Whether default script dialogs are enabled
    */
    HRESULT SetAreDefaultScriptDialogsEnabled(bool enabled);
    bool AreDefaultScriptDialogsEnabled() const;

    /** Whether the default context menu is allowed
    */
    HRESULT SetAreDefaultContextMenusEnabled(bool enabled);
    bool AreDefaultContextMenusEnabled() const;

    /** Whether zoom control is disabled
    */
    HRESULT SetZoomControlEnabled(bool enabled);
    bool IsZoomControlEnabled() const;

    // Event registration
    HRESULT SetWebMessageReceivedCallback(WebMessageReceivedCallback callback);
    HRESULT SetNavigationStateChangedCallback(NavigationStateChangedCallback callback);
    HRESULT SetDocumentTitleChangedCallback(DocumentTitleChangedCallback callback);
    HRESULT SetSourceChangedCallback(SourceChangedCallback callback);
    HRESULT SetNewWindowRequestedCallback(NewWindowRequestedCallback callback);
    HRESULT SetHistoryChangedCallback(HistoryChangedCallback callback);
    HRESULT SetZoomFactorChangedCallback(ZoomFactorChangedCallback callback);
    void SetFavIconChangedCallback(FavIconChangedCallback callback);

    // Content management
    HRESULT CapturePreview(const DString& filePath,
                           std::function<void(const DString& filePath, HRESULT hr)> callback);

    // Accessors
    bool IsInitializing() const;
    bool IsInitialized() const;
    DString GetUrl() const;
    DString GetTitle() const;
    bool IsNavigating() const;
    bool CanGoBack() const;
    bool CanGoForward() const;

    // Window management
    void Resize(RECT rect);

    //Sets the error code of the last operation
    void SetLastErrorCode(HRESULT hr);

    //Gets the error code of the last operation
    HRESULT GetLastErrorCode() const;

    /** Sets the associated window
    */
    void SetWindow(Window* pWindow);

    /** Sets the visible property
    */
    void SetVisible(bool bVisible);

    /** Sets whether developer tools are enabled
    */
    void SetAreDevToolsEnabled(bool bAreDevToolsEnabled);

    /** Gets whether developer tools are enabled
    */
    bool AreDevToolsEnabled() const;

    /** Opens the developer tools
    */
    bool OpenDevToolsWindow();

    /** Downloads the favicon
    */
    bool DownloadFavIconImage();

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

public:
    /** Gets the ICoreWebView2Environment interface
    */
    ui::ComPtr<ICoreWebView2Environment> GetWebView2Environment() const { return m_spWebView2Environment; }

    /** Gets the ICoreWebView2Controller interface
    */
    ui::ComPtr<ICoreWebView2Controller> GetWebView2Controller() const { return m_spWebView2Controller; }

    /** Gets the ICoreWebView2 interface
    */
    ui::ComPtr<ICoreWebView2> GetWebView2() const { return m_spWebView2; }

private:

    // Initialization flow
    HRESULT CallCreateCoreWebView2EnvironmentWithOptions(PCWSTR browserExecutableFolder,
                                                         PCWSTR userDataFolder,
                                                         ICoreWebView2EnvironmentOptions* environmentOptions,
                                                         ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* environmentCreatedHandler);

    ui::ComPtr<ICoreWebView2EnvironmentOptions> CreateCoreWebView2EnvironmentOptionsObj();

    HRESULT CreateEnvironmentAsync();
    HRESULT CreateControllerAsync();
    void InitializeSettings();
    void InitializeFocusChanged();//Focus change event
    void InitializeAcceleratorKeyPressed();//Support for the F11/F12/ESC shortcut keys
    void OnInitializationCompleted(HRESULT result);

    // Add the new window callback function
    void AddNewWindowRequestedCallback();

    // Add the navigation state change event
    void AddNavigationStateChangedCallback();

private:
    // COM objects
    ui::ComPtr<ICoreWebView2Environment> m_spWebView2Environment;
    ui::ComPtr<ICoreWebView2Controller> m_spWebView2Controller;
    ui::ComPtr<ICoreWebView2> m_spWebView2;

    // Event tokens
    EventRegistrationToken m_webMessageReceivedToken = {0};
    EventRegistrationToken m_documentTitleChangedToken = {0};
    EventRegistrationToken m_navigationStartingToken = { 0 };
    EventRegistrationToken m_navigationCompletedToken = {0};
    EventRegistrationToken m_sourceChangedToken = {0};
    EventRegistrationToken m_newWindowRequestedToken = {0};
    EventRegistrationToken m_historyChangedToken = {0};
    EventRegistrationToken m_zoomFactorChangedToken = { 0 };
    EventRegistrationToken m_faviconChangedToken = { 0 };
    EventRegistrationToken m_gotFocusToken = { 0 };

    // Event token, used internally
    EventRegistrationToken m_myAcceleratorKeyPressedToken = { 0 };

    // Callback functions
    InitializeCompletedCallback m_initializeCompletedCallback = nullptr;
    WebMessageReceivedCallback m_webMessageReceivedCallback = nullptr;
    NavigationStateChangedCallback m_navigationStateChangedCallback = nullptr;
    DocumentTitleChangedCallback m_documentTitleChangedCallback = nullptr;
    SourceChangedCallback m_sourceChangedCallback = nullptr;
    NewWindowRequestedCallback m_newWindowRequestedCallback = nullptr;
    HistoryChangedCallback m_historyChangedCallback = nullptr;
    ZoomFactorChangedCallback m_zoomFactorChangedCallback = nullptr;
    FavIconChangedCallback m_favIconChangedCallback = nullptr;

    // The initial URL
    DString m_navigateUrl;

    // The URL of the website icon (favicon)
    DStringW m_favIconImageUrl;

    // State variables
    Control* m_pControl;
    DString m_userDataFolder;
    DString m_userAgent;

    HRESULT m_lastError;
    bool m_bInitializing;
    bool m_bInitialized;
    bool m_bNavigating;

    // Whether developer tools are enabled
    bool m_bAreDevToolsEnabled;
    bool m_bAreDevToolsEnabledSet;

    // Whether the context menu is enabled
    bool m_bAreDefaultContextMenusEnabled;
    bool m_bAreDefaultContextMenusEnabledSet;

    // Whether JS dialogs are allowed
    bool m_bAreDefaultScriptDialogsEnabled;
    bool m_bAreDefaultScriptDialogsEnabledSet;

    // Whether web messages are allowed
    bool m_bWebMessageEnabled;
    bool m_bWebMessageEnabledSet;

    // Whether JS is allowed
    bool m_bScriptEnabled;
    bool m_bScriptEnabledSet;

    // Whether page zoom is allowed
    bool m_bZoomControlEnabled;
    bool m_bZoomControlEnabledSet;

    // Whether the F12 shortcut key is allowed (developer tools)
    bool m_bEnableF12;

    // Whether the F11 shortcut key is allowed (page fullscreen / exit page fullscreen)
    bool m_bEnableF11;

#ifdef DUI_USE_WEBVIEW2_LOADER_DLL
private:
    /** The handle of WebView2Loader.dll
    */
    HMODULE m_hWebView2Loader = nullptr;
#endif
};

} //namespace ui

#endif //DUI_BUILD_FOR_WEBVIEW2

#endif //UI_WEBVIEW2_WEBVIEW2_CONTROL_IMPL_H_
