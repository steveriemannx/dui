#include "MainForm.h"
#include "dui/Utils/StringConvert.h"
#include "dui/Utils/UiBuilder.h"

MainForm::MainForm():
    m_fLoadingPercent(0)
{
}

void MainForm::SetupWindow()
{
    ui::UiSize size; bool scaledCX = false; bool scaledCY = false;
    bool percentCX = false; bool percentCY = false;
    ui::AttributeUtil::ParseWindowSize(this, "85%,85%", size, &scaledCX, &scaledCY, &percentCX, &percentCY);
    SetWindowSize(size.cx, size.cy);
    SetWindowMinimumSize(ui::UiSize(80, 50), true);
    SetUseSystemCaption(false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), true);
#if !defined(DUI_BUILD_FOR_LINUX)
    SetLayeredWindow(false, false);
#endif
    SetSizeBox(ui::UiRect(4, 4, 4, 4), true);
#if defined(DUI_BUILD_FOR_LINUX)
    SetShadowType(ui::Shadow::ShadowType::kShadowDrawDefault);
#else
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
#endif
    SetShadowBorderSize(0);
    SetShadowAttached(true);
    // System shadow type: normalize and force non-layered window
    ui::Shadow::ShadowType supportedType =
        ui::Shadow::GetSupportedShadowType(this, ui::Shadow::ShadowType::kShadowSystemDefault);
    if (supportedType != ui::Shadow::ShadowType::kShadowSystemDefault) {
        SetShadowType(supportedType);
    }
    if (ui::Shadow::IsSystemShadowType(supportedType)) {
#if !defined(DUI_BUILD_FOR_LINUX)
        SetLayeredWindow(false, false);
#endif
    }
    CenterWindow();
}

