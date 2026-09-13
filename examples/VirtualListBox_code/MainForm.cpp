#include "MainForm.h"
#include "DataProvider.h"
#include "dui/Utils/UiBuilder.h"

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

void MainForm::SetupWindow()
{
    ui::UiRect rcWork;
    ui::WindowBase::GetPrimaryMonitorWorkRect(rcWork);
    SetWindowSize(static_cast<int32_t>(rcWork.Width() * 0.75f),
                  static_cast<int32_t>(rcWork.Height() * 0.75f));
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

    SetWindowMinimumSize(ui::UiSize(750, 500), true);
}

void MainForm::BuildUI()
{
    // Corresponding to the main.xml layout
    auto* pRoot = ui::Create<ui::VBox>(this, {});
    pRoot->SetBkColor("bk_wnd_darkcolor");

    // Title bar area
    auto* pCaption = ui::Create<ui::HBox>(this, {{"name", "window_caption_bar"}, {"width", "stretch"}, {"height", "36"}});
    pCaption->SetBkColor("bk_wnd_lightcolor");
    pRoot->AddItem(pCaption);

    auto* pTitle = ui::Create<ui::Label>(this, {{"font", "system_14"}, {"valign", "center"}, {"margin", "8"}, {"mouse_enabled", "false"}});
    pTitle->SetText("Virtual List (VirtualHTileListBox | VirtualVTileListBox | VirtualHListBox | VirtualVListBox)");
    pCaption->AddItem(pTitle);

    auto* pSpacer = ui::Create<ui::Control>(this, {{"mouse_enabled", "false"}});
    pCaption->AddItem(pSpacer);

    auto* pMinBtn = ui::Create<ui::Button>(this, {{"height", "32"}, {"width", "40"}, {"margin", "0,2,0,2"}});
    pMinBtn->SetClass("btn_wnd_min_11");
    pMinBtn->SetName("minbtn");
    pMinBtn->SetToolTipText("Minimize");
    pCaption->AddItem(pMinBtn);

    auto* pMaxBox = ui::Create<ui::Box>(this, {{"height", "stretch"}, {"width", "40"}, {"margin", "0,2,0,2"}});
    pCaption->AddItem(pMaxBox);

    auto* pMaxBtn = ui::Create<ui::Button>(this, {{"height", "32"}, {"width", "stretch"}});
    pMaxBtn->SetClass("btn_wnd_max_11");
    pMaxBtn->SetName("maxbtn");
    pMaxBtn->SetToolTipText("Maximize");
    pMaxBox->AddItem(pMaxBtn);

    auto* pRestoreBtn = ui::Create<ui::Button>(this, {{"height", "32"}, {"width", "stretch"}});
    pRestoreBtn->SetClass("btn_wnd_restore_11");
    pRestoreBtn->SetName("restorebtn");
    pRestoreBtn->SetVisible(false);
    pRestoreBtn->SetToolTipText("Restore");
    pMaxBox->AddItem(pRestoreBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{"height", "stretch"}, {"width", "40"}, {"margin", "0,0,0,2"}});
    pCloseBtn->SetClass("btn_wnd_close_11");
    pCloseBtn->SetName("closebtn");
    pCloseBtn->SetToolTipText("Close");
    pCaption->AddItem(pCloseBtn);

    // Virtual list type display
    auto* pTypeRow = ui::Create<ui::HBox>(this, {{"height", "30"}});
    pRoot->AddItem(pTypeRow);

    auto* pTypeLabel = ui::Create<ui::Label>(this, {{"font", "system_14"}, {"valign", "center"}, {"margin", "8,0,8,0"}});
    pTypeLabel->SetText("Current virtual list container type:");
    pTypeRow->AddItem(pTypeLabel);

    auto* pListType = ui::Create<ui::Label>(this, {{"font", "system_14"}, {"valign", "center"}});
    pListType->SetName("list_box_type");
    pListType->SetText("VirtualHTileListBox");
    pTypeRow->AddItem(pListType);

    // Main content area
    auto* pMain = ui::Create<ui::HBox>(this, {{"child_margin", "5"}, {"padding", "5,5,5,10"}});
    pRoot->AddItem(pMain);

    // Left settings panel
    auto* pSettings = ui::Create<ui::VBox>(this, {{"width", "300"}, {"border_size", "1"}, {"border_color", "splitline_level1"}, {"padding", "5,10,5,10"}, {"child_margin", "10"}});
    pMain->AddItem(pSettings);

    // Row/column settings
    auto* pColumnGroup = ui::Create<ui::VBox>(this, {{"height", "80"}});
    pSettings->AddItem(pColumnGroup);

    auto* pColumnLabel = ui::Create<ui::Label>(this, {{"font", "system_bold_14"}, {"width", "auto"}, {"height", "auto"}, {"valign", "center"}});
    pColumnLabel->SetName("label_column_row");
    pColumnLabel->SetText("Rows/Columns:");
    pColumnGroup->AddItem(pColumnLabel);

    auto* pOptionColumnAuto = ui::Create<ui::Option>(this, {{"group", "option_group"}, {"height", "28"}, {"margin", "80,0,0,0"}});
    pOptionColumnAuto->SetClass("option_1");
    pOptionColumnAuto->SetName("option_column_auto");
    pOptionColumnAuto->SetText("Auto Calculate");
    pOptionColumnAuto->Selected(true);
    pColumnGroup->AddItem(pOptionColumnAuto);

    auto* pColumnFixRow = ui::Create<ui::HBox>(this, {{"height", "30"}});
    pColumnGroup->AddItem(pColumnFixRow);

    auto* pOptionColumnFix = ui::Create<ui::Option>(this, {{"group", "option_group"}, {"margin", "80,0,0,0"}});
    pOptionColumnFix->SetClass("option_1");
    pOptionColumnFix->SetName("option_column_fix");
    pOptionColumnFix->SetText("Fixed Columns");
    pColumnFixRow->AddItem(pOptionColumnFix);

    auto* pEditColumn = ui::Create<ui::RichEdit>(this, {{"min_number", "1"}, {"max_number", "12"}, {"text_padding", "1,0,8,0"}, {"width", "50"}, {"height", "28"}, {"margin", "6,0,0,0"}, {"default_context_menu", "true"}, {"number_only", "true"}, {"limit_text", "3"}, {"valign", "center"}, {"text_align", "vcenter"}, {"border_size", "1"}, {"border_color", "light_gray"}});
    pEditColumn->SetClass("simple rich_edit_spin");
    pEditColumn->SetName("edit_column");
    pEditColumn->SetText("3");
    pEditColumn->SetVisible(false);
    pEditColumn->SetBkColor("white");
    pColumnFixRow->AddItem(pEditColumn);

    auto* pSplit1 = ui::Create<ui::Control>(this, {{"height", "1"}});
    pSplit1->SetBkColor("splitline_level1");
    pSettings->AddItem(pSplit1);

    // Alignment
    auto* pAlignGroup = ui::Create<ui::VBox>(this, {{"height", "auto"}});
    pSettings->AddItem(pAlignGroup);

    auto* pAlignLabel = ui::Create<ui::Label>(this, {{"font", "system_bold_14"}, {"width", "auto"}, {"height", "auto"}, {"valign", "center"}});
    pAlignLabel->SetText("Alignment:");
    pAlignGroup->AddItem(pAlignLabel);

    auto* pOptionAlign1 = ui::Create<ui::Option>(this, {{"group", "option_group_align"}, {"height", "28"}, {"margin", "80,0,0,0"}});
    pOptionAlign1->SetClass("option_1");
    pOptionAlign1->SetName("option_align1");
    pOptionAlign1->SetText("Align Left (left)");
    pOptionAlign1->Selected(true);
    pAlignGroup->AddItem(pOptionAlign1);

    auto* pOptionAlign2 = ui::Create<ui::Option>(this, {{"group", "option_group_align"}, {"height", "28"}, {"margin", "80,0,0,0"}});
    pOptionAlign2->SetClass("option_1");
    pOptionAlign2->SetName("option_align2");
    pOptionAlign2->SetText("Align Center (center)");
    pAlignGroup->AddItem(pOptionAlign2);

    auto* pOptionAlign3 = ui::Create<ui::Option>(this, {{"group", "option_group_align"}, {"height", "28"}, {"margin", "80,0,0,0"}});
    pOptionAlign3->SetClass("option_1");
    pOptionAlign3->SetName("option_align3");
    pOptionAlign3->SetText("Align Right (right)");
    pAlignGroup->AddItem(pOptionAlign3);

    auto* pSplit2 = ui::Create<ui::Control>(this, {{"height", "1"}});
    pSplit2->SetBkColor("splitline_level1");
    pSettings->AddItem(pSplit2);

    // Child spacing
    auto* pMarginGroup = ui::Create<ui::VBox>(this, {{"height", "auto"}});
    pSettings->AddItem(pMarginGroup);

    auto* pMarginXRow = ui::Create<ui::HBox>(this, {{"height", "30"}, {"child_margin", "10"}});
    pMarginGroup->AddItem(pMarginXRow);

    auto* pMarginXLabel = ui::Create<ui::Label>(this, {{"font", "system_bold_14"}, {"width", "auto"}, {"height", "auto"}, {"valign", "center"}});
    pMarginXLabel->SetName("label_child_margin_x");
    pMarginXLabel->SetText("Child Margin X:");
    pMarginXRow->AddItem(pMarginXLabel);

    auto* pMarginXBox = ui::Create<ui::Box>(this, {{"border_size", "1"}, {"border_color", "splitline_level1"}, {"width", "90"}, {"height", "26"}, {"valign", "center"}});
    pMarginXRow->AddItem(pMarginXBox);

    auto* pEditMarginX = ui::Create<ui::RichEdit>(this, {{"font", "system_14"}, {"width", "stretch"}, {"height", "stretch"}, {"valign", "center"}, {"normal_text_color", "black"}, {"multi_line", "false"}, {"prompt_mode", "true"}, {"prompt_text", "Child Margin X"}, {"text_align", "vcenter"}, {"text_padding", "2,0,0,0"}, {"prompt_color", "splitline_level1"}, {"auto_hscroll", "true"}});
    pEditMarginX->SetName("edit_child_margin_x");
    pEditMarginX->SetBkColor("white");
    pMarginXBox->AddItem(pEditMarginX);

    auto* pMarginYRow = ui::Create<ui::HBox>(this, {{"height", "30"}, {"child_margin", "10"}});
    pMarginGroup->AddItem(pMarginYRow);

    auto* pMarginYLabel = ui::Create<ui::Label>(this, {{"font", "system_bold_14"}, {"width", "auto"}, {"height", "auto"}, {"valign", "center"}});
    pMarginYLabel->SetName("label_child_margin_y");
    pMarginYLabel->SetText("Child Margin Y:");
    pMarginYRow->AddItem(pMarginYLabel);

    auto* pMarginYBox = ui::Create<ui::Box>(this, {{"border_size", "1"}, {"border_color", "splitline_level1"}, {"width", "90"}, {"height", "26"}, {"valign", "center"}});
    pMarginYRow->AddItem(pMarginYBox);

    auto* pEditMarginY = ui::Create<ui::RichEdit>(this, {{"font", "system_14"}, {"width", "stretch"}, {"height", "stretch"}, {"valign", "center"}, {"normal_text_color", "black"}, {"multi_line", "false"}, {"prompt_mode", "true"}, {"prompt_text", "Child Margin Y"}, {"text_align", "vcenter"}, {"text_padding", "2,0,0,0"}, {"prompt_color", "splitline_level1"}, {"auto_hscroll", "true"}});
    pEditMarginY->SetName("edit_child_margin_y");
    pEditMarginY->SetBkColor("white");
    pMarginYBox->AddItem(pEditMarginY);

    // Total data
    auto* pTotalRow = ui::Create<ui::HBox>(this, {{"height", "30"}, {"child_margin", "10"}});
    pSettings->AddItem(pTotalRow);

    auto* pTotalLabel = ui::Create<ui::Label>(this, {{"font", "system_bold_14"}, {"width", "auto"}, {"height", "auto"}, {"valign", "center"}});
    pTotalLabel->SetText("Total Data:");
    pTotalRow->AddItem(pTotalLabel);

    auto* pTotalBox = ui::Create<ui::Box>(this, {{"border_size", "1"}, {"border_color", "splitline_level1"}, {"width", "180"}, {"height", "26"}, {"valign", "center"}});
    pTotalRow->AddItem(pTotalBox);

    auto* pEditTotal = ui::Create<ui::RichEdit>(this, {{"number_only", "true"}, {"min_number", "1"}, {"font", "system_14"}, {"width", "stretch"}, {"height", "stretch"}, {"valign", "center"}, {"normal_text_color", "black"}, {"multi_line", "false"}, {"prompt_mode", "true"}, {"prompt_text", "Total Data"}, {"text_align", "vcenter"}, {"text_padding", "2,0,0,0"}, {"prompt_color", "splitline_level1"}, {"auto_hscroll", "true"}});
    pEditTotal->SetName("edit_total");
    pEditTotal->SetBkColor("white");
    pEditTotal->SetText("10000");
    pTotalBox->AddItem(pEditTotal);

    // Change/set buttons
    auto* pSetTotalRow = ui::Create<ui::Box>(this, {{"height", "30"}, {"child_margin", "10"}, {"width", "stretch"}});
    pSettings->AddItem(pSetTotalRow);

    auto* pSetTotalBtn = ui::Create<ui::Button>(this, {{"halign", "center"}, {"valign", "center"}});
    pSetTotalBtn->SetClass("btn_global_blue_80x30");
    pSetTotalBtn->SetName("btn_set_total");
    pSetTotalBtn->SetText("Apply/Set");
    pSetTotalRow->AddItem(pSetTotalBtn);

    auto* pSplit3 = ui::Create<ui::Control>(this, {{"height", "1"}});
    pSplit3->SetBkColor("splitline_level1");
    pSettings->AddItem(pSplit3);

    // Modify data
    auto* pUpdateRow = ui::Create<ui::HBox>(this, {{"height", "30"}, {"child_margin", "10"}});
    pSettings->AddItem(pUpdateRow);

    auto* pUpdateLabel = ui::Create<ui::Label>(this, {{"font", "system_bold_14"}, {"width", "auto"}, {"height", "auto"}, {"valign", "center"}});
    pUpdateLabel->SetText("Modify Data:");
    pUpdateRow->AddItem(pUpdateLabel);

    auto* pUpdateBox1 = ui::Create<ui::Box>(this, {{"border_size", "1"}, {"border_color", "splitline_level1"}, {"height", "26"}, {"valign", "center"}});
    pUpdateRow->AddItem(pUpdateBox1);

    auto* pEditUpdate = ui::Create<ui::RichEdit>(this, {{"number_only", "true"}, {"min_number", "0"}, {"font", "system_14"}, {"width", "stretch"}, {"height", "stretch"}, {"valign", "center"}, {"normal_text_color", "black"}, {"multi_line", "false"}, {"text_align", "vcenter"}, {"text_padding", "2,0,0,0"}, {"prompt_mode", "true"}, {"prompt_text", "Index (subscript)"}, {"prompt_color", "splitline_level1"}, {"auto_hscroll", "true"}});
    pEditUpdate->SetName("edit_update");
    pEditUpdate->SetBkColor("white");
    pUpdateBox1->AddItem(pEditUpdate);

    auto* pUpdateBox2 = ui::Create<ui::Box>(this, {{"border_size", "1"}, {"border_color", "splitline_level1"}, {"height", "26"}, {"valign", "center"}});
    pUpdateRow->AddItem(pUpdateBox2);

    auto* pEditTaskName = ui::Create<ui::RichEdit>(this, {{"font", "system_14"}, {"width", "stretch"}, {"height", "stretch"}, {"valign", "center"}, {"normal_text_color", "black"}, {"multi_line", "false"}, {"text_align", "vcenter"}, {"text_padding", "2,0,0,0"}, {"prompt_mode", "true"}, {"prompt_text", "New Task Name"}, {"prompt_color", "splitline_level1"}, {"auto_hscroll", "true"}});
    pEditTaskName->SetName("edit_task_name");
    pEditTaskName->SetBkColor("white");
    pUpdateBox2->AddItem(pEditTaskName);

    auto* pUpdateBtnRow = ui::Create<ui::Box>(this, {{"height", "30"}, {"child_margin", "10"}, {"width", "stretch"}});
    pSettings->AddItem(pUpdateBtnRow);

    auto* pUpdateBtn = ui::Create<ui::Button>(this, {{"halign", "center"}, {"valign", "center"}});
    pUpdateBtn->SetClass("btn_global_blue_80x30");
    pUpdateBtn->SetName("btn_update");
    pUpdateBtn->SetText("Update Data");
    pUpdateBtnRow->AddItem(pUpdateBtn);

    auto* pSplit4 = ui::Create<ui::Control>(this, {{"height", "1"}});
    pSplit4->SetBkColor("splitline_level1");
    pSettings->AddItem(pSplit4);

    // Delete data
    auto* pDeleteRow = ui::Create<ui::HBox>(this, {{"height", "30"}, {"child_margin", "10"}});
    pSettings->AddItem(pDeleteRow);

    auto* pDeleteLabel = ui::Create<ui::Label>(this, {{"font", "system_bold_14"}, {"width", "auto"}, {"height", "auto"}, {"valign", "center"}});
    pDeleteLabel->SetText("Delete Data:");
    pDeleteRow->AddItem(pDeleteLabel);

    auto* pDeleteBox = ui::Create<ui::Box>(this, {{"border_size", "1"}, {"border_color", "splitline_level1"}, {"height", "26"}, {"valign", "center"}});
    pDeleteRow->AddItem(pDeleteBox);

    auto* pEditDelete = ui::Create<ui::RichEdit>(this, {{"number_only", "true"}, {"min_number", "0"}, {"font", "system_14"}, {"width", "stretch"}, {"height", "stretch"}, {"valign", "center"}, {"text_align", "vcenter"}, {"text_padding", "2,0,0,0"}, {"normal_text_color", "black"}, {"multi_line", "false"}, {"prompt_mode", "true"}, {"prompt_text", "Index (subscript)"}, {"prompt_color", "splitline_level1"}, {"auto_hscroll", "true"}});
    pEditDelete->SetName("edit_delete");
    pEditDelete->SetBkColor("white");
    pDeleteBox->AddItem(pEditDelete);

    auto* pDeleteBtn = ui::Create<ui::Button>(this, {{"halign", "center"}, {"valign", "center"}});
    pDeleteBtn->SetClass("btn_global_red_80x30");
    pDeleteBtn->SetName("btn_delete");
    pDeleteBtn->SetText("Delete Data");
    pDeleteRow->AddItem(pDeleteBtn);

    // Virtual list on the right
    auto* pListArea = ui::Create<ui::Box>(this, {});
    pMain->AddItem(pListArea);

    auto* pList = ui::Create<ui::VirtualVTileListBox>(this, {{"frame_selection", "true"}, {"select_none_when_click_blank", "true"}, {"select_like_list_ctrl", "true"}, {"multi_select", "true"}, {"scroll_select", "false"}, {"item_size", "240,64"}, {"auto_calc_item_size", "false"}, {"vscrollbar", "true"}, {"hscrollbar", "true"}, {"border_size", "1"}, {"border_color", "splitline_level1"}, {"padding", "1,1,1,1"}});
    pList->SetClass("list");
    pList->SetName("list");
    pList->SetBkColor("white");
    pListArea->AddItem(pList);

    AttachBox(pRoot);

    // Initialize control pointers
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

    pOptionAlign1 = ui::Find<ui::Option>(this, "option_align1");
    pOptionAlign2 = ui::Find<ui::Option>(this, "option_align2");
    pOptionAlign3 = ui::Find<ui::Option>(this, "option_align3");
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

    pListType = ui::Find<ui::Label>(this, "list_box_type");
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
    SetupWindow();
    BuildUI();
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
        std::string sInfo = GetEventDisplayInfo(args, pListBox);
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

std::string MainForm::GetEventDisplayInfo(const ui::EventArgs& args, ui::VirtualListBox* pListBox)
{
    std::string sInfo = ui::EventUtils::EventTypeToString(args.eventType);
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

void MainForm::OutputDebugLog(const std::string& logMsg)
{
#if defined DUI_BUILD_FOR_WIN && defined _DEBUG
    //::OutputDebugString(logMsg.c_str());
#endif
}
