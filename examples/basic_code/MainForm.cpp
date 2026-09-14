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
#if defined(DUI_BUILD_FOR_LINUX)
    SetShadowType(ui::Shadow::ShadowType::kShadowDrawDefault);
#else
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
#endif
#if !defined(DUI_BUILD_FOR_LINUX)
    // Linux self-drawn shadows require a layered window for alpha compositing.
    SetLayeredWindow(false, false);
#endif
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
        {"width", "stretch"},
        {"mouse_enabled", "false"}
    });
    ui::Attach(pCaption, pSpacer);

    // Window caption buttons (closebtn/minbtn/maxbtn/restorebtn/fullscreenbtn)
    // are wired automatically by the framework via their names.
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