void MainForm::BuildUI()
{
    // Root VBox
    auto* pRoot = ui::Create<ui::VBox>(this, {{"bkcolor", "bk_wnd_darkcolor"}, {"visible", "true"}});

    // ---- Title bar ----
    auto* pCaption = ui::Create<ui::HBox>(this, {{"name", "window_caption_bar"}, {"width", "stretch"}, {"height", "36"}, {"bkcolor", "bk_wnd_lightcolor"}});
    ui::Attach(pRoot, pCaption);
    // Title bar: display area at the top-left of the window (cross-platform)
    auto* pCaptionLeft = ui::Create<ui::HBox>(this, {{"margin", "0,0,30,0"}, {"valign", "center"}, {"width", "auto"}, {"height", "auto"}, {"mouse_enabled", "false"}});
    pCaption->AddItem(pCaptionLeft);
    auto* pTitle = ui::Create<ui::Label>(this, {{"valign", "center"}, {"margin", "8,0,0,0"}, {"mouse_enabled", "false"}});
    pTitle->SetText("ListCtrl");
    pCaptionLeft->AddItem(pTitle);
    auto* pCaptionSpacer = ui::Create<ui::Control>(this, {{"mouse_enabled", "false"}});
    ui::Attach(pCaption, pCaptionSpacer);
    auto* pFullscreenBtn = ui::Create<ui::Button>(this, {{"class", "btn_wnd_fullscreen_11"}, {"height", "32"}, {"width", "40"}, {"name", "fullscreenbtn"}, {"margin", "0,2,0,2"}, {"tooltip_text", "Fullscreen, press ESC to exit fullscreen"}});
    ui::Attach(pCaption, pFullscreenBtn);
    auto* pMinBtn = ui::Create<ui::Button>(this, {{"class", "btn_wnd_min_11"}, {"height", "32"}, {"width", "40"}, {"name", "minbtn"}, {"margin", "0,2,0,2"}, {"tooltip_text", "Minimize"}});
    ui::Attach(pCaption, pMinBtn);
    auto* pMaxBox = ui::Create<ui::Box>(this, {{"height", "stretch"}, {"width", "40"}, {"margin", "0,2,0,2"}});
    ui::Attach(pCaption, pMaxBox);
    auto* pMaxBtn = ui::Create<ui::Button>(this, {{"class", "btn_wnd_max_11"}, {"height", "32"}, {"width", "stretch"}, {"name", "maxbtn"}, {"tooltip_text", "Maximize"}});
    ui::Attach(pMaxBox, pMaxBtn);
    auto* pRestoreBtn = ui::Create<ui::Button>(this, {{"class", "btn_wnd_restore_11"}, {"height", "32"}, {"width", "stretch"}, {"name", "restorebtn"}, {"visible", "false"}, {"tooltip_text", "Restore"}});
    ui::Attach(pMaxBox, pRestoreBtn);
    auto* pCloseBtn = ui::Create<ui::Button>(this, {{"class", "btn_wnd_close_11"}, {"height", "stretch"}, {"width", "40"}, {"name", "closebtn"}, {"margin", "0,0,0,2"}, {"tooltip_text", "Close"}});
    ui::Attach(pCaption, pCloseBtn);

    // ---- Work area (VBox) ----
    auto* pWorkArea = ui::Create<ui::VBox>(this, {});
    ui::Attach(pRoot, pWorkArea);

    // -- Row: top controls (type combo + multi-select) --
    auto* pTopRow = ui::Create<ui::HBox>(this, {{"height", "auto"}});
    ui::Attach(pWorkArea, pTopRow);
    auto* pTypeLabel = ui::Create<ui::Label>(this, {{"text", "Table Type:"}, {"valign", "center"}, {"margin", "2,0,2,0"}});
    ui::Attach(pTopRow, pTypeLabel);
    auto* pTypeCombo = ui::Create<ui::Combo>(this, {{"class", "combo"}, {"name", "list_ctrl_type_combo"}, {"combo_type", "drop_list"}, {"dropbox_size", "0,300"}, {"shadow_type", "system_round"}, {"height", "26"}, {"width", "80"}, {"margin", "0,0,0,1"}, {"valign", "center"}});
    ui::Attach(pTopRow, pTypeCombo);
    auto* pTypeReportNode = ui::Create<ui::TreeNode>(this, {{"class", "tree_node"}, {"padding", "4"}, {"text", "Report"}, {"user_dataid", "0"}});
    pTypeCombo->GetTreeView()->GetRootNode()->AddChildNode(pTypeReportNode);
    auto* pTypeIconNode = ui::Create<ui::TreeNode>(this, {{"class", "tree_node"}, {"padding", "4"}, {"text", "Icon"}, {"user_dataid", "1"}});
    pTypeCombo->GetTreeView()->GetRootNode()->AddChildNode(pTypeIconNode);
    auto* pTypeListNode = ui::Create<ui::TreeNode>(this, {{"class", "tree_node"}, {"padding", "4"}, {"text", "List"}, {"user_dataid", "2"}});
    pTypeCombo->GetTreeView()->GetRootNode()->AddChildNode(pTypeListNode);
    auto* pMultiSelect = ui::Create<ui::CheckBox>(this, {{"class", "checkbox_1"}, {"name", "checkbox_multi_select"}, {"text", "Multi-select"}, {"margin", "4,0,0,0"}, {"valign", "center"}});
    ui::Attach(pTopRow, pMultiSelect);

    // -- Row: main content area (Report Group + Other Tests) --
    auto* pMainRow = ui::Create<ui::HBox>(this, {{"height", "auto"}});
    ui::Attach(pWorkArea, pMainRow);

    // ==== Report Group (left panel) ====
    auto* pReportGroup = ui::Create<ui::GroupVBox>(this, {{"name", "report_group"}, {"height", "auto"}, {"width", "770"}, {"text", "Report Type"}});
    ui::Attach(pMainRow, pReportGroup);

    // Row 1: Header Controls
    auto* pRow1 = ui::Create<ui::HBox>(this, {{"minheight", "18"}, {"bkcolor", "bk_wnd_darkcolor"}, {"height", "auto"}, {"margin", "4,18,4,0"}});
    ui::Attach(pReportGroup, pRow1);
    auto* pHeaderCtrlLabel = ui::Create<ui::Label>(this, {{"text", "Header Controls:"}, {"valign", "center"}, {"margin", "2,0,2,0"}});
    ui::Attach(pRow1, pHeaderCtrlLabel);
    auto* pHeaderHide = ui::Create<ui::Option>(this, {{"class", "option_2"}, {"group", "show"}, {"selected", "false"}, {"width", "64"}, {"height", "32"}, {"text", "Hide"}, {"padding", "2,2,2,2"}, {"borderround", "2,2"}, {"valign", "center"}});
    ui::Attach(pRow1, pHeaderHide);
    auto* pHeaderShow = ui::Create<ui::Option>(this, {{"class", "option_2"}, {"group", "show"}, {"selected", "true"}, {"width", "64"}, {"height", "32"}, {"text", "Show"}, {"padding", "2,2,2,2"}, {"borderround", "2,2"}, {"valign", "center"}});
    ui::Attach(pRow1, pHeaderShow);
    auto* pLine1a = ui::Create<ui::Line>(this, {{"vertical", "true"}, {"margin", "0,8,4,8"}, {"width", "2"}});
    ui::Attach(pRow1, pLine1a);
    auto* pDragLabel = ui::Create<ui::Label>(this, {{"text", "Drag Header to Reorder:"}, {"valign", "center"}, {"margin", "2,0,2,0"}});
    ui::Attach(pRow1, pDragLabel);
    auto* pDragForbidden = ui::Create<ui::Option>(this, {{"class", "option_2"}, {"group", "drag_order"}, {"selected", "false"}, {"width", "64"}, {"height", "32"}, {"text", "Forbidden"}, {"padding", "2,2,2,2"}, {"borderround", "2,2"}, {"valign", "center"}});
    ui::Attach(pRow1, pDragForbidden);
    auto* pDragAllowed = ui::Create<ui::Option>(this, {{"class", "option_2"}, {"group", "drag_order"}, {"selected", "true"}, {"width", "64"}, {"height", "32"}, {"text", "Allowed"}, {"padding", "2,2,2,2"}, {"borderround", "2,2"}, {"valign", "center"}});
    ui::Attach(pRow1, pDragAllowed);
    auto* pLine1b = ui::Create<ui::Line>(this, {{"vertical", "true"}, {"margin", "0,8,4,8"}, {"width", "2"}});
    ui::Attach(pRow1, pLine1b);
    auto* pHeaderHeightLabel = ui::Create<ui::Label>(this, {{"text", "Header Height:"}, {"valign", "center"}, {"margin", "2,0,2,0"}});
    ui::Attach(pRow1, pHeaderHeightLabel);
    auto* pHeaderHeightEdit = ui::Create<ui::RichEdit>(this, {{"class", "simple simple_border rich_edit_spin"}, {"name", "header_height_edit"}, {"min_number", "0"}, {"max_number", "512"}, {"limit_text", "3"}, {"text", "0"}, {"margin", "0,2,0,0"}});
    ui::Attach(pRow1, pHeaderHeightEdit);
    auto* pStretchBtn = ui::Create<ui::Button>(this, {{"class", "btn_global_color_gray"}, {"name", "set_column_stretch"}, {"text", "Set Column Widths Proportionally"}, {"width", "auto"}, {"height", "30"}, {"border_round", "3,3"}, {"text_padding", "8,0,8,0"}, {"margin", "4,0,0,0"}});
    ui::Attach(pRow1, pStretchBtn);

    // Row 2: Column Controls
    auto* pRow2 = ui::Create<ui::HBox>(this, {{"minheight", "18"}, {"bkcolor", "bk_wnd_darkcolor"}, {"height", "auto"}, {"margin", "4,0,4,0"}});
    ui::Attach(pReportGroup, pRow2);
    auto* pColumnLabel = ui::Create<ui::Label>(this, {{"text", "Column Controls:"}, {"valign", "center"}, {"margin", "2,0,2,0"}});
    ui::Attach(pRow2, pColumnLabel);
    auto* pColumnCombo = ui::Create<ui::Combo>(this, {{"class", "combo"}, {"name", "column_combo"}, {"combo_type", "drop_list"}, {"dropbox_size", "0,300"}, {"shadow_type", "system_default"}, {"height", "26"}, {"width", "80"}, {"margin", "0,0,0,1"}, {"valign", "center"}});
    ui::Attach(pRow2, pColumnCombo);
    auto* pLine2 = ui::Create<ui::Line>(this, {{"vertical", "true"}, {"margin", "8,4,4,4"}, {"width", "2"}});
    ui::Attach(pRow2, pLine2);
    auto* pColShow = ui::Create<ui::CheckBox>(this, {{"class", "checkbox_1"}, {"name", "checkbox_column_show"}, {"text", "Show This Column"}, {"margin", "4,0,0,0"}, {"valign", "center"}});
    ui::Attach(pRow2, pColShow);
    auto* pColWidth = ui::Create<ui::CheckBox>(this, {{"class", "checkbox_1"}, {"name", "checkbox_column_width"}, {"text", "Resizable Width"}, {"margin", "4,0,0,0"}, {"valign", "center"}});
    ui::Attach(pRow2, pColWidth);
    auto* pColSort = ui::Create<ui::CheckBox>(this, {{"class", "checkbox_1"}, {"name", "checkbox_column_sort"}, {"text", "Sortable"}, {"margin", "4,0,0,0"}, {"valign", "center"}});
    ui::Attach(pRow2, pColSort);
    auto* pColIconTop = ui::Create<ui::CheckBox>(this, {{"class", "checkbox_1"}, {"name", "checkbox_column_icon_at_top"}, {"text", "Sort Icon on Top"}, {"margin", "4,0,0,0"}, {"valign", "center"}});
    ui::Attach(pRow2, pColIconTop);
    auto* pColDrag = ui::Create<ui::CheckBox>(this, {{"class", "checkbox_1"}, {"name", "checkbox_column_drag_order"}, {"text", "Drag to Reorder"}, {"margin", "4,0,0,0"}, {"valign", "center"}});
    ui::Attach(pRow2, pColDrag);
    auto* pColEditable = ui::Create<ui::CheckBox>(this, {{"class", "checkbox_1"}, {"name", "checkbox_column_editable"}, {"text", "Editable Text"}, {"margin", "4,0,0,0"}, {"valign", "center"}});
    ui::Attach(pRow2, pColEditable);

    // Row 3: Column CheckBox & Icon options
    auto* pRow3 = ui::Create<ui::HBox>(this, {{"minheight", "18"}, {"bkcolor", "bk_wnd_darkcolor"}, {"height", "auto"}, {"margin", "4,0,4,0"}});
    ui::Attach(pReportGroup, pRow3);
    auto* pRow3Spacer = ui::Create<ui::Control>(this, {{"width", "153"}});
    ui::Attach(pRow3, pRow3Spacer);
    auto* pLine3 = ui::Create<ui::Line>(this, {{"vertical", "true"}, {"margin", "8,2,4,2"}, {"width", "2"}});
    ui::Attach(pRow3, pLine3);
    auto* pColCbLabel = ui::Create<ui::Label>(this, {{"text", "Column CheckBox:"}, {"valign", "center"}, {"margin", "2,0,2,0"}, {"tooltip_text", "Each column header and cell can show a CheckBox"}});
    ui::Attach(pRow3, pColCbLabel);
    auto* pColHeaderCb = ui::Create<ui::CheckBox>(this, {{"class", "checkbox_1"}, {"name", "checkbox_column_show_header_checkbox"}, {"text", "Show in Header"}, {"margin", "4,0,0,0"}, {"valign", "center"}});
    ui::Attach(pRow3, pColHeaderCb);
    auto* pColShowCb = ui::Create<ui::CheckBox>(this, {{"class", "checkbox_1"}, {"name", "checkbox_column_show_checkbox"}, {"text", "Show in Each Column"}, {"margin", "4,0,0,0"}, {"valign", "center"}});
    ui::Attach(pRow3, pColShowCb);
    auto* pLine3b = ui::Create<ui::Line>(this, {{"vertical", "true"}, {"margin", "8,2,4,2"}, {"width", "2"}});
    ui::Attach(pRow3, pLine3b);
    auto* pColIconLabel = ui::Create<ui::Label>(this, {{"text", "Column Icons:"}, {"valign", "center"}, {"margin", "2,0,2,0"}, {"tooltip_text", "Each column header and cell can show an icon"}});
    ui::Attach(pRow3, pColIconLabel);
    auto* pColHeaderIcon = ui::Create<ui::CheckBox>(this, {{"class", "checkbox_1"}, {"name", "checkbox_column_show_header_icon"}, {"text", "Show in Header"}, {"margin", "4,0,0,0"}, {"valign", "center"}});
    ui::Attach(pRow3, pColHeaderIcon);
    auto* pColShowIcon = ui::Create<ui::CheckBox>(this, {{"class", "checkbox_1"}, {"name", "checkbox_column_show_icon"}, {"text", "Show in Each Column"}, {"margin", "4,0,0,0"}, {"valign", "center"}});
    ui::Attach(pRow3, pColShowIcon);

    // Row 4: Header Text alignment + Cell Text alignment
    auto* pRow4 = ui::Create<ui::HBox>(this, {{"minheight", "18"}, {"bkcolor", "bk_wnd_darkcolor"}, {"height", "auto"}, {"margin", "4,0,4,0"}});
    ui::Attach(pReportGroup, pRow4);
    auto* pRow4Spacer = ui::Create<ui::Control>(this, {{"width", "153"}});
    ui::Attach(pRow4, pRow4Spacer);
    auto* pLine4 = ui::Create<ui::Line>(this, {{"vertical", "true"}, {"margin", "8,8,4,8"}, {"width", "2"}});
    ui::Attach(pRow4, pLine4);
    auto* pHeaderTextLabel = ui::Create<ui::Label>(this, {{"text", "Header Text:"}, {"valign", "center"}, {"margin", "6,0,2,0"}});
    ui::Attach(pRow4, pHeaderTextLabel);
    auto* pHeaderTextLeft = ui::Create<ui::Option>(this, {{"class", "option_2"}, {"name", "header_text_align_left"}, {"group", "header_text_align"}, {"selected", "false"}, {"width", "64"}, {"height", "32"}, {"text", "Left"}, {"padding", "2,2,2,2"}, {"borderround", "2,2"}, {"valign", "center"}});
    ui::Attach(pRow4, pHeaderTextLeft);
    auto* pHeaderTextCenter = ui::Create<ui::Option>(this, {{"class", "option_2"}, {"name", "header_text_align_center"}, {"group", "header_text_align"}, {"selected", "true"}, {"width", "64"}, {"height", "32"}, {"text", "Center"}, {"padding", "2,2,2,2"}, {"borderround", "2,2"}, {"valign", "center"}});
    ui::Attach(pRow4, pHeaderTextCenter);
    auto* pHeaderTextRight = ui::Create<ui::Option>(this, {{"class", "option_2"}, {"name", "header_text_align_right"}, {"group", "header_text_align"}, {"selected", "false"}, {"width", "64"}, {"height", "32"}, {"text", "Right"}, {"padding", "2,2,2,2"}, {"borderround", "2,2"}, {"valign", "center"}});
    ui::Attach(pRow4, pHeaderTextRight);
    auto* pLine4b = ui::Create<ui::Line>(this, {{"vertical", "true"}, {"margin", "8,8,4,8"}, {"width", "2"}});
    ui::Attach(pRow4, pLine4b);
    auto* pCellTextLabel = ui::Create<ui::Label>(this, {{"text", "Cell Text:"}, {"valign", "center"}, {"margin", "6,0,2,0"}});
    ui::Attach(pRow4, pCellTextLabel);
    auto* pCellTextLeft = ui::Create<ui::Option>(this, {{"class", "option_2"}, {"name", "column_text_align_left"}, {"group", "column_text_align"}, {"selected", "true"}, {"width", "64"}, {"height", "32"}, {"text", "Left"}, {"padding", "2,2,2,2"}, {"borderround", "2,2"}, {"valign", "center"}});
    ui::Attach(pRow4, pCellTextLeft);
    auto* pCellTextCenter = ui::Create<ui::Option>(this, {{"class", "option_2"}, {"name", "column_text_align_center"}, {"group", "column_text_align"}, {"selected", "false"}, {"width", "64"}, {"height", "32"}, {"text", "Center"}, {"padding", "2,2,2,2"}, {"borderround", "2,2"}, {"valign", "center"}});
    ui::Attach(pRow4, pCellTextCenter);
    auto* pCellTextRight = ui::Create<ui::Option>(this, {{"class", "option_2"}, {"name", "column_text_align_right"}, {"group", "column_text_align"}, {"selected", "false"}, {"width", "64"}, {"height", "32"}, {"text", "Right"}, {"padding", "2,2,2,2"}, {"borderround", "2,2"}, {"valign", "center"}});
    ui::Attach(pRow4, pCellTextRight);

    // Row 5: Table Properties (grid lines + row height)
    auto* pRow5 = ui::Create<ui::HBox>(this, {{"minheight", "18"}, {"bkcolor", "bk_wnd_darkcolor"}, {"height", "auto"}, {"margin", "4,0,4,0"}});
    ui::Attach(pReportGroup, pRow5);
    auto* pTablePropLabel = ui::Create<ui::Label>(this, {{"text", "Table Properties:"}, {"valign", "center"}, {"margin", "2,0,2,0"}});
    ui::Attach(pRow5, pTablePropLabel);
    auto* pHorGridLabel = ui::Create<ui::Label>(this, {{"text", "Horizontal Grid:"}, {"valign", "center"}, {"margin", "2,0,2,0"}});
    ui::Attach(pRow5, pHorGridLabel);
    auto* pHorGridHide = ui::Create<ui::Option>(this, {{"class", "option_2"}, {"group", "grid_line_row"}, {"selected", "false"}, {"width", "64"}, {"height", "32"}, {"text", "Hide"}, {"padding", "2,2,2,2"}, {"borderround", "2,2"}, {"valign", "center"}});
    ui::Attach(pRow5, pHorGridHide);
    auto* pHorGridShow = ui::Create<ui::Option>(this, {{"class", "option_2"}, {"group", "grid_line_row"}, {"selected", "true"}, {"width", "64"}, {"height", "32"}, {"text", "Show"}, {"padding", "2,2,2,2"}, {"borderround", "2,2"}, {"valign", "center"}});
    ui::Attach(pRow5, pHorGridShow);
    auto* pLine5a = ui::Create<ui::Line>(this, {{"vertical", "true"}, {"margin", "0,8,4,8"}, {"width", "2"}});
    ui::Attach(pRow5, pLine5a);
    auto* pVerGridLabel = ui::Create<ui::Label>(this, {{"text", "Vertical Grid:"}, {"valign", "center"}, {"margin", "2,0,2,0"}});
    ui::Attach(pRow5, pVerGridLabel);
    auto* pVerGridHide = ui::Create<ui::Option>(this, {{"class", "option_2"}, {"group", "grid_line_column"}, {"selected", "false"}, {"width", "64"}, {"height", "32"}, {"text", "Hide"}, {"padding", "2,2,2,2"}, {"borderround", "2,2"}, {"valign", "center"}});
    ui::Attach(pRow5, pVerGridHide);
    auto* pVerGridShow = ui::Create<ui::Option>(this, {{"class", "option_2"}, {"group", "grid_line_column"}, {"selected", "true"}, {"width", "64"}, {"height", "32"}, {"text", "Show"}, {"padding", "2,2,2,2"}, {"borderround", "2,2"}, {"valign", "center"}});
    ui::Attach(pRow5, pVerGridShow);
    auto* pLine5b = ui::Create<ui::Line>(this, {{"vertical", "true"}, {"margin", "0,8,4,8"}, {"width", "2"}});
    ui::Attach(pRow5, pLine5b);
    auto* pRowHeightLabel = ui::Create<ui::Label>(this, {{"text", "Row Height:"}, {"valign", "center"}, {"margin", "2,0,2,0"}});
    ui::Attach(pRow5, pRowHeightLabel);
    auto* pRowHeightEdit = ui::Create<ui::RichEdit>(this, {{"class", "simple simple_border rich_edit_spin"}, {"name", "list_item_height_edit"}, {"min_number", "0"}, {"max_number", "512"}, {"limit_text", "3"}, {"text", "0"}, {"margin", "0,2,0,0"}});
    ui::Attach(pRow5, pRowHeightEdit);

    // Row 6: Row CheckBox & Icon options
    auto* pRow6 = ui::Create<ui::HBox>(this, {{"minheight", "18"}, {"bkcolor", "bk_wnd_darkcolor"}, {"height", "auto"}, {"margin", "4,0,4,8"}});
    ui::Attach(pReportGroup, pRow6);
    auto* pRow6Spacer = ui::Create<ui::Control>(this, {{"width", "153"}});
    ui::Attach(pRow6, pRow6Spacer);
    auto* pLine6a = ui::Create<ui::Line>(this, {{"vertical", "true"}, {"margin", "8,2,4,2"}, {"width", "2"}});
    ui::Attach(pRow6, pLine6a);
    auto* pRowCbLabel = ui::Create<ui::Label>(this, {{"text", "Row CheckBox:"}, {"valign", "center"}, {"margin", "2,0,2,0"}, {"tooltip_text", "Each row header and row start can show a CheckBox"}});
    ui::Attach(pRow6, pRowCbLabel);
    auto* pRowHeaderCb = ui::Create<ui::CheckBox>(this, {{"class", "checkbox_1"}, {"name", "checkbox_show_header_checkbox"}, {"text", "Show in Header"}, {"margin", "4,0,0,0"}, {"valign", "center"}});
    ui::Attach(pRow6, pRowHeaderCb);
    auto* pRowShowCb = ui::Create<ui::CheckBox>(this, {{"class", "checkbox_1"}, {"name", "checkbox_show_checkbox"}, {"text", "Show at Row Start"}, {"margin", "4,0,0,0"}, {"valign", "center"}});
    ui::Attach(pRow6, pRowShowCb);
    auto* pLine6b = ui::Create<ui::Line>(this, {{"vertical", "true"}, {"margin", "8,2,4,2"}, {"width", "2"}});
    ui::Attach(pRow6, pLine6b);
    auto* pRowIconLabel = ui::Create<ui::Label>(this, {{"text", "Row Icons:"}, {"valign", "center"}, {"margin", "2,0,2,0"}, {"tooltip_text", "Each row header and row start can show an icon"}});
    ui::Attach(pRow6, pRowIconLabel);
    auto* pRowShowIcon = ui::Create<ui::CheckBox>(this, {{"class", "checkbox_1"}, {"name", "checkbox_show_icon"}, {"text", "Show at Row Start"}, {"margin", "4,0,0,0"}, {"valign", "center"}});
    ui::Attach(pRow6, pRowShowIcon);

    // ==== Right panel (Other Tests) ====
    auto* pRightPanel = ui::Create<ui::HBox>(this, {});
    ui::Attach(pMainRow, pRightPanel);

    auto* pOtherTests1 = ui::Create<ui::GroupVBox>(this, {{"text", "Other Tests"}});
    ui::Attach(pRightPanel, pOtherTests1);
    auto* pLoadBtn1 = ui::Create<ui::Button>(this, {{"class", "btn_global_color_gray"}, {"name", "loading_progress_btn1"}, {"text", "Loading Function Test (Progress Bar 1)"}, {"width", "200"}, {"height", "30"}, {"border_round", "3,3"}, {"margin", "20,25,10,0"}});
    ui::Attach(pOtherTests1, pLoadBtn1);
    auto* pLoadBtn2 = ui::Create<ui::Button>(this, {{"class", "btn_global_color_gray"}, {"name", "loading_progress_btn2"}, {"text", "Loading Function Test (Progress Bar 2)"}, {"width", "200"}, {"height", "30"}, {"border_round", "3,3"}, {"margin", "20,4,10,0"}});
    ui::Attach(pOtherTests1, pLoadBtn2);
    auto* pLoadBtn3 = ui::Create<ui::Button>(this, {{"class", "btn_global_color_gray"}, {"name", "loading_btn1"}, {"text", "Loading Function Test 1"}, {"width", "200"}, {"height", "30"}, {"border_round", "3,3"}, {"margin", "20,4,10,0"}});
    ui::Attach(pOtherTests1, pLoadBtn3);
    auto* pLoadBtn4 = ui::Create<ui::Button>(this, {{"class", "btn_global_color_gray"}, {"name", "loading_btn2"}, {"text", "Loading Function Test 2"}, {"width", "200"}, {"height", "30"}, {"border_round", "3,3"}, {"margin", "20,4,10,0"}});
    ui::Attach(pOtherTests1, pLoadBtn4);

    auto* pOtherTests2 = ui::Create<ui::GroupVBox>(this, {{"text", "Other Tests"}});
    ui::Attach(pRightPanel, pOtherTests2);
    auto* pLoadBtn5 = ui::Create<ui::Button>(this, {{"class", "btn_global_color_gray"}, {"name", "loading_btn3"}, {"text", "Loading Function Test 3"}, {"width", "200"}, {"height", "30"}, {"border_round", "3,3"}, {"margin", "20,25,10,0"}});
    ui::Attach(pOtherTests2, pLoadBtn5);
    auto* pLoadBtn6 = ui::Create<ui::Button>(this, {{"class", "btn_global_color_gray"}, {"name", "loading_btn4"}, {"text", "Loading Function Test 4"}, {"width", "200"}, {"height", "30"}, {"border_round", "3,3"}, {"margin", "20,4,10,0"}});
    ui::Attach(pOtherTests2, pLoadBtn6);
    auto* pLoadBtn7 = ui::Create<ui::Button>(this, {{"class", "btn_global_color_gray"}, {"name", "loading_btn5"}, {"text", "Loading Function Test 5"}, {"width", "200"}, {"height", "30"}, {"border_round", "3,3"}, {"margin", "20,4,10,0"}});
    ui::Attach(pOtherTests2, pLoadBtn7);
    auto* pLoadBtn8 = ui::Create<ui::Button>(this, {{"class", "btn_global_color_gray"}, {"name", "loading_btn6"}, {"text", "Loading Function Test 6"}, {"width", "200"}, {"height", "30"}, {"border_round", "3,3"}, {"margin", "20,4,10,0"}});
    ui::Attach(pOtherTests2, pLoadBtn8);

    // ---- Splitter bar ----
    auto* pSplit = ui::Create<ui::Split>(this, {{"bkcolor", "splitline_level1"}, {"height", "2"}});
    ui::Attach(pWorkArea, pSplit);

    // ---- ListCtrl area ----
    auto* pListCtrlBox = ui::Create<ui::VBox>(this, {{"margin", "0,0,0,0"}, {"valign", "center"}, {"halign", "center"}});
    ui::Attach(pWorkArea, pListCtrlBox);
    auto* pListCtrl = ui::Create<ui::ListCtrl>(this, {
        {"name", "list_ctrl"}, {"bkcolor", "YellowGreen"},
        {"type", "report"}, {"show_header", "true"},
        {"header_class", "list_ctrl_header"},
        {"header_item_class", "list_ctrl_header_item"},
        {"header_split_box_class", "list_ctrl_header_split_box"},
        {"header_split_control_class", "list_ctrl_header_split_control"},
        {"header_height", "32"}, {"enable_header_drag_order", "true"},
        {"check_box_class", "list_ctrl_checkbox"},
        {"data_item_class", "list_ctrl_item"},
        {"data_sub_item_class", "list_ctrl_sub_item"},
        {"report_view_class", "list_ctrl_report_view"},
        {"data_item_height", "46"},
        {"row_grid_line_width", "1"}, {"row_grid_line_color", "lightgray"},
        {"column_grid_line_width", "1"}, {"column_grid_line_color", "lightgray"},
        {"multi_select", "true"}, {"auto_check_select", "false"},
        {"show_header_checkbox", "true"}, {"show_data_item_checkbox", "true"},
        {"icon_view_class", "list_ctrl_icon_view"},
        {"icon_view_item_class", "list_ctrl_icon_view_item"},
        {"icon_view_item_image_class", "list_ctrl_icon_view_item_image"},
        {"icon_view_item_label_class", "list_ctrl_icon_view_item_label"},
        {"list_view_class", "list_ctrl_list_view"},
        {"list_view_item_class", "list_ctrl_list_view_item"},
        {"list_view_item_image_class", "list_ctrl_list_view_item_image"},
        {"list_view_item_label_class", "list_ctrl_list_view_item_label"},
        {"enable_item_edit", "true"}, {"list_ctrl_richedit_class", "list_ctrl_richedit"},
        {"loading", "file='loading_progress1.xml' width='0' height='0' offset_x='-1' offset_y='-1' valign='center' halign='center' fade='255' animation_control='loading_animation' auto_stop='true'"}
    });
    ui::Attach(pListCtrlBox, pListCtrl);

    // Final: attach root to window
    ui::Attach(this, pRoot);
}

