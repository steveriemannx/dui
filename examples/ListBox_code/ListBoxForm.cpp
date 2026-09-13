#include "ListBoxForm.h"
#include "Item.h"
#include "dui/Utils/UiBuilder.h"

void ListBoxForm::SetupWindow()
{
    SetWindowSize(540, 720);
    CenterWindow();

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
}

void ListBoxForm::BuildUI()
{
    // Register the local style classes in item.xml (the Item template is now built in code)
    AddClass("list_box_item_test",
             " hot_color=\"bk_listitem_hovered\" pushed_color=\"bk_listitem_selected\" selected_normal_color=\"bk_listitem_selected\" fade_hot=\"false\"");
    AddClass("list_box_item_checkbox_1",
             " height=\"20\" text_padding=\"20,0,0,0\" font=\"system_14\" normal_image=\"file='public/CheckBox/checkbox-outline-unchecked.svg' margin='2,0,0,0' valign='center'\" disabled_image=\"file='public/CheckBox/checkbox-outline-unchecked.svg' margin='2,0,0,0' valign='center' fade='80'\" selected_normal_image=\"file='public/CheckBox/checkbox-outline-checked.svg' margin='2,0,0,0' valign='center'\" selected_disabled_image=\"file='public/CheckBox/checkbox-outline-checked.svg' margin='2,0,0,0' valign='center' fade='80'\"");

    // Corresponding to the list_box.xml layout
    auto* pRoot = ui::Create<ui::VBox>(this, {});
    pRoot->SetBkColor("bk_wnd_darkcolor");

    // Title bar area
    auto* pCaption = ui::Create<ui::HBox>(this, {{"name", "window_caption_bar"}, {"width", "stretch"}, {"height", "36"}});
    pCaption->SetBkColor("bk_wnd_lightcolor");
    pRoot->AddItem(pCaption);

    auto* pTitle = ui::Create<ui::Label>(this, {{"font", "system_14"}, {"valign", "center"}, {"margin", "8"}, {"width", "stretch"}, {"mouse_enabled", "false"}});
    pTitle->SetText("List (VTileListBox | HTileListBox | VListBox | HListBox)");
    pCaption->AddItem(pTitle);

    auto* pCaptionBtns = ui::Create<ui::HBox>(this, {{"width", "auto"}});
    pCaption->AddItem(pCaptionBtns);

    auto* pMinBtn = ui::Create<ui::Button>(this, {{"height", "32"}, {"width", "40"}, {"margin", "0,2,0,2"}});
    pMinBtn->SetClass("btn_wnd_min_11");
    pMinBtn->SetName("minbtn");
    pMinBtn->SetToolTipText("Minimize");
    pCaptionBtns->AddItem(pMinBtn);

    auto* pMaxBox = ui::Create<ui::Box>(this, {{"height", "stretch"}, {"width", "40"}, {"margin", "0,2,0,2"}});
    pCaptionBtns->AddItem(pMaxBox);

    auto* pMaxBtn = ui::Create<ui::Button>(this, {{"height", "32"}, {"width", "stretch"}});
    pMaxBtn->SetClass("btn_wnd_max_11");
    pMaxBtn->SetName("maxbtn");
    pMaxBtn->SetToolTipText("Maximize");
    pMaxBox->AddItem(pMaxBtn);

    auto* pRestoreBtn = ui::Create<ui::Button>(this, {{"height", "32"}, {"width", "stretch"}});
    pRestoreBtn->SetClass("btn_wnd_restore_11");
    pRestoreBtn->SetName("restorebtn");
    pRestoreBtn->SetToolTipText("Restore");
    pRestoreBtn->SetVisible(false);
    pMaxBox->AddItem(pRestoreBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{"height", "stretch"}, {"width", "40"}, {"margin", "0,0,0,2"}});
    pCloseBtn->SetClass("btn_wnd_close_11");
    pCloseBtn->SetName("closebtn");
    pCloseBtn->SetToolTipText("Close");
    pCaptionBtns->AddItem(pCloseBtn);

    // List area
    auto* pContent = ui::Create<ui::Box>(this, {});
    pRoot->AddItem(pContent);

    auto* pListBox = ui::Create<ui::VListBox>(this, {{"vscrollbar", "true"}, {"hscrollbar", "true"}, {"multi_select", "false"}, {"scroll_select", "false"}});
    pListBox->SetName("list");
    pListBox->SetBkColor("bk_wnd_lightcolor");
    pContent->AddItem(pListBox);

    AttachBox(pRoot);
}

