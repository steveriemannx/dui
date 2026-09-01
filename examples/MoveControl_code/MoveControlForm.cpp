#include "MoveControlForm.h"
#include "dui/Utils/UiBuilder.h"

using namespace ui;
using namespace std;


MoveControlForm::MoveControlForm()
{
}

void MoveControlForm::SetupWindow()
{
    // Corresponding to the <Window> attributes in main.xml
    SetWindowSize(540, 535);
    CenterWindow();
    SetShadowAttached(true);
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);
    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);
}

void MoveControlForm::BuildUI()
{
    // Corresponding to the main.xml layout
    auto* pRoot = ui::Create<ui::VBox>(this, {});

    // Title bar area
    auto* pCaption = ui::Create<ui::HBox>(this, {{"name", "window_caption_bar"}, {"width", "stretch"}, {"height", "36"}});
    pCaption->SetBkColor("bk_wnd_lightcolor");
    pRoot->AddItem(pCaption);

    auto* pTitle = ui::Create<ui::Label>(this, {{"font", "system_bold_14"}, {"margin", "10,10"}, {"mouse_enabled", "false"}});
    pTitle->SetText("App List");
    pCaption->AddItem(pTitle);

    auto* pSpacer = ui::Create<ui::Control>(this, {{"mouse_enabled", "false"}});
    pCaption->AddItem(pSpacer);

    auto* pMinBtn = ui::Create<ui::Button>(this, {{"height", "32"}, {"width", "40"}, {"margin", "0,2,0,2"}});
    pMinBtn->SetClass("btn_wnd_min_11");
    pMinBtn->SetName("minbtn");
    pMinBtn->SetToolTipText("Minimize");
    pCaption->AddItem(pMinBtn);

    auto* pMaxBox = ui::Create<ui::Box>(this, {{"height", "stretch"}, {"width", "40"}, {"margin", "0,2,0,2"}});
    pCaption->AddItem(pMaxBox);

    auto* pMaxBtn = ui::Create<ui::Button>(this, {{"height", "32"}, {"width", "stretch"}});
    pMaxBtn->SetClass("btn_wnd_max_11");
    pMaxBtn->SetName("maxbtn");
    pMaxBtn->SetToolTipText("Maximize");
    pMaxBox->AddItem(pMaxBtn);

    auto* pRestoreBtn = ui::Create<ui::Button>(this, {{"height", "32"}, {"width", "stretch"}});
    pRestoreBtn->SetClass("btn_wnd_restore_11");
    pRestoreBtn->SetName("restorebtn");
    pRestoreBtn->SetVisible(false);
    pRestoreBtn->SetToolTipText("Restore");
    pMaxBox->AddItem(pRestoreBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{"height", "stretch"}, {"width", "40"}, {"margin", "0,0,0,2"}});
    pCloseBtn->SetClass("btn_wnd_close_11");
    pCloseBtn->SetName("closebtn");
    pCloseBtn->SetToolTipText("Close");
    pCaption->AddItem(pCloseBtn);

    auto* pSplit1 = ui::Create<ui::Control>(this, {});
    pSplit1->SetClass("splitline_hor_level1");
    pRoot->AddItem(pSplit1);

    // Frequent apps
    auto* pFrequentSection = ui::Create<ui::VBox>(this, {{"height", "200"}});
    pFrequentSection->SetBkColor("bk_wnd_lightcolor");
    pRoot->AddItem(pFrequentSection);

    auto* pFrequentTitle = ui::Create<ui::HBox>(this, {{"height", "35"}});
    pFrequentTitle->SetBkColor("bk_wnd_darkcolor");
    pFrequentSection->AddItem(pFrequentTitle);

    auto* pFrequentLabel = ui::Create<ui::Label>(this, {{"font", "system_14"}, {"margin", "10,10"}});
    pFrequentLabel->SetText("Common Apps");
    pFrequentTitle->AddItem(pFrequentLabel);

    auto* pFrequentBox = ui::Create<ui::VTileBox>(this, {{"child_halign", "left"}, {"drop_in_id", "1"}, {"drag_out_id", "1"}});
    pFrequentBox->SetName("frequent_app");
    pFrequentBox->SetBkColor("white");
    pFrequentSection->AddItem(pFrequentBox);

    auto* pSplit2 = ui::Create<ui::Control>(this, {});
    pSplit2->SetClass("splitline_hor_level1");
    pRoot->AddItem(pSplit2);

    // My apps
    auto* pMySection = ui::Create<ui::VBox>(this, {});
    pMySection->SetBkColor("bk_wnd_lightcolor");
    pRoot->AddItem(pMySection);

    auto* pMyTitle = ui::Create<ui::HBox>(this, {{"height", "35"}});
    pMyTitle->SetBkColor("bk_wnd_darkcolor");
    pMySection->AddItem(pMyTitle);

    auto* pMyLabel = ui::Create<ui::Label>(this, {{"font", "system_14"}, {"margin", "10,10"}});
    pMyLabel->SetText("My Apps");
    pMyTitle->AddItem(pMyLabel);

    auto* pMyBox = ui::Create<ui::VTileBox>(this, {{"child_halign", "left"}, {"drop_in_id", "1"}, {"drag_out_id", "1"}});
    pMyBox->SetName("my_app");
    pMyBox->SetBkColor("white");
    pMySection->AddItem(pMyBox);

    AttachBox(pRoot);
}

void MoveControlForm::OnInitWindow()
{
    SetupWindow();
    BuildUI();

    // Add apps. Apps may be pushed from the server; usually they are also saved locally
    //loadFromDb
    // getFromServer----> The backend can save to the db first, then post a message; the UI reloads from the db

    // Hard-coded for the demo
    std::vector<AppItem> applist;
    AppDb::GetInstance().LoadFromDb(applist);

    m_frequent_app = ui::Find<ui::Box>(this, "frequent_app");
    m_my_app = ui::Find<ui::Box>(this, "my_app");
    ASSERT(m_frequent_app != nullptr);
    ASSERT(m_my_app != nullptr);

    for (const auto& item: applist) {
        AppItemUi* pAppUi = AppItemUi::Create(item, m_frequent_app);
        pAppUi->SetFrequentBox(m_frequent_app);
        if (item.m_isFrequent) {
            m_frequent_app->AddItem(pAppUi);
        }
        else
        {
            m_my_app->AddItem(pAppUi);
        }
    }

    BindEvents();
    BaseClass::OnInitWindow();
}

void MoveControlForm::BindEvents()
{
}
