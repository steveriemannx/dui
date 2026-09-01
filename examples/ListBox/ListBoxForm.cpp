#include "ListBoxForm.h"
#include "Item.h"
#include "dui/Utils/UiBuilder.h"

void ListBoxForm::OnInitWindow()
{
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
        Item* item = new Item(this);
        ui::GlobalManager::Instance().FillBoxWithCache(item, ui::FilePath("list_box/item.xml"));

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

        DString img = "icon.png";
        DString title = ui::StringUtil::Printf("Task [%02d]", i);

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
        DString keyName = ui::Keyboard::GetKeyName(args.vkCode, false);
        DString modifierKey;
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

void ListBoxForm::OutputDebugLog(const DString& logMsg)
{
#if defined DUI_BUILD_FOR_WIN && defined _DEBUG
    //::OutputDebugString(logMsg.c_str());
#endif
}
