#include "MainForm.h"

MainForm::MainForm()
{
}

MainForm::~MainForm()
{
}

DString MainForm::GetSkinFolder()
{
    return _T("");
}

DString MainForm::GetSkinFile()
{
    // Pure code mode: no layout XML is loaded
    return _T("");
}

void MainForm::GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs)
{
    attrs.m_bInitSizeDefined = true;
    attrs.m_szInitSize.cx = 600;
    attrs.m_szInitSize.cy = 400;
    attrs.m_bShadowAttached = true;
    attrs.m_bShadowAttachedDefined = true;
    attrs.m_bIsLayeredWindow = true;
    attrs.m_bIsLayeredWindowDefined = true;
    attrs.m_rcSizeBox = ui::UiRect(4, 4, 4, 4);
    attrs.m_bSizeBoxDefined = true;
    attrs.m_rcCaption = ui::UiRect(0, 0, 0, 36);
    attrs.m_bCaptionDefined = true;
    BaseClass::GetCreateWindowAttributes(attrs);
}

void MainForm::BuildUI()
{
    // Window-level Class definition (corresponding to <Class name="btn_language"> in MultiLang.xml)
    AddClass(_T("btn_language"),
             _T(" normal_image=\"file='language.svg' width='18' height='18' valign='center' halign='center'\" hot_color=\"AliceBlue\" pushed_color=\"Lavender\""));

    ui::VBox* pRoot = new ui::VBox(this);
    pRoot->SetBkColor(_T("bk_wnd_darkcolor"));

    // Title bar area
    ui::HBox* pCaption = new ui::HBox(this);
    pCaption->SetAttribute(_T("name"), _T("window_caption_bar"));
    pCaption->SetAttribute(_T("width"), _T("stretch"));
    pCaption->SetAttribute(_T("height"), _T("36"));
    pCaption->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    ui::Label* pTitle = new ui::Label(this);
    pTitle->SetTextId(_T("MULTI_LANG_WINDOW_TEXT"));
    pTitle->SetAttribute(_T("height"), _T("32"));
    pTitle->SetAttribute(_T("width"), _T("stretch"));
    pTitle->SetAttribute(_T("margin"), _T("8,2,0,2"));
    pTitle->SetAttribute(_T("text_align"), _T("vcenter,left"));
    pTitle->SetAttribute(_T("mouse_enabled"), _T("false"));
    pCaption->AddItem(pTitle);

    ui::Control* pSpacer = new ui::Control(this);
    pSpacer->SetAttribute(_T("width"), _T("40"));
    pSpacer->SetAttribute(_T("mouse_enabled"), _T("false"));
    pCaption->AddItem(pSpacer);

    ui::Button* pLangBtn = new ui::Button(this);
    pLangBtn->SetClass(_T("btn_language"));
    pLangBtn->SetAttribute(_T("height"), _T("32"));
    pLangBtn->SetAttribute(_T("width"), _T("40"));
    pLangBtn->SetName(_T("language"));
    pLangBtn->SetAttribute(_T("margin"), _T("0,2,0,2"));
    pLangBtn->SetToolTipTextId(_T("MULTI_LANG_SELECT_LANGUAGE"));
    pCaption->AddItem(pLangBtn);

    ui::Button* pMinBtn = new ui::Button(this);
    pMinBtn->SetClass(_T("btn_wnd_min_11"));
    pMinBtn->SetAttribute(_T("height"), _T("32"));
    pMinBtn->SetAttribute(_T("width"), _T("40"));
    pMinBtn->SetName(_T("minbtn"));
    pMinBtn->SetAttribute(_T("margin"), _T("0,2,0,2"));
    pMinBtn->SetToolTipTextId(_T("MULTI_LANG_SELECT_WINDOW_MIN"));
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
    pMaxBtn->SetToolTipTextId(_T("MULTI_LANG_SELECT_WINDOW_MAX"));
    pMaxBox->AddItem(pMaxBtn);

    ui::Button* pRestoreBtn = new ui::Button(this);
    pRestoreBtn->SetClass(_T("btn_wnd_restore_11"));
    pRestoreBtn->SetAttribute(_T("height"), _T("32"));
    pRestoreBtn->SetAttribute(_T("width"), _T("stretch"));
    pRestoreBtn->SetName(_T("restorebtn"));
    pRestoreBtn->SetVisible(false);
    pRestoreBtn->SetToolTipTextId(_T("MULTI_LANG_SELECT_WINDOW_RESTORE"));
    pMaxBox->AddItem(pRestoreBtn);

    ui::Button* pCloseBtn = new ui::Button(this);
    pCloseBtn->SetClass(_T("btn_wnd_close_11"));
    pCloseBtn->SetAttribute(_T("height"), _T("stretch"));
    pCloseBtn->SetAttribute(_T("width"), _T("40"));
    pCloseBtn->SetName(_T("closebtn"));
    pCloseBtn->SetAttribute(_T("margin"), _T("0,0,0,2"));
    pCloseBtn->SetToolTipTextId(_T("MULTI_LANG_SELECT_WINDOW_CLOSE"));
    pCaption->AddItem(pCloseBtn);

    // Work area
    ui::Box* pContent = new ui::Box(this);
    pRoot->AddItem(pContent);

    ui::VBox* pCenter = new ui::VBox(this);
    pCenter->SetAttribute(_T("margin"), _T("0,0,0,0"));
    pCenter->SetAttribute(_T("valign"), _T("center"));
    pCenter->SetAttribute(_T("halign"), _T("center"));
    pContent->AddItem(pCenter);

    ui::Label* pLabel1 = new ui::Label(this);
    pLabel1->SetTextId(_T("LANGUAGE_DISPLAY_NAME"));
    pLabel1->SetAttribute(_T("height"), _T("20%"));
    pLabel1->SetAttribute(_T("width"), _T("100%"));
    pLabel1->SetAttribute(_T("text_align"), _T("hcenter,vcenter"));
    pLabel1->SetBkColor(_T("AntiqueWhite"));
    pCenter->AddItem(pLabel1);

    ui::Label* pLabel2 = new ui::Label(this);
    pLabel2->SetTextId(_T("MULTI_LANG_LABEL_TEXT"));
    pLabel2->SetAttribute(_T("height"), _T("20%"));
    pLabel2->SetAttribute(_T("width"), _T("100%"));
    pLabel2->SetAttribute(_T("text_align"), _T("hcenter,vcenter"));
    pLabel2->SetBkColor(_T("LightCyan"));
    pCenter->AddItem(pLabel2);

    // RichText rich text (the text_id content is provided by the language file, with no embedded markup)
    ui::RichText* pRichText = new ui::RichText(this);
    pRichText->SetTextId(_T("MULTI_LANG_RICH_TEXT"));
    pRichText->SetAttribute(_T("trim_policy"), _T("none"));
    pRichText->SetAttribute(_T("height"), _T("20%"));
    pRichText->SetAttribute(_T("width"), _T("100%"));
    pRichText->SetAttribute(_T("text_align"), _T("hcenter,vcenter"));
    pRichText->SetBkColor(_T("NavajoWhite"));
    pCenter->AddItem(pRichText);

    ui::Label* pLabel3 = new ui::Label(this);
    pLabel3->SetAttribute(_T("rich_text"), _T("true"));
    pLabel3->SetTextId(_T("MULTI_LANG_RICH_TEXT"));
    pLabel3->SetAttribute(_T("height"), _T("auto"));
    pLabel3->SetAttribute(_T("width"), _T("auto"));
    pLabel3->SetAttribute(_T("text_align"), _T("hcenter,vcenter"));
    pLabel3->SetAttribute(_T("text_padding"), _T("10,10,10,10"));
    pLabel3->SetBkColor(_T("LightCyan"));
    pCenter->AddItem(pLabel3);

    AttachBox(pRoot);
}

