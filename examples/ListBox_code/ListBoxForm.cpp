#include "ListBoxForm.h"
#include "Item.h"
#include "dui/Utils/UiBuilder.h"

void ListBoxForm::SetupWindow()
{
    SetWindowSize(540, 720);
    CenterWindow();

    SetShadowAttached(true);
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);

    SetSizeBox(ui::UiRect(4, 4, 4, 4), true);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), true);
}

void ListBoxForm::BuildUI()
{
    // Register the local style classes in item.xml (the Item template is now built in code)
    AddClass(DUI_T("list_box_item_test"),
             DUI_T(" hot_color=\"bk_listitem_hovered\" pushed_color=\"bk_listitem_selected\" selected_normal_color=\"bk_listitem_selected\" fade_hot=\"false\""));
    AddClass(DUI_T("list_box_item_checkbox_1"),
             DUI_T(" height=\"20\" text_padding=\"20,0,0,0\" font=\"system_14\" normal_image=\"file='public/CheckBox/checkbox-outline-unchecked.svg' margin='2,0,0,0' valign='center'\" disabled_image=\"file='public/CheckBox/checkbox-outline-unchecked.svg' margin='2,0,0,0' valign='center' fade='80'\" selected_normal_image=\"file='public/CheckBox/checkbox-outline-checked.svg' margin='2,0,0,0' valign='center'\" selected_disabled_image=\"file='public/CheckBox/checkbox-outline-checked.svg' margin='2,0,0,0' valign='center' fade='80'\""));

    // Corresponding to the list_box.xml layout
    auto* pRoot = ui::Create<ui::VBox>(this, {});
    pRoot->SetBkColor(DUI_T("bk_wnd_darkcolor"));

    // Title bar area
    auto* pCaption = ui::Create<ui::HBox>(this, {{DUI_T("name"), DUI_T("window_caption_bar")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("36")}});
    pCaption->SetBkColor(DUI_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    auto* pTitle = ui::Create<ui::Label>(this, {{DUI_T("font"), DUI_T("system_14")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("mouse_enabled"), DUI_T("false")}});
    pTitle->SetText(DUI_T("List (VTileListBox | HTileListBox | VListBox | HListBox)"));
    pCaption->AddItem(pTitle);

    auto* pCaptionBtns = ui::Create<ui::HBox>(this, {{DUI_T("width"), DUI_T("auto")}});
    pCaption->AddItem(pCaptionBtns);

    auto* pMinBtn = ui::Create<ui::Button>(this, {{DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,2,0,2")}});
    pMinBtn->SetClass(DUI_T("btn_wnd_min_11"));
    pMinBtn->SetName(DUI_T("minbtn"));
    pMinBtn->SetToolTipText(DUI_T("Minimize"));
    pCaptionBtns->AddItem(pMinBtn);

    auto* pMaxBox = ui::Create<ui::Box>(this, {{DUI_T("height"), DUI_T("stretch")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,2,0,2")}});
    pCaptionBtns->AddItem(pMaxBox);

    auto* pMaxBtn = ui::Create<ui::Button>(this, {{DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("stretch")}});
    pMaxBtn->SetClass(DUI_T("btn_wnd_max_11"));
    pMaxBtn->SetName(DUI_T("maxbtn"));
    pMaxBtn->SetToolTipText(DUI_T("Maximize"));
    pMaxBox->AddItem(pMaxBtn);

    auto* pRestoreBtn = ui::Create<ui::Button>(this, {{DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("stretch")}});
    pRestoreBtn->SetClass(DUI_T("btn_wnd_restore_11"));
    pRestoreBtn->SetName(DUI_T("restorebtn"));
    pRestoreBtn->SetToolTipText(DUI_T("Restore"));
    pRestoreBtn->SetVisible(false);
    pMaxBox->AddItem(pRestoreBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{DUI_T("height"), DUI_T("stretch")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,0,0,2")}});
    pCloseBtn->SetClass(DUI_T("btn_wnd_close_11"));
    pCloseBtn->SetName(DUI_T("closebtn"));
    pCloseBtn->SetToolTipText(DUI_T("Close"));
    pCaptionBtns->AddItem(pCloseBtn);

    // List area
    auto* pContent = ui::Create<ui::Box>(this, {});
    pRoot->AddItem(pContent);

    auto* pListBox = ui::Create<ui::VListBox>(this, {{DUI_T("vscrollbar"), DUI_T("true")}, {DUI_T("hscrollbar"), DUI_T("true")}, {DUI_T("multi_select"), DUI_T("false")}, {DUI_T("scroll_select"), DUI_T("false")}});
    pListBox->SetName(DUI_T("list"));
    pListBox->SetBkColor(DUI_T("bk_wnd_lightcolor"));
    pContent->AddItem(pListBox);

    AttachBox(pRoot);
}

void ListBoxForm::OnInitWindow()
{
    SetupWindow();
    BuildUI();

    m_pListBox = ui::Find<ui::ListBox>(this, DUI_T("list"));
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
        m_pListBox->SetAttribute(DUI_T("columns"), DUI_T("2"));
        m_pListBox->SetAttribute(DUI_T("auto_calc_item_size"), DUI_T("true"));
    }
    else if (bHTileListBox) {
        //HTileListBox: set to fixed 2 rows, auto-calculate the Item height
        //m_pListBox->SetAttribute("item_size", "200，80");
        m_pListBox->SetAttribute(DUI_T("rows"), DUI_T("2"));
        m_pListBox->SetAttribute(DUI_T("auto_calc_item_size"), DUI_T("true"));
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

        DString img = DUI_T("icon.png");
        DString title = ui::StringUtil::Printf(DUI_T("Task [%02d]"), i);

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
        DString sInfo = GetEventDisplayInfo(args, pListBox);
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

DString ListBoxForm::GetEventDisplayInfo(const ui::EventArgs& args, ui::ListBox* pListBox)
{
    DString sInfo = ui::EventUtils::EventTypeToString(args.eventType);
    while (sInfo.size() < 24) {
        sInfo += DUI_T(" ");
    }
    if (args.eventType == ui::kEventSelect) {
        size_t nNewItemIndex = (size_t)args.wParam;
        size_t nOldItemIndex = (size_t)args.lParam;
        if (nOldItemIndex != ui::Box::InvalidIndex) {
            sInfo += ui::StringUtil::Printf(DUI_T("NewItemIndex=%zu, OldItemIndex=%zu"),
                                            nNewItemIndex, nOldItemIndex);
        }
        else {
            sInfo += ui::StringUtil::Printf(DUI_T("NewItemIndex=%zu"), nNewItemIndex);
        }
    }
    else if (args.eventType == ui::kEventUnSelect) {
        size_t nItemIndex = (size_t)args.wParam;
        sInfo += ui::StringUtil::Printf(DUI_T("ItemIndex=%zu"), nItemIndex);
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
            sInfo += DUI_T("no params");
        }
        else {
            sInfo += ui::StringUtil::Printf(DUI_T("ItemIndex=%zu"), nItemIndex);
        }
    }
    else if ((args.eventType == ui::kEventKeyDown) || (args.eventType == ui::kEventKeyUp)) {
        //Keyboard message
        DString keyName = ui::Keyboard::GetKeyName(args.vkCode, false);
        DString modifierKey;
        if (args.vkCode != ui::VirtualKeyCode::kVK_CONTROL) {
            if (ui::Keyboard::IsKeyDown(ui::VirtualKeyCode::kVK_CONTROL)) {
                modifierKey += DUI_T("Ctrl+");
            }
        }
        if (args.vkCode != ui::VirtualKeyCode::kVK_SHIFT) {
            if (ui::Keyboard::IsKeyDown(ui::VirtualKeyCode::kVK_SHIFT)) {
                modifierKey += DUI_T("Shift+");
            }
        }
        if (args.vkCode != ui::VirtualKeyCode::kVK_MENU) {
            if (ui::Keyboard::IsKeyDown(ui::VirtualKeyCode::kVK_MENU)) {
                modifierKey += DUI_T("Alt+");
            }
        }
        sInfo += DUI_T("<");
        sInfo += modifierKey;
        sInfo += keyName;
        sInfo += DUI_T(">");
        sInfo += DUI_T(" ");

        size_t nItemIndex = (size_t)args.wParam;
        if (nItemIndex == ui::Box::InvalidIndex) {
            sInfo += DUI_T("no params");
        }
        else {
            sInfo += ui::StringUtil::Printf(DUI_T("ItemIndex=%zu"), nItemIndex);
        }
    }
    else {
        ASSERT(0);
    }
    return sInfo;
}

void ListBoxForm::OutputDebugLog(const DString& logMsg)
{
#if defined DUI_BUILD_FOR_WIN && defined _DEBUG
    //::OutputDebugString(logMsg.c_str());
#endif
}