void ListBoxForm::OnInitWindow()
{
    SetupWindow();
    BuildUI();

    m_pListBox = ui::Find<ui::ListBox>(this, "list");
    ASSERT(m_pListBox != nullptr);
    if (m_pListBox == nullptr) {
        return;
    }
    bool bHListBox = dynamic_cast<ui::HListBox*>(m_pListBox) != nullptr;
    bool bVListBox = dynamic_cast<ui::VListBox*>(m_pListBox) != nullptr;
    bool bHTileListBox = dynamic_cast<ui::HTileListBox*>(m_pListBox) != nullptr;
    bool bVTileListBox = dynamic_cast<ui::VTileListBox*>(m_pListBox) != nullptr;

    if (bVTileListBox) {
        //VTileListBox: set to fixed 2 columns, auto-calculate the Item width
        //m_pListBox->SetAttribute("item_size", "200，80");
        m_pListBox->SetAttribute("columns", "2");
        m_pListBox->SetAttribute("auto_calc_item_size", "true");
    }
    else if (bHTileListBox) {
        //HTileListBox: set to fixed 2 rows, auto-calculate the Item height
        //m_pListBox->SetAttribute("item_size", "200，80");
        m_pListBox->SetAttribute("rows", "2");
        m_pListBox->SetAttribute("auto_calc_item_size", "true");
    }

    for (auto i = 0; i < 300; i++) {
        //The Item template is built in code (item.xml is no longer loaded)
        Item* item = new Item(this);

        if (bVListBox) {
            //VListBox: set to the stretch type
            item->SetFixedWidth(ui::UiFixedInt::MakeStretch(), true, true);
        }
        else if(bHListBox) {
            //HListBox: set to a fixed width
            item->SetFixedWidth(ui::UiFixedInt(200), true, true);
        }
        else if (bVTileListBox) {
            //VTileListBox: auto-calculated height
            item->SetFixedHeight(ui::UiFixedInt::MakeAuto(), true, true);
        }
        else if (bHTileListBox) {
            //HTileListBox: fixed width, auto-calculated height
            item->SetFixedWidth(ui::UiFixedInt(200), true, true);
            item->SetFixedHeight(ui::UiFixedInt::MakeAuto(), true, true);
        }

        std::string img = "icon.png";
        std::string title = ui::StringUtil::Printf("Task [%02d]", i);

        item->InitSubControls(img, title);
        m_pListBox->AddItem(item);
    }

    m_pListBox->SetFocus();

    BindEvents();
    BaseClass::OnInitWindow();
}

void ListBoxForm::BindEvents()
{
    TestListBoxEvents(m_pListBox);
}

void ListBoxForm::TestListBoxEvents(ui::ListBox* pListBox)
{
    if (pListBox == nullptr) {
        return;
    }
    //Attach events to test the event interfaces
    auto OnVirtualListBoxEvents = [this, pListBox](const ui::EventArgs& args) {
        ASSERT(pListBox == args.GetSender());
        std::string sInfo = GetEventDisplayInfo(args, pListBox);
        OutputDebugLog(sInfo);
        };

    //Attach events
    pListBox->AttachSelect([this, OnVirtualListBoxEvents](const ui::EventArgs& args) {
        OnVirtualListBoxEvents(args);
        return true;
        });
    pListBox->AttachUnSelect([this, OnVirtualListBoxEvents](const ui::EventArgs& args) {
        OnVirtualListBoxEvents(args);
        return true;
        });
    pListBox->AttachSelChanged([this, OnVirtualListBoxEvents](const ui::EventArgs& args) {
        OnVirtualListBoxEvents(args);
        return true;
        });
    pListBox->AttachDoubleClick([this, OnVirtualListBoxEvents](const ui::EventArgs& args) {
        OnVirtualListBoxEvents(args);
        return true;
        });
    pListBox->AttachClick([this, OnVirtualListBoxEvents](const ui::EventArgs& args) {
        OnVirtualListBoxEvents(args);
        return true;
        });
    pListBox->AttachRClick([this, OnVirtualListBoxEvents](const ui::EventArgs& args) {
        OnVirtualListBoxEvents(args);
        return true;
        });
    pListBox->AttachItemMouseEnter([this, OnVirtualListBoxEvents](const ui::EventArgs& args) {
        OnVirtualListBoxEvents(args);
        return true;
        });
    pListBox->AttachItemMouseLeave([this, OnVirtualListBoxEvents](const ui::EventArgs& args) {
        OnVirtualListBoxEvents(args);
        return true;
        });
    pListBox->AttachReturn([this, OnVirtualListBoxEvents](const ui::EventArgs& args) {
        OnVirtualListBoxEvents(args);
        return true;
        });
    pListBox->AttachKeyDown([this, OnVirtualListBoxEvents](const ui::EventArgs& args) {
        OnVirtualListBoxEvents(args);
        return true;
        });
    pListBox->AttachKeyUp([this, OnVirtualListBoxEvents](const ui::EventArgs& args) {
        OnVirtualListBoxEvents(args);
        return true;
        });
}

