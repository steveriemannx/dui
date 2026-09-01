#include "RenderForm.h"
#include "dui/Utils/UiBuilder.h"
#include "RenderTest1.h"
#include "RenderTest2.h"

void BuildUIFromXmlRender(ui::Window* pWindow);

void RenderForm::SetupWindow()
{
    ui::UiRect rcWork;
    ui::WindowBase::GetPrimaryMonitorWorkRect(rcWork);
    SetWindowSize((int32_t)(rcWork.Width() * 0.85f), (int32_t)(rcWork.Height() * 0.95f));
    CenterWindow();

    SetShadowAttached(true);
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);

    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);
}

void RenderForm::BuildUI()
{
    //Pure-code-built UI (corresponding to the render.xml layout)
    BuildUIFromXmlRender(this);

    // Add the custom draw-test controls to the Draw pages.
    if (ui::Box* pPage5 = ui::Find<ui::Box>(this, "main_view_page_05")) {
        ui::RenderTest1* pRenderTest1 = new ui::RenderTest1(this);
        pRenderTest1->SetAttribute("width", "stretch");
        pRenderTest1->SetAttribute("height", "stretch");
        pPage5->AddItem(pRenderTest1);
    }
    if (ui::Box* pPage6 = ui::Find<ui::Box>(this, "main_view_page_06")) {
        ui::RenderTest2* pRenderTest2 = new ui::RenderTest2(this);
        pRenderTest2->SetAttribute("width", "stretch");
        pRenderTest2->SetAttribute("height", "stretch");
        pPage6->AddItem(pRenderTest2);
    }
}