void MainForm::OnInitWindow()
{
    SetupWindow();
    BuildUI();

    ui::ListCtrl* pListCtrl = ui::Find<ui::ListCtrl>(this, "list_ctrl");
    ASSERT(pListCtrl != nullptr);
    if (pListCtrl == nullptr) {
        return;
    }
    ui::ImageListPtr pReportImageList = std::make_shared<ui::ImageList>();
    ui::ImageListPtr pIconImageList = std::make_shared<ui::ImageList>();
    ui::ImageListPtr pListImageList = std::make_shared<ui::ImageList>();
    pListCtrl->SetImageList(ui::ListCtrlType::Report, pReportImageList);
    pListCtrl->SetImageList(ui::ListCtrlType::Icon, pIconImageList);
    pListCtrl->SetImageList(ui::ListCtrlType::List, pListImageList);

    pReportImageList->SetImageSize(ui::UiSize(22, 22), Dpi(), true);
    pListImageList->SetImageSize(ui::UiSize(32, 32), Dpi(), true);
    pIconImageList->SetImageSize(ui::UiSize(64, 64), Dpi(), true);

    // Add image resources
    uint32_t imageId = pReportImageList->AddImageString("file='display-color.svg' width='22' height='22'", Dpi());
    pListImageList->AddImageString("file='display-color.svg' width='32' height='32' valign='center' halign='center'", Dpi());
    pIconImageList->AddImageString("file='display-color.svg' width='64' height='64' valign='center' halign='center'", Dpi());

    // Fill data
    InsertItemData(400, 9, (int32_t)imageId);

    BindEvents();
    BaseClass::OnInitWindow();
}

void MainForm::BindEvents()
{
    ui::ListCtrl* pListCtrl = ui::Find<ui::ListCtrl>(this, "list_ctrl");
    ASSERT(pListCtrl != nullptr);
    if (pListCtrl == nullptr) {
        return;
    }
    InitListCtrlEvents(pListCtrl);
    TestListCtrlEvents(pListCtrl);
    TestListCtrlLoading(pListCtrl);
}

void MainForm::OnInitLayout()
{
    // Test automatically resizing column widths proportionally
    /*ui::ListCtrl* pListCtrl = ui::Find<ui::ListCtrl>(this, "list_ctrl");
    ASSERT(pListCtrl != nullptr);
    if (pListCtrl == nullptr) {
        return;
    }
    std::vector<ui::UiFixedInt> columnWidthList;
    size_t nColumnCount = pListCtrl->GetColumnCount();
    for (size_t nColumnIndex = 0; nColumnIndex < nColumnCount; nColumnIndex++) {
        columnWidthList.push_back(ui::UiFixedInt::MakeStretch());
    }
    bool bRet = pListCtrl->SetColumnWidth(columnWidthList, true);
    UNUSED_VARIABLE(bRet);*/
}

