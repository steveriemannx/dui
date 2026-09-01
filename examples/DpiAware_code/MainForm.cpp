//MainForm.cpp
#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

void MainForm::SetupWindow()
{
    SetWindowSize(800, 600);
    CenterWindow();
    SetWindowMinimumSize(ui::UiSize(240, 100), true);
    SetShadowAttached(true);
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);
    SetSizeBox(ui::UiRect(4, 4, 4, 4), true);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), true);
    SetWindowIcon("public/caption/logo.ico");
}

void MainForm::BuildUI()
{
    // Corresponding to the DpiAware.xml layout
    auto* pRoot = ui::Create<ui::VBox>(this, {{"bkcolor", "bk_wnd_darkcolor"}, {"visible", "true"}});

    // Title bar area
    auto* pCaption = ui::Create<ui::HBox>(this, {{"name", "window_caption_bar"}, {"width", "stretch"}, {"height", "36"}, {"bkcolor", "bk_wnd_lightcolor"}});
    ui::Attach(pRoot, pCaption);

    auto* pSpacer = ui::Create<ui::Control>(this, {{"mouse_enabled", "false"}});
    ui::Attach(pCaption, pSpacer);

    auto* pFullscreenBtn = ui::Create<ui::Button>(this, {{"class", "btn_wnd_fullscreen_11"}, {"name", "fullscreenbtn"}, {"height", "32"}, {"width", "40"}, {"margin", "0,2,0,2"}, {"tooltip_text", "Fullscreen, press ESC to exit fullscreen"}});
    ui::Attach(pCaption, pFullscreenBtn);

    auto* pMinBtn = ui::Create<ui::Button>(this, {{"class", "btn_wnd_min_11"}, {"name", "minbtn"}, {"height", "32"}, {"width", "40"}, {"margin", "0,2,0,2"}, {"tooltip_text", "Minimize"}});
    ui::Attach(pCaption, pMinBtn);

    auto* pMaxBox = ui::Create<ui::Box>(this, {{"height", "stretch"}, {"width", "40"}, {"margin", "0,2,0,2"}});
    ui::Attach(pCaption, pMaxBox);

    auto* pMaxBtn = ui::Create<ui::Button>(this, {{"class", "btn_wnd_max_11"}, {"name", "maxbtn"}, {"height", "32"}, {"width", "stretch"}, {"tooltip_text", "Maximize"}});
    ui::Attach(pMaxBox, pMaxBtn);

    auto* pRestoreBtn = ui::Create<ui::Button>(this, {{"class", "btn_wnd_restore_11"}, {"name", "restorebtn"}, {"height", "32"}, {"width", "stretch"}, {"visible", "false"}, {"tooltip_text", "Restore"}});
    ui::Attach(pMaxBox, pRestoreBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{"class", "btn_wnd_close_11"}, {"name", "closebtn"}, {"height", "stretch"}, {"width", "40"}, {"margin", "0,0,0,2"}, {"tooltip_text", "Close"}});
    ui::Attach(pCaption, pCloseBtn);

    // Work area
    auto* pContent = ui::Create<ui::Box>(this, {});
    ui::Attach(pRoot, pContent);

    auto* pGroupBox = ui::Create<ui::GroupVBox>(this, {{"name", "group_box_test"}, {"halign", "center"}, {"valign", "center"}, {"width", "640"}, {"height", "auto"}});
    ui::Attach(pContent, pGroupBox);

    // Title row
    auto* pRow1 = ui::Create<ui::HBox>(this, {{"height", "40"}, {"width", "100%"}, {"valign", "center"}, {"child_halign", "center"}});
    ui::Attach(pGroupBox, pRow1);

    auto* pLabel = ui::Create<ui::Label>(this, {{"height", "32"}, {"width", "auto"}, {"valign", "center"}, {"text_align", "left,vcenter"}, {"text", "DPI-Aware Application Example (High-DPI Support)"}, {"margin", "4,0,0,0"}});
    ui::Attach(pRow1, pLabel);

    auto* pGroupPos = ui::Create<ui::Label>(this, {{"name", "group_box_pos"}, {"text", "[left: 0, top: 0]"}, {"height", "40"}, {"width", "auto"}, {"text_align", "left,vcenter"}});
    ui::Attach(pRow1, pGroupPos);

    // DPI awareness mode row
    auto* pRow2 = ui::Create<ui::HBox>(this, {{"height", "auto"}, {"width", "100%"}, {"valign", "center"}, {"padding", "20,0,0,0"}});
    ui::Attach(pGroupBox, pRow2);

    pLabel = ui::Create<ui::Label>(this, {{"text", "Current process DPI awareness mode:"}});
    ui::Attach(pRow2, pLabel);

    pLabel = ui::Create<ui::Label>(this, {{"name", "dpi_awareness"}, {"text", "PROCESS_DPI_UNAWARE"}});
    ui::Attach(pRow2, pLabel);

    // Primary display scale row
    auto* pRow3 = ui::Create<ui::HBox>(this, {{"height", "auto"}, {"width", "100%"}, {"valign", "center"}, {"padding", "20,0,0,0"}});
    ui::Attach(pGroupBox, pRow3);

    pLabel = ui::Create<ui::Label>(this, {{"text", "Primary monitor display scale:"}});
    ui::Attach(pRow3, pLabel);

    pLabel = ui::Create<ui::Label>(this, {{"name", "primary_monitor_display_scale"}, {"text", "200%"}});
    ui::Attach(pRow3, pLabel);

    // Window scale row
    auto* pRow4 = ui::Create<ui::HBox>(this, {{"height", "auto"}, {"width", "100%"}, {"valign", "center"}, {"padding", "20,0,0,0"}});
    ui::Attach(pGroupBox, pRow4);

    pLabel = ui::Create<ui::Label>(this, {{"text", "Current window UI scale:"}});
    ui::Attach(pRow4, pLabel);

    pLabel = ui::Create<ui::Label>(this, {{"name", "window_display_scale"}, {"text", "200%"}});
    ui::Attach(pRow4, pLabel);

    // Window position and size
    auto* pSizeVBox = ui::Create<ui::VBox>(this, {{"height", "auto"}, {"width", "100%"}, {"valign", "center"}, {"padding", "20,0,0,0"}});
    ui::Attach(pGroupBox, pSizeVBox);

    auto* pRow5 = ui::Create<ui::HBox>(this, {{"height", "auto"}});
    ui::Attach(pSizeVBox, pRow5);

    pLabel = ui::Create<ui::Label>(this, {{"text", "Current Window Position and Size:"}});
    ui::Attach(pRow5, pLabel);

    pLabel = ui::Create<ui::Label>(this, {{"name", "window_size"}, {"text", "L:0,T:0,W:0,H:0"}});
    ui::Attach(pRow5, pLabel);

    auto* pRow6 = ui::Create<ui::HBox>(this, {{"height", "auto"}});
    ui::Attach(pSizeVBox, pRow6);

    pLabel = ui::Create<ui::Label>(this, {{"text", "Current Window Client Size:"}});
    ui::Attach(pRow6, pLabel);

    pLabel = ui::Create<ui::Label>(this, {{"name", "window_client_size"}, {"text", "L:0,T:0,W:0,H:0"}});
    ui::Attach(pRow6, pLabel);

    // SDL info area
    auto* pSDLVBox = ui::Create<ui::VBox>(this, {{"name", "SDL"}, {"height", "auto"}, {"width", "100%"}, {"valign", "center"}, {"padding", "20,0,0,0"}, {"margin", "0,4,0,0"}});
    ui::Attach(pGroupBox, pSDLVBox);

    struct SdlRow { DString name; DString label; DString init; };
    const SdlRow sdlRows[] = {
        { "SDL_GetWindowSize", "SDL_GetWindowSize：", "W:0,H:0" },
        { "SDL_GetWindowSizeInPixels", "SDL_GetWindowSizeInPixels：", "W:0,H:0" },
        { "SDL_GetDisplayContentScale", "SDL_GetDisplayContentScale：", "0" },
        { "SDL_GetWindowDisplayScale", "SDL_GetWindowDisplayScale：", "0" },
        { "SDL_GetWindowPixelDensity", "SDL_GetWindowPixelDensity：", "0" },
    };
    for (const auto& row : sdlRows) {
    auto* pSdlRow = ui::Create<ui::HBox>(this, {{"height", "auto"}});
        ui::Attach(pSDLVBox, pSdlRow);

        auto* pSdlLabel = ui::Create<ui::Label>(this, {{"width", "300"}, {"text_align", "right,vcenter"}, {"text", row.label}});
        ui::Attach(pSdlRow, pSdlLabel);

        auto* pSdlValue = ui::Create<ui::Label>(this, {{"name", row.name}, {"text", row.init}, {"width", "auto"}, {"text_align", "left,vcenter"}});
        ui::Attach(pSdlRow, pSdlValue);
    }

    // Rich text area
    auto* pRichRow = ui::Create<ui::HBox>(this, {{"height", "auto"}, {"width", "100%"}, {"valign", "center"}, {"margin", "0,8,0,8"}});
    ui::Attach(pGroupBox, pRichRow);

    auto* pRichText = ui::Create<ui::RichText>(this, {{"class", "rich_text"}, {"bkcolor", "green"}, {"row_spacing_mul", "1.5"}, {"width", "100%"}, {"height", "auto"}, {"margin", "4,0,4,0"}});
    ui::Attach(pRichRow, pRichText);

    // Rich text content (corresponding to the <RichText> content in DpiAware.xml)
    ui::WindowBuilder::ParseRichTextXmlText("<RichText>"
        "RichText text: <b>bold, <font color=\"#FF0000\">bold red, </font></b>"
        "<font face=\"Microsoft YaHei\" size=\"16\">Microsoft YaHei 16pt demo, </font>"
        "<font face=\"Microsoft YaHei\" size=\"14\">Microsoft YaHei 14pt demo, </font>"
        "<font face=\"Microsoft YaHei\" size=\"12\">Microsoft YaHei 12pt demo, </font>"
        "<font face=\"Microsoft YaHei\" size=\"10\">Microsoft YaHei 10pt demo, </font>"
        "<font face=\"Microsoft YaHei\" size=\"8\">Microsoft YaHei 8pt demo, </font>"
        "<i>italic demo, </i><b>bold demo, </b><s>strikethrough demo, </s><u>underline demo</u>"
        "</RichText>", pRichText);

    // Scale adjustment row
    auto* pScaleRow = ui::Create<ui::HBox>(this, {{"height", "auto"}});
    ui::Attach(pGroupBox, pScaleRow);

    auto* pLeftCol = ui::Create<ui::VBox>(this, {{"height", "auto"}});
    ui::Attach(pScaleRow, pLeftCol);

    auto* pScaleInner = ui::Create<ui::HBox>(this, {{"height", "auto"}, {"valign", "center"}});
    ui::Attach(pLeftCol, pScaleInner);

    pLabel = ui::Create<ui::Label>(this, {{"text", "Adjust window UI scale (60-500):"}});
    ui::Attach(pScaleInner, pLabel);

    auto* pScaleEdit = ui::Create<ui::RichEdit>(this, {{"class", "simple simple_border"}, {"name", "display_scale_factor"}, {"bkcolor", "white"}, {"text", ""}, {"number", "true"}, {"height", "32"}, {"width", "60"}, {"max_char", "4"}, {"min_number", "0"}, {"max_number", "500"}, {"valign", "center"}, {"text_align", "hcenter,vcenter"}, {"margin", "1,0,4,0"}});
    ui::Attach(pScaleInner, pScaleEdit);

    auto* pSetScaleBtn = ui::Create<ui::Button>(this, {{"class", "btn_global_blue_80x30"}, {"name", "set_display_scale_factor"}, {"text", "Change Display Scale"}, {"height", "32"}, {"width", "auto"}, {"valign", "center"}, {"text_padding", "10,0,10,0"}});
    ui::Attach(pScaleInner, pSetScaleBtn);

    auto* pNewWndRow = ui::Create<ui::HBox>(this, {{"height", "40"}, {"valign", "center"}, {"halign", "center"}, {"margin", "160,40,1,1"}, {"child_halign", "left"}});
    ui::Attach(pLeftCol, pNewWndRow);

    auto* pNewWndBtn = ui::Create<ui::Button>(this, {{"class", "btn_global_blue_80x30"}, {"name", "NewWindow"}, {"text", "Create New Window"}, {"width", "auto"}, {"text_padding", "10,0,10,0"}});
    ui::Attach(pNewWndRow, pNewWndBtn);

    // Right-side image
    auto* pRightCol = ui::Create<ui::VBox>(this, {{"border_size", "1"}, {"border_color", "blue"}, {"width", "auto"}, {"height", "auto"}, {"margin", "4,4,4,4"}});
    ui::Attach(pScaleRow, pRightCol);

    auto* pImage = ui::Create<ui::Control>(this, {{"bkimage", "autumn.png"}, {"width", "auto"}, {"height", "auto"}, {"halign", "center"}, {"valign", "center"}, {"margin", "2,2,2,2"}, {"border_size", "1"}, {"border_color", "red"}});
    ui::Attach(pRightCol, pImage);

    ui::Attach(this, pRoot);
}

