#include "dui/CEFControl/internal/CefClientApp.h"

#ifdef DUI_BUILD_FOR_CEF

#include "dui/CEFControl/CefManager.h"
#include "dui/Utils/StringUtil.h"
#include "dui/Utils/StringConvert.h"

#if defined (DUI_BUILD_FOR_LINUX) && defined (DUI_BUILD_FOR_SDL)
    #include "dui/Core/MessageLoop_SDL.h"
#endif

#pragma warning (push)
#pragma warning (disable:4100)
#include "include/cef_cookie.h"
#include "include/cef_process_message.h"
#include "include/cef_task.h"
#include "include/cef_v8.h"
#pragma warning (pop)

#if defined (DUI_BUILD_FOR_LINUX) && defined (DUI_BUILD_FOR_SDL)
    #include <cstdlib> // for getenv
#endif

namespace ui
{

#if defined (DUI_BUILD_FOR_LINUX) && defined (DUI_BUILD_FOR_SDL)
    static bool IsWaylandEnvironment()
    {
        // The following environment variables are usually set in a Wayland environment
        const char* waylandDisplay = getenv("WAYLAND_DISPLAY");
        const char* xdgSessionType = getenv("XDG_SESSION_TYPE");

        return (waylandDisplay != nullptr && *waylandDisplay != '\0') ||
               (xdgSessionType != nullptr && strcmp(xdgSessionType, "wayland") == 0);
    }
#endif

CefClientApp::CefClientApp()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// CefApp methods.
void CefClientApp::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
{
    // Pass additional command-line flags to the browser process.
    if (process_type.empty()) {

        // Use the same render process for the same site
        command_line->AppendSwitch("process-per-site");
        command_line->AppendSwitch("disable-gpu");
        command_line->AppendSwitch("disable-gpu-compositing");

#if defined (DUI_BUILD_FOR_LINUX) && defined (DUI_BUILD_FOR_SDL)
        // Disable the GPU sandbox; when this option is enabled, the GPU process cannot start properly on the Linux platform
        command_line->AppendSwitch("disable-gpu-sandbox");

        DString currentVideoDriver = StringUtil::MakeLowerString(MessageLoop_SDL::GetCurrentVideoDriverName());
        // The backend rendering mode of CEF must stay consistent with SDL, otherwise the child window mode will not work properly
        if (currentVideoDriver == _T("wayland")) {
            // Force the Ozone platform to Wayland (disable X11)
            command_line->AppendSwitchWithValue("ozone-platform", "wayland");
        }
        else if (currentVideoDriver == _T("x11")) {
            // Force the Ozone platform to X11 (disable Wayland)
            command_line->AppendSwitchWithValue("ozone-platform", "x11");
        }
     
        if (IsWaylandEnvironment() && (currentVideoDriver == _T("x11"))) {
            // XWayland environment: fix the issue where the DPI adaptation feature of the CEF page fails
            DString dpiFactor;
            float scale = MessageLoop_SDL::GetPrimaryDisplayContentScale();
            if (scale > 0.001f) {
                dpiFactor = StringUtil::Printf(_T("%.02f"), scale);
            }
            if (!dpiFactor.empty()) {
                command_line->AppendSwitchWithValue("force-device-scale-factor", CefString(dpiFactor.c_str()));
            }
        }

#endif //DUI_BUILD_FOR_LINUX && DUI_BUILD_FOR_SDL

        // Enable off-screen rendering
        if (CefManager::GetInstance()->IsEnableOffScreenRendering()) {
            command_line->AppendSwitch("disable-surfaces");
            command_line->AppendSwitch("enable-begin-frame-scheduling");
        }

        // Add extra parameters (application-layer configuration)
        const std::vector<std::pair<DString, DString>>& cefSwitchWithValues = CefManager::GetInstance()->GetSwitchWithValues();
        if (!cefSwitchWithValues.empty()) {
            for (const std::pair<DString, DString>& switchWithValue : cefSwitchWithValues) {
                if (switchWithValue.first.empty()) {
                    DStringA value = StringConvert::TToUTF8(switchWithValue.second);
                    command_line->AppendSwitch(value.c_str());
                }
                else {
                    DStringA name = StringConvert::TToUTF8(switchWithValue.first);
                    DStringA value = StringConvert::TToUTF8(switchWithValue.second);
                    command_line->AppendSwitchWithValue(name.c_str(), value.c_str());
                }
            }
        }
    }
}

void CefClientApp::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> /*registrar*/)
{
}

CefRefPtr<CefBrowserProcessHandler> CefClientApp::GetBrowserProcessHandler()
{
    return this;
}

CefRefPtr<CefRenderProcessHandler> CefClientApp::GetRenderProcessHandler()
{
    return this;
}

} //namespace ui

#endif //DUI_BUILD_FOR_CEF
