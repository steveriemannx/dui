#include "ControlForm.h"
#include "dui/Utils/UiBuilder.h"
#include "AboutForm.h"
#include "TestForm.h"
#include "AnimationForm.h"

#include <fstream>

static void BuildUIFromXmlControls(ui::Window* pWindow);

//ID of the system global hotkey
#define SYSTEM_HOTKEY_ID 111

ControlForm::ControlForm()
{
}


ControlForm::~ControlForm()
{
}

void ControlForm::OnInitWindow()
{
    SetupWindow();
    BuildUI();
    if (auto* pRichText = ui::Find<ui::RichText>(this, "rich_text_demo")) {
        pRichText->SetText("RichText: <a href=\"www.baidu.com\">Click to visit Baidu</a><br/>"
                           "RichText: <a href=\"www.sohu.com\">Click to visit Sohu</a>");
    }
    BindEvents();

    BaseClass::OnInitWindow();
}

void ControlForm::SetupWindow()
{
    // In code mode the window is not loaded from XML, so the skin folder must be
    // set explicitly; otherwise GetResourcePath() is empty and LoadRichEditData()
    // (and the popup menu skin folder) cannot resolve "controls/controls.xml".
    SetWindowSize(1000, 760);
    CenterWindow();

    // Use the OS-provided system shadow on all platforms.
    SetShadowAttached(true);
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);

    SetSizeBox(ui::UiRect(4, 4, 4, 4), true);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), true);
    SetUseSystemCaption(false);
    SetEnableSnapLayoutMenu(true);
    SetEnableSysMenu(true);
    SetSysMenuRect(ui::UiRect(0, 0, 36, 36), true);
    SetWindowMinimumSize(ui::UiSize(400, 320), true);
}

void ControlForm::BuildUI()
{
    //Build the UI purely in code (corresponds to the controls.xml layout)
    BuildUIFromXmlControls(this);
}

