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
#if defined(DUI_BUILD_FOR_LINUX)
    SetShadowType(ui::Shadow::ShadowType::kShadowDrawDefault);
#else
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
#endif
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
    auto* pCaption = ui::Create<ui::HBox>(this, {{DUI_T("name"), DUI_T("window_caption_bar")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("36")}});
    pCaption->SetBkColor(DUI_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    auto* pTitle = ui::Create<ui::Label>(this, {{DUI_T("font"), DUI_T("system_bold_14")}, {DUI_T("margin"), DUI_T("10,10")}, {DUI_T("mouse_enabled"), DUI_T("false")}});
    pTitle->SetText(DUI_T("App List"));
    pCaption->AddItem(pTitle);

    auto* pSpacer = ui::Create<ui::Control>(this, {{DUI_T("mouse_enabled"), DUI_T("false")}});
    pCaption->AddItem(pSpacer);

    auto* pMinBtn = ui::Create<ui::Button>(this, {{DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,2,0,2")}});
    pMinBtn->SetClass(DUI_T("btn_wnd_min_11"));
    pMinBtn->SetName(DUI_T("minbtn"));
    pMinBtn->SetToolTipText(DUI_T("Minimize"));
    pCaption->AddItem(pMinBtn);

    auto* pMaxBox = ui::Create<ui::Box>(this, {{DUI_T("height"), DUI_T("stretch")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,2,0,2")}});
    pCaption->AddItem(pMaxBox);

    auto* pMaxBtn = ui::Create<ui::Button>(this, {{DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("stretch")}});
    pMaxBtn->SetClass(DUI_T("btn_wnd_max_11"));
    pMaxBtn->SetName(DUI_T("maxbtn"));
    pMaxBtn->SetToolTipText(DUI_T("Maximize"));
    pMaxBox->AddItem(pMaxBtn);

    auto* pRestoreBtn = ui::Create<ui::Button>(this, {{DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("stretch")}});
    pRestoreBtn->SetClass(DUI_T("btn_wnd_restore_11"));
    pRestoreBtn->SetName(DUI_T("restorebtn"));
    pRestoreBtn->SetVisible(false);
    pRestoreBtn->SetToolTipText(DUI_T("Restore"));
    pMaxBox->AddItem(pRestoreBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{DUI_T("height"), DUI_T("stretch")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,0,0,2")}});
    pCloseBtn->SetClass(DUI_T("btn_wnd_close_11"));
    pCloseBtn->SetName(DUI_T("closebtn"));
    pCloseBtn->SetToolTipText(DUI_T("Close"));
    pCaption->AddItem(pCloseBtn);

    auto* pSplit1 = ui::Create<ui::Control>(this, {});
    pSplit1->SetClass(DUI_T("splitline_hor_level1"));
    pRoot->AddItem(pSplit1);

    // Frequent apps
    auto* pFrequentSection = ui::Create<ui::VBox>(this, {{DUI_T("height"), DUI_T("200")}});
    pFrequentSection->SetBkColor(DUI_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pFrequentSection);

    auto* pFrequentTitle = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("35")}});
    pFrequentTitle->SetBkColor(DUI_T("bk_wnd_darkcolor"));
    pFrequentSection->AddItem(pFrequentTitle);

    auto* pFrequentLabel = ui::Create<ui::Label>(this, {{DUI_T("font"), DUI_T("system_14")}, {DUI_T("margin"), DUI_T("10,10")}});
    pFrequentLabel->SetText(DUI_T("Common Apps"));
    pFrequentTitle->AddItem(pFrequentLabel);

    auto* pFrequentBox = ui::Create<ui::VTileBox>(this, {{DUI_T("child_halign"), DUI_T("left")}, {DUI_T("drop_in_id"), DUI_T("1")}, {DUI_T("drag_out_id"), DUI_T("1")}});
    pFrequentBox->SetName(DUI_T("frequent_app"));
    pFrequentBox->SetBkColor(DUI_T("white"));
    pFrequentSection->AddItem(pFrequentBox);

    auto* pSplit2 = ui::Create<ui::Control>(this, {});
    pSplit2->SetClass(DUI_T("splitline_hor_level1"));
    pRoot->AddItem(pSplit2);

    // My apps
    auto* pMySection = ui::Create<ui::VBox>(this, {});
    pMySection->SetBkColor(DUI_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pMySection);

    auto* pMyTitle = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("35")}});
    pMyTitle->SetBkColor(DUI_T("bk_wnd_darkcolor"));
    pMySection->AddItem(pMyTitle);

    auto* pMyLabel = ui::Create<ui::Label>(this, {{DUI_T("font"), DUI_T("system_14")}, {DUI_T("margin"), DUI_T("10,10")}});
    pMyLabel->SetText(DUI_T("My Apps"));
    pMyTitle->AddItem(pMyLabel);

    auto* pMyBox = ui::Create<ui::VTileBox>(this, {{DUI_T("child_halign"), DUI_T("left")}, {DUI_T("drop_in_id"), DUI_T("1")}, {DUI_T("drag_out_id"), DUI_T("1")}});
    pMyBox->SetName(DUI_T("my_app"));
    pMyBox->SetBkColor(DUI_T("white"));
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

    m_frequent_app = ui::Find<ui::Box>(this, DUI_T("frequent_app"));
    m_my_app = ui::Find<ui::Box>(this, DUI_T("my_app"));
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
