#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

void MainForm::OnInitWindow()
{
    SetupWindow();
    BuildUI();
    BindEvents();

    BaseClass::OnInitWindow();
}

void MainForm::SetupWindow()
{
    SetWindowSize(800, 600);

    SetShadowAttached(true);
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);

    SetSizeBox(ui::UiRect(4, 4, 4, 4), true);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), true);
    SetWindowIcon(DUI_T("public/caption/logo.ico"));
}

void MainForm::BuildUI()
{
    auto* pRoot = ui::Create<ui::VBox>(this, {
        {DUI_T("bkcolor"), DUI_T("bk_wnd_darkcolor")},
        {DUI_T("visible"), DUI_T("true")}
    });

    // Title bar area
    auto* pCaption = ui::Create<ui::HBox>(this, {
        {DUI_T("name"), DUI_T("window_caption_bar")},
        {DUI_T("width"), DUI_T("stretch")},
        {DUI_T("height"), DUI_T("36")},
        {DUI_T("bkcolor"), DUI_T("bk_wnd_lightcolor")}
    });
    ui::Attach(pRoot, pCaption);

    auto* pSpacer = ui::Create<ui::Control>(this, {
        {DUI_T("mouse_enabled"), DUI_T("false")}
    });
    ui::Attach(pCaption, pSpacer);

    auto* pFullscreenBtn = ui::Create<ui::Button>(this, {
        {DUI_T("class"), DUI_T("btn_wnd_fullscreen_11")},
        {DUI_T("name"), DUI_T("fullscreenbtn")},
        {DUI_T("height"), DUI_T("32")},
        {DUI_T("width"), DUI_T("40")},
        {DUI_T("margin"), DUI_T("0,2,0,2")},
        {DUI_T("tooltip_text"), DUI_T("Fullscreen, press ESC to exit fullscreen")}
    });
    ui::Attach(pCaption, pFullscreenBtn);

    auto* pMinBtn = ui::Create<ui::Button>(this, {
        {DUI_T("class"), DUI_T("btn_wnd_min_11")},
        {DUI_T("name"), DUI_T("minbtn")},
        {DUI_T("height"), DUI_T("32")},
        {DUI_T("width"), DUI_T("40")},
        {DUI_T("margin"), DUI_T("0,2,0,2")},
        {DUI_T("tooltip_text"), DUI_T("Minimize")}
    });
    ui::Attach(pCaption, pMinBtn);

    auto* pMaxBox = ui::Create<ui::Box>(this, {
        {DUI_T("height"), DUI_T("stretch")},
        {DUI_T("width"), DUI_T("40")},
        {DUI_T("margin"), DUI_T("0,2,0,2")}
    });
    ui::Attach(pCaption, pMaxBox);

    auto* pMaxBtn = ui::Create<ui::Button>(this, {
        {DUI_T("class"), DUI_T("btn_wnd_max_11")},
        {DUI_T("name"), DUI_T("maxbtn")},
        {DUI_T("height"), DUI_T("32")},
        {DUI_T("width"), DUI_T("stretch")},
        {DUI_T("tooltip_text"), DUI_T("Maximize")}
    });
    ui::Attach(pMaxBox, pMaxBtn);

    auto* pRestoreBtn = ui::Create<ui::Button>(this, {
        {DUI_T("class"), DUI_T("btn_wnd_restore_11")},
        {DUI_T("name"), DUI_T("restorebtn")},
        {DUI_T("height"), DUI_T("32")},
        {DUI_T("width"), DUI_T("stretch")},
        {DUI_T("visible"), DUI_T("false")},
        {DUI_T("tooltip_text"), DUI_T("Restore")}
    });
    ui::Attach(pMaxBox, pRestoreBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {
        {DUI_T("class"), DUI_T("btn_wnd_close_11")},
        {DUI_T("name"), DUI_T("closebtn")},
        {DUI_T("height"), DUI_T("stretch")},
        {DUI_T("width"), DUI_T("40")},
        {DUI_T("margin"), DUI_T("0,0,0,2")},
        {DUI_T("tooltip_text"), DUI_T("Close")}
    });
    ui::Attach(pCaption, pCloseBtn);

    // Work area
    auto* pContent = ui::Create<ui::Box>(this, {});
    ui::Attach(pRoot, pContent);

    auto* pCenter = ui::Create<ui::VBox>(this, {
        {DUI_T("valign"), DUI_T("center")},
        {DUI_T("halign"), DUI_T("center")},
        {DUI_T("height"), DUI_T("100")}
    });
    ui::Attach(pContent, pCenter);

    auto* pLabel = ui::Create<ui::Label>(this, {
        {DUI_T("name"), DUI_T("hello_label")},
        {DUI_T("text"), DUI_T("Hello, dui!")},
        {DUI_T("height"), DUI_T("40")},
        {DUI_T("width"), DUI_T("100%")},
        {DUI_T("text_align"), DUI_T("hcenter,vcenter")},
        {DUI_T("margin"), DUI_T("0,0,0,16")}
    });
    ui::Attach(pCenter, pLabel);

    auto* pButton = ui::Create<ui::Button>(this, {
        {DUI_T("name"), DUI_T("hello_btn")},
        {DUI_T("class"), DUI_T("btn_global_blue_80x30")},
        {DUI_T("text"), DUI_T("Click Me")},
        {DUI_T("halign"), DUI_T("center")},
        {DUI_T("width"), DUI_T("90")},
        {DUI_T("height"), DUI_T("32")}
    });
    ui::Attach(pCenter, pButton);

    ui::Attach(this, pRoot);
}

void MainForm::BindEvents()
{
    if (auto* pButton = ui::Find<ui::Button>(this, DUI_T("hello_btn"))) {
        pButton->AttachClick([this](const ui::EventArgs&) {
            if (auto* pLabel = ui::Find<ui::Label>(this, DUI_T("hello_label"))) {
                pLabel->SetText(DUI_T("Hello from pure code mode!"));
            }
            return true;
        });
    }
}
