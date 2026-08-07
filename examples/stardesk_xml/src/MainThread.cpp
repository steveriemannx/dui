#include "MainThread.h"

#include "ui/MainWindowXml.h"

#include "examples/stardesk/src/app/AppConfig.h"

namespace sdk {

namespace {

/** Register the base fonts/colors the XML skin references (global.xml may be
 *  absent in standalone distributions). */
void InitFontsAndColors()
{
    using namespace ui;

    auto& fm = GlobalManager::Instance().Font();
    fm.SetDefaultFontFamilyNames(
        _T("PingFang SC,Microsoft YaHei,Noto Sans CJK SC,Helvetica Neue,Arial,sans-serif"));

    UiFont f;
    f.m_fontName = _T("system"); f.m_fontSize = 12;
    fm.AddFont(_T("system_12"), f, true);
    f.m_fontSize = 14; fm.AddFont(_T("system_14"), f, false);
    f.m_fontSize = 16; fm.AddFont(_T("system_16"), f, false);
    f.m_bBold = true;
    f.m_fontSize = 12; fm.AddFont(_T("system_bold_12"), f, false);
    f.m_fontSize = 14; fm.AddFont(_T("system_bold_14"), f, false);
    f.m_bBold = false;

    auto& cm = GlobalManager::Instance().Color();
    cm.AddColor(_T("white"), UiColor(0xFFFFFFFF));
    cm.AddColor(_T("black"), UiColor(0xFF000000));
    cm.AddColor(_T("gray"), UiColor(0xFF8E99A6));
    cm.AddColor(_T("green"), UiColor(0xFF00BB96));
    cm.AddColor(_T("red"), UiColor(0xFFC63535));
    cm.AddColor(_T("blue"), UiColor(0xFF006DD9));
    cm.AddColor(_T("bk_wnd_darkcolor"), UiColor(0xFFF0F2F5));
    cm.AddColor(_T("bk_wnd_lightcolor"), UiColor(0xFFFFFFFF));
    cm.AddColor(_T("bk_listitem_hovered"), UiColor(0xFFF0F2F5));
    cm.AddColor(_T("bk_listitem_selected"), UiColor(0xFFE4E7EB));
    cm.SetDefaultTextColor(_T("black"));
}

} // namespace

MainThread::MainThread() : BaseClass(_T("StarDeskXML"), ui::kThreadUI)
{
}

MainThread::~MainThread()
{
}

void MainThread::OnInit()
{
    ui::FilePath resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();
    resourcePath += _T("resources/");
    ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath));

    InitFontsAndColors();
    App::Instance().Init();

    MainWindowXml* window = new MainWindowXml();
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
