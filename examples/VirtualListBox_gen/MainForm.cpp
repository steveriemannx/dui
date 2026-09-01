#include "MainForm.h"
#include "DataProvider.h"
#include "dui/Utils/UiBuilder.h"
#include "generated_ui.inc"  // Build-time generated pure C++ UI code (from main.xml)

MainForm::MainForm():
    m_pTileList(nullptr),
    m_pDataProvider(nullptr),
    m_pEditColumn(nullptr),
    m_pEditTotal(nullptr),
    m_pOptionColumnFix(nullptr),
    m_pEditUpdate(nullptr),
    m_pEditTaskName(nullptr),
    m_pEditDelete(nullptr),
    m_pEditChildMarginX(nullptr),
    m_pEditChildMarginY(nullptr)
{

}

void MainForm::BuildUI()
{
    InitMain(this);
}

void MainForm::BindEvents()
{
    // Manually wire up the <Event> tags in the XML:
    // option_column_fix select → edit_column visible; unselect → hidden
    ui::Option* pOptionFix = ui::Find<ui::Option>(this, "option_column_fix");
    if (pOptionFix != nullptr) {
        pOptionFix->AttachSelect([this](const ui::EventArgs&) {
            if (auto* pEditColumn = ui::Find<ui::Control>(this, "edit_column")) {
                pEditColumn->SetVisible(true);
            }
            return true;
        });
        pOptionFix->AttachUnSelect([this](const ui::EventArgs&) {
            if (auto* pEditColumn = ui::Find<ui::Control>(this, "edit_column")) {
                pEditColumn->SetVisible(false);
            }
            return true;
        });
    }

    GetRoot()->AttachBubbledEvent(ui::kEventClick, UiBind(&MainForm::OnClicked, this, std::placeholders::_1), 0);

    ui::Option* pOptionAlign1 = ui::Find<ui::Option>(this, "option_align1");
    ui::Option* pOptionAlign2 = ui::Find<ui::Option>(this, "option_align2");
    ui::Option* pOptionAlign3 = ui::Find<ui::Option>(this, "option_align3");
    if ((pOptionAlign1 != nullptr) && (pOptionAlign2 != nullptr) && (pOptionAlign3 != nullptr)) {
        ui::LayoutType layoutType = m_pTileList->GetLayout()->GetLayoutType();
        if ((layoutType == ui::LayoutType::VirtualHTileLayout) || (layoutType == ui::LayoutType::VirtualHLayout)) {
            //Horizontal layout
            pOptionAlign1->AttachSelect([this](const ui::EventArgs&) {
                if (m_pTileList) {
                    m_pTileList->SetAttribute("child_valign", "top");
                    m_pTileList->Refresh();
                }
                return true;
                });
            pOptionAlign2->AttachSelect([this](const ui::EventArgs&) {
                if (m_pTileList) {
                    m_pTileList->SetAttribute("child_valign", "center");
                    m_pTileList->Refresh();
                }
                return true;
                });
            pOptionAlign3->AttachSelect([this](const ui::EventArgs&) {
                if (m_pTileList) {
                    m_pTileList->SetAttribute("child_valign", "bottom");
                    m_pTileList->Refresh();
                }
                return true;
                });
        }
        else {
            //Vertical layout
            pOptionAlign1->AttachSelect([this](const ui::EventArgs&) {
                if (m_pTileList) {
                    m_pTileList->SetAttribute("child_halign", "left");
                    m_pTileList->Refresh();
                }
                return true;
                });
            pOptionAlign2->AttachSelect([this](const ui::EventArgs&) {
                if (m_pTileList) {
                    m_pTileList->SetAttribute("child_halign", "center");
                    m_pTileList->Refresh();
                }
                return true;
                });
            pOptionAlign3->AttachSelect([this](const ui::EventArgs&) {
                if (m_pTileList) {
                    m_pTileList->SetAttribute("child_halign", "right");
                    m_pTileList->Refresh();
                }
                return true;
                });
        }
    }

    //Test the virtual list events
    TestVirtualListBoxEvents(m_pTileList);
}

