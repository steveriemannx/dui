#include "MainForm.h"
#include "LayoutBuildUI.inc"  // Pure-code-built UI (corresponding to the layout.xml layout)

MainForm::MainForm()
{
}

MainForm::~MainForm()
{
}

DString MainForm::GetSkinFolder()
{
    // Keep the same resource path as the XML/gen versions so layout images
    // and resources are resolved identically.
    return _T("layout");
}

DString MainForm::GetSkinFile()
{
    // Pure code mode: no layout XML is loaded
    return _T("");
}

void MainForm::GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs)
{
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

    BaseClass::GetCreateWindowAttributes(attrs);
}

void MainForm::OnInitWindow()
{
    // Use the OS-provided system shadow on all platforms.
    SetShadowAttached(true);
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);

    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);

    //Pure-code-built UI (corresponding to the layout.xml layout)
    BuildUIFromXml(this);
    BaseClass::OnInitWindow();
}

void MainForm::OnCloseWindow()
{
    //After the window is closed, exit the main thread message loop and shut down the program
    PostQuitMsg(0L);
}
