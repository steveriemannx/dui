#include "dui/CEFControl/CefManager_Linux.h"

#if defined (DUI_BUILD_FOR_LINUX) && defined (DUI_BUILD_FOR_CEF)

#include "dui/CEFControl/internal/CefClientApp.h"
#include "dui/CEFControl/internal/CefBrowserHandler.h"

namespace ui
{
CefManager_Linux::CefManager_Linux()
{
}

CefManager_Linux::~CefManager_Linux()
{
}

bool CefManager_Linux::Initialize(bool bEnableOffScreenRendering,
                                  const DString& appName,
                                  int argc,
                                  char** argv,
                                  OnCefSettingsEvent callback,
                                  int32_t& nExitCode)
{
    if (!BaseClass::Initialize(bEnableOffScreenRendering, appName, argc, argv, callback, nExitCode)) {
        return false;
    }

    CefMainArgs main_args(argc, argv);
    CefRefPtr<CefClientApp> app(new CefClientApp);

    // If called in a child process, it will block until the child process exits, and exit_code returns a value >= 0
    // If called in the Browser process, it returns -1 immediately
    int exit_code = CefExecuteProcess(main_args, app.get(), nullptr);
    if (exit_code >= 0) {
        nExitCode = exit_code;
        return false;
    }

    CefSettings settings;
    GetCefSetting(settings);

    bool bRet = CefInitialize(main_args, settings, app.get(), nullptr);
    if (!bRet) {
        return false;
    }
    return true;
}

} //namespace ui

#endif //defined (DUI_BUILD_FOR_LINUX/DUI_BUILD_FOR_CEF)