std::string ListBoxForm::GetEventDisplayInfo(const ui::EventArgs& args, ui::ListBox* pListBox)
{
    std::string sInfo = ui::EventUtils::EventTypeToString(args.eventType);
    while (sInfo.size() < 24) {
        sInfo += " ";
    }
    if (args.eventType == ui::kEventSelect) {
        size_t nNewItemIndex = (size_t)args.wParam;
        size_t nOldItemIndex = (size_t)args.lParam;
        if (nOldItemIndex != ui::Box::InvalidIndex) {
            sInfo += ui::StringUtil::Printf("NewItemIndex=%zu, OldItemIndex=%zu",
                                            nNewItemIndex, nOldItemIndex);
        }
        else {
            sInfo += ui::StringUtil::Printf("NewItemIndex=%zu", nNewItemIndex);
        }
    }
    else if (args.eventType == ui::kEventUnSelect) {
        size_t nItemIndex = (size_t)args.wParam;
        sInfo += ui::StringUtil::Printf("ItemIndex=%zu", nItemIndex);
    }
    else if (args.eventType == ui::kEventSelChanged) {
        //No parameters
    }
    else if ((args.eventType == ui::kEventItemMouseEnter) ||
             (args.eventType == ui::kEventItemMouseLeave) ||
             (args.eventType == ui::kEventMouseDoubleClick) ||
             (args.eventType == ui::kEventClick) ||
             (args.eventType == ui::kEventRClick) ||
             (args.eventType == ui::kEventReturn)) {
        size_t nItemIndex = (size_t)args.wParam;
        if (nItemIndex == ui::Box::InvalidIndex) {
            sInfo += "no params";
        }
        else {
            sInfo += ui::StringUtil::Printf("ItemIndex=%zu", nItemIndex);
        }
    }
    else if ((args.eventType == ui::kEventKeyDown) || (args.eventType == ui::kEventKeyUp)) {
        //Keyboard message
        std::string keyName = ui::Keyboard::GetKeyName(args.vkCode, false);
        std::string modifierKey;
        if (args.vkCode != ui::VirtualKeyCode::kVK_CONTROL) {
            if (ui::Keyboard::IsKeyDown(ui::VirtualKeyCode::kVK_CONTROL)) {
                modifierKey += "Ctrl+";
            }
        }
        if (args.vkCode != ui::VirtualKeyCode::kVK_SHIFT) {
            if (ui::Keyboard::IsKeyDown(ui::VirtualKeyCode::kVK_SHIFT)) {
                modifierKey += "Shift+";
            }
        }
        if (args.vkCode != ui::VirtualKeyCode::kVK_MENU) {
            if (ui::Keyboard::IsKeyDown(ui::VirtualKeyCode::kVK_MENU)) {
                modifierKey += "Alt+";
            }
        }
        sInfo += "<";
        sInfo += modifierKey;
        sInfo += keyName;
        sInfo += ">";
        sInfo += " ";

        size_t nItemIndex = (size_t)args.wParam;
        if (nItemIndex == ui::Box::InvalidIndex) {
            sInfo += "no params";
        }
        else {
            sInfo += ui::StringUtil::Printf("ItemIndex=%zu", nItemIndex);
        }
    }
    else {
        ASSERT(0);
    }
    return sInfo;
}

void ListBoxForm::OutputDebugLog(const std::string& logMsg)
{
#if defined DUI_BUILD_FOR_WIN && defined _DEBUG
    //::OutputDebugString(logMsg.c_str());
#endif
}