void MainForm::OnInitWindow()
{
    SetupWindow();
    BuildUI();
    BindEvents();
    BaseClass::OnInitWindow();
    UpdateUI();
}

void MainForm::BindEvents()
{
    if (auto* pButton = ui::Find<ui::Button>(this, "NewWindow")) {
        pButton->AttachClick([this](const ui::EventArgs& /*args*/) {
            // Pop up a new window
            MainForm* window = new MainForm();
            window->CreateWnd(this, ui::WindowCreateParam("DpiAware", true));
            window->ShowWindow(ui::kSW_SHOW_NORMAL);
            return true;
            });
    }

    if (auto* pButton = ui::Find<ui::Button>(this, "set_display_scale_factor")) {
        pButton->AttachClick([this](const ui::EventArgs& /*args*/) {
            // Change the UI display scale
            if (auto* pRichEdit = ui::Find<ui::RichEdit>(this, "display_scale_factor")) {
            auto nNewDisplayScaleFactor = ui::StringUtil::StringToInt32(pRichEdit->GetText());
                if (nNewDisplayScaleFactor > 0) {
                    this->ChangeDisplayScale((uint32_t)nNewDisplayScaleFactor);
                    UpdateUI();
                }
            }
            return true;
            });
    }

    if (auto* pGroupTest = ui::Find<ui::Control>(this, "group_box_test")) {
        pGroupTest->AttachPosChanged([this](const ui::EventArgs& /*args*/) {
            UpdateUI();
            return true;
            });
    }
}

