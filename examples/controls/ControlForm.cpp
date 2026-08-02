#include "ControlForm.h"
#include "AboutForm.h"
#include "TestForm.h"
#include "AnimationForm.h"

#include <fstream>

//ID of the system global hotkey
#define SYSTEM_HOTKEY_ID 111

ControlForm::ControlForm()
{
}


ControlForm::~ControlForm()
{
}

DString ControlForm::GetSkinFolder()
{
    return _T("controls");
}

DString ControlForm::GetSkinFile()
{
    return _T("controls.xml");
}

void ControlForm::OnInitWindow()
{
#ifdef DUILIB_BUILD_FOR_SDL
    //Display basic SDL information
    ui::Label* pTitle = dynamic_cast<ui::Label*>(FindControl(_T("window_title")));
    if (pTitle != nullptr) {
        DString title = pTitle->GetText();
        DString driverName = GetVideoDriverName();
        DString renderName = GetWindowRenderName();
        DString newTitle = ui::StringUtil::Printf(_T("%s[SDL: VideoDriver:\"%s\", RenderName:\"%s\"]"), title.c_str(), driverName.c_str(), renderName.c_str());
        pTitle->SetText(newTitle);
    }
#endif

    /* Initialize ListBox data */
    ui::ListBox* list = dynamic_cast<ui::ListBox*>(FindControl(_T("list")));
    if (list != nullptr) {
        for (auto i = 0; i < 30; i++)
        {
            ui::ListBoxItem* element = new ui::ListBoxItem(this);
            element->SetText(ui::StringUtil::Printf(_T("ui::VListBox::ListBoxItem %d"), i));
            element->SetClass(_T("listitem"));
            element->SetFixedHeight(ui::UiFixedInt(20), true, true);
            list->AddItem(element);
        }
    }

    ui::TreeView* pTree = dynamic_cast<ui::TreeView*>(FindControl(_T("tree")));
    if (pTree != nullptr) {
        ui::TreeNode* pRootNode = pTree->GetRootNode();
        ASSERT(pRootNode != nullptr);
        if (pRootNode != nullptr) {
            ui::TreeNode* pTestNode = pRootNode->FindChildNodeByText(_T("ui::TreeView Parent Node 2"), true);
            ASSERT(pTestNode != nullptr);
            if (pTestNode != nullptr) {
                ui::TreeNode* pNode0 = new ui::TreeNode(this);
                pNode0->SetClass(_T("tree_node"));
                pNode0->SetText(_T("Dynamic Node 0(top)"));
                pTestNode->AddChildNodeAt(pNode0, 0);

                ui::TreeNode* pNode2 = new ui::TreeNode(this);
                pNode2->SetClass(_T("tree_node"));
                pNode2->SetText(_T("Dynamic Node 1(end)"));
                pTestNode->AddChildNode(pNode2);

                ui::TreeNode* pNode1 = new ui::TreeNode(this);
                pNode1->SetClass(_T("tree_node"));
                pNode1->SetText(_T("Dynamic Node 2(at index 2)"));
                pTestNode->AddChildNodeAt(pNode1, 2);
            }
        }
    }

    //Initialize the Combo data
    ui::Combo* combo = dynamic_cast<ui::Combo*>(FindControl(_T("combo")));
    if (combo != nullptr) {
        ui::TreeView* pTreeView = combo->GetTreeView();
        ui::TreeNode* pTreeNode = pTreeView->GetRootNode();
        for (auto i = 0; i < 10; i++) {
            ui::TreeNode* node = new ui::TreeNode(this);
            node->SetClass(_T("tree_node"));
            node->SetText(ui::StringUtil::Printf(_T("ui::Combo::TreeNode %d"), i));
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
//    combo->SetItemText(2, _T("2"));
//    ASSERT(combo->GetItemText(2) == _T("2"));
//
//    size_t nIndex = combo->AddTextItem(_T("Last"));
//    ASSERT(combo->GetItemText(nIndex) == _T("Last"));
//
//    nIndex = combo->InsertTextItem(nIndex, _T("Last2"));
//    ASSERT(combo->GetItemText(nIndex) == _T("Last2"));
//
//    ASSERT(combo->DeleteItem(nIndex));
//
//    ASSERT(combo->GetItemText(combo->GetCurSel()) == combo->GetText());
//    combo->SetText(_T("Test"));
//    ASSERT(combo->GetText() == _T("Test"));
//#endif

    ui::FilterCombo* filterCombo = dynamic_cast<ui::FilterCombo*>(FindControl(_T("filter_combo")));
    if (filterCombo != nullptr) {
        for (auto i = 0; i < 100; i++) {
            filterCombo->AddTextItem(ui::StringUtil::Printf(_T("Item %d FilterCombo"), i));
        }
    }

    ui::CheckCombo* check_combo = dynamic_cast<ui::CheckCombo*>(FindControl(_T("check_combo")));
    if (check_combo != nullptr) {
        check_combo->AddTextItem(_T("Monday"));
        check_combo->AddTextItem(_T("Tuesday"));
        check_combo->AddTextItem(_T("Wednesday"));
        check_combo->AddTextItem(_T("Thursday"));
        check_combo->AddTextItem(_T("Friday"));
        check_combo->AddTextItem(_T("Saturday"));
        check_combo->AddTextItem(_T("Sunday"));
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
    ui::Button* settings = dynamic_cast<ui::Button*>(FindControl(_T("settings")));
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
    AttachRichEditEvents(dynamic_cast<ui::RichEdit*>(FindControl(_T("edit"))));
    AttachRichEditEvents(dynamic_cast<ui::RichEdit*>(FindControl(_T("edit2"))));

    //Show the color picker of a modal dialog
    ui::Button* pShowColorPicker = dynamic_cast<ui::Button*>(FindControl(_T("show_color_picker")));
    if (pShowColorPicker != nullptr) {
        pShowColorPicker->AttachClick([this](const ui::EventArgs& args) {
            ShowColorPicker(true);
            return true;
            });
    }

    //Show a modal dialog
    ui::Button* pShowModal = dynamic_cast<ui::Button*>(FindControl(_T("domodal2")));
    if (pShowModal != nullptr) {
        pShowModal->AttachClick([this](const ui::EventArgs& args) {
            ShowDoModalDlg();
            return true;
            });
    }

    //RichText displays hyperlinks
    ui::RichText* pRichText = dynamic_cast<ui::RichText*>(FindControl(_T("rich_text_demo")));
    if (pRichText != nullptr) {
        pRichText->AttachLinkClick([this](const ui::EventArgs& args) {
            const DString::value_type* url = (const DString::value_type*)args.wParam;
            if (url != nullptr) {
                ui::SystemUtil::ShowMessageBox(this, url, _T("RichText Click HyperLink"));
            }
            return true;
            });
    }

    //HyperLink control
    ui::HyperLink* pHyperLink = dynamic_cast<ui::HyperLink*>(FindControl(_T("hyper_link1")));
    if (pHyperLink != nullptr) {
        pHyperLink->AttachLinkClick([this](const ui::EventArgs& args) {
            const DString::value_type* url = (const DString::value_type*)args.wParam;
            if (url != nullptr) {
                ui::SystemUtil::ShowMessageBox(this, url, _T("HyperLink Click HyperLink"));
            }
            return true;
            });
    }

    pHyperLink = dynamic_cast<ui::HyperLink*>(FindControl(_T("hyper_link2")));
    if (pHyperLink != nullptr) {
        pHyperLink->AttachLinkClick([this](const ui::EventArgs& /*args*/) {
            ui::SystemUtil::ShowMessageBox(this, _T("Text Button Event Response"), _T("HyperLink Click"));
            return true;
            });
    }

    //Hotkey settings
    ui::HotKey* pHotKey = dynamic_cast<ui::HotKey*>(FindControl(_T("set_hot_key")));
    ui::Button* pHotKeyButton = dynamic_cast<ui::Button*>(FindControl(_T("btn_set_hot_key")));
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

    pHotKey = dynamic_cast<ui::HotKey*>(FindControl(_T("set_system_hot_key")));
    pHotKeyButton = dynamic_cast<ui::Button*>(FindControl(_T("btn_set_system_hot_key")));
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
    ui::Button* pTestBtn = dynamic_cast<ui::Button*>(FindControl(_T("test_btn")));
    if (pTestBtn != nullptr) {
        pTestBtn->AttachClick([this](const ui::EventArgs&) {
            ShowTestWindow();
            return true;
            });
    }

    //Response function of the animation test button
    ui::Button* pAnimationBtn = dynamic_cast<ui::Button*>(FindControl(_T("animation_btn")));
    if (pAnimationBtn != nullptr) {
        pAnimationBtn->AttachClick([this](const ui::EventArgs&) {
            ShowAnimationWindow();
            return true;
            });
    }

    //Tray icon functionality
    ui::CheckBox* pTrayIconCheckBox = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_tray_icon")));
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
    ui::Button* pFullscreenBtn = dynamic_cast<ui::Button*>(FindControl(_T("rich_edit_fullscreen_btn")));
    if (pFullscreenBtn != nullptr) {
        pFullscreenBtn->AttachClick([this](const ui::EventArgs&) {
            ui::Control* pControl = FindControl(_T("edit"));
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
        ui::RichEdit* pEdit = dynamic_cast<ui::RichEdit*>(FindControl(_T("edit")));
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
    simpleWnd.InitSkin(_T("controls"), _T("about.xml"));
    ui::WindowCreateParam createParam;
    createParam.m_dwStyle = ui::kWS_POPUP;
    createParam.m_dwExStyle = ui::kWS_EX_LAYERED;
    createParam.m_windowTitle = _T("AboutForm");
    createParam.m_bCenterWindow = true;
    simpleWnd.DoModal(this, createParam);
}

void ControlForm::ShowTestWindow()
{
    TestForm* testForm = new TestForm();
    ui::WindowCreateParam createParam;
    createParam.m_dwStyle = ui::kWS_POPUP;
    createParam.m_dwExStyle = ui::kWS_EX_LAYERED;
    createParam.m_windowTitle = _T("TestWindow");
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
    createParam.m_windowTitle = _T("AnimationWindow");
    createParam.m_bCenterWindow = true;
    testForm->CreateWnd(this, createParam);
    testForm->ShowModalFake();
}

void ControlForm::ShowPopupMenu(const ui::UiPoint& point, ui::Control* pRelatedControl)
{
    ui::Menu* menu = new ui::Menu(this, pRelatedControl);//A parent window is required, otherwise the program status bar becomes inactive when the menu pops up
    menu->SetSkinFolder(GetResourcePath().ToString());
    DString xml(_T("menu/settings_menu.xml"));
    menu->ShowMenu(xml, point);

    //Add a submenu item to the second-level menu
    ui::MenuItem* menu_fourth = static_cast<ui::MenuItem*>(menu->FindControl(_T("fourth")));
    if (menu_fourth != nullptr) {
        ui::MenuItem* menu_item = new ui::MenuItem(menu);
        menu_item->SetText(_T("Dynamically created"));
        menu_item->SetClass(_T("menu_element"));
        menu_item->SetFixedWidth(ui::UiFixedInt(180), true, true);
        menu_item->SetFontId(_T("system_14"));
        menu_item->SetTextPadding({ 20, 0, 20, 0 }, true);
        menu_fourth->AddSubMenuItemAt(menu_item, 1);//After adding, the resource is managed by the menu
    }
    

    //Add a submenu item to the first-level menu
    /*
    menu_item = new ui::MenuItem(menu);
    menu_item->SetWindow(menu);
    menu_item->SetText(_T("Dynamically created"));
    menu_item->SetClass(_T("menu_element"));
    menu_item->SetFixedWidth(180);
    menu_item->SetFontId(_T("system_14"));
    menu_item->SetTextPadding({ 20, 0, 20, 0 });
    menu->AddMenuItemAt(menu_item, 4);//After adding, the resource is managed by the menu
    */

    //Demonstration of the checkbox menu item functionality
    static bool s_is_checked_01_flag = false;
    bool& flag = s_is_checked_01_flag;
    ui::MenuItem* menu_check_01 = dynamic_cast<ui::MenuItem*>(menu->FindControl(_T("menu_check_01")));
    if (menu_check_01 != nullptr) {
        menu_check_01->AttachClick([&flag](const ui::EventArgs& args) {
            flag = true;
            return true;
            });
    }
    ui::CheckBox* menuCheckBox01 = dynamic_cast<ui::CheckBox*>(menu->FindControl(_T("menu_checkbox_01")));
    if (menuCheckBox01 != nullptr) {
        menuCheckBox01->Selected(s_is_checked_01_flag);
    }

    ui::MenuItem* menu_check_02 = dynamic_cast<ui::MenuItem*>(menu->FindControl(_T("menu_check_02")));
    if (menu_check_02 != nullptr) {
        menu_check_02->AttachClick([&flag](const ui::EventArgs& args) {
            flag = false;
            return true;
            });
    }

    ui::CheckBox* menuCheckBox02 = dynamic_cast<ui::CheckBox*>(menu->FindControl(_T("menu_checkbox_02")));
    if (menuCheckBox02 != nullptr) {
        menuCheckBox02->Selected(!s_is_checked_01_flag);
    }


    /* About menu */
    ui::MenuItem* menu_about = static_cast<ui::MenuItem*>(menu->FindControl(_T("about")));
    if (menu_about != nullptr) {
        menu_about->AttachClick([this](const ui::EventArgs& args) {
            AboutForm* about_form = new AboutForm();
            ui::WindowCreateParam createParam;
            createParam.m_dwStyle = ui::kWS_POPUP;
            createParam.m_dwExStyle = ui::kWS_EX_LAYERED;
            createParam.m_windowTitle = _T("AboutForm");
            createParam.m_bCenterWindow = true;
            about_form->CreateWnd(this, createParam);
            about_form->ShowModalFake();
            return true;
            });
    }
}

void ControlForm::LoadRichEditData()
{
    std::streamoff length = 0;
    
    ui::FilePath controls_xml = ui::GlobalManager::Instance().GetResourcePath();
    controls_xml += GetResourcePath();
    controls_xml += GetSkinFile();

    //XML files are loaded in UTF-8 encoding
    std::string xml;
    std::vector<uint8_t> xmlData;
    ui::FileUtil::ReadFileData(controls_xml, xmlData);
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
    ui::RichEdit* pRichEdit = static_cast<ui::RichEdit*>(FindControl(_T("edit2")));
    if (pRichEdit) {
        pRichEdit->SetText(xml);
        pRichEdit->HomeUp();
    }
    pRichEdit = static_cast<ui::RichEdit*>(FindControl(_T("edit")));
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
    auto progress = static_cast<ui::Progress*>(FindControl(_T("progress")));
    if (progress) {
        progress->SetValue(value);
    }

    auto circleprogress = static_cast<ui::Progress*>(FindControl(_T("circleprogress")));
    if (circleprogress)    {
        circleprogress->SetValue(value);
        circleprogress->SetText(ui::StringUtil::Printf(_T("%.0f%%"), value));
    }
}

LRESULT ControlForm::OnHotKeyMsg(int32_t hotkeyId, ui::VirtualKeyCode vkCode, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    LRESULT lResult = BaseClass::OnHotKeyMsg(hotkeyId, vkCode, modifierKey, nativeMsg, bHandled);
    bHandled = true;
    if (hotkeyId == SYSTEM_HOTKEY_ID) {
        SetWindowForeground();
        ui::SystemUtil::ShowMessageBox(this, _T("System HotKey Command Received"), _T("ControlForm::OnHotKeyMsg"));
    }
    return lResult;
}

void ControlForm::ShowTrayIcon(bool bShow)
{
    if (bShow) {
        // Create the tray icon
        m_pTrayIcon = ui::TrayIcon::Create(this, _T("public/caption/logo.ico"), _T("controls(nim_duilib)"));
        if (m_pTrayIcon != nullptr) {
            // Set the message callback
            m_pTrayIcon->SetMessageCallback([this](ui::TrayIconMessageType msgType, int32_t x, int32_t y)
                {
                    OnTrayIconMessage(msgType, x, y);
                });

            // Show the welcome balloon
            m_pTrayIcon->ShowBalloon(_T("Hint"), _T("App has started!"), 3000);
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
    menu->SetSkinFolder(GetResourcePath().ToString());
    DString xml(_T("menu/tray_menu.xml"));
    menu->ShowMenu(xml, ui::UiPoint(x, y));

    //Menu item click response
    ui::MenuItem* pMenuItem = static_cast<ui::MenuItem*>(menu->FindControl(_T("tray_menu_item_1")));
    if (pMenuItem != nullptr) {
        pMenuItem->AttachClick([this](const ui::EventArgs& /*args*/) {
            ui::SystemUtil::ShowMessageBox(this, _T("tray_menu_item_1 clicked!"), _T("TrayIconTest"));
            return true;
            });
    }
    pMenuItem = static_cast<ui::MenuItem*>(menu->FindControl(_T("tray_menu_item_2")));
    if (pMenuItem != nullptr) {
        pMenuItem->AttachClick([this](const ui::EventArgs& /*args*/) {
            ui::SystemUtil::ShowMessageBox(this, _T("tray_menu_item_2 clicked!"), _T("TrayIconTest"));
            return true;
            });
    }
    pMenuItem = static_cast<ui::MenuItem*>(menu->FindControl(_T("tray_menu_item_3")));
    if (pMenuItem != nullptr) {
        pMenuItem->AttachClick([this](const ui::EventArgs& /*args*/) {
            ui::SystemUtil::ShowMessageBox(this, _T("tray_menu_item_3 clicked!"), _T("TrayIconTest"));
            return true;
            });
    }
    pMenuItem = static_cast<ui::MenuItem*>(menu->FindControl(_T("tray_menu_item_4")));
    if (pMenuItem != nullptr) {
        pMenuItem->AttachClick([this](const ui::EventArgs& /*args*/) {
            ui::SystemUtil::ShowMessageBox(this, _T("tray_menu_item_4 clicked!"), _T("TrayIconTest"));
            return true;
            });
    }
    pMenuItem = static_cast<ui::MenuItem*>(menu->FindControl(_T("tray_menu_exit")));
    if (pMenuItem != nullptr) {
        pMenuItem->AttachClick([this](const ui::EventArgs& /*args*/) {
            this->CloseWnd();
            return true;
            });
    }
}