void MainForm::InitListCtrlEvents(ui::ListCtrl* pListCtrl)
{
    // Table type
    ui::Combo* pTypeCombo = ui::Find<ui::Combo>(this, "list_ctrl_type_combo");
    if (pTypeCombo != nullptr) {
        pTypeCombo->SetCurSel((int32_t)pListCtrl->GetListCtrlType());
        pTypeCombo->AttachSelect([this, pListCtrl, pTypeCombo](const ui::EventArgs& args) {
            size_t nCurSel = args.wParam;
            size_t nType = pTypeCombo->GetItemData(nCurSel);
            if (nType == 0) {
                if (pListCtrl) {
                    pListCtrl->SetListCtrlType(ui::ListCtrlType::Report);
                }
            }
            else if (nType == 1) {
                if (pListCtrl) {
                    pListCtrl->SetListCtrlType(ui::ListCtrlType::Icon);
                }
            }
            else if (nType == 2) {
                if (pListCtrl) {
                    pListCtrl->SetListCtrlType(ui::ListCtrlType::List);
                }
            }
            return true;
            });
    }

    // Header height control
    ui::RichEdit* pHeaderHeightEdit = ui::Find<ui::RichEdit>(this, "header_height_edit");
    if (pHeaderHeightEdit != nullptr) {
        pHeaderHeightEdit->SetText(ui::StringUtil::Printf("%d", pListCtrl->GetHeaderHeight()));
        pHeaderHeightEdit->AttachTextChanged([this, pHeaderHeightEdit, pListCtrl](const ui::EventArgs&) {
            int32_t height = ui::StringUtil::StringToInt32(pHeaderHeightEdit->GetText());
            if (height >= 0) {
                pListCtrl->SetHeaderHeight(height, false);
            }
            return true;
            });
    }

    // Row height control
    ui::RichEdit* pItemHeightEdit = ui::Find<ui::RichEdit>(this, "list_item_height_edit");
    if (pItemHeightEdit != nullptr) {
        pItemHeightEdit->SetText(ui::StringUtil::Printf("%d", pListCtrl->GetDataItemHeight()));
        pItemHeightEdit->AttachTextChanged([this, pItemHeightEdit, pListCtrl](const ui::EventArgs&) {
            int32_t height = ui::StringUtil::StringToInt32(pItemHeightEdit->GetText());
            if (height >= 0) {
                pListCtrl->SetDataItemHeight(height, false);
            }
            return true;
            });
    }

    // Column controls
    ui::Combo* pColumnCombo = ui::Find<ui::Combo>(this, "column_combo");
    if (pColumnCombo != nullptr) {
        // Fill column data
        size_t nColumnCount = pListCtrl->GetColumnCount();
        for (size_t i = 0; i < nColumnCount; ++i) {
            ui::ListCtrlHeaderItem* pHeaderItem = pListCtrl->GetColumn(i);
            ASSERT(pHeaderItem != nullptr);
            if (pHeaderItem != nullptr) {
                size_t nItem = pColumnCombo->AddTextItem(pHeaderItem->GetText());
                ASSERT(nItem != ui::Box::InvalidIndex);
                // Keep the column ID
                pColumnCombo->SetItemData(nItem, pHeaderItem->GetColumnId());
            }
        }
        // Bind events
        pColumnCombo->AttachSelect([this, pColumnCombo](const ui::EventArgs& args) {
            size_t nCurSel = args.wParam;
            size_t nColumnId = pColumnCombo->GetItemData(nCurSel);
            OnColumnChanged(nColumnId);
            return true;
            });

        pColumnCombo->SetCurSel(0);
        OnColumnChanged(pColumnCombo->GetItemData(0));
    }

    ui::CheckBox* pColumnShow = ui::Find<ui::CheckBox>(this, "checkbox_column_show");
    ui::CheckBox* pColumnWidth = ui::Find<ui::CheckBox>(this, "checkbox_column_width");
    ui::CheckBox* pColumnSort = ui::Find<ui::CheckBox>(this, "checkbox_column_sort");
    ui::CheckBox* pColumnIcon = ui::Find<ui::CheckBox>(this, "checkbox_column_icon_at_top");
    ui::CheckBox* pColumnDragOrder = ui::Find<ui::CheckBox>(this, "checkbox_column_drag_order");
    ui::CheckBox* pColumnEditable = ui::Find<ui::CheckBox>(this, "checkbox_column_editable");
    ui::CheckBox* pColumnHeaderCheckBox = ui::Find<ui::CheckBox>(this, "checkbox_column_show_header_checkbox");
    ui::CheckBox* pColumnShowCheckBox = ui::Find<ui::CheckBox>(this, "checkbox_column_show_checkbox");

    ui::CheckBox* pColumnHeaderIcon = ui::Find<ui::CheckBox>(this, "checkbox_column_show_header_icon");
    ui::CheckBox* pColumnShowIcon = ui::Find<ui::CheckBox>(this, "checkbox_column_show_icon");

    ui::Option* pColumnHeaderTextAlignLeft = ui::Find<ui::Option>(this, "header_text_align_left");
    ui::Option* pColumnHeaderTextAlignCenter = ui::Find<ui::Option>(this, "header_text_align_center");
    ui::Option* pColumnHeaderTextAlignRight = ui::Find<ui::Option>(this, "header_text_align_right");

    ui::Option* pColumnTextAlignLeft = ui::Find<ui::Option>(this, "column_text_align_left");
    ui::Option* pColumnTextAlignCenter = ui::Find<ui::Option>(this, "column_text_align_center");
    ui::Option* pColumnTextAlignRight = ui::Find<ui::Option>(this, "column_text_align_right");

    ui::CheckBox* pHeaderCheckBox = ui::Find<ui::CheckBox>(this, "checkbox_show_header_checkbox");
    ui::CheckBox* pShowCheckBox = ui::Find<ui::CheckBox>(this, "checkbox_show_checkbox");

    ui::CheckBox* pShowIcon = ui::Find<ui::CheckBox>(this, "checkbox_show_icon");

    // Implement showing this column
    auto OnColumnShowHide = [this, pColumnCombo, pListCtrl](bool bColumnVisible) {
        size_t nColumnId = pColumnCombo->GetItemData(pColumnCombo->GetCurSel());
        ui::ListCtrlHeaderItem* pHeaderItem = pListCtrl->GetColumnById(nColumnId);
        ASSERT(pHeaderItem != nullptr);
        if (pHeaderItem != nullptr) {
            pHeaderItem->SetColumnVisible(bColumnVisible);
        }
        };
    pColumnShow->AttachSelect([this, OnColumnShowHide](const ui::EventArgs&) {
        OnColumnShowHide(true);
        return true;
        });
    pColumnShow->AttachUnSelect([this, OnColumnShowHide](const ui::EventArgs&) {
        OnColumnShowHide(false);
        return true;
        });

    // Whether column width adjustment is supported
    auto OnColumnResizeable = [this, pColumnCombo, pListCtrl](bool bResizeable) {
        size_t nColumnId = pColumnCombo->GetItemData(pColumnCombo->GetCurSel());
        ui::ListCtrlHeaderItem* pHeaderItem = pListCtrl->GetColumnById(nColumnId);
        ASSERT(pHeaderItem != nullptr);
        if (pHeaderItem != nullptr) {
            pHeaderItem->SetColumnResizeable(bResizeable);
        }
        };
    pColumnWidth->AttachSelect([this, OnColumnResizeable](const ui::EventArgs&) {
        OnColumnResizeable(true);
        return true;
        });
    pColumnWidth->AttachUnSelect([this, OnColumnResizeable](const ui::EventArgs& args) {
        OnColumnResizeable(false);
        return true;
        });

    // Whether sorting is supported
    auto OnColumnSort = [this, pColumnCombo, pListCtrl](bool bSort) {
        size_t nColumnId = pColumnCombo->GetItemData(pColumnCombo->GetCurSel());
        ui::ListCtrlHeaderItem* pHeaderItem = pListCtrl->GetColumnById(nColumnId);
        ASSERT(pHeaderItem != nullptr);
        if (pHeaderItem != nullptr) {
            if (bSort) {
                pHeaderItem->SetSortMode(ui::ListCtrlHeaderItem::SortMode::kUp, true);
            }
            else {
                pHeaderItem->SetSortMode(ui::ListCtrlHeaderItem::SortMode::kNone);
            }
        }
        };
    pColumnSort->AttachSelect([this, OnColumnSort](const ui::EventArgs&) {
        OnColumnSort(true);
        return true;
        });
    pColumnSort->AttachUnSelect([this, OnColumnSort](const ui::EventArgs& args) {
        OnColumnSort(false);
        return true;
        });

    // Whether the icon is displayed at the top
    auto OnColumnShowIconOnTop = [this, pColumnCombo, pListCtrl](bool bShowIconAtTop) {
        size_t nColumnId = pColumnCombo->GetItemData(pColumnCombo->GetCurSel());
        ui::ListCtrlHeaderItem* pHeaderItem = pListCtrl->GetColumnById(nColumnId);
        ASSERT(pHeaderItem != nullptr);
        if (pHeaderItem != nullptr) {
            pHeaderItem->SetShowIconAtTop(bShowIconAtTop);
        }
        };
    pColumnIcon->AttachSelect([this, OnColumnShowIconOnTop](const ui::EventArgs&) {
        OnColumnShowIconOnTop(true);
        return true;
        });
    pColumnIcon->AttachUnSelect([this, OnColumnShowIconOnTop](const ui::EventArgs& args) {
        OnColumnShowIconOnTop(false);
        return true;
        });


    // Whether the order can be adjusted by dragging
    auto OnColumnDragOrder = [this, pColumnCombo, pListCtrl](bool bEnableDragOrder) {
        size_t nColumnId = pColumnCombo->GetItemData(pColumnCombo->GetCurSel());
        ui::ListCtrlHeaderItem* pHeaderItem = pListCtrl->GetColumnById(nColumnId);
        ASSERT(pHeaderItem != nullptr);
        if (pHeaderItem != nullptr) {
            pHeaderItem->SetEnableDragOrder(bEnableDragOrder);
        }
        };
    pColumnDragOrder->AttachSelect([this, OnColumnDragOrder](const ui::EventArgs&) {
        OnColumnDragOrder(true);
        return true;
        });
    pColumnDragOrder->AttachUnSelect([this, OnColumnDragOrder](const ui::EventArgs& args) {
        OnColumnDragOrder(false);
        return true;
        });

    // Whether editable
    auto OnColumnEditable = [this, pColumnCombo, pListCtrl](bool bEditable) {
        size_t nColumnId = pColumnCombo->GetItemData(pColumnCombo->GetCurSel());
        size_t nColumnIndex = pListCtrl->GetColumnIndex(nColumnId);
        size_t nItemCount = pListCtrl->GetDataItemCount();
        for (size_t nItemIndex = 0; nItemIndex < nItemCount; ++nItemIndex) {
            pListCtrl->SetSubItemEditable(nItemIndex, nColumnIndex, bEditable);
        }
        };
    pColumnEditable->AttachSelect([this, OnColumnEditable](const ui::EventArgs&) {
        OnColumnEditable(true);
        return true;
        });
    pColumnEditable->AttachUnSelect([this, OnColumnEditable](const ui::EventArgs& args) {
        OnColumnEditable(false);
        return true;
        });

    // Column-level CheckBox: whether the header displays a CheckBox
    auto OnSetCheckBoxVisible = [this, pColumnCombo, pListCtrl](bool bCheckBoxVisible) {
        size_t nColumnId = pColumnCombo->GetItemData(pColumnCombo->GetCurSel());
        ui::ListCtrlHeaderItem* pHeaderItem = pListCtrl->GetColumnById(nColumnId);
        ASSERT(pHeaderItem != nullptr);
        if (pHeaderItem != nullptr) {
            pHeaderItem->SetShowCheckBox(bCheckBoxVisible);
        }
        };
    pColumnHeaderCheckBox->AttachSelect([this, OnSetCheckBoxVisible](const ui::EventArgs&) {
        OnSetCheckBoxVisible(true);
        return true;
        });
    pColumnHeaderCheckBox->AttachUnSelect([this, OnSetCheckBoxVisible](const ui::EventArgs& args) {
        OnSetCheckBoxVisible(false);
        return true;
        });

    // Column-level CheckBox: show a CheckBox in each column
    auto OnShowCheckBox = [this, pColumnCombo, pListCtrl](bool bShowCheckBox) {
        size_t nColumnId = pColumnCombo->GetItemData(pColumnCombo->GetCurSel());
        size_t nColumnIndex = pListCtrl->GetColumnIndex(nColumnId);
        size_t nItemCount = pListCtrl->GetDataItemCount();
        for (size_t nItemIndex = 0; nItemIndex < nItemCount; ++nItemIndex) {
            pListCtrl->SetSubItemShowCheckBox(nItemIndex, nColumnIndex, bShowCheckBox);
        }
        };
    if (pColumnShowCheckBox != nullptr) {
        bool bCheckBoxVisible = false;
        if (pListCtrl->GetColumnCount() > 0) {
            ui::ListCtrlHeaderItem* pHeaderItem = pListCtrl->GetColumn(0);
            ASSERT(pHeaderItem != nullptr);
            if (pHeaderItem != nullptr) {
                bCheckBoxVisible = pHeaderItem->IsShowCheckBox();
            }
        }
        pColumnShowCheckBox->Selected(bCheckBoxVisible, false);
        pColumnShowCheckBox->AttachSelect([this, OnShowCheckBox](const ui::EventArgs&) {
            OnShowCheckBox(true);
            return true;
            });
        pColumnShowCheckBox->AttachUnSelect([this, OnShowCheckBox](const ui::EventArgs&) {
            OnShowCheckBox(false);
            return true;
            });
    }

    if (pColumnHeaderIcon != nullptr) {
        auto OnSetHeaderShowIcon = [this, pColumnCombo, pListCtrl](bool bShow) {
            size_t nColumnId = pColumnCombo->GetItemData(pColumnCombo->GetCurSel());
            ui::ListCtrlHeaderItem* pHeaderItem = pListCtrl->GetColumnById(nColumnId);
            ASSERT(pHeaderItem != nullptr);
            if (pHeaderItem != nullptr) {
                pHeaderItem->SetImageId(bShow ? 0 : -1);
            }
            };
        pColumnHeaderIcon->AttachSelect([this, OnSetHeaderShowIcon](const ui::EventArgs&) {
            OnSetHeaderShowIcon(true);
            return true;
            });
        pColumnHeaderIcon->AttachUnSelect([this, OnSetHeaderShowIcon](const ui::EventArgs&) {
            OnSetHeaderShowIcon(false);
            return true;
            });
    }
    if (pColumnShowIcon != nullptr) {
        auto OnShowColumnIcon = [this, pColumnCombo, pListCtrl](bool bShow) {
            size_t nColumnId = pColumnCombo->GetItemData(pColumnCombo->GetCurSel());
            size_t nColumnIndex = pListCtrl->GetColumnIndex(nColumnId);
            size_t nItemCount = pListCtrl->GetDataItemCount();
            for (size_t nItemIndex = 0; nItemIndex < nItemCount; ++nItemIndex) {
                pListCtrl->SetSubItemImageId(nItemIndex, nColumnIndex, bShow ? 0 : -1);
            }
            };
        pColumnShowIcon->AttachSelect([this, OnShowColumnIcon](const ui::EventArgs&) {
            OnShowColumnIcon(true);
            return true;
            });
        pColumnShowIcon->AttachUnSelect([this, OnShowColumnIcon](const ui::EventArgs&) {
            OnShowColumnIcon(false);
            return true;
            });
    }

    auto OnHeaderTextAlign = [this, pColumnCombo, pListCtrl](ui::HorAlignType alignType) {
        size_t nColumnId = pColumnCombo->GetItemData(pColumnCombo->GetCurSel());
        ui::ListCtrlHeaderItem* pHeaderItem = pListCtrl->GetColumnById(nColumnId);
        ASSERT(pHeaderItem != nullptr);
        if (pHeaderItem != nullptr) {
            pHeaderItem->SetTextHorAlign(alignType);
        }
        };
    pColumnHeaderTextAlignLeft->AttachSelect([this, OnHeaderTextAlign](const ui::EventArgs&) {
        OnHeaderTextAlign(ui::HorAlignType::kAlignLeft);
        return true;
        });
    pColumnHeaderTextAlignCenter->AttachSelect([this, OnHeaderTextAlign](const ui::EventArgs&) {
        OnHeaderTextAlign(ui::HorAlignType::kAlignCenter);
        return true;
        });
    pColumnHeaderTextAlignRight->AttachSelect([this, OnHeaderTextAlign](const ui::EventArgs&) {
        OnHeaderTextAlign(ui::HorAlignType::kAlignRight);
        return true;
        });

    auto OnColumnTextAlign = [this, pColumnCombo, pListCtrl](int32_t nTextFormat) {
        size_t nColumnId = pColumnCombo->GetItemData(pColumnCombo->GetCurSel());
        size_t nColumnIndex = pListCtrl->GetColumnIndex(nColumnId);
        if (nColumnIndex != ui::Box::InvalidIndex) {
            size_t nCount = pListCtrl->GetDataItemCount();
            for (size_t index = 0; index < nCount; ++index) {
                int32_t nNewTextFormat = pListCtrl->GetSubItemTextFormat(index, nColumnIndex);
                if (nTextFormat & ui::TEXT_HCENTER) {
                    nNewTextFormat &= ~(ui::TEXT_HALIGN_ALL);
                    nNewTextFormat |= ui::TEXT_HCENTER;
                }
                else if (nTextFormat & ui::TEXT_RIGHT) {
                    nNewTextFormat &= ~(ui::TEXT_HALIGN_ALL);
                    nNewTextFormat |= ui::TEXT_RIGHT;
                }
                else {
                    nNewTextFormat &= ~(ui::TEXT_HALIGN_ALL);
                    nNewTextFormat |= ui::TEXT_LEFT;
                }
                pListCtrl->SetSubItemTextFormat(index, nColumnIndex, nNewTextFormat);
            }
        }
        };
    pColumnTextAlignLeft->AttachSelect([this, OnColumnTextAlign](const ui::EventArgs&) {
        OnColumnTextAlign(ui::TEXT_LEFT);
        return true;
        });
    pColumnTextAlignCenter->AttachSelect([this, OnColumnTextAlign](const ui::EventArgs&) {
        OnColumnTextAlign(ui::TEXT_HCENTER);
        return true;
        });
    pColumnTextAlignRight->AttachSelect([this, OnColumnTextAlign](const ui::EventArgs&) {
        OnColumnTextAlign(ui::TEXT_RIGHT);
        return true;
        });

    // Whether multi-selection is supported
    ui::CheckBox* pMultiSelect = ui::Find<ui::CheckBox>(this, "checkbox_multi_select");
    if (pMultiSelect != nullptr) {
        pMultiSelect->Selected(pListCtrl->IsMultiSelect(), false);
    }
    // Right-click on the list header
    ui::ListCtrlHeader* pHeaderCtrl = pListCtrl->GetHeaderCtrl();
    if (pHeaderCtrl != nullptr) {
        pHeaderCtrl->AttachRClick([this](const ui::EventArgs&) {
#if defined (DUI_BUILD_FOR_WIN)
            // Wide literals: MessageBox is MessageBoxW under UNICODE, which this block is.
            if (::MessageBoxW(nullptr, L"ListCtrlHeader RClick! Run function test?", L"", MB_YESNO) == IDYES) {
                RunListCtrlTest();
            }
#else
            ui::SystemUtil::ShowMessageBox(this, "Start Function Test", "ListCtrlHeader RClick!");
            RunListCtrlTest();
#endif
            return true;
            });
    }

    // Set each column's width proportionally to fill the entire view
    ui::Button* pAutoStretchBtn = ui::Find<ui::Button>(this, "set_column_stretch");
    if (pAutoStretchBtn != nullptr) {
        pAutoStretchBtn->AttachClick([pListCtrl, this](const ui::EventArgs& /*args*/) {
            std::vector<ui::UiFixedInt> columnWidthList;
            size_t nColumnCount = pListCtrl->GetColumnCount();
            for (size_t nColumnIndex = 0; nColumnIndex < nColumnCount; nColumnIndex++) {
                columnWidthList.push_back(ui::UiFixedInt::MakeStretch());
            }
            bool bRet = pListCtrl->SetColumnWidth(columnWidthList, true);
            UNUSED_VARIABLE(bRet);
            return true;
            });
    }

    // Control whether a CheckBox is shown in the header or at the start of rows
    if ((pHeaderCtrl != nullptr) && pHeaderCtrl->IsVisible() && pHeaderCtrl->IsShowCheckBox()) {
        pHeaderCheckBox->Selected(true, false);
    }
    else {
        pHeaderCheckBox->Selected(false, false);
    }
    pHeaderCheckBox->AttachSelect([this, pListCtrl](const ui::EventArgs&) {
        pListCtrl->SetHeaderShowCheckBox(true);
        return true;
        });
    pHeaderCheckBox->AttachUnSelect([this, pListCtrl](const ui::EventArgs&) {
        pListCtrl->SetHeaderShowCheckBox(false);
        return true;
        });

    pShowCheckBox->Selected(pListCtrl->IsDataItemShowCheckBox());
    pShowCheckBox->AttachSelect([this, pListCtrl](const ui::EventArgs&) {
        pListCtrl->SetDataItemShowCheckBox(true);
        return true;
        });
    pShowCheckBox->AttachUnSelect([this, pListCtrl](const ui::EventArgs&) {
        pListCtrl->SetDataItemShowCheckBox(false);
        return true;
        });

    if (pShowIcon != nullptr) {
        if (pListCtrl->GetDataItemCount() > 0) {
            pShowIcon->Selected(pListCtrl->GetDataItemImageId(0) >= 0, false);
        }
        auto OnShowDataItemIcon = [this, pListCtrl](bool bShow) {
            size_t nItemCount = pListCtrl->GetDataItemCount();
            pListCtrl->SetEnableRefresh(false);
            for (size_t nItemIndex = 0; nItemIndex < nItemCount; ++nItemIndex) {
                pListCtrl->SetDataItemImageId(nItemIndex, bShow ? 0 : -1);
            }
            pListCtrl->SetEnableRefresh(true);
            pListCtrl->Refresh();
            };
        pShowIcon->AttachSelect([this, OnShowDataItemIcon](const ui::EventArgs&) {
            OnShowDataItemIcon(true);
            return true;
            });
        pShowIcon->AttachUnSelect([this, OnShowDataItemIcon](const ui::EventArgs&) {
            OnShowDataItemIcon(false);
            return true;
            });
    }
}

