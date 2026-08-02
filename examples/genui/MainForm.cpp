#include "MainForm.h"
#include "generated_ui.inc"  // Build-time generated pure C++ UI code

MainForm::MainForm()
{
}

MainForm::~MainForm()
{
}

DString MainForm::GetSkinFolder()
{
    return _T("basic");
}

DString MainForm::GetSkinFile()
{
    // No XML file - UI is generated at build time from basic.xml
    return _T("");
}

void MainForm::GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs)
{
    // Window attributes from basic.xml (handled at build time)
    attrs.m_bInitSizeDefined = true;
    attrs.m_szInitSize.cx = 1000;
    attrs.m_szInitSize.cy = 600;
    attrs.m_bShadowAttached = true;
    attrs.m_bShadowAttachedDefined = true;
    attrs.m_bIsLayeredWindow = true;
    attrs.m_bIsLayeredWindowDefined = true;
    attrs.m_rcSizeBox = ui::UiRect(4, 4, 4, 4);
    attrs.m_bSizeBoxDefined = true;
    attrs.m_rcCaption = ui::UiRect(0, 0, 0, 36);
    attrs.m_bCaptionDefined = true;
    BaseClass::GetCreateWindowAttributes(attrs);
}

void MainForm::OnInitWindow()
{
    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);

    // Build-time generated from basic.xml: InitBasic(pWindow)
    // (calls AttachBox automatically since basic.xml root is <Window>)
    InitBasic(this);

    // Wire up caption button click handlers (names set by generated code)
    auto wireBtn = [this](const DString& name) {
        ui::Button* btn = dynamic_cast<ui::Button*>(FindControl(name));
        if (btn) btn->AttachClick(ui::UiBind(&MainForm::OnButtonClick, this, std::placeholders::_1));
    };
    wireBtn(_T("closebtn"));
    wireBtn(_T("minbtn"));
    wireBtn(_T("maxbtn"));
    wireBtn(_T("restorebtn"));
    wireBtn(_T("fullscreenbtn"));

    BaseClass::OnInitWindow();
}

bool MainForm::OnButtonClick(const ui::EventArgs& msg)
{
    ui::Control* pSender = msg.GetSender();
    if (pSender == nullptr) return false;
    DString sName = pSender->GetName();
    if (sName == DUI_CTR_BUTTON_CLOSE) {
        CloseWnd();
    } else if (sName == DUI_CTR_BUTTON_MIN) {
        ShowWindow(ui::kSW_MINIMIZE);
    } else if (sName == DUI_CTR_BUTTON_MAX) {
        ShowWindow(ui::kSW_SHOW_MAXIMIZED);
    } else if (sName == DUI_CTR_BUTTON_RESTORE) {
        ShowWindow(ui::kSW_RESTORE);
    }
    return true;
}
