#include "MainForm.h"

MainForm::MainForm():
    m_fLoadingPercent(0)
{
}

MainForm::~MainForm()
{
}

DString MainForm::GetSkinFolder()
{
    return _T("");
}

DString MainForm::GetSkinFile()
{
    // Pure code mode: no layout XML is loaded
    return _T("");
}

void MainForm::BuildUI()
{
    // Corresponds to the list_ctrl.xml layout (hand-written pure code)
    ui::VBox* pRoot = new ui::VBox(this);
    pRoot->SetBkColor(_T("bk_wnd_darkcolor"));

    // Title bar area
    ui::HBox* pCaption = new ui::HBox(this);
    pCaption->SetAttribute(_T("name"), _T("window_caption_bar"));
    pCaption->SetAttribute(_T("width"), _T("stretch"));
    pCaption->SetAttribute(_T("height"), _T("36"));
    pCaption->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    ui::Control* pSpacer = new ui::Control(this);
    pSpacer->SetAttribute(_T("mouse_enabled"), _T("false"));
    pCaption->AddItem(pSpacer);

    ui::Button* pFullscreenBtn = new ui::Button(this);
    pFullscreenBtn->SetClass(_T("btn_wnd_fullscreen_11"));
    pFullscreenBtn->SetAttribute(_T("height"), _T("32"));
    pFullscreenBtn->SetAttribute(_T("width"), _T("40"));
    pFullscreenBtn->SetName(_T("fullscreenbtn"));
    pFullscreenBtn->SetAttribute(_T("margin"), _T("0,2,0,2"));
    pFullscreenBtn->SetToolTipText(_T("Fullscreen, press ESC to exit fullscreen"));
    pCaption->AddItem(pFullscreenBtn);

    ui::Button* pMinBtn = new ui::Button(this);
    pMinBtn->SetClass(_T("btn_wnd_min_11"));
    pMinBtn->SetAttribute(_T("height"), _T("32"));
    pMinBtn->SetAttribute(_T("width"), _T("40"));
    pMinBtn->SetName(_T("minbtn"));
    pMinBtn->SetAttribute(_T("margin"), _T("0,2,0,2"));
    pMinBtn->SetToolTipText(_T("Minimize"));
    pCaption->AddItem(pMinBtn);

    ui::Box* pMaxBox = new ui::Box(this);
    pMaxBox->SetAttribute(_T("height"), _T("stretch"));
    pMaxBox->SetAttribute(_T("width"), _T("40"));
    pMaxBox->SetAttribute(_T("margin"), _T("0,2,0,2"));
    pCaption->AddItem(pMaxBox);

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
    pCaption->AddItem(pCloseBtn);

    // Work area
    ui::VBox* pContent = new ui::VBox(this);
    pRoot->AddItem(pContent);

    // Table type row
    ui::HBox* pTypeRow = new ui::HBox(this);
    pTypeRow->SetAttribute(_T("height"), _T("auto"));
    pContent->AddItem(pTypeRow);

    ui::Label* pTypeLabel = new ui::Label(this);
    pTypeLabel->SetText(_T("Table Type:"));
    pTypeLabel->SetAttribute(_T("valign"), _T("center"));
    pTypeLabel->SetAttribute(_T("margin"), _T("2,0,2,0"));
    pTypeRow->AddItem(pTypeLabel);

    ui::Combo* pTypeCombo = new ui::Combo(this);
    pTypeCombo->SetClass(_T("combo"));
    pTypeCombo->SetName(_T("list_ctrl_type_combo"));
    pTypeCombo->SetAttribute(_T("combo_type"), _T("drop_list"));
    pTypeCombo->SetAttribute(_T("dropbox_size"), _T("0,300"));
    pTypeCombo->SetAttribute(_T("combo_icon_class"), _T(""));
    pTypeCombo->SetAttribute(_T("height"), _T("26"));
    pTypeCombo->SetAttribute(_T("width"), _T("80"));
    pTypeCombo->SetAttribute(_T("margin"), _T("0,0,0,1"));
    pTypeCombo->SetAttribute(_T("valign"), _T("center"));
    pTypeRow->AddItem(pTypeCombo);

    {
        struct ComboItem { DString text; int32_t userData; };
        const ComboItem items[] = {
            { _T("Report"), 0 }, { _T("Icon"), 1 }, { _T("List"), 2 },
        };
        for (const auto& item : items) {
            ui::TreeNode* pNode = new ui::TreeNode(this);
            pNode->SetClass(_T("tree_node"));
            pNode->SetAttribute(_T("padding"), _T("4"));
            pNode->SetText(item.text);
            pNode->SetUserDataID((size_t)item.userData);
            pTypeCombo->GetTreeView()->GetRootNode()->AddChildNode(pNode);
        }
    }

    ui::CheckBox* pMultiSelect = new ui::CheckBox(this);
    pMultiSelect->SetClass(_T("checkbox_1"));
    pMultiSelect->SetName(_T("checkbox_multi_select"));
    pMultiSelect->SetText(_T("Multi-select"));
    pMultiSelect->SetAttribute(_T("margin"), _T("4,0,0,0"));
    pMultiSelect->SetAttribute(_T("valign"), _T("center"));
    pTypeRow->AddItem(pMultiSelect);

    // Tool area
    ui::HBox* pToolArea = new ui::HBox(this);
    pToolArea->SetAttribute(_T("height"), _T("auto"));
    pContent->AddItem(pToolArea);

    // Report type control group
    ui::GroupVBox* pReportGroup = new ui::GroupVBox(this);
    pReportGroup->SetName(_T("report_group"));
    pReportGroup->SetAttribute(_T("height"), _T("auto"));
    pReportGroup->SetAttribute(_T("width"), _T("770"));
    pReportGroup->SetAttribute(_T("text"), _T("Report Type"));
    pToolArea->AddItem(pReportGroup);

    // Row 1: header controls
    ui::HBox* pRow1 = new ui::HBox(this);
    pRow1->SetAttribute(_T("minheight"), _T("18"));
    pRow1->SetBkColor(_T("bk_wnd_darkcolor"));
    pRow1->SetAttribute(_T("height"), _T("auto"));
    pRow1->SetAttribute(_T("margin"), _T("4,18,4,0"));
    pReportGroup->AddItem(pRow1);

    ui::Label* pLbl = new ui::Label(this);
    pLbl->SetText(_T("Header Controls:"));
    pLbl->SetAttribute(_T("valign"), _T("center"));
    pLbl->SetAttribute(_T("margin"), _T("2,0,2,0"));
    pRow1->AddItem(pLbl);

    ui::Option* pOpt = new ui::Option(this);
    pOpt->SetClass(_T("option_2"));
    pOpt->SetAttribute(_T("group"), _T("show"));
    pOpt->SetAttribute(_T("width"), _T("64"));
    pOpt->SetAttribute(_T("height"), _T("32"));
    pOpt->SetText(_T("Hide"));
    pOpt->SetAttribute(_T("padding"), _T("2,2,2,2"));
    pOpt->SetAttribute(_T("borderround"), _T("2,2"));
    pOpt->SetAttribute(_T("valign"), _T("center"));
    pRow1->AddItem(pOpt);

    pOpt = new ui::Option(this);
    pOpt->SetClass(_T("option_2"));
    pOpt->SetAttribute(_T("group"), _T("show"));
    pOpt->SetAttribute(_T("width"), _T("64"));
    pOpt->SetAttribute(_T("height"), _T("32"));
    pOpt->SetText(_T("Show"));
    pOpt->SetAttribute(_T("padding"), _T("2,2,2,2"));
    pOpt->SetAttribute(_T("borderround"), _T("2,2"));
    pOpt->SetAttribute(_T("valign"), _T("center"));
    pOpt->Selected(true);
    pRow1->AddItem(pOpt);

    ui::Line* pLine = new ui::Line(this);
    pLine->SetAttribute(_T("vertical"), _T("true"));
    pLine->SetAttribute(_T("margin"), _T("0,8,4,8"));
    pLine->SetAttribute(_T("width"), _T("2"));
    pRow1->AddItem(pLine);

    pLbl = new ui::Label(this);
    pLbl->SetText(_T("Drag Header to Reorder:"));
    pLbl->SetAttribute(_T("valign"), _T("center"));
    pLbl->SetAttribute(_T("margin"), _T("2,0,2,0"));
    pRow1->AddItem(pLbl);

    pOpt = new ui::Option(this);
    pOpt->SetClass(_T("option_2"));
    pOpt->SetAttribute(_T("group"), _T("drag_order"));
    pOpt->SetAttribute(_T("width"), _T("64"));
    pOpt->SetAttribute(_T("height"), _T("32"));
    pOpt->SetText(_T("Forbidden"));
    pOpt->SetAttribute(_T("padding"), _T("2,2,2,2"));
    pOpt->SetAttribute(_T("borderround"), _T("2,2"));
    pOpt->SetAttribute(_T("valign"), _T("center"));
    pRow1->AddItem(pOpt);

    pOpt = new ui::Option(this);
    pOpt->SetClass(_T("option_2"));
    pOpt->SetAttribute(_T("group"), _T("drag_order"));
    pOpt->SetAttribute(_T("width"), _T("64"));
    pOpt->SetAttribute(_T("height"), _T("32"));
    pOpt->SetText(_T("Allowed"));
    pOpt->SetAttribute(_T("padding"), _T("2,2,2,2"));
    pOpt->SetAttribute(_T("borderround"), _T("2,2"));
    pOpt->SetAttribute(_T("valign"), _T("center"));
    pOpt->Selected(true);
    pRow1->AddItem(pOpt);

    pLine = new ui::Line(this);
    pLine->SetAttribute(_T("vertical"), _T("true"));
    pLine->SetAttribute(_T("margin"), _T("0,8,4,8"));
    pLine->SetAttribute(_T("width"), _T("2"));
    pRow1->AddItem(pLine);

    pLbl = new ui::Label(this);
    pLbl->SetText(_T("Header Height:"));
    pLbl->SetAttribute(_T("valign"), _T("center"));
    pLbl->SetAttribute(_T("margin"), _T("2,0,2,0"));
    pRow1->AddItem(pLbl);

    ui::RichEdit* pHeaderHeight = new ui::RichEdit(this);
    pHeaderHeight->SetClass(_T("simple simple_border rich_edit_spin"));
    pHeaderHeight->SetName(_T("header_height_edit"));
    pHeaderHeight->SetAttribute(_T("min_number"), _T("0"));
    pHeaderHeight->SetAttribute(_T("max_number"), _T("512"));
    pHeaderHeight->SetAttribute(_T("limit_text"), _T("3"));
    pHeaderHeight->SetText(_T("0"));
    pHeaderHeight->SetAttribute(_T("margin"), _T("0,2,0,0"));
    pRow1->AddItem(pHeaderHeight);

    ui::Button* pStretchBtn = new ui::Button(this);
    pStretchBtn->SetClass(_T("btn_global_color_gray"));
    pStretchBtn->SetName(_T("set_column_stretch"));
    pStretchBtn->SetText(_T("Set Column Widths Proportionally"));
    pStretchBtn->SetAttribute(_T("width"), _T("auto"));
    pStretchBtn->SetAttribute(_T("height"), _T("30"));
    pStretchBtn->SetAttribute(_T("border_round"), _T("3,3"));
    pStretchBtn->SetAttribute(_T("text_padding"), _T("8,0,8,0"));
    pStretchBtn->SetAttribute(_T("margin"), _T("4,0,0,0"));
    pRow1->AddItem(pStretchBtn);

    // Row 2: column controls
    ui::HBox* pRow2 = new ui::HBox(this);
    pRow2->SetAttribute(_T("minheight"), _T("18"));
    pRow2->SetBkColor(_T("bk_wnd_darkcolor"));
    pRow2->SetAttribute(_T("height"), _T("auto"));
    pRow2->SetAttribute(_T("margin"), _T("4,0,4,0"));
    pReportGroup->AddItem(pRow2);

    pLbl = new ui::Label(this);
    pLbl->SetText(_T("Column Controls:"));
    pLbl->SetAttribute(_T("valign"), _T("center"));
    pLbl->SetAttribute(_T("margin"), _T("2,0,2,0"));
    pRow2->AddItem(pLbl);

    ui::Combo* pColumnCombo = new ui::Combo(this);
    pColumnCombo->SetClass(_T("combo"));
    pColumnCombo->SetName(_T("column_combo"));
    pColumnCombo->SetAttribute(_T("combo_type"), _T("drop_list"));
    pColumnCombo->SetAttribute(_T("dropbox_size"), _T("0,300"));
    pColumnCombo->SetAttribute(_T("combo_icon_class"), _T(""));
    pColumnCombo->SetAttribute(_T("height"), _T("26"));
    pColumnCombo->SetAttribute(_T("width"), _T("80"));
    pColumnCombo->SetAttribute(_T("margin"), _T("0,0,0,1"));
    pColumnCombo->SetAttribute(_T("valign"), _T("center"));
    pRow2->AddItem(pColumnCombo);

    pLine = new ui::Line(this);
    pLine->SetAttribute(_T("vertical"), _T("true"));
    pLine->SetAttribute(_T("margin"), _T("8,4,4,4"));
    pLine->SetAttribute(_T("width"), _T("2"));
    pRow2->AddItem(pLine);

    struct Row2Check { DString name; DString text; };
    const Row2Check row2Checks[] = {
        { _T("checkbox_column_show"), _T("Show This Column") },
        { _T("checkbox_column_width"), _T("Resizable Width") },
        { _T("checkbox_column_sort"), _T("Sortable") },
        { _T("checkbox_column_icon_at_top"), _T("Sort Icon on Top") },
        { _T("checkbox_column_drag_order"), _T("Drag to Reorder") },
        { _T("checkbox_column_editable"), _T("Editable Text") },
    };
    for (const auto& item : row2Checks) {
        ui::CheckBox* pCheck = new ui::CheckBox(this);
        pCheck->SetClass(_T("checkbox_1"));
        pCheck->SetName(item.name);
        pCheck->SetText(item.text);
        pCheck->SetAttribute(_T("margin"), _T("4,0,0,0"));
        pCheck->SetAttribute(_T("valign"), _T("center"));
        pRow2->AddItem(pCheck);
    }

    // Row 3: column-level CheckBox/icon
    ui::HBox* pRow3 = new ui::HBox(this);
    pRow3->SetAttribute(_T("minheight"), _T("18"));
    pRow3->SetBkColor(_T("bk_wnd_darkcolor"));
    pRow3->SetAttribute(_T("height"), _T("auto"));
    pRow3->SetAttribute(_T("margin"), _T("4,0,4,0"));
    pReportGroup->AddItem(pRow3);

    ui::Control* pW153 = new ui::Control(this);
    pW153->SetAttribute(_T("width"), _T("153"));
    pRow3->AddItem(pW153);

    pLine = new ui::Line(this);
    pLine->SetAttribute(_T("vertical"), _T("true"));
    pLine->SetAttribute(_T("margin"), _T("8,2,4,2"));
    pLine->SetAttribute(_T("width"), _T("2"));
    pRow3->AddItem(pLine);

    pLbl = new ui::Label(this);
    pLbl->SetText(_T("Column CheckBox:"));
    pLbl->SetAttribute(_T("valign"), _T("center"));
    pLbl->SetAttribute(_T("margin"), _T("2,0,2,0"));
    pLbl->SetToolTipText(_T("Each column header and cell can show a CheckBox"));
    pRow3->AddItem(pLbl);

    ui::CheckBox* pCheck = new ui::CheckBox(this);
    pCheck->SetClass(_T("checkbox_1"));
    pCheck->SetName(_T("checkbox_column_show_header_checkbox"));
    pCheck->SetText(_T("Show in Header"));
    pCheck->SetAttribute(_T("margin"), _T("4,0,0,0"));
    pCheck->SetAttribute(_T("valign"), _T("center"));
    pRow3->AddItem(pCheck);

    pCheck = new ui::CheckBox(this);
    pCheck->SetClass(_T("checkbox_1"));
    pCheck->SetName(_T("checkbox_column_show_checkbox"));
    pCheck->SetText(_T("Show in Each Column"));
    pCheck->SetAttribute(_T("margin"), _T("4,0,0,0"));
    pCheck->SetAttribute(_T("valign"), _T("center"));
    pRow3->AddItem(pCheck);

    pLine = new ui::Line(this);
    pLine->SetAttribute(_T("vertical"), _T("true"));
    pLine->SetAttribute(_T("margin"), _T("8,2,4,2"));
    pLine->SetAttribute(_T("width"), _T("2"));
    pRow3->AddItem(pLine);

    pLbl = new ui::Label(this);
    pLbl->SetText(_T("Column Icons:"));
    pLbl->SetAttribute(_T("valign"), _T("center"));
    pLbl->SetAttribute(_T("margin"), _T("2,0,2,0"));
    pLbl->SetToolTipText(_T("Each column header and cell can show an icon"));
    pRow3->AddItem(pLbl);

    pCheck = new ui::CheckBox(this);
    pCheck->SetClass(_T("checkbox_1"));
    pCheck->SetName(_T("checkbox_column_show_header_icon"));
    pCheck->SetText(_T("Show in Header"));
    pCheck->SetAttribute(_T("margin"), _T("4,0,0,0"));
    pCheck->SetAttribute(_T("valign"), _T("center"));
    pRow3->AddItem(pCheck);

    pCheck = new ui::CheckBox(this);
    pCheck->SetClass(_T("checkbox_1"));
    pCheck->SetName(_T("checkbox_column_show_icon"));
    pCheck->SetText(_T("Show in Each Column"));
    pCheck->SetAttribute(_T("margin"), _T("4,0,0,0"));
    pCheck->SetAttribute(_T("valign"), _T("center"));
    pRow3->AddItem(pCheck);

    // Row 4: header/table text alignment
    ui::HBox* pRow4 = new ui::HBox(this);
    pRow4->SetAttribute(_T("minheight"), _T("18"));
    pRow4->SetBkColor(_T("bk_wnd_darkcolor"));
    pRow4->SetAttribute(_T("height"), _T("auto"));
    pRow4->SetAttribute(_T("margin"), _T("4,0,4,0"));
    pReportGroup->AddItem(pRow4);

    pW153 = new ui::Control(this);
    pW153->SetAttribute(_T("width"), _T("153"));
    pRow4->AddItem(pW153);

    pLine = new ui::Line(this);
    pLine->SetAttribute(_T("vertical"), _T("true"));
    pLine->SetAttribute(_T("margin"), _T("8,8,4,8"));
    pLine->SetAttribute(_T("width"), _T("2"));
    pRow4->AddItem(pLine);

    pLbl = new ui::Label(this);
    pLbl->SetText(_T("Header Text:"));
    pLbl->SetAttribute(_T("valign"), _T("center"));
    pLbl->SetAttribute(_T("margin"), _T("6,0,2,0"));
    pRow4->AddItem(pLbl);

    struct AlignOpt { DString name; DString group; DString text; bool selected; };
    const AlignOpt headerAligns[] = {
        { _T("header_text_align_left"), _T("header_text_align"), _T("Left"), false },
        { _T("header_text_align_center"), _T("header_text_align"), _T("Center"), true },
        { _T("header_text_align_right"), _T("header_text_align"), _T("Right"), false },
    };
    for (const auto& item : headerAligns) {
        ui::Option* pAlign = new ui::Option(this);
        pAlign->SetClass(_T("option_2"));
        pAlign->SetName(item.name);
        pAlign->SetAttribute(_T("group"), item.group);
        pAlign->SetAttribute(_T("width"), _T("64"));
        pAlign->SetAttribute(_T("height"), _T("32"));
        pAlign->SetText(item.text);
        pAlign->SetAttribute(_T("padding"), _T("2,2,2,2"));
        pAlign->SetAttribute(_T("borderround"), _T("2,2"));
        pAlign->SetAttribute(_T("valign"), _T("center"));
        if (item.selected) pAlign->Selected(true);
        pRow4->AddItem(pAlign);
    }

    pLine = new ui::Line(this);
    pLine->SetAttribute(_T("vertical"), _T("true"));
    pLine->SetAttribute(_T("margin"), _T("8,8,4,8"));
    pLine->SetAttribute(_T("width"), _T("2"));
    pRow4->AddItem(pLine);

    pLbl = new ui::Label(this);
    pLbl->SetText(_T("Cell Text:"));
    pLbl->SetAttribute(_T("valign"), _T("center"));
    pLbl->SetAttribute(_T("margin"), _T("6,0,2,0"));
    pRow4->AddItem(pLbl);

    const AlignOpt columnAligns[] = {
        { _T("column_text_align_left"), _T("column_text_align"), _T("Left"), true },
        { _T("column_text_align_center"), _T("column_text_align"), _T("Center"), false },
        { _T("column_text_align_right"), _T("column_text_align"), _T("Right"), false },
    };
    for (const auto& item : columnAligns) {
        ui::Option* pAlign = new ui::Option(this);
        pAlign->SetClass(_T("option_2"));
        pAlign->SetName(item.name);
        pAlign->SetAttribute(_T("group"), item.group);
        pAlign->SetAttribute(_T("width"), _T("64"));
        pAlign->SetAttribute(_T("height"), _T("32"));
        pAlign->SetText(item.text);
        pAlign->SetAttribute(_T("padding"), _T("2,2,2,2"));
        pAlign->SetAttribute(_T("borderround"), _T("2,2"));
        pAlign->SetAttribute(_T("valign"), _T("center"));
        if (item.selected) pAlign->Selected(true);
        pRow4->AddItem(pAlign);
    }

    // Row 5: table properties (grid/row height)
    ui::HBox* pRow5 = new ui::HBox(this);
    pRow5->SetAttribute(_T("minheight"), _T("18"));
    pRow5->SetBkColor(_T("bk_wnd_darkcolor"));
    pRow5->SetAttribute(_T("height"), _T("auto"));
    pRow5->SetAttribute(_T("margin"), _T("4,0,4,0"));
    pReportGroup->AddItem(pRow5);

    pLbl = new ui::Label(this);
    pLbl->SetText(_T("Table Properties:"));
    pLbl->SetAttribute(_T("valign"), _T("center"));
    pLbl->SetAttribute(_T("margin"), _T("2,0,2,0"));
    pRow5->AddItem(pLbl);

    pLbl = new ui::Label(this);
    pLbl->SetText(_T("Horizontal Grid:"));
    pLbl->SetAttribute(_T("valign"), _T("center"));
    pLbl->SetAttribute(_T("margin"), _T("2,0,2,0"));
    pRow5->AddItem(pLbl);

    struct GridOpt { DString group; DString text; bool selected; };
    const GridOpt gridRow[] = {
        { _T("grid_line_row"), _T("Hide"), false },
        { _T("grid_line_row"), _T("Show"), true },
    };
    for (const auto& item : gridRow) {
        ui::Option* pGrid = new ui::Option(this);
        pGrid->SetClass(_T("option_2"));
        pGrid->SetAttribute(_T("group"), item.group);
        pGrid->SetAttribute(_T("width"), _T("64"));
        pGrid->SetAttribute(_T("height"), _T("32"));
        pGrid->SetText(item.text);
        pGrid->SetAttribute(_T("padding"), _T("2,2,2,2"));
        pGrid->SetAttribute(_T("borderround"), _T("2,2"));
        pGrid->SetAttribute(_T("valign"), _T("center"));
        if (item.selected) pGrid->Selected(true);
        pRow5->AddItem(pGrid);
    }

    pLine = new ui::Line(this);
    pLine->SetAttribute(_T("vertical"), _T("true"));
    pLine->SetAttribute(_T("margin"), _T("0,8,4,8"));
    pLine->SetAttribute(_T("width"), _T("2"));
    pRow5->AddItem(pLine);

    pLbl = new ui::Label(this);
    pLbl->SetText(_T("Vertical Grid:"));
    pLbl->SetAttribute(_T("valign"), _T("center"));
    pLbl->SetAttribute(_T("margin"), _T("2,0,2,0"));
    pRow5->AddItem(pLbl);

    const GridOpt gridColumn[] = {
        { _T("grid_line_column"), _T("Hide"), false },
        { _T("grid_line_column"), _T("Show"), true },
    };
    for (const auto& item : gridColumn) {
        ui::Option* pGrid = new ui::Option(this);
        pGrid->SetClass(_T("option_2"));
        pGrid->SetAttribute(_T("group"), item.group);
        pGrid->SetAttribute(_T("width"), _T("64"));
        pGrid->SetAttribute(_T("height"), _T("32"));
        pGrid->SetText(item.text);
        pGrid->SetAttribute(_T("padding"), _T("2,2,2,2"));
        pGrid->SetAttribute(_T("borderround"), _T("2,2"));
        pGrid->SetAttribute(_T("valign"), _T("center"));
        if (item.selected) pGrid->Selected(true);
        pRow5->AddItem(pGrid);
    }

    pLine = new ui::Line(this);
    pLine->SetAttribute(_T("vertical"), _T("true"));
    pLine->SetAttribute(_T("margin"), _T("0,8,4,8"));
    pLine->SetAttribute(_T("width"), _T("2"));
    pRow5->AddItem(pLine);

    pLbl = new ui::Label(this);
    pLbl->SetText(_T("Row Height:"));
    pLbl->SetAttribute(_T("valign"), _T("center"));
    pLbl->SetAttribute(_T("margin"), _T("2,0,2,0"));
    pRow5->AddItem(pLbl);

    ui::RichEdit* pItemHeight = new ui::RichEdit(this);
    pItemHeight->SetClass(_T("simple simple_border rich_edit_spin"));
    pItemHeight->SetName(_T("list_item_height_edit"));
    pItemHeight->SetAttribute(_T("min_number"), _T("0"));
    pItemHeight->SetAttribute(_T("max_number"), _T("512"));
    pItemHeight->SetAttribute(_T("limit_text"), _T("3"));
    pItemHeight->SetText(_T("0"));
    pItemHeight->SetAttribute(_T("margin"), _T("0,2,0,0"));
    pRow5->AddItem(pItemHeight);

    // Row 6: row-level CheckBox/icon
    ui::HBox* pRow6 = new ui::HBox(this);
    pRow6->SetAttribute(_T("minheight"), _T("18"));
    pRow6->SetBkColor(_T("bk_wnd_darkcolor"));
    pRow6->SetAttribute(_T("height"), _T("auto"));
    pRow6->SetAttribute(_T("margin"), _T("4,0,4,8"));
    pReportGroup->AddItem(pRow6);

    pW153 = new ui::Control(this);
    pW153->SetAttribute(_T("width"), _T("153"));
    pRow6->AddItem(pW153);

    pLine = new ui::Line(this);
    pLine->SetAttribute(_T("vertical"), _T("true"));
    pLine->SetAttribute(_T("margin"), _T("8,2,4,2"));
    pLine->SetAttribute(_T("width"), _T("2"));
    pRow6->AddItem(pLine);

    pLbl = new ui::Label(this);
    pLbl->SetText(_T("Row CheckBox:"));
    pLbl->SetAttribute(_T("valign"), _T("center"));
    pLbl->SetAttribute(_T("margin"), _T("2,0,2,0"));
    pLbl->SetToolTipText(_T("Each row header and row start can show a CheckBox"));
    pRow6->AddItem(pLbl);

    pCheck = new ui::CheckBox(this);
    pCheck->SetClass(_T("checkbox_1"));
    pCheck->SetName(_T("checkbox_show_header_checkbox"));
    pCheck->SetText(_T("Show in Header"));
    pCheck->SetAttribute(_T("margin"), _T("4,0,0,0"));
    pCheck->SetAttribute(_T("valign"), _T("center"));
    pRow6->AddItem(pCheck);

    pCheck = new ui::CheckBox(this);
    pCheck->SetClass(_T("checkbox_1"));
    pCheck->SetName(_T("checkbox_show_checkbox"));
    pCheck->SetText(_T("Show at Row Start"));
    pCheck->SetAttribute(_T("margin"), _T("4,0,0,0"));
    pCheck->SetAttribute(_T("valign"), _T("center"));
    pRow6->AddItem(pCheck);

    pLine = new ui::Line(this);
    pLine->SetAttribute(_T("vertical"), _T("true"));
    pLine->SetAttribute(_T("margin"), _T("8,2,4,2"));
    pLine->SetAttribute(_T("width"), _T("2"));
    pRow6->AddItem(pLine);

    pLbl = new ui::Label(this);
    pLbl->SetText(_T("Row Icons:"));
    pLbl->SetAttribute(_T("valign"), _T("center"));
    pLbl->SetAttribute(_T("margin"), _T("2,0,2,0"));
    pLbl->SetToolTipText(_T("Each row header and row start can show an icon"));
    pRow6->AddItem(pLbl);

    pCheck = new ui::CheckBox(this);
    pCheck->SetClass(_T("checkbox_1"));
    pCheck->SetName(_T("checkbox_show_icon"));
    pCheck->SetText(_T("Show at Row Start"));
    pCheck->SetAttribute(_T("margin"), _T("4,0,0,0"));
    pCheck->SetAttribute(_T("valign"), _T("center"));
    pRow6->AddItem(pCheck);

    // Other tests (Loading)
    ui::HBox* pOtherArea = new ui::HBox(this);
    pToolArea->AddItem(pOtherArea);

    ui::GroupVBox* pOther1 = new ui::GroupVBox(this);
    pOther1->SetAttribute(_T("text"), _T("Other Tests"));
    pOtherArea->AddItem(pOther1);

    struct LoadingBtn { DString name; DString text; int32_t marginTop; };
    const LoadingBtn loadingBtns1[] = {
        { _T("loading_progress_btn1"), _T("Loading Function Test (Progress Bar 1)"), 25 },
        { _T("loading_progress_btn2"), _T("Loading Function Test (Progress Bar 2)"), 4 },
        { _T("loading_btn1"), _T("Loading Function Test 1"), 4 },
        { _T("loading_btn2"), _T("Loading Function Test 2"), 4 },
    };
    for (const auto& item : loadingBtns1) {
        ui::Button* pBtn = new ui::Button(this);
        pBtn->SetClass(_T("btn_global_color_gray"));
        pBtn->SetName(item.name);
        pBtn->SetText(item.text);
        pBtn->SetAttribute(_T("width"), _T("200"));
        pBtn->SetAttribute(_T("height"), _T("30"));
        pBtn->SetAttribute(_T("border_round"), _T("3,3"));
        pBtn->SetAttribute(_T("margin"), ui::StringUtil::Printf(_T("20,%d,10,0"), item.marginTop));
        pOther1->AddItem(pBtn);
    }

    ui::GroupVBox* pOther2 = new ui::GroupVBox(this);
    pOther2->SetAttribute(_T("text"), _T("Other Tests"));
    pOtherArea->AddItem(pOther2);

    const LoadingBtn loadingBtns2[] = {
        { _T("loading_btn3"), _T("Loading Function Test 3"), 25 },
        { _T("loading_btn4"), _T("Loading Function Test 4"), 4 },
        { _T("loading_btn5"), _T("Loading Function Test 5"), 4 },
        { _T("loading_btn6"), _T("Loading Function Test 6"), 4 },
    };
    for (const auto& item : loadingBtns2) {
        ui::Button* pBtn = new ui::Button(this);
        pBtn->SetClass(_T("btn_global_color_gray"));
        pBtn->SetName(item.name);
        pBtn->SetText(item.text);
        pBtn->SetAttribute(_T("width"), _T("200"));
        pBtn->SetAttribute(_T("height"), _T("30"));
        pBtn->SetAttribute(_T("border_round"), _T("3,3"));
        pBtn->SetAttribute(_T("margin"), ui::StringUtil::Printf(_T("20,%d,10,0"), item.marginTop));
        pOther2->AddItem(pBtn);
    }

    // Splitter bar
    ui::Split* pSplit = new ui::Split(this);
    pSplit->SetBkColor(_T("splitline_level1"));
    pSplit->SetAttribute(_T("height"), _T("2"));
    pContent->AddItem(pSplit);

    // ListCtrl
    ui::VBox* pListArea = new ui::VBox(this);
    pListArea->SetAttribute(_T("margin"), _T("0,0,0,0"));
    pListArea->SetAttribute(_T("valign"), _T("center"));
    pListArea->SetAttribute(_T("halign"), _T("center"));
    pContent->AddItem(pListArea);

    ui::ListCtrl* pListCtrl = new ui::ListCtrl(this);
    pListCtrl->SetName(_T("list_ctrl"));
    pListCtrl->SetBkColor(_T("YellowGreen"));
    pListCtrl->SetAttribute(_T("type"), _T("report"));
    pListCtrl->SetAttribute(_T("show_header"), _T("true"));
    pListCtrl->SetAttribute(_T("header_class"), _T("list_ctrl_header"));
    pListCtrl->SetAttribute(_T("header_item_class"), _T("list_ctrl_header_item"));
    pListCtrl->SetAttribute(_T("header_split_box_class"), _T("list_ctrl_header_split_box"));
    pListCtrl->SetAttribute(_T("header_split_control_class"), _T("list_ctrl_header_split_control"));
    pListCtrl->SetAttribute(_T("header_height"), _T("32"));
    pListCtrl->SetAttribute(_T("enable_header_drag_order"), _T("true"));
    pListCtrl->SetAttribute(_T("check_box_class"), _T("list_ctrl_checkbox"));
    pListCtrl->SetAttribute(_T("data_item_class"), _T("list_ctrl_item"));
    pListCtrl->SetAttribute(_T("data_sub_item_class"), _T("list_ctrl_sub_item"));
    pListCtrl->SetAttribute(_T("report_view_class"), _T("list_ctrl_report_view"));
    pListCtrl->SetAttribute(_T("data_item_height"), _T("46"));
    pListCtrl->SetAttribute(_T("row_grid_line_width"), _T("1"));
    pListCtrl->SetAttribute(_T("row_grid_line_color"), _T("lightgray"));
    pListCtrl->SetAttribute(_T("column_grid_line_width"), _T("1"));
    pListCtrl->SetAttribute(_T("column_grid_line_color"), _T("lightgray"));
    pListCtrl->SetAttribute(_T("multi_select"), _T("true"));
    pListCtrl->SetAttribute(_T("auto_check_select"), _T("false"));
    pListCtrl->SetAttribute(_T("show_header_checkbox"), _T("true"));
    pListCtrl->SetAttribute(_T("show_data_item_checkbox"), _T("true"));
    pListCtrl->SetAttribute(_T("icon_view_class"), _T("list_ctrl_icon_view"));
    pListCtrl->SetAttribute(_T("icon_view_item_class"), _T("list_ctrl_icon_view_item"));
    pListCtrl->SetAttribute(_T("icon_view_item_image_class"), _T("list_ctrl_icon_view_item_image"));
    pListCtrl->SetAttribute(_T("icon_view_item_label_class"), _T("list_ctrl_icon_view_item_label"));
    pListCtrl->SetAttribute(_T("list_view_class"), _T("list_ctrl_list_view"));
    pListCtrl->SetAttribute(_T("list_view_item_class"), _T("list_ctrl_list_view_item"));
    pListCtrl->SetAttribute(_T("list_view_item_image_class"), _T("list_ctrl_list_view_item_image"));
    pListCtrl->SetAttribute(_T("list_view_item_label_class"), _T("list_ctrl_list_view_item_label"));
    pListCtrl->SetAttribute(_T("enable_item_edit"), _T("true"));
    pListCtrl->SetAttribute(_T("list_ctrl_richedit_class"), _T("list_ctrl_richedit"));
    pListCtrl->SetAttribute(_T("loading"), _T("file='loading_progress1.xml' width='0' height='0' offset_x='-1' offset_y='-1' valign='center' halign='center' fade='255' animation_control='loading_animation' auto_stop='true'"));
    pListArea->AddItem(pListCtrl);

    AttachBox(pRoot);
}