void MainForm::OnInitWindow()
{
    BuildUI();

    m_pEditColumn = ui::Find<ui::RichEdit>(this, "edit_column");
    m_pEditTotal = ui::Find<ui::RichEdit>(this, "edit_total");
    m_pTileList = ui::Find<ui::VirtualListBox>(this, "list");
    m_pOptionColumnFix = ui::Find<ui::Option>(this, "option_column_fix");
    m_pEditUpdate = ui::Find<ui::RichEdit>(this, "edit_update");
    m_pEditTaskName = ui::Find<ui::RichEdit>(this, "edit_task_name");
    m_pEditDelete = ui::Find<ui::RichEdit>(this, "edit_delete");
    m_pEditChildMarginX = ui::Find<ui::RichEdit>(this, "edit_child_margin_x");
    m_pEditChildMarginY = ui::Find<ui::RichEdit>(this, "edit_child_margin_y");

    // Set the data provider
    m_pDataProvider = new DataProvider;
    m_pTileList->SetDataProvider(m_pDataProvider);

    const ui::LayoutType layoutType = m_pTileList->GetLayout()->GetLayoutType();
    if ((layoutType != ui::LayoutType::VirtualHTileLayout) &&
        (layoutType != ui::LayoutType::VirtualVTileLayout)){
        if (m_pEditColumn != nullptr) {
            m_pEditColumn->SetEnabled(false);
            m_pEditColumn->SetText("  ");
        }
    }

    if (layoutType == ui::LayoutType::VirtualHTileLayout) {
        m_pOptionColumnFix->SetText("Fixed Rows");
    }
    else if (layoutType == ui::LayoutType::VirtualVTileLayout) {
        m_pOptionColumnFix->SetText("Fixed Columns");
    }
    else {
        m_pOptionColumnFix->SetEnabled(false);
        if (auto* pControl = ui::Find<ui::Control>(this, "option_column_auto")) {
            pControl->SetEnabled(false);
        }
        if (auto* pControl = ui::Find<ui::Control>(this, "label_column_row")) {
            pControl->SetEnabled(false);
        }
    }

    ui::Option* pOptionAlign1 = ui::Find<ui::Option>(this, "option_align1");
    ui::Option* pOptionAlign2 = ui::Find<ui::Option>(this, "option_align2");
    ui::Option* pOptionAlign3 = ui::Find<ui::Option>(this, "option_align3");
    if ((pOptionAlign1 != nullptr) && (pOptionAlign2 != nullptr) && (pOptionAlign3 != nullptr)) {
        if ((layoutType == ui::LayoutType::VirtualHTileLayout) || (layoutType == ui::LayoutType::VirtualHLayout)) {
            pOptionAlign1->SetText("Align Top (top)");
            pOptionAlign2->SetText("Align Center (center)");
            pOptionAlign3->SetText("Align Bottom (bottom)");
            pOptionAlign2->Selected(true, true);
        }
        else {
            pOptionAlign1->SetText("Align Left (left)");
            pOptionAlign2->SetText("Align Center (center)");
            pOptionAlign3->SetText("Align Right (right)");
            pOptionAlign2->Selected(true, true);
        }
    }

    ui::Label* pListType = ui::Find<ui::Label>(this, "list_box_type");
    if (pListType != nullptr) {
        if (m_pTileList->GetType() == DUI_CTR_VIRTUAL_HLISTBOX) {
            pListType->SetText("VirtualHListBox");
        }
        else if (m_pTileList->GetType() == DUI_CTR_VIRTUAL_VLISTBOX) {
            pListType->SetText("VirtualVListBox");
        }
        else if (m_pTileList->GetType() == DUI_CTR_VIRTUAL_HTILE_LISTBOX) {
            pListType->SetText("VirtualHTileListBox");
        }
        else if (m_pTileList->GetType() == DUI_CTR_VIRTUAL_VTILE_LISTBOX) {
            pListType->SetText("VirtualVTileListBox");
        }
    }

    if (layoutType == ui::LayoutType::VirtualHLayout) {
        if (m_pEditChildMarginY != nullptr) {
            m_pEditChildMarginY->SetEnabled(false);
        }
        if (auto* pControl = ui::Find<ui::Control>(this, "label_child_margin_y")) {
            pControl->SetEnabled(false);
        }
    }
    else if (layoutType == ui::LayoutType::VirtualVLayout) {
        if (m_pEditChildMarginX != nullptr) {
            m_pEditChildMarginX->SetEnabled(false);
        }
        if (auto* pControl = ui::Find<ui::Control>(this, "label_child_margin_x")) {
            pControl->SetEnabled(false);
        }
    }

    if ((m_pDataProvider != nullptr) && (m_pEditTotal != nullptr)) {
        int nTotal = ui::StringUtil::StringToInt32(m_pEditTotal->GetText().c_str());
        if (nTotal > 0) {
            m_pDataProvider->SetTotal(nTotal);
        }
    }

    BindEvents();
    BaseClass::OnInitWindow();
}

