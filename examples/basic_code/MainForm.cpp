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
    CenterWindow();
    SetWindowMinimumSize(ui::UiSize(240, 100), true);
    SetUseSystemCaption(false);

    SetShadowAttached(true);
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);
    SetEnableSnapLayoutMenu(true);
    SetEnableSysMenu(true);
    SetSysMenuRect(ui::UiRect(0, 0, 36, 36), true);
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

    // Work area
    auto* pContent = ui::Create<ui::Box>(this, {});
    ui::Attach(pRoot, pContent);

    auto* pCenter = ui::Create<ui::VBox>(this, {
        {"valign", "center"},
        {"halign", "center"}
    });
    ui::Attach(pContent, pCenter);

    auto* pLabel = ui::Create<ui::Label>(this, {
        {"name", "tooltip"},
        {"text", "A simple window with a title bar and standard buttons."},
        {"height", "100%"},
        {"width", "100%"},
        {"text_align", "hcenter,vcenter"}
    });
    ui::Attach(pCenter, pLabel);

    ui::Attach(this, pRoot);
}

void MainForm::BindEvents()
{
    // Window caption buttons (closebtn/minbtn/maxbtn/restorebtn/fullscreenbtn)
    // are wired automatically by the framework via their names.
}