void MainForm::TestListCtrlLoading(ui::ListCtrl* pListCtrl)
{
    // Test the loading feature
    ui::Button* pLoadingBtn = ui::Find<ui::Button>(this, "loading_progress_btn1");
    if (pLoadingBtn != nullptr) {
        pLoadingBtn->AttachClick([pListCtrl, this](const ui::EventArgs& args) {
            if (!pListCtrl->IsLoading()) {
                pListCtrl->SetLoadingAttribute("file='loading_progress1.xml' width='0' height='0' offset_x='-1' offset_y='-1' valign='center' halign='center' fade='255' animation_control='loading_animation' auto_stop='true'");
                OnTestLoadingProgress();
            }
            return true;
            });
    }

    pLoadingBtn = ui::Find<ui::Button>(this, "loading_progress_btn2");
    if (pLoadingBtn != nullptr) {
        pLoadingBtn->AttachClick([pListCtrl, this](const ui::EventArgs& args) {
            if (!pListCtrl->IsLoading()) {
                pListCtrl->SetLoadingAttribute("file='loading_progress2.xml' width='0' height='0' offset_x='-1' offset_y='-1' valign='center' halign='center' fade='255' animation_control='loading_animation' auto_stop='true'");
                OnTestLoadingProgress();
            }
            return true;
            });
    }

    pLoadingBtn = ui::Find<ui::Button>(this, "loading_btn1");
    if (pLoadingBtn != nullptr) {
        pLoadingBtn->AttachClick([pListCtrl, this](const ui::EventArgs& args) {
            if (!pListCtrl->IsLoading()) {
                pListCtrl->SetLoadingAttribute("file='loading1.xml' width='0' height='0' offset_x='-1' offset_y='-1' valign='center' halign='center' fade='255' animation_control='loading_animation' auto_stop='true'");
                // In real applications, event handling can refer to the logic of OnTestLoadingProgress
                pListCtrl->StartLoading(100, -1);
            }
            return true;
            });
    }
    pLoadingBtn = ui::Find<ui::Button>(this, "loading_btn2");
    if (pLoadingBtn != nullptr) {
        pLoadingBtn->AttachClick([pListCtrl, this](const ui::EventArgs& args) {
            if (!pListCtrl->IsLoading()) {
                pListCtrl->SetLoadingAttribute("file='loading2.xml' width='0' height='0' offset_x='-1' offset_y='-1' valign='center' halign='center' fade='255' animation_control='loading_animation' auto_stop='true'");
                // In real applications, event handling can refer to the logic of OnTestLoadingProgress
                pListCtrl->StartLoading(100, -1);
            }
            return true;
            });
    }
    pLoadingBtn = ui::Find<ui::Button>(this, "loading_btn3");
    if (pLoadingBtn != nullptr) {
        pLoadingBtn->AttachClick([pListCtrl, this](const ui::EventArgs& args) {
            if (!pListCtrl->IsLoading()) {
                pListCtrl->SetLoadingAttribute("file='loading3.xml' width='0' height='0' offset_x='-1' offset_y='-1' valign='center' halign='center' fade='255' animation_control='loading_animation' auto_stop='true'");
                // In real applications, event handling can refer to the logic of OnTestLoadingProgress
                pListCtrl->StartLoading(100, -1);
            }
            return true;
            });
    }
    pLoadingBtn = ui::Find<ui::Button>(this, "loading_btn4");
    if (pLoadingBtn != nullptr) {
        pLoadingBtn->AttachClick([pListCtrl, this](const ui::EventArgs& args) {
            if (!pListCtrl->IsLoading()) {
                pListCtrl->SetLoadingAttribute("file='loading4.xml' width='0' height='0' offset_x='-1' offset_y='-1' valign='center' halign='center' fade='255' animation_control='loading_animation' auto_stop='true'");
                // In real applications, event handling can refer to the logic of OnTestLoadingProgress
                pListCtrl->StartLoading(100, -1);
            }
            return true;
            });
    }
    pLoadingBtn = ui::Find<ui::Button>(this, "loading_btn5");
    if (pLoadingBtn != nullptr) {
        pLoadingBtn->AttachClick([pListCtrl, this](const ui::EventArgs& args) {
            if (!pListCtrl->IsLoading()) {
                pListCtrl->SetLoadingAttribute("file='loading5.xml' width='0' height='0' offset_x='-1' offset_y='-1' valign='center' halign='center' fade='255' animation_control='loading_animation' auto_stop='true'");
                // In real applications, event handling can refer to the logic of OnTestLoadingProgress
                pListCtrl->StartLoading(100, -1);
            }
            return true;
            });
    }
    pLoadingBtn = ui::Find<ui::Button>(this, "loading_btn6");
    if (pLoadingBtn != nullptr) {
        pLoadingBtn->AttachClick([pListCtrl, this](const ui::EventArgs& args) {
            if (!pListCtrl->IsLoading()) {
                pListCtrl->SetLoadingAttribute("file='loading6.xml' width='0' height='0' offset_x='-1' offset_y='-1' valign='center' halign='center' fade='255' animation_control='loading_animation' auto_stop='true'");
                // In real applications, event handling can refer to the logic of OnTestLoadingProgress
                pListCtrl->StartLoading(100, -1);
            }
            return true;
            });
    }
}

void MainForm::OnTestLoadingProgress()
{
    ui::ListCtrl* pListCtrl = ui::Find<ui::ListCtrl>(this, "list_ctrl");
    ASSERT(pListCtrl != nullptr);
    if (pListCtrl == nullptr) {
        return;
    }
    if (pListCtrl->IsLoading()) {
        return;
    }

    m_fLoadingPercent = 0;// Current progress, simulated value
    pListCtrl->DetachEvent(ui::kEventLoadingStart); // Unbind callbacks registered elsewhere to avoid interference
    pListCtrl->AttachLoadingStart([](const ui::EventArgs& args) {
        ui::ControlLoadingStatus* pLoadingStatus = (ui::ControlLoadingStatus*)args.wParam;
        if (pLoadingStatus != nullptr) {
            // Interface to get the associated control; check for nullptr before use
            //ui::ControlPtrT<ui::Control> pControl = pLoadingStatus->m_pControl;
            //ui::ControlPtrT<ui::Box> pLoadingUiRootBox = pLoadingStatus->m_pLoadingUiRootBox;
            //ui::ControlPtrT<ui::Control> pAnimationControl = pLoadingStatus->m_pAnimationControl;

        }
        return true;
        });
    pListCtrl->DetachEvent(ui::kEventLoading); // Unbind callbacks registered elsewhere to avoid interference
    pListCtrl->AttachLoading([pListCtrl, this](const ui::EventArgs& args) {
        if (!pListCtrl->IsLoading()) {
            // The loading state has terminated; stop dispatching this event (this happens when multiple event callbacks are registered)
            return false;
        }
        bool bRet = true;
        ui::ControlLoadingStatus* pLoadingStatus = (ui::ControlLoadingStatus*)args.wParam;
        if (pLoadingStatus != nullptr) {
            // Interface to get the associated control; check for nullptr before use
            ui::ControlPtrT<ui::Control> pControl = pLoadingStatus->m_pControl;
            ui::ControlPtrT<ui::Box> pLoadingUiRootBox = pLoadingStatus->m_pLoadingUiRootBox;
            ui::ControlPtrT<ui::Control> pAnimationControl = pLoadingStatus->m_pAnimationControl;

            // Set progress
            bool bFinished = false;
            if (pAnimationControl != nullptr) {
                uint32_t nFrameCount = pAnimationControl->GetImageAnimationFrameCount();
                uint32_t nFrameIndex = (uint32_t)(m_fLoadingPercent * nFrameCount / 100);
                if (nFrameIndex >= nFrameCount) {
                    // Done
                    nFrameIndex = nFrameCount - 1;
                    bFinished = true;
                }
                pAnimationControl->SetImageAnimationFrame(nFrameIndex);
            }
            if (bFinished) {
                // Stop the loading state
                pLoadingStatus->m_bStopLoading = true;
            }
            else {
                m_fLoadingPercent += 0.35f;// Current progress, simulated value
            }
        }
        return bRet;
        });
    pListCtrl->DetachEvent(ui::kEventLoadingStop); // Unbind callbacks registered elsewhere to avoid interference
    pListCtrl->AttachLoadingStop([pListCtrl](const ui::EventArgs& args) {
        ui::ControlLoadingStatus* pLoadingStatus = (ui::ControlLoadingStatus*)args.wParam;
        if (pLoadingStatus != nullptr) {
            // Interface to get the associated control; check for nullptr before use
            //ui::ControlPtrT<ui::Control> pControl = pLoadingStatus->m_pControl;
            //ui::ControlPtrT<ui::Box> pLoadingUiRootBox = pLoadingStatus->m_pLoadingUiRootBox;
            //ui::ControlPtrT<ui::Control> pAnimationControl = pLoadingStatus->m_pAnimationControl;

        }
        // Unregister callbacks to avoid affecting other business logic
        pListCtrl->DetachEvent(ui::kEventLoadingStart);
        pListCtrl->DetachEvent(ui::kEventLoading);
        pListCtrl->DetachEvent(ui::kEventLoadingStop);
        return true;
        });

    pListCtrl->StartLoading(33, -1);
}

