#ifndef UI_WEBVIEW2_WEBVIEW2_MANAGER_H_
#define UI_WEBVIEW2_WEBVIEW2_MANAGER_H_

#include "duilib/Core/UiTypes.h"

#if defined (DUILIB_BUILD_FOR_WIN) && defined (DUILIB_BUILD_FOR_WEBVIEW2)

namespace ui {

/** Public configuration management for WebView2 controls
 */
class DUILIB_API WebView2Manager
{
public:
    WebView2Manager();
    ~WebView2Manager();
    WebView2Manager(const WebView2Manager&) = delete;
    WebView2Manager& operator=(const WebView2Manager&) = delete;

public:
    /** The singleton object
    */
    static WebView2Manager& GetInstance();

    /** Initialize
    * @param [in] userDataFolder The web page cache directory
    * @param [in] language The language, such as "zh-CN", "en-US", etc.
    * @param [in] userAgent The UserAgent string
    * @param [in] additionalBrowserArguments Additional browser arguments
    * @param [in] browserExecutableFolder The directory where the WebView2 module is located
    */
    bool Initialize(const DString& userDataFolder = _T(""),
                    const DString& language = _T(""),
                    const DString& userAgent = _T(""),
                    const DString& additionalBrowserArguments = _T(""),
                    const DString& browserExecutableFolder = _T(""));

    /** Clean up
    */
    void UnInitialize();

    /** Get the default web page cache directory
    * @param [in] appName The application name
    */
    DString GetDefaultUserDataFolder(const DString& appName) const;

public:
    /** The directory where the WebView2 module is located
    */
    void SetBrowserExecutableFolder(const DString& browserExecutableFolder);
    const DString& GetBrowserExecutableFolder() const;

    /** The web page cache directory
    */
    void SetUserDataFolder(const DString& userDataFolder);
    const DString& GetUserDataFolder() const;

    /** The language
    */
    void SetLanguage(const DString& language);
    const DString& GetLanguage() const;

    /** The UserAgent
    */
    void SetUserAgent(const DString& userAgent);
    const DString& GetUserAgent() const;

    /** Additional arguments
    */
    void SetAdditionalBrowserArguments(const DString& additionalBrowserArguments);
    const DString& GetAdditionalBrowserArguments() const;

    /** Whether JavaScript is enabled
    */
    void SetScriptEnabled(bool bScriptEnabled);
    bool IsScriptEnabled() const;

    /** Whether Web messages are enabled
    */
    void SetWebMessageEnabled(bool bWebMessageEnabled);
    bool IsWebMessageEnabled() const;

    /** Whether default script dialogs are enabled
    */
    void SetAreDefaultScriptDialogsEnabled(bool bAreDefaultScriptDialogsEnabled);
    bool AreDefaultScriptDialogsEnabled() const;

    /** Whether the default context menu is allowed
    */
    void SetAreDefaultContextMenusEnabled(bool bAreDefaultContextMenusEnabled);
    bool AreDefaultContextMenusEnabled() const;

    /** Whether zoom controls are disabled
    */
    void SetZoomControlEnabled(bool bZoomControlEnabled);
    bool IsZoomControlEnabled() const;

    /** Whether developer tools are enabled
    */
    void SetAreDevToolsEnabled(bool bAreDevToolsEnabled);
    bool AreDevToolsEnabled() const;

public:
    /** Set whether the F12 shortcut key is allowed (developer tools)
    */
    void SetEnableF12(bool bEnableF12);

    /** Whether the F12 shortcut key is allowed (developer tools)
    */
    bool IsEnableF12() const;

    /** Set whether the F11 shortcut key is allowed (fullscreen / exit fullscreen)
    */
    void SetEnableF11(bool bEnableF11);

    /** Whether the F11 shortcut key is allowed (page fullscreen / exit page fullscreen)
    */
    bool IsEnableF11() const;

private:
    /** The directory where the WebView2 module is located
    */
    DString m_browserExecutableFolder;

    /** The web page cache directory
    */
    DString m_userDataFolder;

    /** The language
    */
    DString m_language;

    /** The UserAgent
    */
    DString m_userAgent;

    /** Additional arguments
    */
    DString m_additionalBrowserArguments;

    /** Whether JavaScript is enabled
    */
    bool m_bScriptEnabled;

    /** Whether Web messages are enabled
    */
    bool m_bWebMessageEnabled;
    
    /** Whether default script dialogs are enabled
    */
    bool m_bAreDefaultScriptDialogsEnabled;

    /** Whether the default context menu is allowed
    */
    bool m_bAreDefaultContextMenusEnabled;

    /** Whether zoom controls are disabled
    */
    bool m_bZoomControlEnabled;

    /** Whether developer tools are enabled
    */
    bool m_bAreDevToolsEnabled;

    /** Whether the F12 shortcut key is allowed (developer tools)
    */
    bool m_bEnableF12;

    /** Whether the F11 shortcut key is allowed (page fullscreen / exit page fullscreen)
    */
    bool m_bEnableF11;
};

} //namespace ui

#endif //DUILIB_BUILD_FOR_WEBVIEW2

#endif //UI_WEBVIEW2_WEBVIEW2_MANAGER_H_