void ControlForm::BindEvents()
{
#ifdef DUI_BUILD_FOR_SDL
    //Display basic SDL information
    ui::Label* pTitle = ui::Find<ui::Label>(this, "window_title");
    if (pTitle != nullptr) {
        DString title = pTitle->GetText();
        DString driverName = GetVideoDriverName();
        DString renderName = GetWindowRenderName();
        DString newTitle = ui::StringUtil::Printf("%s[SDL: VideoDriver:\"%s\", RenderName:\"%s\"]", title.c_str(), driverName.c_str(), renderName.c_str());
        pTitle->SetText(newTitle);
    }
#endif

    /* Initialize ListBox data */
    ui::ListBox* list = ui::Find<ui::ListBox>(this, "list");
    if (list != nullptr) {
        for (auto i = 0; i < 30; i++)
        {
            auto* element = ui::Create<ui::ListBoxItem>(this, {});
            element->SetText(ui::StringUtil::Printf("ui::VListBox::ListBoxItem %d", i));
            element->SetClass("listitem");
            element->SetFixedHeight(ui::UiFixedInt(20), true, true);
            list->AddItem(element);
        }
    }

    ui::TreeView* pTree = ui::Find<ui::TreeView>(this, "tree");
    if (pTree != nullptr) {
        ui::TreeNode* pRootNode = pTree->GetRootNode();
        ASSERT(pRootNode != nullptr);
        if (pRootNode != nullptr) {
            ui::TreeNode* pTestNode = pRootNode->FindChildNodeByText("ui::TreeView Parent Node 2", true);
            ASSERT(pTestNode != nullptr);
            if (pTestNode != nullptr) {
                auto* pNode0 = ui::Create<ui::TreeNode>(this, {});
                pNode0->SetClass("tree_node");
                pNode0->SetText("Dynamic Node 0(top)");
                pTestNode->AddChildNodeAt(pNode0, 0);

                auto* pNode2 = ui::Create<ui::TreeNode>(this, {});
                pNode2->SetClass("tree_node");
                pNode2->SetText("Dynamic Node 1(end)");
                pTestNode->AddChildNode(pNode2);

                auto* pNode1 = ui::Create<ui::TreeNode>(this, {});
                pNode1->SetClass("tree_node");
                pNode1->SetText("Dynamic Node 2(at index 2)");
                pTestNode->AddChildNodeAt(pNode1, 2);
            }
        }
    }

    //Initialize the Combo data
    ui::Combo* combo = ui::Find<ui::Combo>(this, "combo");
    if (combo != nullptr) {
        ui::TreeView* pTreeView = combo->GetTreeView();
        ui::TreeNode* pTreeNode = pTreeView->GetRootNode();
        for (auto i = 0; i < 10; i++) {
            auto* node = ui::Create<ui::TreeNode>(this, {});
            node->SetClass("tree_node");
            node->SetText(ui::StringUtil::Printf("ui::Combo::TreeNode %d", i));
            pTreeNode->AddChildNode(node);
        }
    }
//#ifdef _DEBUG
//    ASSERT(combo->GetCount() == 10);
//    ASSERT(combo->GetCurSel() == -1);
//    combo->SetCurSel(1);
//    ASSERT(combo->GetCurSel() == 1);
//    combo->SetItemData(1, 0xFF1234);
//    ASSERT(combo->GetItemData(1) == 0xFF1234);
//    combo->SetItemText(2, "2");
//    ASSERT(combo->GetItemText(2) == "2");
//
//    size_t nIndex = combo->AddTextItem("Last");
//    ASSERT(combo->GetItemText(nIndex) == "Last");
//
//    nIndex = combo->InsertTextItem(nIndex, "Last2");
//    ASSERT(combo->GetItemText(nIndex) == "Last2");
//
//    ASSERT(combo->DeleteItem(nIndex));
//
//    ASSERT(combo->GetItemText(combo->GetCurSel()) == combo->GetText());
//    combo->SetText("Test");
//    ASSERT(combo->GetText() == "Test");
//#endif

    ui::FilterCombo* filterCombo = ui::Find<ui::FilterCombo>(this, "filter_combo");
    if (filterCombo != nullptr) {
        for (auto i = 0; i < 100; i++) {
            filterCombo->AddTextItem(ui::StringUtil::Printf("Item %d FilterCombo", i));
        }
    }

    ui::CheckCombo* check_combo = ui::Find<ui::CheckCombo>(this, "check_combo");
    if (check_combo != nullptr) {
        check_combo->AddTextItem("Monday");
        check_combo->AddTextItem("Tuesday");
        check_combo->AddTextItem("Wednesday");
        check_combo->AddTextItem("Thursday");
        check_combo->AddTextItem("Friday");
        check_combo->AddTextItem("Saturday");
        check_combo->AddTextItem("Sunday");
    }

    int32_t nThreadIdentifier = ui::ThreadIdentifier::kThreadWorker;
    if (!ui::GlobalManager::Instance().Thread().HasThread(nThreadIdentifier)) {
        nThreadIdentifier = ui::ThreadIdentifier::kThreadUI;
    }

    /* Load xml file content in global misc thread, and post update RichEdit task to UI thread */
    ui::GlobalManager::Instance().Thread().PostTask(nThreadIdentifier, UiBind(&ControlForm::LoadRichEditData, this));

    /* Post repeat task to update progress value 200 milliseconds once */
    /* Using ToWeakCallback to protect closure when if [ControlForm] was destoryed */    
    ui::GlobalManager::Instance().Thread().PostRepeatedTask(nThreadIdentifier,
        ui::UiBind(this, [this]() {
            float fProgress = (float)(std::time(nullptr) % 100);
            ui::GlobalManager::Instance().Thread().PostTask(ui::kThreadUI, UiBind(&ControlForm::OnProgressValueChagned, this, fProgress));
            }),
        300);

    /* Show settings menu */
    ui::Button* settings = ui::Find<ui::Button>(this, "settings");
    if (settings != nullptr) {
        settings->AttachClick([this, settings](const ui::EventArgs& args) {
            ui::UiRect rect = args.GetSender()->GetPos();
            ui::UiPoint point;
            point.x = rect.left;
            point.y = rect.bottom;
            ClientToScreen(point);

            //Show the menu and keep the settings button in the Push state
            ShowPopupMenu(point, settings);
            return true;
            });
    }

    //Register a context menu, demonstrating the feature (both methods can register a context menu)
    AttachRichEditEvents(ui::Find<ui::RichEdit>(this, "edit"));
    AttachRichEditEvents(ui::Find<ui::RichEdit>(this, "edit2"));

    //Show the color picker of a modal dialog
    ui::Button* pShowColorPicker = ui::Find<ui::Button>(this, "show_color_picker");
    if (pShowColorPicker != nullptr) {
        pShowColorPicker->AttachClick([this](const ui::EventArgs& args) {
            ShowColorPicker(true);
            return true;
            });
    }

    //Show a modal dialog
    ui::Button* pShowModal = ui::Find<ui::Button>(this, "domodal2");
    if (pShowModal != nullptr) {
        pShowModal->AttachClick([this](const ui::EventArgs& args) {
            ShowDoModalDlg();
            return true;
            });
    }

    //RichText displays hyperlinks
    ui::RichText* pRichText = ui::Find<ui::RichText>(this, "rich_text_demo");
    if (pRichText != nullptr) {
        pRichText->AttachLinkClick([this](const ui::EventArgs& args) {
            const DString::value_type* url = (const DString::value_type*)args.wParam;
            if (url != nullptr) {
                ui::SystemUtil::ShowMessageBox(this, url, "RichText Click HyperLink");
            }
            return true;
            });
    }

    //HyperLink control
    ui::HyperLink* pHyperLink = ui::Find<ui::HyperLink>(this, "hyper_link1");
    if (pHyperLink != nullptr) {
        pHyperLink->AttachLinkClick([this](const ui::EventArgs& args) {
            const DString::value_type* url = (const DString::value_type*)args.wParam;
            if (url != nullptr) {
                ui::SystemUtil::ShowMessageBox(this, url, "HyperLink Click HyperLink");
            }
            return true;
            });
    }

    pHyperLink = ui::Find<ui::HyperLink>(this, "hyper_link2");
    if (pHyperLink != nullptr) {
        pHyperLink->AttachLinkClick([this](const ui::EventArgs& /*args*/) {
            ui::SystemUtil::ShowMessageBox(this, "Text Button Event Response", "HyperLink Click");
            return true;
            });
    }

    //Hotkey settings
    ui::HotKey* pHotKey = ui::Find<ui::HotKey>(this, "set_hot_key");
    ui::Button* pHotKeyButton = ui::Find<ui::Button>(this, "btn_set_hot_key");
    if (pHotKey && pHotKeyButton) {
        pHotKeyButton->AttachClick([this, pHotKey](const ui::EventArgs& args) {
            uint8_t wVirtualKeyCode = 0;
            uint8_t wModifiers = 0;
            pHotKey->GetHotKey(wVirtualKeyCode, wModifiers);
            if (wVirtualKeyCode != 0) {
                //Set as the hotkey of the active window
                int32_t nRet = SetWindowHotKey(wVirtualKeyCode, wModifiers);
                ASSERT(nRet == 1);
            }

            //if (1) {
            //    //Test code
            //    DString hotKeyName = pHotKey->GetHotKeyName();
            //    uint8_t wVirtualKeyCode = 0;
            //    uint8_t wModifiers = 0;
            //    pHotKey->GetHotKey(wVirtualKeyCode, wModifiers);
            //    ASSERT(pHotKey->GetHotKey() == MAKEWORD(wVirtualKeyCode, wModifiers));

            //    const uint8_t wModifiers2 = ui::kHotKey_Shift | ui::kHotKey_Contrl | ui::kHotKey_Alt | ui::kHotKey_Ext;
            //    const uint8_t wVirtualKeyCode2 = kVK_HOME;

            //    pHotKey->SetHotKey(wVirtualKeyCode2, wModifiers2);
            //    pHotKey->GetHotKey(wVirtualKeyCode, wModifiers);
            //    ASSERT(wModifiers2 == wModifiers);
            //    ASSERT(wVirtualKeyCode2 == wVirtualKeyCode);

            //    ASSERT(pHotKey->GetHotKey() == MAKEWORD(wVirtualKeyCode, wModifiers));

            //    pHotKey->SetHotKey(MAKEWORD(wVirtualKeyCode2, wModifiers2));
            //    ASSERT(pHotKey->GetHotKey() == MAKEWORD(wVirtualKeyCode2, wModifiers2));
            //}

            return true;
            });
    }

    pHotKey = ui::Find<ui::HotKey>(this, "set_system_hot_key");
    pHotKeyButton = ui::Find<ui::Button>(this, "btn_set_system_hot_key");
    if (pHotKey && pHotKeyButton) {
        pHotKeyButton->AttachClick([this, pHotKey](const ui::EventArgs& args) {
            uint8_t wVirtualKeyCode = 0;
            uint8_t wModifiers = 0;
            pHotKey->GetHotKey(wVirtualKeyCode, wModifiers);
            if (wVirtualKeyCode != 0) {
                //Set as a system global hotkey
                bool nRet = RegisterHotKey(wVirtualKeyCode, wModifiers, SYSTEM_HOTKEY_ID);
                ASSERT(nRet);
            }
            return true;
        });
    }

    //Test page, open a new window
    ui::Button* pTestBtn = ui::Find<ui::Button>(this, "test_btn");
    if (pTestBtn != nullptr) {
        pTestBtn->AttachClick([this](const ui::EventArgs&) {
            ShowTestWindow();
            return true;
            });
    }

    //Response function of the animation test button
    ui::Button* pAnimationBtn = ui::Find<ui::Button>(this, "animation_btn");
    if (pAnimationBtn != nullptr) {
        pAnimationBtn->AttachClick([this](const ui::EventArgs&) {
            ShowAnimationWindow();
            return true;
            });
    }

    //Tray icon functionality
    ui::CheckBox* pTrayIconCheckBox = ui::Find<ui::CheckBox>(this, "checkbox_tray_icon");
    if (pTrayIconCheckBox != nullptr) {
        pTrayIconCheckBox->AttachSelect([this](const ui::EventArgs&) {
            //Enable
            ShowTrayIcon(true);
            return true;
            });
        pTrayIconCheckBox->AttachUnSelect([this](const ui::EventArgs&) {
            //Disable
            ShowTrayIcon(false);
            return true;
        });
    }
}

void ControlForm::OnCloseWindow()
{
    //Close the tray icon
    ShowTrayIcon(false);

    BaseClass::OnCloseWindow();
}

