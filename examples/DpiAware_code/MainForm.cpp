//MainForm.cpp
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
    // Corresponding to the <Window> attributes in DpiAware.xml
    attrs.m_bInitSizeDefined = true;
    attrs.m_szInitSize.cx = 900;
    attrs.m_szInitSize.cy = 640;
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

void MainForm::BuildUI()
{
    // Corresponding to the DpiAware.xml layout
    auto* pRoot = ui::Create<ui::VBox>(this, {});
    pRoot->SetBkColor(_T("bk_wnd_darkcolor"));

    // Title bar area
    auto* pCaption = ui::Create<ui::HBox>(this, {{_T("name"), _T("window_caption_bar")}, {_T("width"), _T("stretch")}, {_T("height"), _T("36")}});
    pCaption->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    auto* pTitle = ui::Create<ui::Label>(this, {{_T("width"), _T("100%")}, {_T("height"), _T("100%")}, {_T("mouse_enabled"), _T("false")}, {_T("text_padding"), _T("10,0,0,0")}, {_T("text_align"), _T("vcenter")}});
    pTitle->SetName(_T("title"));
    pCaption->AddItem(pTitle);

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

    auto* pGroupBox = ui::Create<ui::GroupVBox>(this, {{_T("halign"), _T("center")}, {_T("valign"), _T("center")}, {_T("width"), _T("640")}, {_T("height"), _T("auto")}});
    pGroupBox->SetName(_T("group_box_test"));
    pContent->AddItem(pGroupBox);

    // Title row
    auto* pRow1 = ui::Create<ui::HBox>(this, {{_T("height"), _T("40")}, {_T("width"), _T("100%")}, {_T("valign"), _T("center")}, {_T("child_halign"), _T("center")}});
    pGroupBox->AddItem(pRow1);

    auto* pLabel = ui::Create<ui::Label>(this, {{_T("height"), _T("40")}, {_T("width"), _T("auto")}, {_T("text_align"), _T("right,vcenter")}, {_T("width"), _T("auto")}, {_T("text_align"), _T("left,vcenter")}, {_T("width"), _T("100%")}, {_T("text_align"), _T("left,vcenter")}, {_T("width"), _T("auto")}, {_T("text_align"), _T("left,vcenter")}, {_T("width"), _T("100%")}, {_T("text_align"), _T("left,vcenter")}, {_T("width"), _T("auto")}, {_T("text_align"), _T("left,vcenter")}, {_T("width"), _T("100%")}, {_T("text_align"), _T("left,vcenter")}, {_T("width"), _T("auto")}, {_T("text_align"), _T("left,vcenter")}, {_T("width"), _T("auto")}, {_T("text_align"), _T("left,vcenter")}, {_T("width"), _T("auto")}, {_T("text_align"), _T("left,vcenter")}, {_T("width"), _T("auto")}, {_T("text_align"), _T("left,vcenter")}, {_T("height"), _T("32")}, {_T("valign"), _T("center")}, {_T("text_align"), _T("left,vcenter")}, {_T("margin"), _T("4,0,0,0")}});
    pLabel->SetText(_T("DPI-Aware Application Example (High-DPI Support)"));
    pRow1->AddItem(pLabel);

    auto* pGroupPos = ui::Create<ui::Label>(this, {{_T("height"), _T("40")}, {_T("width"), _T("auto")}, {_T("text_align"), _T("left,vcenter")}});
    pGroupPos->SetName(_T("group_box_pos"));
    pGroupPos->SetText(_T("[left: 0, top: 0]"));
    pRow1->AddItem(pGroupPos);

    // DPI awareness mode row
    auto* pRow2 = ui::Create<ui::HBox>(this, {{_T("height"), _T("auto")}, {_T("width"), _T("100%")}, {_T("valign"), _T("center")}, {_T("padding"), _T("20,0,0,0")}});
    pGroupBox->AddItem(pRow2);

    pLabel = new ui::Label(this);
    pLabel->SetText(_T("Current process DPI awareness mode:"));
    pRow2->AddItem(pLabel);

    pLabel = new ui::Label(this);
    pLabel->SetName(_T("dpi_awareness"));
    pLabel->SetText(_T("PROCESS_DPI_UNAWARE"));
    pRow2->AddItem(pLabel);

    // Primary display scale row
    auto* pRow3 = ui::Create<ui::HBox>(this, {{_T("height"), _T("auto")}, {_T("width"), _T("100%")}, {_T("valign"), _T("center")}, {_T("padding"), _T("20,0,0,0")}});
    pGroupBox->AddItem(pRow3);

    pLabel = new ui::Label(this);
    pLabel->SetText(_T("Primary monitor display scale:"));
    pRow3->AddItem(pLabel);

    pLabel = new ui::Label(this);
    pLabel->SetName(_T("primary_monitor_display_scale"));
    pLabel->SetText(_T("200%"));
    pRow3->AddItem(pLabel);

    // Window scale row
    auto* pRow4 = ui::Create<ui::HBox>(this, {{_T("height"), _T("auto")}, {_T("width"), _T("100%")}, {_T("valign"), _T("center")}, {_T("padding"), _T("20,0,0,0")}});
    pGroupBox->AddItem(pRow4);

    pLabel = new ui::Label(this);
    pLabel->SetText(_T("Current window UI scale:"));
    pRow4->AddItem(pLabel);

    pLabel = new ui::Label(this);
    pLabel->SetName(_T("window_display_scale"));
    pLabel->SetText(_T("200%"));
    pRow4->AddItem(pLabel);

    // Window position and size
    auto* pSizeVBox = ui::Create<ui::VBox>(this, {{_T("height"), _T("auto")}, {_T("width"), _T("100%")}, {_T("valign"), _T("center")}, {_T("padding"), _T("20,0,0,0")}});
    pGroupBox->AddItem(pSizeVBox);

    auto* pRow5 = ui::Create<ui::HBox>(this, {{_T("height"), _T("auto")}});
    pSizeVBox->AddItem(pRow5);

    pLabel = new ui::Label(this);
    pLabel->SetText(_T("Current Window Position and Size:"));
    pRow5->AddItem(pLabel);

    pLabel = new ui::Label(this);
    pLabel->SetName(_T("window_size"));
    pLabel->SetText(_T("L:0,T:0,W:0,H:0"));
    pRow5->AddItem(pLabel);

    auto* pRow6 = ui::Create<ui::HBox>(this, {{_T("height"), _T("auto")}});
    pSizeVBox->AddItem(pRow6);

    pLabel = new ui::Label(this);
    pLabel->SetText(_T("Current Window Client Size:"));
    pRow6->AddItem(pLabel);

    pLabel = new ui::Label(this);
    pLabel->SetName(_T("window_client_size"));
    pLabel->SetText(_T("L:0,T:0,W:0,H:0"));
    pRow6->AddItem(pLabel);

    // SDL info area
    auto* pSDLVBox = ui::Create<ui::VBox>(this, {{_T("height"), _T("auto")}, {_T("width"), _T("100%")}, {_T("valign"), _T("center")}, {_T("padding"), _T("20,0,0,0")}, {_T("margin"), _T("0,4,0,0")}});
    pSDLVBox->SetName(_T("SDL"));
    pGroupBox->AddItem(pSDLVBox);

    struct SdlRow { DString name; DString label; DString init; };
    const SdlRow sdlRows[] = {
        { _T("SDL_GetWindowSize"), _T("SDL_GetWindowSize："), _T("W:0,H:0") },
        { _T("SDL_GetWindowSizeInPixels"), _T("SDL_GetWindowSizeInPixels："), _T("W:0,H:0") },
        { _T("SDL_GetDisplayContentScale"), _T("SDL_GetDisplayContentScale："), _T("0") },
        { _T("SDL_GetWindowDisplayScale"), _T("SDL_GetWindowDisplayScale："), _T("0") },
        { _T("SDL_GetWindowPixelDensity"), _T("SDL_GetWindowPixelDensity："), _T("0") },
    };
    for (const auto& row : sdlRows) {
    auto* pSdlRow = ui::Create<ui::HBox>(this, {{_T("height"), _T("auto")}});
        pSDLVBox->AddItem(pSdlRow);

    auto* pSdlLabel = ui::Create<ui::Label>(this, {{_T("width"), _T("300")}, {_T("text_align"), _T("right,vcenter")}});
        pSdlLabel->SetText(row.label);
        pSdlRow->AddItem(pSdlLabel);

    auto* pSdlValue = ui::Create<ui::Label>(this, {{_T("width"), _T("auto")}, {_T("text_align"), _T("left,vcenter")}});
        pSdlValue->SetName(row.name);
        pSdlValue->SetText(row.init);
        pSdlRow->AddItem(pSdlValue);
    }

    // Rich text area
    auto* pRichRow = ui::Create<ui::HBox>(this, {{_T("height"), _T("auto")}, {_T("width"), _T("100%")}, {_T("valign"), _T("center")}, {_T("margin"), _T("0,8,0,8")}});
    pGroupBox->AddItem(pRichRow);

    auto* pRichText = ui::Create<ui::RichText>(this, {{_T("row_spacing_mul"), _T("1.5")}, {_T("width"), _T("100%")}, {_T("height"), _T("auto")}, {_T("margin"), _T("4,0,4,0")}});
    pRichText->SetClass(_T("rich_text"));
    pRichText->SetBkColor(_T("green"));
    pRichRow->AddItem(pRichText);

    // Rich text content (corresponding to the <RichText> content in DpiAware.xml)
    ui::WindowBuilder::ParseRichTextXmlText(_T("<RichText>")
        _T("RichText text: <b>bold, <font color=\"#FF0000\">bold red, </font></b>")
        _T("<font face=\"Microsoft YaHei\" size=\"16\">Microsoft YaHei 16pt demo, </font>")
        _T("<font face=\"Microsoft YaHei\" size=\"14\">Microsoft YaHei 14pt demo, </font>")
        _T("<font face=\"Microsoft YaHei\" size=\"12\">Microsoft YaHei 12pt demo, </font>")
        _T("<font face=\"Microsoft YaHei\" size=\"10\">Microsoft YaHei 10pt demo, </font>")
        _T("<font face=\"Microsoft YaHei\" size=\"8\">Microsoft YaHei 8pt demo, </font>")
        _T("<i>italic demo, </i><b>bold demo, </b><s>strikethrough demo, </s><u>underline demo</u>")
        _T("</RichText>"), pRichText);

    // Scale adjustment row
    auto* pScaleRow = ui::Create<ui::HBox>(this, {{_T("height"), _T("auto")}});
    pGroupBox->AddItem(pScaleRow);

    auto* pLeftCol = ui::Create<ui::VBox>(this, {{_T("height"), _T("auto")}});
    pScaleRow->AddItem(pLeftCol);

    auto* pScaleInner = ui::Create<ui::HBox>(this, {{_T("height"), _T("auto")}, {_T("valign"), _T("center")}});
    pLeftCol->AddItem(pScaleInner);

    pLabel = new ui::Label(this);
    pLabel->SetText(_T("Adjust window UI scale (60-500):"));
    pScaleInner->AddItem(pLabel);

    auto* pScaleEdit = ui::Create<ui::RichEdit>(this, {{_T("text"), _T("")}, {_T("number"), _T("true")}, {_T("height"), _T("32")}, {_T("width"), _T("60")}, {_T("max_char"), _T("4")}, {_T("min_number"), _T("0")}, {_T("max_number"), _T("500")}, {_T("valign"), _T("center")}, {_T("text_align"), _T("hcenter,vcenter")}, {_T("margin"), _T("1,0,4,0")}});
    pScaleEdit->SetClass(_T("simple simple_border"));
    pScaleEdit->SetName(_T("display_scale_factor"));
    pScaleEdit->SetBkColor(_T("white"));
    pScaleInner->AddItem(pScaleEdit);

    auto* pSetScaleBtn = ui::Create<ui::Button>(this, {{_T("height"), _T("32")}, {_T("width"), _T("auto")}, {_T("valign"), _T("center")}, {_T("text_padding"), _T("10,0,10,0")}});
    pSetScaleBtn->SetClass(_T("btn_global_blue_80x30"));
    pSetScaleBtn->SetName(_T("set_display_scale_factor"));
    pSetScaleBtn->SetText(_T("Change Display Scale"));
    pScaleInner->AddItem(pSetScaleBtn);

    auto* pNewWndRow = ui::Create<ui::HBox>(this, {{_T("height"), _T("40")}, {_T("valign"), _T("center")}, {_T("halign"), _T("center")}, {_T("margin"), _T("160,40,1,1")}, {_T("child_halign"), _T("left")}});
    pLeftCol->AddItem(pNewWndRow);

    auto* pNewWndBtn = ui::Create<ui::Button>(this, {{_T("width"), _T("auto")}, {_T("text_padding"), _T("10,0,10,0")}});
    pNewWndBtn->SetClass(_T("btn_global_blue_80x30"));
    pNewWndBtn->SetName(_T("NewWindow"));
    pNewWndBtn->SetText(_T("Create New Window"));
    pNewWndRow->AddItem(pNewWndBtn);

    // Right-side image
    auto* pRightCol = ui::Create<ui::VBox>(this, {{_T("border_size"), _T("1")}, {_T("border_color"), _T("blue")}, {_T("width"), _T("auto")}, {_T("height"), _T("auto")}, {_T("margin"), _T("4,4,4,4")}});
    pScaleRow->AddItem(pRightCol);

    auto* pImage = ui::Create<ui::Control>(this, {{_T("width"), _T("auto")}, {_T("height"), _T("auto")}, {_T("halign"), _T("center")}, {_T("valign"), _T("center")}, {_T("margin"), _T("2,2,2,2")}, {_T("border_size"), _T("1")}, {_T("border_color"), _T("red")}});
    pImage->SetBkImage(_T("autumn.png"));
    pRightCol->AddItem(pImage);

    AttachBox(pRoot);
}

