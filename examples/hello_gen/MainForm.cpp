#include "MainForm.h"
#include "generated_ui.inc"  // Build-time generated pure C++ UI code (from ../hello/hello.xml)

MainForm::MainForm()
{
}

MainForm::~MainForm()
{
}

DString MainForm::GetSkinFolder()
{
    return _T("hello");
}

DString MainForm::GetSkinFile()
{
    // No XML file - UI is generated at build time from hello.xml
    return _T("");
}

void MainForm::GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs)
{
    // Window attributes from hello.xml (handled at build time)
    attrs.m_bInitSizeDefined = true;
    attrs.m_szInitSize.cx = 600;
    attrs.m_szInitSize.cy = 400;
    attrs.m_bShadowAttached = true;
    attrs.m_bShadowAttachedDefined = true;
    attrs.m_bIsLayeredWindow = true;
    attrs.m_bIsLayeredWindowDefined = true;
    attrs.m_rcSizeBox = ui::UiRect(4, 4, 4, 4);
    attrs.m_bSizeBoxDefined = true;
    attrs.m_rcCaption = ui::UiRect(0, 0, 0, 36);
    attrs.m_bCaptionDefined = true;

    // Shadow nine-patch parameters, corresponding to shadow_type="default" in hello.xml
    ui::Shadow::ShadowType nShadowType = ui::Shadow::ShadowType::kShadowDefault;
    ui::UiSize szBorderRound;
    ui::UiPadding rcShadowCorner;
    DString shadowImage;
    if (ui::Shadow::GetShadowParam(nShadowType, szBorderRound, rcShadowCorner, shadowImage)) {
        attrs.m_rcShadowCorner = rcShadowCorner;
        // The window size includes the shadow area, matching the size="600,400"
        // behavior of hello.xml (WindowBuilder adds the shadow corner to the size)
        attrs.m_szInitSize.cx += rcShadowCorner.left + rcShadowCorner.right;
        attrs.m_szInitSize.cy += rcShadowCorner.top + rcShadowCorner.bottom;
    }
    BaseClass::GetCreateWindowAttributes(attrs);
}

void MainForm::OnInitWindow()
{
    SetSizeBox(ui::UiRect(4, 4, 4, 4), true);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), true);

    // Build-time generated from hello.xml: InitHello(pWindow)
    // (calls AttachBox automatically since hello.xml root is <Window>)
    InitHello(this);

    // Wire up the "Click Me" button (XML on_click attributes are not generated)
    ui::Button* pBtn = dynamic_cast<ui::Button*>(FindControl(_T("hello_btn")));
    if (pBtn != nullptr) {
        pBtn->AttachClick([this](const ui::EventArgs& /*args*/) {
            ui::Label* pLabel = dynamic_cast<ui::Label*>(FindControl(_T("hello_label")));
            if (pLabel != nullptr) {
                pLabel->SetText(_T("Hello from codegen mode!"));
            }
            return true;
        });
    }

    // Set the window icon (the Dock icon on macOS), matching
    // icon="public/caption/logo.ico" in hello.xml
    SetWindowIcon(_T("public/caption/logo.ico"));

    BaseClass::OnInitWindow();
}
