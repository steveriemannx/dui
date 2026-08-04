#include "ListBoxForm.h"
#include "Item.h"

ListBoxForm::ListBoxForm():
    m_pListBox(nullptr)
{
}


ListBoxForm::~ListBoxForm()
{
}

DString ListBoxForm::GetSkinFolder()
{
    return _T("");
}

DString ListBoxForm::GetSkinFile()
{
    // Pure code mode: no layout XML is loaded
    return _T("");
}

void ListBoxForm::BuildUI()
{
    // Register the local style classes in item.xml (the Item template is now built in code)
    AddClass(_T("list_box_item_test"),
             _T(" hot_color=\"bk_listitem_hovered\" pushed_color=\"bk_listitem_selected\" selected_normal_color=\"bk_listitem_selected\" fade_hot=\"false\""));
    AddClass(_T("list_box_item_checkbox_1"),
             _T(" height=\"20\" text_padding=\"20,0,0,0\" font=\"system_14\" normal_image=\"file='public/CheckBox/checkbox-outline-unchecked.svg' margin='2,0,0,0' valign='center'\" disabled_image=\"file='public/CheckBox/checkbox-outline-unchecked.svg' margin='2,0,0,0' valign='center' fade='80'\" selected_normal_image=\"file='public/CheckBox/checkbox-outline-checked.svg' margin='2,0,0,0' valign='center'\" selected_disabled_image=\"file='public/CheckBox/checkbox-outline-checked.svg' margin='2,0,0,0' valign='center' fade='80'\""));

    // Corresponding to the list_box.xml layout
    ui::VBox* pRoot = new ui::VBox(this);
    pRoot->SetBkColor(_T("bk_wnd_darkcolor"));

    // Title bar area
    ui::HBox* pCaption = new ui::HBox(this);
    pCaption->SetAttribute(_T("name"), _T("window_caption_bar"));
    pCaption->SetAttribute(_T("width"), _T("stretch"));
    pCaption->SetAttribute(_T("height"), _T("36"));
    pCaption->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    ui::Label* pTitle = new ui::Label(this);
    pTitle->SetText(_T("List (VTileListBox | HTileListBox | VListBox | HListBox)"));
    pTitle->SetAttribute(_T("font"), _T("system_14"));
    pTitle->SetAttribute(_T("valign"), _T("center"));
    pTitle->SetAttribute(_T("margin"), _T("8"));
    pTitle->SetAttribute(_T("width"), _T("stretch"));
    pTitle->SetAttribute(_T("mouse_enabled"), _T("false"));
    pCaption->AddItem(pTitle);

    ui::HBox* pCaptionBtns = new ui::HBox(this);
    pCaptionBtns->SetAttribute(_T("width"), _T("auto"));
    pCaption->AddItem(pCaptionBtns);

    ui::Button* pMinBtn = new ui::Button(this);
    pMinBtn->SetClass(_T("btn_wnd_min_11"));
    pMinBtn->SetAttribute(_T("height"), _T("32"));
    pMinBtn->SetAttribute(_T("width"), _T("40"));
    pMinBtn->SetName(_T("minbtn"));
    pMinBtn->SetAttribute(_T("margin"), _T("0,2,0,2"));
    pMinBtn->SetToolTipText(_T("Minimize"));
    pCaptionBtns->AddItem(pMinBtn);

    ui::Box* pMaxBox = new ui::Box(this);
    pMaxBox->SetAttribute(_T("height"), _T("stretch"));
    pMaxBox->SetAttribute(_T("width"), _T("40"));
    pMaxBox->SetAttribute(_T("margin"), _T("0,2,0,2"));
    pCaptionBtns->AddItem(pMaxBox);

    ui::Button* pMaxBtn = new ui::Button(this);
    pMaxBtn->SetClass(_T("btn_wnd_max_11"));
    pMaxBtn->SetAttribute(_T("height"), _T("32"));
    pMaxBtn->SetAttribute(_T("width"), _T("stretch"));
    pMaxBtn->SetName(_T("maxbtn"));
    pMaxBtn->SetToolTipText(_T("Maximize"));
    pMaxBox->AddItem(pMaxBtn);

    ui::Button* pRestoreBtn = new ui::Button(this);
    pRestoreBtn->SetClass(_T("btn_wnd_restore_11"));
    pRestoreBtn->SetAttribute(_T("height"), _T("32"));
    pRestoreBtn->SetAttribute(_T("width"), _T("stretch"));
    pRestoreBtn->SetName(_T("restorebtn"));
    pRestoreBtn->SetVisible(false);
    pRestoreBtn->SetToolTipText(_T("Restore"));
    pMaxBox->AddItem(pRestoreBtn);

    ui::Button* pCloseBtn = new ui::Button(this);
    pCloseBtn->SetClass(_T("btn_wnd_close_11"));
    pCloseBtn->SetAttribute(_T("height"), _T("stretch"));
    pCloseBtn->SetAttribute(_T("width"), _T("40"));
    pCloseBtn->SetName(_T("closebtn"));
    pCloseBtn->SetAttribute(_T("margin"), _T("0,0,0,2"));
    pCloseBtn->SetToolTipText(_T("Close"));
    pCaptionBtns->AddItem(pCloseBtn);

    // List area
    ui::Box* pContent = new ui::Box(this);
    pRoot->AddItem(pContent);

    ui::VListBox* pListBox = new ui::VListBox(this);
    pListBox->SetName(_T("list"));
    pListBox->SetBkColor(_T("bk_wnd_lightcolor"));
    pListBox->SetAttribute(_T("vscrollbar"), _T("true"));
    pListBox->SetAttribute(_T("hscrollbar"), _T("true"));
    pListBox->SetAttribute(_T("multi_select"), _T("false"));
    pListBox->SetAttribute(_T("scroll_select"), _T("false"));
    pContent->AddItem(pListBox);

    AttachBox(pRoot);
}

