#include "RenderForm.h"
#include "dui/Utils/UiBuilder.h"
#include "generated_ui.inc"  // Build-time generated pure C++ UI code (from render.xml)
#include "RenderTest1.h"
#include "RenderTest2.h"

void RenderForm::BuildUI()
{
    // Build-time generated from render.xml
    ::InitRender(this);

    // Add the custom draw-test controls to the Draw pages.
    if (ui::Box* pPage5 = ui::Find<ui::Box>(this, DUI_T("main_view_page_05"))) {
        ui::RenderTest1* pRenderTest1 = new ui::RenderTest1(this);
        pRenderTest1->SetAttribute(DUI_T("width"), DUI_T("stretch"));
        pRenderTest1->SetAttribute(DUI_T("height"), DUI_T("stretch"));
        pPage5->AddItem(pRenderTest1);
    }
    if (ui::Box* pPage6 = ui::Find<ui::Box>(this, DUI_T("main_view_page_06"))) {
        ui::RenderTest2* pRenderTest2 = new ui::RenderTest2(this);
        pRenderTest2->SetAttribute(DUI_T("width"), DUI_T("stretch"));
        pRenderTest2->SetAttribute(DUI_T("height"), DUI_T("stretch"));
        pPage6->AddItem(pRenderTest2);
    }
}

void RenderForm::OnInitWindow()
{
    BuildUI();
    TestPropertyGrid();

    //MenuBar test (pure code menu: menu items are built by the builder callback, no XML template)
    ui::MenuBar* pMenuBar = ui::Find<ui::MenuBar>(this, DUI_T("menu_bar_test"));
    if (pMenuBar != nullptr) {
        auto menuBuilder = [](ui::Menu* pMenu) {
            for (int32_t i = 1; i <= 5; ++i) {
                ui::MenuItem* pItem = new ui::MenuItem(pMenu);
                pItem->SetClass(DUI_T("menu_element"));
                pItem->SetName(ui::StringUtil::Printf(DUI_T("menu_item_%d"), i));
                pItem->SetText(ui::StringUtil::Printf(DUI_T("Menu Item %d"), i));
                pItem->SetFixedWidth(ui::UiFixedInt(160), true, true);
                pMenu->AddMenuItem(pItem);
            }
        };
        pMenuBar->AddTopMenu(DUI_T("1"), DUI_T("File"),      menuBuilder);
        pMenuBar->AddTopMenu(DUI_T("2"), DUI_T("Edit"),      menuBuilder);
        pMenuBar->AddTopMenu(DUI_T("3"), DUI_T("Selection"), menuBuilder);
        pMenuBar->AddTopMenu(DUI_T("4"), DUI_T("View"),      menuBuilder);
        pMenuBar->AddTopMenu(DUI_T("5"), DUI_T("View"),       menuBuilder);
        pMenuBar->AddTopMenu(DUI_T("6"), DUI_T("  ...  "),   menuBuilder, DUI_T(""), DUI_T("text_padding='8,1,8,7'"));
    }

    BindEvents();
    BaseClass::OnInitWindow();
}

