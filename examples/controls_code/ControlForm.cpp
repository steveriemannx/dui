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
    if (auto* pRichText = ui::Find<ui::RichText>(this, DUI_T("rich_text_demo"))) {
        pRichText->SetText(DUI_T("RichText: <a href=\"www.baidu.com\">Click to visit Baidu</a><br/>")
                           DUI_T("RichText: <a href=\"www.sohu.com\">Click to visit Sohu</a>"));
    }
    BindEvents();

    BaseClass::OnInitWindow();
    CenterWindow();
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
#ifdef DUI_BUILD_FOR_WAYLAND
    //Display basic native backend information
    ui::Label* pTitle = ui::Find<ui::Label>(this, DUI_CTR_CAPTION_TITLE);
    if (pTitle != nullptr) {
        DString title = pTitle->GetText();
        DString driverName = GetVideoDriverName();
        DString renderName = GetWindowRenderName();
        DString newTitle = ui::StringUtil::Printf(DUI_T("%s[native backend: VideoDriver:\"%s\", RenderName:\"%s\"]"), title.c_str(), driverName.c_str(), renderName.c_str());
        pTitle->SetText(newTitle);
    }
#endif

    /* Initialize ListBox data */
    ui::ListBox* list = ui::Find<ui::ListBox>(this, DUI_T("list"));
    if (list != nullptr) {
        for (auto i = 0; i < 30; i++)
        {
            auto* element = ui::Create<ui::ListBoxItem>(this, {});
            element->SetText(ui::StringUtil::Printf(DUI_T("ui::VListBox::ListBoxItem %d"), i));
            element->SetClass(DUI_T("listitem"));
            element->SetFixedHeight(ui::UiFixedInt(20), true, true);
            list->AddItem(element);
        }
    }

    ui::TreeView* pTree = ui::Find<ui::TreeView>(this, DUI_T("tree"));
    if (pTree != nullptr) {
        ui::TreeNode* pRootNode = pTree->GetRootNode();
        ASSERT(pRootNode != nullptr);
        if (pRootNode != nullptr) {
            ui::TreeNode* pTestNode = pRootNode->FindChildNodeByText(DUI_T("ui::TreeView Parent Node 2"), true);
            ASSERT(pTestNode != nullptr);
            if (pTestNode != nullptr) {
                auto* pNode0 = ui::Create<ui::TreeNode>(this, {});
                pNode0->SetClass(DUI_T("tree_node"));
                pNode0->SetText(DUI_T("Dynamic Node 0(top)"));
                pTestNode->AddChildNodeAt(pNode0, 0);

                auto* pNode2 = ui::Create<ui::TreeNode>(this, {});
                pNode2->SetClass(DUI_T("tree_node"));
                pNode2->SetText(DUI_T("Dynamic Node 1(end)"));
                pTestNode->AddChildNode(pNode2);

                auto* pNode1 = ui::Create<ui::TreeNode>(this, {});
                pNode1->SetClass(DUI_T("tree_node"));
                pNode1->SetText(DUI_T("Dynamic Node 2(at index 2)"));
                pTestNode->AddChildNodeAt(pNode1, 2);
            }
        }
    }

    //Initialize the Combo data
    ui::Combo* combo = ui::Find<ui::Combo>(this, DUI_T("combo"));
    if (combo != nullptr) {
        ui::TreeView* pTreeView = combo->GetTreeView();
        ui::TreeNode* pTreeNode = pTreeView->GetRootNode();
        for (auto i = 0; i < 10; i++) {
            auto* node = ui::Create<ui::TreeNode>(this, {});
            node->SetClass(DUI_T("tree_node"));
            node->SetText(ui::StringUtil::Printf(DUI_T("ui::Combo::TreeNode %d"), i));
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

    ui::FilterCombo* filterCombo = ui::Find<ui::FilterCombo>(this, DUI_T("filter_combo"));
    if (filterCombo != nullptr) {
        for (auto i = 0; i < 100; i++) {
            filterCombo->AddTextItem(ui::StringUtil::Printf(DUI_T("Item %d FilterCombo"), i));
        }
    }

    ui::CheckCombo* check_combo = ui::Find<ui::CheckCombo>(this, DUI_T("check_combo"));
    if (check_combo != nullptr) {
        check_combo->AddTextItem(DUI_T("Monday"));
        check_combo->AddTextItem(DUI_T("Tuesday"));
        check_combo->AddTextItem(DUI_T("Wednesday"));
        check_combo->AddTextItem(DUI_T("Thursday"));
        check_combo->AddTextItem(DUI_T("Friday"));
        check_combo->AddTextItem(DUI_T("Saturday"));
        check_combo->AddTextItem(DUI_T("Sunday"));
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
    ui::Button* settings = ui::Find<ui::Button>(this, DUI_T("settings"));
    if (settings != nullptr) {
        settings->AttachClick([this, settings](const ui::EventArgs& args) {
            ui::UiRect rect = args.GetSender()->GetPos();
            ui::UiPoint point;
            point.x = rect.left;
            point.y = rect.bottom;
#if defined(DUI_BUILD_FOR_X11)
            // GetPos() is relative to the caption HBox; include the window shadow origin.
            ui::UiPadding shadow;
            GetShadowCorner(shadow);
            point.Offset(shadow.left, shadow.top);
#endif
            ClientToScreen(point);

            //Show the menu and keep the settings button in the Push state
            ShowPopupMenu(point, settings);
            return true;
            });
    }

    //Register a context menu, demonstrating the feature (both methods can register a context menu)
    AttachRichEditEvents(ui::Find<ui::RichEdit>(this, DUI_T("edit")));
    AttachRichEditEvents(ui::Find<ui::RichEdit>(this, DUI_T("edit2")));

    //Show the color picker of a modal dialog
    ui::Button* pShowColorPicker = ui::Find<ui::Button>(this, DUI_T("show_color_picker"));
    if (pShowColorPicker != nullptr) {
        pShowColorPicker->AttachClick([this](const ui::EventArgs& args) {
            ShowColorPicker(true);
            return true;
            });
    }

    //Show a modal dialog
    ui::Button* pShowModal = ui::Find<ui::Button>(this, DUI_T("domodal2"));
    if (pShowModal != nullptr) {
        pShowModal->AttachClick([this](const ui::EventArgs& args) {
            ShowDoModalDlg();
            return true;
            });
    }

    //RichText displays hyperlinks
    ui::RichText* pRichText = ui::Find<ui::RichText>(this, DUI_T("rich_text_demo"));
    if (pRichText != nullptr) {
        pRichText->AttachLinkClick([this](const ui::EventArgs& args) {
            const DString::value_type* url = (const DString::value_type*)args.wParam;
            if (url != nullptr) {
                ui::SystemUtil::ShowMessageBox(this, url, DUI_T("RichText Click HyperLink"));
            }
            return true;
            });
    }

    //HyperLink control
    ui::HyperLink* pHyperLink = ui::Find<ui::HyperLink>(this, DUI_T("hyper_link1"));
    if (pHyperLink != nullptr) {
        pHyperLink->AttachLinkClick([this](const ui::EventArgs& args) {
            const DString::value_type* url = (const DString::value_type*)args.wParam;
            if (url != nullptr) {
                ui::SystemUtil::ShowMessageBox(this, url, DUI_T("HyperLink Click HyperLink"));
            }
            return true;
            });
    }

    pHyperLink = ui::Find<ui::HyperLink>(this, DUI_T("hyper_link2"));
    if (pHyperLink != nullptr) {
        pHyperLink->AttachLinkClick([this](const ui::EventArgs& /*args*/) {
            ui::SystemUtil::ShowMessageBox(this, DUI_T("Text Button Event Response"), DUI_T("HyperLink Click"));
            return true;
            });
    }

    //Hotkey settings
    ui::HotKey* pHotKey = ui::Find<ui::HotKey>(this, DUI_T("set_hot_key"));
    ui::Button* pHotKeyButton = ui::Find<ui::Button>(this, DUI_T("btn_set_hot_key"));
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

    pHotKey = ui::Find<ui::HotKey>(this, DUI_T("set_system_hot_key"));
    pHotKeyButton = ui::Find<ui::Button>(this, DUI_T("btn_set_system_hot_key"));
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
    ui::Button* pTestBtn = ui::Find<ui::Button>(this, DUI_T("test_btn"));
    if (pTestBtn != nullptr) {
        pTestBtn->AttachClick([this](const ui::EventArgs&) {
            ShowTestWindow();
            return true;
            });
    }

    //Response function of the animation test button
    ui::Button* pAnimationBtn = ui::Find<ui::Button>(this, DUI_T("animation_btn"));
    if (pAnimationBtn != nullptr) {
        pAnimationBtn->AttachClick([this](const ui::EventArgs&) {
            ShowAnimationWindow();
            return true;
            });
    }

    //Tray icon functionality
    ui::CheckBox* pTrayIconCheckBox = ui::Find<ui::CheckBox>(this, DUI_T("checkbox_tray_icon"));
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
        else if (args.wParam == ui::kControlDropTypeWayland) {
            const ui::ControlDropData_Wayland* dropData = (const ui::ControlDropData_Wayland*)args.lParam;
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
    ui::Button* pFullscreenBtn = ui::Find<ui::Button>(this, DUI_T("rich_edit_fullscreen_btn"));
    if (pFullscreenBtn != nullptr) {
        pFullscreenBtn->AttachClick([this](const ui::EventArgs&) {
            ui::Control* pControl = ui::Find<ui::Control>(this, DUI_T("edit"));
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
        ui::RichEdit* pEdit = ui::Find<ui::RichEdit>(this, DUI_T("edit"));
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
    simpleWnd.InitSkin(DUI_T("controls"), DUI_T("about.xml"));
    ui::WindowCreateParam createParam;
    createParam.m_dwStyle = ui::kWS_POPUP;
    createParam.m_dwExStyle = ui::kWS_EX_LAYERED;
    createParam.m_windowTitle = DUI_T("AboutForm");
    createParam.m_bCenterWindow = true;
    simpleWnd.DoModal(this, createParam);
}

void ControlForm::ShowTestWindow()
{
    TestForm* testForm = new TestForm();
    ui::WindowCreateParam createParam;
    createParam.m_dwStyle = ui::kWS_POPUP;
    createParam.m_dwExStyle = ui::kWS_EX_LAYERED;
    createParam.m_windowTitle = DUI_T("TestWindow");
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
    createParam.m_windowTitle = DUI_T("AnimationWindow");
    createParam.m_bCenterWindow = true;
    testForm->CreateWnd(this, createParam);
    testForm->ShowModalFake();
}

void ControlForm::ShowPopupMenu(const ui::UiPoint& point, ui::Control* pRelatedControl)
{
    ui::Menu* menu = new ui::Menu(this, pRelatedControl);//A parent window is required, otherwise the program status bar becomes inactive when the menu pops up
    menu->SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    menu->SetSkinFolder(GetResourcePath().ToString());
    DString xml(DUI_T("menu/settings_menu.xml"));
    menu->ShowMenu(xml, point);

    //Add a submenu item to the second-level menu
    ui::MenuItem* menu_fourth = ui::Find<ui::MenuItem>(menu, DUI_T("fourth"));
    if (menu_fourth != nullptr) {
        auto* menu_item = ui::Create<ui::MenuItem>(menu, {{DUI_T("text"), DUI_T("Dynamically created")}, {DUI_T("class"), DUI_T("menu_element")}});
        menu_item->SetFixedWidth(ui::UiFixedInt(200), true, true);
        menu_item->SetFontId(DUI_T("system_14"));
        menu_item->SetTextPadding({ 20, 0, 20, 0 }, true);
        menu_fourth->AddSubMenuItemAt(menu_item, 1);//After adding, the resource is managed by the menu
    }
    

    //Add a submenu item to the first-level menu
    /*
    menu_item = new ui::MenuItem(menu);
    menu_item->SetWindow(menu);
    menu_item->SetText(DUI_T("Dynamically created"));
    menu_item->SetClass(DUI_T("menu_element"));
    menu_item->SetFixedWidth(180);
    menu_item->SetFontId(DUI_T("system_14"));
    menu_item->SetTextPadding({ 20, 0, 20, 0 });
    menu->AddMenuItemAt(menu_item, 4);//After adding, the resource is managed by the menu
    */

    //Demonstration of the checkbox menu item functionality
    static bool s_is_checked_01_flag = false;
    bool& flag = s_is_checked_01_flag;
    ui::MenuItem* menu_check_01 = ui::Find<ui::MenuItem>(menu, DUI_T("menu_check_01"));
    if (menu_check_01 != nullptr) {
        menu_check_01->AttachClick([&flag](const ui::EventArgs& args) {
            flag = true;
            return true;
            });
    }
    ui::CheckBox* menuCheckBox01 = ui::Find<ui::CheckBox>(menu, DUI_T("menu_checkbox_01"));
    if (menuCheckBox01 != nullptr) {
        menuCheckBox01->Selected(s_is_checked_01_flag);
    }

    ui::MenuItem* menu_check_02 = ui::Find<ui::MenuItem>(menu, DUI_T("menu_check_02"));
    if (menu_check_02 != nullptr) {
        menu_check_02->AttachClick([&flag](const ui::EventArgs& args) {
            flag = false;
            return true;
            });
    }

    ui::CheckBox* menuCheckBox02 = ui::Find<ui::CheckBox>(menu, DUI_T("menu_checkbox_02"));
    if (menuCheckBox02 != nullptr) {
        menuCheckBox02->Selected(!s_is_checked_01_flag);
    }


    /* About menu */
    ui::MenuItem* menu_about = menu->GetMenuItemByName(DUI_T("about"));
    if (menu_about != nullptr) {
        menu_about->AttachClick([this](const ui::EventArgs&) {
            ui::GlobalManager::Instance().Thread().PostTask(ui::kThreadUI,
                [this]() {
                    AboutForm* about_form = new AboutForm();
                    ui::WindowCreateParam createParam;
                    createParam.m_dwStyle = ui::kWS_OVERLAPPEDWINDOW;
                    createParam.m_dwExStyle = ui::kWS_EX_LAYERED;
                    createParam.m_windowTitle = DUI_T("AboutForm");
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
    controls_xml += DUI_T("controls/controls.xml");

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
                defaultXml += DUI_T("controls/controls.xml");
            bReadOk = ui::GlobalManager::Instance().MemoryResources().GetData(defaultXml, xmlData);
            }
            if (!bReadOk || xmlData.empty()) {
                xmlData.clear();
            bReadOk = ui::GlobalManager::Instance().MemoryResources().GetData(ui::FilePath(DUI_T("themes/default/controls/controls.xml")), xmlData);
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
                defaultXml += DUI_T("controls/controls.xml");
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
    ui::RichEdit* pRichEdit = ui::Find<ui::RichEdit>(this, DUI_T("edit2"));
    if (pRichEdit) {
        pRichEdit->SetText(xml);
        pRichEdit->HomeUp();
    }
    pRichEdit = ui::Find<ui::RichEdit>(this, DUI_T("edit"));
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
    auto progress = ui::Find<ui::Progress>(this, DUI_T("progress"));
    if (progress) {
        progress->SetValue(value);
    }

    auto circleprogress = ui::Find<ui::Progress>(this, DUI_T("circleprogress"));
    if (circleprogress)    {
        circleprogress->SetValue(value);
        circleprogress->SetText(ui::StringUtil::Printf(DUI_T("%.0f%%"), value));
    }
}

LRESULT ControlForm::OnHotKeyMsg(int32_t hotkeyId, ui::VirtualKeyCode vkCode, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    LRESULT lResult = BaseClass::OnHotKeyMsg(hotkeyId, vkCode, modifierKey, nativeMsg, bHandled);
    bHandled = true;
    if (hotkeyId == SYSTEM_HOTKEY_ID) {
        SetWindowForeground();
        ui::SystemUtil::ShowMessageBox(this, DUI_T("System HotKey Command Received"), DUI_T("ControlForm::OnHotKeyMsg"));
    }
    return lResult;
}

void ControlForm::ShowTrayIcon(bool bShow)
{
    if (bShow) {
        // Create the tray icon
        m_pTrayIcon = ui::TrayIcon::Create(this, DUI_T("public/caption/logo.ico"), DUI_T("controls(dui)"));
        if (m_pTrayIcon != nullptr) {
            // Set the message callback
            m_pTrayIcon->SetMessageCallback([this](ui::TrayIconMessageType msgType, int32_t x, int32_t y)
                {
                    OnTrayIconMessage(msgType, x, y);
                });

            // Show the welcome balloon
            m_pTrayIcon->ShowBalloon(DUI_T("Hint"), DUI_T("App has started!"), 3000);
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
    menu->ShowMenu(DUI_T(""), ui::UiPoint(x, y));
    {
        struct TrayItem { DString name; DString text; };
        const TrayItem items[] = {
            { DUI_T("tray_menu_item_1"), DUI_T("Tray Menu Item 1") },
            { DUI_T("tray_menu_item_2"), DUI_T("Tray Menu Item 2") },
            { DUI_T("tray_menu_item_3"), DUI_T("Tray Menu Item 3") },
            { DUI_T("tray_menu_item_4"), DUI_T("Tray Menu Item 4") },
            { DUI_T("tray_menu_exit"), DUI_T("Exit") },
        };
        for (const auto& item : items) {
            auto* pMenuItem = ui::Create<ui::MenuItem>(menu, {{DUI_T("class"), DUI_T("menu_element")}, {DUI_T("name"), item.name}});
            pMenuItem->SetFixedWidth(ui::UiFixedInt(200), true, true);
    auto* pLabel = ui::Create<ui::Label>(menu, {{DUI_T("class"), DUI_T("menu_text")}, {DUI_T("text"), item.text}, {DUI_T("margin"), DUI_T("30,0,0,0")}, {DUI_T("mouse_enabled"), DUI_T("false")}, {DUI_T("keyboard_enabled"), DUI_T("false")}});
    ui::Attach(pMenuItem, pLabel);
            menu->AddMenuItem(pMenuItem);
        }
    }

    //Menu item click response
    ui::MenuItem* pMenuItem = ui::Find<ui::MenuItem>(menu, DUI_T("tray_menu_item_1"));
    if (pMenuItem != nullptr) {
        pMenuItem->AttachClick([this](const ui::EventArgs& /*args*/) {
            ui::SystemUtil::ShowMessageBox(this, DUI_T("tray_menu_item_1 clicked!"), DUI_T("TrayIconTest"));
            return true;
            });
    }
    pMenuItem = ui::Find<ui::MenuItem>(menu, DUI_T("tray_menu_item_2"));
    if (pMenuItem != nullptr) {
        pMenuItem->AttachClick([this](const ui::EventArgs& /*args*/) {
            ui::SystemUtil::ShowMessageBox(this, DUI_T("tray_menu_item_2 clicked!"), DUI_T("TrayIconTest"));
            return true;
            });
    }
    pMenuItem = ui::Find<ui::MenuItem>(menu, DUI_T("tray_menu_item_3"));
    if (pMenuItem != nullptr) {
        pMenuItem->AttachClick([this](const ui::EventArgs& /*args*/) {
            ui::SystemUtil::ShowMessageBox(this, DUI_T("tray_menu_item_3 clicked!"), DUI_T("TrayIconTest"));
            return true;
            });
    }
    pMenuItem = ui::Find<ui::MenuItem>(menu, DUI_T("tray_menu_item_4"));
    if (pMenuItem != nullptr) {
        pMenuItem->AttachClick([this](const ui::EventArgs& /*args*/) {
            ui::SystemUtil::ShowMessageBox(this, DUI_T("tray_menu_item_4 clicked!"), DUI_T("TrayIconTest"));
            return true;
            });
    }
    pMenuItem = ui::Find<ui::MenuItem>(menu, DUI_T("tray_menu_exit"));
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
#if defined(DUI_BUILD_FOR_LINUX)
    w.SetShadowType(ui::Shadow::ShadowType::kShadowDrawDefault);
#else
    w.SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
#endif
    w.SetEnableShadowSnap(true);
    w.SetShadowBorderSize(0);
    w.SetSizeBox(ui::UiRect(4, 4, 4, 4), true);
    w.SetWindowMinimumSize(ui::UiSize(400, 320), true);
    w.SetWindowIcon(DUI_T("public/caption/logo.ico"));
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
    pWindow->AddClass(DUI_T("check_combo"), DUI_T(" bordersize=\"1\" bordercolor=\"splitline_level1\" dropbox=\"padding='0,0,0,0' bkcolor='bk_wnd_lightcolor' border_color='splitline_level1' border_size='0,0,0,0' vscrollbar='true'\" dropbox_item_class=\"width={stretch} height={24} text_padding={20,0,2,1} text_align={left,vcenter}                                normal_image={file='controls_round/checkbox_round_outline_unchecked.svg' valign='center'}                                selected_normal_image={file='controls_round/checkbox_round_outline_checked.svg' valign='center'}\" selected_item_class=\"width={auto} height={22} margin={4,2,4,2} bkcolor={bk_menuitem_selected} text_padding={2,1,2,1}\""));
    auto* p0 = ui::Create<ui::VBox>(pWindow, {{DUI_T("bkcolor"), DUI_T("bk_wnd_darkcolor")}});
    auto* p1 = ui::Create<ui::HBox>(pWindow, {{DUI_T("name"), DUI_T("window_caption_bar")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("36")}, {DUI_T("bkcolor"), DUI_T("bk_wnd_lightcolor")}});
    auto* p2 = ui::Create<ui::Control>(pWindow, {{DUI_T("width"), DUI_T("18")}, {DUI_T("height"), DUI_T("18")}, {DUI_T("bkimage"), DUI_T("public/caption/logo.svg")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8")}});
    ui::Attach(p1, p2);

    auto* p3 = ui::Create<ui::Label>(pWindow, {{DUI_T("name"), DUI_CTR_CAPTION_TITLE}, {DUI_T("text"), DUI_T("Controls")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8")}, {DUI_T("mouse_enabled"), DUI_T("false")}});
    ui::Attach(p1, p3);

    auto* p4 = ui::Create<ui::Control>(pWindow, {{DUI_T("mouse_enabled"), DUI_T("false")}});
    ui::Attach(p1, p4);

    auto* p5 = ui::Create<ui::Button>(pWindow, {{DUI_T("class"), DUI_T("btn_wnd_settings_11")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("name"), DUI_T("settings")}, {DUI_T("margin"), DUI_T("0,2,0,2")}, {DUI_T("tooltip_text"), DUI_T("Settings")}});
    ui::Attach(p1, p5);

    auto* p6 = ui::Create<ui::Button>(pWindow, {{DUI_T("class"), DUI_T("btn_wnd_min_11")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("name"), DUI_T("minbtn")}, {DUI_T("margin"), DUI_T("0,2,0,2")}, {DUI_T("tooltip_text"), DUI_T("Minimize")}});
    ui::Attach(p1, p6);

    auto* pMaxBox = ui::Create<ui::Box>(pWindow, {{DUI_T("height"), DUI_T("stretch")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,2,0,2")} });
    auto* pMaxBtn = ui::Create<ui::Button>(pWindow, {{DUI_T("class"), DUI_T("btn_wnd_max_11")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("name"), DUI_T("maxbtn")}, {DUI_T("tooltip_text"), DUI_T("Maximize")} });
    ui::Attach(pMaxBox, pMaxBtn);
    auto* pRestoreBtn = ui::Create<ui::Button>(pWindow, {{DUI_T("class"), DUI_T("btn_wnd_restore_11")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("name"), DUI_T("restorebtn")}, {DUI_T("visible"), DUI_T("false")}, {DUI_T("tooltip_text"), DUI_T("Restore")} });
    ui::Attach(pMaxBox, pRestoreBtn);
    ui::Attach(p1, pMaxBox);

    auto* pCloseBtn = ui::Create<ui::Button>(pWindow, {{DUI_T("class"), DUI_T("btn_wnd_close_11")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("name"), DUI_T("closebtn")}, {DUI_T("margin"), DUI_T("0,0,8,2")}, {DUI_T("tooltip_text"), DUI_T("Close")}});
    ui::Attach(p1, pCloseBtn);

    ui::Attach(p0, p1);

    auto* p8 = ui::Create<ui::VBox>(pWindow, {{DUI_T("padding"), DUI_T("10,10,10,10")}});
    auto* p9 = ui::Create<ui::HBox>(pWindow, {{DUI_T("height"), DUI_T("170")}, {DUI_T("padding"), DUI_T("0,0,0,10")}});
    auto* p10 = ui::Create<ui::VBox>(pWindow, {{DUI_T("width"), DUI_T("120")}});
    auto* p11 = ui::Create<ui::Button>(pWindow, {{DUI_T("class"), DUI_T("btn_global_blue_80x30")}, {DUI_T("text"), DUI_T("blue")}, {DUI_T("tooltip_text"), DUI_T("ui::Buttons")}});
    ui::Attach(p10, p11);

    auto* p12 = ui::Create<ui::Button>(pWindow, {{DUI_T("class"), DUI_T("btn_global_white_80x30")}, {DUI_T("text"), DUI_T("white")}});
    ui::Attach(p10, p12);

    auto* p13 = ui::Create<ui::Button>(pWindow, {{DUI_T("class"), DUI_T("btn_global_red_80x30")}, {DUI_T("text"), DUI_T("red")}});
    ui::Attach(p10, p13);

    auto* p14 = ui::Create<ui::Button>(pWindow, {{DUI_T("class"), DUI_T("btn_global_color_gray")}, {DUI_T("text"), DUI_T("Control Test")}, {DUI_T("name"), DUI_T("test_btn")}, {DUI_T("width"), DUI_T("80")}, {DUI_T("height"), DUI_T("30")}, {DUI_T("border_round"), DUI_T("3,3")}, {DUI_T("margin"), DUI_T("0,2,0,0")}});
    ui::Attach(p10, p14);

    auto* p15 = ui::Create<ui::Button>(pWindow, {{DUI_T("class"), DUI_T("btn_global_color_gray")}, {DUI_T("name"), DUI_T("animation_btn")}, {DUI_T("text"), DUI_T("Animation Test")}, {DUI_T("width"), DUI_T("80")}, {DUI_T("height"), DUI_T("30")}, {DUI_T("border_round"), DUI_T("3,3")}, {DUI_T("margin"), DUI_T("0,2,0,0")}});
    ui::Attach(p10, p15);

    ui::Attach(p9, p10);

    auto* p16 = ui::Create<ui::VBox>(pWindow, {{DUI_T("width"), DUI_T("120")}});
    auto* p17 = ui::Create<ui::CheckBox>(pWindow, {{DUI_T("class"), DUI_T("checkbox_1")}, {DUI_T("font"), DUI_T("system_14")}, {DUI_T("text"), DUI_T("CheckBox 1")}, {DUI_T("margin"), DUI_T("0,3,0,3")}, {DUI_T("selected"), DUI_T("true")}, {DUI_T("tooltip_text"), DUI_T("ui::Checkbox")}});
    ui::Attach(p16, p17);

    auto* p18 = ui::Create<ui::CheckBox>(pWindow, {{DUI_T("class"), DUI_T("checkbox_1")}, {DUI_T("text"), DUI_T("CheckBox 2")}, {DUI_T("margin"), DUI_T("0,3,0,3")}});
    ui::Attach(p16, p18);

    auto* p19 = ui::Create<ui::CheckBox>(pWindow, {{DUI_T("class"), DUI_T("checkbox_2")}, {DUI_T("text"), DUI_T("CheckBox 3")}, {DUI_T("margin"), DUI_T("0,3,0,3")}});
    ui::Attach(p16, p19);

    auto* p20 = ui::Create<ui::Button>(pWindow, {{DUI_T("class"), DUI_T("btn_global_color_gray")}, {DUI_T("name"), DUI_T("domodal2")}, {DUI_T("text"), DUI_T("Modal Dialog")}, {DUI_T("width"), DUI_T("110")}, {DUI_T("height"), DUI_T("30")}, {DUI_T("border_round"), DUI_T("3,3")}, {DUI_T("margin"), DUI_T("0,12,0,0")}});
    ui::Attach(p16, p20);

    auto* p21 = ui::Create<ui::Button>(pWindow, {{DUI_T("class"), DUI_T("btn_global_color_gray")}, {DUI_T("name"), DUI_T("show_color_picker")}, {DUI_T("text"), DUI_T("Color Picker (Modal)")}, {DUI_T("width"), DUI_T("110")}, {DUI_T("height"), DUI_T("30")}, {DUI_T("border_round"), DUI_T("3,3")}, {DUI_T("margin"), DUI_T("0,2,0,0")}});
    ui::Attach(p16, p21);

    ui::Attach(p9, p16);

    auto* p22 = ui::Create<ui::VBox>(pWindow, {{DUI_T("width"), DUI_T("160")}, {DUI_T("margin"), DUI_T("20,0,0,0")}});
    auto* p23 = ui::Create<ui::Option>(pWindow, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("option_group")}, {DUI_T("text"), DUI_T("Radio 1")}, {DUI_T("margin"), DUI_T("0,3,0,3")}, {DUI_T("selected"), DUI_T("true")}, {DUI_T("tooltip_text"), DUI_T("ui::Option")}});
    ui::Attach(p22, p23);

    auto* p24 = ui::Create<ui::Option>(pWindow, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("option_group")}, {DUI_T("text"), DUI_T("Radio 2")}, {DUI_T("margin"), DUI_T("0,3,0,3")}});
    ui::Attach(p22, p24);

    auto* p25 = ui::Create<ui::Option>(pWindow, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("option_group")}, {DUI_T("text"), DUI_T("Radio 3")}, {DUI_T("margin"), DUI_T("0,3,0,3")}});
    ui::Attach(p22, p25);

    auto* p26 = ui::Create<ui::HBox>(pWindow, {{DUI_T("height"), DUI_T("36")}});
    auto* p27 = ui::Create<ui::Label>(pWindow, {{DUI_T("text"), DUI_T("Tray Icon")}, {DUI_T("height"), DUI_T("36")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(p26, p27);

    auto* p28 = ui::Create<ui::CheckBox>(pWindow, {{DUI_T("class"), DUI_T("checkbox_toggle_1")}, {DUI_T("name"), DUI_T("checkbox_tray_icon")}, {DUI_T("selected"), DUI_T("false")}, {DUI_T("margin"), DUI_T("4,3,0,3")}});
    ui::Attach(p26, p28);

    ui::Attach(p22, p26);

    auto* p29 = ui::Create<ui::HBox>(pWindow, {{DUI_T("height"), DUI_T("36")}});
    auto* p30 = ui::Create<ui::Label>(pWindow, {{DUI_T("text"), DUI_T("Switch Example")}, {DUI_T("height"), DUI_T("36")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(p29, p30);

    auto* p31 = ui::Create<ui::CheckBox>(pWindow, {{DUI_T("class"), DUI_T("checkbox_toggle_2")}, {DUI_T("margin"), DUI_T("4,3,0,3")}});
    ui::Attach(p29, p31);

    ui::Attach(p22, p29);

    ui::Attach(p9, p22);

    auto* p32 = ui::Create<ui::VListBox>(pWindow, {{DUI_T("class"), DUI_T("list")}, {DUI_T("name"), DUI_T("list")}, {DUI_T("padding"), DUI_T("5,3,5,3")}, {DUI_T("tooltip_text"), DUI_T("ui::VListBox")}});
    auto* p33 = ui::Create<ui::ListBoxItem>(pWindow, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("ui::VListBox::ListBoxItem")}});
    ui::Attach(p32, p33);

    ui::Attach(p9, p32);

    auto* p34 = ui::Create<ui::TreeView>(pWindow, {{DUI_T("class"), DUI_T("list")}, {DUI_T("name"), DUI_T("tree")}, {DUI_T("padding"), DUI_T("5,3,5,3")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("indent"), DUI_T("20")}, {DUI_T("tooltip_text"), DUI_T("ui::TreeView")}});
    auto* p35 = ui::Create<ui::TreeNode>(pWindow, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("ui::TreeView Parent Node 0")}});
    p34->GetRootNode()->AddChildNode(p35);

    auto* p36 = ui::Create<ui::TreeNode>(pWindow, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("ui::TreeView Parent Node 1")}});
    auto* p37 = ui::Create<ui::TreeNode>(pWindow, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("ui::TreeView::TreeNode 1-1")}});
    p36->AddChildNode(p37);

    auto* p38 = ui::Create<ui::TreeNode>(pWindow, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("ui::TreeView::TreeNode 1-2")}});
    p36->AddChildNode(p38);

    auto* p39 = ui::Create<ui::TreeNode>(pWindow, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("ui::TreeView::TreeNode 1-3")}});
    auto* p40 = ui::Create<ui::TreeNode>(pWindow, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("ui::TreeView::TreeNode 1-3-1")}});
    p39->AddChildNode(p40);

    auto* p41 = ui::Create<ui::TreeNode>(pWindow, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("ui::TreeView::TreeNode 1-3-2")}});
    auto* p42 = ui::Create<ui::TreeNode>(pWindow, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("ui::TreeView::TreeNode 1-3-2-1")}});
    p41->AddChildNode(p42);

    p39->AddChildNode(p41);

    p36->AddChildNode(p39);

    p34->GetRootNode()->AddChildNode(p36);

    auto* p43 = ui::Create<ui::TreeNode>(pWindow, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("ui::TreeView Parent Node 2")}});
    auto* p44 = ui::Create<ui::TreeNode>(pWindow, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("ui::TreeView::TreeNode 2-1")}});
    p43->AddChildNode(p44);

    auto* p45 = ui::Create<ui::TreeNode>(pWindow, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("ui::TreeView::TreeNode 2-2")}});
    p43->AddChildNode(p45);

    auto* p46 = ui::Create<ui::TreeNode>(pWindow, {{DUI_T("class"), DUI_T("listitem")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("text"), DUI_T("ui::TreeView::TreeNode 2-3")}});
    p43->AddChildNode(p46);

    p34->GetRootNode()->AddChildNode(p43);

    ui::Attach(p9, p34);

    ui::Attach(p8, p9);

    auto* p47 = ui::Create<ui::Control>(pWindow, {{DUI_T("class"), DUI_T("splitline_hor_level1")}});
    ui::Attach(p8, p47);

    auto* p48 = ui::Create<ui::HBox>(pWindow, {{DUI_T("height"), DUI_T("220")}});
    auto* p49 = ui::Create<ui::VBox>(pWindow, {{DUI_T("width"), DUI_T("300")}, {DUI_T("margin"), DUI_T("0,0,10")}});
    auto* p50 = ui::Create<ui::Combo>(pWindow, {{DUI_T("class"), DUI_T("combo")}, {DUI_T("name"), DUI_T("combo")}, {DUI_T("combo_type"), DUI_T("drop_list")}, {DUI_T("dropbox_size"), DUI_T("0,150")}, {DUI_T("height"), DUI_T("30")}, {DUI_T("margin"), DUI_T("0,12,0,0")}, {DUI_T("tooltip_text"), DUI_T("ui::Combo")}, {DUI_T("shadow_type"), DUI_T("system_small_round")}});
    ui::Attach(p49, p50);

    auto* p51 = ui::Create<ui::FilterCombo>(pWindow, {{DUI_T("class"), DUI_T("filter_combo")}, {DUI_T("name"), DUI_T("filter_combo")}, {DUI_T("dropbox_size"), DUI_T("0,150")}, {DUI_T("height"), DUI_T("30")}, {DUI_T("margin"), DUI_T("0,12,0,0")}, {DUI_T("tooltip_text"), DUI_T("ui::FilterCombo")}, {DUI_T("shadow_type"), DUI_T("system_small_round")}});
    ui::Attach(p49, p51);

    auto* p52 = ui::Create<ui::RichEdit>(pWindow, {{DUI_T("class"), DUI_T("simple prompt simple_border_bottom")}, {DUI_T("height"), DUI_T("30")}, {DUI_T("margin"), DUI_T("0,3")}, {DUI_T("font"), DUI_T("system_14")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("text_padding"), DUI_T("8,4,8,4")}, {DUI_T("default_context_menu"), DUI_T("true")}, {DUI_T("prompt_text"), DUI_T("ui::RichEdit: single line text")}});
    ui::Attach(p49, p52);

    auto* p53 = ui::Create<ui::CheckCombo>(pWindow, {{DUI_T("class"), DUI_T("check_combo")}, {DUI_T("name"), DUI_T("check_combo")}, {DUI_T("height"), DUI_T("30")}, {DUI_T("margin"), DUI_T("0,3")}, {DUI_T("tooltip_text"), DUI_T("nim_comp::CheckCombo")}, {DUI_T("shadow_type"), DUI_T("system_small_round")}});
    ui::Attach(p49, p53);

    auto* p54 = ui::Create<ui::Button>(pWindow, {{DUI_T("class"), DUI_T("btn_global_color_gray")}, {DUI_T("text"), DUI_T("Fullscreen RichEdit")}, {DUI_T("name"), DUI_T("rich_edit_fullscreen_btn")}, {DUI_T("width"), DUI_T("160")}, {DUI_T("height"), DUI_T("30")}, {DUI_T("border_round"), DUI_T("3,3")}, {DUI_T("margin"), DUI_T("0,2,0,0")}});
    ui::Attach(p49, p54);

    ui::Attach(p48, p49);

    auto* p55 = ui::Create<ui::VBox>(pWindow, {{DUI_T("height"), DUI_T("240")}});
    auto* p56 = ui::Create<ui::HBox>(pWindow, {});
    auto* p57 = ui::Create<ui::VBox>(pWindow, {{DUI_T("width"), DUI_T("200")}});
    auto* p58 = ui::Create<ui::HBox>(pWindow, {{DUI_T("margin"), DUI_T("0,10")}, {DUI_T("height"), DUI_T("32")}});
    auto* p59 = ui::Create<ui::Control>(pWindow, {{DUI_T("width"), DUI_T("20")}, {DUI_T("height"), DUI_T("20")}, {DUI_T("bkimage"), DUI_T("file='public/animation/loading3.json'")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(p58, p59);

    auto* p60 = ui::Create<ui::Progress>(pWindow, {{DUI_T("class"), DUI_T("progress_horizontal_blue")}, {DUI_T("name"), DUI_T("progress")}, {DUI_T("value"), DUI_T("0")}, {DUI_T("margin"), DUI_T("10")}, {DUI_T("tooltip_text"), DUI_T("ui::Progress")}});
    ui::Attach(p58, p60);

    ui::Attach(p57, p58);

    auto* p61 = ui::Create<ui::HBox>(pWindow, {{DUI_T("margin"), DUI_T("0,0,0,10")}, {DUI_T("height"), DUI_T("32")}});
    auto* p62 = ui::Create<ui::Control>(pWindow, {{DUI_T("width"), DUI_T("24")}, {DUI_T("height"), DUI_T("24")}, {DUI_T("bkimage"), DUI_T("file='set_speaker.svg' width='20' height='20' valign='center' halign='center'")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(p61, p62);

    auto* p63 = ui::Create<ui::Slider>(pWindow, {{DUI_T("class"), DUI_T("slider_horizontal_green")}, {DUI_T("value"), DUI_T("70")}, {DUI_T("margin"), DUI_T("10")}, {DUI_T("tooltip_text"), DUI_T("ui::Slider")}});
    ui::Attach(p61, p63);

    ui::Attach(p57, p61);

    ui::Attach(p56, p57);

    auto* p64 = ui::Create<ui::VBox>(pWindow, {{DUI_T("width"), DUI_T("90")}, {DUI_T("margin"), DUI_T("10,0")}});
    auto* p65 = ui::Create<ui::CircleProgress>(pWindow, {{DUI_T("name"), DUI_T("circleprogress")}, {DUI_T("circular"), DUI_T("true")}, {DUI_T("height"), DUI_T("80")}, {DUI_T("width"), DUI_T("80")}, {DUI_T("circle_width"), DUI_T("12")}, {DUI_T("bgcolor"), DUI_T("gray")}, {DUI_T("fgcolor"), DUI_T("green")}, {DUI_T("gradient_color"), DUI_T("red")}, {DUI_T("clockwise"), DUI_T("true")}, {DUI_T("min"), DUI_T("0")}, {DUI_T("max"), DUI_T("100")}, {DUI_T("value"), DUI_T("75")}, {DUI_T("margin"), DUI_T("10")}, {DUI_T("text_padding"), DUI_T("10,32,10,10")}, {DUI_T("normal_text_color"), DUI_T("darkcolor")}, {DUI_T("indicator"), DUI_T("file='public/progress/indicator.svg' width='12' height='12'")}, {DUI_T("tooltip_text"), DUI_T("ui::CircleProgress")}});
    ui::Attach(p64, p65);

    ui::Attach(p56, p64);

    auto* p66 = ui::Create<ui::VBox>(pWindow, {{DUI_T("margin"), DUI_T("4,8,0,0")}});
    auto* p67 = ui::Create<ui::HBox>(pWindow, {{DUI_T("child_margin"), DUI_T("4")}, {DUI_T("width"), DUI_T("300")}});
    auto* p68 = ui::Create<ui::Label>(pWindow, {{DUI_T("text"), DUI_T("Date")}, {DUI_T("height"), DUI_T("24")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("hot_text_color"), DUI_T("red")}, {DUI_T("pushed_text_color"), DUI_T("blue")}});
    ui::Attach(p67, p68);

    auto* p69 = ui::Create<ui::DateTime>(pWindow, {{DUI_T("width"), DUI_T("100%")}, {DUI_T("height"), DUI_T("24")}, {DUI_T("text"), DUI_T("2022-02-01")}});
    ui::Attach(p67, p69);

    auto* p70 = ui::Create<ui::DateTime>(pWindow, {{DUI_T("width"), DUI_T("120")}, {DUI_T("height"), DUI_T("24")}, {DUI_T("edit_format"), DUI_T("date_up_down")}});
    ui::Attach(p67, p70);

    ui::Attach(p66, p67);

    auto* p71 = ui::Create<ui::HBox>(pWindow, {{DUI_T("child_margin"), DUI_T("4")}, {DUI_T("width"), DUI_T("320")}});
    auto* p72 = ui::Create<ui::Label>(pWindow, {{DUI_T("text"), DUI_T("Time")}, {DUI_T("height"), DUI_T("24")}, {DUI_T("text_align"), DUI_T("vcenter")}});
    ui::Attach(p71, p72);

    auto* p73 = ui::Create<ui::DateTime>(pWindow, {{DUI_T("width"), DUI_T("100%")}, {DUI_T("height"), DUI_T("24")}, {DUI_T("edit_format"), DUI_T("date_time_up_down")}});
    ui::Attach(p71, p73);

    auto* p74 = ui::Create<ui::DateTime>(pWindow, {{DUI_T("width"), DUI_T("72")}, {DUI_T("height"), DUI_T("24")}, {DUI_T("edit_format"), DUI_T("minute_up_down")}});
    ui::Attach(p71, p74);

    ui::Attach(p66, p71);

    ui::Attach(p56, p66);

    ui::Attach(p55, p56);

    auto* p75 = ui::Create<ui::HBox>(pWindow, {});
    auto* p76 = ui::Create<ui::VBox>(pWindow, {{DUI_T("margin"), DUI_T("10,0,0,0")}});
    auto* p77 = ui::Create<ui::HBox>(pWindow, {{DUI_T("child_margin"), DUI_T("10")}});
    auto* p78 = ui::Create<ui::Label>(pWindow, {{DUI_T("text"), DUI_T("IP Address")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("text_align"), DUI_T("vcenter")}});
    ui::Attach(p77, p78);

    auto* p79 = ui::Create<ui::IPAddress>(pWindow, {{DUI_T("class"), DUI_T("ip_address")}, {DUI_T("ip"), DUI_T("192.168.0.1")}});
    ui::Attach(p77, p79);

    ui::Attach(p76, p77);

    auto* p80 = ui::Create<ui::HBox>(pWindow, {{DUI_T("child_margin"), DUI_T("10")}});
    auto* p81 = ui::Create<ui::Label>(pWindow, {{DUI_T("text"), DUI_T("RichEdit Spin")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("text_align"), DUI_T("vcenter")}});
    ui::Attach(p80, p81);

    auto* p82 = ui::Create<ui::RichEdit>(pWindow, {{DUI_T("class"), DUI_T("simple simple_border rich_edit_spin")}, {DUI_T("min_number"), DUI_T("-64")}, {DUI_T("max_number"), DUI_T("64")}, {DUI_T("limit_text"), DUI_T("3")}, {DUI_T("text"), DUI_T("0")}});
    ui::Attach(p80, p82);

    ui::Attach(p76, p80);

    ui::Attach(p75, p76);

    auto* p83 = ui::Create<ui::VBox>(pWindow, {{DUI_T("margin"), DUI_T("10,0,0,0")}});
    auto* p84 = ui::Create<ui::HBox>(pWindow, {{DUI_T("child_margin"), DUI_T("10")}});
    auto* p85 = ui::Create<ui::HotKey>(pWindow, {{DUI_T("class"), DUI_T("hot_key")}, {DUI_T("name"), DUI_T("set_hot_key")}, {DUI_T("default_text"), DUI_T("None")}});
    ui::Attach(p84, p85);

    auto* p86 = ui::Create<ui::Button>(pWindow, {{DUI_T("class"), DUI_T("btn_global_color_gray")}, {DUI_T("name"), DUI_T("btn_set_hot_key")}, {DUI_T("text"), DUI_T("Set Window HotKey")}, {DUI_T("width"), DUI_T("100%")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("border_round"), DUI_T("3,3")}, {DUI_T("text_align"), DUI_T("vcenter")}});
    ui::Attach(p84, p86);

    ui::Attach(p83, p84);

    auto* p87 = ui::Create<ui::HBox>(pWindow, {{DUI_T("child_margin"), DUI_T("10")}});
    auto* p88 = ui::Create<ui::HotKey>(pWindow, {{DUI_T("class"), DUI_T("hot_key")}, {DUI_T("name"), DUI_T("set_system_hot_key")}, {DUI_T("default_text"), DUI_T("None")}});
    ui::Attach(p87, p88);

    auto* p89 = ui::Create<ui::Button>(pWindow, {{DUI_T("class"), DUI_T("btn_global_color_gray")}, {DUI_T("name"), DUI_T("btn_set_system_hot_key")}, {DUI_T("text"), DUI_T("Set System HotKey")}, {DUI_T("width"), DUI_T("100%")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("border_round"), DUI_T("3,3")}, {DUI_T("text_align"), DUI_T("vcenter")}});
    ui::Attach(p87, p89);

    ui::Attach(p83, p87);

    ui::Attach(p75, p83);

    ui::Attach(p55, p75);

    auto* p90 = ui::Create<ui::HBox>(pWindow, {{DUI_T("child_margin"), DUI_T("10")}});
    auto* p91 = ui::Create<ui::HBox>(pWindow, {{DUI_T("child_margin"), DUI_T("10")}});
    auto* p92 = ui::Create<ui::RichText>(pWindow, {{DUI_T("class"), DUI_T("rich_text")}, {DUI_T("name"), DUI_T("rich_text_demo")}, {DUI_T("width"), DUI_T("300")}, {DUI_T("height"), DUI_T("60")}, {DUI_T("row_spacing_mul"), DUI_T("1.2")}});
    ui::WindowBuilder::ParseRichTextXmlText(DUI_T("<RichText>\n                                RichText:　<a href=\"www.baidu.com\">Click to visit Baidu</a><br/>\n                                RichText:　<a href=\"www.sohu.com\">Click to visit Sohu</a></RichText>"), p92);
    ui::Attach(p91, p92);

    auto* p93 = ui::Create<ui::VBox>(pWindow, {{DUI_T("width"), DUI_T("80")}});
    auto* p94 = ui::Create<ui::HyperLink>(pWindow, {{DUI_T("class"), DUI_T("hyper_link")}, {DUI_T("name"), DUI_T("hyper_link1")}, {DUI_T("text"), DUI_T("Baidu Homepage")}, {DUI_T("url"), DUI_T("www.baidu.com")}, {DUI_T("tooltip_text"), DUI_T("This is a HyperLink control")}});
    ui::Attach(p93, p94);

    auto* p95 = ui::Create<ui::HyperLink>(pWindow, {{DUI_T("class"), DUI_T("hyper_link")}, {DUI_T("name"), DUI_T("hyper_link2")}, {DUI_T("text"), DUI_T("Text Button")}, {DUI_T("tooltip_text"), DUI_T("This is a HyperLink control")}});
    ui::Attach(p93, p95);

    ui::Attach(p91, p93);

    ui::Attach(p90, p91);

    auto* p96 = ui::Create<ui::HBox>(pWindow, {{DUI_T("child_margin"), DUI_T("10")}});
    auto* p97 = ui::Create<ui::Label>(pWindow, {{DUI_T("text"), DUI_T("Line:")}, {DUI_T("valign"), DUI_T("center")}});
    ui::Attach(p96, p97);

    auto* p98 = ui::Create<ui::VBox>(pWindow, {});
    auto* p99 = ui::Create<ui::Line>(pWindow, {{DUI_T("vertical"), DUI_T("false")}, {DUI_T("line_color"), DUI_T("blue")}, {DUI_T("dash_style"), DUI_T("solid")}, {DUI_T("line_width"), DUI_T("2")}});
    ui::Attach(p98, p99);

    auto* p100 = ui::Create<ui::Line>(pWindow, {{DUI_T("vertical"), DUI_T("false")}, {DUI_T("line_color"), DUI_T("red")}, {DUI_T("dash_style"), DUI_T("dash")}, {DUI_T("line_width"), DUI_T("2")}});
    ui::Attach(p98, p100);

    auto* p101 = ui::Create<ui::Line>(pWindow, {{DUI_T("vertical"), DUI_T("false")}, {DUI_T("line_color"), DUI_T("green")}, {DUI_T("dash_style"), DUI_T("dot")}, {DUI_T("line_width"), DUI_T("2")}});
    ui::Attach(p98, p101);

    auto* p102 = ui::Create<ui::Line>(pWindow, {{DUI_T("vertical"), DUI_T("false")}, {DUI_T("line_color"), DUI_T("black")}, {DUI_T("dash_style"), DUI_T("dash_dot")}, {DUI_T("line_width"), DUI_T("2")}});
    ui::Attach(p98, p102);

    auto* p103 = ui::Create<ui::Line>(pWindow, {{DUI_T("vertical"), DUI_T("false")}, {DUI_T("line_color"), DUI_T("orange")}, {DUI_T("dash_style"), DUI_T("dash_dot_dot")}, {DUI_T("line_width"), DUI_T("2")}});
    ui::Attach(p98, p103);

    ui::Attach(p96, p98);

    auto* p104 = ui::Create<ui::VBox>(pWindow, {});
    auto* p105 = ui::Create<ui::HBox>(pWindow, {{DUI_T("margin"), DUI_T("0,4,0,4")}});
    auto* p106 = ui::Create<ui::Line>(pWindow, {{DUI_T("vertical"), DUI_T("true")}, {DUI_T("line_color"), DUI_T("blue")}, {DUI_T("dash_style"), DUI_T("solid")}, {DUI_T("line_width"), DUI_T("1")}});
    ui::Attach(p105, p106);

    auto* p107 = ui::Create<ui::Line>(pWindow, {{DUI_T("vertical"), DUI_T("true")}, {DUI_T("line_color"), DUI_T("red")}, {DUI_T("dash_style"), DUI_T("dash")}, {DUI_T("line_width"), DUI_T("1")}});
    ui::Attach(p105, p107);

    auto* p108 = ui::Create<ui::Line>(pWindow, {{DUI_T("vertical"), DUI_T("true")}, {DUI_T("line_color"), DUI_T("green")}, {DUI_T("dash_style"), DUI_T("dot")}, {DUI_T("line_width"), DUI_T("1")}});
    ui::Attach(p105, p108);

    auto* p109 = ui::Create<ui::Line>(pWindow, {{DUI_T("vertical"), DUI_T("true")}, {DUI_T("line_color"), DUI_T("black")}, {DUI_T("dash_style"), DUI_T("dash_dot")}, {DUI_T("line_width"), DUI_T("1")}});
    ui::Attach(p105, p109);

    auto* p110 = ui::Create<ui::Line>(pWindow, {{DUI_T("vertical"), DUI_T("true")}, {DUI_T("line_color"), DUI_T("orange")}, {DUI_T("dash_style"), DUI_T("dash_dot_dot")}, {DUI_T("line_width"), DUI_T("1")}});
    ui::Attach(p105, p110);

    ui::Attach(p104, p105);

    ui::Attach(p96, p104);

    ui::Attach(p90, p96);

    ui::Attach(p55, p90);

    ui::Attach(p48, p55);

    ui::Attach(p8, p48);

    auto* p111 = ui::Create<ui::Control>(pWindow, {{DUI_T("class"), DUI_T("splitline_hor_level1")}, {DUI_T("margin"), DUI_T("0,8,0,4")}});
    ui::Attach(p8, p111);

    auto* p112 = ui::Create<ui::HBox>(pWindow, {});
    auto* p113 = ui::Create<ui::RichEdit>(pWindow, {{DUI_T("class"), DUI_T("prompt")}, {DUI_T("name"), DUI_T("edit")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("hide_selection"), DUI_T("false")}, {DUI_T("multiline"), DUI_T("true")}, {DUI_T("auto_vscroll"), DUI_T("true")}, {DUI_T("vscrollbar"), DUI_T("true")}, {DUI_T("hscrollbar"), DUI_T("true")}, {DUI_T("want_return_msg"), DUI_T("true")}, {DUI_T("rich"), DUI_T("false")}, {DUI_T("enable_drag_drop"), DUI_T("true")}, {DUI_T("enable_drop_file"), DUI_T("true")}, {DUI_T("drop_file_types"), DUI_T(".txt;.md")}});
    ui::Attach(p112, p113);

    auto* p114 = ui::Create<ui::RichEdit>(pWindow, {{DUI_T("class"), DUI_T("prompt")}, {DUI_T("name"), DUI_T("edit2")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("hide_selection"), DUI_T("false")}, {DUI_T("multiline"), DUI_T("true")}, {DUI_T("auto_vscroll"), DUI_T("true")}, {DUI_T("vscrollbar"), DUI_T("true")}, {DUI_T("hscrollbar"), DUI_T("true")}, {DUI_T("scrollbar_float"), DUI_T("true")}, {DUI_T("vscrollbar_class"), DUI_T("vscrollbar2")}, {DUI_T("hscrollbar_class"), DUI_T("hscrollbar2")}, {DUI_T("vscrollbar_style"), DUI_T("auto_hide_scroll='false'")}, {DUI_T("hscrollbar_style"), DUI_T("auto_hide_scroll='false'")}, {DUI_T("want_return_msg"), DUI_T("true")}, {DUI_T("rich"), DUI_T("false")}, {DUI_T("margin"), DUI_T("4,0,0,0")}, {DUI_T("enable_drag_drop"), DUI_T("true")}, {DUI_T("enable_drop_file"), DUI_T("false")}});
    ui::Attach(p112, p114);

    ui::Attach(p8, p112);

    ui::Attach(p0, p8);


    ui::Attach(pWindow, p0);
}