void ControlForm::AttachRichEditEvents(ui::RichEdit* edit)
{
    if (edit == nullptr) {
        return;
    }
    //Right-click menu
    edit->AttachContextMenu([this](const ui::EventArgs& args) {
        if (args.eventType == ui::kEventContextMenu) {
            ui::UiPoint pt = args.ptMouse;
            if ((pt.x != -1) && (pt.y != -1)) {
                ui::Control* pControl = (ui::Control*)args.lParam;//Control at the current click point

                //Context menu generated by a mouse message
                ClientToScreen(pt);
                ShowPopupMenu(pt, nullptr);
            }
            else {
                //Press Shift + F10 to let the system generate a context menu
                pt = { 100, 100 };
                ClientToScreen(pt);
                ShowPopupMenu(pt, nullptr);
            }
        }
        return true;
        });

    //File drag-and-drop support
    edit->AttachDropData([this, edit](const ui::EventArgs& args) {
        ui::FilePath filePath;
        if (args.wParam == ui::kControlDropTypeWindows) {
            const ui::ControlDropData_Windows* dropData = (const ui::ControlDropData_Windows*)args.lParam;
            if ((dropData != nullptr) && !dropData->m_fileList.empty()) {
                filePath = dropData->m_fileList[0];
            }
        }
        else if (args.wParam == ui::kControlDropTypeSDL) {
            const ui::ControlDropData_SDL* dropData = (const ui::ControlDropData_SDL*)args.lParam;
            if ((dropData != nullptr) && !dropData->m_fileList.empty()) {
                filePath = dropData->m_fileList[0];
            }
        }
        if (!filePath.IsEmpty()) {
            ui::FileUtil fileUtil;
            std::vector<uint8_t> fileData;
            fileUtil.ReadFileData(filePath, fileData);
            if (!fileData.empty()) {
                std::wstring result;
                if (ui::StringCharset::GetDataAsString((const char*)fileData.data(), (uint32_t)fileData.size(), result)) {
                    DString text = ui::StringConvert::WStringToT(result);
                    if (edit != nullptr) {
                        edit->SetText(text);
                    }
                }
            }
        }
        return true;
        });

    //Display RichEdit in full screen
    ui::Button* pFullscreenBtn = ui::Find<ui::Button>(this, "rich_edit_fullscreen_btn");
    if (pFullscreenBtn != nullptr) {
        pFullscreenBtn->AttachClick([this](const ui::EventArgs&) {
            ui::Control* pControl = ui::Find<ui::Control>(this, "edit");
            if (pControl != nullptr) {
                this->SetFullscreenControl(pControl);
            }
            return true;
            });
    }
}

void ControlForm::ShowColorPicker(bool bDoModal)
{
    bool bModalDlg = bDoModal; //Whether to use a modal dialog
    ui::ColorPicker colorDlg;
    ui::ColorPicker* pColorPicker = nullptr;
    if (!bModalDlg) {
        pColorPicker = new ui::ColorPicker;
    }
    else {
        pColorPicker = &colorDlg;
    }

    //Function executed after the window is created
    auto OnInitColorPicker = [this, pColorPicker](const ui::EventArgs&) {
        ui::RichEdit* pEdit = ui::Find<ui::RichEdit>(this, "edit");
        if (pEdit != nullptr) {
            DString oldTextColor = pEdit->GetTextColor();
            if (!oldTextColor.empty()) {
                pColorPicker->SetSelectedColor(pEdit->GetUiColor(oldTextColor));
            }
            //If a color is selected in the UI, temporarily update the text color of the RichEdit control
            pColorPicker->AttachSelectColor([this, pEdit](const ui::EventArgs& args) {
                ui::UiColor newColor = ui::UiColor((uint32_t)args.wParam);
                pEdit->SetTextColor(pEdit->GetColorString(newColor));
                return true;
                });

            //Window close event
            pColorPicker->AttachWindowClose([this, pColorPicker, pEdit, oldTextColor](const ui::EventArgs& args) {
                ui::UiColor newColor = pColorPicker->GetSelectedColor();
                if ((args.wParam == ui::kWindowCloseOK) && !newColor.IsEmpty()) {
                    //If it is "OK", set the text color of the RichEdit control
                    pEdit->SetTextColor(pEdit->GetColorString(newColor));
                }
                else {
                    //If it is "Cancel" or the window is closed, restore the original color
                    pEdit->SetTextColor(oldTextColor);
                }
                return true;
                });

        }
        return true;
        };
    pColorPicker->AttachWindowCreateMsg(OnInitColorPicker);

    ui::WindowCreateParam createParam;
    createParam.m_dwStyle = ui::kWS_POPUP;
    createParam.m_dwExStyle = ui::kWS_EX_LAYERED;
    createParam.m_bCenterWindow = true;
    if (!bModalDlg) {
        pColorPicker->CreateWnd(this, createParam);
        pColorPicker->ShowModalFake();
    }
    else {
        pColorPicker->DoModal(this, createParam);
    }
}

void ControlForm::ShowDoModalDlg()
{
    ui::WindowImplBase simpleWnd;
    simpleWnd.InitSkin("controls", "about.xml");
    ui::WindowCreateParam createParam;
    createParam.m_dwStyle = ui::kWS_POPUP;
    createParam.m_dwExStyle = ui::kWS_EX_LAYERED;
    createParam.m_windowTitle = "AboutForm";
    createParam.m_bCenterWindow = true;
    simpleWnd.DoModal(this, createParam);
}

void ControlForm::ShowTestWindow()
{
    TestForm* testForm = new TestForm();
    ui::WindowCreateParam createParam;
    createParam.m_dwStyle = ui::kWS_POPUP;
    createParam.m_dwExStyle = ui::kWS_EX_LAYERED;
    createParam.m_windowTitle = "TestWindow";
    createParam.m_bCenterWindow = true;
    testForm->CreateWnd(this, createParam);
    testForm->ShowModalFake();
}

void ControlForm::ShowAnimationWindow()
{
    AnimationForm* testForm = new AnimationForm();
    ui::WindowCreateParam createParam;
    createParam.m_dwStyle = ui::kWS_POPUP;
    createParam.m_dwExStyle = ui::kWS_EX_LAYERED;
    createParam.m_windowTitle = "AnimationWindow";
    createParam.m_bCenterWindow = true;
    testForm->CreateWnd(this, createParam);
    testForm->ShowModalFake();
}

void ControlForm::ShowPopupMenu(const ui::UiPoint& point, ui::Control* pRelatedControl)
{
    ui::Menu* menu = new ui::Menu(this, pRelatedControl);//A parent window is required, otherwise the program status bar becomes inactive when the menu pops up
    menu->SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    menu->SetSkinFolder(GetResourcePath().ToString());
    DString xml("menu/settings_menu.xml");
    menu->ShowMenu(xml, point);

    //Add a submenu item to the second-level menu
    ui::MenuItem* menu_fourth = ui::Find<ui::MenuItem>(menu, "fourth");
    if (menu_fourth != nullptr) {
        auto* menu_item = ui::Create<ui::MenuItem>(menu, {{"text", "Dynamically created"}, {"class", "menu_element"}});
        menu_item->SetFixedWidth(ui::UiFixedInt(200), true, true);
        menu_item->SetFontId("system_14");
        menu_item->SetTextPadding({ 20, 0, 20, 0 }, true);
        menu_fourth->AddSubMenuItemAt(menu_item, 1);//After adding, the resource is managed by the menu
    }
    

    //Add a submenu item to the first-level menu
    /*
    menu_item = new ui::MenuItem(menu);
    menu_item->SetWindow(menu);
    menu_item->SetText("Dynamically created");
    menu_item->SetClass("menu_element");
    menu_item->SetFixedWidth(180);
    menu_item->SetFontId("system_14");
    menu_item->SetTextPadding({ 20, 0, 20, 0 });
    menu->AddMenuItemAt(menu_item, 4);//After adding, the resource is managed by the menu
    */

    //Demonstration of the checkbox menu item functionality
    static bool s_is_checked_01_flag = false;
    bool& flag = s_is_checked_01_flag;
    ui::MenuItem* menu_check_01 = ui::Find<ui::MenuItem>(menu, "menu_check_01");
    if (menu_check_01 != nullptr) {
        menu_check_01->AttachClick([&flag](const ui::EventArgs& args) {
            flag = true;
            return true;
            });
    }
    ui::CheckBox* menuCheckBox01 = ui::Find<ui::CheckBox>(menu, "menu_checkbox_01");
    if (menuCheckBox01 != nullptr) {
        menuCheckBox01->Selected(s_is_checked_01_flag);
    }

    ui::MenuItem* menu_check_02 = ui::Find<ui::MenuItem>(menu, "menu_check_02");
    if (menu_check_02 != nullptr) {
        menu_check_02->AttachClick([&flag](const ui::EventArgs& args) {
            flag = false;
            return true;
            });
    }

    ui::CheckBox* menuCheckBox02 = ui::Find<ui::CheckBox>(menu, "menu_checkbox_02");
    if (menuCheckBox02 != nullptr) {
        menuCheckBox02->Selected(!s_is_checked_01_flag);
    }


    /* About menu */
    ui::MenuItem* menu_about = menu->GetMenuItemByName("about");
    if (menu_about != nullptr) {
        menu_about->AttachClick([this](const ui::EventArgs&) {
            ui::GlobalManager::Instance().Thread().PostTask(ui::kThreadUI,
                [this]() {
                    AboutForm* about_form = new AboutForm();
                    ui::WindowCreateParam createParam;
                    createParam.m_dwStyle = ui::kWS_OVERLAPPEDWINDOW;
                    createParam.m_dwExStyle = ui::kWS_EX_LAYERED;
                    createParam.m_windowTitle = "AboutForm";
                    createParam.m_nWidth = 400;
                    createParam.m_nHeight = 220;
                    createParam.m_bCenterWindow = true;
                    about_form->CreateWnd(this, createParam);
                    about_form->ShowModalFake();
                });
            return true;
        });
    }
}