void MainForm::OnColumnChanged(size_t nColumnId)
{
    ui::ListCtrl* pListCtrl = ui::Find<ui::ListCtrl>(this, "list_ctrl");
    ASSERT(pListCtrl != nullptr);
    if (pListCtrl == nullptr) {
        return;
    }
    if (pListCtrl->GetDataItemCount() == 0) {
        // No data
        return;
    }

    ui::ListCtrlHeaderItem* pHeaderItem = pListCtrl->GetColumnById(nColumnId);
    ASSERT(pHeaderItem != nullptr);
    if (pHeaderItem == nullptr) {
        return;
    }


    ui::CheckBox* pColumnShow = ui::Find<ui::CheckBox>(this, "checkbox_column_show");
    ui::CheckBox* pColumnWidth = ui::Find<ui::CheckBox>(this, "checkbox_column_width");
    ui::CheckBox* pColumnSort = ui::Find<ui::CheckBox>(this, "checkbox_column_sort");
    ui::CheckBox* pColumnIcon = ui::Find<ui::CheckBox>(this, "checkbox_column_icon_at_top");
    ui::CheckBox* pColumnDragOrder = ui::Find<ui::CheckBox>(this, "checkbox_column_drag_order");
    ui::CheckBox* pColumnEditable = ui::Find<ui::CheckBox>(this, "checkbox_column_editable");
    ui::CheckBox* pColumnHeaderCheckBox = ui::Find<ui::CheckBox>(this, "checkbox_column_show_header_checkbox");
    ui::CheckBox* pColumnShowCheckBox = ui::Find<ui::CheckBox>(this, "checkbox_column_show_checkbox");

    ui::CheckBox* pColumnHeaderIcon = ui::Find<ui::CheckBox>(this, "checkbox_column_show_header_icon");
    ui::CheckBox* pColumnShowIcon = ui::Find<ui::CheckBox>(this, "checkbox_column_show_icon");

    ui::Option* pColumnHeaderTextAlignLeft = ui::Find<ui::Option>(this, "header_text_align_left");
    ui::Option* pColumnHeaderTextAlignCenter = ui::Find<ui::Option>(this, "header_text_align_center");
    ui::Option* pColumnHeaderTextAlignRight = ui::Find<ui::Option>(this, "header_text_align_right");

    ui::Option* pColumnTextAlignLeft = ui::Find<ui::Option>(this, "column_text_align_left");
    ui::Option* pColumnTextAlignCenter = ui::Find<ui::Option>(this, "column_text_align_center");
    ui::Option* pColumnTextAlignRight = ui::Find<ui::Option>(this, "column_text_align_right");

    ASSERT(pHeaderItem->IsColumnVisible() == pHeaderItem->IsVisible());
    pColumnShow->Selected(pHeaderItem->IsColumnVisible(), false);
    pColumnWidth->Selected(pHeaderItem->IsColumnResizeable(), false);

    ui::ListCtrlHeaderItem::SortMode sortMode = pHeaderItem->GetSortMode();
    pColumnSort->Selected(sortMode != ui::ListCtrlHeaderItem::SortMode::kNone, false);

    pColumnIcon->Selected(pHeaderItem->IsShowIconAtTop(), false);
    pColumnDragOrder->Selected(pHeaderItem->IsEnableDragOrder(), false);
    pColumnHeaderCheckBox->Selected(pHeaderItem->IsShowCheckBox(), false);
    pColumnHeaderIcon->SetSelected(pHeaderItem->GetImageId() >= 0);

    bool bColumnDataHasCheckBox = false;
    bool bColumnDataHasIcon = false;
    bool bColumnEditable = pListCtrl->IsSubItemEditable(0, 0);
    ui::ListCtrlItem* pItem = pListCtrl->GetFirstDisplayItem();
    if (pItem != nullptr) {
        ui::ListCtrlSubItem* pSubItem = pItem->GetSubItem(pListCtrl->GetColumnIndex(nColumnId));
        if (pSubItem != nullptr) {
            bColumnDataHasCheckBox = pSubItem->IsShowCheckBox();
            bColumnDataHasIcon = pSubItem->GetImageId() >= 0;
        }
    }
    else {
        bColumnDataHasCheckBox = pListCtrl->IsSubItemShowCheckBox(0, 0);
        bColumnDataHasIcon = pListCtrl->GetSubItemImageId(0, 0) >= 0;
    }
    pColumnShowCheckBox->Selected(bColumnDataHasCheckBox, false);
    pColumnShowIcon->Selected(bColumnDataHasIcon, false);
    pColumnEditable->Selected(bColumnEditable, false);

    ui::HorAlignType hAlignType = pHeaderItem->GetTextHorAlign();
    if (hAlignType == ui::HorAlignType::kAlignCenter) {
        pColumnHeaderTextAlignCenter->Selected(true, false);
    }
    else if (hAlignType == ui::HorAlignType::kAlignRight) {
        pColumnHeaderTextAlignRight->Selected(true, false);
    }
    else {
        pColumnHeaderTextAlignLeft->Selected(true, false);
    }

    int32_t nTextFormat = pListCtrl->GetSubItemTextFormat(0, pListCtrl->GetColumnIndex(nColumnId));
    if (nTextFormat & ui::TEXT_HCENTER) {
        pColumnTextAlignCenter->Selected(true, false);
    }
    else if (nTextFormat & ui::TEXT_RIGHT) {
        pColumnTextAlignRight->Selected(true, false);
    }
    else {
        pColumnTextAlignLeft->Selected(true, false);
    }
}

void MainForm::InsertItemData(int32_t nRows, int32_t nColumns, int32_t nImageId)
{
    ui::ListCtrl* pListCtrl = ui::Find<ui::ListCtrl>(this, "list_ctrl");
    ASSERT(pListCtrl != nullptr);
    if (pListCtrl == nullptr) {
        return;
    }
    const size_t columnCount = nColumns;
    const size_t rowCount = nRows;
    bool bShowCheckBox = true; // Whether to show a CheckBox
    // Add a column
    for (size_t i = 0; i < columnCount; ++i) {
        ui::ListCtrlColumn columnInfo;
        columnInfo.nColumnWidth = 200;
        //columnInfo.nTextFormat = TEXT_LEFT | TEXT_VCENTER;
        columnInfo.text = ui::StringUtil::Printf("Column %d", i);
        columnInfo.bShowCheckBox = bShowCheckBox;
        columnInfo.nImageId = nImageId;
        pListCtrl->InsertColumn(-1, columnInfo);
    }
    // Fill data
    pListCtrl->SetDataItemCount(rowCount);
    ASSERT(pListCtrl->GetDataItemCount() == rowCount);
    for (size_t itemIndex = 0; itemIndex < rowCount; ++itemIndex) {
        for (size_t columnIndex = 0; columnIndex < columnCount; ++columnIndex) {
            ui::ListCtrlSubItemData subItemData;
            subItemData.text = ui::StringUtil::Printf("Row %03d / Column %02d", itemIndex, columnIndex);
            subItemData.bShowCheckBox = bShowCheckBox;
            subItemData.nImageId = nImageId;
            if (columnIndex == 0) {
                subItemData.text += "-test1234567890-test1234567890-test1234567890-test1234567890";
            }
            pListCtrl->SetSubItemData(itemIndex, columnIndex, subItemData);
        }
    }
    // Sort, ascending by default
    pListCtrl->SortDataItems(0, true);

    // Set special attributes
    if (nRows > 100) {
        pListCtrl->SetDataItemAlwaysAtTop(100, 0);
        pListCtrl->SetDataItemAlwaysAtTop(101, 1);

        pListCtrl->SetDataItemVisible(103, false);
        pListCtrl->SetDataItemHeight(1, 100, true);
        pListCtrl->SetDataItemHeight(2, 200, true);

        pListCtrl->SetSubItemBkColor(100, 0, ui::UiColor(ui::UiColors::MistyRose));
        pListCtrl->SetSubItemBkColor(101, 0, ui::UiColor(ui::UiColors::MistyRose));
        pListCtrl->SetSubItemBkColor(102, 0, ui::UiColor(ui::UiColors::MistyRose));
        pListCtrl->SetSubItemBkColor(103, 0, ui::UiColor(ui::UiColors::MistyRose));
        pListCtrl->SetSubItemBkColor(104, 0, ui::UiColor(ui::UiColors::MistyRose));
    }
    // Redraw
    UpdateWindow();
}

