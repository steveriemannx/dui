#include "RenderForm.h"
#include "RenderTest1.h"
#include "RenderTest2.h"
#include "dui/Utils/UiBuilder.h"

void RenderForm::OnInitWindow()
{
    TestPropertyGrid();

    //MenuBar test
    ui::MenuBar* pMenuBar = ui::Find<ui::MenuBar>(this, "menu_bar_test");
    if (pMenuBar != nullptr) {
        pMenuBar->AddTopMenu("1", "File",      "", "public/menu/rich_edit_menu.xml");
        pMenuBar->AddTopMenu("2", "Edit",      "", "controls/menu/settings_menu.xml");
        pMenuBar->AddTopMenu("3", "Selection", "", "public/menu/rich_edit_menu.xml");
        pMenuBar->AddTopMenu("4", "View",      "", "controls/menu/settings_menu.xml");
        pMenuBar->AddTopMenu("5", "View",       "", "public/menu/rich_edit_menu.xml");
        pMenuBar->AddTopMenu("6", "  ...  ",   "", "public/menu/rich_edit_menu.xml", "", "text_padding='8,1,8,7'");
    }

    BindEvents();
    BaseClass::OnInitWindow();
}

void RenderForm::BindEvents()
{
    //Register the response function for menu command activation
    ui::MenuBar* pMenuBar = ui::Find<ui::MenuBar>(this, "menu_bar_test");
    if (pMenuBar == nullptr) {
        return;
    }
    ui::MenuBarItemActivatedEvent callback = [](const std::string& menuItemId,
                                                const std::string& menuName, int32_t nMenuLevel,
                                                const std::string& itemName, size_t nItemIndex) {
            (void)menuItemId;
            (void)menuName;
            (void)nMenuLevel;
            (void)itemName;
            (void)nItemIndex;
            int ii = 0;
        };
    pMenuBar->AttachMenuBarItemActivated(callback);
}

void RenderForm::OnCloseWindow()
{
    CheckPropertyGridResult();
    //After the window is closed, exit the main thread message loop and shut down the program
    PostQuitMsg(0L);
}

ui::Control* RenderForm::CreateControl(const std::string& strClass)
{
    if (strClass == "RenderTest1") {
        return new ui::RenderTest1(this);
    }
    else if (strClass == "RenderTest2") {
        return new ui::RenderTest2(this);
    }
    return nullptr;
}

