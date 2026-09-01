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
    SetWindowIcon("public/caption/logo.ico");
}

void MainForm::BuildUI()
{
    auto* pRoot = ui::Create<ui::VBox>(this, {
        {"bkcolor", "bk_wnd_darkcolor"},
        {"visible", "true"}
    });

    // Title bar area
    auto* pCaption = ui::Create<ui::HBox>(this, {
        {"name", "window_caption_bar"},
        {"width", "stretch"},
        {"height", "36"},
        {"bkcolor", "bk_wnd_lightcolor"}
    });
    ui::Attach(pRoot, pCaption);

    auto* pSpacer = ui::Create<ui::Control>(this, {
        {"mouse_enabled", "false"}
    });
    ui::Attach(pCaption, pSpacer);

    auto* pFullscreenBtn = ui::Create<ui::Button>(this, {
        {"class", "btn_wnd_fullscreen_11"},
        {"name", "fullscreenbtn"},
        {"height", "32"},
        {"width", "40"},
        {"margin", "0,2,0,2"},
        {"tooltip_text", "Fullscreen, press ESC to exit fullscreen"}
    });
    ui::Attach(pCaption, pFullscreenBtn);

    auto* pMinBtn = ui::Create<ui::Button>(this, {
        {"class", "btn_wnd_min_11"},
        {"name", "minbtn"},
        {"height", "32"},
        {"width", "40"},
        {"margin", "0,2,0,2"},
        {"tooltip_text", "Minimize"}
    });
    ui::Attach(pCaption, pMinBtn);

    auto* pMaxBox = ui::Create<ui::Box>(this, {
        {"height", "stretch"},
        {"width", "40"},
        {"margin", "0,2,0,2"}
    });
    ui::Attach(pCaption, pMaxBox);

    auto* pMaxBtn = ui::Create<ui::Button>(this, {
        {"class", "btn_wnd_max_11"},
        {"name", "maxbtn"},
        {"height", "32"},
        {"width", "stretch"},
        {"tooltip_text", "Maximize"}
    });
    ui::Attach(pMaxBox, pMaxBtn);

    auto* pRestoreBtn = ui::Create<ui::Button>(this, {
        {"class", "btn_wnd_restore_11"},
        {"name", "restorebtn"},
        {"height", "32"},
        {"width", "stretch"},
        {"visible", "false"},
        {"tooltip_text", "Restore"}
    });
    ui::Attach(pMaxBox, pRestoreBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {
        {"class", "btn_wnd_close_11"},
        {"name", "closebtn"},
        {"height", "stretch"},
        {"width", "40"},
        {"margin", "0,0,0,2"},
        {"tooltip_text", "Close"}
    });
    ui::Attach(pCaption, pCloseBtn);

    // Work area
    auto* pContent = ui::Create<ui::Box>(this, {});
    ui::Attach(pRoot, pContent);

    auto* pCenter = ui::Create<ui::VBox>(this, {
        {"valign", "center"},
        {"halign", "center"},
        {"height", "100"}
    });
    ui::Attach(pContent, pCenter);

    auto* pLabel = ui::Create<ui::Label>(this, {
        {"name", "hello_label"},
        {"text", "Hello, dui!"},
        {"height", "40"},
        {"width", "100%"},
        {"text_align", "hcenter,vcenter"},
        {"margin", "0,0,0,16"}
    });
    ui::Attach(pCenter, pLabel);

    auto* pButton = ui::Create<ui::Button>(this, {
        {"name", "hello_btn"},
        {"class", "btn_global_blue_80x30"},
        {"text", "Click Me"},
        {"halign", "center"},
        {"width", "90"},
        {"height", "32"}
    });
    ui::Attach(pCenter, pButton);

    ui::Attach(this, pRoot);
}

void MainForm::BindEvents()
{
    if (auto* pButton = ui::Find<ui::Button>(this, "hello_btn")) {
        pButton->AttachClick([this](const ui::EventArgs&) {
            if (auto* pLabel = ui::Find<ui::Label>(this, "hello_label")) {
                pLabel->SetText("Hello from pure code mode!");
            }
            return true;
        });
    }
}
