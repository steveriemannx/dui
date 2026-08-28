#include "MainForm.h"
#include "generated_ui.inc"  // Build-time generated pure C++ UI code (from basic.xml)

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
    ui::UiRect rcWork;
    ui::WindowBase::GetPrimaryMonitorWorkRect(rcWork);
    attrs.m_bInitSizeDefined = true;
    attrs.m_szInitSize.cx = (int32_t)(rcWork.Width() * 0.75f);
    attrs.m_szInitSize.cy = (int32_t)(rcWork.Height() * 0.75f);
    attrs.m_bShadowAttached = true;
    attrs.m_bShadowAttachedDefined = true;
    attrs.m_bIsLayeredWindow = true;
    attrs.m_bIsLayeredWindowDefined = true;
    attrs.m_rcSizeBox = ui::UiRect(4, 4, 4, 4);
    attrs.m_bSizeBoxDefined = true;
    attrs.m_rcCaption = ui::UiRect(0, 0, 0, 36);
    attrs.m_bCaptionDefined = true;

    // The XML basic.xml uses size="75%,75%" with a system shadow, so no
    // self-drawn shadow corner should be added here. Keeping the size exactly
    // in sync with the XML window is what main-mac expects.
    BaseClass::GetCreateWindowAttributes(attrs);
}

void MainForm::OnInitWindow()
{
    SetSizeBox(ui::UiRect(4, 4, 4, 4), true);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), true);
    // Use the OS-provided system shadow on all platforms.
    SetShadowAttached(true);
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);

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
    // The full-screen button is handled automatically by the WindowImplBase framework
    return true;
}
