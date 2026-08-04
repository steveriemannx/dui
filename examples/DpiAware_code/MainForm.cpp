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
    ui::VBox* pRoot = new ui::VBox(this);
    pRoot->SetBkColor(_T("bk_wnd_darkcolor"));

    // Title bar area
    ui::HBox* pCaption = new ui::HBox(this);
    pCaption->SetAttribute(_T("name"), _T("window_caption_bar"));
    pCaption->SetAttribute(_T("width"), _T("stretch"));
    pCaption->SetAttribute(_T("height"), _T("36"));
    pCaption->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    ui::Label* pTitle = new ui::Label(this);
    pTitle->SetName(_T("title"));
    pTitle->SetAttribute(_T("width"), _T("100%"));
    pTitle->SetAttribute(_T("height"), _T("100%"));
    pTitle->SetAttribute(_T("mouse_enabled"), _T("false"));
    pTitle->SetAttribute(_T("text_padding"), _T("10,0,0,0"));
    pTitle->SetAttribute(_T("text_align"), _T("vcenter"));
    pCaption->AddItem(pTitle);

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

    ui::GroupVBox* pGroupBox = new ui::GroupVBox(this);
    pGroupBox->SetName(_T("group_box_test"));
    pGroupBox->SetAttribute(_T("halign"), _T("center"));
    pGroupBox->SetAttribute(_T("valign"), _T("center"));
    pGroupBox->SetAttribute(_T("width"), _T("640"));
    pGroupBox->SetAttribute(_T("height"), _T("auto"));
    pContent->AddItem(pGroupBox);

    // Title row
    ui::HBox* pRow1 = new ui::HBox(this);
    pRow1->SetAttribute(_T("height"), _T("40"));
    pRow1->SetAttribute(_T("width"), _T("100%"));
    pRow1->SetAttribute(_T("valign"), _T("center"));
    pRow1->SetAttribute(_T("child_halign"), _T("center"));
    pGroupBox->AddItem(pRow1);

    ui::Label* pLabel = new ui::Label(this);
    pLabel->SetText(_T("DPI-Aware Application Example (High-DPI Support)"));
    pLabel->SetAttribute(_T("height"), _T("40"));
    pLabel->SetAttribute(_T("width"), _T("auto"));
    pLabel->SetAttribute(_T("text_align"), _T("right,vcenter"));
    pRow1->AddItem(pLabel);

    ui::Label* pGroupPos = new ui::Label(this);
    pGroupPos->SetName(_T("group_box_pos"));
    pGroupPos->SetText(_T("[left: 0, top: 0]"));
    pGroupPos->SetAttribute(_T("height"), _T("40"));
    pGroupPos->SetAttribute(_T("width"), _T("auto"));
    pGroupPos->SetAttribute(_T("text_align"), _T("left,vcenter"));
    pRow1->AddItem(pGroupPos);

    // DPI awareness mode row
    ui::HBox* pRow2 = new ui::HBox(this);
    pRow2->SetAttribute(_T("height"), _T("auto"));
    pRow2->SetAttribute(_T("width"), _T("100%"));
    pRow2->SetAttribute(_T("valign"), _T("center"));
    pRow2->SetAttribute(_T("padding"), _T("20,0,0,0"));
    pGroupBox->AddItem(pRow2);

    pLabel = new ui::Label(this);
    pLabel->SetText(_T("Current process DPI awareness mode:"));
    pLabel->SetAttribute(_T("width"), _T("auto"));
    pLabel->SetAttribute(_T("text_align"), _T("left,vcenter"));
    pRow2->AddItem(pLabel);

    pLabel = new ui::Label(this);
    pLabel->SetName(_T("dpi_awareness"));
    pLabel->SetText(_T("PROCESS_DPI_UNAWARE"));
    pLabel->SetAttribute(_T("width"), _T("100%"));
    pLabel->SetAttribute(_T("text_align"), _T("left,vcenter"));
    pRow2->AddItem(pLabel);

    // Primary display scale row
    ui::HBox* pRow3 = new ui::HBox(this);
    pRow3->SetAttribute(_T("height"), _T("auto"));
    pRow3->SetAttribute(_T("width"), _T("100%"));
    pRow3->SetAttribute(_T("valign"), _T("center"));
    pRow3->SetAttribute(_T("padding"), _T("20,0,0,0"));
    pGroupBox->AddItem(pRow3);

    pLabel = new ui::Label(this);
    pLabel->SetText(_T("Primary monitor display scale:"));
    pLabel->SetAttribute(_T("width"), _T("auto"));
    pLabel->SetAttribute(_T("text_align"), _T("left,vcenter"));
    pRow3->AddItem(pLabel);

    pLabel = new ui::Label(this);
    pLabel->SetName(_T("primary_monitor_display_scale"));
    pLabel->SetText(_T("200%"));
    pLabel->SetAttribute(_T("width"), _T("100%"));
    pLabel->SetAttribute(_T("text_align"), _T("left,vcenter"));
    pRow3->AddItem(pLabel);

    // Window scale row
    ui::HBox* pRow4 = new ui::HBox(this);
    pRow4->SetAttribute(_T("height"), _T("auto"));
    pRow4->SetAttribute(_T("width"), _T("100%"));
    pRow4->SetAttribute(_T("valign"), _T("center"));
    pRow4->SetAttribute(_T("padding"), _T("20,0,0,0"));
    pGroupBox->AddItem(pRow4);

    pLabel = new ui::Label(this);
    pLabel->SetText(_T("Current window UI scale:"));
    pLabel->SetAttribute(_T("width"), _T("auto"));
    pLabel->SetAttribute(_T("text_align"), _T("left,vcenter"));
    pRow4->AddItem(pLabel);

    pLabel = new ui::Label(this);
    pLabel->SetName(_T("window_display_scale"));
    pLabel->SetText(_T("200%"));
    pLabel->SetAttribute(_T("width"), _T("100%"));
    pLabel->SetAttribute(_T("text_align"), _T("left,vcenter"));
    pRow4->AddItem(pLabel);

    // Window position and size
    ui::VBox* pSizeVBox = new ui::VBox(this);
    pSizeVBox->SetAttribute(_T("height"), _T("auto"));
    pSizeVBox->SetAttribute(_T("width"), _T("100%"));
    pSizeVBox->SetAttribute(_T("valign"), _T("center"));
    pSizeVBox->SetAttribute(_T("padding"), _T("20,0,0,0"));
    pGroupBox->AddItem(pSizeVBox);

    ui::HBox* pRow5 = new ui::HBox(this);
    pRow5->SetAttribute(_T("height"), _T("auto"));
    pSizeVBox->AddItem(pRow5);

    pLabel = new ui::Label(this);
    pLabel->SetText(_T("Current Window Position and Size:"));
    pLabel->SetAttribute(_T("width"), _T("auto"));
    pLabel->SetAttribute(_T("text_align"), _T("left,vcenter"));
    pRow5->AddItem(pLabel);

    pLabel = new ui::Label(this);
    pLabel->SetName(_T("window_size"));
    pLabel->SetText(_T("L:0,T:0,W:0,H:0"));
    pLabel->SetAttribute(_T("width"), _T("auto"));
    pLabel->SetAttribute(_T("text_align"), _T("left,vcenter"));
    pRow5->AddItem(pLabel);

    ui::HBox* pRow6 = new ui::HBox(this);
    pRow6->SetAttribute(_T("height"), _T("auto"));
    pSizeVBox->AddItem(pRow6);

    pLabel = new ui::Label(this);
    pLabel->SetText(_T("Current Window Client Size:"));
    pLabel->SetAttribute(_T("width"), _T("auto"));
    pLabel->SetAttribute(_T("text_align"), _T("left,vcenter"));
    pRow6->AddItem(pLabel);

    pLabel = new ui::Label(this);
    pLabel->SetName(_T("window_client_size"));
    pLabel->SetText(_T("L:0,T:0,W:0,H:0"));
    pLabel->SetAttribute(_T("width"), _T("auto"));
    pLabel->SetAttribute(_T("text_align"), _T("left,vcenter"));
    pRow6->AddItem(pLabel);

    // SDL info area
    ui::VBox* pSDLVBox = new ui::VBox(this);
    pSDLVBox->SetName(_T("SDL"));
    pSDLVBox->SetAttribute(_T("height"), _T("auto"));
    pSDLVBox->SetAttribute(_T("width"), _T("100%"));
    pSDLVBox->SetAttribute(_T("valign"), _T("center"));
    pSDLVBox->SetAttribute(_T("padding"), _T("20,0,0,0"));
    pSDLVBox->SetAttribute(_T("margin"), _T("0,4,0,0"));
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
        ui::HBox* pSdlRow = new ui::HBox(this);
        pSdlRow->SetAttribute(_T("height"), _T("auto"));
        pSDLVBox->AddItem(pSdlRow);

        ui::Label* pSdlLabel = new ui::Label(this);
        pSdlLabel->SetText(row.label);
        pSdlLabel->SetAttribute(_T("width"), _T("300"));
        pSdlLabel->SetAttribute(_T("text_align"), _T("right,vcenter"));
        pSdlRow->AddItem(pSdlLabel);

        ui::Label* pSdlValue = new ui::Label(this);
        pSdlValue->SetName(row.name);
        pSdlValue->SetText(row.init);
        pSdlValue->SetAttribute(_T("width"), _T("auto"));
        pSdlValue->SetAttribute(_T("text_align"), _T("left,vcenter"));
        pSdlRow->AddItem(pSdlValue);
    }

    // Rich text area
    ui::HBox* pRichRow = new ui::HBox(this);
    pRichRow->SetAttribute(_T("height"), _T("auto"));
    pRichRow->SetAttribute(_T("width"), _T("100%"));
    pRichRow->SetAttribute(_T("valign"), _T("center"));
    pRichRow->SetAttribute(_T("margin"), _T("0,8,0,8"));
    pGroupBox->AddItem(pRichRow);

    ui::RichText* pRichText = new ui::RichText(this);
    pRichText->SetClass(_T("rich_text"));
    pRichText->SetBkColor(_T("green"));
    pRichText->SetAttribute(_T("row_spacing_mul"), _T("1.5"));
    pRichText->SetAttribute(_T("width"), _T("100%"));
    pRichText->SetAttribute(_T("height"), _T("auto"));
    pRichText->SetAttribute(_T("margin"), _T("4,0,4,0"));
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
    ui::HBox* pScaleRow = new ui::HBox(this);
    pScaleRow->SetAttribute(_T("height"), _T("auto"));
    pGroupBox->AddItem(pScaleRow);

    ui::VBox* pLeftCol = new ui::VBox(this);
    pLeftCol->SetAttribute(_T("height"), _T("auto"));
    pScaleRow->AddItem(pLeftCol);

    ui::HBox* pScaleInner = new ui::HBox(this);
    pScaleInner->SetAttribute(_T("height"), _T("auto"));
    pScaleInner->SetAttribute(_T("valign"), _T("center"));
    pLeftCol->AddItem(pScaleInner);

    pLabel = new ui::Label(this);
    pLabel->SetText(_T("Adjust window UI scale (60-500):"));
    pLabel->SetAttribute(_T("height"), _T("32"));
    pLabel->SetAttribute(_T("valign"), _T("center"));
    pLabel->SetAttribute(_T("text_align"), _T("left,vcenter"));
    pLabel->SetAttribute(_T("margin"), _T("4,0,0,0"));
    pScaleInner->AddItem(pLabel);

    ui::RichEdit* pScaleEdit = new ui::RichEdit(this);
    pScaleEdit->SetClass(_T("simple simple_border"));
    pScaleEdit->SetName(_T("display_scale_factor"));
    pScaleEdit->SetAttribute(_T("text"), _T(""));
    pScaleEdit->SetAttribute(_T("number"), _T("true"));
    pScaleEdit->SetAttribute(_T("height"), _T("32"));
    pScaleEdit->SetAttribute(_T("width"), _T("60"));
    pScaleEdit->SetAttribute(_T("max_char"), _T("4"));
    pScaleEdit->SetAttribute(_T("min_number"), _T("0"));
    pScaleEdit->SetAttribute(_T("max_number"), _T("500"));
    pScaleEdit->SetAttribute(_T("valign"), _T("center"));
    pScaleEdit->SetAttribute(_T("text_align"), _T("hcenter,vcenter"));
    pScaleEdit->SetBkColor(_T("white"));
    pScaleEdit->SetAttribute(_T("margin"), _T("1,0,4,0"));
    pScaleInner->AddItem(pScaleEdit);

    ui::Button* pSetScaleBtn = new ui::Button(this);
    pSetScaleBtn->SetClass(_T("btn_global_blue_80x30"));
    pSetScaleBtn->SetAttribute(_T("height"), _T("32"));
    pSetScaleBtn->SetAttribute(_T("width"), _T("auto"));
    pSetScaleBtn->SetAttribute(_T("valign"), _T("center"));
    pSetScaleBtn->SetName(_T("set_display_scale_factor"));
    pSetScaleBtn->SetText(_T("Change Display Scale"));
    pSetScaleBtn->SetAttribute(_T("text_padding"), _T("10,0,10,0"));
    pScaleInner->AddItem(pSetScaleBtn);

    ui::HBox* pNewWndRow = new ui::HBox(this);
    pNewWndRow->SetAttribute(_T("height"), _T("40"));
    pNewWndRow->SetAttribute(_T("valign"), _T("center"));
    pNewWndRow->SetAttribute(_T("halign"), _T("center"));
    pNewWndRow->SetAttribute(_T("margin"), _T("160,40,1,1"));
    pNewWndRow->SetAttribute(_T("child_halign"), _T("left"));
    pLeftCol->AddItem(pNewWndRow);

    ui::Button* pNewWndBtn = new ui::Button(this);
    pNewWndBtn->SetClass(_T("btn_global_blue_80x30"));
    pNewWndBtn->SetAttribute(_T("width"), _T("auto"));
    pNewWndBtn->SetName(_T("NewWindow"));
    pNewWndBtn->SetText(_T("Create New Window"));
    pNewWndBtn->SetAttribute(_T("text_padding"), _T("10,0,10,0"));
    pNewWndRow->AddItem(pNewWndBtn);

    // Right-side image
    ui::VBox* pRightCol = new ui::VBox(this);
    pRightCol->SetAttribute(_T("border_size"), _T("1"));
    pRightCol->SetAttribute(_T("border_color"), _T("blue"));
    pRightCol->SetAttribute(_T("width"), _T("auto"));
    pRightCol->SetAttribute(_T("height"), _T("auto"));
    pRightCol->SetAttribute(_T("margin"), _T("4,4,4,4"));
    pScaleRow->AddItem(pRightCol);

    ui::Control* pImage = new ui::Control(this);
    pImage->SetBkImage(_T("autumn.png"));
    pImage->SetAttribute(_T("width"), _T("auto"));
    pImage->SetAttribute(_T("height"), _T("auto"));
    pImage->SetAttribute(_T("halign"), _T("center"));
    pImage->SetAttribute(_T("valign"), _T("center"));
    pImage->SetAttribute(_T("margin"), _T("2,2,2,2"));
    pImage->SetAttribute(_T("border_size"), _T("1"));
    pImage->SetAttribute(_T("border_color"), _T("red"));
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