void ControlForm::LoadRichEditData()
{
    ui::FilePath controls_xml = ui::GlobalManager::Instance().GetResourcePath();
    controls_xml += "controls/controls.xml";

    //XML files are loaded in UTF-8 encoding
    std::string xml;
    std::vector<unsigned char> xmlData;
    std::vector<uint8_t> fileData;
    bool bReadOk = false;
    if (ui::GlobalManager::Instance().MemoryResources().IsOpen()) {
        bReadOk = ui::GlobalManager::Instance().MemoryResources().GetData(controls_xml, xmlData);
        if (!bReadOk || xmlData.empty()) {
            xmlData.clear();
            const ui::FilePath themeDefaultPath = ui::GlobalManager::Instance().GetThemeDefaultPath();
            if (!themeDefaultPath.IsEmpty()) {
                ui::FilePath defaultXml = themeDefaultPath;
                defaultXml += "controls/controls.xml";
            bReadOk = ui::GlobalManager::Instance().MemoryResources().GetData(defaultXml, xmlData);
            }
            if (!bReadOk || xmlData.empty()) {
                xmlData.clear();
            bReadOk = ui::GlobalManager::Instance().MemoryResources().GetData(ui::FilePath("themes/default/controls/controls.xml"), xmlData);
            }
        }
        if (!bReadOk || xmlData.empty()) {
            xmlData.clear();
            if (ui::FileUtil::ReadFileData(controls_xml, fileData) && !fileData.empty()) {
                xmlData.assign(fileData.begin(), fileData.end());
                bReadOk = true;
            }
        }
    }
    else {
        if (ui::FileUtil::ReadFileData(controls_xml, fileData) && !fileData.empty()) {
            xmlData.assign(fileData.begin(), fileData.end());
            bReadOk = true;
        }
        if (!bReadOk || xmlData.empty()) {
            const ui::FilePath themeDefaultPath = ui::GlobalManager::Instance().GetThemeDefaultPath();
            if (!themeDefaultPath.IsEmpty()) {
                ui::FilePath defaultXml = themeDefaultPath;
                defaultXml += "controls/controls.xml";
                fileData.clear();
                if (ui::FileUtil::ReadFileData(defaultXml, fileData) && !fileData.empty()) {
                    xmlData.assign(fileData.begin(), fileData.end());
                    bReadOk = true;
                }
            }
        }
    }
    if (!xmlData.empty()) {
        xml.append((const char*)xmlData.data(), xmlData.size());
    }
    DString xmlU = ui::StringConvert::UTF8ToT(xml);

    // Post task to UI thread
    ui::GlobalManager::Instance().Thread().PostTask(ui::kThreadUI, UiBind(&ControlForm::OnResourceFileLoaded, this, xmlU));
}

void ControlForm::OnResourceFileLoaded(const DString& xml)
{
    if (xml.empty()) {
        return;
    }
    ui::RichEdit* pRichEdit = ui::Find<ui::RichEdit>(this, "edit2");
    if (pRichEdit) {
        pRichEdit->SetText(xml);
        pRichEdit->HomeUp();
    }
    pRichEdit = ui::Find<ui::RichEdit>(this, "edit");
    if (pRichEdit) {
        pRichEdit->SetText(xml);
        pRichEdit->SetFocus();
        pRichEdit->HomeUp();
    }
}

void ControlForm::OnProgressValueChagned(float value)
{
    //The progress range given by the callback is [0, 99), convert it to [0, 100]
    value = value * 100 / 99 + 0.5f;
    auto progress = ui::Find<ui::Progress>(this, "progress");
    if (progress) {
        progress->SetValue(value);
    }

    auto circleprogress = ui::Find<ui::Progress>(this, "circleprogress");
    if (circleprogress)    {
        circleprogress->SetValue(value);
        circleprogress->SetText(ui::StringUtil::Printf("%.0f%%", value));
    }
}

LRESULT ControlForm::OnHotKeyMsg(int32_t hotkeyId, ui::VirtualKeyCode vkCode, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    LRESULT lResult = BaseClass::OnHotKeyMsg(hotkeyId, vkCode, modifierKey, nativeMsg, bHandled);
    bHandled = true;
    if (hotkeyId == SYSTEM_HOTKEY_ID) {
        SetWindowForeground();
        ui::SystemUtil::ShowMessageBox(this, "System HotKey Command Received", "ControlForm::OnHotKeyMsg");
    }
    return lResult;
}

void ControlForm::ShowTrayIcon(bool bShow)
{
    if (bShow) {
        // Create the tray icon
        m_pTrayIcon = ui::TrayIcon::Create(this, "public/caption/logo.ico", "controls(dui)");
        if (m_pTrayIcon != nullptr) {
            // Set the message callback
            m_pTrayIcon->SetMessageCallback([this](ui::TrayIconMessageType msgType, int32_t x, int32_t y)
                {
                    OnTrayIconMessage(msgType, x, y);
                });

            // Show the welcome balloon
            m_pTrayIcon->ShowBalloon("Hint", "App has started!", 3000);
        }

    }
    else {
        m_pTrayIcon.reset();
    }
}

void ControlForm::OnTrayIconMessage(ui::TrayIconMessageType msgType, int32_t x, int32_t y)
{
    switch (msgType)
    {
    case ui::TrayIconMessageType::kLeftClick:
        // Left-click: show/hide the window
        if (IsWindowVisible()) {
            ShowWindow(ui::ShowWindowCommands::kSW_HIDE);
        }
        else {
            ShowWindow(ui::ShowWindowCommands::kSW_SHOW_NORMAL);
            SetWindowForeground();
        }
        break;

    case ui::TrayIconMessageType::kLeftDoubleClick:
        // Left double-click: show the window
        ShowWindow(ui::ShowWindowCommands::kSW_SHOW_NORMAL);
        SetWindowForeground();
        break;

    case ui::TrayIconMessageType::kRightClick:
        // Right-click: show the right-click menu
        ShowTrayMenu(x, y);
        break;
    case ui::TrayIconMessageType::kShowBalloon:
        // The balloon is clicked
        break;
    default:
        break;
    }
}