void MainForm::OnInitWindow()
{
    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);

    BuildUI();

    // Window initialization is complete; this form can now be initialized
    UpdateUI();

    ui::Button* pButton = dynamic_cast<ui::Button*>(FindControl(_T("NewWindow")));
    if (pButton != nullptr) {
        pButton->AttachClick([this](const ui::EventArgs& /*args*/) {
            // Pop up a new window
            MainForm* window = new MainForm();
            ui::WindowCreateParam createParam;
            createParam.m_dwStyle = ui::kWS_POPUP | ui::kWS_VISIBLE;
            createParam.m_dwExStyle = ui::kWS_EX_LAYERED;
            createParam.m_windowTitle = _T("DpiAware");
            createParam.m_bCenterWindow = true;
            window->CreateWnd(this, createParam);
            window->ShowWindow(ui::kSW_SHOW_NORMAL);
            return true;
            });
    }

    pButton = dynamic_cast<ui::Button*>(FindControl(_T("set_display_scale_factor")));
    if (pButton != nullptr) {
        pButton->AttachClick([this](const ui::EventArgs& /*args*/) {
            // Change the UI display scale
            ui::RichEdit* pRichEdit = dynamic_cast<ui::RichEdit*>(FindControl(_T("display_scale_factor")));
            if (pRichEdit != nullptr) {
                int32_t nNewDisplayScaleFactor = ui::StringUtil::StringToInt32(pRichEdit->GetText());
                if (nNewDisplayScaleFactor > 0) {
                    this->ChangeDisplayScale((uint32_t)nNewDisplayScaleFactor);
                    UpdateUI();
                }
            }
            return true;
            });
    }

    ui::Control* pGroupTest = FindControl(_T("group_box_test"));
    if (pGroupTest != nullptr) {
        pGroupTest->AttachPosChanged([this](const ui::EventArgs& /*args*/) {
            UpdateUI();
            return true;
            });
    }

    BaseClass::OnInitWindow();
}