void MainForm::OnWindowDisplayScaleChanged(uint32_t nOldScaleFactor, uint32_t nNewScaleFactor)
{
    ui::WindowImplBase::OnWindowDisplayScaleChanged(nOldScaleFactor, nNewScaleFactor);
    UpdateUI();
}

LRESULT MainForm::OnSizeMsg(ui::WindowSizeType sizeType, const ui::UiSize& newWindowSize, const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    auto lResult = ui::WindowImplBase::OnSizeMsg(sizeType, newWindowSize, nativeMsg, bHandled);
    UpdateUI();
    return lResult;
}

void MainForm::UpdateUI()
{
    if (auto* pLabel = ui::Find<ui::Label>(this, "dpi_awareness")) {
        DString text;
        auto mode = ui::GlobalManager::Instance().Dpi().GetDpiAwareness();
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

    const auto* pGroupTest = ui::Find<ui::Control>(this, "group_box_test");
    if (auto* pLabel = ui::Find<ui::Label>(this, "group_box_pos")) {
        if (pGroupTest != nullptr) {
            auto text = ui::StringUtil::Printf(_T("[left: %d, top: %d]"), pGroupTest->GetRect().left, pGroupTest->GetRect().top);
            pLabel->SetText(text);
        }
    }

    if (auto* pLabel = ui::Find<ui::Label>(this, "primary_monitor_display_scale")) {
        auto nScaleFactor = ui::GlobalManager::Instance().Dpi().GetDisplayScaleFactor();
        float fScale = ui::GlobalManager::Instance().Dpi().GetDisplayScale();
        auto text = ui::StringUtil::Printf(_T("%d%% (DisplayScale: %.02f)"), nScaleFactor, fScale);
        pLabel->SetText(text);
    }
    if (auto* pLabel = ui::Find<ui::Label>(this, "window_display_scale")) {
        auto nScaleFactor = Dpi().GetDisplayScaleFactor();
        float fScale = Dpi().GetDisplayScale();
        auto text = ui::StringUtil::Printf(_T("%d%% (DisplayScale: %.02f)"), nScaleFactor, fScale);
        pLabel->SetText(text);
    }
    if (auto* pRichEdit = ui::Find<ui::RichEdit>(this, "display_scale_factor")) {
        auto nScaleFactor = Dpi().GetDisplayScaleFactor();
        pRichEdit->SetTextNumber((int64_t)nScaleFactor);
    }
    if (auto* pLabel = ui::Find<ui::Label>(this, "window_size")) {
        ui::UiRect rcWindow;
        GetWindowRect(rcWindow);
        auto text = ui::StringUtil::Printf(_T("W:%d, H:%d [Left:%d, Top:%d]"), rcWindow.Width(), rcWindow.Height(), rcWindow.left, rcWindow.top);
        pLabel->SetText(text);
    }
    if (auto* pLabel = ui::Find<ui::Label>(this, "window_client_size")) {
        ui::UiRect rcClient;
        GetClientRect(rcClient);
        auto text = ui::StringUtil::Printf(_T("W:%d, H:%d [Left:%d, Top:%d]"), rcClient.Width(), rcClient.Height(), rcClient.left, rcClient.top);
        pLabel->SetText(text);
    }
#ifdef DUI_BUILD_FOR_SDL
    if (auto* pLabel = ui::Find<ui::Label>(this, "SDL_GetWindowSize")) {
        int32_t w = 0;
        int32_t h = 0;
        NativeWnd()->GetWindowSize(&w, &h);
        auto text = ui::StringUtil::Printf("W:%d, H:%d", w, h);
        pLabel->SetText(text);
    }
    if (auto* pLabel = ui::Find<ui::Label>(this, "SDL_GetWindowSizeInPixels")) {
        int32_t w = 0;
        int32_t h = 0;
        NativeWnd()->GetWindowSizeInPixels(&w, &h);
        auto text = ui::StringUtil::Printf("W:%d, H:%d", w, h);
        pLabel->SetText(text);
    }
    if (auto* pLabel = ui::Find<ui::Label>(this, "SDL_GetDisplayContentScale")) {
        float scale = NativeWnd()->GetDisplayContentScale();
        auto text = ui::StringUtil::Printf("%.02f", scale);
        pLabel->SetText(text);
    }
    if (auto* pLabel = ui::Find<ui::Label>(this, "SDL_GetWindowDisplayScale")) {
        float scale = NativeWnd()->GetWindowDisplayScale();
        auto text = ui::StringUtil::Printf("%.02f", scale);
        pLabel->SetText(text);
    }
    if (auto* pLabel = ui::Find<ui::Label>(this, "SDL_GetWindowPixelDensity")) {
        float scale = NativeWnd()->GetWindowPixelDensity();
        auto text = ui::StringUtil::Printf("%.02f", scale);
        pLabel->SetText(text);
    }
#else
    if (auto* pSDL = ui::Find<ui::Control>(this, "SDL")) {
        pSDL->SetVisible(false);
    }
#endif
}