void ControlForm::ShowTrayMenu(int32_t x, int32_t y)
{
    ui::Window* pParentWnd = IsWindowVisible() ? this : nullptr;//When the window is hidden, do not set the parent window to avoid the menu not being displayed
    ui::Menu* menu = new ui::Menu(pParentWnd, nullptr);
    menu->SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    //Pure code menu: no XML template, menu items are added by code (corresponds to tray_menu.xml)
    menu->ShowMenu("", ui::UiPoint(x, y));
    {
        struct TrayItem { DString name; DString text; };
        const TrayItem items[] = {
            { "tray_menu_item_1", "Tray Menu Item 1" },
            { "tray_menu_item_2", "Tray Menu Item 2" },
            { "tray_menu_item_3", "Tray Menu Item 3" },
            { "tray_menu_item_4", "Tray Menu Item 4" },
            { "tray_menu_exit", "Exit" },
        };
        for (const auto& item : items) {
            auto* pMenuItem = ui::Create<ui::MenuItem>(menu, {{"class", "menu_element"}, {"name", item.name}});
            pMenuItem->SetFixedWidth(ui::UiFixedInt(200), true, true);
    auto* pLabel = ui::Create<ui::Label>(menu, {{"class", "menu_text"}, {"text", item.text}, {"margin", "30,0,0,0"}, {"mouse_enabled", "false"}, {"keyboard_enabled", "false"}});
    ui::Attach(pMenuItem, pLabel);
            menu->AddMenuItem(pMenuItem);
        }
    }

    //Menu item click response
    ui::MenuItem* pMenuItem = ui::Find<ui::MenuItem>(menu, "tray_menu_item_1");
    if (pMenuItem != nullptr) {
        pMenuItem->AttachClick([this](const ui::EventArgs& /*args*/) {
            ui::SystemUtil::ShowMessageBox(this, "tray_menu_item_1 clicked!", "TrayIconTest");
            return true;
            });
    }
    pMenuItem = ui::Find<ui::MenuItem>(menu, "tray_menu_item_2");
    if (pMenuItem != nullptr) {
        pMenuItem->AttachClick([this](const ui::EventArgs& /*args*/) {
            ui::SystemUtil::ShowMessageBox(this, "tray_menu_item_2 clicked!", "TrayIconTest");
            return true;
            });
    }
    pMenuItem = ui::Find<ui::MenuItem>(menu, "tray_menu_item_3");
    if (pMenuItem != nullptr) {
        pMenuItem->AttachClick([this](const ui::EventArgs& /*args*/) {
            ui::SystemUtil::ShowMessageBox(this, "tray_menu_item_3 clicked!", "TrayIconTest");
            return true;
            });
    }
    pMenuItem = ui::Find<ui::MenuItem>(menu, "tray_menu_item_4");
    if (pMenuItem != nullptr) {
        pMenuItem->AttachClick([this](const ui::EventArgs& /*args*/) {
            ui::SystemUtil::ShowMessageBox(this, "tray_menu_item_4 clicked!", "TrayIconTest");
            return true;
            });
    }
    pMenuItem = ui::Find<ui::MenuItem>(menu, "tray_menu_exit");
    if (pMenuItem != nullptr) {
        pMenuItem->AttachClick([this](const ui::EventArgs& /*args*/) {
            this->CloseWnd();
            return true;
            });
    }
}

///////////////////////////////////////////////////////////////////////////
//   AUTO-GENERATED by xml_to_code - DO NOT EDIT BY HAND
//
//   Source XML files:
//     resources/themes/macos26/controls/controls.xml
//
//   Each XML file produces one function:
//     BuildUIFromXml + FileStem
//     BuildUIFromXmlControls(ui::Window* pWindow)
//
//   Functions ending in 'Window' call ui::Attach(pWindow, p0) automatically.
//   Other functions (templates, items) do NOT attach the root - the
//   caller must add the root control to a parent container.
///////////////////////////////////////////////////////////////////////////

#include "dui/dui.h"
#include "dui/Utils/UiBuilder.h"

