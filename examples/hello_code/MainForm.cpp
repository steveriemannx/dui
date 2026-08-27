#include "MainForm.h"

MainForm::MainForm()
{
}

MainForm::~MainForm()
{
}

DString MainForm::GetSkinFolder()
{
    return _T("");
}

DString MainForm::GetSkinFile()
{
    // Pure code mode: no layout XML is loaded
    return _T("");
}

void MainForm::GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs)
{
    // Window attributes correspond to the <Window> attributes in hello.xml
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

    // Build the UI in handwritten pure code (corresponding to the hello.xml layout)
    auto* pRoot = ui::Create<ui::VBox>(this, {});
    pRoot->SetBkColor(_T("bk_wnd_darkcolor"));

    // Title bar area
    auto* pCaption = ui::Create<ui::HBox>(this, {{_T("name"), _T("window_caption_bar")}, {_T("width"), _T("stretch")}, {_T("height"), _T("36")}});
    pCaption->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    auto* pSpacer = ui::Create<ui::Control>(this, {{_T("mouse_enabled"), _T("false")}});
    pCaption->AddItem(pSpacer);

    auto* pFullscreenBtn = ui::Create<ui::Button>(this, {{_T("height"), _T("32")}, {_T("width"), _T("40")}, {_T("margin"), _T("0,2,0,2")}});
    pFullscreenBtn->SetClass(_T("btn_wnd_fullscreen_11"));
    pFullscreenBtn->SetName(_T("fullscreenbtn"));
    pFullscreenBtn->SetToolTipText(_T("Fullscreen, press ESC to exit fullscreen"));
    pCaption->AddItem(pFullscreenBtn);

    auto* pMinBtn = ui::Create<ui::Button>(this, {{_T("height"), _T("32")}, {_T("width"), _T("40")}, {_T("margin"), _T("0,2,0,2")}});
    pMinBtn->SetClass(_T("btn_wnd_min_11"));
    pMinBtn->SetName(_T("minbtn"));
    pMinBtn->SetToolTipText(_T("Minimize"));
    pCaption->AddItem(pMinBtn);

    auto* pMaxBox = ui::Create<ui::Box>(this, {{_T("height"), _T("stretch")}, {_T("width"), _T("40")}, {_T("margin"), _T("0,2,0,2")}});
    pCaption->AddItem(pMaxBox);

    auto* pMaxBtn = ui::Create<ui::Button>(this, {{_T("height"), _T("32")}, {_T("width"), _T("stretch")}});
    pMaxBtn->SetClass(_T("btn_wnd_max_11"));
    pMaxBtn->SetName(_T("maxbtn"));
    pMaxBtn->SetToolTipText(_T("Maximize"));
    pMaxBox->AddItem(pMaxBtn);

    auto* pRestoreBtn = ui::Create<ui::Button>(this, {{_T("height"), _T("32")}, {_T("width"), _T("stretch")}});
    pRestoreBtn->SetClass(_T("btn_wnd_restore_11"));
    pRestoreBtn->SetName(_T("restorebtn"));
    pRestoreBtn->SetVisible(false);
    pRestoreBtn->SetToolTipText(_T("Restore"));
    pMaxBox->AddItem(pRestoreBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{_T("height"), _T("stretch")}, {_T("width"), _T("40")}, {_T("margin"), _T("0,0,0,2")}});
    pCloseBtn->SetClass(_T("btn_wnd_close_11"));
    pCloseBtn->SetName(_T("closebtn"));
    pCloseBtn->SetToolTipText(_T("Close"));
    pCaption->AddItem(pCloseBtn);

    // Work area
    auto* pContent = ui::Create<ui::Box>(this, {});
    pRoot->AddItem(pContent);

    auto* pCenter = ui::Create<ui::VBox>(this, {{_T("valign"), _T("center")}, {_T("halign"), _T("center")}, {_T("height"), _T("100")}});
    pContent->AddItem(pCenter);

    auto* pLabel = ui::Create<ui::Label>(this, {{_T("height"), _T("40")}, {_T("width"), _T("100%")}, {_T("text_align"), _T("hcenter,vcenter")}, {_T("margin"), _T("0,0,0,16")}});
    pLabel->SetName(_T("hello_label"));
    pLabel->SetText(_T("Hello, dui!"));
    pCenter->AddItem(pLabel);

    auto* pButton = ui::Create<ui::Button>(this, {{_T("halign"), _T("center")}, {_T("width"), _T("90")}, {_T("height"), _T("32")}});
    pButton->SetName(_T("hello_btn"));
    pButton->SetClass(_T("btn_global_blue_80x30"));
    pButton->SetText(_T("Click Me"));
    pButton->AttachClick([this](const ui::EventArgs& /*args*/) {
        ui::Label* pLabel = dynamic_cast<ui::Label*>(FindControl(_T("hello_label")));
        if (pLabel != nullptr) {
            pLabel->SetText(_T("Hello from pure code mode!"));
        }
        return true;
    });
    pCenter->AddItem(pButton);

    AttachBox(pRoot);

    // Set the window icon (the Dock icon on macOS), matching
    // icon="public/caption/logo.ico" in hello.xml
    SetWindowIcon(_T("public/caption/logo.ico"));

    BaseClass::OnInitWindow();
}
