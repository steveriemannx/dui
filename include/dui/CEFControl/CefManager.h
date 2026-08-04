#ifndef UI_CEF_CONTROL_CEF_MANAGER_H_
#define UI_CEF_CONTROL_CEF_MANAGER_H_

#include "dui/Core/Callback.h"

#ifdef DUI_BUILD_FOR_CEF

#pragma warning (push)
#pragma warning (disable:4100)
    #include "include/cef_app.h"
    #include "include/cef_version.h"
#pragma warning (pop)

#include <vector>
#include <atomic>
#include <chrono>

namespace ui
{
/** Callback for singleton control of the Browser process (used only on Windows with CEF 109; not used in other cases)
 * (1) Since one Browser process needs to start many Renderer child processes, the resource overhead is high, so it is necessary to ensure that only one Browser process exists
 * (2) CEF 133 and later have built-in singleton control; just implementing the interface is enough. CEF 109 has no such feature and allows multiple Browser processes to coexist
 * @param [in] argumentList Launch argument list of the relaunched Browser process
 */
typedef std::function<void (const std::vector<DString>& argumentList)> OnAlreadyRunningAppRelaunchEvent;

/** Callback to set the CEF initialization parameters; the parameters can be modified in the callback function
 * @param [in] settings Initialization parameters of the CEF module
 */
typedef std::function<void (CefSettings& settings)> OnCefSettingsEvent;

//Window type
class Window;

/** Manages the initialization, destruction, and message loop of the Cef component
 * @copyright (c) 2016, NetEase Inc. All rights reserved
 * @author Redrain
 * @date 2016/7/19
 */
class DUI_API CefManager : public ui::SupportWeakCallback
{
public:
    CefManager();
    CefManager(const CefManager&) = delete;
    CefManager& operator=(const CefManager&) = delete;
protected:
    virtual ~CefManager();

public:
    /** Singleton object
    */
    static CefManager* GetInstance();

public:
    /** Set the CEF web page cache directory (if not set, the default rule directory is used)
    *   It can be a relative path. If a relative directory is set, the corresponding relative directory will be created in the current program directory
    *   Default rule: "${program directory}/cef_cache/${program name}"
    */
    virtual void SetCefCachePath(const DString& cefCachePath);

    /** Get the current CEF web page cache directory
    */
    virtual DString GetCefCachePath() const;

    /** Set the path of the CEF module runtime library file (libcef.dll) (Windows only; if not set, the default rule directory is used)
    *   It can be a relative path. If a relative directory is set, the corresponding relative directory will be matched in the current program directory
    *   Default rule:
    *       Windows x64: "libcef_win\\x64"
    *       Windows x86: "libcef_win\\Win32"
    *       Windows x64: "libcef_win_109\\x64" (CEF 109)
    *       Windows x86: "libcef_win_109\\Win32" (CEF 109)
    */
    virtual void SetCefMoudlePath(const DString& cefMoudlePath);

    /** Get the path of the CEF module runtime library file
    */
    virtual DString GetCefMoudlePath() const;

    /** Set the default language; if not set, the default is Simplified Chinese ("zh-CN")
    * @param [in] lang Language string, e.g. "en-US" for English
    */
    virtual void SetCefLanguage(const DString& lang);

    /** Get the default language
    */
    virtual DString GetCefLanguage() const;

    /** Set the CEF log severity level
    */
    virtual void SetLogSeverity(cef_log_severity_t log_severity);

    /** Get the CEF log severity level
    */
    virtual cef_log_severity_t GetLogSeverity() const;

    /** Initialize the runtime environment; must be called before using CEF module features
    */
    virtual bool InitEnv();

    /** Initialize the cef component
    * @param [in] bEnableOffScreenRendering Whether to enable off-screen rendering
    * @param [in] appName Product name identifier (required parameter, must not be empty; used to generate the CEF cache path and control process uniqueness)
    * @param [in] argc Number of arguments at program startup (used only on Linux; pass 0 on other platforms)
    * @param [in] argv Argument list at program startup (used only on Linux; pass nullptr on other platforms)
    * @param [in] callback Callback function for setting CEF module parameters, used by the application layer to modify initialization parameters
    * @param [in] nExitCode Exit code of the process when the function returns false
    * @return bool true to continue running, false to end the program
    */
    virtual bool Initialize(bool bEnableOffScreenRendering,
                            const DString& appName,
                            int argc,
                            char** argv,
                            OnCefSettingsEvent callback,
                            int32_t& nExitCode);

    /** Clean up the cef component
    */
    virtual void UnInitialize();

    /** Whether CEF is currently running in multi-threaded message loop mode (supported on Windows/Linux, but not on MacOS)
    */
    virtual bool IsMultiThreadedMessageLoop() const;

    /** Whether it has been initialized
    */
    bool IsCefInited() const;

    /** Start a timer to call the CEF message processing function: CefDoMessageLoopWork (enabled only when IsMultiThreadedMessageLoop() is false)
    *   If the CEF message loop is not enabled, a fallback timer must be started (without the fallback, behavior is abnormal; the OnScheduleMessagePumpWork function logic does not work as expected)
    */
    void ScheduleCefDoMessageLoopWork();

