#include "dui/CEFControl/internal/CefClientApp.h"

#ifdef DUI_BUILD_FOR_CEF

#include "dui/CEFControl/CefManager.h"
#include "dui/Core/GlobalManager.h"

namespace ui
{
//////////////////////////////////////////////////////////////////////////////////////////
// CefBrowserProcessHandler methods.
void CefClientApp::OnRegisterCustomPreferences(cef_preferences_type_t /*type*/, CefRawPtr<CefPreferenceRegistrar> /*registrar*/)
{
}

void CefClientApp::OnContextInitialized()
{
}

void CefClientApp::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> /*command_line*/)
{
}

#if CEF_VERSION_MAJOR > 109
//CEF newer versions
bool CefClientApp::OnAlreadyRunningAppRelaunch(CefRefPtr<CefCommandLine> command_line, const CefString& /*current_directory*/)
{
    // Another Browser process has been started; the process singleton mode needs to be maintained
    OnAlreadyRunningAppRelaunchEvent pfnAlreadyRunningAppRelaunch = CefManager::GetInstance()->GetAlreadyRunningAppRelaunch();
    if (pfnAlreadyRunningAppRelaunch != nullptr) {
        std::vector<DString> argumentList;
        if (command_line != nullptr) {
            CefCommandLine::ArgumentList arguments;
            command_line->GetArguments(arguments);
            for (const CefString& arg : arguments) {
                argumentList.push_back(arg);
            }
        }
        pfnAlreadyRunningAppRelaunch(argumentList);
    }
    // Return true to intercept
    return true;
}
#endif

void CefClientApp::OnScheduleMessagePumpWork(int64_t delay_ms)
{
    if (CefManager::GetInstance()->IsMultiThreadedMessageLoop()) {
        return;
    }

    if (GlobalManager::Instance().Thread().IsMainThreadExit()) {
        // The main thread has exited; the process is exiting
        if (CefManager::GetInstance()->IsCefInited()) {
            CefDoMessageLoopWork();
        }
    }
    else {
        // delay_ms indicates the suggested delay (in milliseconds) for the next processing
        if (delay_ms <= 0) {
            // Trigger message processing immediately
            GlobalManager::Instance().Thread().PostTask(ui::kThreadUI, []() {
                    // Perform a single CEF message processing
                    if (CefManager::GetInstance()->IsCefInited()) {
                        CefDoMessageLoopWork();
                    }
                });
        }
        else {
            // Set the delayed trigger
            GlobalManager::Instance().Thread().PostDelayedTask(ui::kThreadUI, []() {
                    // Perform a single CEF message processing
                    if (CefManager::GetInstance()->IsCefInited()) {
                        CefDoMessageLoopWork();
                    }
                }, (int32_t)delay_ms);
        }
    }
}

CefRefPtr<CefClient> CefClientApp::GetDefaultClient()
{
    return nullptr;
}

#if CEF_VERSION_MAJOR > 109
//CEF newer versions
CefRefPtr<CefRequestContextHandler> CefClientApp::GetDefaultRequestContextHandler()
{
    return nullptr;
}
#endif

} //namespace ui

#endif //DUI_BUILD_FOR_CEF
