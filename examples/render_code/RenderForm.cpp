#include "RenderForm.h"
#include "RenderBuildUI.inc"  // Pure-code-built UI (corresponding to the render.xml layout)
#include "RenderTest1.h"
#include "RenderTest2.h"

RenderForm::RenderForm()
{
}

RenderForm::~RenderForm()
{
}

DString RenderForm::GetSkinFolder()
{
    return _T("");
}

DString RenderForm::GetSkinFile()
{
    // Pure code mode: no layout XML is loaded
    return _T("");
}

void RenderForm::OnInitWindow()
{
    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);

    //Pure-code-built UI (corresponding to the render.xml layout)
    BuildUIFromXml(this);

    TestPropertyGrid();

    //MenuBar test (pure code menu: menu items are built by the builder callback, no XML template)
    ui::MenuBar* pMenuBar = dynamic_cast<ui::MenuBar*>(FindControl(_T("menu_bar_test")));
    if (pMenuBar != nullptr) {
        auto menuBuilder = [](ui::Menu* pMenu) {
            for (int32_t i = 1; i <= 5; ++i) {
                ui::MenuItem* pItem = new ui::MenuItem(pMenu);
                pItem->SetClass(_T("menu_element"));
                pItem->SetName(ui::StringUtil::Printf(_T("menu_item_%d"), i));
                pItem->SetText(ui::StringUtil::Printf(_T("Menu Item %d"), i));
                pItem->SetFixedWidth(ui::UiFixedInt(160), true, true);
                pMenu->AddMenuItem(pItem);
            }
        };
        pMenuBar->AddTopMenu(_T("1"), _T("File"),      menuBuilder);
        pMenuBar->AddTopMenu(_T("2"), _T("Edit"),      menuBuilder);
        pMenuBar->AddTopMenu(_T("3"), _T("Selection"), menuBuilder);
        pMenuBar->AddTopMenu(_T("4"), _T("View"),      menuBuilder);
        pMenuBar->AddTopMenu(_T("5"), _T("View"),       menuBuilder);
        pMenuBar->AddTopMenu(_T("6"), _T("  ...  "),   menuBuilder, _T(""), _T("text_padding='8,1,8,7'"));
    }

    //Register the response function for menu command activation
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
    if (strClass == _T("RenderTest1")) {
        return new ui::RenderTest1(this);
    }
    else if (strClass == _T("RenderTest2")) {
        return new ui::RenderTest2(this);
    }
    return nullptr;
}