void RenderForm::OnInitWindow()
{
    SetupWindow();
    BuildUI();
    TestPropertyGrid();

    //MenuBar test (pure code menu: menu items are built by the builder callback, no XML template)
    ui::MenuBar* pMenuBar = ui::Find<ui::MenuBar>(this, "menu_bar_test");
    if (pMenuBar != nullptr) {
        auto menuBuilder = [](ui::Menu* pMenu) {
            for (int32_t i = 1; i <= 5; ++i) {
                ui::MenuItem* pItem = new ui::MenuItem(pMenu);
                pItem->SetClass("menu_element");
                pItem->SetName(ui::StringUtil::Printf("menu_item_%d", i));
                pItem->SetText(ui::StringUtil::Printf("Menu Item %d", i));
                pItem->SetFixedWidth(ui::UiFixedInt(160), true, true);
                pMenu->AddMenuItem(pItem);
            }
        };
        pMenuBar->AddTopMenu("1", "File",      menuBuilder);
        pMenuBar->AddTopMenu("2", "Edit",      menuBuilder);
        pMenuBar->AddTopMenu("3", "Selection", menuBuilder);
        pMenuBar->AddTopMenu("4", "View",      menuBuilder);
        pMenuBar->AddTopMenu("5", "View",       menuBuilder);
        pMenuBar->AddTopMenu("6", "  ...  ",   menuBuilder, "", "text_padding='8,1,8,7'");
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

///////////////////////////////////////////////////////////////////////////
//   AUTO-GENERATED by xml_to_code - DO NOT EDIT BY HAND
//
//   Source XML files:
//     resources/themes/macos26/render/render.xml
//
//   Each XML file produces one function:
//     BuildUIFromXml + FileStem
//     BuildUIFromXmlRender(ui::Window* pWindow)
//
//   Functions ending in 'Window' call ui::Attach(pWindow, p0) automatically.
//   Other functions (templates, items) do NOT attach the root - the
//   caller must add the root control to a parent container.
///////////////////////////////////////////////////////////////////////////

#include "dui/dui.h"
#include "dui/Utils/UiBuilder.h"

void BuildUIFromXmlRender(ui::Window* pWindow) {
    auto& w = *pWindow;
    { ui::UiSize size; bool scaledCX = false; bool scaledCY = false;
      bool percentCX = false; bool percentCY = false;
      ui::AttributeUtil::ParseWindowSize(pWindow, "85%,95%", size, &scaledCX, &scaledCY, &percentCX, &percentCY);
      w.SetWindowSize(size.cx, size.cy); }
    w.SetUseSystemCaption(false);
    w.SetEnableSnapLayoutMenu(true);
    w.SetEnableSysMenu(true);
    w.SetSysMenuRect(ui::UiRect(0, 0, 36, 36), true);
    w.SetCaptionRect(ui::UiRect(0, 0, 0, 36), true);
    w.SetLayeredWindow(false, false);
    w.SetSizeBox(ui::UiRect(4, 4, 4, 4), true);
    w.SetText("Render Engine Test Program");
    w.SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    w.SetShadowBorderSize(0);
    w.SetShadowAttached(true);
    w.CenterWindow();
    auto* p0 = ui::Create<ui::VBox>(pWindow, {{"bkcolor", "bk_wnd_darkcolor"}});
    auto* p1 = ui::Create<ui::HBox>(pWindow, {{"name", "window_caption_bar"}, {"width", "stretch"}, {"height", "36"}, {"bkcolor", "bk_wnd_lightcolor"}});
    auto* p2 = ui::Create<ui::HBox>(pWindow, {{"margin", "0,0,30,0"}, {"valign", "center"}, {"width", "auto"}, {"height", "auto"}, {"mouse_enabled", "false"}});
    auto* p3 = ui::Create<ui::Control>(pWindow, {{"width", "18"}, {"height", "18"}, {"bkimage", "public/caption/logo.svg"}, {"valign", "center"}, {"margin", "8,0,0,0"}});
    ui::Attach(p2, p3);

    auto* p4 = ui::Create<ui::Label>(pWindow, {{"text", "Render Engine Test"}, {"valign", "center"}, {"margin", "8,0,0,0"}, {"mouse_enabled", "false"}});
    ui::Attach(p2, p4);

    ui::Attach(p1, p2);

    auto* p5 = ui::Create<ui::Control>(pWindow, {{"mouse_enabled", "false"}});
    ui::Attach(p1, p5);

    auto* p6 = ui::Create<ui::HBox>(pWindow, {{"margin", "0,0,0,0"}, {"valign", "center"}, {"width", "auto"}, {"height", "36"}});
    ui::Attach(p1, p6);

    ui::Attach(p0, p1);

    auto* p7 = ui::Create<ui::VBox>(pWindow, {});
    auto* p8 = ui::Create<ui::TabCtrl>(pWindow, {{"class", "tab_ctrl"}, {"tab_box_name", "main_view_tab"}, {"selected_id", "0"}});
    auto* p9 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Image 1"}, {"tab_box_item_index", "0"}, {"tooltip_text", "Image 1"}});
    ui::Attach(p8, p9);

    auto* p10 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Image 2"}, {"tab_box_item_index", "1"}, {"tooltip_text", "Image 2"}});
    ui::Attach(p8, p10);

    auto* p11 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Image 3"}, {"tab_box_item_index", "2"}, {"tooltip_text", "Image 3"}});
    ui::Attach(p8, p11);

    auto* p12 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Text - Horizontal"}, {"tab_box_item_index", "3"}, {"tooltip_text", "Text - Horizontal"}});
    ui::Attach(p8, p12);

    auto* p13 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Text - Vertical"}, {"tab_box_item_index", "4"}, {"tooltip_text", "Text - Vertical"}});
    ui::Attach(p8, p13);

    auto* p14 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Draw"}, {"tab_box_item_index", "5"}, {"tooltip_text", "Draw"}});
    ui::Attach(p8, p14);

    auto* p15 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Draw"}, {"tab_box_item_index", "6"}, {"tooltip_text", "Draw"}});
    ui::Attach(p8, p15);

    auto* p16 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "BoxShadow"}, {"tab_box_item_index", "7"}, {"tooltip_text", "BoxShadow"}});
    ui::Attach(p8, p16);

    auto* p17 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Property and Tab Pages"}, {"tab_box_item_index", "8"}, {"tooltip_text", "Property and Tab Pages"}});
    ui::Attach(p8, p17);

    auto* p18 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Window Shadow"}, {"tab_box_item_index", "9"}, {"tooltip_text", "Window Shadow"}});
    ui::Attach(p8, p18);

    auto* p19 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Bitmap Control"}, {"tab_box_item_index", "10"}, {"tooltip_text", "Bitmap Control"}});
    ui::Attach(p8, p19);

    auto* p20 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Control Test"}, {"tab_box_item_index", "11"}, {"tooltip_text", "Control Test"}});
    ui::Attach(p8, p20);

    auto* p21 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Control Animation"}, {"tab_box_item_index", "12"}, {"tooltip_text", "Control Animation"}});
    ui::Attach(p8, p21);

    ui::Attach(p7, p8);

    auto* p22 = ui::Create<ui::TabBox>(pWindow, {{"name", "main_view_tab"}, {"bkcolor", "white"}, {"selected_id", "0"}, {"fade_switch_type", "FadeInOut"}, {"fade_switch_frame_interval_ms", "16"}, {"fade_switch_total_ms", "180"}, {"fade_switch_easing_function", "EaseOutCubic"}});
    auto* p23 = ui::Create<ui::VBox>(pWindow, {{"name", "main_view_page_00"}});
    auto* p24 = ui::Create<ui::Label>(pWindow, {{"text", "Page 1: Test all supported image formats with standard controls"}});
    ui::Attach(p23, p24);

    auto* p25 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p26 = ui::Create<ui::Control>(pWindow, {{"width", "auto"}, {"height", "auto"}, {"bkimage", "file='bmp_test_24.bmp' width='140'"}, {"valign", "center"}, {"margin", "8"}});
    ui::Attach(p25, p26);

    auto* p27 = ui::Create<ui::Control>(pWindow, {{"width", "auto"}, {"height", "auto"}, {"bkimage", "file='webp_test2.webp' width='140'"}, {"valign", "center"}, {"margin", "8"}});
    ui::Attach(p25, p27);

    auto* p28 = ui::Create<ui::Control>(pWindow, {{"width", "auto"}, {"height", "auto"}, {"bkimage", "file='jpg_test.jpg' width='140'"}, {"valign", "center"}, {"margin", "8"}});
    ui::Attach(p25, p28);

    auto* p29 = ui::Create<ui::Control>(pWindow, {{"width", "auto"}, {"height", "auto"}, {"bkimage", "file='icon_test.ico' width='140' icon_size='256'"}, {"valign", "center"}, {"margin", "8"}});
    ui::Attach(p25, p29);

    auto* p30 = ui::Create<ui::Control>(pWindow, {{"width", "auto"}, {"height", "auto"}, {"bkimage", "file='autumn.png' width='140'"}, {"valign", "center"}, {"margin", "8"}});
    ui::Attach(p25, p30);

    auto* p31 = ui::Create<ui::Control>(pWindow, {{"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.svg' width='140'"}, {"valign", "center"}, {"margin", "8"}});
    ui::Attach(p25, p31);

    auto* p32 = ui::Create<ui::Control>(pWindow, {{"width", "auto"}, {"height", "auto"}, {"bkimage", "file='gif_test2.gif' width='140'"}, {"valign", "center"}, {"margin", "8"}});
    ui::Attach(p25, p32);

    ui::Attach(p23, p25);

    auto* p33 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p34 = ui::Create<ui::Label>(pWindow, {{"width", "140"}, {"height", "auto"}, {"text", "BMP Format"}, {"text_align", "center"}, {"margin", "8"}});
    ui::Attach(p33, p34);

    auto* p35 = ui::Create<ui::Label>(pWindow, {{"width", "140"}, {"height", "auto"}, {"text", "WEBP Format"}, {"text_align", "center"}, {"margin", "8"}});
    ui::Attach(p33, p35);

    auto* p36 = ui::Create<ui::Label>(pWindow, {{"width", "140"}, {"height", "auto"}, {"text", "JPG Format"}, {"text_align", "center"}, {"margin", "8"}});
    ui::Attach(p33, p36);

    auto* p37 = ui::Create<ui::Label>(pWindow, {{"width", "140"}, {"height", "auto"}, {"text", "ICO Format"}, {"text_align", "center"}, {"margin", "8"}});
    ui::Attach(p33, p37);

    auto* p38 = ui::Create<ui::Label>(pWindow, {{"width", "140"}, {"height", "auto"}, {"text", "PNG Format"}, {"text_align", "center"}, {"margin", "8"}});
    ui::Attach(p33, p38);

    auto* p39 = ui::Create<ui::Label>(pWindow, {{"width", "140"}, {"height", "auto"}, {"text", "SVG Format"}, {"text_align", "center"}, {"margin", "8"}});
    ui::Attach(p33, p39);

    auto* p40 = ui::Create<ui::Label>(pWindow, {{"width", "140"}, {"height", "auto"}, {"text", "GIF Format"}, {"text_align", "center"}, {"margin", "8"}});
    ui::Attach(p33, p40);

    ui::Attach(p23, p33);

    auto* p41 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p42 = ui::Create<ui::Control>(pWindow, {{"width", "150"}, {"height", "auto"}, {"bkimage", "file='gif_test.gif' width='90' playcount='-1' halign='center' valign='center'"}, {"valign", "center"}, {"margin", "8"}});
    ui::Attach(p41, p42);

    auto* p43 = ui::Create<ui::Control>(pWindow, {{"width", "auto"}, {"height", "auto"}, {"bkimage", "file='apng_test.png' width='150' playcount='-1'"}, {"valign", "center"}, {"margin", "8"}});
    ui::Attach(p41, p43);

    auto* p44 = ui::Create<ui::Control>(pWindow, {{"width", "auto"}, {"height", "auto"}, {"bkimage", "file='webp_test.webp' width='150' playcount='-1'"}, {"valign", "center"}, {"margin", "8"}});
    ui::Attach(p41, p44);

    auto* p45 = ui::Create<ui::Control>(pWindow, {{"width", "auto"}, {"height", "auto"}, {"bkimage", "file='lottie.json' width='150' playcount='-1'"}, {"valign", "center"}, {"margin", "8"}});
    ui::Attach(p41, p45);

    auto* p46 = ui::Create<ui::Control>(pWindow, {{"width", "auto"}, {"height", "auto"}, {"bkimage", "file='pag_logo.pag' width='150' playcount='-1'"}, {"valign", "center"}, {"margin", "8"}});
    ui::Attach(p41, p46);

    ui::Attach(p23, p41);

    auto* p47 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p48 = ui::Create<ui::Label>(pWindow, {{"width", "150"}, {"height", "auto"}, {"text", "GIF Animation"}, {"text_align", "center"}, {"margin", "8"}});
    ui::Attach(p47, p48);

    auto* p49 = ui::Create<ui::Label>(pWindow, {{"width", "150"}, {"height", "auto"}, {"text", "APNG Animation"}, {"text_align", "center"}, {"margin", "8"}});
    ui::Attach(p47, p49);

    auto* p50 = ui::Create<ui::Label>(pWindow, {{"width", "150"}, {"height", "auto"}, {"text", "WEBP Animation"}, {"text_align", "center"}, {"margin", "8"}});
    ui::Attach(p47, p50);

    auto* p51 = ui::Create<ui::Label>(pWindow, {{"width", "150"}, {"height", "auto"}, {"text", "Lottie JSON Animation"}, {"text_align", "center"}, {"margin", "8"}});
    ui::Attach(p47, p51);

    auto* p52 = ui::Create<ui::Label>(pWindow, {{"width", "150"}, {"height", "auto"}, {"text", "PAG Animation"}, {"text_align", "center"}, {"margin", "8"}});
    ui::Attach(p47, p52);

    ui::Attach(p23, p47);

    auto* p53 = ui::Create<ui::HBox>(pWindow, {});
    auto* p54 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "256"}, {"margin", "20, 10, 0, 0"}});
    auto* p55 = ui::Create<ui::VBox>(pWindow, {{"width", "200"}, {"height", "220"}});
    auto* p56 = ui::Create<ui::Control>(pWindow, {{"width", "100%"}, {"height", "100%"}, {"bkimage", "file='fan.gif' width='190' height='190' auto_play='false' play_count='-1' valign='center' halign='center'"}});
    ui::Attach(p55, p56);

    auto* p57 = ui::Create<ui::Label>(pWindow, {{"width", "100%"}, {"height", "20"}, {"text", "Play animation on mouse over image"}, {"text_align", "center"}, {"margin", "8"}});
    ui::Attach(p55, p57);

    ui::Attach(p54, p55);

    ui::Attach(p53, p54);

    ui::Attach(p23, p53);

    ui::Attach(p22, p23);

    auto* p58 = ui::Create<ui::VBox>(pWindow, {{"name", "main_view_page_01"}});
    auto* p59 = ui::Create<ui::Label>(pWindow, {{"text", "Page 2: Test supported image properties with standard controls (SVG supports vector scaling)"}});
    ui::Attach(p58, p59);

    pWindow->AddClass("page_image2_image", " width=\"128\" height=\"100\" border_size=\"1\" border_color=\"green\"");
    pWindow->AddClass("page_image2_text", " width=\"128\" height=\"auto\" multi_line=\"true\" replace_newline=\"true\" text_align=\"left,vcenter\"");
    pWindow->AddClass("page_image2_case", " width=\"128\" height=\"auto\" multi_line=\"true\" replace_newline=\"true\" text_align=\"hcenter,vcenter\"");
    auto* p60 = ui::Create<ui::VScrollBox>(pWindow, {{"vscrollbar", "true"}, {"hscrollbar", "false"}, {"bkcolor", "white"}});
    auto* p61 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "Test 1: Original (48x48), control auto size, no stretch; tests the effect of dpi_scale on all properties"}, {"width", "100%"}, {"margin", "0,4,0,0"}, {"font", "system_bold_14"}});
    ui::Attach(p60, p61);

    auto* p62 = ui::Create<ui::HFlowScrollBox>(pWindow, {{"width", "auto"}, {"height", "auto"}, {"child_margin_x", "2"}, {"child_margin_y", "2"}, {"padding", "2,2,2,2"}});
    auto* p63 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p64 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' dpi_scale='false' halign='center' valign='center'"}});
    ui::Attach(p63, p64);

    auto* p65 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (1.1)"}});
    ui::Attach(p63, p65);

    auto* p66 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dpi_scale='false' halign='center' valign='center'"}});
    ui::Attach(p63, p66);

    ui::Attach(p62, p63);

    auto* p67 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p68 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' dpi_scale='true' halign='center' valign='center'"}});
    ui::Attach(p67, p68);

    auto* p69 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (1.2)"}});
    ui::Attach(p67, p69);

    auto* p70 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dpi_scale='true' halign='center' valign='center'"}});
    ui::Attach(p67, p70);

    ui::Attach(p62, p67);

    auto* p71 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p72 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.svg' dpi_scale='false'"}});
    ui::Attach(p71, p72);

    auto* p73 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (1.3)"}});
    ui::Attach(p71, p73);

    auto* p74 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dpi_scale='false'"}});
    ui::Attach(p71, p74);

    ui::Attach(p62, p71);

    auto* p75 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p76 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.svg' dpi_scale='true'"}});
    ui::Attach(p75, p76);

    auto* p77 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (1.4)"}});
    ui::Attach(p75, p77);

    auto* p78 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dpi_scale='true'"}});
    ui::Attach(p75, p78);

    ui::Attach(p62, p75);

    auto* p79 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p80 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.svg' width='56' height='40' dpi_scale='false'"}});
    ui::Attach(p79, p80);

    auto* p81 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (1.5)"}});
    ui::Attach(p79, p81);

    auto* p82 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dpi_scale='false'\\nwidth='56' height='40'"}});
    ui::Attach(p79, p82);

    ui::Attach(p62, p79);

    auto* p83 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p84 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.svg' width='56' height='40' dpi_scale='true'"}});
    ui::Attach(p83, p84);

    auto* p85 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (1.6)"}});
    ui::Attach(p83, p85);

    auto* p86 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dpi_scale='true'\\nwidth='56' height='40'"}});
    ui::Attach(p83, p86);

    ui::Attach(p62, p83);

    auto* p87 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p88 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.svg' width='56' dpi_scale='false'"}});
    ui::Attach(p87, p88);

    auto* p89 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (1.7)"}});
    ui::Attach(p87, p89);

    auto* p90 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dpi_scale='false'\\nwidth='56'"}});
    ui::Attach(p87, p90);

    ui::Attach(p62, p87);

    auto* p91 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p92 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.svg' width='56' dpi_scale='true'"}});
    ui::Attach(p91, p92);

    auto* p93 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (1.8)"}});
    ui::Attach(p91, p93);

    auto* p94 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dpi_scale='true'\\nwidth='56'"}});
    ui::Attach(p91, p94);

    ui::Attach(p62, p91);

    auto* p95 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p96 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.svg' height='40' dpi_scale='false'"}});
    ui::Attach(p95, p96);

    auto* p97 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (1.9)"}});
    ui::Attach(p95, p97);

    auto* p98 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dpi_scale='false'\\nheight='40'"}});
    ui::Attach(p95, p98);

    ui::Attach(p62, p95);

    auto* p99 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p100 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.svg' width='40' dpi_scale='true'"}});
    ui::Attach(p99, p100);

    auto* p101 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (1.10)"}});
    ui::Attach(p99, p101);

    auto* p102 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dpi_scale='true'\\nheight='40'"}});
    ui::Attach(p99, p102);

    ui::Attach(p62, p99);

    auto* p103 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p104 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.svg' width='150%' height='200%' dpi_scale='false'"}});
    ui::Attach(p103, p104);

    auto* p105 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (1.11)"}});
    ui::Attach(p103, p105);

    auto* p106 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dpi_scale='false'\\nwidth='150%' \\nheight='200%'"}});
    ui::Attach(p103, p106);

    ui::Attach(p62, p103);

    auto* p107 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p108 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.svg' width='150%' height='200%' dpi_scale='true'"}});
    ui::Attach(p107, p108);

    auto* p109 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (1.12)"}});
    ui::Attach(p107, p109);

    auto* p110 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dpi_scale='true'\\nwidth='150%' \\nheight='200%'"}});
    ui::Attach(p107, p110);

    ui::Attach(p62, p107);

    auto* p111 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p112 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.svg' width='150%' dpi_scale='false'"}});
    ui::Attach(p111, p112);

    auto* p113 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (1.13)"}});
    ui::Attach(p111, p113);

    auto* p114 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dpi_scale='false'\\nwidth='150%'"}});
    ui::Attach(p111, p114);

    ui::Attach(p62, p111);

    auto* p115 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p116 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.svg' width='150%' dpi_scale='true'"}});
    ui::Attach(p115, p116);

    auto* p117 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (1.14)"}});
    ui::Attach(p115, p117);

    auto* p118 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dpi_scale='true'\\nwidth='150%'"}});
    ui::Attach(p115, p118);

    ui::Attach(p62, p115);

    auto* p119 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p120 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.svg' height='200%' dpi_scale='false'"}});
    ui::Attach(p119, p120);

    auto* p121 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (1.15)"}});
    ui::Attach(p119, p121);

    auto* p122 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dpi_scale='false'\\nheight='200%'"}});
    ui::Attach(p119, p122);

    ui::Attach(p62, p119);

    auto* p123 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p124 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.svg' height='200%' dpi_scale='true'"}});
    ui::Attach(p123, p124);

    auto* p125 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (1.16)"}});
    ui::Attach(p123, p125);

    auto* p126 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dpi_scale='true'\\nheight='200%'"}});
    ui::Attach(p123, p126);

    ui::Attach(p62, p123);

    auto* p127 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p128 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.svg' src='5,6,42,36' dpi_scale='false'"}});
    ui::Attach(p127, p128);

    auto* p129 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (1.17)"}});
    ui::Attach(p127, p129);

    auto* p130 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dpi_scale='false'\\nsrc='5,6,42,36'"}});
    ui::Attach(p127, p130);

    ui::Attach(p62, p127);

    auto* p131 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p132 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.svg' src='5,6,42,36' dpi_scale='true'"}});
    ui::Attach(p131, p132);

    auto* p133 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (1.18)"}});
    ui::Attach(p131, p133);

    auto* p134 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dpi_scale='true'\\nsrc='5,6,42,36'"}});
    ui::Attach(p131, p134);

    ui::Attach(p62, p131);

    auto* p135 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p136 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.svg' dest='5,6,55,46' dest_scale='false'"}});
    ui::Attach(p135, p136);

    auto* p137 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (1.19)"}});
    ui::Attach(p135, p137);

    auto* p138 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dest_scale='false'\\ndest='5,6,55,46'"}});
    ui::Attach(p135, p138);

    ui::Attach(p62, p135);

    auto* p139 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p140 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.svg' dest='5,6,55,46' dest_scale='true'"}});
    ui::Attach(p139, p140);

    auto* p141 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (1.20)"}});
    ui::Attach(p139, p141);

    auto* p142 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dest_scale='true'\\ndest='5,6,55,46'"}});
    ui::Attach(p139, p142);

    ui::Attach(p62, p139);

    auto* p143 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p144 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.svg' dest='5,6,45,46' dest_scale='false' src='5,6,42,36' dpi_scale='false'"}});
    ui::Attach(p143, p144);

    auto* p145 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (1.21)"}});
    ui::Attach(p143, p145);

    auto* p146 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dest_scale='false'\\ndest='5,6,45,46' \\nsrc='5,6,42,36' \\ndpi_scale='false'"}});
    ui::Attach(p143, p146);

    ui::Attach(p62, p143);

    auto* p147 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p148 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.svg' dest='5,6,45,46' dest_scale='true' src='5,6,42,36' dpi_scale='true'"}});
    ui::Attach(p147, p148);

    auto* p149 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (1.22)"}});
    ui::Attach(p147, p149);

    auto* p150 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dest_scale='true'\\ndest='5,6,45,46' \\nsrc='5,6,42,36' \\ndpi_scale='true'"}});
    ui::Attach(p147, p150);

    ui::Attach(p62, p147);

    auto* p151 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p152 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.svg' dest='5,6,45,46' dest_scale='false' src='2,2,48,48' dpi_scale='false'"}});
    ui::Attach(p151, p152);

    auto* p153 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (1.23)"}});
    ui::Attach(p151, p153);

    auto* p154 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dest_scale='false'\\ndest='5,6,45,46' \\nsrc='2,2,48,48' \\ndpi_scale='false'"}});
    ui::Attach(p151, p154);

    ui::Attach(p62, p151);

    auto* p155 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p156 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.svg' dest='5,6,45,46' dest_scale='true' src='2,2,48,48' dpi_scale='true'"}});
    ui::Attach(p155, p156);

    auto* p157 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (1.24)"}});
    ui::Attach(p155, p157);

    auto* p158 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dest_scale='true'\\ndest='5,6,45,46' \\nsrc='2,2,48,48' \\ndpi_scale='true'"}});
    ui::Attach(p155, p158);

    ui::Attach(p62, p155);

    ui::Attach(p60, p62);

    auto* p159 = ui::Create<ui::Line>(pWindow, {{"class", "splitline_hor_level1"}, {"margin", "4,4,4,4"}});
    ui::Attach(p60, p159);

    auto* p160 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "Test 2: Original (48x48), combination test of various use cases"}, {"width", "100%"}, {"margin", "0,4,0,0"}, {"font", "system_bold_14"}});
    ui::Attach(p60, p160);

    auto* p161 = ui::Create<ui::HFlowScrollBox>(pWindow, {{"width", "auto"}, {"height", "auto"}, {"child_margin_x", "2"}, {"child_margin_y", "2"}, {"padding", "2,2,2,2"}});
    auto* p162 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p163 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' dpi_scale='false' halign='center' valign='center'"}});
    ui::Attach(p162, p163);

    auto* p164 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.1)"}});
    ui::Attach(p162, p164);

    auto* p165 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dpi_scale='false' halign='center' valign='center'  no stretch"}, {"text_align", "center,vcenter"}});
    ui::Attach(p162, p165);

    ui::Attach(p161, p162);

    auto* p166 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p167 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' dpi_scale='true' halign='center' valign='center'"}});
    ui::Attach(p166, p167);

    auto* p168 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.2)"}});
    ui::Attach(p166, p168);

    auto* p169 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dpi_scale='true'  halign='center' valign='center' no stretch"}, {"text_align", "center,vcenter"}});
    ui::Attach(p166, p169);

    ui::Attach(p161, p166);

    auto* p170 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p171 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' dpi_scale='true'"}});
    ui::Attach(p170, p171);

    auto* p172 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.3)"}, {"text_align", "center,vcenter"}});
    ui::Attach(p170, p172);

    auto* p173 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "dpi_scale='true'  original (stretch)"}, {"text_align", "center,vcenter"}});
    ui::Attach(p170, p173);

    ui::Attach(p161, p170);

    auto* p174 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p175 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34' halign='center' valign='center'"}});
    ui::Attach(p174, p175);

    auto* p176 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.4)"}});
    ui::Attach(p174, p176);

    auto* p177 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "src='12,10,36,34'  halign='center'  valign='center' no stretch"}});
    ui::Attach(p174, p177);

    ui::Attach(p161, p174);

    auto* p178 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p179 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34'"}});
    ui::Attach(p178, p179);

    auto* p180 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.5)"}});
    ui::Attach(p178, p180);

    auto* p181 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "src='12,10,36,34' stretch"}});
    ui::Attach(p178, p181);

    ui::Attach(p161, p178);

    auto* p182 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p183 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34' dpi_scale='false' halign='center' valign='center'"}});
    ui::Attach(p182, p183);

    auto* p184 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.6)"}});
    ui::Attach(p182, p184);

    auto* p185 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "src='12,10,36,34' \\ndpi_scale='false' \\nhalign='center' \\nvalign='center'"}});
    ui::Attach(p182, p185);

    ui::Attach(p161, p182);

    auto* p186 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p187 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34' dpi_scale='false'"}});
    ui::Attach(p186, p187);

    auto* p188 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.7)"}});
    ui::Attach(p186, p188);

    auto* p189 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "src='12,10,36,34'  dpi_scale='false'  stretch"}});
    ui::Attach(p186, p189);

    ui::Attach(p161, p186);

    auto* p190 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p191 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34' margin='10,10,10,10'"}});
    ui::Attach(p190, p191);

    auto* p192 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.8)"}});
    ui::Attach(p190, p192);

    auto* p193 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "src='12,10,36,34'  margin='10,10,10,10'  stretch"}});
    ui::Attach(p190, p193);

    ui::Attach(p161, p190);

    auto* p194 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p195 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34' margin='10,0,0,0'"}});
    ui::Attach(p194, p195);

    auto* p196 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.9)"}});
    ui::Attach(p194, p196);

    auto* p197 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "src='12,10,36,34'  margin='10,0,0,0'  stretch"}});
    ui::Attach(p194, p197);

    ui::Attach(p161, p194);

    auto* p198 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p199 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34' margin='0,10,0,0'"}});
    ui::Attach(p198, p199);

    auto* p200 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.10)"}});
    ui::Attach(p198, p200);

    auto* p201 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "src='12,10,36,34'  margin='0,10,0,0'  stretch"}});
    ui::Attach(p198, p201);

    ui::Attach(p161, p198);

    auto* p202 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p203 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34' margin='0,0,10,0'"}});
    ui::Attach(p202, p203);

    auto* p204 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.11)"}});
    ui::Attach(p202, p204);

    auto* p205 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "src='12,10,36,34'  margin='0,0,10,0'  stretch"}});
    ui::Attach(p202, p205);

    ui::Attach(p161, p202);

    auto* p206 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p207 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34' margin='0,0,0,10'"}});
    ui::Attach(p206, p207);

    auto* p208 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.12)"}});
    ui::Attach(p206, p208);

    auto* p209 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "src='12,10,36,34'  margin='0,0,0,10'  stretch"}});
    ui::Attach(p206, p209);

    ui::Attach(p161, p206);

    auto* p210 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p211 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34' halign='left' valign='top'"}});
    ui::Attach(p210, p211);

    auto* p212 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.13)"}});
    ui::Attach(p210, p212);

    auto* p213 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "src='12,10,36,34'  halign='left'  valign='top'  no stretch"}});
    ui::Attach(p210, p213);

    ui::Attach(p161, p210);

    auto* p214 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p215 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34' halign='center' valign='center'"}});
    ui::Attach(p214, p215);

    auto* p216 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.14)"}});
    ui::Attach(p214, p216);

    auto* p217 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "src='12,10,36,34'  halign='center'  valign='center  no stretch'"}});
    ui::Attach(p214, p217);

    ui::Attach(p161, p214);

    auto* p218 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p219 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34' halign='right' valign='bottom'"}});
    ui::Attach(p218, p219);

    auto* p220 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.15)"}});
    ui::Attach(p218, p220);

    auto* p221 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "src='12,10,36,34'  halign='right'  valign='bottom'  no stretch"}});
    ui::Attach(p218, p221);

    ui::Attach(p161, p218);

    auto* p222 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p223 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34' dest='12,12,48,48'"}});
    ui::Attach(p222, p223);

    auto* p224 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.16)"}});
    ui::Attach(p222, p224);

    auto* p225 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "src='12,10,36,34'  dest='12,12,48,48'  stretch"}});
    ui::Attach(p222, p225);

    ui::Attach(p161, p222);

    auto* p226 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p227 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34' dest='12,12,48,48' dest_scale='false'"}});
    ui::Attach(p226, p227);

    auto* p228 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.17)"}});
    ui::Attach(p226, p228);

    auto* p229 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "src='12,10,36,34'  dest='12,12,48,48'  dest_scale='false'  stretch"}});
    ui::Attach(p226, p229);

    ui::Attach(p161, p226);

    auto* p230 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p231 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34' dest='24,24'"}});
    ui::Attach(p230, p231);

    auto* p232 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.18)"}});
    ui::Attach(p230, p232);

    auto* p233 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "src='12,10,36,34'  dest='24,24'  no stretch"}});
    ui::Attach(p230, p233);

    ui::Attach(p161, p230);

    auto* p234 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p235 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34' dest='24,24' dest_scale='false'"}});
    ui::Attach(p234, p235);

    auto* p236 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.19)"}});
    ui::Attach(p234, p236);

    auto* p237 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "src='12,10,36,34'  dest='24,24'  dest_scale='false'  no stretch"}});
    ui::Attach(p234, p237);

    ui::Attach(p161, p234);

    auto* p238 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p239 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34' dpi_scale='false' dest='24,24' dest_scale='false'"}});
    ui::Attach(p238, p239);

    auto* p240 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.20)"}});
    ui::Attach(p238, p240);

    auto* p241 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "src='12,10,36,34' dpi_scale='false'  dest='24,24'  dest_scale='false'  no stretch"}});
    ui::Attach(p238, p241);

    ui::Attach(p161, p238);

    auto* p242 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p243 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' halign='center' valign='center'"}});
    ui::Attach(p242, p243);

    auto* p244 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.21)"}});
    ui::Attach(p242, p244);

    auto* p245 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "Original (No Stretch)"}, {"text_align", "center,vcenter"}});
    ui::Attach(p242, p245);

    ui::Attach(p161, p242);

    auto* p246 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p247 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' width='96' halign='center' valign='center'"}});
    ui::Attach(p246, p247);

    auto* p248 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.22)"}});
    ui::Attach(p246, p248);

    auto* p249 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "width='96' \\nhalign='center' \\nvalign='center'"}});
    ui::Attach(p246, p249);

    ui::Attach(p161, p246);

    auto* p250 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p251 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' height='64' halign='center' valign='center'"}});
    ui::Attach(p250, p251);

    auto* p252 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.23)"}});
    ui::Attach(p250, p252);

    auto* p253 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "height='64' halign='center' valign='center'"}});
    ui::Attach(p250, p253);

    ui::Attach(p161, p250);

    auto* p254 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p255 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' width='96' height='64' halign='center' valign='center'"}});
    ui::Attach(p254, p255);

    auto* p256 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.24)"}});
    ui::Attach(p254, p256);

    auto* p257 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "width='96' \\nheight='64' halign='center' valign='center'"}});
    ui::Attach(p254, p257);

    ui::Attach(p161, p254);

    auto* p258 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p259 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' width='150' height='150' dest='0,0'"}});
    ui::Attach(p258, p259);

    auto* p260 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.25)"}});
    ui::Attach(p258, p260);

    auto* p261 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "width='150'  height='150'  dest='0,0' no stretch"}});
    ui::Attach(p258, p261);

    ui::Attach(p161, p258);

    auto* p262 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p263 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' width='150' height='150'"}});
    ui::Attach(p262, p263);

    auto* p264 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.26)"}});
    ui::Attach(p262, p264);

    auto* p265 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "width='150'  height='150'  stretch"}});
    ui::Attach(p262, p265);

    ui::Attach(p161, p262);

    auto* p266 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p267 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' width='500%' halign='center' valign='center'"}});
    ui::Attach(p266, p267);

    auto* p268 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.27)"}});
    ui::Attach(p266, p268);

    auto* p269 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "width='500%'  halign='center'  valign='center'  large image adaptive"}});
    ui::Attach(p266, p269);

    ui::Attach(p161, p266);

    auto* p270 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p271 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' height='200%' halign='center' valign='center'"}});
    ui::Attach(p270, p271);

    auto* p272 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.28)"}});
    ui::Attach(p270, p272);

    auto* p273 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "height='200%' halign='center' valign='center'"}});
    ui::Attach(p270, p273);

    ui::Attach(p161, p270);

    auto* p274 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p275 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' width='500%' height='200%' halign='center' valign='center'"}});
    ui::Attach(p274, p275);

    auto* p276 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.29)"}});
    ui::Attach(p274, p276);

    auto* p277 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "width='500%' \\nheight='200%' halign='center' valign='center'"}});
    ui::Attach(p274, p277);

    ui::Attach(p161, p274);

    auto* p278 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p279 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' width='1000%' height='800%' halign='center' valign='center'"}});
    ui::Attach(p278, p279);

    auto* p280 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.30)"}});
    ui::Attach(p278, p280);

    auto* p281 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "width='1000%' \\nheight='800%' halign='center' valign='center'"}});
    ui::Attach(p278, p281);

    ui::Attach(p161, p278);

    auto* p282 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p283 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='svg_test.svg' width='1000%' height='800%''"}});
    ui::Attach(p282, p283);

    auto* p284 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.31)"}});
    ui::Attach(p282, p284);

    auto* p285 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "width='1000%' \\nheight='800%'"}, {"text_align", "center,vcenter"}});
    ui::Attach(p282, p285);

    ui::Attach(p161, p282);

    auto* p286 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p287 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "80"}, {"height", "120"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34'"}});
    ui::Attach(p286, p287);

    auto* p288 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.32)"}});
    ui::Attach(p286, p288);

    auto* p289 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "src='12,10,36,34'  stretch"}, {"text_align", "center,vcenter"}});
    ui::Attach(p286, p289);

    ui::Attach(p161, p286);

    auto* p290 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p291 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "80"}, {"height", "120"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34' adaptive_dest_rect='true'"}});
    ui::Attach(p290, p291);

    auto* p292 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.33)"}});
    ui::Attach(p290, p292);

    auto* p293 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "src='12,10,36,34'  adaptive_dest_rect='true'  adaptive"}});
    ui::Attach(p290, p293);

    ui::Attach(p161, p290);

    auto* p294 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p295 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "80"}, {"height", "120"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34' adaptive_dest_rect='true' valign='center'"}});
    ui::Attach(p294, p295);

    auto* p296 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.34)"}});
    ui::Attach(p294, p296);

    auto* p297 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "src='12,10,36,34' \\nadaptive_dest_rect='true' \\nvalign='center'"}});
    ui::Attach(p294, p297);

    ui::Attach(p161, p294);

    auto* p298 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p299 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "80"}, {"height", "120"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34' adaptive_dest_rect='true' valign='bottom'"}});
    ui::Attach(p298, p299);

    auto* p300 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.35)"}});
    ui::Attach(p298, p300);

    auto* p301 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "src='12,10,36,34' \\nadaptive_dest_rect='true' \\nvalign='bottom'"}});
    ui::Attach(p298, p301);

    ui::Attach(p161, p298);

    auto* p302 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p303 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "160"}, {"height", "60"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34' adaptive_dest_rect='true'"}});
    ui::Attach(p302, p303);

    auto* p304 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.36)"}});
    ui::Attach(p302, p304);

    auto* p305 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"width", "160"}, {"text", "src='12,10,36,34' \\nadaptive_dest_rect='true'"}});
    ui::Attach(p302, p305);

    ui::Attach(p161, p302);

    auto* p306 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p307 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "160"}, {"height", "60"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34' adaptive_dest_rect='true' halign='center'"}});
    ui::Attach(p306, p307);

    auto* p308 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.37)"}});
    ui::Attach(p306, p308);

    auto* p309 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"width", "160"}, {"text", "src='12,10,36,34' \\nadaptive_dest_rect='true'\\nhalign='center'"}});
    ui::Attach(p306, p309);

    ui::Attach(p161, p306);

    auto* p310 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p311 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "160"}, {"height", "60"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34' adaptive_dest_rect='true' halign='right'"}});
    ui::Attach(p310, p311);

    auto* p312 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.38)"}});
    ui::Attach(p310, p312);

    auto* p313 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"width", "160"}, {"text", "src='12,10,36,34' \\nadaptive_dest_rect='true'\\nhalign='right'"}});
    ui::Attach(p310, p313);

    ui::Attach(p161, p310);

    auto* p314 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p315 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "40"}, {"height", "10"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34' adaptive_dest_rect='true' halign='left'"}});
    ui::Attach(p314, p315);

    auto* p316 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.39)"}});
    ui::Attach(p314, p316);

    auto* p317 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "src='12,10,36,34' \\nadaptive_dest_rect='true' \\nhalign='left'"}});
    ui::Attach(p314, p317);

    ui::Attach(p161, p314);

    auto* p318 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p319 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "40"}, {"height", "10"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34' adaptive_dest_rect='true' halign='center'"}});
    ui::Attach(p318, p319);

    auto* p320 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.40)"}});
    ui::Attach(p318, p320);

    auto* p321 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "src='12,10,36,34' \\nadaptive_dest_rect='true' \\nhalign='center'"}});
    ui::Attach(p318, p321);

    ui::Attach(p161, p318);

    auto* p322 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p323 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "40"}, {"height", "10"}, {"bkimage", "file='svg_test.svg' src='12,10,36,34' adaptive_dest_rect='true' halign='right'"}});
    ui::Attach(p322, p323);

    auto* p324 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (2.41)"}});
    ui::Attach(p322, p324);

    auto* p325 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "src='12,10,36,34' \\nadaptive_dest_rect='true' \\nhalign='right'"}});
    ui::Attach(p322, p325);

    ui::Attach(p161, p322);

    ui::Attach(p60, p161);

    auto* p326 = ui::Create<ui::Line>(pWindow, {{"class", "splitline_hor_level1"}, {"margin", "4,4,4,4"}});
    ui::Attach(p60, p326);

    auto* p327 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "Test 3: Nine-patch drawing, combination test of other use cases"}, {"width", "100%"}, {"margin", "0,4,0,0"}, {"font", "system_bold_14"}});
    ui::Attach(p60, p327);

    auto* p328 = ui::Create<ui::HFlowScrollBox>(pWindow, {{"width", "auto"}, {"height", "auto"}, {"child_margin_x", "2"}, {"child_margin_y", "2"}, {"padding", "2,2,2,2"}});
    auto* p329 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p330 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='nine-patch.png' dpi_scale='false'"}});
    ui::Attach(p329, p330);

    auto* p331 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (3.1)"}});
    ui::Attach(p329, p331);

    auto* p332 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "original: 100x100  dpi_scale='false'"}});
    ui::Attach(p329, p332);

    ui::Attach(p328, p329);

    auto* p333 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p334 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='nine-patch.png' dpi_scale='true'"}});
    ui::Attach(p333, p334);

    auto* p335 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (3.2)"}});
    ui::Attach(p333, p335);

    auto* p336 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "original: 100x100  dpi_scale='true'"}});
    ui::Attach(p333, p336);

    ui::Attach(p328, p333);

    auto* p337 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p338 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='nine-patch.png' dpi_scale='false' dest='0,0,180,180' dest_scale='false' corner='13,13,13,13'"}});
    ui::Attach(p337, p338);

    auto* p339 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (3.3)"}});
    ui::Attach(p337, p339);

    auto* p340 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "Nine-patch drawing \\ndpi_scale='false' \\ndest='0,0,180,180' \\ndest_scale='false' \\ncorner='13,13,13,13'"}, {"width", "auto"}});
    ui::Attach(p337, p340);

    ui::Attach(p328, p337);

    auto* p341 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p342 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "120"}, {"height", "120"}, {"bkimage", "file='libre-gui-frame.svg' dpi_scale='false' halign='center' valign='center'"}});
    ui::Attach(p341, p342);

    auto* p343 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (3.4)"}});
    ui::Attach(p341, p343);

    auto* p344 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "original (14*14)  no stretch  dpi_scale='false'  halign='center'  valign='center'"}});
    ui::Attach(p341, p344);

    ui::Attach(p328, p341);

    auto* p345 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p346 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "120"}, {"height", "120"}, {"bkimage", "file='libre-gui-frame.svg' dpi_scale='false' corner='6,6,6,6'"}});
    ui::Attach(p345, p346);

    auto* p347 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (3.5)"}});
    ui::Attach(p345, p347);

    auto* p348 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "Nine-patch drawing  original dpi_scale='false'  corner='6,6,6,6'"}});
    ui::Attach(p345, p348);

    ui::Attach(p328, p345);

    auto* p349 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p350 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "120"}, {"height", "120"}, {"bkimage", "file='libre-gui-frame.svg' dpi_scale='true' halign='center' valign='center'"}});
    ui::Attach(p349, p350);

    auto* p351 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (3.6)"}});
    ui::Attach(p349, p351);

    auto* p352 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "original (14*14)  DPI adaptive  dpi_scale='true'  halign='center'  valign='center'"}});
    ui::Attach(p349, p352);

    ui::Attach(p328, p349);

    auto* p353 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p354 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "120"}, {"height", "120"}, {"bkimage", "file='libre-gui-frame.svg' dpi_scale='true' corner='6,6,6,6'"}});
    ui::Attach(p353, p354);

    auto* p355 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (3.7)"}});
    ui::Attach(p353, p355);

    auto* p356 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "Nine-patch drawing  DPI adaptive dpi_scale='true'  corner='6,6,6,6'"}});
    ui::Attach(p353, p356);

    ui::Attach(p328, p353);

    auto* p357 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p358 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "120"}, {"height", "120"}, {"bkimage", "file='libre-gui-frame.svg' dpi_scale='false' width='28' corner='12,12,12,12'"}});
    ui::Attach(p357, p358);

    auto* p359 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (3.8)"}});
    ui::Attach(p357, p359);

    auto* p360 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "Nine-patch drawing  manual zoom dpi_scale='false'  width='28'  corner='12,12,12,12'"}});
    ui::Attach(p357, p360);

    ui::Attach(p328, p357);

    auto* p361 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p362 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "120"}, {"height", "120"}, {"bkimage", "file='libre-gui-frame.svg' dpi_scale='false'"}});
    ui::Attach(p361, p362);

    auto* p363 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (3.9)"}});
    ui::Attach(p361, p363);

    auto* p364 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "original (14*14) stretch dpi_scale='false' "}});
    ui::Attach(p361, p364);

    ui::Attach(p328, p361);

    auto* p365 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p366 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"bkimage", "file='autumn.png' fade='64'"}});
    ui::Attach(p365, p366);

    auto* p367 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (3.10)"}});
    ui::Attach(p365, p367);

    auto* p368 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "fade='64'  opacity 64"}, {"text_align", "center,vcenter"}});
    ui::Attach(p365, p368);

    ui::Attach(p328, p365);

    ui::Attach(p60, p328);

    auto* p369 = ui::Create<ui::Line>(pWindow, {{"class", "splitline_hor_level1"}, {"margin", "4,4,4,4"}});
    ui::Attach(p60, p369);

    auto* p370 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_text"}, {"text", "Test 4: Nine-patch drawing, stretch and tile test of the middle part"}, {"width", "100%"}, {"margin", "0,4,0,0"}, {"font", "system_bold_14"}});
    ui::Attach(p60, p370);

    auto* p371 = ui::Create<ui::HFlowScrollBox>(pWindow, {{"width", "auto"}, {"height", "auto"}, {"child_margin_x", "2"}, {"child_margin_y", "2"}, {"padding", "2,2,2,2"}, {"bkcolor", "Cornsilk"}});
    auto* p372 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p373 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p374 = ui::Create<ui::VBox>(pWindow, {{"width", "212"}, {"height", "200"}});
    auto* p375 = ui::Create<ui::HBox>(pWindow, {});
    auto* p376 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.svg' dpi_scale='false' corner='14,14,14,14'"}});
    ui::Attach(p375, p376);

    auto* p377 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"width", "5"}, {"border_color", "white"}, {"border_size", "2,0,2,0"}});
    ui::Attach(p375, p377);

    auto* p378 = ui::Create<ui::Control>(pWindow, {{"min_width", "7"}, {"width", "7"}});
    ui::Attach(p375, p378);

    ui::Attach(p374, p375);

    ui::Attach(p373, p374);

    auto* p379 = ui::Create<ui::VBox>(pWindow, {{"width", "200"}, {"height", "212"}});
    auto* p380 = ui::Create<ui::VBox>(pWindow, {});
    auto* p381 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.svg' dpi_scale='false' corner='14,14,14,14'"}});
    ui::Attach(p380, p381);

    auto* p382 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"height", "5"}, {"border_color", "white"}, {"border_size", "0,2,0,2"}});
    ui::Attach(p380, p382);

    auto* p383 = ui::Create<ui::Control>(pWindow, {{"min_height", "7"}, {"height", "7"}});
    ui::Attach(p380, p383);

    ui::Attach(p379, p380);

    ui::Attach(p373, p379);

    ui::Attach(p372, p373);

    auto* p384 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (4.1)"}, {"width", "100%"}});
    ui::Attach(p372, p384);

    auto* p385 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Nine-patch Drawing Stretch (default)"}, {"width", "100%"}});
    ui::Attach(p372, p385);

    ui::Attach(p371, p372);

    auto* p386 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p387 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p388 = ui::Create<ui::VBox>(pWindow, {{"width", "212"}, {"height", "200"}});
    auto* p389 = ui::Create<ui::HBox>(pWindow, {});
    auto* p390 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.svg' dpi_scale='false' corner='14,14,14,14' xtiled='true' full_xtiled='true'"}});
    ui::Attach(p389, p390);

    auto* p391 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"width", "5"}, {"border_color", "white"}, {"border_size", "2,0,2,0"}});
    ui::Attach(p389, p391);

    auto* p392 = ui::Create<ui::Control>(pWindow, {{"min_width", "7"}, {"width", "7"}});
    ui::Attach(p389, p392);

    ui::Attach(p388, p389);

    ui::Attach(p387, p388);

    auto* p393 = ui::Create<ui::VBox>(pWindow, {{"width", "200"}, {"height", "212"}});
    auto* p394 = ui::Create<ui::VBox>(pWindow, {});
    auto* p395 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.svg' dpi_scale='false' corner='14,14,14,14' xtiled='true' full_xtiled='true'"}});
    ui::Attach(p394, p395);

    auto* p396 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"height", "5"}, {"border_color", "white"}, {"border_size", "0,2,0,2"}});
    ui::Attach(p394, p396);

    auto* p397 = ui::Create<ui::Control>(pWindow, {{"min_height", "7"}, {"height", "7"}});
    ui::Attach(p394, p397);

    ui::Attach(p393, p394);

    ui::Attach(p387, p393);

    ui::Attach(p386, p387);

    auto* p398 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (4.2)"}, {"width", "100%"}});
    ui::Attach(p386, p398);

    auto* p399 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Nine-patch tiling xtiled='true'  full_xtiled='true'"}, {"width", "100%"}});
    ui::Attach(p386, p399);

    ui::Attach(p371, p386);

    auto* p400 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p401 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p402 = ui::Create<ui::VBox>(pWindow, {{"width", "212"}, {"height", "200"}});
    auto* p403 = ui::Create<ui::HBox>(pWindow, {});
    auto* p404 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.svg' dpi_scale='false' corner='14,14,14,14' xtiled='true' full_xtiled='false'"}});
    ui::Attach(p403, p404);

    auto* p405 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"width", "5"}, {"border_color", "white"}, {"border_size", "2,0,2,0"}});
    ui::Attach(p403, p405);

    auto* p406 = ui::Create<ui::Control>(pWindow, {{"min_width", "7"}, {"width", "7"}});
    ui::Attach(p403, p406);

    ui::Attach(p402, p403);

    ui::Attach(p401, p402);

    auto* p407 = ui::Create<ui::VBox>(pWindow, {{"width", "200"}, {"height", "212"}});
    auto* p408 = ui::Create<ui::VBox>(pWindow, {});
    auto* p409 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.svg' dpi_scale='false' corner='14,14,14,14' xtiled='true' full_xtiled='false'"}});
    ui::Attach(p408, p409);

    auto* p410 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"height", "5"}, {"border_color", "white"}, {"border_size", "0,2,0,2"}});
    ui::Attach(p408, p410);

    auto* p411 = ui::Create<ui::Control>(pWindow, {{"min_height", "7"}, {"height", "7"}});
    ui::Attach(p408, p411);

    ui::Attach(p407, p408);

    ui::Attach(p401, p407);

    ui::Attach(p400, p401);

    auto* p412 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (4.3)"}, {"width", "100%"}});
    ui::Attach(p400, p412);

    auto* p413 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Nine-patch tiling xtiled='true'  full_xtiled='false'"}, {"width", "100%"}});
    ui::Attach(p400, p413);

    ui::Attach(p371, p400);

    auto* p414 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p415 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p416 = ui::Create<ui::VBox>(pWindow, {{"width", "212"}, {"height", "200"}});
    auto* p417 = ui::Create<ui::HBox>(pWindow, {});
    auto* p418 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.svg' dpi_scale='false' corner='14,14,14,14' ytiled='true' full_ytiled='true'"}});
    ui::Attach(p417, p418);

    auto* p419 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"width", "5"}, {"border_color", "white"}, {"border_size", "2,0,2,0"}});
    ui::Attach(p417, p419);

    auto* p420 = ui::Create<ui::Control>(pWindow, {{"min_width", "7"}, {"width", "7"}});
    ui::Attach(p417, p420);

    ui::Attach(p416, p417);

    ui::Attach(p415, p416);

    auto* p421 = ui::Create<ui::VBox>(pWindow, {{"width", "200"}, {"height", "212"}});
    auto* p422 = ui::Create<ui::VBox>(pWindow, {});
    auto* p423 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.svg' dpi_scale='false' corner='14,14,14,14' ytiled='true' full_ytiled='true'"}});
    ui::Attach(p422, p423);

    auto* p424 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"height", "5"}, {"border_color", "white"}, {"border_size", "0,2,0,2"}});
    ui::Attach(p422, p424);

    auto* p425 = ui::Create<ui::Control>(pWindow, {{"min_height", "7"}, {"height", "7"}});
    ui::Attach(p422, p425);

    ui::Attach(p421, p422);

    ui::Attach(p415, p421);

    ui::Attach(p414, p415);

    auto* p426 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (4.4)"}, {"width", "100%"}});
    ui::Attach(p414, p426);

    auto* p427 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Nine-patch tiling ytiled='true'  full_ytiled='true'"}, {"width", "100%"}});
    ui::Attach(p414, p427);

    ui::Attach(p371, p414);

    auto* p428 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p429 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p430 = ui::Create<ui::VBox>(pWindow, {{"width", "212"}, {"height", "200"}});
    auto* p431 = ui::Create<ui::HBox>(pWindow, {});
    auto* p432 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.svg' dpi_scale='false' corner='14,14,14,14' ytiled='true' full_ytiled='false'"}});
    ui::Attach(p431, p432);

    auto* p433 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"width", "5"}, {"border_color", "white"}, {"border_size", "2,0,2,0"}});
    ui::Attach(p431, p433);

    auto* p434 = ui::Create<ui::Control>(pWindow, {{"min_width", "7"}, {"width", "7"}});
    ui::Attach(p431, p434);

    ui::Attach(p430, p431);

    ui::Attach(p429, p430);

    auto* p435 = ui::Create<ui::VBox>(pWindow, {{"width", "200"}, {"height", "212"}});
    auto* p436 = ui::Create<ui::VBox>(pWindow, {});
    auto* p437 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.svg' dpi_scale='false' corner='14,14,14,14' ytiled='true' full_ytiled='false'"}});
    ui::Attach(p436, p437);

    auto* p438 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"height", "5"}, {"border_color", "white"}, {"border_size", "0,2,0,2"}});
    ui::Attach(p436, p438);

    auto* p439 = ui::Create<ui::Control>(pWindow, {{"min_height", "7"}, {"height", "7"}});
    ui::Attach(p436, p439);

    ui::Attach(p435, p436);

    ui::Attach(p429, p435);

    ui::Attach(p428, p429);

    auto* p440 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (4.5)"}, {"width", "100%"}});
    ui::Attach(p428, p440);

    auto* p441 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Nine-patch tiling ytiled='true'  full_ytiled='false'"}, {"width", "100%"}});
    ui::Attach(p428, p441);

    ui::Attach(p371, p428);

    auto* p442 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p443 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p444 = ui::Create<ui::VBox>(pWindow, {{"width", "212"}, {"height", "200"}});
    auto* p445 = ui::Create<ui::HBox>(pWindow, {});
    auto* p446 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.svg' dpi_scale='false' corner='14,14,14,14' xtiled='true' ytiled='true' full_xtiled='true' full_ytiled='true'"}});
    ui::Attach(p445, p446);

    auto* p447 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"width", "5"}, {"border_color", "white"}, {"border_size", "2,0,2,0"}});
    ui::Attach(p445, p447);

    auto* p448 = ui::Create<ui::Control>(pWindow, {{"min_width", "7"}, {"width", "7"}});
    ui::Attach(p445, p448);

    ui::Attach(p444, p445);

    ui::Attach(p443, p444);

    auto* p449 = ui::Create<ui::VBox>(pWindow, {{"width", "200"}, {"height", "212"}});
    auto* p450 = ui::Create<ui::VBox>(pWindow, {});
    auto* p451 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.svg' dpi_scale='false' corner='14,14,14,14' xtiled='true' ytiled='true' full_xtiled='true' full_ytiled='true'"}});
    ui::Attach(p450, p451);

    auto* p452 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"height", "5"}, {"border_color", "white"}, {"border_size", "0,2,0,2"}});
    ui::Attach(p450, p452);

    auto* p453 = ui::Create<ui::Control>(pWindow, {{"min_height", "7"}, {"height", "7"}});
    ui::Attach(p450, p453);

    ui::Attach(p449, p450);

    ui::Attach(p443, p449);

    ui::Attach(p442, p443);

    auto* p454 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (4.6)"}, {"width", "100%"}});
    ui::Attach(p442, p454);

    auto* p455 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Nine-patch tiling xtiled='true'  full_xtiled='true'  ytiled='true'  full_ytiled='true'"}, {"width", "100%"}});
    ui::Attach(p442, p455);

    ui::Attach(p371, p442);

    auto* p456 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p457 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p458 = ui::Create<ui::VBox>(pWindow, {{"width", "212"}, {"height", "200"}});
    auto* p459 = ui::Create<ui::HBox>(pWindow, {});
    auto* p460 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.svg' dpi_scale='false' corner='14,14,14,14' xtiled='true' ytiled='true' full_xtiled='false' full_ytiled='true'"}});
    ui::Attach(p459, p460);

    auto* p461 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"width", "5"}, {"border_color", "white"}, {"border_size", "2,0,2,0"}});
    ui::Attach(p459, p461);

    auto* p462 = ui::Create<ui::Control>(pWindow, {{"min_width", "7"}, {"width", "7"}});
    ui::Attach(p459, p462);

    ui::Attach(p458, p459);

    ui::Attach(p457, p458);

    auto* p463 = ui::Create<ui::VBox>(pWindow, {{"width", "200"}, {"height", "212"}});
    auto* p464 = ui::Create<ui::VBox>(pWindow, {});
    auto* p465 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.svg' dpi_scale='false' corner='14,14,14,14' xtiled='true' ytiled='true' full_xtiled='false' full_ytiled='true'"}});
    ui::Attach(p464, p465);

    auto* p466 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"height", "5"}, {"border_color", "white"}, {"border_size", "0,2,0,2"}});
    ui::Attach(p464, p466);

    auto* p467 = ui::Create<ui::Control>(pWindow, {{"min_height", "7"}, {"height", "7"}});
    ui::Attach(p464, p467);

    ui::Attach(p463, p464);

    ui::Attach(p457, p463);

    ui::Attach(p456, p457);

    auto* p468 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (4.7)"}, {"width", "100%"}});
    ui::Attach(p456, p468);

    auto* p469 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Nine-patch tiling xtiled='true'  full_xtiled='false'  ytiled='true'  full_ytiled='true'"}, {"width", "100%"}});
    ui::Attach(p456, p469);

    ui::Attach(p371, p456);

    auto* p470 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p471 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p472 = ui::Create<ui::VBox>(pWindow, {{"width", "212"}, {"height", "200"}});
    auto* p473 = ui::Create<ui::HBox>(pWindow, {});
    auto* p474 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.svg' dpi_scale='false' corner='14,14,14,14' xtiled='true' ytiled='true' full_xtiled='true' full_ytiled='false'"}});
    ui::Attach(p473, p474);

    auto* p475 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"width", "5"}, {"border_color", "white"}, {"border_size", "2,0,2,0"}});
    ui::Attach(p473, p475);

    auto* p476 = ui::Create<ui::Control>(pWindow, {{"min_width", "7"}, {"width", "7"}});
    ui::Attach(p473, p476);

    ui::Attach(p472, p473);

    ui::Attach(p471, p472);

    auto* p477 = ui::Create<ui::VBox>(pWindow, {{"width", "200"}, {"height", "212"}});
    auto* p478 = ui::Create<ui::VBox>(pWindow, {});
    auto* p479 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.svg' dpi_scale='false' corner='14,14,14,14' xtiled='true' ytiled='true' full_xtiled='true' full_ytiled='false'"}});
    ui::Attach(p478, p479);

    auto* p480 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"height", "5"}, {"border_color", "white"}, {"border_size", "0,2,0,2"}});
    ui::Attach(p478, p480);

    auto* p481 = ui::Create<ui::Control>(pWindow, {{"min_height", "7"}, {"height", "7"}});
    ui::Attach(p478, p481);

    ui::Attach(p477, p478);

    ui::Attach(p471, p477);

    ui::Attach(p470, p471);

    auto* p482 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (4.8)"}, {"width", "100%"}});
    ui::Attach(p470, p482);

    auto* p483 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Nine-patch tiling xtiled='true'  full_xtiled='true'  ytiled='true'  full_ytiled='false'"}, {"width", "100%"}});
    ui::Attach(p470, p483);

    ui::Attach(p371, p470);

    auto* p484 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p485 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p486 = ui::Create<ui::VBox>(pWindow, {{"width", "212"}, {"height", "200"}});
    auto* p487 = ui::Create<ui::HBox>(pWindow, {});
    auto* p488 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.svg' dpi_scale='false' corner='14,14,14,14' xtiled='true' ytiled='true' full_xtiled='false' full_ytiled='false'"}});
    ui::Attach(p487, p488);

    auto* p489 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"width", "5"}, {"border_color", "white"}, {"border_size", "2,0,2,0"}});
    ui::Attach(p487, p489);

    auto* p490 = ui::Create<ui::Control>(pWindow, {{"min_width", "7"}, {"width", "7"}});
    ui::Attach(p487, p490);

    ui::Attach(p486, p487);

    ui::Attach(p485, p486);

    auto* p491 = ui::Create<ui::VBox>(pWindow, {{"width", "200"}, {"height", "212"}});
    auto* p492 = ui::Create<ui::VBox>(pWindow, {});
    auto* p493 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.svg' dpi_scale='false' corner='14,14,14,14' xtiled='true' ytiled='true' full_xtiled='false' full_ytiled='false'"}});
    ui::Attach(p492, p493);

    auto* p494 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"height", "5"}, {"border_color", "white"}, {"border_size", "0,2,0,2"}});
    ui::Attach(p492, p494);

    auto* p495 = ui::Create<ui::Control>(pWindow, {{"min_height", "7"}, {"height", "7"}});
    ui::Attach(p492, p495);

    ui::Attach(p491, p492);

    ui::Attach(p485, p491);

    ui::Attach(p484, p485);

    auto* p496 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (4.9)"}, {"width", "100%"}});
    ui::Attach(p484, p496);

    auto* p497 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Nine-patch tiling xtiled='true'  full_xtiled='false'  ytiled='true'  full_ytiled='false'"}, {"width", "100%"}});
    ui::Attach(p484, p497);

    ui::Attach(p371, p484);

    auto* p498 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p499 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p500 = ui::Create<ui::VBox>(pWindow, {{"width", "212"}, {"height", "200"}});
    auto* p501 = ui::Create<ui::HBox>(pWindow, {});
    auto* p502 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.svg' dpi_scale='false' corner='14,14,14,14' xtiled='true' ytiled='true' full_xtiled='false' full_ytiled='false' tiled_margin='1'"}});
    ui::Attach(p501, p502);

    auto* p503 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"width", "5"}, {"border_color", "white"}, {"border_size", "2,0,2,0"}});
    ui::Attach(p501, p503);

    auto* p504 = ui::Create<ui::Control>(pWindow, {{"min_width", "7"}, {"width", "7"}});
    ui::Attach(p501, p504);

    ui::Attach(p500, p501);

    ui::Attach(p499, p500);

    auto* p505 = ui::Create<ui::VBox>(pWindow, {{"width", "200"}, {"height", "212"}});
    auto* p506 = ui::Create<ui::VBox>(pWindow, {});
    auto* p507 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.svg' dpi_scale='false' corner='14,14,14,14' xtiled='true' ytiled='true' full_xtiled='false' full_ytiled='false' tiled_margin='1'"}});
    ui::Attach(p506, p507);

    auto* p508 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"height", "5"}, {"border_color", "white"}, {"border_size", "0,2,0,2"}});
    ui::Attach(p506, p508);

    auto* p509 = ui::Create<ui::Control>(pWindow, {{"min_height", "7"}, {"height", "7"}});
    ui::Attach(p506, p509);

    ui::Attach(p505, p506);

    ui::Attach(p499, p505);

    ui::Attach(p498, p499);

    auto* p510 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (4.10)"}, {"width", "100%"}});
    ui::Attach(p498, p510);

    auto* p511 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Nine-patch tiling xtiled='true'  full_xtiled='false'  ytiled='true'  full_ytiled='false'  tiled_margin='1'"}, {"width", "100%"}});
    ui::Attach(p498, p511);

    ui::Attach(p371, p498);

    auto* p512 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p513 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p514 = ui::Create<ui::VBox>(pWindow, {{"width", "212"}, {"height", "200"}});
    auto* p515 = ui::Create<ui::HBox>(pWindow, {});
    auto* p516 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.svg' dpi_scale='false' corner='14,14,14,14' xtiled='true' ytiled='true' full_xtiled='false' full_ytiled='false' tiled_margin='1' tiled_padding='1,1,1,1'"}});
    ui::Attach(p515, p516);

    auto* p517 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"width", "5"}, {"border_color", "white"}, {"border_size", "2,0,2,0"}});
    ui::Attach(p515, p517);

    auto* p518 = ui::Create<ui::Control>(pWindow, {{"min_width", "7"}, {"width", "7"}});
    ui::Attach(p515, p518);

    ui::Attach(p514, p515);

    ui::Attach(p513, p514);

    auto* p519 = ui::Create<ui::VBox>(pWindow, {{"width", "200"}, {"height", "212"}});
    auto* p520 = ui::Create<ui::VBox>(pWindow, {});
    auto* p521 = ui::Create<ui::Control>(pWindow, {{"class", "page_image2_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.svg' dpi_scale='false' corner='14,14,14,14' xtiled='true' ytiled='true' full_xtiled='false' full_ytiled='false' tiled_margin='1' tiled_padding='1,1,1,1'"}});
    ui::Attach(p520, p521);

    auto* p522 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"height", "5"}, {"border_color", "white"}, {"border_size", "0,2,0,2"}});
    ui::Attach(p520, p522);

    auto* p523 = ui::Create<ui::Control>(pWindow, {{"min_height", "7"}, {"height", "7"}});
    ui::Attach(p520, p523);

    ui::Attach(p519, p520);

    ui::Attach(p513, p519);

    ui::Attach(p512, p513);

    auto* p524 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Test Case (4.11)"}, {"width", "100%"}});
    ui::Attach(p512, p524);

    auto* p525 = ui::Create<ui::Label>(pWindow, {{"class", "page_image2_case"}, {"text", "Nine-patch tiling xtiled='true'  full_xtiled='false'  ytiled='true'  full_ytiled='false'  tiled_margin='1'  tiled_padding='1,1,1,1'"}, {"width", "100%"}});
    ui::Attach(p512, p525);

    ui::Attach(p371, p512);

    ui::Attach(p60, p371);

    ui::Attach(p58, p60);

    ui::Attach(p22, p58);

    auto* p526 = ui::Create<ui::VBox>(pWindow, {{"name", "main_view_page_02"}});
    auto* p527 = ui::Create<ui::Label>(pWindow, {{"text", "Page 3: Test supported image properties with standard controls (non-SVG formats do not support vector scaling)"}});
    ui::Attach(p526, p527);

    pWindow->AddClass("page_image3_image", " width=\"128\" height=\"100\" border_size=\"1\" border_color=\"green\"");
    pWindow->AddClass("page_image3_text", " width=\"128\" height=\"auto\" multi_line=\"true\" replace_newline=\"true\" text_align=\"left,vcenter\"");
    pWindow->AddClass("page_image3_case", " width=\"128\" height=\"auto\" multi_line=\"true\" replace_newline=\"true\" text_align=\"hcenter,vcenter\"");
    auto* p528 = ui::Create<ui::VScrollBox>(pWindow, {{"vscrollbar", "true"}, {"hscrollbar", "false"}, {"bkcolor", "white"}});
    auto* p529 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "Test 1: Original (48x48), control auto size, no stretch; tests the effect of dpi_scale on all properties"}, {"width", "100%"}, {"margin", "0,4,0,0"}, {"font", "system_bold_14"}});
    ui::Attach(p528, p529);

    auto* p530 = ui::Create<ui::HFlowScrollBox>(pWindow, {{"width", "auto"}, {"height", "auto"}, {"child_margin_x", "2"}, {"child_margin_y", "2"}, {"padding", "2,2,2,2"}});
    auto* p531 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p532 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' dpi_scale='false' halign='center' valign='center'"}});
    ui::Attach(p531, p532);

    auto* p533 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (1.1)"}});
    ui::Attach(p531, p533);

    auto* p534 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dpi_scale='false' halign='center' valign='center'"}});
    ui::Attach(p531, p534);

    ui::Attach(p530, p531);

    auto* p535 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p536 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' dpi_scale='true' halign='center' valign='center'"}});
    ui::Attach(p535, p536);

    auto* p537 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (1.2)"}});
    ui::Attach(p535, p537);

    auto* p538 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dpi_scale='true' halign='center' valign='center'"}});
    ui::Attach(p535, p538);

    ui::Attach(p530, p535);

    auto* p539 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p540 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.png' dpi_scale='false'"}});
    ui::Attach(p539, p540);

    auto* p541 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (1.3)"}});
    ui::Attach(p539, p541);

    auto* p542 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dpi_scale='false'"}});
    ui::Attach(p539, p542);

    ui::Attach(p530, p539);

    auto* p543 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p544 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.png' dpi_scale='true'"}});
    ui::Attach(p543, p544);

    auto* p545 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (1.4)"}});
    ui::Attach(p543, p545);

    auto* p546 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dpi_scale='true'"}});
    ui::Attach(p543, p546);

    ui::Attach(p530, p543);

    auto* p547 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p548 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.png' width='56' height='40' dpi_scale='false'"}});
    ui::Attach(p547, p548);

    auto* p549 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (1.5)"}});
    ui::Attach(p547, p549);

    auto* p550 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dpi_scale='false'\\nwidth='56' height='40'"}});
    ui::Attach(p547, p550);

    ui::Attach(p530, p547);

    auto* p551 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p552 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.png' width='56' height='40' dpi_scale='true'"}});
    ui::Attach(p551, p552);

    auto* p553 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (1.6)"}});
    ui::Attach(p551, p553);

    auto* p554 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dpi_scale='true'\\nwidth='56' height='40'"}});
    ui::Attach(p551, p554);

    ui::Attach(p530, p551);

    auto* p555 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p556 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.png' width='56' dpi_scale='false'"}});
    ui::Attach(p555, p556);

    auto* p557 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (1.7)"}});
    ui::Attach(p555, p557);

    auto* p558 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dpi_scale='false'\\nwidth='56'"}});
    ui::Attach(p555, p558);

    ui::Attach(p530, p555);

    auto* p559 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p560 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.png' width='56' dpi_scale='true'"}});
    ui::Attach(p559, p560);

    auto* p561 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (1.8)"}});
    ui::Attach(p559, p561);

    auto* p562 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dpi_scale='true'\\nwidth='56'"}});
    ui::Attach(p559, p562);

    ui::Attach(p530, p559);

    auto* p563 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p564 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.png' height='40' dpi_scale='false'"}});
    ui::Attach(p563, p564);

    auto* p565 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (1.9)"}});
    ui::Attach(p563, p565);

    auto* p566 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dpi_scale='false'\\nheight='40'"}});
    ui::Attach(p563, p566);

    ui::Attach(p530, p563);

    auto* p567 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p568 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.png' width='40' dpi_scale='true'"}});
    ui::Attach(p567, p568);

    auto* p569 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (1.10)"}});
    ui::Attach(p567, p569);

    auto* p570 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dpi_scale='true'\\nheight='40'"}});
    ui::Attach(p567, p570);

    ui::Attach(p530, p567);

    auto* p571 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p572 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.png' width='150%' height='200%' dpi_scale='false'"}});
    ui::Attach(p571, p572);

    auto* p573 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (1.11)"}});
    ui::Attach(p571, p573);

    auto* p574 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dpi_scale='false'\\nwidth='150%' \\nheight='200%'"}});
    ui::Attach(p571, p574);

    ui::Attach(p530, p571);

    auto* p575 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p576 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.png' width='150%' height='200%' dpi_scale='true'"}});
    ui::Attach(p575, p576);

    auto* p577 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (1.12)"}});
    ui::Attach(p575, p577);

    auto* p578 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dpi_scale='true'\\nwidth='150%' \\nheight='200%'"}});
    ui::Attach(p575, p578);

    ui::Attach(p530, p575);

    auto* p579 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p580 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.png' width='150%' dpi_scale='false'"}});
    ui::Attach(p579, p580);

    auto* p581 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (1.13)"}});
    ui::Attach(p579, p581);

    auto* p582 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dpi_scale='false'\\nwidth='150%'"}});
    ui::Attach(p579, p582);

    ui::Attach(p530, p579);

    auto* p583 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p584 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.png' width='150%' dpi_scale='true'"}});
    ui::Attach(p583, p584);

    auto* p585 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (1.14)"}});
    ui::Attach(p583, p585);

    auto* p586 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dpi_scale='true'\\nwidth='150%'"}});
    ui::Attach(p583, p586);

    ui::Attach(p530, p583);

    auto* p587 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p588 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.png' height='200%' dpi_scale='false'"}});
    ui::Attach(p587, p588);

    auto* p589 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (1.15)"}});
    ui::Attach(p587, p589);

    auto* p590 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dpi_scale='false'\\nheight='200%'"}});
    ui::Attach(p587, p590);

    ui::Attach(p530, p587);

    auto* p591 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p592 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.png' height='200%' dpi_scale='true'"}});
    ui::Attach(p591, p592);

    auto* p593 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (1.16)"}});
    ui::Attach(p591, p593);

    auto* p594 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dpi_scale='true'\\nheight='200%'"}});
    ui::Attach(p591, p594);

    ui::Attach(p530, p591);

    auto* p595 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p596 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.png' src='5,6,42,36' dpi_scale='false'"}});
    ui::Attach(p595, p596);

    auto* p597 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (1.17)"}});
    ui::Attach(p595, p597);

    auto* p598 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dpi_scale='false'\\nsrc='5,6,42,36'"}});
    ui::Attach(p595, p598);

    ui::Attach(p530, p595);

    auto* p599 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p600 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.png' src='5,6,42,36' dpi_scale='true'"}});
    ui::Attach(p599, p600);

    auto* p601 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (1.18)"}});
    ui::Attach(p599, p601);

    auto* p602 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dpi_scale='true'\\nsrc='5,6,42,36'"}});
    ui::Attach(p599, p602);

    ui::Attach(p530, p599);

    auto* p603 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p604 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.png' dest='5,6,55,46' dest_scale='false'"}});
    ui::Attach(p603, p604);

    auto* p605 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (1.19)"}});
    ui::Attach(p603, p605);

    auto* p606 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dest_scale='false'\\ndest='5,6,55,46'"}});
    ui::Attach(p603, p606);

    ui::Attach(p530, p603);

    auto* p607 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p608 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.png' dest='5,6,55,46' dest_scale='true'"}});
    ui::Attach(p607, p608);

    auto* p609 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (1.20)"}});
    ui::Attach(p607, p609);

    auto* p610 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dest_scale='true'\\ndest='5,6,55,46'"}});
    ui::Attach(p607, p610);

    ui::Attach(p530, p607);

    auto* p611 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p612 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.png' dest='5,6,45,46' dest_scale='false' src='5,6,42,36' dpi_scale='false'"}});
    ui::Attach(p611, p612);

    auto* p613 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (1.21)"}});
    ui::Attach(p611, p613);

    auto* p614 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dest_scale='false'\\ndest='5,6,45,46' \\nsrc='5,6,42,36' \\ndpi_scale='false'"}});
    ui::Attach(p611, p614);

    ui::Attach(p530, p611);

    auto* p615 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p616 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.png' dest='5,6,45,46' dest_scale='true' src='5,6,42,36' dpi_scale='true'"}});
    ui::Attach(p615, p616);

    auto* p617 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (1.22)"}});
    ui::Attach(p615, p617);

    auto* p618 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dest_scale='true'\\ndest='5,6,45,46' \\nsrc='5,6,42,36' \\ndpi_scale='true'"}});
    ui::Attach(p615, p618);

    ui::Attach(p530, p615);

    auto* p619 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p620 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.png' dest='5,6,45,46' dest_scale='false' src='2,2,48,48' dpi_scale='false'"}});
    ui::Attach(p619, p620);

    auto* p621 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (1.23)"}});
    ui::Attach(p619, p621);

    auto* p622 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dest_scale='false'\\ndest='5,6,45,46' \\nsrc='2,2,48,48' \\ndpi_scale='false'"}});
    ui::Attach(p619, p622);

    ui::Attach(p530, p619);

    auto* p623 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p624 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='svg_test.png' dest='5,6,45,46' dest_scale='true' src='2,2,48,48' dpi_scale='true'"}});
    ui::Attach(p623, p624);

    auto* p625 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (1.24)"}});
    ui::Attach(p623, p625);

    auto* p626 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dest_scale='true'\\ndest='5,6,45,46' \\nsrc='2,2,48,48' \\ndpi_scale='true'"}});
    ui::Attach(p623, p626);

    ui::Attach(p530, p623);

    ui::Attach(p528, p530);

    auto* p627 = ui::Create<ui::Line>(pWindow, {{"class", "splitline_hor_level1"}, {"margin", "4,4,4,4"}});
    ui::Attach(p528, p627);

    auto* p628 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "Test 2: Original (48x48), combination test of various use cases"}, {"width", "100%"}, {"margin", "0,4,0,0"}, {"font", "system_bold_14"}});
    ui::Attach(p528, p628);

    auto* p629 = ui::Create<ui::HFlowScrollBox>(pWindow, {{"width", "auto"}, {"height", "auto"}, {"child_margin_x", "2"}, {"child_margin_y", "2"}, {"padding", "2,2,2,2"}});
    auto* p630 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p631 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' dpi_scale='false' halign='center' valign='center'"}});
    ui::Attach(p630, p631);

    auto* p632 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.1)"}});
    ui::Attach(p630, p632);

    auto* p633 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dpi_scale='false' halign='center' valign='center'  no stretch"}, {"text_align", "center,vcenter"}});
    ui::Attach(p630, p633);

    ui::Attach(p629, p630);

    auto* p634 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p635 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' dpi_scale='true' halign='center' valign='center'"}});
    ui::Attach(p634, p635);

    auto* p636 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.2)"}});
    ui::Attach(p634, p636);

    auto* p637 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dpi_scale='true'  halign='center' valign='center' no stretch"}, {"text_align", "center,vcenter"}});
    ui::Attach(p634, p637);

    ui::Attach(p629, p634);

    auto* p638 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p639 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' dpi_scale='true'"}});
    ui::Attach(p638, p639);

    auto* p640 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.3)"}, {"text_align", "center,vcenter"}});
    ui::Attach(p638, p640);

    auto* p641 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "dpi_scale='true'  original (stretch)"}, {"text_align", "center,vcenter"}});
    ui::Attach(p638, p641);

    ui::Attach(p629, p638);

    auto* p642 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p643 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' src='12,10,36,34' halign='center' valign='center'"}});
    ui::Attach(p642, p643);

    auto* p644 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.4)"}});
    ui::Attach(p642, p644);

    auto* p645 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "src='12,10,36,34'  halign='center'  valign='center' no stretch"}});
    ui::Attach(p642, p645);

    ui::Attach(p629, p642);

    auto* p646 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p647 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' src='12,10,36,34'"}});
    ui::Attach(p646, p647);

    auto* p648 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.5)"}});
    ui::Attach(p646, p648);

    auto* p649 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "src='12,10,36,34' stretch"}});
    ui::Attach(p646, p649);

    ui::Attach(p629, p646);

    auto* p650 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p651 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' src='12,10,36,34' dpi_scale='false' halign='center' valign='center'"}});
    ui::Attach(p650, p651);

    auto* p652 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.6)"}});
    ui::Attach(p650, p652);

    auto* p653 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "src='12,10,36,34' \\ndpi_scale='false' \\nhalign='center' \\nvalign='center'"}});
    ui::Attach(p650, p653);

    ui::Attach(p629, p650);

    auto* p654 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p655 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' src='12,10,36,34' dpi_scale='false'"}});
    ui::Attach(p654, p655);

    auto* p656 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.7)"}});
    ui::Attach(p654, p656);

    auto* p657 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "src='12,10,36,34'  dpi_scale='false'  stretch"}});
    ui::Attach(p654, p657);

    ui::Attach(p629, p654);

    auto* p658 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p659 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' src='12,10,36,34' margin='10,10,10,10'"}});
    ui::Attach(p658, p659);

    auto* p660 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.8)"}});
    ui::Attach(p658, p660);

    auto* p661 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "src='12,10,36,34'  margin='10,10,10,10'  stretch"}});
    ui::Attach(p658, p661);

    ui::Attach(p629, p658);

    auto* p662 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p663 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' src='12,10,36,34' margin='10,0,0,0'"}});
    ui::Attach(p662, p663);

    auto* p664 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.9)"}});
    ui::Attach(p662, p664);

    auto* p665 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "src='12,10,36,34'  margin='10,0,0,0'  stretch"}});
    ui::Attach(p662, p665);

    ui::Attach(p629, p662);

    auto* p666 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p667 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' src='12,10,36,34' margin='0,10,0,0'"}});
    ui::Attach(p666, p667);

    auto* p668 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.10)"}});
    ui::Attach(p666, p668);

    auto* p669 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "src='12,10,36,34'  margin='0,10,0,0'  stretch"}});
    ui::Attach(p666, p669);

    ui::Attach(p629, p666);

    auto* p670 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p671 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' src='12,10,36,34' margin='0,0,10,0'"}});
    ui::Attach(p670, p671);

    auto* p672 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.11)"}});
    ui::Attach(p670, p672);

    auto* p673 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "src='12,10,36,34'  margin='0,0,10,0'  stretch"}});
    ui::Attach(p670, p673);

    ui::Attach(p629, p670);

    auto* p674 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p675 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' src='12,10,36,34' margin='0,0,0,10'"}});
    ui::Attach(p674, p675);

    auto* p676 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.12)"}});
    ui::Attach(p674, p676);

    auto* p677 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "src='12,10,36,34'  margin='0,0,0,10'  stretch"}});
    ui::Attach(p674, p677);

    ui::Attach(p629, p674);

    auto* p678 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p679 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' src='12,10,36,34' halign='left' valign='top'"}});
    ui::Attach(p678, p679);

    auto* p680 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.13)"}});
    ui::Attach(p678, p680);

    auto* p681 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "src='12,10,36,34'  halign='left'  valign='top'  no stretch"}});
    ui::Attach(p678, p681);

    ui::Attach(p629, p678);

    auto* p682 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p683 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' src='12,10,36,34' halign='center' valign='center'"}});
    ui::Attach(p682, p683);

    auto* p684 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.14)"}});
    ui::Attach(p682, p684);

    auto* p685 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "src='12,10,36,34'  halign='center'  valign='center  no stretch'"}});
    ui::Attach(p682, p685);

    ui::Attach(p629, p682);

    auto* p686 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p687 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' src='12,10,36,34' halign='right' valign='bottom'"}});
    ui::Attach(p686, p687);

    auto* p688 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.15)"}});
    ui::Attach(p686, p688);

    auto* p689 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "src='12,10,36,34'  halign='right'  valign='bottom'  no stretch"}});
    ui::Attach(p686, p689);

    ui::Attach(p629, p686);

    auto* p690 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p691 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' src='12,10,36,34' dest='12,12,48,48'"}});
    ui::Attach(p690, p691);

    auto* p692 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.16)"}});
    ui::Attach(p690, p692);

    auto* p693 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "src='12,10,36,34'  dest='12,12,48,48'  stretch"}});
    ui::Attach(p690, p693);

    ui::Attach(p629, p690);

    auto* p694 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p695 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' src='12,10,36,34' dest='12,12,48,48' dest_scale='false'"}});
    ui::Attach(p694, p695);

    auto* p696 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.17)"}});
    ui::Attach(p694, p696);

    auto* p697 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "src='12,10,36,34'  dest='12,12,48,48'  dest_scale='false'  stretch"}});
    ui::Attach(p694, p697);

    ui::Attach(p629, p694);

    auto* p698 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p699 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' src='12,10,36,34' dest='24,24'"}});
    ui::Attach(p698, p699);

    auto* p700 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.18)"}});
    ui::Attach(p698, p700);

    auto* p701 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "src='12,10,36,34'  dest='24,24'  no stretch"}});
    ui::Attach(p698, p701);

    ui::Attach(p629, p698);

    auto* p702 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p703 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' src='12,10,36,34' dest='24,24' dest_scale='false'"}});
    ui::Attach(p702, p703);

    auto* p704 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.19)"}});
    ui::Attach(p702, p704);

    auto* p705 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "src='12,10,36,34'  dest='24,24'  dest_scale='false'  no stretch"}});
    ui::Attach(p702, p705);

    ui::Attach(p629, p702);

    auto* p706 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p707 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' src='12,10,36,34' dpi_scale='false' dest='24,24' dest_scale='false'"}});
    ui::Attach(p706, p707);

    auto* p708 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.20)"}});
    ui::Attach(p706, p708);

    auto* p709 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "src='12,10,36,34' dpi_scale='false'  dest='24,24'  dest_scale='false'  no stretch"}});
    ui::Attach(p706, p709);

    ui::Attach(p629, p706);

    auto* p710 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p711 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' halign='center' valign='center'"}});
    ui::Attach(p710, p711);

    auto* p712 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.21)"}});
    ui::Attach(p710, p712);

    auto* p713 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "Original (No Stretch)"}, {"text_align", "center,vcenter"}});
    ui::Attach(p710, p713);

    ui::Attach(p629, p710);

    auto* p714 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p715 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' width='96' halign='center' valign='center'"}});
    ui::Attach(p714, p715);

    auto* p716 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.22)"}});
    ui::Attach(p714, p716);

    auto* p717 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "width='96' \\nhalign='center' \\nvalign='center'"}});
    ui::Attach(p714, p717);

    ui::Attach(p629, p714);

    auto* p718 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p719 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' height='64' halign='center' valign='center'"}});
    ui::Attach(p718, p719);

    auto* p720 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.23)"}});
    ui::Attach(p718, p720);

    auto* p721 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "height='64' halign='center' valign='center'"}});
    ui::Attach(p718, p721);

    ui::Attach(p629, p718);

    auto* p722 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p723 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' width='96' height='64' halign='center' valign='center'"}});
    ui::Attach(p722, p723);

    auto* p724 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.24)"}});
    ui::Attach(p722, p724);

    auto* p725 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "width='96' \\nheight='64' halign='center' valign='center'"}});
    ui::Attach(p722, p725);

    ui::Attach(p629, p722);

    auto* p726 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p727 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' width='150' height='150' dest='0,0'"}});
    ui::Attach(p726, p727);

    auto* p728 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.25)"}});
    ui::Attach(p726, p728);

    auto* p729 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "width='150'  height='150'  dest='0,0' no stretch"}});
    ui::Attach(p726, p729);

    ui::Attach(p629, p726);

    auto* p730 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p731 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' width='150' height='150'"}});
    ui::Attach(p730, p731);

    auto* p732 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.26)"}});
    ui::Attach(p730, p732);

    auto* p733 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "width='150'  height='150'  stretch"}});
    ui::Attach(p730, p733);

    ui::Attach(p629, p730);

    auto* p734 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p735 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' width='500%' halign='center' valign='center'"}});
    ui::Attach(p734, p735);

    auto* p736 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.27)"}});
    ui::Attach(p734, p736);

    auto* p737 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "width='500%'  halign='center'  valign='center'  large image adaptive"}});
    ui::Attach(p734, p737);

    ui::Attach(p629, p734);

    auto* p738 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p739 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' height='200%' halign='center' valign='center'"}});
    ui::Attach(p738, p739);

    auto* p740 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.28)"}});
    ui::Attach(p738, p740);

    auto* p741 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "height='200%' halign='center' valign='center'"}});
    ui::Attach(p738, p741);

    ui::Attach(p629, p738);

    auto* p742 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p743 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' width='500%' height='200%' halign='center' valign='center'"}});
    ui::Attach(p742, p743);

    auto* p744 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.29)"}});
    ui::Attach(p742, p744);

    auto* p745 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "width='500%' \\nheight='200%' halign='center' valign='center'"}});
    ui::Attach(p742, p745);

    ui::Attach(p629, p742);

    auto* p746 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p747 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' width='1000%' height='800%' halign='center' valign='center'"}});
    ui::Attach(p746, p747);

    auto* p748 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.30)"}});
    ui::Attach(p746, p748);

    auto* p749 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "width='1000%' \\nheight='800%' halign='center' valign='center'"}});
    ui::Attach(p746, p749);

    ui::Attach(p629, p746);

    auto* p750 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p751 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='svg_test.png' width='1000%' height='800%''"}});
    ui::Attach(p750, p751);

    auto* p752 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.31)"}});
    ui::Attach(p750, p752);

    auto* p753 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "width='1000%' \\nheight='800%'"}, {"text_align", "center,vcenter"}});
    ui::Attach(p750, p753);

    ui::Attach(p629, p750);

    auto* p754 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p755 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "80"}, {"height", "120"}, {"bkimage", "file='svg_test.png' src='12,10,36,34'"}});
    ui::Attach(p754, p755);

    auto* p756 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.32)"}});
    ui::Attach(p754, p756);

    auto* p757 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "src='12,10,36,34'  stretch"}, {"text_align", "center,vcenter"}});
    ui::Attach(p754, p757);

    ui::Attach(p629, p754);

    auto* p758 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p759 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "80"}, {"height", "120"}, {"bkimage", "file='svg_test.png' src='12,10,36,34' adaptive_dest_rect='true'"}});
    ui::Attach(p758, p759);

    auto* p760 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.33)"}});
    ui::Attach(p758, p760);

    auto* p761 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "src='12,10,36,34'  adaptive_dest_rect='true'  adaptive"}});
    ui::Attach(p758, p761);

    ui::Attach(p629, p758);

    auto* p762 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p763 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "80"}, {"height", "120"}, {"bkimage", "file='svg_test.png' src='12,10,36,34' adaptive_dest_rect='true' valign='center'"}});
    ui::Attach(p762, p763);

    auto* p764 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.34)"}});
    ui::Attach(p762, p764);

    auto* p765 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "src='12,10,36,34' \\nadaptive_dest_rect='true' \\nvalign='center'"}});
    ui::Attach(p762, p765);

    ui::Attach(p629, p762);

    auto* p766 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p767 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "80"}, {"height", "120"}, {"bkimage", "file='svg_test.png' src='12,10,36,34' adaptive_dest_rect='true' valign='bottom'"}});
    ui::Attach(p766, p767);

    auto* p768 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.35)"}});
    ui::Attach(p766, p768);

    auto* p769 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "src='12,10,36,34' \\nadaptive_dest_rect='true' \\nvalign='bottom'"}});
    ui::Attach(p766, p769);

    ui::Attach(p629, p766);

    auto* p770 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p771 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "160"}, {"height", "60"}, {"bkimage", "file='svg_test.png' src='12,10,36,34' adaptive_dest_rect='true'"}});
    ui::Attach(p770, p771);

    auto* p772 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.36)"}});
    ui::Attach(p770, p772);

    auto* p773 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"width", "160"}, {"text", "src='12,10,36,34' \\nadaptive_dest_rect='true'"}});
    ui::Attach(p770, p773);

    ui::Attach(p629, p770);

    auto* p774 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p775 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "160"}, {"height", "60"}, {"bkimage", "file='svg_test.png' src='12,10,36,34' adaptive_dest_rect='true' halign='center'"}});
    ui::Attach(p774, p775);

    auto* p776 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.37)"}});
    ui::Attach(p774, p776);

    auto* p777 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"width", "160"}, {"text", "src='12,10,36,34' \\nadaptive_dest_rect='true'\\nhalign='center'"}});
    ui::Attach(p774, p777);

    ui::Attach(p629, p774);

    auto* p778 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p779 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "160"}, {"height", "60"}, {"bkimage", "file='svg_test.png' src='12,10,36,34' adaptive_dest_rect='true' halign='right'"}});
    ui::Attach(p778, p779);

    auto* p780 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.38)"}});
    ui::Attach(p778, p780);

    auto* p781 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"width", "160"}, {"text", "src='12,10,36,34' \\nadaptive_dest_rect='true'\\nhalign='right'"}});
    ui::Attach(p778, p781);

    ui::Attach(p629, p778);

    auto* p782 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p783 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "40"}, {"height", "10"}, {"bkimage", "file='svg_test.png' src='12,10,36,34' adaptive_dest_rect='true' halign='left'"}});
    ui::Attach(p782, p783);

    auto* p784 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.39)"}});
    ui::Attach(p782, p784);

    auto* p785 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "src='12,10,36,34' \\nadaptive_dest_rect='true' \\nhalign='left'"}});
    ui::Attach(p782, p785);

    ui::Attach(p629, p782);

    auto* p786 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p787 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "40"}, {"height", "10"}, {"bkimage", "file='svg_test.png' src='12,10,36,34' adaptive_dest_rect='true' halign='center'"}});
    ui::Attach(p786, p787);

    auto* p788 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.40)"}});
    ui::Attach(p786, p788);

    auto* p789 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "src='12,10,36,34' \\nadaptive_dest_rect='true' \\nhalign='center'"}});
    ui::Attach(p786, p789);

    ui::Attach(p629, p786);

    auto* p790 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p791 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "40"}, {"height", "10"}, {"bkimage", "file='svg_test.png' src='12,10,36,34' adaptive_dest_rect='true' halign='right'"}});
    ui::Attach(p790, p791);

    auto* p792 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (2.41)"}});
    ui::Attach(p790, p792);

    auto* p793 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "src='12,10,36,34' \\nadaptive_dest_rect='true' \\nhalign='right'"}});
    ui::Attach(p790, p793);

    ui::Attach(p629, p790);

    ui::Attach(p528, p629);

    auto* p794 = ui::Create<ui::Line>(pWindow, {{"class", "splitline_hor_level1"}, {"margin", "4,4,4,4"}});
    ui::Attach(p528, p794);

    auto* p795 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "Test 3: Nine-patch drawing, combination test of other use cases"}, {"width", "100%"}, {"margin", "0,4,0,0"}, {"font", "system_bold_14"}});
    ui::Attach(p528, p795);

    auto* p796 = ui::Create<ui::HFlowScrollBox>(pWindow, {{"width", "auto"}, {"height", "auto"}, {"child_margin_x", "2"}, {"child_margin_y", "2"}, {"padding", "2,2,2,2"}});
    auto* p797 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p798 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='nine-patch.png' dpi_scale='false'"}});
    ui::Attach(p797, p798);

    auto* p799 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (3.1)"}});
    ui::Attach(p797, p799);

    auto* p800 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "original: 100x100  dpi_scale='false'"}});
    ui::Attach(p797, p800);

    ui::Attach(p796, p797);

    auto* p801 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p802 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='nine-patch.png' dpi_scale='true'"}});
    ui::Attach(p801, p802);

    auto* p803 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (3.2)"}});
    ui::Attach(p801, p803);

    auto* p804 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "original: 100x100  dpi_scale='true'"}});
    ui::Attach(p801, p804);

    ui::Attach(p796, p801);

    auto* p805 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p806 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "auto"}, {"height", "auto"}, {"bkimage", "file='nine-patch.png' dpi_scale='false' dest='0,0,180,180' dest_scale='false' corner='13,13,13,13'"}});
    ui::Attach(p805, p806);

    auto* p807 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (3.3)"}});
    ui::Attach(p805, p807);

    auto* p808 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "Nine-patch drawing \\ndpi_scale='false' \\ndest='0,0,180,180' \\ndest_scale='false' \\ncorner='13,13,13,13'"}, {"width", "auto"}});
    ui::Attach(p805, p808);

    ui::Attach(p796, p805);

    auto* p809 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p810 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "120"}, {"height", "120"}, {"bkimage", "file='libre-gui-frame.png' dpi_scale='false' halign='center' valign='center'"}});
    ui::Attach(p809, p810);

    auto* p811 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (3.4)"}});
    ui::Attach(p809, p811);

    auto* p812 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "original (14*14)  no stretch  dpi_scale='false'  halign='center'  valign='center'"}});
    ui::Attach(p809, p812);

    ui::Attach(p796, p809);

    auto* p813 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p814 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "120"}, {"height", "120"}, {"bkimage", "file='libre-gui-frame.png' dpi_scale='false' corner='6,6,6,6'"}});
    ui::Attach(p813, p814);

    auto* p815 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (3.5)"}});
    ui::Attach(p813, p815);

    auto* p816 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "Nine-patch drawing  original dpi_scale='false'  corner='6,6,6,6'"}});
    ui::Attach(p813, p816);

    ui::Attach(p796, p813);

    auto* p817 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p818 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "120"}, {"height", "120"}, {"bkimage", "file='libre-gui-frame.png' dpi_scale='true' halign='center' valign='center'"}});
    ui::Attach(p817, p818);

    auto* p819 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (3.6)"}});
    ui::Attach(p817, p819);

    auto* p820 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "original (14*14)  DPI adaptive  dpi_scale='true'  halign='center'  valign='center'"}});
    ui::Attach(p817, p820);

    ui::Attach(p796, p817);

    auto* p821 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p822 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "120"}, {"height", "120"}, {"bkimage", "file='libre-gui-frame.png' dpi_scale='true' corner='6,6,6,6'"}});
    ui::Attach(p821, p822);

    auto* p823 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (3.7)"}});
    ui::Attach(p821, p823);

    auto* p824 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "Nine-patch drawing  DPI adaptive dpi_scale='true'  corner='6,6,6,6'"}});
    ui::Attach(p821, p824);

    ui::Attach(p796, p821);

    auto* p825 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p826 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "120"}, {"height", "120"}, {"bkimage", "file='libre-gui-frame.png' dpi_scale='false' width='28' corner='12,12,12,12'"}});
    ui::Attach(p825, p826);

    auto* p827 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (3.8)"}});
    ui::Attach(p825, p827);

    auto* p828 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "Nine-patch drawing  manual zoom dpi_scale='false'  width='28'  corner='12,12,12,12'"}});
    ui::Attach(p825, p828);

    ui::Attach(p796, p825);

    auto* p829 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p830 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "120"}, {"height", "120"}, {"bkimage", "file='libre-gui-frame.png' dpi_scale='false'"}});
    ui::Attach(p829, p830);

    auto* p831 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (3.9)"}});
    ui::Attach(p829, p831);

    auto* p832 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "original (14*14) stretch dpi_scale='false' "}});
    ui::Attach(p829, p832);

    ui::Attach(p796, p829);

    auto* p833 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p834 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"bkimage", "file='autumn.png' fade='64'"}});
    ui::Attach(p833, p834);

    auto* p835 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (3.10)"}});
    ui::Attach(p833, p835);

    auto* p836 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "fade='64'  opacity 64"}, {"text_align", "center,vcenter"}});
    ui::Attach(p833, p836);

    ui::Attach(p796, p833);

    ui::Attach(p528, p796);

    auto* p837 = ui::Create<ui::Line>(pWindow, {{"class", "splitline_hor_level1"}, {"margin", "4,4,4,4"}});
    ui::Attach(p528, p837);

    auto* p838 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_text"}, {"text", "Test 4: Nine-patch drawing, stretch and tile test of the middle part"}, {"width", "100%"}, {"margin", "0,4,0,0"}, {"font", "system_bold_14"}});
    ui::Attach(p528, p838);

    auto* p839 = ui::Create<ui::HFlowScrollBox>(pWindow, {{"width", "auto"}, {"height", "auto"}, {"child_margin_x", "2"}, {"child_margin_y", "2"}, {"padding", "2,2,2,2"}, {"bkcolor", "Cornsilk"}});
    auto* p840 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p841 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p842 = ui::Create<ui::VBox>(pWindow, {{"width", "212"}, {"height", "200"}});
    auto* p843 = ui::Create<ui::HBox>(pWindow, {});
    auto* p844 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.png' dpi_scale='false' corner='14,14,14,14'"}});
    ui::Attach(p843, p844);

    auto* p845 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"width", "5"}, {"border_color", "white"}, {"border_size", "2,0,2,0"}});
    ui::Attach(p843, p845);

    auto* p846 = ui::Create<ui::Control>(pWindow, {{"min_width", "7"}, {"width", "7"}});
    ui::Attach(p843, p846);

    ui::Attach(p842, p843);

    ui::Attach(p841, p842);

    auto* p847 = ui::Create<ui::VBox>(pWindow, {{"width", "200"}, {"height", "212"}});
    auto* p848 = ui::Create<ui::VBox>(pWindow, {});
    auto* p849 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.png' dpi_scale='false' corner='14,14,14,14'"}});
    ui::Attach(p848, p849);

    auto* p850 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"height", "5"}, {"border_color", "white"}, {"border_size", "0,2,0,2"}});
    ui::Attach(p848, p850);

    auto* p851 = ui::Create<ui::Control>(pWindow, {{"min_height", "7"}, {"height", "7"}});
    ui::Attach(p848, p851);

    ui::Attach(p847, p848);

    ui::Attach(p841, p847);

    ui::Attach(p840, p841);

    auto* p852 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (4.1)"}, {"width", "100%"}});
    ui::Attach(p840, p852);

    auto* p853 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Nine-patch Drawing Stretch (default)"}, {"width", "100%"}});
    ui::Attach(p840, p853);

    ui::Attach(p839, p840);

    auto* p854 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p855 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p856 = ui::Create<ui::VBox>(pWindow, {{"width", "212"}, {"height", "200"}});
    auto* p857 = ui::Create<ui::HBox>(pWindow, {});
    auto* p858 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.png' dpi_scale='false' corner='14,14,14,14' xtiled='true' full_xtiled='true'"}});
    ui::Attach(p857, p858);

    auto* p859 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"width", "5"}, {"border_color", "white"}, {"border_size", "2,0,2,0"}});
    ui::Attach(p857, p859);

    auto* p860 = ui::Create<ui::Control>(pWindow, {{"min_width", "7"}, {"width", "7"}});
    ui::Attach(p857, p860);

    ui::Attach(p856, p857);

    ui::Attach(p855, p856);

    auto* p861 = ui::Create<ui::VBox>(pWindow, {{"width", "200"}, {"height", "212"}});
    auto* p862 = ui::Create<ui::VBox>(pWindow, {});
    auto* p863 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.png' dpi_scale='false' corner='14,14,14,14' xtiled='true' full_xtiled='true'"}});
    ui::Attach(p862, p863);

    auto* p864 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"height", "5"}, {"border_color", "white"}, {"border_size", "0,2,0,2"}});
    ui::Attach(p862, p864);

    auto* p865 = ui::Create<ui::Control>(pWindow, {{"min_height", "7"}, {"height", "7"}});
    ui::Attach(p862, p865);

    ui::Attach(p861, p862);

    ui::Attach(p855, p861);

    ui::Attach(p854, p855);

    auto* p866 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (4.2)"}, {"width", "100%"}});
    ui::Attach(p854, p866);

    auto* p867 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Nine-patch tiling xtiled='true'  full_xtiled='true'"}, {"width", "100%"}});
    ui::Attach(p854, p867);

    ui::Attach(p839, p854);

    auto* p868 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p869 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p870 = ui::Create<ui::VBox>(pWindow, {{"width", "212"}, {"height", "200"}});
    auto* p871 = ui::Create<ui::HBox>(pWindow, {});
    auto* p872 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.png' dpi_scale='false' corner='14,14,14,14' xtiled='true' full_xtiled='false'"}});
    ui::Attach(p871, p872);

    auto* p873 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"width", "5"}, {"border_color", "white"}, {"border_size", "2,0,2,0"}});
    ui::Attach(p871, p873);

    auto* p874 = ui::Create<ui::Control>(pWindow, {{"min_width", "7"}, {"width", "7"}});
    ui::Attach(p871, p874);

    ui::Attach(p870, p871);

    ui::Attach(p869, p870);

    auto* p875 = ui::Create<ui::VBox>(pWindow, {{"width", "200"}, {"height", "212"}});
    auto* p876 = ui::Create<ui::VBox>(pWindow, {});
    auto* p877 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.png' dpi_scale='false' corner='14,14,14,14' xtiled='true' full_xtiled='false'"}});
    ui::Attach(p876, p877);

    auto* p878 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"height", "5"}, {"border_color", "white"}, {"border_size", "0,2,0,2"}});
    ui::Attach(p876, p878);

    auto* p879 = ui::Create<ui::Control>(pWindow, {{"min_height", "7"}, {"height", "7"}});
    ui::Attach(p876, p879);

    ui::Attach(p875, p876);

    ui::Attach(p869, p875);

    ui::Attach(p868, p869);

    auto* p880 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (4.3)"}, {"width", "100%"}});
    ui::Attach(p868, p880);

    auto* p881 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Nine-patch tiling xtiled='true'  full_xtiled='false'"}, {"width", "100%"}});
    ui::Attach(p868, p881);

    ui::Attach(p839, p868);

    auto* p882 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p883 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p884 = ui::Create<ui::VBox>(pWindow, {{"width", "212"}, {"height", "200"}});
    auto* p885 = ui::Create<ui::HBox>(pWindow, {});
    auto* p886 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.png' dpi_scale='false' corner='14,14,14,14' ytiled='true' full_ytiled='true'"}});
    ui::Attach(p885, p886);

    auto* p887 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"width", "5"}, {"border_color", "white"}, {"border_size", "2,0,2,0"}});
    ui::Attach(p885, p887);

    auto* p888 = ui::Create<ui::Control>(pWindow, {{"min_width", "7"}, {"width", "7"}});
    ui::Attach(p885, p888);

    ui::Attach(p884, p885);

    ui::Attach(p883, p884);

    auto* p889 = ui::Create<ui::VBox>(pWindow, {{"width", "200"}, {"height", "212"}});
    auto* p890 = ui::Create<ui::VBox>(pWindow, {});
    auto* p891 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.png' dpi_scale='false' corner='14,14,14,14' ytiled='true' full_ytiled='true'"}});
    ui::Attach(p890, p891);

    auto* p892 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"height", "5"}, {"border_color", "white"}, {"border_size", "0,2,0,2"}});
    ui::Attach(p890, p892);

    auto* p893 = ui::Create<ui::Control>(pWindow, {{"min_height", "7"}, {"height", "7"}});
    ui::Attach(p890, p893);

    ui::Attach(p889, p890);

    ui::Attach(p883, p889);

    ui::Attach(p882, p883);

    auto* p894 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (4.4)"}, {"width", "100%"}});
    ui::Attach(p882, p894);

    auto* p895 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Nine-patch tiling ytiled='true'  full_ytiled='true'"}, {"width", "100%"}});
    ui::Attach(p882, p895);

    ui::Attach(p839, p882);

    auto* p896 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p897 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p898 = ui::Create<ui::VBox>(pWindow, {{"width", "212"}, {"height", "200"}});
    auto* p899 = ui::Create<ui::HBox>(pWindow, {});
    auto* p900 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.png' dpi_scale='false' corner='14,14,14,14' ytiled='true' full_ytiled='false'"}});
    ui::Attach(p899, p900);

    auto* p901 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"width", "5"}, {"border_color", "white"}, {"border_size", "2,0,2,0"}});
    ui::Attach(p899, p901);

    auto* p902 = ui::Create<ui::Control>(pWindow, {{"min_width", "7"}, {"width", "7"}});
    ui::Attach(p899, p902);

    ui::Attach(p898, p899);

    ui::Attach(p897, p898);

    auto* p903 = ui::Create<ui::VBox>(pWindow, {{"width", "200"}, {"height", "212"}});
    auto* p904 = ui::Create<ui::VBox>(pWindow, {});
    auto* p905 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.png' dpi_scale='false' corner='14,14,14,14' ytiled='true' full_ytiled='false'"}});
    ui::Attach(p904, p905);

    auto* p906 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"height", "5"}, {"border_color", "white"}, {"border_size", "0,2,0,2"}});
    ui::Attach(p904, p906);

    auto* p907 = ui::Create<ui::Control>(pWindow, {{"min_height", "7"}, {"height", "7"}});
    ui::Attach(p904, p907);

    ui::Attach(p903, p904);

    ui::Attach(p897, p903);

    ui::Attach(p896, p897);

    auto* p908 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (4.5)"}, {"width", "100%"}});
    ui::Attach(p896, p908);

    auto* p909 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Nine-patch tiling ytiled='true'  full_ytiled='false'"}, {"width", "100%"}});
    ui::Attach(p896, p909);

    ui::Attach(p839, p896);

    auto* p910 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p911 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p912 = ui::Create<ui::VBox>(pWindow, {{"width", "212"}, {"height", "200"}});
    auto* p913 = ui::Create<ui::HBox>(pWindow, {});
    auto* p914 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.png' dpi_scale='false' corner='14,14,14,14' xtiled='true' ytiled='true' full_xtiled='true' full_ytiled='true'"}});
    ui::Attach(p913, p914);

    auto* p915 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"width", "5"}, {"border_color", "white"}, {"border_size", "2,0,2,0"}});
    ui::Attach(p913, p915);

    auto* p916 = ui::Create<ui::Control>(pWindow, {{"min_width", "7"}, {"width", "7"}});
    ui::Attach(p913, p916);

    ui::Attach(p912, p913);

    ui::Attach(p911, p912);

    auto* p917 = ui::Create<ui::VBox>(pWindow, {{"width", "200"}, {"height", "212"}});
    auto* p918 = ui::Create<ui::VBox>(pWindow, {});
    auto* p919 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.png' dpi_scale='false' corner='14,14,14,14' xtiled='true' ytiled='true' full_xtiled='true' full_ytiled='true'"}});
    ui::Attach(p918, p919);

    auto* p920 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"height", "5"}, {"border_color", "white"}, {"border_size", "0,2,0,2"}});
    ui::Attach(p918, p920);

    auto* p921 = ui::Create<ui::Control>(pWindow, {{"min_height", "7"}, {"height", "7"}});
    ui::Attach(p918, p921);

    ui::Attach(p917, p918);

    ui::Attach(p911, p917);

    ui::Attach(p910, p911);

    auto* p922 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (4.6)"}, {"width", "100%"}});
    ui::Attach(p910, p922);

    auto* p923 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Nine-patch tiling xtiled='true'  full_xtiled='true'  ytiled='true'  full_ytiled='true'"}, {"width", "100%"}});
    ui::Attach(p910, p923);

    ui::Attach(p839, p910);

    auto* p924 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p925 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p926 = ui::Create<ui::VBox>(pWindow, {{"width", "212"}, {"height", "200"}});
    auto* p927 = ui::Create<ui::HBox>(pWindow, {});
    auto* p928 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.png' dpi_scale='false' corner='14,14,14,14' xtiled='true' ytiled='true' full_xtiled='false' full_ytiled='true'"}});
    ui::Attach(p927, p928);

    auto* p929 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"width", "5"}, {"border_color", "white"}, {"border_size", "2,0,2,0"}});
    ui::Attach(p927, p929);

    auto* p930 = ui::Create<ui::Control>(pWindow, {{"min_width", "7"}, {"width", "7"}});
    ui::Attach(p927, p930);

    ui::Attach(p926, p927);

    ui::Attach(p925, p926);

    auto* p931 = ui::Create<ui::VBox>(pWindow, {{"width", "200"}, {"height", "212"}});
    auto* p932 = ui::Create<ui::VBox>(pWindow, {});
    auto* p933 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.png' dpi_scale='false' corner='14,14,14,14' xtiled='true' ytiled='true' full_xtiled='false' full_ytiled='true'"}});
    ui::Attach(p932, p933);

    auto* p934 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"height", "5"}, {"border_color", "white"}, {"border_size", "0,2,0,2"}});
    ui::Attach(p932, p934);

    auto* p935 = ui::Create<ui::Control>(pWindow, {{"min_height", "7"}, {"height", "7"}});
    ui::Attach(p932, p935);

    ui::Attach(p931, p932);

    ui::Attach(p925, p931);

    ui::Attach(p924, p925);

    auto* p936 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (4.7)"}, {"width", "100%"}});
    ui::Attach(p924, p936);

    auto* p937 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Nine-patch tiling xtiled='true'  full_xtiled='false'  ytiled='true'  full_ytiled='true'"}, {"width", "100%"}});
    ui::Attach(p924, p937);

    ui::Attach(p839, p924);

    auto* p938 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p939 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p940 = ui::Create<ui::VBox>(pWindow, {{"width", "212"}, {"height", "200"}});
    auto* p941 = ui::Create<ui::HBox>(pWindow, {});
    auto* p942 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.png' dpi_scale='false' corner='14,14,14,14' xtiled='true' ytiled='true' full_xtiled='true' full_ytiled='false'"}});
    ui::Attach(p941, p942);

    auto* p943 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"width", "5"}, {"border_color", "white"}, {"border_size", "2,0,2,0"}});
    ui::Attach(p941, p943);

    auto* p944 = ui::Create<ui::Control>(pWindow, {{"min_width", "7"}, {"width", "7"}});
    ui::Attach(p941, p944);

    ui::Attach(p940, p941);

    ui::Attach(p939, p940);

    auto* p945 = ui::Create<ui::VBox>(pWindow, {{"width", "200"}, {"height", "212"}});
    auto* p946 = ui::Create<ui::VBox>(pWindow, {});
    auto* p947 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.png' dpi_scale='false' corner='14,14,14,14' xtiled='true' ytiled='true' full_xtiled='true' full_ytiled='false'"}});
    ui::Attach(p946, p947);

    auto* p948 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"height", "5"}, {"border_color", "white"}, {"border_size", "0,2,0,2"}});
    ui::Attach(p946, p948);

    auto* p949 = ui::Create<ui::Control>(pWindow, {{"min_height", "7"}, {"height", "7"}});
    ui::Attach(p946, p949);

    ui::Attach(p945, p946);

    ui::Attach(p939, p945);

    ui::Attach(p938, p939);

    auto* p950 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (4.8)"}, {"width", "100%"}});
    ui::Attach(p938, p950);

    auto* p951 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Nine-patch tiling xtiled='true'  full_xtiled='true'  ytiled='true'  full_ytiled='false'"}, {"width", "100%"}});
    ui::Attach(p938, p951);

    ui::Attach(p839, p938);

    auto* p952 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p953 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p954 = ui::Create<ui::VBox>(pWindow, {{"width", "212"}, {"height", "200"}});
    auto* p955 = ui::Create<ui::HBox>(pWindow, {});
    auto* p956 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.png' dpi_scale='false' corner='14,14,14,14' xtiled='true' ytiled='true' full_xtiled='false' full_ytiled='false'"}});
    ui::Attach(p955, p956);

    auto* p957 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"width", "5"}, {"border_color", "white"}, {"border_size", "2,0,2,0"}});
    ui::Attach(p955, p957);

    auto* p958 = ui::Create<ui::Control>(pWindow, {{"min_width", "7"}, {"width", "7"}});
    ui::Attach(p955, p958);

    ui::Attach(p954, p955);

    ui::Attach(p953, p954);

    auto* p959 = ui::Create<ui::VBox>(pWindow, {{"width", "200"}, {"height", "212"}});
    auto* p960 = ui::Create<ui::VBox>(pWindow, {});
    auto* p961 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.png' dpi_scale='false' corner='14,14,14,14' xtiled='true' ytiled='true' full_xtiled='false' full_ytiled='false'"}});
    ui::Attach(p960, p961);

    auto* p962 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"height", "5"}, {"border_color", "white"}, {"border_size", "0,2,0,2"}});
    ui::Attach(p960, p962);

    auto* p963 = ui::Create<ui::Control>(pWindow, {{"min_height", "7"}, {"height", "7"}});
    ui::Attach(p960, p963);

    ui::Attach(p959, p960);

    ui::Attach(p953, p959);

    ui::Attach(p952, p953);

    auto* p964 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (4.9)"}, {"width", "100%"}});
    ui::Attach(p952, p964);

    auto* p965 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Nine-patch tiling xtiled='true'  full_xtiled='false'  ytiled='true'  full_ytiled='false'"}, {"width", "100%"}});
    ui::Attach(p952, p965);

    ui::Attach(p839, p952);

    auto* p966 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p967 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p968 = ui::Create<ui::VBox>(pWindow, {{"width", "212"}, {"height", "200"}});
    auto* p969 = ui::Create<ui::HBox>(pWindow, {});
    auto* p970 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.png' dpi_scale='false' corner='14,14,14,14' xtiled='true' ytiled='true' full_xtiled='false' full_ytiled='false' tiled_margin='1'"}});
    ui::Attach(p969, p970);

    auto* p971 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"width", "5"}, {"border_color", "white"}, {"border_size", "2,0,2,0"}});
    ui::Attach(p969, p971);

    auto* p972 = ui::Create<ui::Control>(pWindow, {{"min_width", "7"}, {"width", "7"}});
    ui::Attach(p969, p972);

    ui::Attach(p968, p969);

    ui::Attach(p967, p968);

    auto* p973 = ui::Create<ui::VBox>(pWindow, {{"width", "200"}, {"height", "212"}});
    auto* p974 = ui::Create<ui::VBox>(pWindow, {});
    auto* p975 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.png' dpi_scale='false' corner='14,14,14,14' xtiled='true' ytiled='true' full_xtiled='false' full_ytiled='false' tiled_margin='1'"}});
    ui::Attach(p974, p975);

    auto* p976 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"height", "5"}, {"border_color", "white"}, {"border_size", "0,2,0,2"}});
    ui::Attach(p974, p976);

    auto* p977 = ui::Create<ui::Control>(pWindow, {{"min_height", "7"}, {"height", "7"}});
    ui::Attach(p974, p977);

    ui::Attach(p973, p974);

    ui::Attach(p967, p973);

    ui::Attach(p966, p967);

    auto* p978 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (4.10)"}, {"width", "100%"}});
    ui::Attach(p966, p978);

    auto* p979 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Nine-patch tiling xtiled='true'  full_xtiled='false'  ytiled='true'  full_ytiled='false'  tiled_margin='1'"}, {"width", "100%"}});
    ui::Attach(p966, p979);

    ui::Attach(p839, p966);

    auto* p980 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p981 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p982 = ui::Create<ui::VBox>(pWindow, {{"width", "212"}, {"height", "200"}});
    auto* p983 = ui::Create<ui::HBox>(pWindow, {});
    auto* p984 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.png' dpi_scale='false' corner='14,14,14,14' xtiled='true' ytiled='true' full_xtiled='false' full_ytiled='false' tiled_margin='1' tiled_padding='1,1,1,1'"}});
    ui::Attach(p983, p984);

    auto* p985 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"width", "5"}, {"border_color", "white"}, {"border_size", "2,0,2,0"}});
    ui::Attach(p983, p985);

    auto* p986 = ui::Create<ui::Control>(pWindow, {{"min_width", "7"}, {"width", "7"}});
    ui::Attach(p983, p986);

    ui::Attach(p982, p983);

    ui::Attach(p981, p982);

    auto* p987 = ui::Create<ui::VBox>(pWindow, {{"width", "200"}, {"height", "212"}});
    auto* p988 = ui::Create<ui::VBox>(pWindow, {});
    auto* p989 = ui::Create<ui::Control>(pWindow, {{"class", "page_image3_image"}, {"width", "100%"}, {"height", "100%"}, {"bkimage", "file='svg_test.png' dpi_scale='false' corner='14,14,14,14' xtiled='true' ytiled='true' full_xtiled='false' full_ytiled='false' tiled_margin='1' tiled_padding='1,1,1,1'"}});
    ui::Attach(p988, p989);

    auto* p990 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"height", "5"}, {"border_color", "white"}, {"border_size", "0,2,0,2"}});
    ui::Attach(p988, p990);

    auto* p991 = ui::Create<ui::Control>(pWindow, {{"min_height", "7"}, {"height", "7"}});
    ui::Attach(p988, p991);

    ui::Attach(p987, p988);

    ui::Attach(p981, p987);

    ui::Attach(p980, p981);

    auto* p992 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Test Case (4.11)"}, {"width", "100%"}});
    ui::Attach(p980, p992);

    auto* p993 = ui::Create<ui::Label>(pWindow, {{"class", "page_image3_case"}, {"text", "Nine-patch tiling xtiled='true'  full_xtiled='false'  ytiled='true'  full_ytiled='false'  tiled_margin='1'  tiled_padding='1,1,1,1'"}, {"width", "100%"}});
    ui::Attach(p980, p993);

    ui::Attach(p839, p980);

    ui::Attach(p528, p839);

    ui::Attach(p526, p528);

    ui::Attach(p22, p526);

    auto* p994 = ui::Create<ui::VBox>(pWindow, {{"name", "main_view_page_03"}});
    auto* p995 = ui::Create<ui::Label>(pWindow, {{"text", "Page 4: Text Drawing - Horizontal Text"}});
    ui::Attach(p994, p995);

    auto* p996 = ui::Create<ui::HBox>(pWindow, {});
    auto* p997 = ui::Create<ui::VBox>(pWindow, {{"width", "50%"}, {"margin", "4"}});
    auto* p998 = ui::Create<ui::HBox>(pWindow, {{"height", "56"}, {"bkcolor", "blue"}});
    auto* p999 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text (H: Left, V: Top)"}, {"text_align", "left,top"}});
    ui::Attach(p998, p999);

    ui::Attach(p997, p998);

    auto* p1000 = ui::Create<ui::HBox>(pWindow, {{"height", "56"}, {"bkcolor", "dark_gray"}});
    auto* p1001 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text (H: Left, V: Center)"}, {"text_align", "left,vcenter"}});
    ui::Attach(p1000, p1001);

    ui::Attach(p997, p1000);

    auto* p1002 = ui::Create<ui::HBox>(pWindow, {{"height", "56"}, {"bkcolor", "blue"}});
    auto* p1003 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text (H: Left, V: Bottom)"}, {"text_align", "left,bottom"}});
    ui::Attach(p1002, p1003);

    ui::Attach(p997, p1002);

    auto* p1004 = ui::Create<ui::HBox>(pWindow, {{"height", "56"}, {"bkcolor", "dark_gray"}});
    auto* p1005 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text (H: Center, V: Top)"}, {"text_align", "center,top"}});
    ui::Attach(p1004, p1005);

    ui::Attach(p997, p1004);

    auto* p1006 = ui::Create<ui::HBox>(pWindow, {{"height", "56"}, {"bkcolor", "blue"}});
    auto* p1007 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text (H: Center, V: Center)"}, {"text_align", "center,vcenter"}});
    ui::Attach(p1006, p1007);

    ui::Attach(p997, p1006);

    auto* p1008 = ui::Create<ui::HBox>(pWindow, {{"height", "56"}, {"bkcolor", "dark_gray"}});
    auto* p1009 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text (H: Center, V: Bottom)"}, {"text_align", "center,bottom"}});
    ui::Attach(p1008, p1009);

    ui::Attach(p997, p1008);

    auto* p1010 = ui::Create<ui::HBox>(pWindow, {{"height", "56"}, {"bkcolor", "blue"}});
    auto* p1011 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text (H: Right, V: Top)"}, {"text_align", "right,top"}});
    ui::Attach(p1010, p1011);

    ui::Attach(p997, p1010);

    auto* p1012 = ui::Create<ui::HBox>(pWindow, {{"height", "56"}, {"bkcolor", "dark_gray"}});
    auto* p1013 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text (H: Right, V: Center)"}, {"text_align", "right,vcenter"}});
    ui::Attach(p1012, p1013);

    ui::Attach(p997, p1012);

    auto* p1014 = ui::Create<ui::HBox>(pWindow, {{"height", "56"}, {"bkcolor", "blue"}});
    auto* p1015 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text (H: Right, V: Bottom)"}, {"text_align", "right,bottom"}});
    ui::Attach(p1014, p1015);

    ui::Attach(p997, p1014);

    auto* p1016 = ui::Create<ui::HBox>(pWindow, {{"height", "56"}, {"bkcolor", "dark_gray"}});
    auto* p1017 = ui::Create<ui::Label>(pWindow, {{"height", "auto"}, {"width", "auto"}, {"text", "MeasureString result test 1, MeasureString result test 2, MeasureString result test 3, "}, {"single_line", "false"}, {"bkcolor", "red"}, {"text_align", "hcenter,vcenter"}});
    ui::Attach(p1016, p1017);

    ui::Attach(p997, p1016);

    auto* p1018 = ui::Create<ui::HBox>(pWindow, {{"height", "56"}, {"bkcolor", "blue"}});
    auto* p1019 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text Align (H: Justified, V: Center) Multi-line text test with line breaks"}, {"text_align", "hjustify,vcenter"}, {"multi_line", "true"}, {"replace_newline", "true"}});
    ui::Attach(p1018, p1019);

    ui::Attach(p997, p1018);

    auto* p1020 = ui::Create<ui::HBox>(pWindow, {{"height", "80"}, {"bkcolor", "dark_gray"}});
    auto* p1021 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text Align (H: Left, V: Center) This case sets line spacing. Normal line spacing"}, {"text_align", "left,vcenter"}, {"multi_line", "true"}, {"replace_newline", "true"}});
    ui::Attach(p1020, p1021);

    ui::Attach(p997, p1020);

    auto* p1022 = ui::Create<ui::HBox>(pWindow, {{"height", "80"}, {"bkcolor", "blue"}});
    auto* p1023 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text Align (H: Left, V: Center) This case sets line spacing. spacing_mul='1.2'"}, {"text_align", "left,vcenter"}, {"multi_line", "true"}, {"replace_newline", "true"}, {"spacing_mul", "1.2"}});
    ui::Attach(p1022, p1023);

    ui::Attach(p997, p1022);

    ui::Attach(p996, p997);

    auto* p1024 = ui::Create<ui::VBox>(pWindow, {{"width", "50%"}, {"margin", "4"}});
    auto* p1025 = ui::Create<ui::HBox>(pWindow, {{"height", "56"}, {"bkcolor", "dark_gray"}});
    auto* p1026 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Long text drawing test, single line text, no automatic ellipsis, 1234567890-. qwertyuiopasdfghjkl;lmnozxccvb.NBVCXZASDFGHJKLPOIUYTREWQ."}, {"autotooltip", "true"}, {"endellipsis", "false"}});
    ui::Attach(p1025, p1026);

    ui::Attach(p1024, p1025);

    auto* p1027 = ui::Create<ui::HBox>(pWindow, {{"height", "56"}, {"bkcolor", "blue"}});
    auto* p1028 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Long text drawing test, single line text, automatically adds ellipsis, 1234567890-. qwertyuiopasdfghjkl;lmnozxccvb.NBVCXZASDFGHJKLPOIUYTREWQ."}, {"autotooltip", "true"}});
    ui::Attach(p1027, p1028);

    ui::Attach(p1024, p1027);

    auto* p1029 = ui::Create<ui::HBox>(pWindow, {{"height", "56"}, {"bkcolor", "dark_gray"}});
    auto* p1030 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Long text drawing test, multi-line text, text is 1234567890-. qwertyuiopasdfghjkl-v."}, {"autotooltip", "true"}, {"singleline", "false"}});
    ui::Attach(p1029, p1030);

    ui::Attach(p1024, p1029);

    auto* p1031 = ui::Create<ui::HBox>(pWindow, {{"height", "56"}, {"bkcolor", "blue"}});
    auto* p1032 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Long text drawing test, multi-line text, automatically adds ellipsis, long text test good bad test text is 1234567890-. qwertyuiopasdfghjkl;lmnozxccvb.NBVCXZASDFGHJKLPOIUYTREWQ. Long text test, long text test, long text test, long text test, long text test, long text test, long text test, long text test, long text test, long text test, long text test, long text test"}, {"autotooltip", "true"}, {"singleline", "false"}});
    ui::Attach(p1031, p1032);

    ui::Attach(p1024, p1031);

    auto* p1033 = ui::Create<ui::HBox>(pWindow, {{"height", "56"}, {"bkcolor", "dark_gray"}});
    auto* p1034 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Long text drawing test, multi-line text, no automatic ellipsis, 1234567890NBVCXZASDFGHJKLPOIUYTREWQNBVCXZASDFGHJKLPOIUYTREWQ long text test good bad test text is -. qwertyuiopasdfghjkl;lmnozxccvb.NBVCXZASDFGHJKLPOIUYTREWQ. Long text test, long text test, long text test, long text test, long text test, long text test, long text test, long text test, long text test, long text test, long text test, long text test"}, {"autotooltip", "true"}, {"singleline", "false"}, {"endellipsis", "false"}});
    ui::Attach(p1033, p1034);

    ui::Attach(p1024, p1033);

    auto* p1035 = ui::Create<ui::HBox>(pWindow, {{"height", "56"}, {"bkcolor", "blue"}});
    auto* p1036 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "C:\\develop\\ProgrammingLearning\\WindowsKernelPrinciples\\Programming\\testProgrammingProgrammingProgrammingProgramming.txt"}, {"autotooltip", "true"}, {"singleline", "true"}, {"path_ellipsis", "true"}});
    ui::Attach(p1035, p1036);

    ui::Attach(p1024, p1035);

    auto* p1037 = ui::Create<ui::HBox>(pWindow, {{"height", "56"}, {"bkcolor", "dark_gray"}});
    auto* p1038 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text - Underlined"}, {"font", "system_underline_14"}});
    ui::Attach(p1037, p1038);

    ui::Attach(p1024, p1037);

    auto* p1039 = ui::Create<ui::HBox>(pWindow, {{"height", "56"}, {"bkcolor", "blue"}});
    auto* p1040 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text - Bold"}, {"font", "system_bold_14"}});
    ui::Attach(p1039, p1040);

    ui::Attach(p1024, p1039);

    auto* p1041 = ui::Create<ui::HBox>(pWindow, {{"height", "56"}, {"bkcolor", "dark_gray"}});
    auto* p1042 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text - Strikethrough"}, {"font", "system_strikeout_14"}});
    ui::Attach(p1041, p1042);

    ui::Attach(p1024, p1041);

    auto* p1043 = ui::Create<ui::HBox>(pWindow, {{"height", "56"}, {"bkcolor", "blue"}});
    auto* p1044 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text - Italic"}, {"font", "system_italic_14"}});
    ui::Attach(p1043, p1044);

    ui::Attach(p1024, p1043);

    auto* p1045 = ui::Create<ui::HBox>(pWindow, {{"height", "56"}, {"bkcolor", "dark_gray"}});
    auto* p1046 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text - All Styles Second column content"}, {"font", "system_fullstyle_20"}, {"normal_text_color", "green"}, {"replace_newline", "true"}, {"multi_line", "true"}});
    ui::Attach(p1045, p1046);

    ui::Attach(p1024, p1045);

    auto* p1047 = ui::Create<ui::HBox>(pWindow, {{"height", "80"}, {"bkcolor", "dark_gray"}});
    auto* p1048 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text Align (H: Left, V: Center) This case sets line spacing. word_spacing='6'"}, {"text_align", "left,vcenter"}, {"multi_line", "true"}, {"replace_newline", "true"}, {"word_spacing", "6"}});
    ui::Attach(p1047, p1048);

    ui::Attach(p1024, p1047);

    auto* p1049 = ui::Create<ui::HBox>(pWindow, {{"height", "80"}, {"bkcolor", "blue"}});
    auto* p1050 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text Align (H: Left, V: Center) This case sets line spacing. spacing_mul='1.2' spacing_add='6'"}, {"text_align", "left,vcenter"}, {"multi_line", "true"}, {"replace_newline", "true"}, {"spacing_mul", "1.2"}, {"spacing_add", "6"}});
    ui::Attach(p1049, p1050);

    ui::Attach(p1024, p1049);

    ui::Attach(p996, p1024);

    auto* p1051 = ui::Create<ui::VBox>(pWindow, {{"width", "260"}, {"margin", "4"}});
    auto* p1052 = ui::Create<ui::RichText>(pWindow, {{"class", "rich_text"}, {"bkcolor", "green"}, {"row_spacing_mul", "1.5"}, {"width", "100%"}, {"height", "auto"}});
    ui::WindowBuilder::ParseRichTextXmlText(_T("<RichText>\n                RichText text<br/>Content 1\n                <b>\n                    Bold\n                    <bgcolor color=\"#FFFFFF\"><u><a href=\"www.baidu.com\">This is a hyperlink: click to visit Baidu</a></u></bgcolor><font color=\"#FF0000\">bold red</font></b><font face=\"SimSun\" size=\"12\">\n                    SimSun 12pt demo 1\n                    <font face=\"KaiTi\" size=\"18\">\n                        KaiTi effect demo: 18pt\n                    </font>\n                    SimSun 12pt demo 2\n                </font><a href=\"www.sohu.com\">This is a hyperlink: click to visit Sohu</a><font face=\"SimSun\" size=\"8\">\n                    SimSun 8pt demo\n                </font><i>italic demo</i><b>bold demo</b><s>strikethrough demo</s><u>underline demo</u><font face=\"Microsoft YaHei\" size=\"14\">\n                    Microsoft YaHei 14pt demo\n                </font><bgcolor color=\"#802378\">Background Color Demo</bgcolor>                \n                RichText content 2 content 3 content 4 content 5 content 6 content 7 content 8 content 9 content 10\n            </RichText>"), p1052);
    ui::Attach(p1051, p1052);

    auto* p1053 = ui::Create<ui::ScrollBox>(pWindow, {{"vscrollbar", "true"}, {"hscrollbar", "true"}, {"width", "260"}, {"height", "100"}, {"bkcolor", "white"}, {"border_size", "2"}, {"border_color", "red"}, {"padding", "4,4,4,4"}});
    auto* p1054 = ui::Create<ui::RichText>(pWindow, {{"class", "rich_text"}, {"bkcolor", "green"}, {"row_spacing_mul", "1.5"}, {"width", "auto"}, {"height", "auto"}, {"word_wrap", "false"}});
    ui::WindowBuilder::ParseRichTextXmlText(_T("<RichText>\n                    Line 1: RichText in ScrollBox<br/>\n                    Line 2: RichText text<br/>\n                    Line 3: RichText text<br/>\n                    Line 4: RichText text<br/>\n                    Line 5: RichText text<br/>\n                    Line 6: RichText text<br/>\n                    Line 7: RichText text<br/>\n                    Line 8: RichText text<br/>\n                    Line 9: RichText text<br/>\n                    Line 10: RichText content 2 content 3 content 4 content 5 content 6 content 7 content 8 content 9 content 10<br/></RichText>"), p1054);
    ui::Attach(p1053, p1054);

    ui::Attach(p1051, p1053);

    auto* p1055 = ui::Create<ui::RichTextBox>(pWindow, {{"text", "{u}RichTextBox{/u}"}, {"bkcolor", "blue"}});
    auto* p1056 = ui::Create<ui::Label>(pWindow, {{"text", "Label in RichTextBox"}, {"valign", "center"}});
    ui::Attach(p1055, p1056);

    ui::Attach(p1051, p1055);

    auto* p1057 = ui::Create<ui::RichTextHBox>(pWindow, {{"text", "{s}RichTextHBox{/s}"}, {"bkcolor", "dark_gray"}});
    auto* p1058 = ui::Create<ui::Label>(pWindow, {{"text", "Label in RichTextHBox"}, {"valign", "center"}});
    ui::Attach(p1057, p1058);

    ui::Attach(p1051, p1057);

    auto* p1059 = ui::Create<ui::RichTextVBox>(pWindow, {{"text", "{i}RichTextVBox{/i}"}, {"bkcolor", "blue"}, {"child_valign", "center"}});
    auto* p1060 = ui::Create<ui::Label>(pWindow, {{"text", "Label in RichTextVBox"}, {"valign", "center"}});
    ui::Attach(p1059, p1060);

    ui::Attach(p1051, p1059);

    ui::Attach(p996, p1051);

    auto* p1061 = ui::Create<ui::VBox>(pWindow, {{"width", "300"}, {"margin", "4"}});
    auto* p1062 = ui::Create<ui::HBox>(pWindow, {{"height", "60"}, {"bkcolor", "blue"}});
    auto* p1063 = ui::Create<ui::RichText>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "RichText text (H: Left, V: Top)"}, {"text_align", "left,top"}});
    ui::WindowBuilder::ParseRichTextXmlText(_T("<RichText><b><a href=\"www.sohu.com\">This is a hyperlink: click to visit Sohu</a></b></RichText>"), p1063);
    ui::Attach(p1062, p1063);

    ui::Attach(p1061, p1062);

    auto* p1064 = ui::Create<ui::HBox>(pWindow, {{"height", "60"}, {"bkcolor", "dark_gray"}});
    auto* p1065 = ui::Create<ui::RichText>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "RichText text (H: Left, V: Center)"}, {"text_align", "left,vcenter"}});
    ui::WindowBuilder::ParseRichTextXmlText(_T("<RichText><b><a href=\"www.sohu.com\">This is a hyperlink: click to visit Sohu</a></b></RichText>"), p1065);
    ui::Attach(p1064, p1065);

    ui::Attach(p1061, p1064);

    auto* p1066 = ui::Create<ui::HBox>(pWindow, {{"height", "60"}, {"bkcolor", "blue"}});
    auto* p1067 = ui::Create<ui::RichText>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "RichText text (H: Left, V: Bottom)"}, {"text_align", "left,bottom"}});
    ui::WindowBuilder::ParseRichTextXmlText(_T("<RichText><b><a href=\"www.sohu.com\">This is a hyperlink: click to visit Sohu</a></b></RichText>"), p1067);
    ui::Attach(p1066, p1067);

    ui::Attach(p1061, p1066);

    auto* p1068 = ui::Create<ui::HBox>(pWindow, {{"height", "60"}, {"bkcolor", "dark_gray"}});
    auto* p1069 = ui::Create<ui::RichText>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "RichText text (H: Center, V: Top)"}, {"text_align", "center,top"}});
    ui::WindowBuilder::ParseRichTextXmlText(_T("<RichText><b><a href=\"www.sohu.com\">This is a hyperlink: click to visit Sohu</a></b></RichText>"), p1069);
    ui::Attach(p1068, p1069);

    ui::Attach(p1061, p1068);

    auto* p1070 = ui::Create<ui::HBox>(pWindow, {{"height", "60"}, {"bkcolor", "blue"}});
    auto* p1071 = ui::Create<ui::RichText>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "RichText text (H: Center, V: Center)"}, {"text_align", "center,vcenter"}});
    ui::WindowBuilder::ParseRichTextXmlText(_T("<RichText><b><a href=\"www.sohu.com\">This is a hyperlink: click to visit Sohu</a></b></RichText>"), p1071);
    ui::Attach(p1070, p1071);

    ui::Attach(p1061, p1070);

    auto* p1072 = ui::Create<ui::HBox>(pWindow, {{"height", "60"}, {"bkcolor", "dark_gray"}});
    auto* p1073 = ui::Create<ui::RichText>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "RichText text (H: Center, V: Bottom)"}, {"text_align", "center,bottom"}});
    ui::WindowBuilder::ParseRichTextXmlText(_T("<RichText><b><a href=\"www.sohu.com\">This is a hyperlink: click to visit Sohu</a></b></RichText>"), p1073);
    ui::Attach(p1072, p1073);

    ui::Attach(p1061, p1072);

    auto* p1074 = ui::Create<ui::HBox>(pWindow, {{"height", "60"}, {"bkcolor", "blue"}});
    auto* p1075 = ui::Create<ui::RichText>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "RichText text (H: Right, V: Top)"}, {"text_align", "right,top"}});
    ui::WindowBuilder::ParseRichTextXmlText(_T("<RichText><b><a href=\"www.sohu.com\">This is a hyperlink: click to visit Sohu</a></b></RichText>"), p1075);
    ui::Attach(p1074, p1075);

    ui::Attach(p1061, p1074);

    auto* p1076 = ui::Create<ui::HBox>(pWindow, {{"height", "60"}, {"bkcolor", "dark_gray"}});
    auto* p1077 = ui::Create<ui::RichText>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "RichText text (H: Right, V: Center)"}, {"text_align", "right,vcenter"}});
    ui::WindowBuilder::ParseRichTextXmlText(_T("<RichText><b><a href=\"www.sohu.com\">This is a hyperlink: click to visit Sohu</a></b></RichText>"), p1077);
    ui::Attach(p1076, p1077);

    ui::Attach(p1061, p1076);

    auto* p1078 = ui::Create<ui::HBox>(pWindow, {{"height", "60"}, {"bkcolor", "blue"}});
    auto* p1079 = ui::Create<ui::RichText>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "RichText text (H: Right, V: Bottom)"}, {"text_align", "right,bottom"}});
    ui::WindowBuilder::ParseRichTextXmlText(_T("<RichText><b><a href=\"www.sohu.com\">This is a hyperlink: click to visit Sohu</a></b></RichText>"), p1079);
    ui::Attach(p1078, p1079);

    ui::Attach(p1061, p1078);

    ui::Attach(p996, p1061);

    ui::Attach(p994, p996);

    ui::Attach(p22, p994);

    auto* p1080 = ui::Create<ui::VBox>(pWindow, {{"name", "main_view_page_04"}});
    auto* p1081 = ui::Create<ui::Label>(pWindow, {{"text", "Page 5: Text Drawing - Vertical Text"}});
    ui::Attach(p1080, p1081);

    auto* p1082 = ui::Create<ui::HBox>(pWindow, {});
    auto* p1083 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"margin", "4"}});
    auto* p1084 = ui::Create<ui::HBox>(pWindow, {{"width", "80"}, {"bkcolor", "blue"}});
    auto* p1085 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text Align (H: Left, V: Top)"}, {"text_align", "left,top"}, {"vertical_text", "true"}});
    ui::Attach(p1084, p1085);

    ui::Attach(p1083, p1084);

    auto* p1086 = ui::Create<ui::HBox>(pWindow, {{"width", "80"}, {"bkcolor", "dark_gray"}});
    auto* p1087 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text Align (H: Left, V: Center)"}, {"text_align", "left,vcenter"}, {"vertical_text", "true"}});
    ui::Attach(p1086, p1087);

    ui::Attach(p1083, p1086);

    auto* p1088 = ui::Create<ui::HBox>(pWindow, {{"width", "80"}, {"bkcolor", "blue"}});
    auto* p1089 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text Align (H: Left, V: Bottom)"}, {"text_align", "left,bottom"}, {"vertical_text", "true"}});
    ui::Attach(p1088, p1089);

    ui::Attach(p1083, p1088);

    ui::Attach(p1082, p1083);

    auto* p1090 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"margin", "4"}});
    auto* p1091 = ui::Create<ui::HBox>(pWindow, {{"width", "80"}, {"bkcolor", "dark_gray"}});
    auto* p1092 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text Align (H: Center, V: Top)"}, {"text_align", "center,top"}, {"vertical_text", "true"}});
    ui::Attach(p1091, p1092);

    ui::Attach(p1090, p1091);

    auto* p1093 = ui::Create<ui::HBox>(pWindow, {{"width", "80"}, {"bkcolor", "blue"}});
    auto* p1094 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text Align (H: Center, V: Center)"}, {"text_align", "center,vcenter"}, {"vertical_text", "true"}});
    ui::Attach(p1093, p1094);

    ui::Attach(p1090, p1093);

    auto* p1095 = ui::Create<ui::HBox>(pWindow, {{"width", "80"}, {"bkcolor", "dark_gray"}});
    auto* p1096 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text Align (H: Center, V: Bottom)"}, {"text_align", "center,bottom"}, {"vertical_text", "true"}});
    ui::Attach(p1095, p1096);

    ui::Attach(p1090, p1095);

    ui::Attach(p1082, p1090);

    auto* p1097 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"margin", "4"}});
    auto* p1098 = ui::Create<ui::HBox>(pWindow, {{"width", "80"}, {"bkcolor", "blue"}});
    auto* p1099 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text Align (H: Right, V: Top)"}, {"text_align", "right,top"}, {"vertical_text", "true"}});
    ui::Attach(p1098, p1099);

    ui::Attach(p1097, p1098);

    auto* p1100 = ui::Create<ui::HBox>(pWindow, {{"width", "80"}, {"bkcolor", "dark_gray"}});
    auto* p1101 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text Align (H: Right, V: Center)"}, {"text_align", "right,vcenter"}, {"vertical_text", "true"}});
    ui::Attach(p1100, p1101);

    ui::Attach(p1097, p1100);

    auto* p1102 = ui::Create<ui::HBox>(pWindow, {{"width", "80"}, {"bkcolor", "blue"}});
    auto* p1103 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text Align (H: Right, V: Bottom)"}, {"text_align", "right,bottom"}, {"vertical_text", "true"}});
    ui::Attach(p1102, p1103);

    ui::Attach(p1097, p1102);

    ui::Attach(p1082, p1097);

    auto* p1104 = ui::Create<ui::VBox>(pWindow, {});
    auto* p1105 = ui::Create<ui::HBox>(pWindow, {{"margin", "4"}});
    auto* p1106 = ui::Create<ui::HBox>(pWindow, {{"width", "80"}, {"bkcolor", "blue"}});
    auto* p1107 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text Align (H: Center, V: Justified)"}, {"text_align", "hcenter,vjustify"}, {"vertical_text", "true"}});
    ui::Attach(p1106, p1107);

    ui::Attach(p1105, p1106);

    auto* p1108 = ui::Create<ui::HBox>(pWindow, {{"width", "80"}, {"bkcolor", "dark_gray"}});
    auto* p1109 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Long text drawing test, single column text, 1234567890-. qwertyuiopasdfghjkl;lmnozxccvb.NBVCXZASDFGHJKLPOIUYTREWQ."}, {"autotooltip", "true"}, {"vertical_text", "true"}, {"text_align", "right,top"}});
    ui::Attach(p1108, p1109);

    ui::Attach(p1105, p1108);

    auto* p1110 = ui::Create<ui::HBox>(pWindow, {{"width", "80"}, {"bkcolor", "blue"}});
    auto* p1111 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Long text drawing test, multi-column text, text is 1234567890-. Long text drawing test: 1+2=3, 8/4=2, multi-column text qwertyuiopasdfghjkl-v. End of long text drawing test."}, {"autotooltip", "true"}, {"single_line", "false"}, {"vertical_text", "true"}, {"text_align", "right,top"}});
    ui::Attach(p1110, p1111);

    ui::Attach(p1105, p1110);

    auto* p1112 = ui::Create<ui::HBox>(pWindow, {{"width", "80"}, {"bkcolor", "dark_gray"}});
    auto* p1113 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text - Normal Font"}, {"font", "system_14"}, {"vertical_text", "true"}, {"text_align", "right,top"}});
    ui::Attach(p1112, p1113);

    ui::Attach(p1105, p1112);

    auto* p1114 = ui::Create<ui::HBox>(pWindow, {{"width", "80"}, {"bkcolor", "blue"}});
    auto* p1115 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text - Bold"}, {"font", "system_bold_14"}, {"vertical_text", "true"}, {"text_align", "right,top"}});
    ui::Attach(p1114, p1115);

    ui::Attach(p1105, p1114);

    auto* p1116 = ui::Create<ui::HBox>(pWindow, {{"width", "80"}, {"bkcolor", "dark_gray"}});
    auto* p1117 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text - Italic"}, {"font", "system_italic_14"}, {"vertical_text", "true"}, {"text_align", "right,top"}});
    ui::Attach(p1116, p1117);

    ui::Attach(p1105, p1116);

    auto* p1118 = ui::Create<ui::HBox>(pWindow, {{"width", "80"}, {"bkcolor", "blue"}});
    auto* p1119 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "123 Text - Strikethrough"}, {"font", "system_strikeout_14"}, {"vertical_text", "true"}, {"text_align", "right,top"}});
    ui::Attach(p1118, p1119);

    ui::Attach(p1105, p1118);

    auto* p1120 = ui::Create<ui::HBox>(pWindow, {{"width", "80"}, {"bkcolor", "dark_gray"}});
    auto* p1121 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "A Text - Underlined"}, {"font", "system_underline_14"}, {"vertical_text", "true"}, {"text_align", "right,top"}});
    ui::Attach(p1120, p1121);

    ui::Attach(p1105, p1120);

    auto* p1122 = ui::Create<ui::HBox>(pWindow, {{"width", "80"}, {"bkcolor", "blue"}});
    auto* p1123 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text - All Styles Second column content"}, {"font", "system_fullstyle_16"}, {"normal_text_color", "white"}, {"vertical_text", "true"}, {"text_align", "right,top"}, {"replace_newline", "true"}, {"multi_line", "true"}});
    ui::Attach(p1122, p1123);

    ui::Attach(p1105, p1122);

    auto* p1124 = ui::Create<ui::HBox>(pWindow, {{"width", "80"}, {"bkcolor", "dark_gray"}});
    auto* p1125 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text Align (H: Right, V: Center) This case sets line spacing. Normal line spacing"}, {"text_align", "right,vcenter"}, {"multi_line", "true"}, {"replace_newline", "true"}, {"vertical_text", "true"}});
    ui::Attach(p1124, p1125);

    ui::Attach(p1105, p1124);

    auto* p1126 = ui::Create<ui::HBox>(pWindow, {{"width", "80"}, {"bkcolor", "blue"}});
    auto* p1127 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text Align (H: Right, V: Center) This case sets line spacing. spacing_mul='1.2'"}, {"text_align", "right,vcenter"}, {"multi_line", "true"}, {"replace_newline", "true"}, {"spacing_mul", "1.2"}, {"vertical_text", "true"}});
    ui::Attach(p1126, p1127);

    ui::Attach(p1105, p1126);

    auto* p1128 = ui::Create<ui::HBox>(pWindow, {{"width", "80"}, {"bkcolor", "dark_gray"}});
    auto* p1129 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text Align (H: Right, V: Center) This case sets line spacing. word_spacing='6'"}, {"text_align", "right,vcenter"}, {"multi_line", "true"}, {"replace_newline", "true"}, {"word_spacing", "6"}, {"vertical_text", "true"}});
    ui::Attach(p1128, p1129);

    ui::Attach(p1105, p1128);

    auto* p1130 = ui::Create<ui::HBox>(pWindow, {{"width", "80"}, {"bkcolor", "blue"}});
    auto* p1131 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text Align (H: Right, V: Center) This case sets line spacing. spacing_mul='1.2' spacing_add='6'"}, {"text_align", "right,vcenter"}, {"multi_line", "true"}, {"replace_newline", "true"}, {"spacing_mul", "1.2"}, {"spacing_add", "6"}, {"vertical_text", "true"}});
    ui::Attach(p1130, p1131);

    ui::Attach(p1105, p1130);

    ui::Attach(p1104, p1105);

    auto* p1132 = ui::Create<ui::HBox>(pWindow, {{"margin", "4,4,0,0"}});
    auto* p1133 = ui::Create<ui::HBox>(pWindow, {{"width", "80"}, {"bkcolor", "dark_gray"}});
    auto* p1134 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text Align (H: Center, V: Justified) Multi-line text test with line breaks Long text drawing test: 1+2=3, 8/4=2, end of multi-column text."}, {"text_align", "hcenter,vjustify"}, {"multi_line", "true"}, {"vertical_text", "true"}, {"replace_newline", "true"}});
    ui::Attach(p1133, p1134);

    ui::Attach(p1132, p1133);

    auto* p1135 = ui::Create<ui::HBox>(pWindow, {{"width", "80"}, {"bkcolor", "blue"}});
    auto* p1136 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Letters and numbers are not rotated 90 degrees, 1234567890-. qwertyuiopasdfghjkl;lmnozxccvb.NBVCXZASDFGHJKLPOIUYTREWQ."}, {"single_line", "false"}, {"vertical_text", "true"}, {"ascii_rotate_90", "false"}, {"text_align", "right,top"}});
    ui::Attach(p1135, p1136);

    ui::Attach(p1132, p1135);

    auto* p1137 = ui::Create<ui::HBox>(pWindow, {{"width", "80"}, {"bkcolor", "dark_gray"}});
    auto* p1138 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text displayed at actual height without fixed font height: 1234567890-. Long text drawing test: 1+2=3, 8/4=2, multi-column text qwertyuiopasdfghjkl-v. End of long text drawing test."}, {"single_line", "false"}, {"vertical_text", "true"}, {"use_font_height", "false"}, {"text_align", "right,top"}});
    ui::Attach(p1137, p1138);

    ui::Attach(p1132, p1137);

    auto* p1139 = ui::Create<ui::HBox>(pWindow, {{"width", "200"}, {"bkcolor", "blue"}});
    auto* p1140 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text displayed at actual height without fixed font height, vertical character spacing: 1234567890-. Long text drawing test: 1+2=3, 8/4=2, multi-column text qwertyuiopasdfghjkl-v. End of long text drawing test."}, {"single_line", "false"}, {"vertical_text", "true"}, {"use_font_height", "false"}, {"word_spacing", "3"}, {"text_align", "right,top"}});
    ui::Attach(p1139, p1140);

    ui::Attach(p1132, p1139);

    auto* p1141 = ui::Create<ui::HBox>(pWindow, {{"width", "180"}, {"bkcolor", "dark_gray"}});
    auto* p1142 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text displayed at actual height without fixed font height, vertical character spacing and column spacing: 1234567890-. Long text drawing test: 1+2=3, 8/4=2, multi-column text qwertyuiopasdfghjkl-v. End of long text drawing test."}, {"single_line", "false"}, {"vertical_text", "true"}, {"use_font_height", "false"}, {"word_spacing", "3"}, {"spacing_mul", "1.5"}, {"spacing_add", "6"}, {"text_align", "right,top"}});
    ui::Attach(p1141, p1142);

    ui::Attach(p1132, p1141);

    auto* p1143 = ui::Create<ui::HBox>(pWindow, {{"width", "180"}, {"bkcolor", "blue"}});
    auto* p1144 = ui::Create<ui::Label>(pWindow, {{"height", "stretch"}, {"width", "stretch"}, {"text", "Text displayed at actual height without fixed font height, vertical character spacing and column spacing: 1234567890-. Long text drawing test: 1+2=3, 8/4=2, multi-column text qwertyuiopasdfghjkl-v. End of long text drawing test."}, {"single_line", "false"}, {"vertical_text", "true"}, {"use_font_height", "false"}, {"word_spacing", "3"}, {"spacing_mul", "1.5"}, {"spacing_add", "12"}, {"text_align", "right,top"}});
    ui::Attach(p1143, p1144);

    ui::Attach(p1132, p1143);

    ui::Attach(p1104, p1132);

    ui::Attach(p1082, p1104);

    ui::Attach(p1080, p1082);

    ui::Attach(p22, p1080);

    auto* p1145 = ui::Create<ui::VBox>(pWindow, {{"name", "main_view_page_05"}});
    auto* p1146 = ui::Create<ui::Label>(pWindow, {{"text", "Page 6: IRender image drawing test (DrawImage, BitBlt, StretchBlt, AlphaBlend, nine-patch, tiling, etc.)"}});
    ui::Attach(p1145, p1146);

    ui::Attach(p22, p1145);

    auto* p1147 = ui::Create<ui::VBox>(pWindow, {{"name", "main_view_page_06"}});
    auto* p1148 = ui::Create<ui::Label>(pWindow, {{"text", "Page 7: IRender basic drawing test (lines, rectangles, filled rectangles)"}});
    ui::Attach(p1147, p1148);

    ui::Attach(p22, p1147);

    auto* p1149 = ui::Create<ui::VBox>(pWindow, {{"name", "main_view_page_07"}});
    auto* p1150 = ui::Create<ui::Label>(pWindow, {{"text", "Page 8: DrawBoxShadow Test"}});
    ui::Attach(p1149, p1150);

    auto* p1151 = ui::Create<ui::VBox>(pWindow, {{"bkcolor", "white"}, {"alpha", "255"}});
    auto* p1152 = ui::Create<ui::HBox>(pWindow, {{"height", "40"}});
    ui::Attach(p1151, p1152);

    auto* p1153 = ui::Create<ui::HBox>(pWindow, {{"height", "auto"}, {"padding", "0,0,0,18"}});
    auto* p1154 = ui::Create<ui::Control>(pWindow, {{"margin", "50,40"}, {"width", "100"}, {"height", "60"}, {"name", "box_shadow_test"}, {"bkcolor", "red"}, {"border_round", "4,4"}, {"box_shadow", "color='gray' offset='1,1' blur_radius='3' spread_radius='3'"}, {"visible", "false"}});
    ui::Attach(p1153, p1154);

    auto* p1155 = ui::Create<ui::Control>(pWindow, {{"margin", "50,40"}, {"width", "100"}, {"height", "60"}, {"border_round", "0,0"}, {"border_color", "blue"}, {"border_size", "1,1,1,1"}, {"bkcolor", "green"}, {"box_shadow", "color='gray' offset='3,3' blur_radius='2' spread_radius='2'"}});
    ui::Attach(p1153, p1155);

    auto* p1156 = ui::Create<ui::Label>(pWindow, {{"margin", "50,40"}, {"width", "240"}, {"height", "100"}, {"bkcolor", "Sienna"}, {"border_round", "12,12"}, {"box_shadow", "color='red' offset='0,0' blur_radius='8' spread_radius='3'"}, {"text", "This is a translucent control"}, {"alpha", "96"}, {"text_align", "hcenter,vcenter"}});
    ui::Attach(p1153, p1156);

    auto* p1157 = ui::Create<ui::Label>(pWindow, {{"margin", "50,40"}, {"width", "600"}, {"height", "100"}, {"border_round", "0,0"}, {"border_color", "blue"}, {"border_size", "1,1,1,1"}, {"bkcolor", "green"}, {"box_shadow", "color='red' offset='1,1' blur_radius='4' spread_radius='1'"}, {"text", "box-shadow usage notes: (1) X and Y expansion range: blur_radius + spread_radius + offset (2) The shadow is drawn beyond the control bounds; the expansion beyond the control rect is as above (3) When setting layout properties, leave drawing space around (e.g. margins) to avoid the shadow being covered"}, {"replace_newline", "true"}, {"text_padding", "2,0,0,0"}});
    ui::Attach(p1153, p1157);

    ui::Attach(p1151, p1153);

    auto* p1158 = ui::Create<ui::HBox>(pWindow, {{"height", "40"}});
    auto* p1159 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_color_gray"}, {"text", "Mouse over shows the control above"}, {"width", "180"}, {"height", "32"}, {"border_round", "3,3"}, {"margin", "20,2,0,0"}, {"valign", "center"}});
    ui::Attach(p1158, p1159);

    ui::Attach(p1151, p1158);

    auto* p1160 = ui::Create<ui::HBox>(pWindow, {{"margin", "20,20,0,0"}, {"height", "70"}, {"box_shadow", "color='red' offset='1,1' blur_radius='4' spread_radius='1'"}, {"margin", "6,6,6,6"}});
    auto* p1161 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"margin", "10,0,0,0"}, {"border_size", "2"}, {"border_color", "gray"}, {"valign", "center"}});
    ui::Attach(p1160, p1161);

    auto* p1162 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"margin", "10,0,0,0"}, {"border_size", "2"}, {"border_color", "gray"}, {"border_dash_style", "solid"}, {"valign", "center"}});
    ui::Attach(p1160, p1162);

    auto* p1163 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"margin", "10,0,0,0"}, {"border_size", "2"}, {"border_color", "gray"}, {"border_dash_style", "dash"}, {"valign", "center"}});
    ui::Attach(p1160, p1163);

    auto* p1164 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"margin", "10,0,0,0"}, {"border_size", "2"}, {"border_color", "gray"}, {"border_dash_style", "dot"}, {"valign", "center"}});
    ui::Attach(p1160, p1164);

    auto* p1165 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"margin", "10,0,0,0"}, {"border_size", "2"}, {"border_color", "gray"}, {"border_dash_style", "dash_dot"}, {"valign", "center"}});
    ui::Attach(p1160, p1165);

    auto* p1166 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"margin", "10,0,0,0"}, {"border_size", "2"}, {"border_color", "gray"}, {"border_dash_style", "dash_dot_dot"}, {"valign", "center"}});
    ui::Attach(p1160, p1166);

    ui::Attach(p1151, p1160);

    auto* p1167 = ui::Create<ui::HBox>(pWindow, {{"margin", "20,20,0,0"}, {"height", "64"}});
    auto* p1168 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"margin", "10,0,0,0"}, {"bkcolor", "blue"}, {"border_size", "2"}, {"border_color", "gold"}});
    ui::Attach(p1167, p1168);

    auto* p1169 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"margin", "10,0,0,0"}, {"bkcolor", "blue"}, {"border_size", "2"}, {"border_color", "gold"}, {"border_dash_style", "solid"}});
    ui::Attach(p1167, p1169);

    auto* p1170 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"margin", "10,0,0,0"}, {"bkcolor", "blue"}, {"border_size", "2"}, {"border_color", "gold"}, {"border_dash_style", "dash"}});
    ui::Attach(p1167, p1170);

    auto* p1171 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"margin", "10,0,0,0"}, {"bkcolor", "blue"}, {"border_size", "2"}, {"border_color", "gold"}, {"border_dash_style", "dot"}});
    ui::Attach(p1167, p1171);

    auto* p1172 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"margin", "10,0,0,0"}, {"bkcolor", "blue"}, {"border_size", "2"}, {"border_color", "gold"}, {"border_dash_style", "dash_dot"}});
    ui::Attach(p1167, p1172);

    auto* p1173 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"margin", "10,0,0,0"}, {"bkcolor", "blue"}, {"border_size", "2"}, {"border_color", "gold"}, {"border_dash_style", "dash_dot_dot"}});
    ui::Attach(p1167, p1173);

    ui::Attach(p1151, p1167);

    auto* p1174 = ui::Create<ui::HBox>(pWindow, {{"margin", "20,2,0,0"}, {"height", "64"}});
    auto* p1175 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"border_round", "8,8"}, {"margin", "10,0,0,0"}, {"border_size", "2"}, {"border_color", "gray"}});
    ui::Attach(p1174, p1175);

    auto* p1176 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"border_round", "8,8"}, {"margin", "10,0,0,0"}, {"border_size", "2"}, {"border_color", "gray"}, {"border_dash_style", "solid"}});
    ui::Attach(p1174, p1176);

    auto* p1177 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"border_round", "8,8"}, {"margin", "10,0,0,0"}, {"border_size", "2"}, {"border_color", "gray"}, {"border_dash_style", "dash"}});
    ui::Attach(p1174, p1177);

    auto* p1178 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"border_round", "8,8"}, {"margin", "10,0,0,0"}, {"border_size", "2"}, {"border_color", "gray"}, {"border_dash_style", "dot"}});
    ui::Attach(p1174, p1178);

    auto* p1179 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"border_round", "8,8"}, {"margin", "10,0,0,0"}, {"border_size", "2"}, {"border_color", "gray"}, {"border_dash_style", "dash_dot"}});
    ui::Attach(p1174, p1179);

    auto* p1180 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"border_round", "8,8"}, {"margin", "10,0,0,0"}, {"border_size", "2"}, {"border_color", "gray"}, {"border_dash_style", "dash_dot_dot"}});
    ui::Attach(p1174, p1180);

    ui::Attach(p1151, p1174);

    auto* p1181 = ui::Create<ui::HBox>(pWindow, {{"margin", "20,2,0,0"}, {"height", "64"}});
    auto* p1182 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"border_round", "8,8"}, {"margin", "10,0,0,0"}, {"bkcolor", "blue"}, {"border_size", "2"}, {"border_color", "gold"}});
    ui::Attach(p1181, p1182);

    auto* p1183 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"border_round", "8,8"}, {"margin", "10,0,0,0"}, {"bkcolor", "blue"}, {"border_size", "2"}, {"border_color", "gold"}, {"border_dash_style", "solid"}});
    ui::Attach(p1181, p1183);

    auto* p1184 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"border_round", "8,8"}, {"margin", "10,0,0,0"}, {"bkcolor", "blue"}, {"border_size", "2"}, {"border_color", "gold"}, {"border_dash_style", "dash"}});
    ui::Attach(p1181, p1184);

    auto* p1185 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"border_round", "8,8"}, {"margin", "10,0,0,0"}, {"bkcolor", "blue"}, {"border_size", "2"}, {"border_color", "gold"}, {"border_dash_style", "dot"}});
    ui::Attach(p1181, p1185);

    auto* p1186 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"border_round", "8,8"}, {"margin", "10,0,0,0"}, {"bkcolor", "blue"}, {"border_size", "2"}, {"border_color", "gold"}, {"border_dash_style", "dash_dot"}});
    ui::Attach(p1181, p1186);

    auto* p1187 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"border_round", "8,8"}, {"margin", "10,0,0,0"}, {"bkcolor", "blue"}, {"border_size", "2"}, {"border_color", "gold"}, {"border_dash_style", "dash_dot_dot"}});
    ui::Attach(p1181, p1187);

    ui::Attach(p1151, p1181);

    auto* p1188 = ui::Create<ui::HBox>(pWindow, {{"margin", "20,2,0,0"}, {"height", "64"}});
    auto* p1189 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"border_round", "8,8"}, {"margin", "10,0,0,0"}, {"border_size", "2,1,2,1"}, {"border_color", "gray"}});
    ui::Attach(p1188, p1189);

    auto* p1190 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"border_round", "8,8"}, {"margin", "10,0,0,0"}, {"border_size", "2,1,2,1"}, {"border_color", "gray"}, {"border_dash_style", "solid"}});
    ui::Attach(p1188, p1190);

    auto* p1191 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"border_round", "8,8"}, {"margin", "10,0,0,0"}, {"border_size", "2,1,2,1"}, {"border_color", "gray"}, {"border_dash_style", "dash"}});
    ui::Attach(p1188, p1191);

    auto* p1192 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"border_round", "8,8"}, {"margin", "10,0,0,0"}, {"border_size", "2,1,2,1"}, {"border_color", "gray"}, {"border_dash_style", "dot"}});
    ui::Attach(p1188, p1192);

    auto* p1193 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"border_round", "8,8"}, {"margin", "10,0,0,0"}, {"border_size", "2,1,2,1"}, {"border_color", "gray"}, {"border_dash_style", "dash_dot"}});
    ui::Attach(p1188, p1193);

    auto* p1194 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"border_round", "8,8"}, {"margin", "10,0,0,0"}, {"border_size", "2,1,2,1"}, {"border_color", "gray"}, {"border_dash_style", "dash_dot_dot"}});
    ui::Attach(p1188, p1194);

    ui::Attach(p1151, p1188);

    auto* p1195 = ui::Create<ui::HBox>(pWindow, {{"margin", "20,2,0,0"}, {"height", "64"}});
    auto* p1196 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"border_round", "8,8"}, {"margin", "10,0,0,0"}, {"bkcolor", "blue"}, {"border_size", "2,1,2,1"}, {"border_color", "gold"}});
    ui::Attach(p1195, p1196);

    auto* p1197 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"border_round", "8,8"}, {"margin", "10,0,0,0"}, {"bkcolor", "blue"}, {"border_size", "2,1,2,1"}, {"border_color", "gold"}, {"border_dash_style", "solid"}});
    ui::Attach(p1195, p1197);

    auto* p1198 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"border_round", "8,8"}, {"margin", "10,0,0,0"}, {"bkcolor", "blue"}, {"border_size", "2,1,2,1"}, {"border_color", "gold"}, {"border_dash_style", "dash"}});
    ui::Attach(p1195, p1198);

    auto* p1199 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"border_round", "8,8"}, {"margin", "10,0,0,0"}, {"bkcolor", "blue"}, {"border_size", "2,1,2,1"}, {"border_color", "gold"}, {"border_dash_style", "dot"}});
    ui::Attach(p1195, p1199);

    auto* p1200 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"border_round", "8,8"}, {"margin", "10,0,0,0"}, {"bkcolor", "blue"}, {"border_size", "2,1,2,1"}, {"border_color", "gold"}, {"border_dash_style", "dash_dot"}});
    ui::Attach(p1195, p1200);

    auto* p1201 = ui::Create<ui::Control>(pWindow, {{"width", "80"}, {"height", "60"}, {"border_round", "8,8"}, {"margin", "10,0,0,0"}, {"bkcolor", "blue"}, {"border_size", "2,1,2,1"}, {"border_color", "gold"}, {"border_dash_style", "dash_dot_dot"}});
    ui::Attach(p1195, p1201);

    ui::Attach(p1151, p1195);

    ui::Attach(p1149, p1151);

    ui::Attach(p22, p1149);

    auto* p1202 = ui::Create<ui::VBox>(pWindow, {{"name", "main_view_page_08"}});
    auto* p1203 = ui::Create<ui::HBox>(pWindow, {});
    auto* p1204 = ui::Create<ui::PropertyGrid>(pWindow, {{"name", "property_grid_test"}, {"bkcolor", "YellowGreen"}, {"width", "320"}, {"left_column_width", "130"}, {"row_grid_line_width", "1"}, {"row_grid_line_color", "darkgray"}, {"column_grid_line_width", "1"}, {"column_grid_line_color", "darkgray"}, {"header_class", "property_grid_header"}, {"group_class", "property_grid_group"}, {"propterty_class", "property_grid_propterty"}});
    ui::Attach(p1203, p1204);

    auto* p1205 = ui::Create<ui::Split>(pWindow, {{"bkcolor", "splitline_level1"}, {"width", "2"}});
    ui::Attach(p1203, p1205);

    auto* p1206 = ui::Create<ui::VBox>(pWindow, {});
    auto* p1207 = ui::Create<ui::HBox>(pWindow, {{"bkcolor", "white"}, {"height", "80"}, {"margin", "0,4,0,0"}});
    auto* p1208 = ui::Create<ui::TabCtrl>(pWindow, {{"class", "tab_ctrl"}, {"name", "tab_list1"}, {"margin", "10,10,10,10"}, {"drag_order", "true"}, {"drag_out_id", "1"}});
    auto* p1209 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Label 1-1"}, {"icon", "file='public/caption/logo.svg' valign='center' halign='center'"}});
    ui::Attach(p1208, p1209);

    auto* p1210 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Label 1-2"}, {"icon", "file='public/caption/logo.svg' valign='center' halign='center'"}});
    ui::Attach(p1208, p1210);

    auto* p1211 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Label 1-3"}, {"icon", "file='public/caption/logo.svg' valign='center' halign='center'"}});
    ui::Attach(p1208, p1211);

    auto* p1212 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Label 1-4"}, {"icon", "file='public/caption/logo.svg' valign='center' halign='center'"}});
    ui::Attach(p1208, p1212);

    ui::Attach(p1207, p1208);

    auto* p1213 = ui::Create<ui::TabCtrl>(pWindow, {{"class", "tab_ctrl"}, {"margin", "10,10,10,10"}, {"drag_order", "true"}, {"drop_in_id", "1"}});
    auto* p1214 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Label 2-1"}, {"icon", "file='public/caption/logo.svg' valign='center' halign='center'"}});
    ui::Attach(p1213, p1214);

    auto* p1215 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Label 2-2"}, {"icon", "file='public/caption/logo.svg' valign='center' halign='center'"}});
    ui::Attach(p1213, p1215);

    auto* p1216 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Label 2-3"}, {"icon", "file='public/caption/logo.svg' valign='center' halign='center'"}});
    ui::Attach(p1213, p1216);

    auto* p1217 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Label 2-4"}, {"icon", "file='public/caption/logo.svg' valign='center' halign='center'"}});
    ui::Attach(p1213, p1217);

    ui::Attach(p1207, p1213);

    ui::Attach(p1206, p1207);

    auto* p1218 = ui::Create<ui::HBox>(pWindow, {{"bkcolor", "white"}});
    auto* p1219 = ui::Create<ui::TabCtrl>(pWindow, {{"class", "tab_ctrl"}, {"margin", "10,10,10,10"}, {"drag_order", "false"}});
    auto* p1220 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Label 3-1: drag reorder not supported"}, {"icon", "file='public/caption/logo.svg' valign='center' halign='center'"}});
    ui::Attach(p1219, p1220);

    auto* p1221 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Label 3-2"}, {"icon", "file='public/caption/logo.svg' valign='center' halign='center'"}});
    ui::Attach(p1219, p1221);

    auto* p1222 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Label 3-3"}, {"icon", "file='public/caption/logo.svg' valign='center' halign='center'"}});
    ui::Attach(p1219, p1222);

    auto* p1223 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Label 3-4"}, {"icon", "file='public/caption/logo.svg' valign='center' halign='center'"}});
    ui::Attach(p1219, p1223);

    ui::Attach(p1218, p1219);

    auto* p1224 = ui::Create<ui::VListBox>(pWindow, {{"margin", "10,10,10,10"}, {"width", "128"}});
    auto* p1225 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"drag_order", "true"}, {"title", "Label - 1"}, {"icon", "file='public/caption/logo.svg' valign='center' halign='center'"}});
    ui::Attach(p1224, p1225);

    auto* p1226 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"drag_order", "true"}, {"title", "Label - 2"}, {"icon", "file='public/caption/logo.svg' valign='center' halign='center'"}});
    ui::Attach(p1224, p1226);

    auto* p1227 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"drag_order", "true"}, {"title", "Label - 3"}, {"icon", "file='public/caption/logo.svg' valign='center' halign='center'"}});
    ui::Attach(p1224, p1227);

    auto* p1228 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"drag_order", "true"}, {"title", "Label - 4"}, {"icon", "file='public/caption/logo.svg' valign='center' halign='center'"}});
    ui::Attach(p1224, p1228);

    ui::Attach(p1218, p1224);

    ui::Attach(p1206, p1218);

    auto* p1229 = ui::Create<ui::VBox>(pWindow, {});
    auto* p1230 = ui::Create<ui::VScrollBox>(pWindow, {{"vscrollbar", "true"}, {"hscrollbar", "true"}, {"width", "stretch"}, {"height", "stretch"}, {"bkcolor", "white"}, {"padding", "20,20,20,20"}});
    auto* p1231 = ui::Create<ui::TabCtrl>(pWindow, {{"class", "tab_ctrl"}, {"margin", "10,10,10,10"}});
    auto* p1232 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Label 3-1"}, {"icon", "file='public/caption/logo.svg' valign='center' halign='center'"}});
    ui::Attach(p1231, p1232);

    auto* p1233 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Label 3-2"}, {"icon", "file='public/caption/logo.svg' valign='center' halign='center'"}});
    ui::Attach(p1231, p1233);

    auto* p1234 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Label 3-3"}, {"icon", "file='public/caption/logo.svg' valign='center' halign='center'"}});
    ui::Attach(p1231, p1234);

    auto* p1235 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Label 3-4"}, {"icon", "file='public/caption/logo.svg' valign='center' halign='center'"}});
    ui::Attach(p1231, p1235);

    ui::Attach(p1230, p1231);

    auto* p1236 = ui::Create<ui::Control>(pWindow, {{"height", "300"}});
    ui::Attach(p1230, p1236);

    auto* p1237 = ui::Create<ui::TabCtrl>(pWindow, {{"class", "tab_ctrl"}, {"margin", "10,10,10,10"}});
    auto* p1238 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Label 3-1"}, {"icon", "file='public/caption/logo.svg' valign='center' halign='center'"}});
    ui::Attach(p1237, p1238);

    auto* p1239 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Label 3-2"}, {"icon", "file='public/caption/logo.svg' valign='center' halign='center'"}});
    ui::Attach(p1237, p1239);

    auto* p1240 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Label 3-3"}, {"icon", "file='public/caption/logo.svg' valign='center' halign='center'"}});
    ui::Attach(p1237, p1240);

    auto* p1241 = ui::Create<ui::TabCtrlItem>(pWindow, {{"class", "tab_ctrl_item"}, {"title", "Label 3-4"}, {"icon", "file='public/caption/logo.svg' valign='center' halign='center'"}});
    ui::Attach(p1237, p1241);

    ui::Attach(p1230, p1237);

    auto* p1242 = ui::Create<ui::Label>(pWindow, {{"text", "Vertical text test, a vertical text below needs to be displayed:"}, {"text_align", "hcenter,top"}});
    ui::Attach(p1230, p1242);

    auto* p1243 = ui::Create<ui::Label>(pWindow, {{"height", "120"}, {"width", "stretch"}, {"text", "Vertical text test: test whether it displays correctly inside the scrollbar."}, {"text_align", "hcenter,top"}, {"vertical_text", "true"}, {"multi_line", "true"}, {"replace_newline", "true"}});
    ui::Attach(p1230, p1243);

    ui::Attach(p1229, p1230);

    ui::Attach(p1206, p1229);

    auto* p1244 = ui::Create<ui::HBox>(pWindow, {{"bkcolor", "white"}, {"margin", "4,4,4,0"}});
    auto* p1245 = ui::Create<ui::Control>(pWindow, {{"bkcolor", "blue"}, {"bkcolor2", "red"}, {"bkcolor2_direction", "1"}, {"margin", "0,0,4,0"}});
    ui::Attach(p1244, p1245);

    auto* p1246 = ui::Create<ui::Control>(pWindow, {{"bkcolor", "blue"}, {"bkcolor2", "red"}, {"bkcolor2_direction", "2"}, {"margin", "0,0,4,0"}});
    ui::Attach(p1244, p1246);

    auto* p1247 = ui::Create<ui::Control>(pWindow, {{"bkcolor", "blue"}, {"bkcolor2", "red"}, {"bkcolor2_direction", "3"}, {"margin", "0,0,4,0"}});
    ui::Attach(p1244, p1247);

    auto* p1248 = ui::Create<ui::Control>(pWindow, {{"bkcolor", "blue"}, {"bkcolor2", "red"}, {"bkcolor2_direction", "4"}});
    ui::Attach(p1244, p1248);

    ui::Attach(p1206, p1244);

    auto* p1249 = ui::Create<ui::HBox>(pWindow, {{"bkcolor", "white"}, {"margin", "4,4,4,4"}});
    auto* p1250 = ui::Create<ui::Label>(pWindow, {{"width", "stretch"}, {"height", "stretch"}, {"text_align", "hcenter,vcenter"}, {"text", "This demonstrates the gradient background color feature"}, {"border_round", "8,8"}, {"bkcolor", "blue"}, {"bkcolor2", "red"}, {"bkcolor2_direction", "1"}, {"margin", "0,0,4,0"}});
    ui::Attach(p1249, p1250);

    auto* p1251 = ui::Create<ui::Control>(pWindow, {{"border_round", "8,8"}, {"bkcolor", "blue"}, {"bkcolor2", "red"}, {"bkcolor2_direction", "2"}, {"margin", "0,0,4,0"}});
    ui::Attach(p1249, p1251);

    auto* p1252 = ui::Create<ui::Control>(pWindow, {{"border_round", "8,8"}, {"bkcolor", "blue"}, {"bkcolor2", "red"}, {"bkcolor2_direction", "3"}, {"margin", "0,0,4,0"}});
    ui::Attach(p1249, p1252);

    auto* p1253 = ui::Create<ui::Control>(pWindow, {{"border_round", "8,8"}, {"bkcolor", "blue"}, {"bkcolor2", "red"}, {"bkcolor2_direction", "4"}});
    ui::Attach(p1249, p1253);

    ui::Attach(p1206, p1249);

    ui::Attach(p1203, p1206);

    ui::Attach(p1202, p1203);

    ui::Attach(p22, p1202);

    auto* p1254 = ui::Create<ui::VBox>(pWindow, {{"name", "main_view_page_09"}});
    auto* p1255 = ui::Create<ui::Label>(pWindow, {{"text", "Page 10: Window Shadow Test"}});
    ui::Attach(p1254, p1255);

    pWindow->AddClass("page_window_shadow_vbox", " width=\"180\" height=\"200\"");
    pWindow->AddClass("page_window_shadow_control", " width=\"180\" height=\"100%\"");
    pWindow->AddClass("page_window_shadow_label", " width=\"100%\" height=\"20\" text_align=\"hcenter\" margin=\"8\"");
    auto* p1256 = ui::Create<ui::VBox>(pWindow, {});
    auto* p1257 = ui::Create<ui::HBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p1258 = ui::Create<ui::VBox>(pWindow, {{"class", "page_window_shadow_vbox"}});
    auto* p1259 = ui::Create<ui::Control>(pWindow, {{"class", "page_window_shadow_control"}, {"bkimage", "file='public/shadow/shadow_big.svg' width='180' valign='center' halign='center'"}});
    ui::Attach(p1258, p1259);

    auto* p1260 = ui::Create<ui::Label>(pWindow, {{"class", "page_window_shadow_label"}, {"text", "Window Shadow: big"}});
    ui::Attach(p1258, p1260);

    ui::Attach(p1257, p1258);

    auto* p1261 = ui::Create<ui::VBox>(pWindow, {{"class", "page_window_shadow_vbox"}});
    auto* p1262 = ui::Create<ui::Control>(pWindow, {{"class", "page_window_shadow_control"}, {"bkimage", "file='public/shadow/shadow_big_round.svg' width='180' valign='center' halign='center'"}});
    ui::Attach(p1261, p1262);

    auto* p1263 = ui::Create<ui::Label>(pWindow, {{"class", "page_window_shadow_label"}, {"text", "Window Shadow: big_round"}});
    ui::Attach(p1261, p1263);

    ui::Attach(p1257, p1261);

    auto* p1264 = ui::Create<ui::VBox>(pWindow, {{"class", "page_window_shadow_vbox"}});
    auto* p1265 = ui::Create<ui::Control>(pWindow, {{"class", "page_window_shadow_control"}, {"bkimage", "file='public/shadow/shadow_small.svg' width='180' valign='center' halign='center'"}});
    ui::Attach(p1264, p1265);

    auto* p1266 = ui::Create<ui::Label>(pWindow, {{"class", "page_window_shadow_label"}, {"text", "Window Shadow: small"}});
    ui::Attach(p1264, p1266);

    ui::Attach(p1257, p1264);

    auto* p1267 = ui::Create<ui::VBox>(pWindow, {{"class", "page_window_shadow_vbox"}});
    auto* p1268 = ui::Create<ui::Control>(pWindow, {{"class", "page_window_shadow_control"}, {"bkimage", "file='public/shadow/shadow_small_round.svg' width='180' valign='center' halign='center'"}});
    ui::Attach(p1267, p1268);

    auto* p1269 = ui::Create<ui::Label>(pWindow, {{"class", "page_window_shadow_label"}, {"text", "Window Shadow: small_round"}});
    ui::Attach(p1267, p1269);

    ui::Attach(p1257, p1267);

    auto* p1270 = ui::Create<ui::VBox>(pWindow, {{"class", "page_window_shadow_vbox"}});
    auto* p1271 = ui::Create<ui::Control>(pWindow, {{"class", "page_window_shadow_control"}, {"bkimage", "file='public/shadow/shadow_menu.svg' width='180' valign='center' halign='center'"}});
    ui::Attach(p1270, p1271);

    auto* p1272 = ui::Create<ui::Label>(pWindow, {{"class", "page_window_shadow_label"}, {"text", "Window Shadow: menu"}});
    ui::Attach(p1270, p1272);

    ui::Attach(p1257, p1270);

    auto* p1273 = ui::Create<ui::VBox>(pWindow, {{"class", "page_window_shadow_vbox"}});
    auto* p1274 = ui::Create<ui::Control>(pWindow, {{"class", "page_window_shadow_control"}, {"bkimage", "file='public/shadow/shadow_menu_round.svg' width='180' valign='center' halign='center'"}});
    ui::Attach(p1273, p1274);

    auto* p1275 = ui::Create<ui::Label>(pWindow, {{"class", "page_window_shadow_label"}, {"text", "Window Shadow: menu_round"}});
    ui::Attach(p1273, p1275);

    ui::Attach(p1257, p1273);

    ui::Attach(p1256, p1257);

    auto* p1276 = ui::Create<ui::VBox>(pWindow, {});
    auto* p1277 = ui::Create<ui::Label>(pWindow, {{"text", "Window Shadow Function Test:"}, {"valign", "center"}, {"margin", "12,30,10,0"}});
    ui::Attach(p1276, p1277);

    auto* p1278 = ui::Create<ui::Option>(pWindow, {{"class", "checkbox_2"}, {"group", "shadow_type"}, {"text", "Shadow type: default, default shadow"}, {"selected", "true"}, {"valign", "center"}, {"margin", "30,4,0,2"}});
    ui::Attach(p1276, p1278);

    auto* p1279 = ui::Create<ui::Option>(pWindow, {{"class", "checkbox_2"}, {"group", "shadow_type"}, {"text", "Shadow type: big, large shadow, square corners (suitable for normal windows)"}, {"selected", "false"}, {"valign", "center"}, {"margin", "30,4,0,2"}});
    ui::Attach(p1276, p1279);

    auto* p1280 = ui::Create<ui::Option>(pWindow, {{"class", "checkbox_2"}, {"group", "shadow_type"}, {"text", "Shadow type: big_round, large shadow, rounded corners (suitable for normal windows)"}, {"selected", "false"}, {"valign", "center"}, {"margin", "30,4,0,2"}});
    ui::Attach(p1276, p1280);

    auto* p1281 = ui::Create<ui::Option>(pWindow, {{"class", "checkbox_2"}, {"group", "shadow_type"}, {"text", "Shadow type: small, small shadow, square corners (suitable for normal windows)"}, {"selected", "false"}, {"valign", "center"}, {"margin", "30,4,0,2"}});
    ui::Attach(p1276, p1281);

    auto* p1282 = ui::Create<ui::Option>(pWindow, {{"class", "checkbox_2"}, {"group", "shadow_type"}, {"text", "Shadow type: small_round, small shadow, rounded corners (suitable for normal windows)"}, {"selected", "false"}, {"valign", "center"}, {"margin", "30,4,0,2"}});
    ui::Attach(p1276, p1282);

    auto* p1283 = ui::Create<ui::Option>(pWindow, {{"class", "checkbox_2"}, {"group", "shadow_type"}, {"text", "Shadow type: menu, small shadow, square corners (suitable for popup windows such as menus)"}, {"selected", "false"}, {"valign", "center"}, {"margin", "30,4,0,2"}});
    ui::Attach(p1276, p1283);

    auto* p1284 = ui::Create<ui::Option>(pWindow, {{"class", "checkbox_2"}, {"group", "shadow_type"}, {"text", "Shadow type: menu_round, small shadow, rounded corners (suitable for popup windows such as menus)"}, {"selected", "false"}, {"valign", "center"}, {"margin", "30,4,0,2"}});
    ui::Attach(p1276, p1284);

    auto* p1285 = ui::Create<ui::Option>(pWindow, {{"class", "checkbox_2"}, {"group", "shadow_type"}, {"text", "Shadow type: none, no shadow, with border, square corners"}, {"selected", "false"}, {"valign", "center"}, {"margin", "30,4,0,2"}});
    ui::Attach(p1276, p1285);

    auto* p1286 = ui::Create<ui::Option>(pWindow, {{"class", "checkbox_2"}, {"group", "shadow_type"}, {"text", "Shadow type: none_round, no shadow, with border, rounded corners"}, {"selected", "false"}, {"valign", "center"}, {"margin", "30,4,0,2"}});
    ui::Attach(p1276, p1286);

    auto* p1287 = ui::Create<ui::Option>(pWindow, {{"class", "checkbox_2"}, {"group", "shadow_type"}, {"text", "Disable Shadow"}, {"selected", "false"}, {"valign", "center"}, {"margin", "30,4,0,2"}});
    ui::Attach(p1276, p1287);

    ui::Attach(p1256, p1276);

    ui::Attach(p1254, p1256);

    ui::Attach(p22, p1254);

    auto* p1288 = ui::Create<ui::VBox>(pWindow, {{"name", "main_view_page_10"}});
    auto* p1289 = ui::Create<ui::Label>(pWindow, {{"text", "Page 11: Bitmap Control Test"}});
    ui::Attach(p1288, p1289);

    pWindow->AddClass("bitmap_control_image", " width=\"200\" height=\"200\" border_size=\"1\" border_color=\"green\"");
    pWindow->AddClass("bitmap_control_text", " width=\"200\" height=\"auto\" multi_line=\"true\" replace_newline=\"true\" text_align=\"left,vcenter\"");
    pWindow->AddClass("bitmap_control_case", " width=\"200\" height=\"auto\" multi_line=\"true\" replace_newline=\"true\" text_align=\"hcenter,vcenter\"");
    auto* p1290 = ui::Create<ui::HFlowScrollBox>(pWindow, {{"width", "auto"}, {"height", "auto"}, {"child_margin_x", "2"}, {"child_margin_y", "2"}, {"padding", "2,2,2,2"}});
    auto* p1291 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p1292 = ui::Create<ui::BitmapControl>(pWindow, {{"class", "bitmap_control_image"}, {"bitmap_file", "render/autumn.png"}});
    ui::Attach(p1291, p1292);

    auto* p1293 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_case"}, {"text", "Test Case (1.1)"}});
    ui::Attach(p1291, p1293);

    auto* p1294 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_text"}, {"text", "Default: Large Area"}, {"text_align", "hcenter,vcenter"}});
    ui::Attach(p1291, p1294);

    ui::Attach(p1290, p1291);

    auto* p1295 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p1296 = ui::Create<ui::BitmapControl>(pWindow, {{"class", "bitmap_control_image"}, {"width", "100"}, {"height", "100"}, {"bitmap_file", "render/autumn.png"}});
    ui::Attach(p1295, p1296);

    auto* p1297 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_case"}, {"text", "Test Case (1.1)"}});
    ui::Attach(p1295, p1297);

    auto* p1298 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_text"}, {"text", "Default: Small Area"}, {"text_align", "hcenter,vcenter"}});
    ui::Attach(p1295, p1298);

    ui::Attach(p1290, p1295);

    auto* p1299 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p1300 = ui::Create<ui::BitmapControl>(pWindow, {{"class", "bitmap_control_image"}, {"width", "auto"}, {"height", "auto"}, {"bitmap_file", "render/autumn.png"}});
    ui::Attach(p1299, p1300);

    auto* p1301 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_case"}, {"text", "Test Case (1.1)"}});
    ui::Attach(p1299, p1301);

    auto* p1302 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_text"}, {"text", "Default: auto"}, {"text_align", "hcenter,vcenter"}});
    ui::Attach(p1299, p1302);

    ui::Attach(p1290, p1299);

    auto* p1303 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p1304 = ui::Create<ui::BitmapControl>(pWindow, {{"class", "bitmap_control_image"}, {"bitmap_file", "render/autumn.png"}, {"bitmap_halign", "left"}, {"bitmap_valign", "top"}});
    ui::Attach(p1303, p1304);

    auto* p1305 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_case"}, {"text", "Test Case (1.2)"}});
    ui::Attach(p1303, p1305);

    auto* p1306 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_text"}, {"text", "bitmap_halign='left'\\nbitmap_valign='top'"}});
    ui::Attach(p1303, p1306);

    ui::Attach(p1290, p1303);

    auto* p1307 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p1308 = ui::Create<ui::BitmapControl>(pWindow, {{"class", "bitmap_control_image"}, {"bitmap_file", "render/autumn.png"}, {"bitmap_halign", "center"}, {"bitmap_valign", "center"}});
    ui::Attach(p1307, p1308);

    auto* p1309 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_case"}, {"text", "Test Case (1.3)"}});
    ui::Attach(p1307, p1309);

    auto* p1310 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_text"}, {"text", "bitmap_halign='center'\\nbitmap_valign='center'"}});
    ui::Attach(p1307, p1310);

    ui::Attach(p1290, p1307);

    auto* p1311 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p1312 = ui::Create<ui::BitmapControl>(pWindow, {{"class", "bitmap_control_image"}, {"bitmap_file", "render/autumn.png"}, {"bitmap_halign", "right"}, {"bitmap_valign", "bottom"}});
    ui::Attach(p1311, p1312);

    auto* p1313 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_case"}, {"text", "Test Case (1.4)"}});
    ui::Attach(p1311, p1313);

    auto* p1314 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_text"}, {"text", "bitmap_halign='right'\\nbitmap_valign='bottom'"}});
    ui::Attach(p1311, p1314);

    ui::Attach(p1290, p1311);

    auto* p1315 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p1316 = ui::Create<ui::BitmapControl>(pWindow, {{"class", "bitmap_control_image"}, {"bitmap_file", "render/autumn.png"}, {"bitmap_halign", "center"}, {"bitmap_valign", "center"}, {"bitmap_alpha", "128"}});
    ui::Attach(p1315, p1316);

    auto* p1317 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_case"}, {"text", "Test Case (1.5)"}});
    ui::Attach(p1315, p1317);

    auto* p1318 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_text"}, {"text", "bitmap_alpha='128'"}, {"text_align", "hcenter,vcenter"}});
    ui::Attach(p1315, p1318);

    ui::Attach(p1290, p1315);

    auto* p1319 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p1320 = ui::Create<ui::BitmapControl>(pWindow, {{"class", "bitmap_control_image"}, {"bitmap_file", "render/autumn.png"}, {"bitmap_halign", "center"}, {"bitmap_valign", "center"}, {"bitmap_margin", "10,20,40,60"}});
    ui::Attach(p1319, p1320);

    auto* p1321 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_case"}, {"text", "Test Case (1.6)"}});
    ui::Attach(p1319, p1321);

    auto* p1322 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_text"}, {"text", "bitmap_margin='10,20,40,60'"}, {"text_align", "hcenter,vcenter"}});
    ui::Attach(p1319, p1322);

    ui::Attach(p1290, p1319);

    auto* p1323 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p1324 = ui::Create<ui::BitmapControl>(pWindow, {{"class", "bitmap_control_image"}, {"bitmap_file", "render/autumn.png"}, {"bitmap_stretch", "true"}});
    ui::Attach(p1323, p1324);

    auto* p1325 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_case"}, {"text", "Test Case (1.7)"}});
    ui::Attach(p1323, p1325);

    auto* p1326 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_text"}, {"text", "bitmap_stretch='true'"}, {"text_align", "hcenter,vcenter"}});
    ui::Attach(p1323, p1326);

    ui::Attach(p1290, p1323);

    auto* p1327 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p1328 = ui::Create<ui::BitmapControl>(pWindow, {{"class", "bitmap_control_image"}, {"bitmap_file", "render/autumn.png"}, {"bitmap_adaptive_dest_rect", "true"}});
    ui::Attach(p1327, p1328);

    auto* p1329 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_case"}, {"text", "Test Case (1.8)"}});
    ui::Attach(p1327, p1329);

    auto* p1330 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_text"}, {"text", "bitmap_adaptive_dest_rect='true'"}, {"text_align", "hcenter,vcenter"}});
    ui::Attach(p1327, p1330);

    ui::Attach(p1290, p1327);

    auto* p1331 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p1332 = ui::Create<ui::BitmapControl>(pWindow, {{"class", "bitmap_control_image"}, {"width", "160"}, {"height", "160"}, {"bitmap_file", "render/autumn.png"}, {"bitmap_adaptive_dest_rect", "true"}});
    ui::Attach(p1331, p1332);

    auto* p1333 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_case"}, {"text", "Test Case (1.9)"}});
    ui::Attach(p1331, p1333);

    auto* p1334 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_text"}, {"text", "bitmap_adaptive_dest_rect='true'"}, {"text_align", "hcenter,vcenter"}});
    ui::Attach(p1331, p1334);

    ui::Attach(p1290, p1331);

    auto* p1335 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p1336 = ui::Create<ui::BitmapControl>(pWindow, {{"class", "bitmap_control_image"}, {"width", "160"}, {"height", "160"}, {"bitmap_file", "render/autumn.png"}, {"bitmap_adaptive_dest_rect", "true"}, {"bitmap_halign", "center"}});
    ui::Attach(p1335, p1336);

    auto* p1337 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_case"}, {"text", "Test Case (1.10)"}});
    ui::Attach(p1335, p1337);

    auto* p1338 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_text"}, {"text", "bitmap_adaptive_dest_rect='true'\\nbitmap_halign='center'"}, {"text_align", "hcenter,vcenter"}});
    ui::Attach(p1335, p1338);

    ui::Attach(p1290, p1335);

    auto* p1339 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p1340 = ui::Create<ui::BitmapControl>(pWindow, {{"class", "bitmap_control_image"}, {"width", "160"}, {"height", "160"}, {"bitmap_file", "render/autumn.png"}, {"bitmap_adaptive_dest_rect", "true"}, {"bitmap_halign", "right"}});
    ui::Attach(p1339, p1340);

    auto* p1341 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_case"}, {"text", "Test Case (1.11)"}});
    ui::Attach(p1339, p1341);

    auto* p1342 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_text"}, {"text", "bitmap_adaptive_dest_rect='true'\\nbitmap_halign='right'"}, {"text_align", "hcenter,vcenter"}});
    ui::Attach(p1339, p1342);

    ui::Attach(p1290, p1339);

    auto* p1343 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p1344 = ui::Create<ui::BitmapControl>(pWindow, {{"class", "bitmap_control_image"}, {"bitmap_file", "render/autumn.png"}, {"bitmap_src", "80,80,160,160"}});
    ui::Attach(p1343, p1344);

    auto* p1345 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_case"}, {"text", "Test Case (1.12)"}});
    ui::Attach(p1343, p1345);

    auto* p1346 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_text"}, {"text", "bitmap_src='80,80,160,160'"}, {"text_align", "hcenter,vcenter"}});
    ui::Attach(p1343, p1346);

    ui::Attach(p1290, p1343);

    auto* p1347 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p1348 = ui::Create<ui::BitmapControl>(pWindow, {{"class", "bitmap_control_image"}, {"width", "auto"}, {"height", "auto"}, {"bitmap_file", "render/autumn.png"}, {"bitmap_src", "80,80,160,160"}});
    ui::Attach(p1347, p1348);

    auto* p1349 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_case"}, {"text", "Test Case (1.12)"}});
    ui::Attach(p1347, p1349);

    auto* p1350 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_text"}, {"text", "bitmap_src='80,80,160,160' auto"}, {"text_align", "hcenter,vcenter"}});
    ui::Attach(p1347, p1350);

    ui::Attach(p1290, p1347);

    auto* p1351 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p1352 = ui::Create<ui::BitmapControl>(pWindow, {{"class", "bitmap_control_image"}, {"bitmap_file", "render/autumn.png"}, {"bitmap_dest", "80,80,160,160"}});
    ui::Attach(p1351, p1352);

    auto* p1353 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_case"}, {"text", "Test Case (1.13)"}});
    ui::Attach(p1351, p1353);

    auto* p1354 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_text"}, {"text", "bitmap_dest='80,80,160,160'"}, {"text_align", "hcenter,vcenter"}});
    ui::Attach(p1351, p1354);

    ui::Attach(p1290, p1351);

    auto* p1355 = ui::Create<ui::VBox>(pWindow, {{"width", "auto"}, {"height", "auto"}});
    auto* p1356 = ui::Create<ui::BitmapControl>(pWindow, {{"class", "bitmap_control_image"}, {"width", "auto"}, {"height", "auto"}, {"bitmap_file", "render/autumn.png"}, {"bitmap_dest", "80,80,160,160"}});
    ui::Attach(p1355, p1356);

    auto* p1357 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_case"}, {"text", "Test Case (1.13)"}});
    ui::Attach(p1355, p1357);

    auto* p1358 = ui::Create<ui::Label>(pWindow, {{"class", "bitmap_control_text"}, {"text", "bitmap_dest='80,80,160,160' auto"}, {"text_align", "hcenter,vcenter"}});
    ui::Attach(p1355, p1358);

    ui::Attach(p1290, p1355);

    ui::Attach(p1288, p1290);

    ui::Attach(p22, p1288);

    auto* p1359 = ui::Create<ui::VBox>(pWindow, {{"name", "main_view_page_11"}});
    auto* p1360 = ui::Create<ui::Label>(pWindow, {{"text", "Page 12: Control Comprehensive Test"}});
    ui::Attach(p1359, p1360);

    auto* p1361 = ui::Create<ui::VBox>(pWindow, {});
    auto* p1362 = ui::Create<ui::HBox>(pWindow, {{"bkcolor", "white"}, {"height", "80"}, {"margin", "0,10,0,0"}});
    auto* p1363 = ui::Create<ui::Label>(pWindow, {{"text", "  MenuBar Example:"}, {"text_align", "right,vcenter"}, {"height", "36"}});
    ui::Attach(p1362, p1363);

    auto* p1364 = ui::Create<ui::MenuBar>(pWindow, {{"name", "menu_bar_test"}, {"bkcolor", "#FFE5E5E5"}, {"height", "36"}, {"margin", "6,0,20,0"}, {"padding", "4,0,0,0"}, {"border_round", "4,4"}});
    ui::Attach(p1362, p1364);

    ui::Attach(p1361, p1362);

    auto* p1365 = ui::Create<ui::HBox>(pWindow, {{"bkcolor", "Gainsboro"}, {"height", "300"}, {"margin", "10,10,10,10"}, {"padding", "4,4,4,4"}});
    auto* p1366 = ui::Create<ui::Label>(pWindow, {{"text", "Controls and containers that can be dragged to change position"}, {"mouse_enabled", "false"}});
    ui::Attach(p1365, p1366);

    auto* p1367 = ui::Create<ui::ControlMovable>(pWindow, {{"bkcolor", "Salmon"}, {"width", "160"}, {"height", "160"}, {"border_round", "8,8"}, {"margin", "8,40,8,8"}, {"valign", "top"}, {"halign", "left"}, {"move_pos_reserve_width", "40"}, {"move_pos_reserve_height", "40"}, {"move_pos_keep_within_parent", "true"}});
    ui::Attach(p1365, p1367);

    auto* p1368 = ui::Create<ui::BoxMovable>(pWindow, {{"bkcolor", "Salmon"}, {"width", "160"}, {"height", "160"}, {"border_round", "8,8"}, {"margin", "8,8,8,8"}, {"valign", "top"}, {"halign", "right"}});
    auto* p1369 = ui::Create<ui::Label>(pWindow, {{"text", "Draggable Container"}, {"valign", "center"}, {"halign", "center"}, {"mouse_enabled", "false"}});
    ui::Attach(p1368, p1369);

    ui::Attach(p1365, p1368);

    ui::Attach(p1361, p1365);

    auto* p1370 = ui::Create<ui::HBox>(pWindow, {{"bkcolor", "Gainsboro"}, {"height", "300"}, {"margin", "10,10,10,10"}});
    auto* p1371 = ui::Create<ui::Label>(pWindow, {{"text", "Controls and containers that can be dragged to resize or move"}, {"mouse_enabled", "false"}});
    ui::Attach(p1370, p1371);

    auto* p1372 = ui::Create<ui::ControlResizable>(pWindow, {{"bkcolor", "Salmon"}, {"width", "160"}, {"height", "160"}, {"border_round", "8,8"}, {"border_size", "1"}, {"border_color", "gray"}, {"margin", "40,40,8,8"}, {"valign", "top"}, {"halign", "left"}, {"min_width", "40"}, {"min_height", "40"}, {"max_width", "300"}, {"max_height", "240"}, {"resize_size_box", "8,8,8,8"}, {"resize_reserve_width", "20"}, {"resize_reserve_height", "20"}, {"enable_move_pos", "true"}});
    ui::Attach(p1370, p1372);

    auto* p1373 = ui::Create<ui::ControlResizable>(pWindow, {{"bkcolor", "Salmon"}, {"width", "160"}, {"height", "160"}, {"border_round", "8,8"}, {"border_size", "1"}, {"border_color", "gray"}, {"margin", "40,40,8,8"}, {"valign", "top"}, {"halign", "center"}, {"resize_size_box", "8,8,8,8"}, {"enable_move_pos", "true"}, {"move_pos_keep_within_parent", "true"}, {"resize_keep_within_parent", "true"}});
    ui::Attach(p1370, p1373);

    auto* p1374 = ui::Create<ui::BoxResizable>(pWindow, {{"bkcolor", "Salmon"}, {"width", "200"}, {"height", "200"}, {"border_round", "8,8"}, {"margin", "8,8,8,8"}, {"valign", "top"}, {"halign", "right"}, {"border_size", "1"}, {"border_color", "gray"}, {"resize_size_box", "8,8,8,8"}, {"enable_move_pos", "true"}});
    auto* p1375 = ui::Create<ui::Label>(pWindow, {{"text", "Draggable container to change position Draggable container to resize"}, {"multi_line", "true"}, {"replace_newline", "true"}, {"valign", "center"}, {"halign", "center"}, {"mouse_enabled", "false"}});
    ui::Attach(p1374, p1375);

    ui::Attach(p1370, p1374);

    ui::Attach(p1361, p1370);

    ui::Attach(p1359, p1361);

    ui::Attach(p22, p1359);

    auto* p1376 = ui::Create<ui::VBox>(pWindow, {{"name", "main_view_page_12"}});
    auto* p1377 = ui::Create<ui::Label>(pWindow, {{"text", "Page 13: Control Animation Test"}});
    ui::Attach(p1376, p1377);

    auto* p1378 = ui::Create<ui::VScrollBox>(pWindow, {{"vscrollbar", "true"}, {"hscrollbar", "false"}, {"bkcolor", "white"}});
    auto* p1379 = ui::Create<ui::HBox>(pWindow, {{"child_margin_x", "8"}});
    auto* p1380 = ui::Create<ui::VBox>(pWindow, {{"bkcolor", "white"}, {"border_size", "1"}, {"width", "300"}, {"height", "180"}, {"border_color", "gray"}, {"padding", "2,2,2,2"}});
    auto* p1381 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_color_gray"}, {"width", "180"}, {"height", "32"}, {"border_round", "3,3"}, {"halign", "center"}, {"valign", "center"}, {"text", "Mouse over shows the control below"}, {"tooltip_text", "fade_alpha='true'"}});
    ui::Attach(p1380, p1381);

    auto* p1382 = ui::Create<ui::Control>(pWindow, {{"name", "fade_alpha_1"}, {"halign", "center"}, {"margin", "30,30,30,30"}, {"width", "160"}, {"height", "100"}, {"bkcolor", "red"}, {"border_round", "4,4"}, {"visible", "false"}, {"fade_alpha", "true"}});
    ui::Attach(p1380, p1382);

    ui::Attach(p1379, p1380);

    auto* p1383 = ui::Create<ui::VBox>(pWindow, {{"bkcolor", "white"}, {"border_size", "1"}, {"width", "300"}, {"height", "180"}, {"border_color", "gray"}, {"padding", "2,2,2,2"}});
    auto* p1384 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_color_gray"}, {"width", "180"}, {"height", "32"}, {"border_round", "3,3"}, {"halign", "center"}, {"valign", "center"}, {"text", "Mouse over shows the control below"}, {"tooltip_text", "fade_alpha='true'"}});
    ui::Attach(p1383, p1384);

    auto* p1385 = ui::Create<ui::Control>(pWindow, {{"name", "fade_alpha_2"}, {"halign", "center"}, {"margin", "30,30,30,30"}, {"width", "160"}, {"height", "100"}, {"bkcolor", "red"}, {"border_round", "4,4"}, {"border_size", "2"}, {"border_color", "blue"}, {"visible", "false"}, {"fade_alpha", "true"}});
    ui::Attach(p1383, p1385);

    ui::Attach(p1379, p1383);

    auto* p1386 = ui::Create<ui::VBox>(pWindow, {{"bkcolor", "white"}, {"border_size", "1"}, {"width", "300"}, {"height", "180"}, {"border_color", "gray"}, {"padding", "2,2,2,2"}});
    auto* p1387 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_color_gray"}, {"width", "180"}, {"height", "32"}, {"border_round", "3,3"}, {"halign", "center"}, {"valign", "center"}, {"text", "Mouse over shows the control below"}, {"tooltip_text", "fade_alpha='true'"}});
    ui::Attach(p1386, p1387);

    auto* p1388 = ui::Create<ui::Control>(pWindow, {{"name", "fade_alpha_3"}, {"halign", "center"}, {"margin", "30,30,30,30"}, {"width", "160"}, {"height", "100"}, {"bkcolor", "red"}, {"border_round", "4,4"}, {"box_shadow", "color='gray' offset='1,1' blur_radius='3' spread_radius='3'"}, {"visible", "false"}, {"fade_alpha", "true"}});
    ui::Attach(p1386, p1388);

    ui::Attach(p1379, p1386);

    ui::Attach(p1378, p1379);

    auto* p1389 = ui::Create<ui::HBox>(pWindow, {{"child_margin_x", "8"}});
    auto* p1390 = ui::Create<ui::VBox>(pWindow, {{"bkcolor", "white"}, {"border_size", "1"}, {"width", "300"}, {"height", "180"}, {"border_color", "gray"}, {"padding", "2,2,2,2"}});
    auto* p1391 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_color_gray"}, {"width", "260"}, {"height", "32"}, {"border_round", "3,3"}, {"halign", "center"}, {"valign", "center"}, {"text", "Mouse over shows the control below (slide in from left)"}, {"tooltip_text", "fade_in_out_x_from_left='true'"}});
    ui::Attach(p1390, p1391);

    auto* p1392 = ui::Create<ui::Control>(pWindow, {{"name", "fade_in_out_x_from_left_1"}, {"halign", "center"}, {"margin", "30,30,30,30"}, {"width", "160"}, {"height", "100"}, {"bkcolor", "red"}, {"border_round", "4,4"}, {"visible", "false"}, {"fade_in_out_x_from_left", "true"}});
    ui::Attach(p1390, p1392);

    ui::Attach(p1389, p1390);

    auto* p1393 = ui::Create<ui::VBox>(pWindow, {{"bkcolor", "white"}, {"border_size", "1"}, {"width", "300"}, {"height", "180"}, {"border_color", "gray"}, {"padding", "2,2,2,2"}});
    auto* p1394 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_color_gray"}, {"width", "260"}, {"height", "32"}, {"border_round", "3,3"}, {"halign", "center"}, {"valign", "center"}, {"text", "Mouse over shows the control below (slide in from right)"}, {"tooltip_text", "fade_in_out_x_from_right='true'"}});
    ui::Attach(p1393, p1394);

    auto* p1395 = ui::Create<ui::Control>(pWindow, {{"name", "fade_in_out_x_from_right_1"}, {"halign", "center"}, {"margin", "30,30,30,30"}, {"width", "160"}, {"height", "100"}, {"bkcolor", "red"}, {"border_round", "4,4"}, {"visible", "false"}, {"fade_in_out_x_from_right", "true"}});
    ui::Attach(p1393, p1395);

    ui::Attach(p1389, p1393);

    auto* p1396 = ui::Create<ui::VBox>(pWindow, {{"bkcolor", "white"}, {"border_size", "1"}, {"width", "300"}, {"height", "180"}, {"border_color", "gray"}, {"padding", "2,2,2,2"}});
    auto* p1397 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_color_gray"}, {"width", "260"}, {"height", "32"}, {"border_round", "3,3"}, {"halign", "center"}, {"valign", "center"}, {"text", "Mouse over shows the control below (slide in from top)"}, {"tooltip_text", "fade_in_out_y_from_top='true'"}});
    ui::Attach(p1396, p1397);

    auto* p1398 = ui::Create<ui::Control>(pWindow, {{"name", "fade_in_out_y_from_top_1"}, {"halign", "center"}, {"margin", "30,30,30,30"}, {"width", "160"}, {"height", "100"}, {"bkcolor", "red"}, {"border_round", "4,4"}, {"visible", "false"}, {"fade_in_out_y_from_top", "true"}});
    ui::Attach(p1396, p1398);

    ui::Attach(p1389, p1396);

    auto* p1399 = ui::Create<ui::VBox>(pWindow, {{"bkcolor", "white"}, {"border_size", "1"}, {"width", "300"}, {"height", "180"}, {"border_color", "gray"}, {"padding", "2,2,2,2"}});
    auto* p1400 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_color_gray"}, {"width", "260"}, {"height", "32"}, {"border_round", "3,3"}, {"halign", "center"}, {"valign", "center"}, {"text", "Mouse over shows the control below (slide in from bottom)"}, {"tooltip_text", "fade_in_out_y_from_bottom='true'"}});
    ui::Attach(p1399, p1400);

    auto* p1401 = ui::Create<ui::Control>(pWindow, {{"name", "fade_in_out_y_from_bottom_1"}, {"halign", "center"}, {"margin", "30,30,30,30"}, {"width", "160"}, {"height", "100"}, {"bkcolor", "red"}, {"border_round", "4,4"}, {"visible", "false"}, {"fade_in_out_y_from_bottom", "true"}});
    ui::Attach(p1399, p1401);

    ui::Attach(p1389, p1399);

    ui::Attach(p1378, p1389);

    auto* p1402 = ui::Create<ui::HBox>(pWindow, {{"child_margin_x", "8"}});
    auto* p1403 = ui::Create<ui::VBox>(pWindow, {{"bkcolor", "white"}, {"border_size", "1"}, {"width", "300"}, {"height", "180"}, {"border_color", "gray"}, {"padding", "2,2,2,2"}});
    auto* p1404 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_color_gray"}, {"width", "260"}, {"height", "32"}, {"border_round", "3,3"}, {"halign", "center"}, {"valign", "center"}, {"text", "Mouse over shows the control below (width fade)"}, {"tooltip_text", "fade_width='true'"}});
    ui::Attach(p1403, p1404);

    auto* p1405 = ui::Create<ui::Control>(pWindow, {{"name", "fade_width_1"}, {"halign", "center"}, {"margin", "30,30,30,30"}, {"width", "160"}, {"height", "100"}, {"bkcolor", "red"}, {"border_round", "4,4"}, {"visible", "false"}, {"fade_width", "true"}});
    ui::Attach(p1403, p1405);

    ui::Attach(p1402, p1403);

    auto* p1406 = ui::Create<ui::VBox>(pWindow, {{"bkcolor", "white"}, {"border_size", "1"}, {"width", "300"}, {"height", "180"}, {"border_color", "gray"}, {"padding", "2,2,2,2"}});
    auto* p1407 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_color_gray"}, {"width", "260"}, {"height", "32"}, {"border_round", "3,3"}, {"halign", "center"}, {"valign", "center"}, {"text", "Mouse over shows the control below (height fade)"}, {"tooltip_text", "fade_height='true'"}});
    ui::Attach(p1406, p1407);

    auto* p1408 = ui::Create<ui::Control>(pWindow, {{"name", "fade_height_1"}, {"halign", "center"}, {"margin", "30,30,30,30"}, {"width", "160"}, {"height", "100"}, {"bkcolor", "red"}, {"border_round", "4,4"}, {"visible", "false"}, {"fade_height", "true"}});
    ui::Attach(p1406, p1408);

    ui::Attach(p1402, p1406);

    auto* p1409 = ui::Create<ui::VBox>(pWindow, {{"bkcolor", "white"}, {"border_size", "1"}, {"width", "300"}, {"height", "180"}, {"border_color", "gray"}, {"padding", "2,2,2,2"}});
    auto* p1410 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_color_gray"}, {"width", "260"}, {"height", "32"}, {"border_round", "3,3"}, {"halign", "center"}, {"valign", "center"}, {"text", "Mouse over shows the control below (size fade)"}, {"tooltip_text", "fade_size='true'"}});
    ui::Attach(p1409, p1410);

    auto* p1411 = ui::Create<ui::Control>(pWindow, {{"name", "fade_size_1"}, {"halign", "center"}, {"margin", "30,30,30,30"}, {"width", "160"}, {"height", "100"}, {"bkcolor", "red"}, {"border_round", "4,4"}, {"visible", "false"}, {"fade_size", "true"}, {"fade_total_ms", "200"}, {"fade_frame_interval_ms", "16"}, {"fade_easing_function", "EaseInOutCubic"}});
    ui::Attach(p1409, p1411);

    ui::Attach(p1402, p1409);

    auto* p1412 = ui::Create<ui::VBox>(pWindow, {{"bkcolor", "white"}, {"border_size", "1"}, {"width", "300"}, {"height", "180"}, {"border_color", "gray"}, {"padding", "2,2,2,2"}});
    auto* p1413 = ui::Create<ui::Label>(pWindow, {{"halign", "center"}, {"text", "Control Hot State Animation"}, {"tooltip_text", "fade_hot='true'"}});
    ui::Attach(p1412, p1413);

    auto* p1414 = ui::Create<ui::HBox>(pWindow, {});
    auto* p1415 = ui::Create<ui::VBox>(pWindow, {{"child_margin_y", "8"}, {"padding", "8,4,8,4"}});
    auto* p1416 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_blue_80x30"}, {"text", "blue"}});
    ui::Attach(p1415, p1416);

    auto* p1417 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_red_80x30"}, {"text", "red"}});
    ui::Attach(p1415, p1417);

    auto* p1418 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_white_80x30"}, {"text", "white"}});
    ui::Attach(p1415, p1418);

    auto* p1419 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_gray_80x30"}, {"text", "gray"}});
    ui::Attach(p1415, p1419);

    ui::Attach(p1414, p1415);

    auto* p1420 = ui::Create<ui::VBox>(pWindow, {{"child_margin_y", "8"}, {"padding", "8,4,8,4"}});
    auto* p1421 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_color_blue"}, {"width", "80"}, {"height", "30"}, {"text", "blue"}});
    ui::Attach(p1420, p1421);

    auto* p1422 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_color_red"}, {"width", "80"}, {"height", "30"}, {"text", "red"}});
    ui::Attach(p1420, p1422);

    auto* p1423 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_color_white"}, {"width", "80"}, {"height", "30"}, {"text", "white"}});
    ui::Attach(p1420, p1423);

    auto* p1424 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_color_gray"}, {"width", "80"}, {"height", "30"}, {"text", "gray"}});
    ui::Attach(p1420, p1424);

    ui::Attach(p1414, p1420);

    auto* p1425 = ui::Create<ui::VBox>(pWindow, {{"child_margin_y", "8"}, {"padding", "8,4,8,4"}});
    auto* p1426 = ui::Create<ui::CheckBox>(pWindow, {{"class", "checkbox_1"}, {"text", "CheckBox"}, {"margin", "0,3,0,3"}, {"selected", "true"}, {"normal_text_color", "red"}, {"hot_text_color", "blue"}, {"selected_normal_text_color", "red"}, {"selected_hot_text_color", "blue"}, {"fade_hot", "true"}});
    ui::Attach(p1425, p1426);

    auto* p1427 = ui::Create<ui::Label>(pWindow, {{"text", "Text Label"}, {"margin", "0,3,0,3"}, {"normal_text_color", "red"}, {"hot_text_color", "blue"}, {"fade_hot", "true"}});
    ui::Attach(p1425, p1427);

    auto* p1428 = ui::Create<ui::Label>(pWindow, {{"text", "Normal Control"}, {"margin", "0,3,0,3"}, {"width", "80"}, {"height", "30"}, {"text_align", "vcenter,hcenter"}, {"hot_color", "blue"}, {"fade_hot", "true"}});
    ui::Attach(p1425, p1428);

    auto* p1429 = ui::Create<ui::Control>(pWindow, {{"margin", "0,3,0,3"}, {"width", "80"}, {"height", "30"}, {"normal_image", "autumn.png"}, {"hot_image", "svg_test.svg"}, {"fade_hot", "true"}, {"fade_hot_total_ms", "200"}, {"fade_hot_frame_interval_ms", "16"}, {"fade_hot_easing_function", "EaseInOutCubic"}});
    ui::Attach(p1425, p1429);

    ui::Attach(p1414, p1425);

    ui::Attach(p1412, p1414);

    ui::Attach(p1402, p1412);

    ui::Attach(p1378, p1402);

    ui::Attach(p1376, p1378);

    ui::Attach(p22, p1376);

    ui::Attach(p7, p22);

    ui::Attach(p0, p7);


    ui::Attach(pWindow, p0);
}
