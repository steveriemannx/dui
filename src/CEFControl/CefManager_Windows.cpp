#include "duilib/CEFControl/CefManager_Windows.h"

#if defined (DUILIB_BUILD_FOR_WIN) && defined (DUILIB_BUILD_FOR_CEF)

#include "duilib/Core/Control.h"
#include "duilib/Utils/FilePathUtil.h"
#include "duilib/Utils/ProcessSingleton.h"
#include "duilib/Utils/StringConvert.h"
#include "duilib/CEFControl/internal/CefClientApp.h"
#include "duilib/CEFControl/internal/CefBrowserHandler.h"

#pragma warning (push)
#pragma warning (disable:4100)
#include "include/wrapper/cef_closure_task.h"
#include "include/base/cef_bind.h"
#include "include/base/cef_callback.h"
#pragma warning (pop)

namespace ui
{
CefManager_Windows::CefManager_Windows():
    m_pfnAlreadyRunningAppRelaunch(nullptr),
    m_bAddedCefDllToPath(false)
{
}

CefManager_Windows::~CefManager_Windows()
{
}

DString CefManager_Windows::GetCefMoudlePath() const
{
    DString cefMoudlePath = BaseClass::GetCefMoudlePath();
    if (cefMoudlePath.empty()) {
        //Use the default rules
#if CEF_VERSION_MAJOR <= 109
    //CEF 109 version
    #ifdef _WIN64
        cefMoudlePath = _T("libcef_win_109\\x64");
    #else
        cefMoudlePath = _T("libcef_win_109\\Win32");
    #endif
#else
    //CEF higher versions
    #ifdef _WIN64
        cefMoudlePath = _T("libcef_win\\x64");
    #else
        cefMoudlePath = _T("libcef_win\\Win32");
    #endif
#endif
    }
    return cefMoudlePath;
}

// Discovered a very strange bug: in off-screen rendering + multi-threaded message loop mode, a popup menu opened by right-clicking on the browser object cannot be closed properly
// After going through the CEF source code, we found that the menu is created with the TrackPopupMenu function. Checking the MSDN documentation, before calling TrackPopupMenu
// you need to call SetForegroundWindow on its parent window. However, this is not done in the CEF source code
// The final solution obtained from the CEF source code is to create a window on the CEF UI thread, and the parent window of this window must be created on the main program's UI thread
// After doing this, the bug where the menu cannot be closed no longer occurs. Although we do not know why, the bug is fixed

// Another issue: if this approach is not taken, drag-and-drop operations in off-screen rendered pages are abnormal; dragging data out will cause the program to freeze
//
static void FixContextMenuBug(HWND hwnd)
{
    ::CreateWindowW(L"Static", L"", WS_CHILD, 0, 0, 0, 0, hwnd, nullptr, nullptr, nullptr);
    ::PostMessage(hwnd, WM_CLOSE, 0, 0);
}

bool CefManager_Windows::InitEnv()
{
    if (!BaseClass::InitEnv()) {
        return false;
    }
    if (!m_bAddedCefDllToPath) {
        m_bAddedCefDllToPath = true;
        AddCefDllToPath();
    }
    return true;
}

bool CefManager_Windows::Initialize(bool bEnableOffScreenRendering,
                                    const DString& appName,
                                    int argc,
                                    char** argv,
                                    OnCefSettingsEvent callback,
                                    int32_t& nExitCode)
{
    if (!BaseClass::Initialize(bEnableOffScreenRendering, appName, argc, argv, callback, nExitCode)) {
        return false;
    }

    //Add the directory containing the libcef.dll file to the "path" environment variable of the program
    if (!m_bAddedCefDllToPath) {
        m_bAddedCefDllToPath = true;
        AddCefDllToPath();
    }

#if CEF_VERSION_MAJOR <= 109
    //CEF 109 version, control process singleton
    // Parse the command-line arguments to identify whether it is the Browser process
    if (!appName.empty()) {
        CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
        command_line->InitFromString(::GetCommandLineW());
        if (!command_line->HasSwitch("type")) {
            // Browser process logic
            m_pProcessSingleton = ProcessSingleton::Create(appName);
            if ((m_pProcessSingleton != nullptr) && m_pProcessSingleton->IsAnotherInstanceRunning()) {
                //Another Browser process is already running; send the startup arguments and then exit
                std::vector<CefString> cmdLineArgv;
                command_line->GetArgv(cmdLineArgv);
                std::vector<std::string> argumentList;
                std::string arg;
                for (size_t i = 1; i < cmdLineArgv.size(); ++i) {
                    arg = StringConvert::WStringToUTF8(cmdLineArgv[i]);
                    if (!arg.empty()) {
                        argumentList.push_back(arg);
                    }
                }
                m_pProcessSingleton->SendArgumentsToExistingInstance(argumentList);
                return false;
            }
        }
    }
#endif

    CefMainArgs main_args(::GetModuleHandle(nullptr));
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

    if (IsEnableOffScreenRendering()) {
        HWND hwnd = ::CreateWindowW(L"Static", L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, nullptr, nullptr);
        CefPostTask(TID_UI, base::BindOnce(&FixContextMenuBug, hwnd));
    }

#if CEF_VERSION_MAJOR <= 109
    //Start the singleton process monitor
    if (m_pProcessSingleton != nullptr) {
        m_pProcessSingleton->StartListener(OnBrowserAlreadyRunningAppRelaunch);
    }
#endif
    return true;
}

void CefManager_Windows::UnInitialize()
{
#if CEF_VERSION_MAJOR <= 109
    //Start the singleton process monitor
    if (m_pProcessSingleton != nullptr) {
        m_pProcessSingleton.reset();
    }
#endif
    BaseClass::UnInitialize();
}

void CefManager_Windows::SetAlreadyRunningAppRelaunch(const OnAlreadyRunningAppRelaunchEvent& callback)
{
    m_pfnAlreadyRunningAppRelaunch = callback;
}

OnAlreadyRunningAppRelaunchEvent CefManager_Windows::GetAlreadyRunningAppRelaunch() const
{
    return m_pfnAlreadyRunningAppRelaunch;
}

void CefManager_Windows::AddCefDllToPath()
{
    DString cefMoudlePath = GetCefMoudlePath();
    if (cefMoudlePath.empty()) {
        return;
    }

    TCHAR path_envirom[4096] = { 0 };
    ::GetEnvironmentVariable(_T("path"), path_envirom, 4096);

    FilePath cefDllDir = ui::FilePathUtil::GetCurrentModuleDirectory();
    cefDllDir /= FilePath(cefMoudlePath);
    cefDllDir.NormalizeDirectoryPath();
    FilePath cefDllPath = cefDllDir;
    cefDllPath /= FilePath(L"libcef.dll");
    if (!cefDllDir.IsExistsDirectory() || !cefDllPath.IsExistsFile()) {
        DStringW errMsg = L"Failed to load libcef.dll!\nPlease extract the CEF binaries (libcef.dll etc.) and resource files to the following directory:\n";
        errMsg += cefDllDir.ToStringW();
        ::MessageBoxW(nullptr, errMsg.c_str(), L"Error Hint", MB_OK);
        exit(0);
    }
    DString new_envirom(cefDllDir.NativePath());
    new_envirom.append(_T(";")).append(path_envirom);
    ::SetEnvironmentVariable(_T("path"), new_envirom.c_str());
}

#if CEF_VERSION_MAJOR <= 109
/** Browser singleton control callback function
*/
void CefManager_Windows::OnBrowserAlreadyRunningAppRelaunch(const std::vector<DString>& argumentList)
{
    OnAlreadyRunningAppRelaunchEvent pfnAlreadyRunningAppRelaunch = CefManager::GetInstance()->GetAlreadyRunningAppRelaunch();
    if (pfnAlreadyRunningAppRelaunch != nullptr) {
        pfnAlreadyRunningAppRelaunch(argumentList);
    }
}
#endif

} //namespace ui

#endif //defined (DUILIB_BUILD_FOR_WIN/DUILIB_BUILD_FOR_CEF)