bool MainForm::OnClicked(const ui::EventArgs& args)
{
    auto sName = args.GetSender()->GetName();
    if (sName == "btn_set_total")
    {
        if (!m_pEditChildMarginX->GetText().empty())    {
            m_pTileList->SetAttribute("child_margin_x", m_pEditChildMarginX->GetText());
        }
        if (!m_pEditChildMarginY->GetText().empty()) {
            m_pTileList->SetAttribute("child_margin_y", m_pEditChildMarginY->GetText());
        }
        if (m_pOptionColumnFix->IsSelected()) {
            //Fixed number of columns/rows
            ui::LayoutType layoutType = m_pTileList->GetLayout()->GetLayoutType();
            if (layoutType == ui::LayoutType::VirtualVTileLayout) {
                m_pTileList->SetAttribute("columns", m_pEditColumn->GetText());
            }
            if (layoutType == ui::LayoutType::VirtualHTileLayout) {
                m_pTileList->SetAttribute("rows", m_pEditColumn->GetText());
            }            
        }
        else {
            //Auto-calculate the number of columns
            m_pTileList->SetAttribute("width", "stretch");
            ui::LayoutType layoutType = m_pTileList->GetLayout()->GetLayoutType();
            if (layoutType == ui::LayoutType::VirtualVTileLayout) {
                m_pTileList->SetAttribute("columns", "auto");
            }
            if (layoutType == ui::LayoutType::VirtualHTileLayout) {
                m_pTileList->SetAttribute("rows", "auto");
            }
        }

        int nTotal = ui::StringUtil::StringToInt32(m_pEditTotal->GetText().c_str());
        if (nTotal > 0) {
            m_pDataProvider->SetTotal(nTotal);
        }
    }
    else if (sName == "btn_update") {
        size_t nIndex = (size_t)ui::StringUtil::StringToInt32(m_pEditUpdate->GetText().c_str());
        ASSERT(nIndex < m_pDataProvider->GetElementCount());
        m_pDataProvider->ChangeTaskName(nIndex, m_pEditTaskName->GetText());
    }
    else if (sName == "btn_delete") {
        size_t nIndex = (size_t)ui::StringUtil::StringToInt32(m_pEditDelete->GetText().c_str());
        ASSERT(nIndex < m_pDataProvider->GetElementCount());
        m_pDataProvider->RemoveTask(nIndex);
    }
    return true;
}

void MainForm::TestVirtualListBoxEvents(ui::VirtualListBox* pListBox)
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
    pListBox->AttachElementFilled([this, OnVirtualListBoxEvents](const ui::EventArgs& args) {
        OnVirtualListBoxEvents(args);
        return true;
        });
}