    /** Set the interval (milliseconds) of the timer that calls the CEF message processing function
    */
    void SetCefDoMessageLoopWorkDelayMs(int32_t nCefDoMessageLoopWorkDelayMs);

    /** Get the interval (milliseconds) of the timer that calls the CEF message processing function
    */
    int32_t GetCefDoMessageLoopWorkDelayMs() const;

public:
    /** Bind a callback function to listen for the Browser process launch event (used only on Windows + CEF 109; no need to set in other cases)
    * @param [in] callback A callback function; see the OnAlreadyRunningAppRelaunchEvent declaration
    */
    virtual void SetAlreadyRunningAppRelaunch(const OnAlreadyRunningAppRelaunchEvent& callback);

    /** Get the callback function that listens for the Browser process launch event
    */
    virtual OnAlreadyRunningAppRelaunchEvent GetAlreadyRunningAppRelaunch() const;

public:
    /** Whether off-screen rendering is enabled (cannot be modified after initialization)
    * @return bool true enabled, false disabled
    */
    bool IsEnableOffScreenRendering() const;

    /** Record the number of browser objects (increment by 1)
    */
    void AddBrowserCount();

    /** Record the number of browser objects (decrement by 1)
    */
    void SubBrowserCount();

    /** Get the number of browser objects
    */
    int32_t GetBrowserCount();

    /** When a window is closing, handle all Browser controls under that window and exit before the window closes
    */
    void ProcessWindowCloseEvent(Window* pWindow);

    /** Send a QUIT message after the Cef browser objects are destroyed to exit the main process message loop
    */
    void PostQuitMessage(int32_t nExitCode);

public:
    /** Add extra startup arguments for the CEF module (see the CEF header: CefCommandLine::AppendSwitchWithValue function)
    *   This function must be called before CefManager::Initialize
    * @param [in] name Name of the argument
    * @param [in] value Value of the argument
    */
    void AppendSwitchWithValue(const DString& name, const DString& value);

    /** Get the extra startup arguments of the CEF module
    */
    const std::vector<std::pair<DString, DString>>& GetSwitchWithValues() const;

public:
    // CEF process type
    enum ProcessType
    {
        BrowserProcess,
        RendererProcess,
        ZygoteProcess,
        OtherProcess,
    };

    /** Parse the CEF process type from the command line
    */
    static ProcessType GetProcessType(CefRefPtr<CefCommandLine> commandLine);

public:
    /** Set whether the F12 shortcut key (developer tools) is allowed
    */
    void SetEnableF12(bool bEnableF12);

    /** Whether the F12 shortcut key (developer tools) is allowed
    */
    bool IsEnableF12() const;

    /** Set whether the F11 shortcut key (fullscreen/exit fullscreen) is allowed
    */
    void SetEnableF11(bool bEnableF11);

    /** Whether the F11 shortcut key (page fullscreen/exit page fullscreen) is allowed
    */
    bool IsEnableF11() const;

protected:
    /** Generate CEF configuration data
    */
    virtual void GetCefSetting(CefSettings& settings);

private:
    /** CEF web page cache directory (if not set, the default rule directory is used)
    *   Default rule: "${program directory}/cef_cache/${program name}"
    */
    DString m_cefCachePath;

    /** Path of the CEF module runtime library file (libcef.dll) (Windows only; if not set, the default rule directory is used)
    */
    DString m_cefMoudlePath;

    /** Application name
    */
    DString m_appName;

    /** Default CEF language
    */
    DString m_lang;

    /** Callback function for CEF configuration
    */
    OnCefSettingsEvent m_cefSettingCallback;

    /** Extra startup arguments of the CEF module
    */
    std::vector<std::pair<DString, DString>> m_cefSwitchWithValues;

    /** CEF log severity level
    */
    cef_log_severity_t m_logSeverity;

    /** Counter for browser controls
    */
    std::atomic<int32_t> m_browserCount;

    /** Interval (milliseconds) of the timer that calls the CEF message processing function
    */
    int32_t m_nCefDoMessageLoopWorkDelayMs;

    /** Exit code
    */
    int32_t m_nExitCode;

    /** Whether the CEF web page cache directory has been set
    */
    bool m_bHasCefCachePath;

    /** Whether off-screen rendering is enabled (cannot be modified after initialization)
    */
    bool m_bEnableOffScreenRendering;

    /** Whether it has been initialized
    */
    bool m_bCefInit;

    /** Whether the CEF message queue is empty
    */
    bool m_bCefMessageLoopEmpty;

    /** Whether the F12 shortcut key (developer tools) is allowed
    */
    bool m_bEnableF12;

    /** Whether the F11 shortcut key (page fullscreen / exit page fullscreen) is allowed
    */
    bool m_bEnableF11;

#ifdef DUI_BUILD_FOR_MACOS
    /** Timestamp when exit started
    */
    std::chrono::steady_clock::time_point m_exitTime;

    /** Whether it is exiting
    */
    bool m_bExiting;
#endif
};

} //namespace ui

#endif //DUI_BUILD_FOR_CEF
#endif //UI_CEF_CONTROL_CEF_MANAGER_H_
