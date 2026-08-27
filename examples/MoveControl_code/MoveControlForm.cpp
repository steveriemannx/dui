#include "MoveControlForm.h"

using namespace ui;
using namespace std;


MoveControlForm::MoveControlForm()
{
}

MoveControlForm::~MoveControlForm()
{
}

DString MoveControlForm::GetSkinFolder()
{
    return _T("");
}

DString MoveControlForm::GetSkinFile()
{
    // Pure code mode: no layout XML is loaded
    return _T("");
}

void MoveControlForm::GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs)
{
    // Corresponding to the <Window> attributes in main.xml
    attrs.m_bInitSizeDefined = true;
    attrs.m_szInitSize.cx = 540;
    attrs.m_szInitSize.cy = 535;
    attrs.m_bShadowAttached = true;
    attrs.m_bShadowAttachedDefined = true;
    attrs.m_bIsLayeredWindow = true;
    attrs.m_bIsLayeredWindowDefined = true;
    attrs.m_rcCaption = ui::UiRect(0, 0, 0, 36);
    attrs.m_bCaptionDefined = true;
    BaseClass::GetCreateWindowAttributes(attrs);
}

void MoveControlForm::BuildUI()
{
    // Corresponding to the main.xml layout
    auto* pRoot = ui::Create<ui::VBox>(this, {});

    // Title bar area
    auto* pCaption = ui::Create<ui::HBox>(this, {{_T("name"), _T("window_caption_bar")}, {_T("width"), _T("stretch")}, {_T("height"), _T("36")}});
    pCaption->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    auto* pTitle = ui::Create<ui::Label>(this, {{_T("font"), _T("system_bold_14")}, {_T("margin"), _T("10,10")}, {_T("mouse_enabled"), _T("false")}});
    pTitle->SetText(_T("App List"));
    pCaption->AddItem(pTitle);

    auto* pSpacer = ui::Create<ui::Control>(this, {{_T("mouse_enabled"), _T("false")}});
    pCaption->AddItem(pSpacer);

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

    auto* pSplit1 = ui::Create<ui::Control>(this, {});
    pSplit1->SetClass(_T("splitline_hor_level1"));
    pRoot->AddItem(pSplit1);

    // Frequent apps
    auto* pFrequentSection = ui::Create<ui::VBox>(this, {{_T("height"), _T("200")}});
    pFrequentSection->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pFrequentSection);

    auto* pFrequentTitle = ui::Create<ui::HBox>(this, {{_T("height"), _T("35")}});
    pFrequentTitle->SetBkColor(_T("bk_wnd_darkcolor"));
    pFrequentSection->AddItem(pFrequentTitle);

    auto* pFrequentLabel = ui::Create<ui::Label>(this, {{_T("font"), _T("system_14")}, {_T("margin"), _T("10,10")}});
    pFrequentLabel->SetText(_T("Common Apps"));
    pFrequentTitle->AddItem(pFrequentLabel);

    auto* pFrequentBox = ui::Create<ui::VTileBox>(this, {{_T("child_halign"), _T("left")}, {_T("drop_in_id"), _T("1")}, {_T("drag_out_id"), _T("1")}});
    pFrequentBox->SetName(_T("frequent_app"));
    pFrequentBox->SetBkColor(_T("white"));
    pFrequentSection->AddItem(pFrequentBox);

    auto* pSplit2 = ui::Create<ui::Control>(this, {});
    pSplit2->SetClass(_T("splitline_hor_level1"));
    pRoot->AddItem(pSplit2);

    // My apps
    auto* pMySection = ui::Create<ui::VBox>(this, {});
    pMySection->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pMySection);

    auto* pMyTitle = ui::Create<ui::HBox>(this, {{_T("height"), _T("35")}});
    pMyTitle->SetBkColor(_T("bk_wnd_darkcolor"));
    pMySection->AddItem(pMyTitle);

    auto* pMyLabel = ui::Create<ui::Label>(this, {{_T("font"), _T("system_14")}, {_T("margin"), _T("10,10")}});
    pMyLabel->SetText(_T("My Apps"));
    pMyTitle->AddItem(pMyLabel);

    auto* pMyBox = ui::Create<ui::VTileBox>(this, {{_T("child_halign"), _T("left")}, {_T("drop_in_id"), _T("1")}, {_T("drag_out_id"), _T("1")}});
    pMyBox->SetName(_T("my_app"));
    pMyBox->SetBkColor(_T("white"));
    pMySection->AddItem(pMyBox);

    AttachBox(pRoot);
}

void MoveControlForm::OnInitWindow()
{
    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);

    BuildUI();

    // Add apps. Apps may be pushed from the server; usually they are also saved locally
    //loadFromDb
    // getFromServer----> The backend can save to the db first, then post a message; the UI reloads from the db

    // Hard-coded for the demo
    std::vector<AppItem> applist;
    AppDb::GetInstance().LoadFromDb(applist);

    m_frequent_app = static_cast<ui::Box*>(FindControl(_T("frequent_app")));
    m_my_app = static_cast<ui::Box*>(FindControl(_T("my_app")));
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
}
