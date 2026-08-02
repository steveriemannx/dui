#include "duilib/CEFControl/CefManager.h"

#ifdef DUILIB_BUILD_FOR_CEF

#include "duilib/CEFControl/CefControlNative.h"
#include "duilib/CEFControl/CefControlOffScreen.h"

#include "duilib/Utils/FilePathUtil.h"
#include "duilib/Core/GlobalManager.h"
#include "duilib/Core/Window.h"
#include "duilib/Core/Box.h"

#if defined (DUILIB_BUILD_FOR_WIN)
    #include "duilib/CEFControl/CefManager_Windows.h"
#elif defined (DUILIB_BUILD_FOR_LINUX)
    #include "duilib/CEFControl/CefManager_Linux.h"
#elif defined (DUILIB_BUILD_FOR_MACOS)
    #include "duilib/CEFControl/CefManager_MacOS.h"
#endif

#pragma warning (push)
#pragma warning (disable:4100 4324)
    #include "include/base/cef_callback.h"
    #include "include/base/cef_bind.h"
    #include "include/wrapper/cef_closure_task.h"
#pragma warning (pop)

namespace ui
{
//Callback function for creating CEF controls
static Control* DuilibCreateCefControl(const DString& className)
{
    Control* pControl = nullptr;
    if (className == _T("CefControl")) {
        if (ui::CefManager::GetInstance()->IsEnableOffScreenRendering()) {
            pControl = new CefControlOffScreen(nullptr);
        }
        else {
            pControl = new CefControlNative(nullptr);
        }
    }
    return pControl;
}

//Unit: milliseconds
#define CEF_DO_MESSAGE_LOOP_WORK_DELAY_MS 60

///////////////////////////////////////////////////////////////////////////////////
CefManager::CefManager():
    m_logSeverity(LOGSEVERITY_DEFAULT),
    m_browserCount(0),
    m_nCefDoMessageLoopWorkDelayMs(CEF_DO_MESSAGE_LOOP_WORK_DELAY_MS),
    m_nExitCode(0),
    m_bHasCefCachePath(false),
    m_bEnableOffScreenRendering(true),
    m_bCefInit(false),
    m_bCefMessageLoopEmpty(false),
    m_bEnableF12(true),
    m_bEnableF11(true)
{
#ifdef DUILIB_BUILD_FOR_MACOS
    m_bExiting = false;
#endif
}

CefManager::~CefManager()
{
}

CefManager* CefManager::GetInstance()
{
#if defined (DUILIB_BUILD_FOR_WIN)
    static CefManager_Windows self;
    return &self;
#elif defined (DUILIB_BUILD_FOR_LINUX)
    static CefManager_Linux self;
    return &self;
#elif defined (DUILIB_BUILD_FOR_MACOS)
    static CefManager_MacOS self;
    return &self;
#else
    ASSERT(0);
    return nullptr;
#endif
}

void CefManager::SetCefCachePath(const DString& cefCachePath)
{
    ASSERT(!m_bCefInit);
    m_cefCachePath = cefCachePath;
    m_bHasCefCachePath = true;
}

DString CefManager::GetCefCachePath() const
{
    if (m_bHasCefCachePath) {
        return m_cefCachePath;
    }
    DString defaultCachePath = _T("cef_cache");
    defaultCachePath += FilePath::GetPathSeparatorStr();
    defaultCachePath += m_appName;
    defaultCachePath += FilePath::GetPathSeparatorStr();
    return defaultCachePath;
}

void CefManager::SetCefMoudlePath(const DString& cefMoudlePath)
{
    ASSERT(!m_bCefInit);
    m_cefMoudlePath = cefMoudlePath;
}

DString CefManager::GetCefMoudlePath() const
{
    return m_cefMoudlePath;
}

void CefManager::SetCefLanguage(const DString& lang)
{
    ASSERT(!m_bCefInit);
    m_lang = lang;
}

DString CefManager::GetCefLanguage() const
{
    if (!m_lang.empty()) {
        return m_lang;
    }
    return _T("zh-CN");
}

void CefManager::SetLogSeverity(cef_log_severity_t log_severity)
{
    ASSERT(!m_bCefInit);
    m_logSeverity = log_severity;
}

cef_log_severity_t CefManager::GetLogSeverity() const
{
    return m_logSeverity;
}

bool CefManager::InitEnv()
{
    return true;
}

bool CefManager::Initialize(bool bEnableOffScreenRendering,
                            const DString& appName,
                            int /*argc*/,
                            char** /*argv*/,
                            OnCefSettingsEvent callback,
                            int32_t& /*nExitCode*/)
{
    ASSERT(!appName.empty());
    ASSERT(!m_bCefInit);
    if (m_bCefInit || appName.empty()) {
        return false;
    }
    m_appName = appName;
    m_cefSettingCallback = callback;
    m_bEnableOffScreenRendering = bEnableOffScreenRendering;

    //Add the callback function for creating window CEF controls
    GlobalManager::Instance().AddCreateControlCallback(DuilibCreateCefControl);
    m_bCefInit = true;
    return true;
}

void CefManager::UnInitialize()
{
    if (m_bCefInit) {
        m_bCefInit = false;
        CefShutdown();
    }
}

bool CefManager::IsCefInited() const
{
    return m_bCefInit;
}

void CefManager::SetAlreadyRunningAppRelaunch(const OnAlreadyRunningAppRelaunchEvent& /*callback*/)
{
}

OnAlreadyRunningAppRelaunchEvent CefManager::GetAlreadyRunningAppRelaunch() const
{
    return nullptr;
}

bool CefManager::IsEnableOffScreenRendering() const
{
    return m_bEnableOffScreenRendering;
}

void CefManager::AddBrowserCount()
{
    m_browserCount++;
}

void CefManager::SubBrowserCount()
{
    m_browserCount--;
    ASSERT(m_browserCount >= 0);
}

int32_t CefManager::GetBrowserCount()
{
    return m_browserCount;
}

static void GetCefControlList(Box* pRoot, std::vector<Control*>& cefControlList)
{
    if (pRoot == nullptr) {
        return;
    }
    std::vector<Box*> boxList;
    size_t nItemCount = pRoot->GetItemCount();
    for (size_t nItem = 0; nItem < nItemCount; ++nItem) {
        Control* pControl = pRoot->GetItemAt(nItem);
        if (pControl == nullptr) {
            continue;
        }
        if (dynamic_cast<CefControl*>(pControl) != nullptr) {
            cefControlList.push_back(pControl);
        }
        Box* pBox = dynamic_cast<Box*>(pControl);
        if (pBox != nullptr) {
            boxList.push_back(pBox);
        }
    }

    for (Box* pBox : boxList) {
        GetCefControlList(pBox, cefControlList);
    }
}

void CefManager::ProcessWindowCloseEvent(Window* pWindow)
{
    Box* pRoot = nullptr;
    if (pWindow != nullptr) {
        pRoot = pWindow->GetRoot();
    }
    std::vector<Control*> cefControlList;
    GetCefControlList(pRoot, cefControlList);

    for (Control* pControl : cefControlList) {
        CefControl* pCefControl = dynamic_cast<CefControl*>(pControl);
        if (pCefControl != nullptr) {
            pCefControl->OnHostWindowClosed();
        }
    }
}

void CefManager::PostQuitMessage(int32_t nExitCode)
{
    m_nExitCode = nExitCode;
    bool bForceExit = false;
#ifdef DUILIB_BUILD_FOR_MACOS
    //Fix the process residue problem on MacOS when exiting in child window mode (CefBrowserHandler::OnBeforeClose is not called; the reason is unknown)
    if (!m_bExiting) {
        m_bExiting = true;
        m_exitTime = std::chrono::steady_clock::now();
    }
    else {
        auto waitSeconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - m_exitTime);
        if (waitSeconds.count() > 15) {
            //Force exit if the wait exceeds 15 seconds
            bForceExit = true;
        }
    }
#endif

