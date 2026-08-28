#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

MainForm::MainForm()
{
}

MainForm::~MainForm()
{
}

DString MainForm::GetSkinFolder()
{
    //Must match the skin folder of the XML version ("MultiLang"), so that
    //image resources declared in code (like "file='language.svg'" of the
    //btn_language class) are found under the theme's skin folder
    return _T("MultiLang");
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
    using namespace ui;

    // Window-level class definition (corresponding to <Class name="btn_language"> in MultiLang.xml)
    AddClass(_T("btn_language"),
        _T(" normal_image=\"file='language.svg' width='18' height='18' valign='center' halign='center'\" hot_color=\"AliceBlue\" pushed_color=\"Lavender\""));

    auto* root = Create<ui::VBox>(this, {{"bkcolor", "bk_wnd_darkcolor"}});

    auto* caption = Create<ui::HBox>(this,
        {{"name", "window_caption_bar"},
         {"width", "stretch"},
         {"height", "36"},
         {"bkcolor", "bk_wnd_lightcolor"}});

    auto* title = Attach<ui::Label>(caption,
        {{"height", "32"}, {"width", "stretch"},
         {"margin", "8,2,0,2"},
         {"text_align", "vcenter,left"},
         {"mouse_enabled", "false"}});
    title->SetTextId(_T("MULTI_LANG_WINDOW_TEXT"));

    Attach<ui::Control>(caption, {{"width", "40"}, {"mouse_enabled", "false"}});

    auto* langBtn = Attach<ui::Button>(caption,
        {{"class", "btn_language"},
         {"height", "32"}, {"width", "40"},
         {"name", "language"},
         {"margin", "0,2,0,2"}});
    langBtn->SetToolTipTextId(_T("MULTI_LANG_SELECT_LANGUAGE"));

    auto* minBtn = Attach<ui::Button>(caption,
        {{"class", "btn_wnd_min_11"},
         {"height", "32"}, {"width", "40"},
         {"name", "minbtn"},
         {"margin", "0,2,0,2"}});
    minBtn->SetToolTipTextId(_T("MULTI_LANG_SELECT_WINDOW_MIN"));

    auto* maxBox = Attach<ui::Box>(caption,
        {{"height", "stretch"}, {"width", "40"}, {"margin", "0,2,0,2"}});

    auto* maxBtn = Attach<ui::Button>(maxBox,
        {{"class", "btn_wnd_max_11"},
         {"height", "32"}, {"width", "stretch"},
         {"name", "maxbtn"}});
    maxBtn->SetToolTipTextId(_T("MULTI_LANG_SELECT_WINDOW_MAX"));

    auto* restoreBtn = Attach<ui::Button>(maxBox,
        {{"class", "btn_wnd_restore_11"},
         {"height", "32"}, {"width", "stretch"},
         {"name", "restorebtn"}, {"visible", "false"}});
    restoreBtn->SetToolTipTextId(_T("MULTI_LANG_SELECT_WINDOW_RESTORE"));

    auto* closeBtn = Attach<ui::Button>(caption,
        {{"class", "btn_wnd_close_11"},
         {"height", "stretch"}, {"width", "40"},
         {"name", "closebtn"},
         {"margin", "0,0,0,2"}});
    closeBtn->SetToolTipTextId(_T("MULTI_LANG_SELECT_WINDOW_CLOSE"));

    root->AddItem(caption);

    auto* content = Attach<ui::Box>(root);
    auto* center = Attach<ui::VBox>(content,
        {{"margin", "0,0,0,0"},
         {"valign", "center"},
         {"halign", "center"}});

    auto* label1 = Attach<ui::Label>(center,
        {{"height", "20%"},
         {"width", "100%"},
         {"text_align", "hcenter,vcenter"},
         {"bkcolor", "AntiqueWhite"}});
    label1->SetTextId(_T("LANGUAGE_DISPLAY_NAME"));

    auto* label2 = Attach<ui::Label>(center,
        {{"height", "20%"},
         {"width", "100%"},
         {"text_align", "hcenter,vcenter"},
         {"bkcolor", "LightCyan"}});
    label2->SetTextId(_T("MULTI_LANG_LABEL_TEXT"));

    auto* richText = Attach<ui::RichText>(center,
        {{"trim_policy", "none"},
         {"height", "20%"},
         {"width", "100%"},
         {"text_align", "hcenter,vcenter"},
         {"bkcolor", "NavajoWhite"}});
    richText->SetTextId(_T("MULTI_LANG_RICH_TEXT"));

    auto* label3 = Attach<ui::Label>(center,
        {{"rich_text", "true"},
         {"height", "auto"},
         {"width", "auto"},
         {"text_align", "hcenter,vcenter"},
         {"text_padding", "10,10,10,10"},
         {"bkcolor", "LightCyan"}});
    label3->SetTextId(_T("MULTI_LANG_RICH_TEXT"));

    AttachBox(root);
}

void MainForm::OnInitWindow()
{
    // Use the OS-provided system shadow on all platforms.
    SetShadowAttached(true);
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);

    BuildUI();

    /* Show select language menu */
    auto* select = ui::Find<ui::Button>(this, _T("language"));
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
    ui::Menu* menu = new ui::Menu(this); // The parent window must be set; otherwise, when the menu pops up, the program status bar becomes inactive
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
