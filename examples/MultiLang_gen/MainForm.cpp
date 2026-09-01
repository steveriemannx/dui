#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"
#include "generated_ui.inc"  // Build-time generated pure C++ UI code (from MultiLang.xml)

void MainForm::OnInitWindow()
{
    BuildUI();
    BindEvents();

    BaseClass::OnInitWindow();
}

void MainForm::BuildUI()
{
    InitMultiLang(this);
}

void MainForm::BindEvents()
{
    /* Show select language menu */
    ui::Button* select = ui::Find<ui::Button>(this, "language");
    ASSERT(select != nullptr);
    if (select == nullptr) {
        return;
    }
    select->AttachClick([this](const ui::EventArgs& args) {
        ui::UiRect rect = args.GetSender()->GetPos();
        ui::UiPoint point;
        //Pop up the menu right below the language button: align the menu's
        //right edge with the button's right edge (RIGHT_TOP alignment)
        point.x = rect.right;
        point.y = rect.bottom;
        ClientToScreen(point);

        ShowPopupMenu(point);
        return true;
    });
}

void MainForm::ShowPopupMenu(const ui::UiPoint& point)
{
    ui::Menu* menu = new ui::Menu(this);// The parent window must be set; otherwise, when the menu pops up, the program status bar becomes inactive
    InitLang_menu(menu);
    menu->ShowMenu("", point);

    // Current language file
    DString currentLangFileName = ui::GlobalManager::Instance().GetLanguageFileName();

    // The list of available language files and their display names
    std::vector<std::pair<DString, DString>> languageList;
    ui::GlobalManager::Instance().GetLanguageList(languageList);
    if (languageList.empty()) {
        languageList.push_back({ currentLangFileName , ""});
    }

    // Add menu items dynamically
    for (auto& lang : languageList) {
        const DString fileName = lang.first;
        DString& displayName = lang.second;

        ui::MenuItem* pMenuItem = new ui::MenuItem(menu);
        pMenuItem->SetClass("menu_element");
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
