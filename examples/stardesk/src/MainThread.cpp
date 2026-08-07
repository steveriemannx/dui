#include "MainThread.h"

#include "app/AppConfig.h"
#include "ui/MainWindow.h"

#include <csignal>

namespace sdk {

namespace {

/** Compile-time theme bootstrap: register the fonts and colors the UI uses,
 *  so the app renders even when the dui resource directory is absent
 *  (standalone distribution). Mirrors what global.xml would provide.
 */
void InitFontsAndColors()
{
    using namespace ui;

    auto& fm = GlobalManager::Instance().Font();
    fm.SetDefaultFontFamilyNames(
        _T("PingFang SC,Microsoft YaHei,Noto Sans CJK SC,Helvetica Neue,Arial,sans-serif"));

    UiFont f;
    f.m_fontName = _T("system"); f.m_fontSize = 12;
    fm.AddFont(_T("system_12"), f, true);   // default
    f.m_fontSize = 14; fm.AddFont(_T("system_14"), f, false);
    f.m_fontSize = 16; fm.AddFont(_T("system_16"), f, false);
    f.m_fontSize = 18; fm.AddFont(_T("system_18"), f, false);
    f.m_fontSize = 20; fm.AddFont(_T("system_20"), f, false);

    f.m_bBold = true;
    f.m_fontSize = 12; fm.AddFont(_T("system_bold_12"), f, false);
    f.m_fontSize = 14; fm.AddFont(_T("system_bold_14"), f, false);
    f.m_fontSize = 16; fm.AddFont(_T("system_bold_16"), f, false);
    f.m_fontSize = 18; fm.AddFont(_T("system_bold_18"), f, false);
    f.m_bBold = false;

    // base colors referenced by dui's default rendering (theme colors used by
    // the UI itself come from sdk::Theme)
    auto& cm = GlobalManager::Instance().Color();
    cm.AddColor(_T("white"),  UiColor(0xFFFFFFFF));
    cm.AddColor(_T("black"),  UiColor(0xFF000000));
    cm.AddColor(_T("darkcolor"), UiColor(0xFF333333));
    cm.AddColor(_T("lightcolor"), UiColor(0xFF888888));
    cm.SetDefaultTextColor(_T("darkcolor"));
}

} // namespace

// ---------------------------------------------------------------- MainThread

MainThread::MainThread() : BaseClass(_T("StarDesk"), ui::kThreadUI)
{
}

MainThread::~MainThread()
{
}

void MainThread::OnInit()
{
#if !defined(_WIN32)
    // A peer that dies mid-stream (crash / network drop / power loss) makes
    // the next send() raise SIGPIPE, which by default KILLS the process - a
    // remote-control host must survive that. Ignore it so send() returns
    // EPIPE instead and the existing "closed connection" handling kicks in.
    std::signal(SIGPIPE, SIG_IGN);
#endif

    // resource root next to the executable (may be absent - all theme data is
    // registered below, so the app works without any resource files)
    ui::FilePath resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();
    resourcePath += _T("resources/");
    ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath));

    InitFontsAndColors();

    // load config, apply theme + language
    App::Instance().Init();

    MainWindow* window = new MainWindow();
    window->CreateWnd(nullptr, ui::WindowCreateParam(_T("StarDesk"), true));
    window->PostQuitMsgWhenClosed(true);
    window->ShowWindow(ui::kSW_SHOW_NORMAL);
}

void MainThread::OnCleanup()
{
    App::Instance().Cleanup();
    ui::GlobalManager::Instance().Shutdown();
}

} // namespace sdk
