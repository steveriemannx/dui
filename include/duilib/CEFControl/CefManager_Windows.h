#ifndef UI_CEF_CONTROL_CEF_MANAGER_WINDOWS_H_
#define UI_CEF_CONTROL_CEF_MANAGER_WINDOWS_H_

#include "duilib/CEFControl/CefManager.h"

#if defined (DUILIB_BUILD_FOR_WIN) && defined (DUILIB_BUILD_FOR_CEF)

#include <memory>

namespace ui
{
//Process singleton control (CEF109)
class ProcessSingleton;

/** CEF component manager (Windows implementation)
 */
class DUILIB_API CefManager_Windows : public CefManager
{
    friend class CefManager;
    typedef CefManager BaseClass;

protected:
    CefManager_Windows();
    CefManager_Windows(const CefManager_Windows&) = delete;
    CefManager_Windows& operator=(const CefManager_Windows&) = delete;
protected:
    virtual ~CefManager_Windows() override;

public:
    /** Get the path where the CEF module runtime library file is located
    */
    virtual DString GetCefMoudlePath() const override;

    /** Initialize the runtime environment; must be called before using the CEF module features
    */
    virtual bool InitEnv() override;

    /** Initialize the CEF component
    * @param [in] bEnableOffScreenRendering Whether to enable off-screen rendering
    * @param [in] appName Product name identifier (required parameter, must not be empty; used to generate the CEF cache path and ensure process uniqueness)
    * @param [in] argc Number of command-line arguments at program startup (used only on the Linux platform; other platforms can pass 0)
    * @param [in] argv List of command-line arguments at program startup (used only on the Linux platform; other platforms can pass nullptr)
    * @param [in] callback Callback function for setting CEF module parameters, used by the application layer to modify the initialization parameters
    * @param [in] nExitCode The process exit code when the function returns false
    * @return bool true to continue running, false to end the program
    */
    virtual bool Initialize(bool bEnableOffScreenRendering,
                            const DString& appName,
                            int argc,
                            char** argv,
                            OnCefSettingsEvent callback,
                            int32_t& nExitCode) override;

    /** Clean up the CEF component
    */
    virtual void UnInitialize() override;

public:
    /** Bind a callback function to listen for the Browser process startup event (used only on Windows + CEF109; not required in other cases)
    * @param [in] callback A callback function; refer to the OnAlreadyRunningAppRelaunchEvent declaration
    */
    virtual void SetAlreadyRunningAppRelaunch(const OnAlreadyRunningAppRelaunchEvent& callback) override;

    /** Get the callback function that listens for the Browser process startup event
    */
    virtual OnAlreadyRunningAppRelaunchEvent GetAlreadyRunningAppRelaunch() const override;

private:
    /** Add the location of the CEF DLL files to the "path" environment variable of the program, so that the DLL files can be placed in a directory other than bin without needing to manually switch DLL files frequently
    */
    void AddCefDllToPath();

#if CEF_VERSION_MAJOR <= 109
    /** Browser singleton control callback function
    */
    static void OnBrowserAlreadyRunningAppRelaunch(const std::vector<DString>& argumentList);
#endif

private:
    /** Callback function invoked when another Browser process starts
    */
    OnAlreadyRunningAppRelaunchEvent m_pfnAlreadyRunningAppRelaunch;

    //Process singleton control (CEF 109)
    std::unique_ptr<ProcessSingleton> m_pProcessSingleton;

    //Whether the process environment has been initialized
    bool m_bAddedCefDllToPath;
};

} //namespace ui

#endif //defined (DUILIB_BUILD_FOR_WIN/DUILIB_BUILD_FOR_CEF)

#endif //UI_CEF_CONTROL_CEF_MANAGER_WINDOWS_H_