void MainForm::OnInitWindow()
{
    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);

    BuildUI();

    ui::ListCtrl* pListCtrl = dynamic_cast<ui::ListCtrl*>(FindControl(_T("list_ctrl")));
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
    uint32_t imageId = pReportImageList->AddImageString(_T("file='display-color.svg' width='22' height='22'"), Dpi());
    pListImageList->AddImageString(_T("file='display-color.svg' width='32' height='32' valign='center' halign='center'"), Dpi());
    pIconImageList->AddImageString(_T("file='display-color.svg' width='64' height='64' valign='center' halign='center'"), Dpi());

    // Fill data
    InsertItemData(400, 9, (int32_t)imageId);

    // Initialize the UI events related to this program's test features
    InitListCtrlEvents(pListCtrl);

    // Event binding, test event interfaces
    TestListCtrlEvents(pListCtrl);

    // Test the loading feature
    TestListCtrlLoading(pListCtrl);

    //pListCtrl->AttachViewSizeChanged([this, pListCtrl](const ui::EventArgs& args) {
    //    if (args.listCtrlType == (int32_t)ui::ListCtrlType::Report) {
    //        // When the view size changes, automatically adjust the column widths
    //        std::vector<ui::UiFixedInt> columnWidthList;
    //        size_t nColumnCount = pListCtrl->GetColumnCount();
    //        for (size_t nColumnIndex = 0; nColumnIndex < nColumnCount; nColumnIndex++) {
    //            columnWidthList.push_back(ui::UiFixedInt::MakeStretch());
    //        }
    //        bool bRet = pListCtrl->SetColumnWidth(columnWidthList, true);
    //        UNUSED_VARIABLE(bRet);
    //    }        
    //    return true;
    //    });
}