void RenderForm::BindEvents()
{
    //Register the response function for menu command activation
    ui::MenuBar* pMenuBar = ui::Find<ui::MenuBar>(this, DUI_T("menu_bar_test"));
    if (pMenuBar == nullptr) {
        return;
    }
    ui::MenuBarItemActivatedEvent callback = [](const DString& menuItemId,
                                                const DString& menuName, int32_t nMenuLevel,
                                                const DString& itemName, size_t nItemIndex) {
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

ui::Control* RenderForm::CreateControl(const DString& strClass)
{
    if (strClass == DUI_T("RenderTest1")) {
        return new ui::RenderTest1(this);
    }
    else if (strClass == DUI_T("RenderTest2")) {
        return new ui::RenderTest2(this);
    }
    return nullptr;
}

void RenderForm::TestPropertyGrid()
{
    ui::PropertyGrid* pPropertyGrid = ui::Find<ui::PropertyGrid>(this, DUI_T("property_grid_test"));
    if (pPropertyGrid == nullptr) {
        return;
    }
    pPropertyGrid->SetEnableHeaderCtrl(true, DUI_T("Property"), DUI_T("Value"));
    pPropertyGrid->SetEnableDescriptionArea(true);

    ui::PropertyGridGroup* pGroup = nullptr;
    
    pGroup = pPropertyGrid->AddGroup(DUI_T("Group 1"), DUI_T("Description 1"), (size_t)this);
    ASSERT(pGroup->GetGroupData() == (size_t)this);
    auto p = pPropertyGrid->AddTextProperty(pGroup, DUI_T("Property 1"), DUI_T("Value 1"), DUI_T("Description of Property 1"), (size_t)this);
    ASSERT(p->GetPropertyData() == (size_t)this);
    auto p00 = pPropertyGrid->AddTextProperty(pGroup, DUI_T("Property 2"), DUI_T("Value 2"), DUI_T("Description of Property 2: Disable"));
    p00->SetEnabled(false);

    pGroup = pPropertyGrid->AddGroup(DUI_T("Group 2"), DUI_T("Description 2"));
    pPropertyGrid->AddTextProperty(pGroup, DUI_T("Property 2"), DUI_T("Value 2"), DUI_T("Description of Property 2"));

    pGroup = pPropertyGrid->AddGroup(DUI_T("Group 3"), DUI_T("Description 3"));
    auto p0 = pPropertyGrid->AddTextProperty(pGroup, DUI_T("Property 3-0 (Text)"), DUI_T("Text Value 3-0"), DUI_T("Description of Property 3"));
    auto p1 = pPropertyGrid->AddTextProperty(pGroup, DUI_T("Property 3-1 (Number)"), DUI_T("3"), DUI_T("Description of Property 3, a number with a Spin control"));
    p1->SetEnableSpin(true, -10, 10);

    auto p2 = pPropertyGrid->AddTextProperty(pGroup, DUI_T("Property 3-2 (Read Only)"), DUI_T("Value 3-2"), DUI_T("Description of Property 3"));
    p2->SetReadOnly(true);

    auto p3 = pPropertyGrid->AddTextProperty(pGroup, DUI_T("Property 3-3 (Password)"), DUI_T("Value 3-3"), DUI_T("Description of Property 3"));
    p3->SetPasswordMode(true);

    pGroup = pPropertyGrid->AddGroup(DUI_T("Group 4"), DUI_T("Description 4"));
    auto p10 = pPropertyGrid->AddComboProperty(pGroup, DUI_T("Property 4-1 (Drop Table)"), DUI_T("Text Value 4"), DUI_T("Description of Property 4"));
    p10->AddOption(DUI_T("Value 1"));
    p10->AddOption(DUI_T("Value 2"));
    p10->AddOption(DUI_T("Value 3"));
    p10->SetComboListMode(true);

    auto p11 = pPropertyGrid->AddComboProperty(pGroup, DUI_T("Property 4-1 (Drop List)"), DUI_T("Text Value 4"), DUI_T("Description of Property 4"));
    p11->AddOption(DUI_T("Value 1"));
    p11->AddOption(DUI_T("Value 2"));
    p11->AddOption(DUI_T("Value 3"));
    p11->SetComboListMode(false);//Default

    pGroup = pPropertyGrid->AddGroup(DUI_T("Group 5"), DUI_T("Description 5: Font"));
    auto p20 = pPropertyGrid->AddFontProperty(pGroup, DUI_T("Font"), DUI_T("SimSun"), DUI_T("Description: Set Font Name"));
    auto p21 = pPropertyGrid->AddFontSizeProperty(pGroup, DUI_T("Font Size"), DUI_T("No.5"), DUI_T("Description: Set Font Size"));

    auto s000 = p21->GetPropertyNewValue();
    auto s001 = p21->GetFontSize();
    auto s002 = p21->GetDpiFontSize();
    auto s003 = p21->GetFontSize(DUI_T("No.6"));
    auto s004 = p21->GetDpiFontSize(DUI_T("No.6"));

    auto p22 = pPropertyGrid->AddColorProperty(pGroup, DUI_T("Color"), DUI_T("Blue"), DUI_T("Description: Set Font Color"));

    pGroup = pPropertyGrid->AddGroup(DUI_T("Group 6"), DUI_T("Description 6: Date Time"));
    pPropertyGrid->AddDateTimeProperty(pGroup, DUI_T("Date"), DUI_T("2023-12-07"), DUI_T("Description: Set Date"));
    pPropertyGrid->AddDateTimeProperty(pGroup, DUI_T("Date"), DUI_T("2023/12/07"), DUI_T("Description: Set Date"));
    pPropertyGrid->AddDateTimeProperty(pGroup, DUI_T("Date"), DUI_T("2023-12-07"), DUI_T("Description: Set Date"), 0,
                                        ui::DateTime::EditFormat::kDateUpDown);
    pPropertyGrid->AddDateTimeProperty(pGroup, DUI_T("Date Time"), DUI_T("2023-12-07 17:30:02"), DUI_T("Description: Set Date Time"), 0, 
                                        ui::DateTime::EditFormat::kDateTimeUpDown);
    pPropertyGrid->AddDateTimeProperty(pGroup, DUI_T("Date Time"), DUI_T("2023-12-07 17:30"), DUI_T("Description: Set Date Time"), 0,
                                        ui::DateTime::EditFormat::kDateMinuteUpDown);
    pPropertyGrid->AddDateTimeProperty(pGroup, DUI_T("Time"), DUI_T("17:30:02"), DUI_T("Description: Set Time"), 0,
                                        ui::DateTime::EditFormat::kTimeUpDown);
    pPropertyGrid->AddDateTimeProperty(pGroup, DUI_T("Time"), DUI_T("17:30"), DUI_T("Description: Set Time"), 0,
                                        ui::DateTime::EditFormat::kMinuteUpDown);

    pGroup = pPropertyGrid->AddGroup(DUI_T("Group 7"), DUI_T("Description 7"));
    pPropertyGrid->AddIPAddressProperty(pGroup, DUI_T("IP Address"), DUI_T("192.168.0.1"), DUI_T("Description: IP Address"));
    pPropertyGrid->AddHotKeyProperty(pGroup, DUI_T("HotKey 1"), DUI_T("Ctrl+C"), DUI_T("Description: HotKey Control 1"));
    pPropertyGrid->AddHotKeyProperty(pGroup, DUI_T("HotKey 2"), DUI_T("Ctrl+Shift+C"), DUI_T("Description: HotKey Control 2"));
    pPropertyGrid->AddHotKeyProperty(pGroup, DUI_T("HotKey 3"), DUI_T("Ctrl+Shift+Alt+C"), DUI_T("Description: HotKey Control 3"));
    pPropertyGrid->AddHotKeyProperty(pGroup, DUI_T("HotKey 4"), DUI_T("Ctrl+Shift"), DUI_T("Description: HotKey Control 4"));

    pGroup = pPropertyGrid->AddGroup(DUI_T("Group 8"), DUI_T("Description 8"));
    auto p80 = pPropertyGrid->AddFileProperty(pGroup, DUI_T("File Path"), DUI_T("C:\\Test-Save.txt"), DUI_T("Description: File Path"), 0,
                                              false, 
                                              { 
                                                  {DUI_T("Text File"), DUI_T("*.txt")},
                                                  {DUI_T("CSV File"), DUI_T("*.csv")},
                                                  {DUI_T("INI File"), DUI_T("*.ini")},
                                                  {DUI_T("All Files"), DUI_T("*.*")}
                                              }, 
                                              0, DUI_T("txt"));
    auto p81 = pPropertyGrid->AddFileProperty(pGroup, DUI_T("File Path"), DUI_T("C:\\Test-Open.txt"), DUI_T("Description: File Path"), 0,
                                              true, 
                                              { 
                                                  {DUI_T("Text File"), DUI_T("*.txt")},
                                                  {DUI_T("CSV File"), DUI_T("*.csv")},
                                                  {DUI_T("INI File"), DUI_T("*.ini")},
                                                  {DUI_T("All Files"), DUI_T("*.*")}
                                              }, 
                                              0, DUI_T("txt"));

    auto p82 = pPropertyGrid->AddDirectoryProperty(pGroup, DUI_T("Folder"), DUI_T("C:\\Test\\"), DUI_T("Description: Folder"));

    return;
}

void RenderForm::CheckPropertyGridResult()
{
    ui::PropertyGrid* pPropertyGrid = ui::Find<ui::PropertyGrid>(this, DUI_T("property_grid_test"));
    if (pPropertyGrid == nullptr) {
        return;
    }

    struct PropertyData
    {
        DString name;
        DString oldValue;
        DString newValue;
    };
    typedef std::vector<PropertyData> PropertyDataList;
    std::map<DString, PropertyDataList> propertyMap;

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