void MainForm::RunListCtrlTest()
{
    ui::ListCtrl* pListCtrl = ui::Find<ui::ListCtrl>(this, "list_ctrl");
    ASSERT(pListCtrl != nullptr);
    if (pListCtrl == nullptr) {
        return;
    }
    const size_t nRows = pListCtrl->GetDataItemCount();
    const size_t nColumns = pListCtrl->GetColumnCount();
    if (nRows <= 100) {
        return;
    }

#ifdef _DEBUG

    // Basic functionality tests
    const std::string text = "1";
    ui::ListCtrlSubItemData subItemData;
    subItemData.text = text;
    const size_t nDataItemIndex = pListCtrl->AddDataItem(subItemData);
    ASSERT(pListCtrl->GetSubItemText(nDataItemIndex, 0) == text);

    ui::ListCtrlItemData itemData;
    itemData.nItemHeight = 63;
    pListCtrl->SetDataItemData(nDataItemIndex, itemData);
    itemData.nItemHeight = 0;
    pListCtrl->GetDataItemData(nDataItemIndex, itemData);
    ASSERT(itemData.nItemHeight == 63);

    ASSERT(pListCtrl->IsDataItemVisible(nDataItemIndex) == true);
    pListCtrl->SetDataItemVisible(nDataItemIndex, false);
    ASSERT(pListCtrl->IsDataItemVisible(nDataItemIndex) == false);
    pListCtrl->SetDataItemVisible(nDataItemIndex, true);

    ASSERT(pListCtrl->IsDataItemSelected(nDataItemIndex) == false);
    pListCtrl->SetDataItemSelected(nDataItemIndex, true);
    ASSERT(pListCtrl->IsDataItemSelected(nDataItemIndex) == true);
    pListCtrl->SetDataItemSelected(nDataItemIndex, false);

    ASSERT(pListCtrl->GetDataItemAlwaysAtTop(nDataItemIndex) == -1);
    pListCtrl->SetDataItemAlwaysAtTop(nDataItemIndex, 1);
    ASSERT(pListCtrl->GetDataItemAlwaysAtTop(nDataItemIndex) == 1);
    pListCtrl->SetDataItemAlwaysAtTop(nDataItemIndex, -1);

    int32_t nOldHeight = pListCtrl->GetDataItemHeight(nDataItemIndex);
    pListCtrl->SetDataItemHeight(nDataItemIndex, nOldHeight + 21, false);
    ASSERT(pListCtrl->GetDataItemHeight(nDataItemIndex) == nOldHeight + 21);
    pListCtrl->SetDataItemHeight(nDataItemIndex, nOldHeight, false);

    ASSERT(pListCtrl->GetDataItemUserData(nDataItemIndex) == 0);
    pListCtrl->SetDataItemUserData(nDataItemIndex, 1);
    ASSERT(pListCtrl->GetDataItemUserData(nDataItemIndex) == 1);
    pListCtrl->SetDataItemUserData(nDataItemIndex, 0);

    size_t nColumnIndex = 1;
    subItemData.text = "3";
    subItemData.textColor = ui::UiColor(ui::UiColors::Crimson);
    subItemData.bkColor = ui::UiColor(ui::UiColors::BlanchedAlmond);
    subItemData.bShowCheckBox = false;
    subItemData.nImageId = 123;
    subItemData.nTextFormat = ui::TEXT_HCENTER | ui::TEXT_VCENTER;
    pListCtrl->SetSubItemData(nDataItemIndex, nColumnIndex, subItemData);

    ui::ListCtrlSubItemData dataItem2;
    pListCtrl->GetSubItemData(nDataItemIndex, nColumnIndex, dataItem2);
    ASSERT(subItemData.text == std::string(dataItem2.text));
    ASSERT(subItemData.textColor == dataItem2.textColor);
    ASSERT(subItemData.bkColor == dataItem2.bkColor);
    ASSERT(subItemData.bShowCheckBox == dataItem2.bShowCheckBox);
    ASSERT(subItemData.nImageId == dataItem2.nImageId);
    ASSERT(subItemData.nTextFormat == dataItem2.nTextFormat);

    ASSERT(pListCtrl->GetSubItemText(nDataItemIndex, nColumnIndex) == "3");

    subItemData.text = "2";
    nColumnIndex = 2;
    pListCtrl->SetSubItemText(nDataItemIndex, nColumnIndex, subItemData.text);
    ASSERT(pListCtrl->GetSubItemText(nDataItemIndex, nColumnIndex) == subItemData.text);

    subItemData.bkColor = ui::UiColor(ui::UiColors::Aqua);
    nColumnIndex = 2;
    pListCtrl->SetSubItemBkColor(nDataItemIndex, nColumnIndex, subItemData.bkColor);
    ASSERT(pListCtrl->GetSubItemBkColor(nDataItemIndex, nColumnIndex) == subItemData.bkColor);

    subItemData.textColor = ui::UiColor(ui::UiColors::Coral);
    nColumnIndex = 2;
    pListCtrl->SetSubItemTextColor(nDataItemIndex, nColumnIndex, subItemData.textColor);
    ASSERT(pListCtrl->GetSubItemTextColor(nDataItemIndex, nColumnIndex) == subItemData.textColor);

    pListCtrl->SetSubItemShowCheckBox(nDataItemIndex, nColumnIndex, false);
    ASSERT(pListCtrl->IsSubItemShowCheckBox(nDataItemIndex, nColumnIndex) == false);
    pListCtrl->SetSubItemShowCheckBox(nDataItemIndex, nColumnIndex, true);
    ASSERT(pListCtrl->IsSubItemShowCheckBox(nDataItemIndex, nColumnIndex) == true);

    pListCtrl->SetSubItemCheck(nDataItemIndex, nColumnIndex, false);
    ASSERT(pListCtrl->IsSubItemChecked(nDataItemIndex, nColumnIndex) == false);
    pListCtrl->SetSubItemCheck(nDataItemIndex, nColumnIndex, true);
    ASSERT(pListCtrl->IsSubItemChecked(nDataItemIndex, nColumnIndex) == true);

    int32_t nOldValue = pListCtrl->GetDataItemImageId(nDataItemIndex);
    pListCtrl->SetDataItemImageId(nDataItemIndex, 666);
    ASSERT(pListCtrl->GetDataItemImageId(nDataItemIndex) == 666);
    pListCtrl->SetDataItemImageId(nDataItemIndex, nOldValue);

    nOldValue = pListCtrl->GetSubItemImageId(nDataItemIndex, nColumnIndex);
    pListCtrl->SetSubItemImageId(nDataItemIndex, nColumnIndex, 667);
    ASSERT(pListCtrl->GetSubItemImageId(nDataItemIndex, nColumnIndex) == 667);
    pListCtrl->SetSubItemImageId(nDataItemIndex, nColumnIndex, nOldValue);

    subItemData.text = "3";
    nColumnIndex = 0;
    pListCtrl->InsertDataItem(nDataItemIndex, subItemData);
    ASSERT(pListCtrl->GetSubItemText(nDataItemIndex, nColumnIndex) == "3");
    //pListCtrl->DeleteDataItem(nDataItemIndex);
    //pListCtrl->DeleteAllDataItems();

    const size_t nTotalRows = pListCtrl->GetDataItemCount();
    bool bOldMultiSelect = pListCtrl->IsMultiSelect();
    std::vector<size_t> oldSelectedIndexs;
    pListCtrl->GetSelectedDataItems(oldSelectedIndexs);

    // Multi-select
    pListCtrl->SetMultiSelect(true);
    pListCtrl->SetSelectNone();
    std::vector<size_t> selectedIndexs;
    pListCtrl->GetSelectedDataItems(selectedIndexs);
    ASSERT(selectedIndexs.empty());

    selectedIndexs.push_back(10);
    selectedIndexs.push_back(20);
    selectedIndexs.push_back(30);
    pListCtrl->SetSelectedDataItems(selectedIndexs, false);

    std::vector<size_t> selectedIndexs2;
    pListCtrl->GetSelectedDataItems(selectedIndexs2);
    ASSERT(selectedIndexs == selectedIndexs2);

    selectedIndexs.push_back(40);
    pListCtrl->SetDataItemSelected(40, true);
    selectedIndexs2.clear();
    pListCtrl->GetSelectedDataItems(selectedIndexs2);
    ASSERT(selectedIndexs == selectedIndexs2);

    selectedIndexs.pop_back();
    pListCtrl->SetSelectedDataItems(selectedIndexs, false);

    selectedIndexs.push_back(40);
    selectedIndexs2.clear();
    pListCtrl->GetSelectedDataItems(selectedIndexs2);
    ASSERT(selectedIndexs == selectedIndexs2);

    selectedIndexs.clear();
    selectedIndexs.push_back(50);
    selectedIndexs.push_back(70);
    pListCtrl->SetSelectedDataItems(selectedIndexs, true);
    selectedIndexs2.clear();
    pListCtrl->GetSelectedDataItems(selectedIndexs2);
    ASSERT(selectedIndexs == selectedIndexs2);

    pListCtrl->SetSelectNone();
    pListCtrl->GetSelectedDataItems(selectedIndexs);
    ASSERT(selectedIndexs.empty());

    for (size_t i = 0; i < nTotalRows; ++i) {
        pListCtrl->SetDataItemVisible(i, true);
        pListCtrl->SetDataItemAlwaysAtTop(i, -1);
    }

    pListCtrl->SetSelectAll();
    pListCtrl->GetSelectedDataItems(selectedIndexs);
    ASSERT(selectedIndexs.size() == nTotalRows);

    // Single-select test
    pListCtrl->SetSelectAll();    
    pListCtrl->SetMultiSelect(false);
    pListCtrl->SetDataItemSelected(40, true);
    ASSERT(!pListCtrl->IsMultiSelect());
    ASSERT(pListCtrl->IsDataItemSelected(40));
    pListCtrl->GetSelectedDataItems(selectedIndexs);
    ASSERT(selectedIndexs.size() == 1);
    ASSERT(selectedIndexs[0] == 40);

    pListCtrl->SetDataItemSelected(40, false);
    ASSERT(!pListCtrl->IsDataItemSelected(40));

    pListCtrl->SetDataItemSelected(50, true);
    ASSERT(pListCtrl->IsDataItemSelected(50));

    pListCtrl->SetDataItemSelected(60, true);
    ASSERT(pListCtrl->IsDataItemSelected(60));
    ASSERT(!pListCtrl->IsDataItemSelected(50));
    pListCtrl->GetSelectedDataItems(selectedIndexs);
    ASSERT(selectedIndexs.size() == 1);
    ASSERT(selectedIndexs[0] == 60);

    pListCtrl->SetSelectNone();
    pListCtrl->GetSelectedDataItems(selectedIndexs);
    ASSERT(selectedIndexs.empty());
    ASSERT(!pListCtrl->IsDataItemSelected(60));

    // Add/delete tests
    std::string text60 = pListCtrl->GetSubItemText(60, 0);
    pListCtrl->SetDataItemSelected(60, true);
    ASSERT(pListCtrl->IsDataItemSelected(60));

    ui::ListCtrlSubItemData dataItem3;
    nColumnIndex = 0;
    dataItem3.text = "Test";
    size_t nDataItemIndex3 = pListCtrl->AddDataItem(dataItem3);
    ASSERT(nDataItemIndex3 > 60);
    ASSERT(pListCtrl->IsDataItemSelected(60));
    ASSERT(pListCtrl->GetSubItemText(60, 0) == text60);
    pListCtrl->GetSelectedDataItems(selectedIndexs);
    ASSERT((selectedIndexs.size() == 1) && (selectedIndexs[0] == 60));

    bool bOk = pListCtrl->InsertDataItem(65, dataItem3);
    ASSERT(bOk);
    ASSERT(pListCtrl->IsDataItemSelected(60));
    ASSERT(pListCtrl->GetSubItemText(60, 0) == text60);
    pListCtrl->GetSelectedDataItems(selectedIndexs);
    ASSERT((selectedIndexs.size() == 1) && (selectedIndexs[0] == 60));

    bOk = pListCtrl->InsertDataItem(50, dataItem3);
    ASSERT(bOk);
    ASSERT(pListCtrl->IsDataItemSelected(61));
    ASSERT(pListCtrl->GetSubItemText(61, 0) == text60);
    pListCtrl->GetSelectedDataItems(selectedIndexs);
    ASSERT((selectedIndexs.size() == 1) && (selectedIndexs[0] == 61));

    pListCtrl->DeleteDataItem(50);
    ASSERT(pListCtrl->IsDataItemSelected(60));
    ASSERT(pListCtrl->GetSubItemText(60, 0) == text60);
    pListCtrl->GetSelectedDataItems(selectedIndexs);
    ASSERT((selectedIndexs.size() == 1) && (selectedIndexs[0] == 60));

    pListCtrl->SortDataItems(0, true);
    pListCtrl->GetSelectedDataItems(selectedIndexs);
    ASSERT(selectedIndexs.size() == 1);
    ASSERT(pListCtrl->GetSubItemText(selectedIndexs[0], 0) == text60);

    pListCtrl->SortDataItems(0, false);
    pListCtrl->GetSelectedDataItems(selectedIndexs);
    ASSERT(selectedIndexs.size() == 1);
    ASSERT(pListCtrl->GetSubItemText(selectedIndexs[0], 0) == text60);

    text60 = pListCtrl->GetSubItemText(60, 0);
    pListCtrl->SetDataItemSelected(60, true);
    ASSERT(pListCtrl->IsDataItemSelected(60));
    pListCtrl->SetDataItemCount(pListCtrl->GetDataItemCount() + 10);
    ASSERT(pListCtrl->IsDataItemSelected(60));
    ASSERT(pListCtrl->GetSubItemText(60, 0) == text60);

    pListCtrl->SetDataItemCount(61);
    ASSERT(pListCtrl->IsDataItemSelected(60));
    ASSERT(pListCtrl->GetSubItemText(60, 0) == text60);

    pListCtrl->SetDataItemCount(60);
    pListCtrl->GetSelectedDataItems(selectedIndexs);
    ASSERT(selectedIndexs.empty());

    pListCtrl->SetDataItemSelected(30, true);
    ASSERT(pListCtrl->IsDataItemSelected(30));

    pListCtrl->DeleteAllDataItems();
    pListCtrl->GetSelectedDataItems(selectedIndexs);
    ASSERT(selectedIndexs.empty());

    pListCtrl->SetDataItemCount(60);
    pListCtrl->SetDataItemSelected(30, true);
    ASSERT(pListCtrl->IsDataItemSelected(30));
    pListCtrl->GetSelectedDataItems(selectedIndexs);
    ASSERT(selectedIndexs.size() == 1);

    while (pListCtrl->DeleteColumn(0)) {
    }
    ASSERT(pListCtrl->GetDataItemCount() == 0);
    pListCtrl->GetSelectedDataItems(selectedIndexs);
    ASSERT(selectedIndexs.empty());

    // Restore after test completion
    pListCtrl->SetMultiSelect(bOldMultiSelect);
    pListCtrl->SetSelectedDataItems(oldSelectedIndexs, true);

    pListCtrl->DeleteAllDataItems();

    InsertItemData((int32_t)nRows, (int32_t)nColumns, -1);
#endif
}

void MainForm::TestListCtrlEvents(ui::ListCtrl* pListCtrl)
{
    // Event binding, test event interfaces
    auto OnListCtrlEvent = [this, pListCtrl](const ui::EventArgs& args) {
        ASSERT(pListCtrl == args.GetSender());
        std::string sInfo = GetEventDisplayInfo(args);
        OutputDebugLog(sInfo);
        };

    // Bind events and forward to the outer layer
    pListCtrl->AttachSelect([this, OnListCtrlEvent](const ui::EventArgs& args) {
        OnListCtrlEvent(args);
        return true;
        });
    pListCtrl->AttachSelChanged([this, OnListCtrlEvent](const ui::EventArgs& args) {
        OnListCtrlEvent(args);
        return true;
        });
    pListCtrl->AttachDoubleClick([this, OnListCtrlEvent](const ui::EventArgs& args) {
        OnListCtrlEvent(args);
        return true;
        });
    pListCtrl->AttachClick([this, OnListCtrlEvent](const ui::EventArgs& args) {
        OnListCtrlEvent(args);
        return true;
        });
    pListCtrl->AttachRClick([this, OnListCtrlEvent](const ui::EventArgs& args) {
        OnListCtrlEvent(args);
        return true;
        });
    pListCtrl->AttachItemMouseEnter([this, OnListCtrlEvent](const ui::EventArgs& args) {
        OnListCtrlEvent(args);
        return true;
        });
    pListCtrl->AttachItemMouseLeave([this, OnListCtrlEvent](const ui::EventArgs& args) {
        OnListCtrlEvent(args);
        return true;
        });
    pListCtrl->AttachSubItemMouseEnter([this, OnListCtrlEvent](const ui::EventArgs& args) {
        OnListCtrlEvent(args);
        OnReportViewSubItemMouseEnter(args);
        return true;
        });
    pListCtrl->AttachSubItemMouseLeave([this, OnListCtrlEvent](const ui::EventArgs& args) {
        OnListCtrlEvent(args);
        OnReportViewSubItemMouseLeave(args);
        return true;
        });
    pListCtrl->AttachReturn([this, OnListCtrlEvent](const ui::EventArgs& args) {
        OnListCtrlEvent(args);
        return true;
        });
    pListCtrl->AttachKeyDown([this, OnListCtrlEvent](const ui::EventArgs& args) {
        OnListCtrlEvent(args);
        return true;
        });
    pListCtrl->AttachKeyUp([this, OnListCtrlEvent](const ui::EventArgs& args) {
        OnListCtrlEvent(args);
        return true;
        });
    pListCtrl->AttachViewTypeChanged([this, OnListCtrlEvent](const ui::EventArgs& args) {
        OnListCtrlEvent(args);
        return true;
        });
    pListCtrl->AttachViewPosChanged([this, OnListCtrlEvent](const ui::EventArgs& args) {
        OnListCtrlEvent(args);
        return true;
        });
    pListCtrl->AttachViewSizeChanged([this, OnListCtrlEvent](const ui::EventArgs& args) {
        OnListCtrlEvent(args);
        return true;
        });

    auto OnListCtrlItemFilledEvent = [this, pListCtrl](const ui::EventArgs& args) {
        ASSERT(pListCtrl == args.GetSender());
        std::string sInfo = GetItemFilledEventDisplayInfo(args);
        OutputDebugLog(sInfo);
        };
    pListCtrl->AttachReportViewItemFilled([this, OnListCtrlItemFilledEvent](const ui::EventArgs& args) {
        OnListCtrlItemFilledEvent(args);
        return true;
        });
    pListCtrl->AttachReportViewSubItemFilled([this, OnListCtrlItemFilledEvent](const ui::EventArgs& args) {
        OnListCtrlItemFilledEvent(args);
        OnReportViewSubItemFilled(args);
        return true;
        });
    pListCtrl->AttachListViewItemFilled([this, OnListCtrlItemFilledEvent](const ui::EventArgs& args) {
        OnListCtrlItemFilledEvent(args);
        return true;
        });
    pListCtrl->AttachIconViewItemFilled([this, OnListCtrlItemFilledEvent](const ui::EventArgs& args) {
        OnListCtrlItemFilledEvent(args);
        return true;
        });
}