static void BuildUIFromXmlControls(ui::Window* pWindow) {
    auto& w = *pWindow;
    w.SetWindowSize(1000, 760);
    w.SetEnableSnapLayoutMenu(true);
    w.SetEnableSysMenu(true);
    w.SetSysMenuRect(ui::UiRect(0, 0, 36, 36), true);
    w.SetCaptionRect(ui::UiRect(0, 0, 0, 36), true);
    w.SetUseSystemCaption(false);
    w.SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    w.SetEnableShadowSnap(true);
    w.SetShadowBorderSize(0);
    w.SetSizeBox(ui::UiRect(4, 4, 4, 4), true);
    w.SetWindowMinimumSize(ui::UiSize(400, 320), true);
    w.SetWindowIcon("public/caption/logo.ico");
    w.SetShadowAttached(true);
    // System shadow type: normalize and force non-layered window
    { ui::Shadow::ShadowType supportedType =
          ui::Shadow::GetSupportedShadowType(pWindow, ui::Shadow::ShadowType::kShadowSystemDefault);
      if (supportedType != ui::Shadow::ShadowType::kShadowSystemDefault) {
          w.SetShadowType(supportedType);
      }
      if (ui::Shadow::IsSystemShadowType(supportedType)) {
          w.SetLayeredWindow(false, false);
      }
    }
    w.CenterWindow();
    pWindow->AddClass("check_combo", " bordersize=\"1\" bordercolor=\"splitline_level1\" dropbox=\"padding='0,0,0,0' bkcolor='bk_wnd_lightcolor' border_color='splitline_level1' border_size='0,0,0,0' vscrollbar='true'\" dropbox_item_class=\"width={stretch} height={24} text_padding={20,0,2,1} text_align={left,vcenter}                                normal_image={file='controls_round/checkbox_round_outline_unchecked.svg' valign='center'}                                selected_normal_image={file='controls_round/checkbox_round_outline_checked.svg' valign='center'}\" selected_item_class=\"width={auto} height={22} margin={4,2,4,2} bkcolor={bk_menuitem_selected} text_padding={2,1,2,1}\"");
    auto* p0 = ui::Create<ui::VBox>(pWindow, {{"bkcolor", "bk_wnd_darkcolor"}});
    auto* p1 = ui::Create<ui::HBox>(pWindow, {{"name", "window_caption_bar"}, {"width", "stretch"}, {"height", "36"}, {"bkcolor", "bk_wnd_lightcolor"}});
    auto* p2 = ui::Create<ui::Control>(pWindow, {{"width", "18"}, {"height", "18"}, {"bkimage", "public/caption/logo.svg"}, {"valign", "center"}, {"margin", "8"}});
    ui::Attach(p1, p2);

    auto* p3 = ui::Create<ui::Label>(pWindow, {{"name", "window_title"}, {"text", "Controls"}, {"valign", "center"}, {"margin", "8"}, {"mouse_enabled", "false"}});
    ui::Attach(p1, p3);

    auto* p4 = ui::Create<ui::Control>(pWindow, {{"mouse_enabled", "false"}});
    ui::Attach(p1, p4);

    auto* p5 = ui::Create<ui::Button>(pWindow, {{"class", "btn_wnd_settings_11"}, {"height", "32"}, {"width", "40"}, {"name", "settings"}, {"margin", "0,2,0,2"}, {"tooltip_text", "Settings"}});
    ui::Attach(p1, p5);

    auto* p6 = ui::Create<ui::Button>(pWindow, {{"class", "btn_wnd_min_11"}, {"height", "32"}, {"width", "40"}, {"name", "minbtn"}, {"margin", "0,2,0,2"}, {"tooltip_text", "Minimize"}});
    ui::Attach(p1, p6);

    auto* p7 = ui::Create<ui::Button>(pWindow, {{"class", "btn_wnd_close_11"}, {"height", "stretch"}, {"width", "40"}, {"name", "closebtn"}, {"margin", "0,0,8,2"}, {"tooltip_text", "Close"}});
    ui::Attach(p1, p7);

    ui::Attach(p0, p1);

    auto* p8 = ui::Create<ui::VBox>(pWindow, {{"padding", "10,10,10,10"}});
    auto* p9 = ui::Create<ui::HBox>(pWindow, {{"height", "170"}, {"padding", "0,0,0,10"}});
    auto* p10 = ui::Create<ui::VBox>(pWindow, {{"width", "120"}});
    auto* p11 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_blue_80x30"}, {"text", "blue"}, {"tooltip_text", "ui::Buttons"}});
    ui::Attach(p10, p11);

    auto* p12 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_white_80x30"}, {"text", "white"}});
    ui::Attach(p10, p12);

    auto* p13 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_red_80x30"}, {"text", "red"}});
    ui::Attach(p10, p13);

    auto* p14 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_color_gray"}, {"text", "Control Test"}, {"name", "test_btn"}, {"width", "80"}, {"height", "30"}, {"border_round", "3,3"}, {"margin", "0,2,0,0"}});
    ui::Attach(p10, p14);

    auto* p15 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_color_gray"}, {"name", "animation_btn"}, {"text", "Animation Test"}, {"width", "80"}, {"height", "30"}, {"border_round", "3,3"}, {"margin", "0,2,0,0"}});
    ui::Attach(p10, p15);

    ui::Attach(p9, p10);

    auto* p16 = ui::Create<ui::VBox>(pWindow, {{"width", "120"}});
    auto* p17 = ui::Create<ui::CheckBox>(pWindow, {{"class", "checkbox_1"}, {"font", "system_14"}, {"text", "CheckBox 1"}, {"margin", "0,3,0,3"}, {"selected", "true"}, {"tooltip_text", "ui::Checkbox"}});
    ui::Attach(p16, p17);

    auto* p18 = ui::Create<ui::CheckBox>(pWindow, {{"class", "checkbox_1"}, {"text", "CheckBox 2"}, {"margin", "0,3,0,3"}});
    ui::Attach(p16, p18);

    auto* p19 = ui::Create<ui::CheckBox>(pWindow, {{"class", "checkbox_2"}, {"text", "CheckBox 3"}, {"margin", "0,3,0,3"}});
    ui::Attach(p16, p19);

    auto* p20 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_color_gray"}, {"name", "domodal2"}, {"text", "Modal Dialog"}, {"width", "110"}, {"height", "30"}, {"border_round", "3,3"}, {"margin", "0,12,0,0"}});
    ui::Attach(p16, p20);

    auto* p21 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_color_gray"}, {"name", "show_color_picker"}, {"text", "Color Picker (Modal)"}, {"width", "110"}, {"height", "30"}, {"border_round", "3,3"}, {"margin", "0,2,0,0"}});
    ui::Attach(p16, p21);

    ui::Attach(p9, p16);

    auto* p22 = ui::Create<ui::VBox>(pWindow, {{"width", "160"}, {"margin", "20,0,0,0"}});
    auto* p23 = ui::Create<ui::Option>(pWindow, {{"class", "option_1"}, {"group", "option_group"}, {"text", "Radio 1"}, {"margin", "0,3,0,3"}, {"selected", "true"}, {"tooltip_text", "ui::Option"}});
    ui::Attach(p22, p23);

    auto* p24 = ui::Create<ui::Option>(pWindow, {{"class", "option_1"}, {"group", "option_group"}, {"text", "Radio 2"}, {"margin", "0,3,0,3"}});
    ui::Attach(p22, p24);

    auto* p25 = ui::Create<ui::Option>(pWindow, {{"class", "option_1"}, {"group", "option_group"}, {"text", "Radio 3"}, {"margin", "0,3,0,3"}});
    ui::Attach(p22, p25);

    auto* p26 = ui::Create<ui::HBox>(pWindow, {{"height", "36"}});
    auto* p27 = ui::Create<ui::Label>(pWindow, {{"text", "Tray Icon"}, {"height", "36"}, {"valign", "center"}});
    ui::Attach(p26, p27);

    auto* p28 = ui::Create<ui::CheckBox>(pWindow, {{"class", "checkbox_toggle_1"}, {"name", "checkbox_tray_icon"}, {"selected", "false"}, {"margin", "4,3,0,3"}});
    ui::Attach(p26, p28);

    ui::Attach(p22, p26);

    auto* p29 = ui::Create<ui::HBox>(pWindow, {{"height", "36"}});
    auto* p30 = ui::Create<ui::Label>(pWindow, {{"text", "Switch Example"}, {"height", "36"}, {"valign", "center"}});
    ui::Attach(p29, p30);

    auto* p31 = ui::Create<ui::CheckBox>(pWindow, {{"class", "checkbox_toggle_2"}, {"margin", "4,3,0,3"}});
    ui::Attach(p29, p31);

    ui::Attach(p22, p29);

    ui::Attach(p9, p22);

    auto* p32 = ui::Create<ui::VListBox>(pWindow, {{"class", "list"}, {"name", "list"}, {"padding", "5,3,5,3"}, {"tooltip_text", "ui::VListBox"}});
    auto* p33 = ui::Create<ui::ListBoxItem>(pWindow, {{"class", "listitem"}, {"height", "20"}, {"text", "ui::VListBox::ListBoxItem"}});
    ui::Attach(p32, p33);

    ui::Attach(p9, p32);

    auto* p34 = ui::Create<ui::TreeView>(pWindow, {{"class", "list"}, {"name", "tree"}, {"padding", "5,3,5,3"}, {"margin", "8,0,0,0"}, {"indent", "20"}, {"tooltip_text", "ui::TreeView"}});
    auto* p35 = ui::Create<ui::TreeNode>(pWindow, {{"class", "listitem"}, {"height", "20"}, {"text", "ui::TreeView Parent Node 0"}});
    p34->GetRootNode()->AddChildNode(p35);

    auto* p36 = ui::Create<ui::TreeNode>(pWindow, {{"class", "listitem"}, {"height", "20"}, {"text", "ui::TreeView Parent Node 1"}});
    auto* p37 = ui::Create<ui::TreeNode>(pWindow, {{"class", "listitem"}, {"height", "20"}, {"text", "ui::TreeView::TreeNode 1-1"}});
    p36->AddChildNode(p37);

    auto* p38 = ui::Create<ui::TreeNode>(pWindow, {{"class", "listitem"}, {"height", "20"}, {"text", "ui::TreeView::TreeNode 1-2"}});
    p36->AddChildNode(p38);

    auto* p39 = ui::Create<ui::TreeNode>(pWindow, {{"class", "listitem"}, {"height", "20"}, {"text", "ui::TreeView::TreeNode 1-3"}});
    auto* p40 = ui::Create<ui::TreeNode>(pWindow, {{"class", "listitem"}, {"height", "20"}, {"text", "ui::TreeView::TreeNode 1-3-1"}});
    p39->AddChildNode(p40);

    auto* p41 = ui::Create<ui::TreeNode>(pWindow, {{"class", "listitem"}, {"height", "20"}, {"text", "ui::TreeView::TreeNode 1-3-2"}});
    auto* p42 = ui::Create<ui::TreeNode>(pWindow, {{"class", "listitem"}, {"height", "20"}, {"text", "ui::TreeView::TreeNode 1-3-2-1"}});
    p41->AddChildNode(p42);

    p39->AddChildNode(p41);

    p36->AddChildNode(p39);

    p34->GetRootNode()->AddChildNode(p36);

    auto* p43 = ui::Create<ui::TreeNode>(pWindow, {{"class", "listitem"}, {"height", "20"}, {"text", "ui::TreeView Parent Node 2"}});
    auto* p44 = ui::Create<ui::TreeNode>(pWindow, {{"class", "listitem"}, {"height", "20"}, {"text", "ui::TreeView::TreeNode 2-1"}});
    p43->AddChildNode(p44);

    auto* p45 = ui::Create<ui::TreeNode>(pWindow, {{"class", "listitem"}, {"height", "20"}, {"text", "ui::TreeView::TreeNode 2-2"}});
    p43->AddChildNode(p45);

    auto* p46 = ui::Create<ui::TreeNode>(pWindow, {{"class", "listitem"}, {"height", "20"}, {"text", "ui::TreeView::TreeNode 2-3"}});
    p43->AddChildNode(p46);

    p34->GetRootNode()->AddChildNode(p43);

    ui::Attach(p9, p34);

    ui::Attach(p8, p9);

    auto* p47 = ui::Create<ui::Control>(pWindow, {{"class", "splitline_hor_level1"}});
    ui::Attach(p8, p47);

    auto* p48 = ui::Create<ui::HBox>(pWindow, {{"height", "220"}});
    auto* p49 = ui::Create<ui::VBox>(pWindow, {{"width", "300"}, {"margin", "0,0,10"}});
    auto* p50 = ui::Create<ui::Combo>(pWindow, {{"class", "combo"}, {"name", "combo"}, {"combo_type", "drop_list"}, {"dropbox_size", "0,150"}, {"height", "30"}, {"margin", "0,12,0,0"}, {"tooltip_text", "ui::Combo"}, {"shadow_type", "system_small_round"}});
    ui::Attach(p49, p50);

    auto* p51 = ui::Create<ui::FilterCombo>(pWindow, {{"class", "filter_combo"}, {"name", "filter_combo"}, {"dropbox_size", "0,150"}, {"height", "30"}, {"margin", "0,12,0,0"}, {"tooltip_text", "ui::FilterCombo"}, {"shadow_type", "system_small_round"}});
    ui::Attach(p49, p51);

    auto* p52 = ui::Create<ui::RichEdit>(pWindow, {{"class", "simple prompt simple_border_bottom"}, {"height", "30"}, {"margin", "0,3"}, {"font", "system_14"}, {"text_align", "vcenter"}, {"text_padding", "8,4,8,4"}, {"default_context_menu", "true"}, {"prompt_text", "ui::RichEdit: single line text"}});
    ui::Attach(p49, p52);

    auto* p53 = ui::Create<ui::CheckCombo>(pWindow, {{"class", "check_combo"}, {"name", "check_combo"}, {"height", "30"}, {"margin", "0,3"}, {"tooltip_text", "nim_comp::CheckCombo"}, {"shadow_type", "system_small_round"}});
    ui::Attach(p49, p53);

    auto* p54 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_color_gray"}, {"text", "Fullscreen RichEdit"}, {"name", "rich_edit_fullscreen_btn"}, {"width", "160"}, {"height", "30"}, {"border_round", "3,3"}, {"margin", "0,2,0,0"}});
    ui::Attach(p49, p54);

    ui::Attach(p48, p49);

    auto* p55 = ui::Create<ui::VBox>(pWindow, {{"height", "240"}});
    auto* p56 = ui::Create<ui::HBox>(pWindow, {});
    auto* p57 = ui::Create<ui::VBox>(pWindow, {{"width", "200"}});
    auto* p58 = ui::Create<ui::HBox>(pWindow, {{"margin", "0,10"}, {"height", "32"}});
    auto* p59 = ui::Create<ui::Control>(pWindow, {{"width", "20"}, {"height", "20"}, {"bkimage", "file='public/animation/loading3.json'"}, {"valign", "center"}});
    ui::Attach(p58, p59);

    auto* p60 = ui::Create<ui::Progress>(pWindow, {{"class", "progress_horizontal_blue"}, {"name", "progress"}, {"value", "0"}, {"margin", "10"}, {"tooltip_text", "ui::Progress"}});
    ui::Attach(p58, p60);

    ui::Attach(p57, p58);

    auto* p61 = ui::Create<ui::HBox>(pWindow, {{"margin", "0,0,0,10"}, {"height", "32"}});
    auto* p62 = ui::Create<ui::Control>(pWindow, {{"width", "24"}, {"height", "24"}, {"bkimage", "file='set_speaker.svg' width='20' height='20' valign='center' halign='center'"}, {"valign", "center"}});
    ui::Attach(p61, p62);

    auto* p63 = ui::Create<ui::Slider>(pWindow, {{"class", "slider_horizontal_green"}, {"value", "70"}, {"margin", "10"}, {"tooltip_text", "ui::Slider"}});
    ui::Attach(p61, p63);

    ui::Attach(p57, p61);

    ui::Attach(p56, p57);

    auto* p64 = ui::Create<ui::VBox>(pWindow, {{"width", "90"}, {"margin", "10,0"}});
    auto* p65 = ui::Create<ui::CircleProgress>(pWindow, {{"name", "circleprogress"}, {"circular", "true"}, {"height", "80"}, {"width", "80"}, {"circle_width", "12"}, {"bgcolor", "gray"}, {"fgcolor", "green"}, {"gradient_color", "red"}, {"clockwise", "true"}, {"min", "0"}, {"max", "100"}, {"value", "75"}, {"margin", "10"}, {"text_padding", "10,32,10,10"}, {"normal_text_color", "darkcolor"}, {"indicator", "file='public/progress/indicator.svg' width='12' height='12'"}, {"tooltip_text", "ui::CircleProgress"}});
    ui::Attach(p64, p65);

    ui::Attach(p56, p64);

    auto* p66 = ui::Create<ui::VBox>(pWindow, {{"margin", "4,8,0,0"}});
    auto* p67 = ui::Create<ui::HBox>(pWindow, {{"child_margin", "4"}, {"width", "300"}});
    auto* p68 = ui::Create<ui::Label>(pWindow, {{"text", "Date"}, {"height", "24"}, {"text_align", "vcenter"}, {"hot_text_color", "red"}, {"pushed_text_color", "blue"}});
    ui::Attach(p67, p68);

    auto* p69 = ui::Create<ui::DateTime>(pWindow, {{"width", "100%"}, {"height", "24"}, {"text", "2022-02-01"}});
    ui::Attach(p67, p69);

    auto* p70 = ui::Create<ui::DateTime>(pWindow, {{"width", "120"}, {"height", "24"}, {"edit_format", "date_up_down"}});
    ui::Attach(p67, p70);

    ui::Attach(p66, p67);

    auto* p71 = ui::Create<ui::HBox>(pWindow, {{"child_margin", "4"}, {"width", "320"}});
    auto* p72 = ui::Create<ui::Label>(pWindow, {{"text", "Time"}, {"height", "24"}, {"text_align", "vcenter"}});
    ui::Attach(p71, p72);

    auto* p73 = ui::Create<ui::DateTime>(pWindow, {{"width", "100%"}, {"height", "24"}, {"edit_format", "date_time_up_down"}});
    ui::Attach(p71, p73);

    auto* p74 = ui::Create<ui::DateTime>(pWindow, {{"width", "72"}, {"height", "24"}, {"edit_format", "minute_up_down"}});
    ui::Attach(p71, p74);

    ui::Attach(p66, p71);

    ui::Attach(p56, p66);

    ui::Attach(p55, p56);

    auto* p75 = ui::Create<ui::HBox>(pWindow, {});
    auto* p76 = ui::Create<ui::VBox>(pWindow, {{"margin", "10,0,0,0"}});
    auto* p77 = ui::Create<ui::HBox>(pWindow, {{"child_margin", "10"}});
    auto* p78 = ui::Create<ui::Label>(pWindow, {{"text", "IP Address"}, {"height", "28"}, {"text_align", "vcenter"}});
    ui::Attach(p77, p78);

    auto* p79 = ui::Create<ui::IPAddress>(pWindow, {{"class", "ip_address"}, {"ip", "192.168.0.1"}});
    ui::Attach(p77, p79);

    ui::Attach(p76, p77);

    auto* p80 = ui::Create<ui::HBox>(pWindow, {{"child_margin", "10"}});
    auto* p81 = ui::Create<ui::Label>(pWindow, {{"text", "RichEdit Spin"}, {"height", "28"}, {"text_align", "vcenter"}});
    ui::Attach(p80, p81);

    auto* p82 = ui::Create<ui::RichEdit>(pWindow, {{"class", "simple simple_border rich_edit_spin"}, {"min_number", "-64"}, {"max_number", "64"}, {"limit_text", "3"}, {"text", "0"}});
    ui::Attach(p80, p82);

    ui::Attach(p76, p80);

    ui::Attach(p75, p76);

    auto* p83 = ui::Create<ui::VBox>(pWindow, {{"margin", "10,0,0,0"}});
    auto* p84 = ui::Create<ui::HBox>(pWindow, {{"child_margin", "10"}});
    auto* p85 = ui::Create<ui::HotKey>(pWindow, {{"class", "hot_key"}, {"name", "set_hot_key"}, {"default_text", "None"}});
    ui::Attach(p84, p85);

    auto* p86 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_color_gray"}, {"name", "btn_set_hot_key"}, {"text", "Set Window HotKey"}, {"width", "100%"}, {"height", "28"}, {"border_round", "3,3"}, {"text_align", "vcenter"}});
    ui::Attach(p84, p86);

    ui::Attach(p83, p84);

    auto* p87 = ui::Create<ui::HBox>(pWindow, {{"child_margin", "10"}});
    auto* p88 = ui::Create<ui::HotKey>(pWindow, {{"class", "hot_key"}, {"name", "set_system_hot_key"}, {"default_text", "None"}});
    ui::Attach(p87, p88);

    auto* p89 = ui::Create<ui::Button>(pWindow, {{"class", "btn_global_color_gray"}, {"name", "btn_set_system_hot_key"}, {"text", "Set System HotKey"}, {"width", "100%"}, {"height", "28"}, {"border_round", "3,3"}, {"text_align", "vcenter"}});
    ui::Attach(p87, p89);

    ui::Attach(p83, p87);

    ui::Attach(p75, p83);

    ui::Attach(p55, p75);

    auto* p90 = ui::Create<ui::HBox>(pWindow, {{"child_margin", "10"}});
    auto* p91 = ui::Create<ui::HBox>(pWindow, {{"child_margin", "10"}});
    auto* p92 = ui::Create<ui::RichText>(pWindow, {{"class", "rich_text"}, {"name", "rich_text_demo"}, {"width", "300"}, {"height", "60"}, {"row_spacing_mul", "1.2"}});
    ui::WindowBuilder::ParseRichTextXmlText(_T("<RichText>\n                                RichText:　<a href=\"www.baidu.com\">Click to visit Baidu</a><br/>\n                                RichText:　<a href=\"www.sohu.com\">Click to visit Sohu</a></RichText>"), p92);
    ui::Attach(p91, p92);

    auto* p93 = ui::Create<ui::VBox>(pWindow, {{"width", "80"}});
    auto* p94 = ui::Create<ui::HyperLink>(pWindow, {{"class", "hyper_link"}, {"name", "hyper_link1"}, {"text", "Baidu Homepage"}, {"url", "www.baidu.com"}, {"tooltip_text", "This is a HyperLink control"}});
    ui::Attach(p93, p94);

    auto* p95 = ui::Create<ui::HyperLink>(pWindow, {{"class", "hyper_link"}, {"name", "hyper_link2"}, {"text", "Text Button"}, {"tooltip_text", "This is a HyperLink control"}});
    ui::Attach(p93, p95);

    ui::Attach(p91, p93);

    ui::Attach(p90, p91);

    auto* p96 = ui::Create<ui::HBox>(pWindow, {{"child_margin", "10"}});
    auto* p97 = ui::Create<ui::Label>(pWindow, {{"text", "Line:"}, {"valign", "center"}});
    ui::Attach(p96, p97);

    auto* p98 = ui::Create<ui::VBox>(pWindow, {});
    auto* p99 = ui::Create<ui::Line>(pWindow, {{"vertical", "false"}, {"line_color", "blue"}, {"dash_style", "solid"}, {"line_width", "2"}});
    ui::Attach(p98, p99);

    auto* p100 = ui::Create<ui::Line>(pWindow, {{"vertical", "false"}, {"line_color", "red"}, {"dash_style", "dash"}, {"line_width", "2"}});
    ui::Attach(p98, p100);

    auto* p101 = ui::Create<ui::Line>(pWindow, {{"vertical", "false"}, {"line_color", "green"}, {"dash_style", "dot"}, {"line_width", "2"}});
    ui::Attach(p98, p101);

    auto* p102 = ui::Create<ui::Line>(pWindow, {{"vertical", "false"}, {"line_color", "black"}, {"dash_style", "dash_dot"}, {"line_width", "2"}});
    ui::Attach(p98, p102);

    auto* p103 = ui::Create<ui::Line>(pWindow, {{"vertical", "false"}, {"line_color", "orange"}, {"dash_style", "dash_dot_dot"}, {"line_width", "2"}});
    ui::Attach(p98, p103);

    ui::Attach(p96, p98);

    auto* p104 = ui::Create<ui::VBox>(pWindow, {});
    auto* p105 = ui::Create<ui::HBox>(pWindow, {{"margin", "0,4,0,4"}});
    auto* p106 = ui::Create<ui::Line>(pWindow, {{"vertical", "true"}, {"line_color", "blue"}, {"dash_style", "solid"}, {"line_width", "1"}});
    ui::Attach(p105, p106);

    auto* p107 = ui::Create<ui::Line>(pWindow, {{"vertical", "true"}, {"line_color", "red"}, {"dash_style", "dash"}, {"line_width", "1"}});
    ui::Attach(p105, p107);

    auto* p108 = ui::Create<ui::Line>(pWindow, {{"vertical", "true"}, {"line_color", "green"}, {"dash_style", "dot"}, {"line_width", "1"}});
    ui::Attach(p105, p108);

    auto* p109 = ui::Create<ui::Line>(pWindow, {{"vertical", "true"}, {"line_color", "black"}, {"dash_style", "dash_dot"}, {"line_width", "1"}});
    ui::Attach(p105, p109);

    auto* p110 = ui::Create<ui::Line>(pWindow, {{"vertical", "true"}, {"line_color", "orange"}, {"dash_style", "dash_dot_dot"}, {"line_width", "1"}});
    ui::Attach(p105, p110);

    ui::Attach(p104, p105);

    ui::Attach(p96, p104);

    ui::Attach(p90, p96);

    ui::Attach(p55, p90);

    ui::Attach(p48, p55);

    ui::Attach(p8, p48);

    auto* p111 = ui::Create<ui::Control>(pWindow, {{"class", "splitline_hor_level1"}, {"margin", "0,8,0,4"}});
    ui::Attach(p8, p111);

    auto* p112 = ui::Create<ui::HBox>(pWindow, {});
    auto* p113 = ui::Create<ui::RichEdit>(pWindow, {{"class", "prompt"}, {"name", "edit"}, {"bkcolor", "white"}, {"width", "stretch"}, {"height", "stretch"}, {"hide_selection", "false"}, {"multiline", "true"}, {"auto_vscroll", "true"}, {"vscrollbar", "true"}, {"hscrollbar", "true"}, {"want_return_msg", "true"}, {"rich", "false"}, {"enable_drag_drop", "true"}, {"enable_drop_file", "true"}, {"drop_file_types", ".txt;.md"}});
    ui::Attach(p112, p113);

    auto* p114 = ui::Create<ui::RichEdit>(pWindow, {{"class", "prompt"}, {"name", "edit2"}, {"bkcolor", "white"}, {"width", "stretch"}, {"height", "stretch"}, {"hide_selection", "false"}, {"multiline", "true"}, {"auto_vscroll", "true"}, {"vscrollbar", "true"}, {"hscrollbar", "true"}, {"scrollbar_float", "true"}, {"vscrollbar_class", "vscrollbar2"}, {"hscrollbar_class", "hscrollbar2"}, {"vscrollbar_style", "auto_hide_scroll='false'"}, {"hscrollbar_style", "auto_hide_scroll='false'"}, {"want_return_msg", "true"}, {"rich", "false"}, {"margin", "4,0,0,0"}, {"enable_drag_drop", "true"}, {"enable_drop_file", "false"}});
    ui::Attach(p112, p114);

    ui::Attach(p8, p112);

    ui::Attach(p0, p8);


    ui::Attach(pWindow, p0);
}