void MainForm::OnWindowDisplayScaleChanged(uint32_t nOldScaleFactor, uint32_t nNewScaleFactor)
{
    ui::WindowImplBase::OnWindowDisplayScaleChanged(nOldScaleFactor, nNewScaleFactor);
    UpdateUI();
}

LRESULT MainForm::OnSizeMsg(ui::WindowSizeType sizeType, const ui::UiSize& newWindowSize, const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    LRESULT lResult = ui::WindowImplBase::OnSizeMsg(sizeType, newWindowSize, nativeMsg, bHandled);
    UpdateUI();
    return lResult;
}

void MainForm::UpdateUI()
{
    ui::Label* pLabel = dynamic_cast<ui::Label*>(FindControl(_T("dpi_awareness")));
    if (pLabel != nullptr) {
        DString text;
        ui::DpiAwarenessMode mode = ui::GlobalManager::Instance().Dpi().GetDpiAwareness();
        if (mode == ui::DpiAwarenessMode::kDpiUnaware) {
            text = _T("kDpiUnaware");
        }
        else if (mode == ui::DpiAwarenessMode::kSystemDpiAware) {
            text = _T("kSystemDpiAware");
        }
        else if (mode == ui::DpiAwarenessMode::kPerMonitorDpiAware) {
            text = _T("kPerMonitorDpiAware");
        }
        else if (mode == ui::DpiAwarenessMode::kPerMonitorDpiAware_V2) {
            text = _T("kPerMonitorDpiAware_V2");
        }
        pLabel->SetText(text);
    }

    const ui::Control* pGroupTest = FindControl(_T("group_box_test"));
    pLabel = dynamic_cast<ui::Label*>(FindControl(_T("group_box_pos")));
    if ((pLabel != nullptr) && (pGroupTest != nullptr)) {
        DString text = ui::StringUtil::Printf(_T("[left: %d, top: %d]"), pGroupTest->GetRect().left, pGroupTest->GetRect().top);
        pLabel->SetText(text);
    }

    pLabel = dynamic_cast<ui::Label*>(FindControl(_T("primary_monitor_display_scale")));
    if (pLabel != nullptr) {
        uint32_t nScaleFactor = ui::GlobalManager::Instance().Dpi().GetDisplayScaleFactor();
        float fScale = ui::GlobalManager::Instance().Dpi().GetDisplayScale();
        DString text = ui::StringUtil::Printf(_T("%d%% (DisplayScale: %.02f)"), nScaleFactor, fScale);
        pLabel->SetText(text);
    }
    pLabel = dynamic_cast<ui::Label*>(FindControl(_T("window_display_scale")));
    if (pLabel != nullptr) {
        uint32_t nScaleFactor = Dpi().GetDisplayScaleFactor();
        float fScale = Dpi().GetDisplayScale();
        DString text = ui::StringUtil::Printf(_T("%d%% (DisplayScale: %.02f)"), nScaleFactor, fScale);
        pLabel->SetText(text);
    }
    ui::RichEdit* pRichEdit = dynamic_cast<ui::RichEdit*>(FindControl(_T("display_scale_factor")));
    if (pRichEdit != nullptr) {
        uint32_t nScaleFactor = Dpi().GetDisplayScaleFactor();
        pRichEdit->SetTextNumber((int64_t)nScaleFactor);
    }
    pLabel = dynamic_cast<ui::Label*>(FindControl(_T("window_size")));
    if (pLabel != nullptr) {
        ui::UiRect rcWindow;
        GetWindowRect(rcWindow);
        DString text = ui::StringUtil::Printf(_T("W:%d, H:%d [Left:%d, Top:%d]"), rcWindow.Width(), rcWindow.Height(), rcWindow.left, rcWindow.top);
        pLabel->SetText(text);
    }
    pLabel = dynamic_cast<ui::Label*>(FindControl(_T("window_client_size")));
    if (pLabel != nullptr) {
        ui::UiRect rcClient;
        GetClientRect(rcClient);
        DString text = ui::StringUtil::Printf(_T("W:%d, H:%d [Left:%d, Top:%d]"), rcClient.Width(), rcClient.Height(), rcClient.left, rcClient.top);
        pLabel->SetText(text);
    }
#ifdef DUI_BUILD_FOR_SDL
    pLabel = dynamic_cast<ui::Label*>(FindControl(_T("SDL_GetWindowSize")));
    if (pLabel != nullptr) {
        int32_t w = 0;
        int32_t h = 0;
        NativeWnd()->GetWindowSize(&w, &h);
        DString text = ui::StringUtil::Printf(_T("W:%d, H:%d"), w, h);
        pLabel->SetText(text);
    }
    pLabel = dynamic_cast<ui::Label*>(FindControl(_T("SDL_GetWindowSizeInPixels")));
    if (pLabel != nullptr) {
        int32_t w = 0;
        int32_t h = 0;
        NativeWnd()->GetWindowSizeInPixels(&w, &h);
        DString text = ui::StringUtil::Printf(_T("W:%d, H:%d"), w, h);
        pLabel->SetText(text);
    }
    pLabel = dynamic_cast<ui::Label*>(FindControl(_T("SDL_GetDisplayContentScale")));
    if (pLabel != nullptr) {
        float scale = NativeWnd()->GetDisplayContentScale();
        DString text = ui::StringUtil::Printf(_T("%.02f"), scale);
        pLabel->SetText(text);
    }
    pLabel = dynamic_cast<ui::Label*>(FindControl(_T("SDL_GetWindowDisplayScale")));
    if (pLabel != nullptr) {
        float scale = NativeWnd()->GetWindowDisplayScale();
        DString text = ui::StringUtil::Printf(_T("%.02f"), scale);
        pLabel->SetText(text);
    }
    pLabel = dynamic_cast<ui::Label*>(FindControl(_T("SDL_GetWindowPixelDensity")));
    if (pLabel != nullptr) {
        float scale = NativeWnd()->GetWindowPixelDensity();
        DString text = ui::StringUtil::Printf(_T("%.02f"), scale);
        pLabel->SetText(text);
    }
#else
    ui::Control* pSDL = FindControl(_T("SDL"));
    if (pSDL != nullptr) {
        pSDL->SetVisible(false);
    }
#endif
}