void MainForm::OnInitLayout()
{
    // Test automatically resizing column widths proportionally
    /*ui::ListCtrl* pListCtrl = dynamic_cast<ui::ListCtrl*>(FindControl(_T("list_ctrl")));
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
    ui::Combo* pTypeCombo = dynamic_cast<ui::Combo*>(FindControl(_T("list_ctrl_type_combo")));
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
    ui::RichEdit* pHeaderHeightEdit = dynamic_cast<ui::RichEdit*>(FindControl(_T("header_height_edit")));
    if (pHeaderHeightEdit != nullptr) {
        pHeaderHeightEdit->SetText(ui::StringUtil::Printf(_T("%d"), pListCtrl->GetHeaderHeight()));
        pHeaderHeightEdit->AttachTextChanged([this, pHeaderHeightEdit, pListCtrl](const ui::EventArgs&) {
            int32_t height = ui::StringUtil::StringToInt32(pHeaderHeightEdit->GetText());
            if (height >= 0) {
                pListCtrl->SetHeaderHeight(height, false);
            }
            return true;
            });
    }

    // Row height control
    ui::RichEdit* pItemHeightEdit = dynamic_cast<ui::RichEdit*>(FindControl(_T("list_item_height_edit")));
    if (pItemHeightEdit != nullptr) {
        pItemHeightEdit->SetText(ui::StringUtil::Printf(_T("%d"), pListCtrl->GetDataItemHeight()));
        pItemHeightEdit->AttachTextChanged([this, pItemHeightEdit, pListCtrl](const ui::EventArgs&) {
            int32_t height = ui::StringUtil::StringToInt32(pItemHeightEdit->GetText());
            if (height >= 0) {
                pListCtrl->SetDataItemHeight(height, false);
            }
            return true;
            });
    }

    // Column controls
    ui::Combo* pColumnCombo = dynamic_cast<ui::Combo*>(FindControl(_T("column_combo")));
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

    ui::CheckBox* pColumnShow = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_column_show")));
    ui::CheckBox* pColumnWidth = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_column_width")));
    ui::CheckBox* pColumnSort = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_column_sort")));
    ui::CheckBox* pColumnIcon = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_column_icon_at_top")));
    ui::CheckBox* pColumnDragOrder = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_column_drag_order")));
    ui::CheckBox* pColumnEditable = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_column_editable")));
    ui::CheckBox* pColumnHeaderCheckBox = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_column_show_header_checkbox")));
    ui::CheckBox* pColumnShowCheckBox = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_column_show_checkbox")));

    ui::CheckBox* pColumnHeaderIcon = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_column_show_header_icon")));
    ui::CheckBox* pColumnShowIcon = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_column_show_icon")));

    ui::Option* pColumnHeaderTextAlignLeft = dynamic_cast<ui::Option*>(FindControl(_T("header_text_align_left")));
    ui::Option* pColumnHeaderTextAlignCenter = dynamic_cast<ui::Option*>(FindControl(_T("header_text_align_center")));
    ui::Option* pColumnHeaderTextAlignRight = dynamic_cast<ui::Option*>(FindControl(_T("header_text_align_right")));

    ui::Option* pColumnTextAlignLeft = dynamic_cast<ui::Option*>(FindControl(_T("column_text_align_left")));
    ui::Option* pColumnTextAlignCenter = dynamic_cast<ui::Option*>(FindControl(_T("column_text_align_center")));
    ui::Option* pColumnTextAlignRight = dynamic_cast<ui::Option*>(FindControl(_T("column_text_align_right")));

    ui::CheckBox* pHeaderCheckBox = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_show_header_checkbox")));
    ui::CheckBox* pShowCheckBox = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_show_checkbox")));

    ui::CheckBox* pShowIcon = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_show_icon")));

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
    ui::CheckBox* pMultiSelect = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_multi_select")));
    if (pMultiSelect != nullptr) {
        pMultiSelect->Selected(pListCtrl->IsMultiSelect(), false);
    }
    // Right-click on the list header
    ui::ListCtrlHeader* pHeaderCtrl = pListCtrl->GetHeaderCtrl();
    if (pHeaderCtrl != nullptr) {
        pHeaderCtrl->AttachRClick([this](const ui::EventArgs&) {
#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
            if (::MessageBox(nullptr, _T("ListCtrlHeader RClick! Run function test?"), _T(""), MB_YESNO) == IDYES) {
                RunListCtrlTest();
            }
#else
            ui::SystemUtil::ShowMessageBox(this, _T("Start Function Test"), _T("ListCtrlHeader RClick!"));
            RunListCtrlTest();
#endif
            return true;
            });
    }

    // Set each column's width proportionally to fill the entire view
    ui::Button* pAutoStretchBtn = dynamic_cast<ui::Button*>(FindControl(_T("set_column_stretch")));
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
    ui::Button* pLoadingBtn = dynamic_cast<ui::Button*>(FindControl(_T("loading_progress_btn1")));
    if (pLoadingBtn != nullptr) {
        pLoadingBtn->AttachClick([pListCtrl, this](const ui::EventArgs& args) {
            if (!pListCtrl->IsLoading()) {
                pListCtrl->SetLoadingAttribute(_T("file='loading_progress1.xml' width='0' height='0' offset_x='-1' offset_y='-1' valign='center' halign='center' fade='255' animation_control='loading_animation' auto_stop='true'"));
                OnTestLoadingProgress();
            }
            return true;
            });
    }

    pLoadingBtn = dynamic_cast<ui::Button*>(FindControl(_T("loading_progress_btn2")));
    if (pLoadingBtn != nullptr) {
        pLoadingBtn->AttachClick([pListCtrl, this](const ui::EventArgs& args) {
            if (!pListCtrl->IsLoading()) {
                pListCtrl->SetLoadingAttribute(_T("file='loading_progress2.xml' width='0' height='0' offset_x='-1' offset_y='-1' valign='center' halign='center' fade='255' animation_control='loading_animation' auto_stop='true'"));
                OnTestLoadingProgress();
            }
            return true;
            });
    }

    pLoadingBtn = dynamic_cast<ui::Button*>(FindControl(_T("loading_btn1")));
    if (pLoadingBtn != nullptr) {
        pLoadingBtn->AttachClick([pListCtrl, this](const ui::EventArgs& args) {
            if (!pListCtrl->IsLoading()) {
                pListCtrl->SetLoadingAttribute(_T("file='loading1.xml' width='0' height='0' offset_x='-1' offset_y='-1' valign='center' halign='center' fade='255' animation_control='loading_animation' auto_stop='true'"));
                // In real applications, event handling can refer to the logic of OnTestLoadingProgress
                pListCtrl->StartLoading(100, -1);
            }
            return true;
            });
    }
    pLoadingBtn = dynamic_cast<ui::Button*>(FindControl(_T("loading_btn2")));
    if (pLoadingBtn != nullptr) {
        pLoadingBtn->AttachClick([pListCtrl, this](const ui::EventArgs& args) {
            if (!pListCtrl->IsLoading()) {
                pListCtrl->SetLoadingAttribute(_T("file='loading2.xml' width='0' height='0' offset_x='-1' offset_y='-1' valign='center' halign='center' fade='255' animation_control='loading_animation' auto_stop='true'"));
                // In real applications, event handling can refer to the logic of OnTestLoadingProgress
                pListCtrl->StartLoading(100, -1);
            }
            return true;
            });
    }
    pLoadingBtn = dynamic_cast<ui::Button*>(FindControl(_T("loading_btn3")));
    if (pLoadingBtn != nullptr) {
        pLoadingBtn->AttachClick([pListCtrl, this](const ui::EventArgs& args) {
            if (!pListCtrl->IsLoading()) {
                pListCtrl->SetLoadingAttribute(_T("file='loading3.xml' width='0' height='0' offset_x='-1' offset_y='-1' valign='center' halign='center' fade='255' animation_control='loading_animation' auto_stop='true'"));
                // In real applications, event handling can refer to the logic of OnTestLoadingProgress
                pListCtrl->StartLoading(100, -1);
            }
            return true;
            });
    }
    pLoadingBtn = dynamic_cast<ui::Button*>(FindControl(_T("loading_btn4")));
    if (pLoadingBtn != nullptr) {
        pLoadingBtn->AttachClick([pListCtrl, this](const ui::EventArgs& args) {
            if (!pListCtrl->IsLoading()) {
                pListCtrl->SetLoadingAttribute(_T("file='loading4.xml' width='0' height='0' offset_x='-1' offset_y='-1' valign='center' halign='center' fade='255' animation_control='loading_animation' auto_stop='true'"));
                // In real applications, event handling can refer to the logic of OnTestLoadingProgress
                pListCtrl->StartLoading(100, -1);
            }
            return true;
            });
    }
    pLoadingBtn = dynamic_cast<ui::Button*>(FindControl(_T("loading_btn5")));
    if (pLoadingBtn != nullptr) {
        pLoadingBtn->AttachClick([pListCtrl, this](const ui::EventArgs& args) {
            if (!pListCtrl->IsLoading()) {
                pListCtrl->SetLoadingAttribute(_T("file='loading5.xml' width='0' height='0' offset_x='-1' offset_y='-1' valign='center' halign='center' fade='255' animation_control='loading_animation' auto_stop='true'"));
                // In real applications, event handling can refer to the logic of OnTestLoadingProgress
                pListCtrl->StartLoading(100, -1);
            }
            return true;
            });
    }
    pLoadingBtn = dynamic_cast<ui::Button*>(FindControl(_T("loading_btn6")));
    if (pLoadingBtn != nullptr) {
        pLoadingBtn->AttachClick([pListCtrl, this](const ui::EventArgs& args) {
            if (!pListCtrl->IsLoading()) {
                pListCtrl->SetLoadingAttribute(_T("file='loading6.xml' width='0' height='0' offset_x='-1' offset_y='-1' valign='center' halign='center' fade='255' animation_control='loading_animation' auto_stop='true'"));
                // In real applications, event handling can refer to the logic of OnTestLoadingProgress
                pListCtrl->StartLoading(100, -1);
            }
            return true;
            });
    }
}

void MainForm::OnTestLoadingProgress()
{
    ui::ListCtrl* pListCtrl = dynamic_cast<ui::ListCtrl*>(FindControl(_T("list_ctrl")));
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
    ui::ListCtrl* pListCtrl = dynamic_cast<ui::ListCtrl*>(FindControl(_T("list_ctrl")));
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


    ui::CheckBox* pColumnShow = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_column_show")));
    ui::CheckBox* pColumnWidth = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_column_width")));
    ui::CheckBox* pColumnSort = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_column_sort")));
    ui::CheckBox* pColumnIcon = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_column_icon_at_top")));
    ui::CheckBox* pColumnDragOrder = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_column_drag_order")));
    ui::CheckBox* pColumnEditable = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_column_editable")));
    ui::CheckBox* pColumnHeaderCheckBox = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_column_show_header_checkbox")));
    ui::CheckBox* pColumnShowCheckBox = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_column_show_checkbox")));

    ui::CheckBox* pColumnHeaderIcon = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_column_show_header_icon")));
    ui::CheckBox* pColumnShowIcon = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_column_show_icon")));

    ui::Option* pColumnHeaderTextAlignLeft = dynamic_cast<ui::Option*>(FindControl(_T("header_text_align_left")));
    ui::Option* pColumnHeaderTextAlignCenter = dynamic_cast<ui::Option*>(FindControl(_T("header_text_align_center")));
    ui::Option* pColumnHeaderTextAlignRight = dynamic_cast<ui::Option*>(FindControl(_T("header_text_align_right")));

    ui::Option* pColumnTextAlignLeft = dynamic_cast<ui::Option*>(FindControl(_T("column_text_align_left")));
    ui::Option* pColumnTextAlignCenter = dynamic_cast<ui::Option*>(FindControl(_T("column_text_align_center")));
    ui::Option* pColumnTextAlignRight = dynamic_cast<ui::Option*>(FindControl(_T("column_text_align_right")));

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
    ui::ListCtrl* pListCtrl = dynamic_cast<ui::ListCtrl*>(FindControl(_T("list_ctrl")));
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
        columnInfo.text = ui::StringUtil::Printf(_T("Column %d"), i);
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
            subItemData.text = ui::StringUtil::Printf(_T("Row %03d / Column %02d"), itemIndex, columnIndex);
            subItemData.bShowCheckBox = bShowCheckBox;
            subItemData.nImageId = nImageId;
            if (columnIndex == 0) {
                subItemData.text += _T("-test1234567890-test1234567890-test1234567890-test1234567890");
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
    ui::ListCtrl* pListCtrl = dynamic_cast<ui::ListCtrl*>(FindControl(_T("list_ctrl")));
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
    const DString text = _T("1");
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
    subItemData.text = _T("3");
    subItemData.textColor = ui::UiColor(ui::UiColors::Crimson);
    subItemData.bkColor = ui::UiColor(ui::UiColors::BlanchedAlmond);
    subItemData.bShowCheckBox = false;
    subItemData.nImageId = 123;
    subItemData.nTextFormat = ui::TEXT_HCENTER | ui::TEXT_VCENTER;
    pListCtrl->SetSubItemData(nDataItemIndex, nColumnIndex, subItemData);

    ui::ListCtrlSubItemData dataItem2;
    pListCtrl->GetSubItemData(nDataItemIndex, nColumnIndex, dataItem2);
    ASSERT(subItemData.text == DString(dataItem2.text));
    ASSERT(subItemData.textColor == dataItem2.textColor);
    ASSERT(subItemData.bkColor == dataItem2.bkColor);
    ASSERT(subItemData.bShowCheckBox == dataItem2.bShowCheckBox);
    ASSERT(subItemData.nImageId == dataItem2.nImageId);
    ASSERT(subItemData.nTextFormat == dataItem2.nTextFormat);

    ASSERT(pListCtrl->GetSubItemText(nDataItemIndex, nColumnIndex) == _T("3"));

    subItemData.text = _T("2");
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

    subItemData.text = _T("3");
    nColumnIndex = 0;
    pListCtrl->InsertDataItem(nDataItemIndex, subItemData);
    ASSERT(pListCtrl->GetSubItemText(nDataItemIndex, nColumnIndex) == _T("3"));
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
    DString text60 = pListCtrl->GetSubItemText(60, 0);
    pListCtrl->SetDataItemSelected(60, true);
    ASSERT(pListCtrl->IsDataItemSelected(60));

    ui::ListCtrlSubItemData dataItem3;
    nColumnIndex = 0;
    dataItem3.text = _T("Test");
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
        DString sInfo = GetEventDisplayInfo(args);
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
        DString sInfo = GetItemFilledEventDisplayInfo(args);
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

DString MainForm::GetEventDisplayInfo(const ui::EventArgs& args)
{
    DString sInfo = ui::EventUtils::EventTypeToString(args.eventType);
    while (sInfo.size() < 24) {
        sInfo += _T(" ");
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
        DString labelText;
        int32_t nDataItemIndex = -1;
        int32_t nDataColumnIndex = -1;
        ui::ListCtrlType listCtrlType = (ui::ListCtrlType)args.listCtrlType;
        if (listCtrlType == ui::ListCtrlType::Report) {
            sInfo += _T("ListCtrlType::Report: ");
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
            sInfo += _T("ListCtrlType::Icon: ");
            ui::ListCtrlIconViewItem* pItem = (ui::ListCtrlIconViewItem*)args.pEventData;
            if (pItem != nullptr) {
                nDataItemIndex = (int32_t)pItem->GetDataItemIndex();
                labelText = pItem->GetLabelText();
            }
        }
        else if (listCtrlType == ui::ListCtrlType::List) {
            sInfo += _T("ListCtrlType::List: ");
            ui::ListCtrlListViewItem* pItem = (ui::ListCtrlListViewItem*)args.pEventData;
            if (pItem != nullptr) {
                nDataItemIndex = (int32_t)pItem->GetDataItemIndex();
                labelText = pItem->GetLabelText();
            }
        }
        else {
            sInfo += _T("ListCtrl: ");
        }
        if (nDataItemIndex >= 0) {
            if ((args.eventType >= ui::kEventKeyBegin) && (args.eventType <= ui::kEventKeyEnd)) {
                // Keyboard message
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
            }
            if (labelText.empty()) {
                if (nDataColumnIndex >= 0) {
                    sInfo += ui::StringUtil::Printf(_T("nDataItemIndex=%d, nDataColumnIndex=%d"), nDataItemIndex, nDataColumnIndex);
                }
                else {
                    sInfo += ui::StringUtil::Printf(_T("nDataItemIndex=%d"), nDataItemIndex);
                }
            }
            else {
                if (nDataColumnIndex >= 0) {
                    sInfo += ui::StringUtil::Printf(_T("nDataItemIndex=%d, nDataColumnIndex=%d, LabelText='%s'"), nDataItemIndex, nDataColumnIndex, labelText.c_str());
                }
                else {
                    sInfo += ui::StringUtil::Printf(_T("nDataItemIndex=%d, LabelText='%s'"), nDataItemIndex, labelText.c_str());
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
            sInfo += _T("ListCtrlType::Report: ");
            ui::ListCtrlReportView* pView = (ui::ListCtrlReportView*)args.pEventData;
            pControl = dynamic_cast<ui::Control*>(pView);
        }
        else if (listCtrlType == ui::ListCtrlType::Icon) {
            sInfo += _T("ListCtrlType::Icon: ");
            ui::ListCtrlIconView* pView = (ui::ListCtrlIconView*)args.pEventData;
            pControl = dynamic_cast<ui::Control*>(pView);
        }
        else if (listCtrlType == ui::ListCtrlType::List) {
            sInfo += _T("ListCtrlType::List: ");
            ui::ListCtrlListView* pView = (ui::ListCtrlListView*)args.pEventData;
            pControl = dynamic_cast<ui::Control*>(pView);
        }
        ASSERT(pControl != nullptr);
        if (args.eventType == ui::kEventViewTypeChanged) {
            // Not shown
            sInfo += _T("ViewTypeChanged");
        }
        else if (args.eventType == ui::kEventViewPosChanged) {
            sInfo += ui::StringUtil::Printf(_T("left:%d, top: %d"), pControl->GetRect().left, pControl->GetRect().top);
        }
        else if (args.eventType == ui::kEventViewSizeChanged) {
            sInfo += ui::StringUtil::Printf(_T("width:%d, height: %d"), pControl->GetRect().Width(), pControl->GetRect().Height());
        }
    }
    else {
        ASSERT(0);
    }
   
    sInfo += _T("\n");
    return sInfo;
}

DString MainForm::GetItemFilledEventDisplayInfo(const ui::EventArgs& args)
{
    DString sInfo = ui::EventUtils::EventTypeToString(args.eventType);
    while (sInfo.size() < 32) {
        sInfo += _T(" ");
    }

    sInfo += ui::StringUtil::Printf(_T("ListBoxItemIndex=%zu "), (size_t)args.wParam);
    sInfo += ui::StringUtil::Printf(_T("DataItemIndex=%zu "), (size_t)args.lParam);

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
            sInfo += ui::StringUtil::Printf(_T("DataColumnIndex='%zu' "), pSubItem->GetDataColumnIndex());
            sInfo += ui::StringUtil::Printf(_T("LabelText='%s' "), pSubItem->GetText().c_str());
        }
    }
    else if (args.eventType == ui::kEventListViewItemFilled) {
        ui::ListCtrlListViewItem* pItem = (ui::ListCtrlListViewItem*)args.pEventData;
        ASSERT(pItem != nullptr);
        if (pItem != nullptr) {
            ASSERT(pItem->GetListBoxIndex() == (size_t)args.wParam);
            ASSERT(pItem->GetDataItemIndex() == (size_t)args.lParam);
            sInfo += ui::StringUtil::Printf(_T("LabelText='%s'"), pItem->GetLabelText().c_str());
        }
    }
    else if (args.eventType == ui::kEventIconViewItemFilled) {
        ui::ListCtrlIconViewItem* pItem = (ui::ListCtrlIconViewItem*)args.pEventData;
        ASSERT(pItem != nullptr);
        if (pItem != nullptr) {
            ASSERT(pItem->GetListBoxIndex() == (size_t)args.wParam);
            ASSERT(pItem->GetDataItemIndex() == (size_t)args.lParam);
            sInfo += ui::StringUtil::Printf(_T("LabelText='%s'"), pItem->GetLabelText().c_str());
        }
    }
    return sInfo;
}

void MainForm::OutputDebugLog(const DString& logMsg)
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
        pHoverButton->SetClass(_T("btn_recycle"));
        pHoverButton->SetAttribute(_T("width"), _T("auto"));
        pHoverButton->SetAttribute(_T("height"), _T("auto"));
        pHoverButton->SetAttribute(_T("halign"), _T("right"));
        pHoverButton->SetAttribute(_T("valign"), _T("top"));
        pHoverButton->SetAttribute(_T("margin"), _T("0,8,8,0"));
        pHoverButton->SetToolTipText(_T("Hover Button"));

        // Floating button
        pHoverButton->SetFloat(true);
        pHoverButton->SetVisible(false);
        pSubItem->AddItem(pHoverButton);

        // Current row and column information
        size_t nDataItemIndex = pSubItem->GetDataItemIndex();
        size_t nDataColumnIndex = pSubItem->GetDataColumnIndex();

        // Bind events
        pHoverButton->AttachClick([this, nDataItemIndex, nDataColumnIndex](const ui::EventArgs& /*args*/){
            DString title = _T("Hover Button Clicked");
            DString content = ui::StringUtil::Printf(_T("DataItemIndex:%zu, DataColumnIndex:%zu"), nDataItemIndex, nDataColumnIndex);
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