DString MainForm::GetEventDisplayInfo(const ui::EventArgs& args, ui::VirtualListBox* pListBox)
{
    DString sInfo = ui::EventUtils::EventTypeToString(args.eventType);
    while (sInfo.size() < 24) {
        sInfo += " ";
    }
    if (args.eventType == ui::kEventSelect) {
        size_t nNewItemIndex = (size_t)args.wParam;
        size_t nOldItemIndex = (size_t)args.lParam;
        size_t nNewElementID = pListBox->GetDisplayItemElementIndex(nNewItemIndex);
        if (nOldItemIndex != ui::Box::InvalidIndex) {
            size_t nOldElementID = pListBox->GetDisplayItemElementIndex(nOldItemIndex);
            sInfo += ui::StringUtil::Printf("NewItemIndex=%zu, NewElementID=%zu; OldItemIndex=%zu, OldElementID=%zu",
                                            nNewItemIndex, nNewElementID, nOldItemIndex, nOldElementID);
        }
        else {
            sInfo += ui::StringUtil::Printf("NewItemIndex=%zu, NewElementID=%zu", nNewItemIndex, nNewElementID);
        }
    }
    else if (args.eventType == ui::kEventSelChanged) {
        //No parameters
    }
    else if ((args.eventType == ui::kEventItemMouseEnter) ||
             (args.eventType == ui::kEventItemMouseLeave) ||
             (args.eventType == ui::kEventMouseDoubleClick) ||
             (args.eventType == ui::kEventClick)  ||
             (args.eventType == ui::kEventRClick) ||
             (args.eventType == ui::kEventReturn)) {
        size_t nItemIndex = (size_t)args.wParam;
        size_t nElementID = (size_t)args.lParam;
        if (nItemIndex == ui::Box::InvalidIndex) {
            sInfo += "no params";
        }
        else {
            size_t nCalcElementID = pListBox->GetDisplayItemElementIndex(nItemIndex);
            ASSERT(nElementID == nCalcElementID);
            sInfo += ui::StringUtil::Printf("ItemIndex=%zu, ElementID=%zu", nItemIndex, nElementID);
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
        size_t nElementID = (size_t)args.lParam;
        if (nItemIndex == ui::Box::InvalidIndex) {
            sInfo += "no params";
        }
        else {
            size_t nCalcElementID = pListBox->GetDisplayItemElementIndex(nItemIndex);
            ASSERT(nElementID == nCalcElementID);
            sInfo += ui::StringUtil::Printf("ItemIndex=%zu, ElementID=%zu", nItemIndex, nElementID);
        }
    }
    else if (args.eventType == ui::kEventElementFilled) {
        size_t nItemIndex = (size_t)args.wParam;
        size_t nElementID = (size_t)args.lParam;
        size_t nCalcElementID = pListBox->GetDisplayItemElementIndex(nItemIndex);
        ASSERT(nElementID == nCalcElementID);
        sInfo += ui::StringUtil::Printf("ItemIndex=%zu, ElementID=%zu, ListBoxItem: 0x%p", nItemIndex, nElementID, args.pEventData);
        ui::IListBoxItem* pListBoxItem = dynamic_cast<ui::IListBoxItem*>((ui::Control*)args.pEventData);
        ASSERT(pListBoxItem != nullptr);
        if ((pListBoxItem != nullptr)) {
            ASSERT(pListBoxItem->GetListBoxIndex() == nItemIndex);
            ASSERT(pListBoxItem->GetElementIndex() == nElementID);
        }
    }
    else {
        ASSERT(0);
    }
    return sInfo;
}

void MainForm::OutputDebugLog(const DString& logMsg)
{
#if defined DUI_BUILD_FOR_WIN && defined _DEBUG
    //::OutputDebugString(logMsg.c_str());
#endif
}