void RenderForm::TestPropertyGrid()
{
    ui::PropertyGrid* pPropertyGrid = ui::Find<ui::PropertyGrid>(this, "property_grid_test");
    if (pPropertyGrid == nullptr) {
        return;
    }
    pPropertyGrid->SetEnableHeaderCtrl(true, "Property", "Value");
    pPropertyGrid->SetEnableDescriptionArea(true);

    ui::PropertyGridGroup* pGroup = nullptr;
    
    pGroup = pPropertyGrid->AddGroup("Group 1", "Description 1", (size_t)this);
    ASSERT(pGroup->GetGroupData() == (size_t)this);
    auto p = pPropertyGrid->AddTextProperty(pGroup, "Property 1", "Value 1", "Description of Property 1", (size_t)this);
    ASSERT(p->GetPropertyData() == (size_t)this);
    auto p00 = pPropertyGrid->AddTextProperty(pGroup, "Property 2", "Value 2", "Description of Property 2: Disable");
    p00->SetEnabled(false);

    pGroup = pPropertyGrid->AddGroup("Group 2", "Description 2");
    pPropertyGrid->AddTextProperty(pGroup, "Property 2", "Value 2", "Description of Property 2");

    pGroup = pPropertyGrid->AddGroup("Group 3", "Description 3");
    auto p0 = pPropertyGrid->AddTextProperty(pGroup, "Property 3-0 (Text)", "Text Value 3-0", "Description of Property 3");
    auto p1 = pPropertyGrid->AddTextProperty(pGroup, "Property 3-1 (Number)", "3", "Description of Property 3, a number with a Spin control");
    p1->SetEnableSpin(true, -10, 10);

    auto p2 = pPropertyGrid->AddTextProperty(pGroup, "Property 3-2 (Read Only)", "Value 3-2", "Description of Property 3");
    p2->SetReadOnly(true);

    auto p3 = pPropertyGrid->AddTextProperty(pGroup, "Property 3-3 (Password)", "Value 3-3", "Description of Property 3");
    p3->SetPasswordMode(true);

    pGroup = pPropertyGrid->AddGroup("Group 4", "Description 4");
    auto p10 = pPropertyGrid->AddComboProperty(pGroup, "Property 4-1 (Drop Table)", "Text Value 4", "Description of Property 4");
    p10->AddOption("Value 1");
    p10->AddOption("Value 2");
    p10->AddOption("Value 3");
    p10->SetComboListMode(true);

    auto p11 = pPropertyGrid->AddComboProperty(pGroup, "Property 4-1 (Drop List)", "Text Value 4", "Description of Property 4");
    p11->AddOption("Value 1");
    p11->AddOption("Value 2");
    p11->AddOption("Value 3");
    p11->SetComboListMode(false);//Default

    pGroup = pPropertyGrid->AddGroup("Group 5", "Description 5: Font");
    auto p20 = pPropertyGrid->AddFontProperty(pGroup, "Font", "SimSun", "Description: Set Font Name");
    auto p21 = pPropertyGrid->AddFontSizeProperty(pGroup, "Font Size", "No.5", "Description: Set Font Size");

    auto s000 = p21->GetPropertyNewValue();
    auto s001 = p21->GetFontSize();
    auto s002 = p21->GetDpiFontSize();
    auto s003 = p21->GetFontSize("No.6");
    auto s004 = p21->GetDpiFontSize("No.6");

    auto p22 = pPropertyGrid->AddColorProperty(pGroup, "Color", "Blue", "Description: Set Font Color");

    pGroup = pPropertyGrid->AddGroup("Group 6", "Description 6: Date Time");
    pPropertyGrid->AddDateTimeProperty(pGroup, "Date", "2023-12-07", "Description: Set Date");
    pPropertyGrid->AddDateTimeProperty(pGroup, "Date", "2023/12/07", "Description: Set Date");
    pPropertyGrid->AddDateTimeProperty(pGroup, "Date", "2023-12-07", "Description: Set Date", 0,
                                        ui::DateTime::EditFormat::kDateUpDown);
    pPropertyGrid->AddDateTimeProperty(pGroup, "Date Time", "2023-12-07 17:30:02", "Description: Set Date Time", 0, 
                                        ui::DateTime::EditFormat::kDateTimeUpDown);
    pPropertyGrid->AddDateTimeProperty(pGroup, "Date Time", "2023-12-07 17:30", "Description: Set Date Time", 0,
                                        ui::DateTime::EditFormat::kDateMinuteUpDown);
    pPropertyGrid->AddDateTimeProperty(pGroup, "Time", "17:30:02", "Description: Set Time", 0,
                                        ui::DateTime::EditFormat::kTimeUpDown);
    pPropertyGrid->AddDateTimeProperty(pGroup, "Time", "17:30", "Description: Set Time", 0,
                                        ui::DateTime::EditFormat::kMinuteUpDown);

    pGroup = pPropertyGrid->AddGroup("Group 7", "Description 7");
    pPropertyGrid->AddIPAddressProperty(pGroup, "IP Address", "192.168.0.1", "Description: IP Address");
    pPropertyGrid->AddHotKeyProperty(pGroup, "HotKey 1", "Ctrl+C", "Description: HotKey Control 1");
    pPropertyGrid->AddHotKeyProperty(pGroup, "HotKey 2", "Ctrl+Shift+C", "Description: HotKey Control 2");
    pPropertyGrid->AddHotKeyProperty(pGroup, "HotKey 3", "Ctrl+Shift+Alt+C", "Description: HotKey Control 3");
    pPropertyGrid->AddHotKeyProperty(pGroup, "HotKey 4", "Ctrl+Shift", "Description: HotKey Control 4");

    pGroup = pPropertyGrid->AddGroup("Group 8", "Description 8");
    auto p80 = pPropertyGrid->AddFileProperty(pGroup, "File Path", "C:\\Test-Save.txt", "Description: File Path", 0,
                                              false, 
                                              { 
                                                  {"Text File", "*.txt"},
                                                  {"CSV File", "*.csv"},
                                                  {"INI File", "*.ini"},
                                                  {"All Files", "*.*"}
                                              }, 
                                              0, "txt");
    auto p81 = pPropertyGrid->AddFileProperty(pGroup, "File Path", "C:\\Test-Open.txt", "Description: File Path", 0,
                                              true, 
                                              { 
                                                  {"Text File", "*.txt"},
                                                  {"CSV File", "*.csv"},
                                                  {"INI File", "*.ini"},
                                                  {"All Files", "*.*"}
                                              }, 
                                              0, "txt");

    auto p82 = pPropertyGrid->AddDirectoryProperty(pGroup, "Folder", "C:\\Test\\", "Description: Folder");

    return;
}

void RenderForm::CheckPropertyGridResult()
{
    ui::PropertyGrid* pPropertyGrid = ui::Find<ui::PropertyGrid>(this, "property_grid_test");
    if (pPropertyGrid == nullptr) {
        return;
    }

    struct PropertyData
    {
        std::string name;
        std::string oldValue;
        std::string newValue;
    };
    typedef std::vector<PropertyData> PropertyDataList;
    std::map<std::string, PropertyDataList> propertyMap;

    std::vector<ui::PropertyGridProperty*> properties;
    std::vector<ui::PropertyGridGroup*> groups;
    pPropertyGrid->GetGroups(groups);
    for (auto pGroup : groups) {
        if (pGroup != nullptr) {
            PropertyDataList& dataList = propertyMap[pGroup->GetGroupName()];
            pGroup->GetProperties(properties);
            for (auto pProperty : properties) {
                if (pProperty != nullptr) {
                    PropertyData data;
                    data.name = pProperty->GetPropertyName();
                    data.oldValue = pProperty->GetPropertyValue();
                    data.newValue = pProperty->GetPropertyNewValue();
                    dataList.push_back(data);
                }
            }
        }
    }
    propertyMap.clear();
}
