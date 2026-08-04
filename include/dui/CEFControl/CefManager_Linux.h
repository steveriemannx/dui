#ifndef UI_CEF_CONTROL_CEF_MANAGER_LINUX_H_
#define UI_CEF_CONTROL_CEF_MANAGER_LINUX_H_

#include "dui/CEFControl/CefManager.h"

#if defined (DUI_BUILD_FOR_LINUX) && defined (DUI_BUILD_FOR_CEF)

namespace ui
{
/** CEF component manager (Linux implementation)
 */
class CefManager_Linux : public CefManager
{
    friend class CefManager;
    typedef CefManager BaseClass;

protected:
    CefManager_Linux();
    CefManager_Linux(const CefManager_Linux&) = delete;
    CefManager_Linux& operator=(const CefManager_Linux&) = delete;
protected:
    virtual ~CefManager_Linux() override;

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

};

} //namespace ui

#endif //defined (DUI_BUILD_FOR_LINUX/DUI_BUILD_FOR_CEF)

#endif //UI_CEF_CONTROL_CEF_MANAGER_LINUX_H_
