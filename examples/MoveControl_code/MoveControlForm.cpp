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
    ui::VBox* pRoot = new ui::VBox(this);

    // Title bar area
    ui::HBox* pCaption = new ui::HBox(this);
    pCaption->SetAttribute(_T("name"), _T("window_caption_bar"));
    pCaption->SetAttribute(_T("width"), _T("stretch"));
    pCaption->SetAttribute(_T("height"), _T("36"));
    pCaption->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    ui::Label* pTitle = new ui::Label(this);
    pTitle->SetText(_T("App List"));
    pTitle->SetAttribute(_T("font"), _T("system_bold_14"));
    pTitle->SetAttribute(_T("margin"), _T("10,10"));
    pTitle->SetAttribute(_T("mouse_enabled"), _T("false"));
    pCaption->AddItem(pTitle);

    ui::Control* pSpacer = new ui::Control(this);
    pSpacer->SetAttribute(_T("mouse_enabled"), _T("false"));
    pCaption->AddItem(pSpacer);

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

    ui::Control* pSplit1 = new ui::Control(this);
    pSplit1->SetClass(_T("splitline_hor_level1"));
    pRoot->AddItem(pSplit1);

    // Frequent apps
    ui::VBox* pFrequentSection = new ui::VBox(this);
    pFrequentSection->SetAttribute(_T("height"), _T("200"));
    pFrequentSection->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pFrequentSection);

    ui::HBox* pFrequentTitle = new ui::HBox(this);
    pFrequentTitle->SetBkColor(_T("bk_wnd_darkcolor"));
    pFrequentTitle->SetAttribute(_T("height"), _T("35"));
    pFrequentSection->AddItem(pFrequentTitle);

    ui::Label* pFrequentLabel = new ui::Label(this);
    pFrequentLabel->SetText(_T("Common Apps"));
    pFrequentLabel->SetAttribute(_T("font"), _T("system_14"));
    pFrequentLabel->SetAttribute(_T("margin"), _T("10,10"));
    pFrequentTitle->AddItem(pFrequentLabel);

    ui::VTileBox* pFrequentBox = new ui::VTileBox(this);
    pFrequentBox->SetName(_T("frequent_app"));
    pFrequentBox->SetAttribute(_T("child_halign"), _T("left"));
    pFrequentBox->SetAttribute(_T("drop_in_id"), _T("1"));
    pFrequentBox->SetAttribute(_T("drag_out_id"), _T("1"));
    pFrequentBox->SetBkColor(_T("white"));
    pFrequentSection->AddItem(pFrequentBox);

    ui::Control* pSplit2 = new ui::Control(this);
    pSplit2->SetClass(_T("splitline_hor_level1"));
    pRoot->AddItem(pSplit2);

    // My apps
    ui::VBox* pMySection = new ui::VBox(this);
    pMySection->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pMySection);

    ui::HBox* pMyTitle = new ui::HBox(this);
    pMyTitle->SetBkColor(_T("bk_wnd_darkcolor"));
    pMyTitle->SetAttribute(_T("height"), _T("35"));
    pMySection->AddItem(pMyTitle);

    ui::Label* pMyLabel = new ui::Label(this);
    pMyLabel->SetText(_T("My Apps"));
    pMyLabel->SetAttribute(_T("font"), _T("system_14"));
    pMyLabel->SetAttribute(_T("margin"), _T("10,10"));
    pMyTitle->AddItem(pMyLabel);

    ui::VTileBox* pMyBox = new ui::VTileBox(this);
    pMyBox->SetName(_T("my_app"));
    pMyBox->SetAttribute(_T("child_halign"), _T("left"));
    pMyBox->SetAttribute(_T("drop_in_id"), _T("1"));
    pMyBox->SetAttribute(_T("drag_out_id"), _T("1"));
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
