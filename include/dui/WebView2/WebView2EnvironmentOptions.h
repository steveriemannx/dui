#ifndef UI_WEBVIEW2_ENVIRONMENT_OPTIONS_H_
#define UI_WEBVIEW2_ENVIRONMENT_OPTIONS_H_

#include "dui/dui_defs.h"

#if defined (DUI_BUILD_FOR_WIN) && defined (DUI_BUILD_FOR_WEBVIEW2)

#include <combaseapi.h>
#include "third_party/Microsoft.Web.WebView2/build/native/include/WebView2.h"

#include <vector>

// Forward declaration
interface ICoreWebView2CustomSchemeRegistration;

// The version of WebView2: note that when updating WebView2.h, you need to check whether this value needs to be updated
#define CORE_WEBVIEW_TARGET_PRODUCT_VERSION L"137.0.3296.44"

namespace ui
{
class DUI_API WebView2EnvironmentOptions :
    public ICoreWebView2EnvironmentOptions,
    public ICoreWebView2EnvironmentOptions2,
    public ICoreWebView2EnvironmentOptions3,
    public ICoreWebView2EnvironmentOptions4,
    public ICoreWebView2EnvironmentOptions5,
    public ICoreWebView2EnvironmentOptions6,
    public ICoreWebView2EnvironmentOptions7,
    public ICoreWebView2EnvironmentOptions8
{
public:
    // Constructor and destructor
    WebView2EnvironmentOptions();
    virtual ~WebView2EnvironmentOptions();

    // IUnknown implementation
    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppvObject) override;
    IFACEMETHODIMP_(ULONG) AddRef() override;
    IFACEMETHODIMP_(ULONG) Release() override;

    // ICoreWebView2EnvironmentOptions implementation
    IFACEMETHODIMP get_AdditionalBrowserArguments(LPWSTR* value) override;
    IFACEMETHODIMP put_AdditionalBrowserArguments(LPCWSTR value) override;
    IFACEMETHODIMP get_Language(LPWSTR* value) override;
    IFACEMETHODIMP put_Language(LPCWSTR value) override;
    IFACEMETHODIMP get_TargetCompatibleBrowserVersion(LPWSTR* value) override;
    IFACEMETHODIMP put_TargetCompatibleBrowserVersion(LPCWSTR value) override;
    IFACEMETHODIMP get_AllowSingleSignOnUsingOSPrimaryAccount(BOOL* allow) override;
    IFACEMETHODIMP put_AllowSingleSignOnUsingOSPrimaryAccount(BOOL allow) override;

    // ICoreWebView2EnvironmentOptions2 implementation
    IFACEMETHODIMP get_ExclusiveUserDataFolderAccess(BOOL* value) override;
    IFACEMETHODIMP put_ExclusiveUserDataFolderAccess(BOOL value) override;

    // ICoreWebView2EnvironmentOptions3 implementation
    IFACEMETHODIMP get_IsCustomCrashReportingEnabled(BOOL* value) override;
    IFACEMETHODIMP put_IsCustomCrashReportingEnabled(BOOL value) override;

    // ICoreWebView2EnvironmentOptions4 implementation
    IFACEMETHODIMP GetCustomSchemeRegistrations(UINT32* count, ICoreWebView2CustomSchemeRegistration*** schemeRegistrations) override;
    IFACEMETHODIMP SetCustomSchemeRegistrations(UINT32 count, ICoreWebView2CustomSchemeRegistration** schemeRegistrations) override;

    // ICoreWebView2EnvironmentOptions5 implementation
    IFACEMETHODIMP get_EnableTrackingPrevention(BOOL* value) override;
    IFACEMETHODIMP put_EnableTrackingPrevention(BOOL value) override;

    // ICoreWebView2EnvironmentOptions6 implementation
    IFACEMETHODIMP get_AreBrowserExtensionsEnabled(BOOL* value) override;
    IFACEMETHODIMP put_AreBrowserExtensionsEnabled(BOOL value) override;

    // ICoreWebView2EnvironmentOptions7 implementation
    IFACEMETHODIMP get_ChannelSearchKind(COREWEBVIEW2_CHANNEL_SEARCH_KIND* value) override;
    IFACEMETHODIMP put_ChannelSearchKind(COREWEBVIEW2_CHANNEL_SEARCH_KIND value) override;
    IFACEMETHODIMP get_ReleaseChannels(COREWEBVIEW2_RELEASE_CHANNELS* value) override;
    IFACEMETHODIMP put_ReleaseChannels(COREWEBVIEW2_RELEASE_CHANNELS value) override;

    // ICoreWebView2EnvironmentOptions8 implementation
    IFACEMETHODIMP get_ScrollBarStyle(COREWEBVIEW2_SCROLLBAR_STYLE* value) override;
    IFACEMETHODIMP put_ScrollBarStyle(COREWEBVIEW2_SCROLLBAR_STYLE value) override;

    // Static method for creating instances
    static HRESULT CreateInstance(ICoreWebView2EnvironmentOptions** ppOptions);

private:
    LONG m_refCount;

    // ICoreWebView2EnvironmentOptions properties
    std::wstring m_additionalBrowserArguments;
    std::wstring m_language;
    std::wstring m_targetCompatibleBrowserVersion;
    BOOL m_allowSingleSignOn;

    // ICoreWebView2EnvironmentOptions2 properties
    BOOL m_exclusiveUserDataFolderAccess;

    // ICoreWebView2EnvironmentOptions3 properties
    BOOL m_isCustomCrashReportingEnabled;

    // ICoreWebView2EnvironmentOptions4 properties
    std::vector<ICoreWebView2CustomSchemeRegistration*> m_customSchemeRegistrations;

    // ICoreWebView2EnvironmentOptions5 properties
    BOOL m_enableTrackingPrevention;

    // ICoreWebView2EnvironmentOptions6 properties
    BOOL m_areBrowserExtensionsEnabled;

    // ICoreWebView2EnvironmentOptions7 properties
    COREWEBVIEW2_CHANNEL_SEARCH_KIND m_channelSearchKind;
    COREWEBVIEW2_RELEASE_CHANNELS m_releaseChannels;

    // ICoreWebView2EnvironmentOptions8 properties
    COREWEBVIEW2_SCROLLBAR_STYLE m_scrollBarStyle;
};

} //namespace ui

#endif //defined (DUI_BUILD_FOR_WIN) && defined (DUI_BUILD_FOR_WEBVIEW2)

#endif //UI_WEBVIEW2_ENVIRONMENT_OPTIONS_H_
