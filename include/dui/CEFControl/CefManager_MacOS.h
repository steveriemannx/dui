#ifndef UI_CEF_CONTROL_CEF_MANAGER_MACOS_H_
#define UI_CEF_CONTROL_CEF_MANAGER_MACOS_H_

#include "dui/CEFControl/CefManager.h"

#if defined (DUI_BUILD_FOR_MACOS) && defined (DUI_BUILD_FOR_CEF)

namespace ui
{
/** CEF component manager (MacOS implementation)
 */
class CefManager_MacOS : public CefManager
{
    friend class CefManager;
    typedef CefManager BaseClass;

protected:
    CefManager_MacOS();
    CefManager_MacOS(const CefManager_MacOS&) = delete;
    CefManager_MacOS& operator=(const CefManager_MacOS&) = delete;
protected:
    virtual ~CefManager_MacOS() override;

public:
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

    /** Whether the current CEF runs in multi-threaded message loop mode (supported on Windows/Linux platforms, but not on MacOS)
    */
    virtual bool IsMultiThreadedMessageLoop() const override;

};

} //namespace ui

#endif //defined (DUI_BUILD_FOR_MACOS/DUI_BUILD_FOR_CEF)

#endif //UI_CEF_CONTROL_CEF_MANAGER_MACOS_H_