void MainForm::OnInitWindow()
{
    BuildUI();

    /* Show select language menu */
    ui::Button* select = dynamic_cast<ui::Button*>(FindControl(_T("language")));
    ASSERT(select != nullptr);
    if (select == nullptr) {
        return;
    }
    select->AttachClick([this](const ui::EventArgs& args) {
        ui::UiRect rect = args.GetSender()->GetPos();
        ui::UiPoint point;
        point.x = rect.left;
        point.y = rect.bottom;
        ClientToScreen(point);

        ShowPopupMenu(point);
        return true;
    });

    BaseClass::OnInitWindow();
}

void MainForm::ShowPopupMenu(const ui::UiPoint& point)
{
    ui::Menu* menu = new ui::Menu(this);// The parent window must be set; otherwise, when the menu pops up, the program status bar becomes inactive
    // Pure code menu: no XML template; all menu items are added by code
    menu->ShowMenu(_T(""), point);

    // Current language file
    DString currentLangFileName = ui::GlobalManager::Instance().GetLanguageFileName();

    // The list of available language files and their display names
    std::vector<std::pair<DString, DString>> languageList;
    ui::GlobalManager::Instance().GetLanguageList(languageList);
    if (languageList.empty()) {
        languageList.push_back({ currentLangFileName , _T("")});
    }

    // Add menu items dynamically
    for (auto& lang : languageList) {
        const DString fileName = lang.first;
        DString& displayName = lang.second;

        ui::MenuItem* pMenuItem = new ui::MenuItem(this);
        pMenuItem->SetClass(_T("menu_element"));
        ui::CheckBox* pCheckBox = new ui::CheckBox(this);
        pCheckBox->SetClass(_T("menu_checkbox"));
        pCheckBox->SetAttribute(_T("margin"), _T("0,5,0,10"));
        pCheckBox->SetText(!displayName.empty() ? displayName : fileName);
        pMenuItem->AddItem(pCheckBox);
        menu->AddMenuItem(pMenuItem);

        if (ui::StringUtil::IsEqualNoCase(fileName, currentLangFileName)) {
            pCheckBox->Selected(true);
        }

        // Attach the language selection event
        pMenuItem->AttachClick([fileName](const ui::EventArgs& args) {
            // Switch language
            ui::GlobalManager& globalManager = ui::GlobalManager::Instance();
            if (globalManager.GetLanguageFileName() != fileName) {
                globalManager.ReloadLanguage(ui::FilePath(), fileName, true);
            }
            return true;
        });
    }
}
