#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

void MainForm::BuildUI()
{
    using namespace ui;

    // Window-level class definition (corresponding to <Class name="btn_language"> in MultiLang.xml)
    AddClass(DUI_T("btn_language"),
        DUI_T(" normal_image=\"file='language.svg' width='18' height='18' valign='center' halign='center'\" hot_color=\"AliceBlue\" pushed_color=\"Lavender\""));

    auto* root = Create<ui::VBox>(this, {{DUI_T("bkcolor"), DUI_T("bk_wnd_darkcolor")}});

    auto* caption = Create<ui::HBox>(this,
        {{DUI_T("name"), DUI_T("window_caption_bar")},
         {DUI_T("width"), DUI_T("stretch")},
         {DUI_T("height"), DUI_T("36")},
         {DUI_T("bkcolor"), DUI_T("bk_wnd_lightcolor")}});

    auto* title = Attach<ui::Label>(caption,
        {{DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("stretch")},
         {DUI_T("margin"), DUI_T("8,2,0,2")},
         {DUI_T("text_align"), DUI_T("vcenter,left")},
         {DUI_T("mouse_enabled"), DUI_T("false")}});
    title->SetTextId(DUI_T("MULTI_LANG_WINDOW_TEXT"));

    Attach<ui::Control>(caption, {{DUI_T("width"), DUI_T("40")}, {DUI_T("mouse_enabled"), DUI_T("false")}});

    auto* langBtn = Attach<ui::Button>(caption,
        {{DUI_T("class"), DUI_T("btn_language")},
         {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("40")},
         {DUI_T("name"), DUI_T("language")},
         {DUI_T("margin"), DUI_T("0,2,8,2")}});
    langBtn->SetToolTipTextId(DUI_T("MULTI_LANG_SELECT_LANGUAGE"));

    root->AddItem(caption);

    auto* content = Attach<ui::Box>(root);
    auto* center = Attach<ui::VBox>(content,
        {{DUI_T("margin"), DUI_T("0,0,0,0")},
         {DUI_T("valign"), DUI_T("center")},
         {DUI_T("halign"), DUI_T("center")}});

    auto* label1 = Attach<ui::Label>(center,
        {{DUI_T("height"), DUI_T("20%")},
         {DUI_T("width"), DUI_T("100%")},
         {DUI_T("text_align"), DUI_T("hcenter,vcenter")},
         {DUI_T("bkcolor"), DUI_T("AntiqueWhite")}});
    label1->SetTextId(DUI_T("LANGUAGE_DISPLAY_NAME"));

    auto* label2 = Attach<ui::Label>(center,
        {{DUI_T("height"), DUI_T("20%")},
         {DUI_T("width"), DUI_T("100%")},
         {DUI_T("text_align"), DUI_T("hcenter,vcenter")},
         {DUI_T("bkcolor"), DUI_T("LightCyan")}});
    label2->SetTextId(DUI_T("MULTI_LANG_LABEL_TEXT"));

    auto* richText = Attach<ui::RichText>(center,
        {{DUI_T("trim_policy"), DUI_T("none")},
         {DUI_T("height"), DUI_T("20%")},
         {DUI_T("width"), DUI_T("100%")},
         {DUI_T("text_align"), DUI_T("hcenter,vcenter")},
         {DUI_T("bkcolor"), DUI_T("NavajoWhite")}});
    richText->SetTextId(DUI_T("MULTI_LANG_RICH_TEXT"));

    auto* label3 = Attach<ui::Label>(center,
        {{DUI_T("rich_text"), DUI_T("true")},
         {DUI_T("height"), DUI_T("auto")},
         {DUI_T("width"), DUI_T("auto")},
         {DUI_T("text_align"), DUI_T("hcenter,vcenter")},
         {DUI_T("text_padding"), DUI_T("10,10,10,10")},
         {DUI_T("bkcolor"), DUI_T("LightCyan")}});
    label3->SetTextId(DUI_T("MULTI_LANG_RICH_TEXT"));

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
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);
    SetSizeBox(ui::UiRect(4, 4, 4, 4), true);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), true);
}

void MainForm::BindEvents()
{
    /* Show select language menu */
    auto* select = ui::Find<ui::Button>(this, DUI_T("language"));
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
    menu->ShowMenu(DUI_T(""), point);

    // Match the XML lang_menu.xml header row: "选择语言" plus a separator,
    // before the dynamically added language items.
    {
        ui::HBox* pHeader = new ui::HBox(this);
        pHeader->SetClass(DUI_T("menu_split_box"));
        pHeader->SetAttribute(DUI_T("height"), DUI_T("36"));
        pHeader->SetAttribute(DUI_T("width"), DUI_T("256"));
        ui::Label* pLabel = new ui::Label(this);
        pLabel->SetClass(DUI_T("menu_text"));
        pLabel->SetTextId(DUI_T("MULTI_LANG_SELECT_LANGUAGE"));
        pLabel->SetAttribute(DUI_T("text_padding"), DUI_T("0,0,6,0"));
        pHeader->AddItem(pLabel);
        menu->AddMenuControl(pHeader);

        ui::Box* pSeparator = new ui::Box(this);
        pSeparator->SetClass(DUI_T("menu_split_box"));
        ui::Control* pLine = new ui::Control(this);
        pLine->SetClass(DUI_T("menu_split_line"));
        pSeparator->AddItem(pLine);
        menu->AddMenuControl(pSeparator);
    }

    // Current language file
    DString currentLangFileName = ui::GlobalManager::Instance().GetLanguageFileName();

    // The list of available language files and their display names
    std::vector<std::pair<DString, DString>> languageList;
    ui::GlobalManager::Instance().GetLanguageList(languageList);
    if (languageList.empty()) {
        languageList.push_back({ currentLangFileName , DUI_T("")});
    }

    // Add menu items dynamically
    for (auto& lang : languageList) {
        const DString fileName = lang.first;
        DString& displayName = lang.second;

        ui::MenuItem* pMenuItem = new ui::MenuItem(menu);
        pMenuItem->SetClass(DUI_T("menu_element"));
        pMenuItem->SetFixedWidth(ui::UiFixedInt(256), true, true);
        ui::CheckBox* pCheckBox = new ui::CheckBox(menu);
        pCheckBox->SetClass(DUI_T("menu_checkbox"));
        pCheckBox->SetAttribute(DUI_T("margin"), DUI_T("0,5,0,10"));
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
