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
    ui::VBox* pRoot = new ui::VBox(this);
    pRoot->SetBkColor(_T("bk_wnd_darkcolor"));

    // Title bar area
    ui::HBox* pCaption = new ui::HBox(this);
    pCaption->SetAttribute(_T("name"), _T("window_caption_bar"));
    pCaption->SetAttribute(_T("width"), _T("stretch"));
    pCaption->SetAttribute(_T("height"), _T("36"));
    pCaption->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    ui::Control* pSpacer = new ui::Control(this);
    pSpacer->SetAttribute(_T("mouse_enabled"), _T("false"));
    pCaption->AddItem(pSpacer);

    ui::Button* pFullscreenBtn = new ui::Button(this);
    pFullscreenBtn->SetClass(_T("btn_wnd_fullscreen_11"));
    pFullscreenBtn->SetAttribute(_T("height"), _T("32"));
    pFullscreenBtn->SetAttribute(_T("width"), _T("40"));
    pFullscreenBtn->SetName(_T("fullscreenbtn"));
    pFullscreenBtn->SetAttribute(_T("margin"), _T("0,2,0,2"));
    pFullscreenBtn->SetToolTipText(_T("Fullscreen, press ESC to exit fullscreen"));
    pCaption->AddItem(pFullscreenBtn);

    ui::Button* pMinBtn = new ui::Button(this);
    pMinBtn->SetClass(_T("btn_wnd_min_11"));
    pMinBtn->SetAttribute(_T("height"), _T("32"));
    pMinBtn->SetAttribute(_T("width"), _T("40"));
    pMinBtn->SetName(_T("minbtn"));
    pMinBtn->SetAttribute(_T("margin"), _T("0,2,0,2"));
    pMinBtn->SetToolTipText(_T("Minimize"));
    pCaption->AddItem(pMinBtn);

    ui::Box* pMaxBox = new ui::Box(this);
    pMaxBox->SetAttribute(_T("height"), _T("stretch"));
    pMaxBox->SetAttribute(_T("width"), _T("40"));
    pMaxBox->SetAttribute(_T("margin"), _T("0,2,0,2"));
    pCaption->AddItem(pMaxBox);

    ui::Button* pMaxBtn = new ui::Button(this);
    pMaxBtn->SetClass(_T("btn_wnd_max_11"));
    pMaxBtn->SetAttribute(_T("height"), _T("32"));
    pMaxBtn->SetAttribute(_T("width"), _T("stretch"));
    pMaxBtn->SetName(_T("maxbtn"));
    pMaxBtn->SetToolTipText(_T("Maximize"));
    pMaxBox->AddItem(pMaxBtn);

    ui::Button* pRestoreBtn = new ui::Button(this);
    pRestoreBtn->SetClass(_T("btn_wnd_restore_11"));
    pRestoreBtn->SetAttribute(_T("height"), _T("32"));
    pRestoreBtn->SetAttribute(_T("width"), _T("stretch"));
    pRestoreBtn->SetName(_T("restorebtn"));
    pRestoreBtn->SetVisible(false);
    pRestoreBtn->SetToolTipText(_T("Restore"));
    pMaxBox->AddItem(pRestoreBtn);

    ui::Button* pCloseBtn = new ui::Button(this);
    pCloseBtn->SetClass(_T("btn_wnd_close_11"));
    pCloseBtn->SetAttribute(_T("height"), _T("stretch"));
    pCloseBtn->SetAttribute(_T("width"), _T("40"));
    pCloseBtn->SetName(_T("closebtn"));
    pCloseBtn->SetAttribute(_T("margin"), _T("0,0,0,2"));
    pCloseBtn->SetToolTipText(_T("Close"));
    pCaption->AddItem(pCloseBtn);

    // Work area
    ui::Box* pContent = new ui::Box(this);
    pRoot->AddItem(pContent);

    ui::VBox* pCenter = new ui::VBox(this);
    pCenter->SetAttribute(_T("valign"), _T("center"));
    pCenter->SetAttribute(_T("halign"), _T("center"));
    pCenter->SetAttribute(_T("height"), _T("100"));
    pContent->AddItem(pCenter);

    ui::Label* pLabel = new ui::Label(this);
    pLabel->SetName(_T("hello_label"));
    pLabel->SetText(_T("Hello, nim_duilib!"));
    pLabel->SetAttribute(_T("height"), _T("40"));
    pLabel->SetAttribute(_T("width"), _T("100%"));
    pLabel->SetAttribute(_T("text_align"), _T("hcenter,vcenter"));
    pLabel->SetAttribute(_T("margin"), _T("0,0,0,16"));
    pCenter->AddItem(pLabel);

    ui::Button* pButton = new ui::Button(this);
    pButton->SetName(_T("hello_btn"));
    pButton->SetClass(_T("btn_global_blue_80x30"));
    pButton->SetText(_T("Click Me"));
    pButton->SetAttribute(_T("halign"), _T("center"));
    pButton->SetAttribute(_T("width"), _T("90"));
    pButton->SetAttribute(_T("height"), _T("32"));
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