    // When we need to end the process, never call ::PostQuitMessage directly, because there may still be browser objects that have not been destroyed
    // We should wait until all browser objects are destroyed before calling ::PostQuitMessage
    if (bForceExit || (m_browserCount <= 0)) {
        if (IsMultiThreadedMessageLoop()) {
            if (m_bCefMessageLoopEmpty) {
                //CEF message loop enabled: exit the main thread message loop
                GlobalManager::Instance().Thread().PostTask(kThreadUI, [nExitCode]() {
                        NativeWindow::PostQuitMsg(nExitCode);
                    });
            }
            CefPostTask(TID_UI, base::BindOnce([]() {
                //After responding, mark the message queue as empty (if there are pending items in the CEF message loop, exiting the main thread message loop directly may cause occasional crashes)
                CefManager::GetInstance()->m_bCefMessageLoopEmpty = true;
                //CEF message loop not enabled: exit the main thread message loop directly
                GlobalManager::Instance().Thread().PostTask(kThreadUI, []() {
                    NativeWindow::PostQuitMsg(CefManager::GetInstance()->m_nExitCode);
                    });
                }));
        }
        else {
            //CEF message loop not enabled: exit the main thread message loop directly
            GlobalManager::Instance().Thread().PostTask(kThreadUI, [nExitCode]() {
                    NativeWindow::PostQuitMsg(nExitCode);
                });
        }
    }
    else {
        auto cb = [nExitCode]()  {
            CefManager::GetInstance()->PostQuitMessage(nExitCode);
        };
        GlobalManager::Instance().Thread().PostDelayedTask(kThreadUI, cb, 200);
    }
}

bool CefManager::IsMultiThreadedMessageLoop() const
{
    return true;
}