std::string MainForm::GetEventDisplayInfo(const ui::EventArgs& args)
{
    std::string sInfo = ui::EventUtils::EventTypeToString(args.eventType);
    while (sInfo.size() < 24) {
        sInfo += " ";
    }
    if ((args.eventType == ui::kEventSelect) ||
        (args.eventType == ui::kEventSelChanged) ||
        (args.eventType == ui::kEventMouseDoubleClick) ||
        (args.eventType == ui::kEventClick) ||
        (args.eventType == ui::kEventRClick) ||
        (args.eventType == ui::kEventItemMouseEnter) ||
        (args.eventType == ui::kEventItemMouseLeave) ||
        (args.eventType == ui::kEventSubItemMouseEnter) ||
        (args.eventType == ui::kEventSubItemMouseLeave) ||
        (args.eventType == ui::kEventReturn) ||
        (args.eventType == ui::kEventKeyDown) ||
        (args.eventType == ui::kEventKeyUp)) {
        std::string labelText;
        int32_t nDataItemIndex = -1;
        int32_t nDataColumnIndex = -1;
        ui::ListCtrlType listCtrlType = (ui::ListCtrlType)args.listCtrlType;
        if (listCtrlType == ui::ListCtrlType::Report) {
            sInfo += "ListCtrlType::Report: ";
            ui::ListCtrlItem* pItem = nullptr;
            if ((args.eventType == ui::kEventSubItemMouseEnter) || (args.eventType == ui::kEventSubItemMouseLeave)) {
                ui::ListCtrlSubItem* pSubItem = (ui::ListCtrlSubItem*)args.pEventData;
                if (pSubItem != nullptr) {
                    pItem = pSubItem->GetListCtrlItem();

                    nDataColumnIndex = (int32_t)pSubItem->GetDataColumnIndex();
                    labelText = pSubItem->GetText();
                }
            }
            else {
                pItem = (ui::ListCtrlItem*)args.pEventData;
            }
            if (pItem != nullptr) {
                nDataItemIndex = (int32_t)pItem->GetDataItemIndex();
                if ((args.eventType >= ui::kEventMouseBegin) && (args.eventType <= ui::kEventMouseEnd)) {
                    // Mouse message: get the current column (get the child control of the column based on the current mouse position)
                    ui::ListCtrlSubItem* pSubItem = pItem->GetSubItem(args.ptMouse);
                    if (pSubItem != nullptr) {
                        nDataColumnIndex = (int32_t)pSubItem->GetDataColumnIndex();
                        labelText = pSubItem->GetText();
                    }
                }
            }
        }
        else if (listCtrlType == ui::ListCtrlType::Icon) {
            sInfo += "ListCtrlType::Icon: ";
            ui::ListCtrlIconViewItem* pItem = (ui::ListCtrlIconViewItem*)args.pEventData;
            if (pItem != nullptr) {
                nDataItemIndex = (int32_t)pItem->GetDataItemIndex();
                labelText = pItem->GetLabelText();
            }
        }
        else if (listCtrlType == ui::ListCtrlType::List) {
            sInfo += "ListCtrlType::List: ";
            ui::ListCtrlListViewItem* pItem = (ui::ListCtrlListViewItem*)args.pEventData;
            if (pItem != nullptr) {
                nDataItemIndex = (int32_t)pItem->GetDataItemIndex();
                labelText = pItem->GetLabelText();
            }
        }
        else {
            sInfo += "ListCtrl: ";
        }
        if (nDataItemIndex >= 0) {
            if ((args.eventType >= ui::kEventKeyBegin) && (args.eventType <= ui::kEventKeyEnd)) {
                // Keyboard message
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
            }
            if (labelText.empty()) {
                if (nDataColumnIndex >= 0) {
                    sInfo += ui::StringUtil::Printf("nDataItemIndex=%d, nDataColumnIndex=%d", nDataItemIndex, nDataColumnIndex);
                }
                else {
                    sInfo += ui::StringUtil::Printf("nDataItemIndex=%d", nDataItemIndex);
                }
            }
            else {
                if (nDataColumnIndex >= 0) {
                    sInfo += ui::StringUtil::Printf("nDataItemIndex=%d, nDataColumnIndex=%d, LabelText='%s'", nDataItemIndex, nDataColumnIndex, labelText.c_str());
                }
                else {
                    sInfo += ui::StringUtil::Printf("nDataItemIndex=%d, LabelText='%s'", nDataItemIndex, labelText.c_str());
                }
            }
        }
    }
    else if ((args.eventType == ui::kEventViewTypeChanged) ||
             (args.eventType == ui::kEventViewPosChanged)  ||
             (args.eventType == ui::kEventViewSizeChanged)) {
        ui::ListCtrlType listCtrlType = (ui::ListCtrlType)args.listCtrlType;
        ui::Control* pControl = nullptr;
        if (listCtrlType == ui::ListCtrlType::Report) {
            sInfo += "ListCtrlType::Report: ";
            ui::ListCtrlReportView* pView = (ui::ListCtrlReportView*)args.pEventData;
            pControl = dynamic_cast<ui::Control*>(pView);
        }
        else if (listCtrlType == ui::ListCtrlType::Icon) {
            sInfo += "ListCtrlType::Icon: ";
            ui::ListCtrlIconView* pView = (ui::ListCtrlIconView*)args.pEventData;
            pControl = dynamic_cast<ui::Control*>(pView);
        }
        else if (listCtrlType == ui::ListCtrlType::List) {
            sInfo += "ListCtrlType::List: ";
            ui::ListCtrlListView* pView = (ui::ListCtrlListView*)args.pEventData;
            pControl = dynamic_cast<ui::Control*>(pView);
        }
        ASSERT(pControl != nullptr);
        if (args.eventType == ui::kEventViewTypeChanged) {
            // Not shown
            sInfo += "ViewTypeChanged";
        }
        else if (args.eventType == ui::kEventViewPosChanged) {
            sInfo += ui::StringUtil::Printf("left:%d, top: %d", pControl->GetRect().left, pControl->GetRect().top);
        }
        else if (args.eventType == ui::kEventViewSizeChanged) {
            sInfo += ui::StringUtil::Printf("width:%d, height: %d", pControl->GetRect().Width(), pControl->GetRect().Height());
        }
    }
    else {
        ASSERT(0);
    }
   
    sInfo += "\n";
    return sInfo;
}

std::string MainForm::GetItemFilledEventDisplayInfo(const ui::EventArgs& args)
{
    std::string sInfo = ui::EventUtils::EventTypeToString(args.eventType);
    while (sInfo.size() < 32) {
        sInfo += " ";
    }

    sInfo += ui::StringUtil::Printf("ListBoxItemIndex=%zu ", (size_t)args.wParam);
    sInfo += ui::StringUtil::Printf("DataItemIndex=%zu ", (size_t)args.lParam);

    if (args.eventType == ui::kEventReportViewItemFilled) {
        ui::ListCtrlItem* pItem = (ui::ListCtrlItem*)args.pEventData;
        ASSERT(pItem != nullptr);
        if (pItem != nullptr) {
            ASSERT(pItem->GetListBoxIndex() == (size_t)args.wParam);
            ASSERT(pItem->GetDataItemIndex() == (size_t)args.lParam);
        }
    }
    else if (args.eventType == ui::kEventReportViewSubItemFilled) {
        ui::ListCtrlSubItem* pSubItem = (ui::ListCtrlSubItem*)args.pEventData;
        ASSERT(pSubItem != nullptr);
        if (pSubItem != nullptr) {
            ASSERT(pSubItem->GetDataItemIndex() == (size_t)args.lParam);
            ui::ListCtrlItem* pItem = pSubItem->GetListCtrlItem();
            ASSERT(pItem != nullptr);
            if (pItem != nullptr) {
                ASSERT(pItem->GetListBoxIndex() == (size_t)args.wParam);
                ASSERT(pItem->GetDataItemIndex() == (size_t)args.lParam);
            }
            sInfo += ui::StringUtil::Printf("DataColumnIndex='%zu' ", pSubItem->GetDataColumnIndex());
            sInfo += ui::StringUtil::Printf("LabelText='%s' ", pSubItem->GetText().c_str());
        }
    }
    else if (args.eventType == ui::kEventListViewItemFilled) {
        ui::ListCtrlListViewItem* pItem = (ui::ListCtrlListViewItem*)args.pEventData;
        ASSERT(pItem != nullptr);
        if (pItem != nullptr) {
            ASSERT(pItem->GetListBoxIndex() == (size_t)args.wParam);
            ASSERT(pItem->GetDataItemIndex() == (size_t)args.lParam);
            sInfo += ui::StringUtil::Printf("LabelText='%s'", pItem->GetLabelText().c_str());
        }
    }
    else if (args.eventType == ui::kEventIconViewItemFilled) {
        ui::ListCtrlIconViewItem* pItem = (ui::ListCtrlIconViewItem*)args.pEventData;
        ASSERT(pItem != nullptr);
        if (pItem != nullptr) {
            ASSERT(pItem->GetListBoxIndex() == (size_t)args.wParam);
            ASSERT(pItem->GetDataItemIndex() == (size_t)args.lParam);
            sInfo += ui::StringUtil::Printf("LabelText='%s'", pItem->GetLabelText().c_str());
        }
    }
    return sInfo;
}

void MainForm::OutputDebugLog(const std::string& logMsg)
{
#if defined DUI_BUILD_FOR_WIN && defined _DEBUG
    //::OutputDebugString(logMsg.c_str());
#endif
}

void MainForm::OnReportViewSubItemFilled(const ui::EventArgs& args)
{
    // This test is disabled by default
    return;

    if (args.eventType != ui::kEventReportViewSubItemFilled) {
        return;
    }
    ui::ListCtrlSubItem* pSubItem = (ui::ListCtrlSubItem*)args.pEventData;
    ASSERT(pSubItem != nullptr);
    if (pSubItem == nullptr) {
        return;
    }

    if (pSubItem->GetItemCount() == 0) {
        // Feature demo: dynamically add a new button
        ui::Button* pHoverButton = new ui::Button(pSubItem->GetWindow());
        pHoverButton->SetClass("btn_recycle");
        pHoverButton->SetAttribute("width", "auto");
        pHoverButton->SetAttribute("height", "auto");
        pHoverButton->SetAttribute("halign", "right");
        pHoverButton->SetAttribute("valign", "top");
        pHoverButton->SetAttribute("margin", "0,8,8,0");
        pHoverButton->SetToolTipText("Hover Button");

        // Floating button
        pHoverButton->SetFloat(true);
        pHoverButton->SetVisible(false);
        pSubItem->AddItem(pHoverButton);

        // Current row and column information
        size_t nDataItemIndex = pSubItem->GetDataItemIndex();
        size_t nDataColumnIndex = pSubItem->GetDataColumnIndex();

        // Bind events
        pHoverButton->AttachClick([this, nDataItemIndex, nDataColumnIndex](const ui::EventArgs& /*args*/){
            std::string title = "Hover Button Clicked";
            std::string content = ui::StringUtil::Printf("DataItemIndex:%zu, DataColumnIndex:%zu", nDataItemIndex, nDataColumnIndex);
            ui::SystemUtil::ShowMessageBox(this, content, title);
            return true;
            });
    }
}

void MainForm::OnReportViewSubItemMouseEnter(const ui::EventArgs& args)
{
    if (args.eventType != ui::kEventSubItemMouseEnter) {
        return;
    }
    if ((ui::ListCtrlType)args.listCtrlType != ui::ListCtrlType::Report) {
        return;
    }
    ui::ListCtrlSubItem* pSubItem = (ui::ListCtrlSubItem*)args.pEventData;
    ASSERT(pSubItem != nullptr);
    if (pSubItem == nullptr) {
        return;
    }
    if (pSubItem->GetItemCount() > 0) {
        // On mouse enter: if a child control exists, show it
        ui::Control* pDemoControl = pSubItem->GetItemAt(0);
        if ((pDemoControl != nullptr) && !pDemoControl->IsVisible()) {
            pDemoControl->SetVisible(true);
        }
    }
}

void MainForm::OnReportViewSubItemMouseLeave(const ui::EventArgs& args)
{
    if (args.eventType != ui::kEventSubItemMouseLeave) {
        return;
    }
    if ((ui::ListCtrlType)args.listCtrlType != ui::ListCtrlType::Report) {
        return;
    }
    ui::ListCtrlSubItem* pSubItem = (ui::ListCtrlSubItem*)args.pEventData;
    ASSERT(pSubItem != nullptr);
    if (pSubItem == nullptr) {
        return;
    }
    if (pSubItem->GetItemCount() > 0) {
        // On mouse leave: if a child control exists, hide it
        ui::Control* pDemoControl = pSubItem->GetItemAt(0);
        if ((pDemoControl != nullptr) && pDemoControl->IsVisible()) {
            pDemoControl->SetVisible(false);
        }
    }
}
