#include "MainForm.h"

MainForm::MainForm()
{
}

MainForm::~MainForm()
{
}

DString MainForm::GetSkinFolder()
{
    return _T("MultiLang");
}

DString MainForm::GetSkinFile()
{
    return _T("MultiLang.xml");
}

void MainForm::OnInitWindow()
{
    // Window initialization is complete; this form can now be initialized

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
}

void MainForm::ShowPopupMenu(const ui::UiPoint& point)
{
    ui::Menu* menu = new ui::Menu(this);// The parent window must be set; otherwise, when the menu pops up, the program status bar becomes inactive
    // Set the directory where the menu XML is located
    menu->SetSkinFolder(GetResourcePath().ToString());
    DString xml(_T("lang_menu.xml"));
    menu->ShowMenu(xml, point);

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