void CefManager::GetCefSetting(CefSettings& settings)
{
    DString appDataRootDir = GetCefCachePath();
    if (!appDataRootDir.empty()) {
        FilePath filePath(appDataRootDir);
        filePath.NormalizeDirectoryPath();
        if(!filePath.IsAbsolutePath()) {
            FilePath runPath = FilePathUtil::GetCurrentModuleDirectory();
            runPath /= filePath;
            filePath.Swap(runPath);
            filePath.NormalizeDirectoryPath();
        }
        appDataRootDir = filePath.NativePath();
        if (!appDataRootDir.empty() && !filePath.IsExistsDirectory()) {
            FilePathUtil::CreateDirectories(appDataRootDir);
        }
    }
    settings.no_sandbox = true;

    //Set localstorage; do not add "\\" at the end of the path, otherwise an error will be reported at runtime
    if (!appDataRootDir.empty()) {
        const DString cachePath = appDataRootDir + _T("CefLocalStorage");
        CefString(&settings.cache_path) = cachePath;
        CefString(&settings.root_cache_path) = cachePath;
    }

    //Set the log severity level
    settings.log_severity = GetLogSeverity();

    // Set the debug log file location
    if (settings.log_severity != cef_log_severity_t::LOGSEVERITY_DISABLE) {
        CefString(&settings.log_file) = appDataRootDir + _T("cef.log");
    }

    // cef2623/2526 debug mode: exiting the program while using multi_threaded_message_loop triggers a breakpoint
    // Adding the disable-extensions argument fixes this problem, but causes errors when opening some pages
    // Enable the Cef multi-threaded message loop, compatible with the nbase message loop
    settings.multi_threaded_message_loop = IsMultiThreadedMessageLoop();

    // Enable off-screen rendering
    settings.windowless_rendering_enabled = IsEnableOffScreenRendering();

    //Set the default language to Simplified Chinese
    CefString(&settings.locale) = GetCefLanguage();

    //Call the application-layer callback to provide a chance to modify the settings
    if (m_cefSettingCallback) {
        bool bMultiThreadedMessageLoop = settings.multi_threaded_message_loop;
        m_cefSettingCallback(settings);

        //Restore the values that are not allowed to be modified
        settings.multi_threaded_message_loop = bMultiThreadedMessageLoop;
    }

    if (!settings.multi_threaded_message_loop) {
        //The external CEF message loop mode needs to be enabled
        settings.external_message_pump = true;
    }
    else {
        settings.external_message_pump = false;
    }
}

void CefManager::ScheduleCefDoMessageLoopWork()
{
    ASSERT(!IsMultiThreadedMessageLoop());
    if (!IsMultiThreadedMessageLoop()) {
        int32_t delayMs = GetCefDoMessageLoopWorkDelayMs();
        GlobalManager::Instance().Thread().PostRepeatedTask(ui::kThreadUI, []() {
                // Execute a single CEF message processing
                if (ui::CefManager::GetInstance()->IsCefInited()) {
                    CefDoMessageLoopWork();
                }
            }, delayMs);
    }
}

void CefManager::SetCefDoMessageLoopWorkDelayMs(int32_t nCefDoMessageLoopWorkDelayMs)
{
    m_nCefDoMessageLoopWorkDelayMs = nCefDoMessageLoopWorkDelayMs;
    if (m_nCefDoMessageLoopWorkDelayMs < 1) {
        m_nCefDoMessageLoopWorkDelayMs = CEF_DO_MESSAGE_LOOP_WORK_DELAY_MS;
    }
}

int32_t CefManager::GetCefDoMessageLoopWorkDelayMs() const
{
    return m_nCefDoMessageLoopWorkDelayMs;
}

namespace {

    // These flags must match the Chromium values.
    const char kProcessType[] = "type";
    const char kRendererProcess[] = "renderer";
#if defined(DUILIB_BUILD_FOR_LINUX)
    const char kZygoteProcess[] = "zygote";
#endif

}  // namespace

CefManager::ProcessType CefManager::GetProcessType(CefRefPtr<CefCommandLine> commandLine)
{
    // The command-line flag won't be specified for the browser process.
    if ((commandLine == nullptr) || !commandLine->HasSwitch(kProcessType)) {
        return BrowserProcess;
    }

    const std::string& processType = commandLine->GetSwitchValue(kProcessType);
    if (processType == kRendererProcess) {
        return RendererProcess;
    }
#if defined(DUILIB_BUILD_FOR_LINUX)
    else if (processType == kZygoteProcess) {
        return ZygoteProcess;
    }
#endif

    return OtherProcess;
}

void CefManager::AppendSwitchWithValue(const DString& name, const DString& value)
{
    ASSERT(!value.empty());
    if (!value.empty()) {
        m_cefSwitchWithValues.push_back(std::pair<DString, DString>(name, value));
    }
}

const std::vector<std::pair<DString, DString>>& CefManager::GetSwitchWithValues() const
{
    return m_cefSwitchWithValues;
}

void CefManager::SetEnableF12(bool bEnableF12)
{
    m_bEnableF12 = bEnableF12;
}

bool CefManager::IsEnableF12() const
{
    return m_bEnableF12;
}

void CefManager::SetEnableF11(bool bEnableF11)
{
    m_bEnableF11 = bEnableF11;
}

bool CefManager::IsEnableF11() const
{
    return m_bEnableF11;
}

} //namespace ui

#endif //DUILIB_BUILD_FOR_CEF
