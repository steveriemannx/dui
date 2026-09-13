#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

void MainForm::BuildUI()
{
    using namespace ui;

    // Window-level class definition (corresponding to <Class name="btn_language"> in MultiLang.xml)
    AddClass("btn_language",
        " normal_image=\"file='language.svg' width='18' height='18' valign='center' halign='center'\" hot_color=\"AliceBlue\" pushed_color=\"Lavender\"");

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
    title->SetTextId("MULTI_LANG_WINDOW_TEXT");

    Attach<ui::Control>(caption, {{"width", "40"}, {"mouse_enabled", "false"}});

    auto* langBtn = Attach<ui::Button>(caption,
        {{"class", "btn_language"},
         {"height", "32"}, {"width", "40"},
         {"name", "language"},
         {"margin", "0,2,8,2"}});
    langBtn->SetToolTipTextId("MULTI_LANG_SELECT_LANGUAGE");

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
    label1->SetTextId("LANGUAGE_DISPLAY_NAME");

    auto* label2 = Attach<ui::Label>(center,
        {{"height", "20%"},
         {"width", "100%"},
         {"text_align", "hcenter,vcenter"},
         {"bkcolor", "LightCyan"}});
    label2->SetTextId("MULTI_LANG_LABEL_TEXT");

    auto* richText = Attach<ui::RichText>(center,
        {{"trim_policy", "none"},
         {"height", "20%"},
         {"width", "100%"},
         {"text_align", "hcenter,vcenter"},
         {"bkcolor", "NavajoWhite"}});
    richText->SetTextId("MULTI_LANG_RICH_TEXT");

    auto* label3 = Attach<ui::Label>(center,
        {{"rich_text", "true"},
         {"height", "auto"},
         {"width", "auto"},
         {"text_align", "hcenter,vcenter"},
         {"text_padding", "10,10,10,10"},
         {"bkcolor", "LightCyan"}});
    label3->SetTextId("MULTI_LANG_RICH_TEXT");

    AttachBox(root);
}

void MainForm::OnInitWindow()
{
    SetupWindow();
    BuildUI();
    BindEvents();

    BaseClass::OnInitWindow();
}

void MainForm::SetupWindow()
{
    SetWindowSize(600, 400);
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
    SetSizeBox(ui::UiRect(4, 4, 4, 4), true);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), true);
}

void MainForm::BindEvents()
{
    /* Show select language menu */
    auto* select = ui::Find<ui::Button>(this, "language");
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
}

void MainForm::ShowPopupMenu(const ui::UiPoint& point)
{
    ui::Menu* menu = new ui::Menu(this); // The parent window must be set; otherwise, when the menu pops up, the program status bar becomes inactive
    menu->SetShadowAttached(true);
    menu->SetShadowType(ui::Shadow::ShadowType::kShadowSystemRound);
    // Pure code menu: no XML template; all menu items are added by code
    menu->ShowMenu("", point);

    // Match the XML lang_menu.xml header row: "选择语言" plus a separator,
    // before the dynamically added language items.
    {
        ui::HBox* pHeader = new ui::HBox(this);
        pHeader->SetClass("menu_split_box");
        pHeader->SetAttribute("height", "36");
        pHeader->SetAttribute("width", "256");
        ui::Label* pLabel = new ui::Label(this);
        pLabel->SetClass("menu_text");
        pLabel->SetTextId("MULTI_LANG_SELECT_LANGUAGE");
        pLabel->SetAttribute("text_padding", "0,0,6,0");
        pHeader->AddItem(pLabel);
        menu->AddMenuControl(pHeader);

        ui::Box* pSeparator = new ui::Box(this);
        pSeparator->SetClass("menu_split_box");
        ui::Control* pLine = new ui::Control(this);
        pLine->SetClass("menu_split_line");
        pSeparator->AddItem(pLine);
        menu->AddMenuControl(pSeparator);
    }

    // Current language file
    std::string currentLangFileName = ui::GlobalManager::Instance().GetLanguageFileName();

    // The list of available language files and their display names
    std::vector<std::pair<std::string, std::string>> languageList;
    ui::GlobalManager::Instance().GetLanguageList(languageList);
    if (languageList.empty()) {
        languageList.push_back({ currentLangFileName , ""});
    }

    // Add menu items dynamically
    for (auto& lang : languageList) {
        const std::string fileName = lang.first;
        std::string& displayName = lang.second;

        ui::MenuItem* pMenuItem = new ui::MenuItem(menu);
        pMenuItem->SetClass("menu_element");
        pMenuItem->SetFixedWidth(ui::UiFixedInt(256), true, true);
        ui::CheckBox* pCheckBox = new ui::CheckBox(menu);
        pCheckBox->SetClass("menu_checkbox");
        pCheckBox->SetAttribute("margin", "0,5,0,10");
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
