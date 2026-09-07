//MainForm.cpp
#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

void MainForm::SetupWindow()
{
    SetWindowSize(800, 600);
    CenterWindow();
    SetWindowMinimumSize(ui::UiSize(240, 100), true);
    SetShadowAttached(true);
#if defined(DUI_BUILD_FOR_LINUX)
    SetShadowType(ui::Shadow::ShadowType::kShadowDrawDefault);
#else
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
#endif
#if !defined(DUI_BUILD_FOR_LINUX)
    SetLayeredWindow(false, false);
#endif
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);
    SetSizeBox(ui::UiRect(4, 4, 4, 4), true);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), true);
    SetWindowIcon(DUI_T("public/caption/logo.ico"));
}

void MainForm::BuildUI()
{
    // Corresponding to the DpiAware.xml layout
    auto* pRoot = ui::Create<ui::VBox>(this, {{DUI_T("bkcolor"), DUI_T("bk_wnd_darkcolor")}, {DUI_T("visible"), DUI_T("true")}});

    // Title bar area
    auto* pCaption = ui::Create<ui::HBox>(this, {{DUI_T("name"), DUI_T("window_caption_bar")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("36")}, {DUI_T("bkcolor"), DUI_T("bk_wnd_lightcolor")}});
    ui::Attach(pRoot, pCaption);

    auto* pSpacer = ui::Create<ui::Control>(this, {{DUI_T("mouse_enabled"), DUI_T("false")}});
    ui::Attach(pCaption, pSpacer);

    auto* pFullscreenBtn = ui::Create<ui::Button>(this, {{DUI_T("class"), DUI_T("btn_wnd_fullscreen_11")}, {DUI_T("name"), DUI_T("fullscreenbtn")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,2,0,2")}, {DUI_T("tooltip_text"), DUI_T("Fullscreen, press ESC to exit fullscreen")}});
    ui::Attach(pCaption, pFullscreenBtn);

    auto* pMinBtn = ui::Create<ui::Button>(this, {{DUI_T("class"), DUI_T("btn_wnd_min_11")}, {DUI_T("name"), DUI_T("minbtn")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,2,0,2")}, {DUI_T("tooltip_text"), DUI_T("Minimize")}});
    ui::Attach(pCaption, pMinBtn);

    auto* pMaxBox = ui::Create<ui::Box>(this, {{DUI_T("height"), DUI_T("stretch")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,2,0,2")}});
    ui::Attach(pCaption, pMaxBox);

    auto* pMaxBtn = ui::Create<ui::Button>(this, {{DUI_T("class"), DUI_T("btn_wnd_max_11")}, {DUI_T("name"), DUI_T("maxbtn")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("tooltip_text"), DUI_T("Maximize")}});
    ui::Attach(pMaxBox, pMaxBtn);

    auto* pRestoreBtn = ui::Create<ui::Button>(this, {{DUI_T("class"), DUI_T("btn_wnd_restore_11")}, {DUI_T("name"), DUI_T("restorebtn")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("visible"), DUI_T("false")}, {DUI_T("tooltip_text"), DUI_T("Restore")}});
    ui::Attach(pMaxBox, pRestoreBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{DUI_T("class"), DUI_T("btn_wnd_close_11")}, {DUI_T("name"), DUI_T("closebtn")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,0,0,2")}, {DUI_T("tooltip_text"), DUI_T("Close")}});
    ui::Attach(pCaption, pCloseBtn);

    // Work area
    auto* pContent = ui::Create<ui::Box>(this, {});
    ui::Attach(pRoot, pContent);

    auto* pGroupBox = ui::Create<ui::GroupVBox>(this, {{DUI_T("name"), DUI_T("group_box_test")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("640")}, {DUI_T("height"), DUI_T("auto")}});
    ui::Attach(pContent, pGroupBox);

    // Title row
    auto* pRow1 = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("40")}, {DUI_T("width"), DUI_T("100%")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("child_halign"), DUI_T("center")}});
    ui::Attach(pGroupBox, pRow1);

    auto* pLabel = ui::Create<ui::Label>(this, {{DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("text_align"), DUI_T("left,vcenter")}, {DUI_T("text"), DUI_T("DPI-Aware Application Example (High-DPI Support)")}, {DUI_T("margin"), DUI_T("4,0,0,0")}});
    ui::Attach(pRow1, pLabel);

    auto* pGroupPos = ui::Create<ui::Label>(this, {{DUI_T("name"), DUI_T("group_box_pos")}, {DUI_T("text"), DUI_T("[left: 0, top: 0]")}, {DUI_T("height"), DUI_T("40")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("text_align"), DUI_T("left,vcenter")}});
    ui::Attach(pRow1, pGroupPos);

    // DPI awareness mode row
    auto* pRow2 = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("auto")}, {DUI_T("width"), DUI_T("100%")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("padding"), DUI_T("20,0,0,0")}});
    ui::Attach(pGroupBox, pRow2);

    pLabel = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Current process DPI awareness mode:")}});
    ui::Attach(pRow2, pLabel);

    pLabel = ui::Create<ui::Label>(this, {{DUI_T("name"), DUI_T("dpi_awareness")}, {DUI_T("text"), DUI_T("PROCESS_DPI_UNAWARE")}});
    ui::Attach(pRow2, pLabel);

    // Primary display scale row
    auto* pRow3 = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("auto")}, {DUI_T("width"), DUI_T("100%")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("padding"), DUI_T("20,0,0,0")}});
    ui::Attach(pGroupBox, pRow3);

    pLabel = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Primary monitor display scale:")}});
    ui::Attach(pRow3, pLabel);

    pLabel = ui::Create<ui::Label>(this, {{DUI_T("name"), DUI_T("primary_monitor_display_scale")}, {DUI_T("text"), DUI_T("200%")}});
    ui::Attach(pRow3, pLabel);

    // Window scale row
    auto* pRow4 = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("auto")}, {DUI_T("width"), DUI_T("100%")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("padding"), DUI_T("20,0,0,0")}});
    ui::Attach(pGroupBox, pRow4);

    pLabel = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Current window UI scale:")}});
    ui::Attach(pRow4, pLabel);

    pLabel = ui::Create<ui::Label>(this, {{DUI_T("name"), DUI_T("window_display_scale")}, {DUI_T("text"), DUI_T("200%")}});
    ui::Attach(pRow4, pLabel);

    // Window position and size
    auto* pSizeVBox = ui::Create<ui::VBox>(this, {{DUI_T("height"), DUI_T("auto")}, {DUI_T("width"), DUI_T("100%")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("padding"), DUI_T("20,0,0,0")}});
    ui::Attach(pGroupBox, pSizeVBox);

    auto* pRow5 = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("auto")}});
    ui::Attach(pSizeVBox, pRow5);

    pLabel = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Current Window Position and Size:")}});
    ui::Attach(pRow5, pLabel);

    pLabel = ui::Create<ui::Label>(this, {{DUI_T("name"), DUI_T("window_size")}, {DUI_T("text"), DUI_T("L:0,T:0,W:0,H:0")}});
    ui::Attach(pRow5, pLabel);

    auto* pRow6 = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("auto")}});
    ui::Attach(pSizeVBox, pRow6);

    pLabel = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Current Window Client Size:")}});
    ui::Attach(pRow6, pLabel);

    pLabel = ui::Create<ui::Label>(this, {{DUI_T("name"), DUI_T("window_client_size")}, {DUI_T("text"), DUI_T("L:0,T:0,W:0,H:0")}});
    ui::Attach(pRow6, pLabel);

    // native backend info area
    auto* backendVBox = ui::Create<ui::VBox>(this, {{DUI_T("name"), DUI_T("native backend")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("width"), DUI_T("100%")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("padding"), DUI_T("20,0,0,0")}, {DUI_T("margin"), DUI_T("0,4,0,0")}});
    ui::Attach(pGroupBox, backendVBox);

    struct NativeRow { DString name; DString label; DString init; };
    const NativeRow nativeRows[] = {
        { DUI_T("Native_GetWindowSize"), DUI_T("Native_GetWindowSize："), DUI_T("W:0,H:0") },
        { DUI_T("Native_GetWindowSizeInPixels"), DUI_T("Native_GetWindowSizeInPixels："), DUI_T("W:0,H:0") },
        { DUI_T("Native_GetDisplayContentScale"), DUI_T("Native_GetDisplayContentScale："), DUI_T("0") },
        { DUI_T("Native_GetWindowDisplayScale"), DUI_T("Native_GetWindowDisplayScale："), DUI_T("0") },
        { DUI_T("Native_GetWindowPixelDensity"), DUI_T("Native_GetWindowPixelDensity："), DUI_T("0") },
    };
    for (const auto& row : nativeRows) {
    auto* pNativeRow = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("auto")}});
        ui::Attach(backendVBox, pNativeRow);

        auto* pNativeLabel = ui::Create<ui::Label>(this, {{DUI_T("width"), DUI_T("300")}, {DUI_T("text_align"), DUI_T("right,vcenter")}, {DUI_T("text"), row.label}});
        ui::Attach(pNativeRow, pNativeLabel);

        auto* pNativeValue = ui::Create<ui::Label>(this, {{DUI_T("name"), row.name}, {DUI_T("text"), row.init}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("text_align"), DUI_T("left,vcenter")}});
        ui::Attach(pNativeRow, pNativeValue);
    }

    // Rich text area
    auto* pRichRow = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("auto")}, {DUI_T("width"), DUI_T("100%")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("0,8,0,8")}});
    ui::Attach(pGroupBox, pRichRow);

    auto* pRichText = ui::Create<ui::RichText>(this, {{DUI_T("class"), DUI_T("rich_text")}, {DUI_T("bkcolor"), DUI_T("green")}, {DUI_T("row_spacing_mul"), DUI_T("1.5")}, {DUI_T("width"), DUI_T("100%")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("margin"), DUI_T("4,0,4,0")}});
    ui::Attach(pRichRow, pRichText);

    // Rich text content (corresponding to the <RichText> content in DpiAware.xml)
    ui::WindowBuilder::ParseRichTextXmlText(DUI_T("<RichText>")
        DUI_T("RichText text: <b>bold, <font color=\"#FF0000\">bold red, </font></b>")
        DUI_T("<font face=\"Microsoft YaHei\" size=\"16\">Microsoft YaHei 16pt demo, </font>")
        DUI_T("<font face=\"Microsoft YaHei\" size=\"14\">Microsoft YaHei 14pt demo, </font>")
        DUI_T("<font face=\"Microsoft YaHei\" size=\"12\">Microsoft YaHei 12pt demo, </font>")
        DUI_T("<font face=\"Microsoft YaHei\" size=\"10\">Microsoft YaHei 10pt demo, </font>")
        DUI_T("<font face=\"Microsoft YaHei\" size=\"8\">Microsoft YaHei 8pt demo, </font>")
        DUI_T("<i>italic demo, </i><b>bold demo, </b><s>strikethrough demo, </s><u>underline demo</u>")
        DUI_T("</RichText>"), pRichText);

    // Scale adjustment row
    auto* pScaleRow = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("auto")}});
    ui::Attach(pGroupBox, pScaleRow);

    auto* pLeftCol = ui::Create<ui::VBox>(this, {{DUI_T("height"), DUI_T("auto")}});
    ui::Attach(pScaleRow, pLeftCol);

    auto* pScaleInner = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("auto")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(pLeftCol, pScaleInner);

    pLabel = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Adjust window UI scale (60-500):")}});
    ui::Attach(pScaleInner, pLabel);

    auto* pScaleEdit = ui::Create<ui::RichEdit>(this, {{DUI_T("class"), DUI_T("simple simple_border")}, {DUI_T("name"), DUI_T("display_scale_factor")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("text"), DUI_T("")}, {DUI_T("number"), DUI_T("true")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("60")}, {DUI_T("max_char"), DUI_T("4")}, {DUI_T("min_number"), DUI_T("0")}, {DUI_T("max_number"), DUI_T("500")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("text_align"), DUI_T("hcenter,vcenter")}, {DUI_T("margin"), DUI_T("1,0,4,0")}});
    ui::Attach(pScaleInner, pScaleEdit);

    auto* pSetScaleBtn = ui::Create<ui::Button>(this, {{DUI_T("class"), DUI_T("btn_global_blue_80x30")}, {DUI_T("name"), DUI_T("set_display_scale_factor")}, {DUI_T("text"), DUI_T("Change Display Scale")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("text_padding"), DUI_T("10,0,10,0")}});
    ui::Attach(pScaleInner, pSetScaleBtn);

    auto* pNewWndRow = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("40")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("160,40,1,1")}, {DUI_T("child_halign"), DUI_T("left")}});
    ui::Attach(pLeftCol, pNewWndRow);

    auto* pNewWndBtn = ui::Create<ui::Button>(this, {{DUI_T("class"), DUI_T("btn_global_blue_80x30")}, {DUI_T("name"), DUI_T("NewWindow")}, {DUI_T("text"), DUI_T("Create New Window")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("text_padding"), DUI_T("10,0,10,0")}});
    ui::Attach(pNewWndRow, pNewWndBtn);

    // Right-side image
    auto* pRightCol = ui::Create<ui::VBox>(this, {{DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("blue")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("margin"), DUI_T("4,4,4,4")}});
    ui::Attach(pScaleRow, pRightCol);

    auto* pImage = ui::Create<ui::Control>(this, {{DUI_T("bkimage"), DUI_T("autumn.png")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("2,2,2,2")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("red")}});
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
    if (auto* pButton = ui::Find<ui::Button>(this, DUI_T("NewWindow"))) {
        pButton->AttachClick([this](const ui::EventArgs& /*args*/) {
            // Pop up a new window
            MainForm* window = new MainForm();
            window->CreateWnd(this, ui::WindowCreateParam(DUI_T("DpiAware"), true));
            window->ShowWindow(ui::kSW_SHOW_NORMAL);
            return true;
            });
    }

    if (auto* pButton = ui::Find<ui::Button>(this, DUI_T("set_display_scale_factor"))) {
        pButton->AttachClick([this](const ui::EventArgs& /*args*/) {
            // Change the UI display scale
            if (auto* pRichEdit = ui::Find<ui::RichEdit>(this, DUI_T("display_scale_factor"))) {
            auto nNewDisplayScaleFactor = ui::StringUtil::StringToInt32(pRichEdit->GetText());
                if (nNewDisplayScaleFactor > 0) {
                    this->ChangeDisplayScale((uint32_t)nNewDisplayScaleFactor);
                    UpdateUI();
                }
            }
            return true;
            });
    }

    if (auto* pGroupTest = ui::Find<ui::Control>(this, DUI_T("group_box_test"))) {
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
    if (auto* pLabel = ui::Find<ui::Label>(this, DUI_T("dpi_awareness"))) {
        DString text;
        auto mode = ui::GlobalManager::Instance().Dpi().GetDpiAwareness();
        if (mode == ui::DpiAwarenessMode::kDpiUnaware) {
            text = DUI_T("kDpiUnaware");
        }
        else if (mode == ui::DpiAwarenessMode::kSystemDpiAware) {
            text = DUI_T("kSystemDpiAware");
        }
        else if (mode == ui::DpiAwarenessMode::kPerMonitorDpiAware) {
            text = DUI_T("kPerMonitorDpiAware");
        }
        else if (mode == ui::DpiAwarenessMode::kPerMonitorDpiAware_V2) {
            text = DUI_T("kPerMonitorDpiAware_V2");
        }
        pLabel->SetText(text);
    }

    const auto* pGroupTest = ui::Find<ui::Control>(this, DUI_T("group_box_test"));
    if (auto* pLabel = ui::Find<ui::Label>(this, DUI_T("group_box_pos"))) {
        if (pGroupTest != nullptr) {
            auto text = ui::StringUtil::Printf(DUI_T("[left: %d, top: %d]"), pGroupTest->GetRect().left, pGroupTest->GetRect().top);
            pLabel->SetText(text);
        }
    }

    if (auto* pLabel = ui::Find<ui::Label>(this, DUI_T("primary_monitor_display_scale"))) {
        auto nScaleFactor = ui::GlobalManager::Instance().Dpi().GetDisplayScaleFactor();
        float fScale = ui::GlobalManager::Instance().Dpi().GetDisplayScale();
        auto text = ui::StringUtil::Printf(DUI_T("%d%% (DisplayScale: %.02f)"), nScaleFactor, fScale);
        pLabel->SetText(text);
    }
    if (auto* pLabel = ui::Find<ui::Label>(this, DUI_T("window_display_scale"))) {
        auto nScaleFactor = Dpi().GetDisplayScaleFactor();
        float fScale = Dpi().GetDisplayScale();
        auto text = ui::StringUtil::Printf(DUI_T("%d%% (DisplayScale: %.02f)"), nScaleFactor, fScale);
        pLabel->SetText(text);
    }
    if (auto* pRichEdit = ui::Find<ui::RichEdit>(this, DUI_T("display_scale_factor"))) {
        auto nScaleFactor = Dpi().GetDisplayScaleFactor();
        pRichEdit->SetTextNumber((int64_t)nScaleFactor);
    }
    if (auto* pLabel = ui::Find<ui::Label>(this, DUI_T("window_size"))) {
        ui::UiRect rcWindow;
        GetWindowRect(rcWindow);
        auto text = ui::StringUtil::Printf(DUI_T("W:%d, H:%d [Left:%d, Top:%d]"), rcWindow.Width(), rcWindow.Height(), rcWindow.left, rcWindow.top);
        pLabel->SetText(text);
    }
    if (auto* pLabel = ui::Find<ui::Label>(this, DUI_T("window_client_size"))) {
        ui::UiRect rcClient;
        GetClientRect(rcClient);
        auto text = ui::StringUtil::Printf(DUI_T("W:%d, H:%d [Left:%d, Top:%d]"), rcClient.Width(), rcClient.Height(), rcClient.left, rcClient.top);
        pLabel->SetText(text);
    }
#ifdef DUI_BUILD_FOR_WAYLAND
    if (auto* pLabel = ui::Find<ui::Label>(this, DUI_T("Native_GetWindowSize"))) {
        int32_t w = 0;
        int32_t h = 0;
        NativeWnd()->GetWindowSize(&w, &h);
        auto text = ui::StringUtil::Printf(DUI_T("W:%d, H:%d"), w, h);
        pLabel->SetText(text);
    }
    if (auto* pLabel = ui::Find<ui::Label>(this, DUI_T("Native_GetWindowSizeInPixels"))) {
        int32_t w = 0;
        int32_t h = 0;
        NativeWnd()->GetWindowSizeInPixels(&w, &h);
        auto text = ui::StringUtil::Printf(DUI_T("W:%d, H:%d"), w, h);
        pLabel->SetText(text);
    }
    if (auto* pLabel = ui::Find<ui::Label>(this, DUI_T("Native_GetDisplayContentScale"))) {
        float scale = NativeWnd()->GetDisplayContentScale();
        auto text = ui::StringUtil::Printf(DUI_T("%.02f"), scale);
        pLabel->SetText(text);
    }
    if (auto* pLabel = ui::Find<ui::Label>(this, DUI_T("Native_GetWindowDisplayScale"))) {
        float scale = NativeWnd()->GetWindowDisplayScale();
        auto text = ui::StringUtil::Printf(DUI_T("%.02f"), scale);
        pLabel->SetText(text);
    }
    if (auto* pLabel = ui::Find<ui::Label>(this, DUI_T("Native_GetWindowPixelDensity"))) {
        float scale = NativeWnd()->GetWindowPixelDensity();
        auto text = ui::StringUtil::Printf(DUI_T("%.02f"), scale);
        pLabel->SetText(text);
    }
#else
    if (auto* nativeBackend = ui::Find<ui::Control>(this, DUI_T("native backend"))) {
        nativeBackend->SetVisible(false);
    }
#endif
}