void RenderForm::TestPropertyGrid()
{
    ui::PropertyGrid* pPropertyGrid = dynamic_cast<ui::PropertyGrid*>(FindControl(_T("property_grid_test")));
    if (pPropertyGrid == nullptr) {
        return;
    }
    pPropertyGrid->SetEnableHeaderCtrl(true, _T("Property"), _T("Value"));
    pPropertyGrid->SetEnableDescriptionArea(true);

    ui::PropertyGridGroup* pGroup = nullptr;
    
    pGroup = pPropertyGrid->AddGroup(_T("Group 1"), _T("Description 1"), (size_t)this);
    ASSERT(pGroup->GetGroupData() == (size_t)this);
    auto p = pPropertyGrid->AddTextProperty(pGroup, _T("Property 1"), _T("Value 1"), _T("Description of Property 1"), (size_t)this);
    ASSERT(p->GetPropertyData() == (size_t)this);
    auto p00 = pPropertyGrid->AddTextProperty(pGroup, _T("Property 2"), _T("Value 2"), _T("Description of Property 2: Disable"));
    p00->SetEnabled(false);

    pGroup = pPropertyGrid->AddGroup(_T("Group 2"), _T("Description 2"));
    pPropertyGrid->AddTextProperty(pGroup, _T("Property 2"), _T("Value 2"), _T("Description of Property 2"));

    pGroup = pPropertyGrid->AddGroup(_T("Group 3"), _T("Description 3"));
    auto p0 = pPropertyGrid->AddTextProperty(pGroup, _T("Property 3-0 (Text)"), _T("Text Value 3-0"), _T("Description of Property 3"));
    auto p1 = pPropertyGrid->AddTextProperty(pGroup, _T("Property 3-1 (Number)"), _T("3"), _T("Description of Property 3, a number with a Spin control"));
    p1->SetEnableSpin(true, -10, 10);

    auto p2 = pPropertyGrid->AddTextProperty(pGroup, _T("Property 3-2 (Read Only)"), _T("Value 3-2"), _T("Description of Property 3"));
    p2->SetReadOnly(true);

    auto p3 = pPropertyGrid->AddTextProperty(pGroup, _T("Property 3-3 (Password)"), _T("Value 3-3"), _T("Description of Property 3"));
    p3->SetPasswordMode(true);

    pGroup = pPropertyGrid->AddGroup(_T("Group 4"), _T("Description 4"));
    auto p10 = pPropertyGrid->AddComboProperty(pGroup, _T("Property 4-1 (Drop Table)"), _T("Text Value 4"), _T("Description of Property 4"));
    p10->AddOption(_T("Value 1"));
    p10->AddOption(_T("Value 2"));
    p10->AddOption(_T("Value 3"));
    p10->SetComboListMode(true);

    auto p11 = pPropertyGrid->AddComboProperty(pGroup, _T("Property 4-1 (Drop List)"), _T("Text Value 4"), _T("Description of Property 4"));
    p11->AddOption(_T("Value 1"));
    p11->AddOption(_T("Value 2"));
    p11->AddOption(_T("Value 3"));
    p11->SetComboListMode(false);//Default

    pGroup = pPropertyGrid->AddGroup(_T("Group 5"), _T("Description 5: Font"));
    auto p20 = pPropertyGrid->AddFontProperty(pGroup, _T("Font"), _T("SimSun"), _T("Description: Set Font Name"));
    auto p21 = pPropertyGrid->AddFontSizeProperty(pGroup, _T("Font Size"), _T("No.5"), _T("Description: Set Font Size"));

    auto s000 = p21->GetPropertyNewValue();
    auto s001 = p21->GetFontSize();
    auto s002 = p21->GetDpiFontSize();
    auto s003 = p21->GetFontSize(_T("No.6"));
    auto s004 = p21->GetDpiFontSize(_T("No.6"));

    auto p22 = pPropertyGrid->AddColorProperty(pGroup, _T("Color"), _T("Blue"), _T("Description: Set Font Color"));

    pGroup = pPropertyGrid->AddGroup(_T("Group 6"), _T("Description 6: Date Time"));
    pPropertyGrid->AddDateTimeProperty(pGroup, _T("Date"), _T("2023-12-07"), _T("Description: Set Date"));
    pPropertyGrid->AddDateTimeProperty(pGroup, _T("Date"), _T("2023/12/07"), _T("Description: Set Date"));
    pPropertyGrid->AddDateTimeProperty(pGroup, _T("Date"), _T("2023-12-07"), _T("Description: Set Date"), 0,
                                        ui::DateTime::EditFormat::kDateUpDown);
    pPropertyGrid->AddDateTimeProperty(pGroup, _T("Date Time"), _T("2023-12-07 17:30:02"), _T("Description: Set Date Time"), 0, 
                                        ui::DateTime::EditFormat::kDateTimeUpDown);
    pPropertyGrid->AddDateTimeProperty(pGroup, _T("Date Time"), _T("2023-12-07 17:30"), _T("Description: Set Date Time"), 0,
                                        ui::DateTime::EditFormat::kDateMinuteUpDown);
    pPropertyGrid->AddDateTimeProperty(pGroup, _T("Time"), _T("17:30:02"), _T("Description: Set Time"), 0,
                                        ui::DateTime::EditFormat::kTimeUpDown);
    pPropertyGrid->AddDateTimeProperty(pGroup, _T("Time"), _T("17:30"), _T("Description: Set Time"), 0,
                                        ui::DateTime::EditFormat::kMinuteUpDown);

    pGroup = pPropertyGrid->AddGroup(_T("Group 7"), _T("Description 7"));
    pPropertyGrid->AddIPAddressProperty(pGroup, _T("IP Address"), _T("192.168.0.1"), _T("Description: IP Address"));
    pPropertyGrid->AddHotKeyProperty(pGroup, _T("HotKey 1"), _T("Ctrl+C"), _T("Description: HotKey Control 1"));
    pPropertyGrid->AddHotKeyProperty(pGroup, _T("HotKey 2"), _T("Ctrl+Shift+C"), _T("Description: HotKey Control 2"));
    pPropertyGrid->AddHotKeyProperty(pGroup, _T("HotKey 3"), _T("Ctrl+Shift+Alt+C"), _T("Description: HotKey Control 3"));
    pPropertyGrid->AddHotKeyProperty(pGroup, _T("HotKey 4"), _T("Ctrl+Shift"), _T("Description: HotKey Control 4"));

    pGroup = pPropertyGrid->AddGroup(_T("Group 8"), _T("Description 8"));
    auto p80 = pPropertyGrid->AddFileProperty(pGroup, _T("File Path"), _T("C:\\Test-Save.txt"), _T("Description: File Path"), 0,
                                              false, 
                                              { 
                                                  {_T("Text File"), _T("*.txt")},
                                                  {_T("CSV File"), _T("*.csv")},
                                                  {_T("INI File"), _T("*.ini")},
                                                  {_T("All Files"), _T("*.*")}
                                              }, 
                                              0, _T("txt"));
    auto p81 = pPropertyGrid->AddFileProperty(pGroup, _T("File Path"), _T("C:\\Test-Open.txt"), _T("Description: File Path"), 0,
                                              true, 
                                              { 
                                                  {_T("Text File"), _T("*.txt")},
                                                  {_T("CSV File"), _T("*.csv")},
                                                  {_T("INI File"), _T("*.ini")},
                                                  {_T("All Files"), _T("*.*")}
                                              }, 
                                              0, _T("txt"));

    auto p82 = pPropertyGrid->AddDirectoryProperty(pGroup, _T("Folder"), _T("C:\\Test\\"), _T("Description: Folder"));

    return;
}

void RenderForm::CheckPropertyGridResult()
{
    ui::PropertyGrid* pPropertyGrid = dynamic_cast<ui::PropertyGrid*>(FindControl(_T("property_grid_test")));
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
