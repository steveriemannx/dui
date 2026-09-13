#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"
#include <fstream>

void MainForm::OnInitWindow()
{
    // TEMP DEBUG: dump font manager state
    {
        ui::IRenderFactory* pRenderFactory = ui::GlobalManager::Instance().GetRenderFactory();
        if (pRenderFactory != nullptr) {
            ui::IFontMgr* pFontMgr = pRenderFactory->GetFontMgr();
            if (pFontMgr != nullptr) {
                std::ofstream ofs("D:/projects-main/dui/font_dump.txt");
                ofs << "count=" << pFontMgr->GetFontCount() << "\n";
                uint32_t nCount = pFontMgr->GetFontCount();
                for (uint32_t i = 0; i < nCount && i < 4000; ++i) {
                    std::string name;
                    pFontMgr->GetFontName(i, name);
                    std::string utf8 = ui::StringConvert::TToUTF8(name);
                    if (utf8.find("YaHei") != std::string::npos || utf8.find("Segoe") != std::string::npos ||
                        utf8.find("Sim") != std::string::npos || utf8.find("Noto") != std::string::npos ||
                        utf8.find("PingFang") != std::string::npos || utf8.find("Roboto") != std::string::npos ||
                        i < 40) {
                        ofs << "fam[" << i << "]=" << utf8 << "\n";
                    }
                }
                {
                    std::string s1 = ui::StringConvert::UTF8ToT("Microsoft YaHei");
                    std::string s2 = ui::StringConvert::UTF8ToT("Segoe UI Variable");
                    std::string s3 = ui::StringConvert::UTF8ToT("Microsoft YaHei UI");
                    ofs << "HasFontName(Microsoft YaHei)=" << (int)pFontMgr->HasFontName(s1) << "\n";
                    ofs << "HasFontName(Segoe UI Variable)=" << (int)pFontMgr->HasFontName(s2) << "\n";
                    ofs << "HasFontName(Microsoft YaHei UI)=" << (int)pFontMgr->HasFontName(s3) << "\n";
                }
                ofs.close();
            }
        }
    }
    BindEvents();
    BaseClass::OnInitWindow();
}

void MainForm::BindEvents()
{
    // Window initialization is complete; this form can now be initialized

    /* Show select language menu */
    ui::Button* select = ui::Find<ui::Button>(this, "language");
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
    std::string xml("lang_menu.xml");
    menu->ShowMenu(xml, point);

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
