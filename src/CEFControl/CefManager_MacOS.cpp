#include "dui/CEFControl/CefManager_MacOS.h"

#if defined (DUI_BUILD_FOR_MACOS) && defined (DUI_BUILD_FOR_CEF)

#include "dui/CEFControl/internal/CefClientApp.h"
#include "dui/CEFControl/internal/CefBrowserHandler.h"

#include "include/wrapper/cef_library_loader.h"

namespace ui
{
CefManager_MacOS::CefManager_MacOS()
{
}

CefManager_MacOS::~CefManager_MacOS()
{
}

bool CefManager_MacOS::Initialize(bool bEnableOffScreenRendering,
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

    CefSettings settings;
    GetCefSetting(settings);

    bool bRet = CefInitialize(main_args, settings, app.get(), nullptr);
    if (!bRet) {
        nExitCode = 1;
        return false;
    }
    return true;
}

bool CefManager_MacOS::IsMultiThreadedMessageLoop() const
{
    return false;
}

} //namespace ui

#endif //defined (DUI_BUILD_FOR_MACOS/DUI_BUILD_FOR_CEF)