void ListBoxForm::OnInitWindow()
{
    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);

    BuildUI();

    m_pListBox = dynamic_cast<ui::ListBox*>(FindControl(_T("list")));
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
        //m_pListBox->SetAttribute(_T("item_size"), _T("200，80"));
        m_pListBox->SetAttribute(_T("columns"), _T("2"));
        m_pListBox->SetAttribute(_T("auto_calc_item_size"), _T("true"));
    }
    else if (bHTileListBox) {
        //HTileListBox: set to fixed 2 rows, auto-calculate the Item height
        //m_pListBox->SetAttribute(_T("item_size"), _T("200，80"));
        m_pListBox->SetAttribute(_T("rows"), _T("2"));
        m_pListBox->SetAttribute(_T("auto_calc_item_size"), _T("true"));
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

        DString img = _T("icon.png");
        DString title = ui::StringUtil::Printf(_T("Task [%02d]"), i);

        item->InitSubControls(img, title);
        m_pListBox->AddItem(item);
    }

    m_pListBox->SetFocus();

    //Test the ListBox events
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
        sInfo += _T(" ");
    }
    if (args.eventType == ui::kEventSelect) {
        size_t nNewItemIndex = (size_t)args.wParam;
        size_t nOldItemIndex = (size_t)args.lParam;
        if (nOldItemIndex != ui::Box::InvalidIndex) {
            sInfo += ui::StringUtil::Printf(_T("NewItemIndex=%zu, OldItemIndex=%zu"),
                                            nNewItemIndex, nOldItemIndex);
        }
        else {
            sInfo += ui::StringUtil::Printf(_T("NewItemIndex=%zu"), nNewItemIndex);
        }
    }
    else if (args.eventType == ui::kEventUnSelect) {
        size_t nItemIndex = (size_t)args.wParam;
        sInfo += ui::StringUtil::Printf(_T("ItemIndex=%zu"), nItemIndex);
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
            sInfo += _T("no params");
        }
        else {
            sInfo += ui::StringUtil::Printf(_T("ItemIndex=%zu"), nItemIndex);
        }
    }
    else if ((args.eventType == ui::kEventKeyDown) || (args.eventType == ui::kEventKeyUp)) {
        //Keyboard message
        DString keyName = ui::Keyboard::GetKeyName(args.vkCode, false);
        DString modifierKey;
        if (args.vkCode != ui::VirtualKeyCode::kVK_CONTROL) {
            if (ui::Keyboard::IsKeyDown(ui::VirtualKeyCode::kVK_CONTROL)) {
                modifierKey += _T("Ctrl+");
            }
        }
        if (args.vkCode != ui::VirtualKeyCode::kVK_SHIFT) {
            if (ui::Keyboard::IsKeyDown(ui::VirtualKeyCode::kVK_SHIFT)) {
                modifierKey += _T("Shift+");
            }
        }
        if (args.vkCode != ui::VirtualKeyCode::kVK_MENU) {
            if (ui::Keyboard::IsKeyDown(ui::VirtualKeyCode::kVK_MENU)) {
                modifierKey += _T("Alt+");
            }
        }
        sInfo += _T("<");
        sInfo += modifierKey;
        sInfo += keyName;
        sInfo += _T(">");
        sInfo += _T(" ");

        size_t nItemIndex = (size_t)args.wParam;
        if (nItemIndex == ui::Box::InvalidIndex) {
            sInfo += _T("no params");
        }
        else {
            sInfo += ui::StringUtil::Printf(_T("ItemIndex=%zu"), nItemIndex);
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
