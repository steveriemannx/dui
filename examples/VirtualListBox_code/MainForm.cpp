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
    pRoot->SetBkColor(DUI_T("bk_wnd_darkcolor"));

    // Title bar area
    auto* pCaption = ui::Create<ui::HBox>(this, {{DUI_T("name"), DUI_T("window_caption_bar")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("36")}});
    pCaption->SetBkColor(DUI_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    auto* pTitle = ui::Create<ui::Label>(this, {{DUI_T("font"), DUI_T("system_14")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8")}, {DUI_T("mouse_enabled"), DUI_T("false")}});
    pTitle->SetText(DUI_T("Virtual List (VirtualHTileListBox | VirtualVTileListBox | VirtualHListBox | VirtualVListBox)"));
    pCaption->AddItem(pTitle);

    auto* pSpacer = ui::Create<ui::Control>(this, {{DUI_T("mouse_enabled"), DUI_T("false")}});
    pCaption->AddItem(pSpacer);

    auto* pMinBtn = ui::Create<ui::Button>(this, {{DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,2,0,2")}});
    pMinBtn->SetClass(DUI_T("btn_wnd_min_11"));
    pMinBtn->SetName(DUI_T("minbtn"));
    pMinBtn->SetToolTipText(DUI_T("Minimize"));
    pCaption->AddItem(pMinBtn);

    auto* pMaxBox = ui::Create<ui::Box>(this, {{DUI_T("height"), DUI_T("stretch")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,2,0,2")}});
    pCaption->AddItem(pMaxBox);

    auto* pMaxBtn = ui::Create<ui::Button>(this, {{DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("stretch")}});
    pMaxBtn->SetClass(DUI_T("btn_wnd_max_11"));
    pMaxBtn->SetName(DUI_T("maxbtn"));
    pMaxBtn->SetToolTipText(DUI_T("Maximize"));
    pMaxBox->AddItem(pMaxBtn);

    auto* pRestoreBtn = ui::Create<ui::Button>(this, {{DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("stretch")}});
    pRestoreBtn->SetClass(DUI_T("btn_wnd_restore_11"));
    pRestoreBtn->SetName(DUI_T("restorebtn"));
    pRestoreBtn->SetVisible(false);
    pRestoreBtn->SetToolTipText(DUI_T("Restore"));
    pMaxBox->AddItem(pRestoreBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{DUI_T("height"), DUI_T("stretch")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,0,0,2")}});
    pCloseBtn->SetClass(DUI_T("btn_wnd_close_11"));
    pCloseBtn->SetName(DUI_T("closebtn"));
    pCloseBtn->SetToolTipText(DUI_T("Close"));
    pCaption->AddItem(pCloseBtn);

    // Virtual list type display
    auto* pTypeRow = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("30")}});
    pRoot->AddItem(pTypeRow);

    auto* pTypeLabel = ui::Create<ui::Label>(this, {{DUI_T("font"), DUI_T("system_14")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,8,0")}});
    pTypeLabel->SetText(DUI_T("Current virtual list container type:"));
    pTypeRow->AddItem(pTypeLabel);

    auto* pListType = ui::Create<ui::Label>(this, {{DUI_T("font"), DUI_T("system_14")}, {DUI_T("valign"), DUI_T("center")}});
    pListType->SetName(DUI_T("list_box_type"));
    pListType->SetText(DUI_T("VirtualHTileListBox"));
    pTypeRow->AddItem(pListType);

    // Main content area
    auto* pMain = ui::Create<ui::HBox>(this, {{DUI_T("child_margin"), DUI_T("5")}, {DUI_T("padding"), DUI_T("5,5,5,10")}});
    pRoot->AddItem(pMain);

    // Left settings panel
    auto* pSettings = ui::Create<ui::VBox>(this, {{DUI_T("width"), DUI_T("300")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("splitline_level1")}, {DUI_T("padding"), DUI_T("5,10,5,10")}, {DUI_T("child_margin"), DUI_T("10")}});
    pMain->AddItem(pSettings);

    // Row/column settings
    auto* pColumnGroup = ui::Create<ui::VBox>(this, {{DUI_T("height"), DUI_T("80")}});
    pSettings->AddItem(pColumnGroup);

    auto* pColumnLabel = ui::Create<ui::Label>(this, {{DUI_T("font"), DUI_T("system_bold_14")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("valign"), DUI_T("center")}});
    pColumnLabel->SetName(DUI_T("label_column_row"));
    pColumnLabel->SetText(DUI_T("Rows/Columns:"));
    pColumnGroup->AddItem(pColumnLabel);

    auto* pOptionColumnAuto = ui::Create<ui::Option>(this, {{DUI_T("group"), DUI_T("option_group")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("margin"), DUI_T("80,0,0,0")}});
    pOptionColumnAuto->SetClass(DUI_T("option_1"));
    pOptionColumnAuto->SetName(DUI_T("option_column_auto"));
    pOptionColumnAuto->SetText(DUI_T("Auto Calculate"));
    pOptionColumnAuto->Selected(true);
    pColumnGroup->AddItem(pOptionColumnAuto);

    auto* pColumnFixRow = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("30")}});
    pColumnGroup->AddItem(pColumnFixRow);

    auto* pOptionColumnFix = ui::Create<ui::Option>(this, {{DUI_T("group"), DUI_T("option_group")}, {DUI_T("margin"), DUI_T("80,0,0,0")}});
    pOptionColumnFix->SetClass(DUI_T("option_1"));
    pOptionColumnFix->SetName(DUI_T("option_column_fix"));
    pOptionColumnFix->SetText(DUI_T("Fixed Columns"));
    pColumnFixRow->AddItem(pOptionColumnFix);

    auto* pEditColumn = ui::Create<ui::RichEdit>(this, {{DUI_T("min_number"), DUI_T("1")}, {DUI_T("max_number"), DUI_T("12")}, {DUI_T("text_padding"), DUI_T("1,0,8,0")}, {DUI_T("width"), DUI_T("50")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("margin"), DUI_T("6,0,0,0")}, {DUI_T("default_context_menu"), DUI_T("true")}, {DUI_T("number_only"), DUI_T("true")}, {DUI_T("limit_text"), DUI_T("3")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("light_gray")}});
    pEditColumn->SetClass(DUI_T("simple rich_edit_spin"));
    pEditColumn->SetName(DUI_T("edit_column"));
    pEditColumn->SetText(DUI_T("3"));
    pEditColumn->SetVisible(false);
    pEditColumn->SetBkColor(DUI_T("white"));
    pColumnFixRow->AddItem(pEditColumn);

    auto* pSplit1 = ui::Create<ui::Control>(this, {{DUI_T("height"), DUI_T("1")}});
    pSplit1->SetBkColor(DUI_T("splitline_level1"));
    pSettings->AddItem(pSplit1);

    // Alignment
    auto* pAlignGroup = ui::Create<ui::VBox>(this, {{DUI_T("height"), DUI_T("auto")}});
    pSettings->AddItem(pAlignGroup);

    auto* pAlignLabel = ui::Create<ui::Label>(this, {{DUI_T("font"), DUI_T("system_bold_14")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("valign"), DUI_T("center")}});
    pAlignLabel->SetText(DUI_T("Alignment:"));
    pAlignGroup->AddItem(pAlignLabel);

    auto* pOptionAlign1 = ui::Create<ui::Option>(this, {{DUI_T("group"), DUI_T("option_group_align")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("margin"), DUI_T("80,0,0,0")}});
    pOptionAlign1->SetClass(DUI_T("option_1"));
    pOptionAlign1->SetName(DUI_T("option_align1"));
    pOptionAlign1->SetText(DUI_T("Align Left (left)"));
    pOptionAlign1->Selected(true);
    pAlignGroup->AddItem(pOptionAlign1);

    auto* pOptionAlign2 = ui::Create<ui::Option>(this, {{DUI_T("group"), DUI_T("option_group_align")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("margin"), DUI_T("80,0,0,0")}});
    pOptionAlign2->SetClass(DUI_T("option_1"));
    pOptionAlign2->SetName(DUI_T("option_align2"));
    pOptionAlign2->SetText(DUI_T("Align Center (center)"));
    pAlignGroup->AddItem(pOptionAlign2);

    auto* pOptionAlign3 = ui::Create<ui::Option>(this, {{DUI_T("group"), DUI_T("option_group_align")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("margin"), DUI_T("80,0,0,0")}});
    pOptionAlign3->SetClass(DUI_T("option_1"));
    pOptionAlign3->SetName(DUI_T("option_align3"));
    pOptionAlign3->SetText(DUI_T("Align Right (right)"));
    pAlignGroup->AddItem(pOptionAlign3);

    auto* pSplit2 = ui::Create<ui::Control>(this, {{DUI_T("height"), DUI_T("1")}});
    pSplit2->SetBkColor(DUI_T("splitline_level1"));
    pSettings->AddItem(pSplit2);

    // Child spacing
    auto* pMarginGroup = ui::Create<ui::VBox>(this, {{DUI_T("height"), DUI_T("auto")}});
    pSettings->AddItem(pMarginGroup);

    auto* pMarginXRow = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("30")}, {DUI_T("child_margin"), DUI_T("10")}});
    pMarginGroup->AddItem(pMarginXRow);

    auto* pMarginXLabel = ui::Create<ui::Label>(this, {{DUI_T("font"), DUI_T("system_bold_14")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("valign"), DUI_T("center")}});
    pMarginXLabel->SetName(DUI_T("label_child_margin_x"));
    pMarginXLabel->SetText(DUI_T("Child Margin X:"));
    pMarginXRow->AddItem(pMarginXLabel);

    auto* pMarginXBox = ui::Create<ui::Box>(this, {{DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("splitline_level1")}, {DUI_T("width"), DUI_T("90")}, {DUI_T("height"), DUI_T("26")}, {DUI_T("valign"), DUI_T("center")}});
    pMarginXRow->AddItem(pMarginXBox);

    auto* pEditMarginX = ui::Create<ui::RichEdit>(this, {{DUI_T("font"), DUI_T("system_14")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("normal_text_color"), DUI_T("black")}, {DUI_T("multi_line"), DUI_T("false")}, {DUI_T("prompt_mode"), DUI_T("true")}, {DUI_T("prompt_text"), DUI_T("Child Margin X")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("text_padding"), DUI_T("2,0,0,0")}, {DUI_T("prompt_color"), DUI_T("splitline_level1")}, {DUI_T("auto_hscroll"), DUI_T("true")}});
    pEditMarginX->SetName(DUI_T("edit_child_margin_x"));
    pEditMarginX->SetBkColor(DUI_T("white"));
    pMarginXBox->AddItem(pEditMarginX);

    auto* pMarginYRow = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("30")}, {DUI_T("child_margin"), DUI_T("10")}});
    pMarginGroup->AddItem(pMarginYRow);

    auto* pMarginYLabel = ui::Create<ui::Label>(this, {{DUI_T("font"), DUI_T("system_bold_14")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("valign"), DUI_T("center")}});
    pMarginYLabel->SetName(DUI_T("label_child_margin_y"));
    pMarginYLabel->SetText(DUI_T("Child Margin Y:"));
    pMarginYRow->AddItem(pMarginYLabel);

    auto* pMarginYBox = ui::Create<ui::Box>(this, {{DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("splitline_level1")}, {DUI_T("width"), DUI_T("90")}, {DUI_T("height"), DUI_T("26")}, {DUI_T("valign"), DUI_T("center")}});
    pMarginYRow->AddItem(pMarginYBox);

    auto* pEditMarginY = ui::Create<ui::RichEdit>(this, {{DUI_T("font"), DUI_T("system_14")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("normal_text_color"), DUI_T("black")}, {DUI_T("multi_line"), DUI_T("false")}, {DUI_T("prompt_mode"), DUI_T("true")}, {DUI_T("prompt_text"), DUI_T("Child Margin Y")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("text_padding"), DUI_T("2,0,0,0")}, {DUI_T("prompt_color"), DUI_T("splitline_level1")}, {DUI_T("auto_hscroll"), DUI_T("true")}});
    pEditMarginY->SetName(DUI_T("edit_child_margin_y"));
    pEditMarginY->SetBkColor(DUI_T("white"));
    pMarginYBox->AddItem(pEditMarginY);

    // Total data
    auto* pTotalRow = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("30")}, {DUI_T("child_margin"), DUI_T("10")}});
    pSettings->AddItem(pTotalRow);

    auto* pTotalLabel = ui::Create<ui::Label>(this, {{DUI_T("font"), DUI_T("system_bold_14")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("valign"), DUI_T("center")}});
    pTotalLabel->SetText(DUI_T("Total Data:"));
    pTotalRow->AddItem(pTotalLabel);

    auto* pTotalBox = ui::Create<ui::Box>(this, {{DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("splitline_level1")}, {DUI_T("width"), DUI_T("180")}, {DUI_T("height"), DUI_T("26")}, {DUI_T("valign"), DUI_T("center")}});
    pTotalRow->AddItem(pTotalBox);

    auto* pEditTotal = ui::Create<ui::RichEdit>(this, {{DUI_T("number_only"), DUI_T("true")}, {DUI_T("min_number"), DUI_T("1")}, {DUI_T("font"), DUI_T("system_14")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("normal_text_color"), DUI_T("black")}, {DUI_T("multi_line"), DUI_T("false")}, {DUI_T("prompt_mode"), DUI_T("true")}, {DUI_T("prompt_text"), DUI_T("Total Data")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("text_padding"), DUI_T("2,0,0,0")}, {DUI_T("prompt_color"), DUI_T("splitline_level1")}, {DUI_T("auto_hscroll"), DUI_T("true")}});
    pEditTotal->SetName(DUI_T("edit_total"));
    pEditTotal->SetBkColor(DUI_T("white"));
    pEditTotal->SetText(DUI_T("10000"));
    pTotalBox->AddItem(pEditTotal);

    // Change/set buttons
    auto* pSetTotalRow = ui::Create<ui::Box>(this, {{DUI_T("height"), DUI_T("30")}, {DUI_T("child_margin"), DUI_T("10")}, {DUI_T("width"), DUI_T("stretch")}});
    pSettings->AddItem(pSetTotalRow);

    auto* pSetTotalBtn = ui::Create<ui::Button>(this, {{DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    pSetTotalBtn->SetClass(DUI_T("btn_global_blue_80x30"));
    pSetTotalBtn->SetName(DUI_T("btn_set_total"));
    pSetTotalBtn->SetText(DUI_T("Apply/Set"));
    pSetTotalRow->AddItem(pSetTotalBtn);

    auto* pSplit3 = ui::Create<ui::Control>(this, {{DUI_T("height"), DUI_T("1")}});
    pSplit3->SetBkColor(DUI_T("splitline_level1"));
    pSettings->AddItem(pSplit3);

    // Modify data
    auto* pUpdateRow = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("30")}, {DUI_T("child_margin"), DUI_T("10")}});
    pSettings->AddItem(pUpdateRow);

    auto* pUpdateLabel = ui::Create<ui::Label>(this, {{DUI_T("font"), DUI_T("system_bold_14")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("valign"), DUI_T("center")}});
    pUpdateLabel->SetText(DUI_T("Modify Data:"));
    pUpdateRow->AddItem(pUpdateLabel);

    auto* pUpdateBox1 = ui::Create<ui::Box>(this, {{DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("splitline_level1")}, {DUI_T("height"), DUI_T("26")}, {DUI_T("valign"), DUI_T("center")}});
    pUpdateRow->AddItem(pUpdateBox1);

    auto* pEditUpdate = ui::Create<ui::RichEdit>(this, {{DUI_T("number_only"), DUI_T("true")}, {DUI_T("min_number"), DUI_T("0")}, {DUI_T("font"), DUI_T("system_14")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("normal_text_color"), DUI_T("black")}, {DUI_T("multi_line"), DUI_T("false")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("text_padding"), DUI_T("2,0,0,0")}, {DUI_T("prompt_mode"), DUI_T("true")}, {DUI_T("prompt_text"), DUI_T("Index (subscript)")}, {DUI_T("prompt_color"), DUI_T("splitline_level1")}, {DUI_T("auto_hscroll"), DUI_T("true")}});
    pEditUpdate->SetName(DUI_T("edit_update"));
    pEditUpdate->SetBkColor(DUI_T("white"));
    pUpdateBox1->AddItem(pEditUpdate);

    auto* pUpdateBox2 = ui::Create<ui::Box>(this, {{DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("splitline_level1")}, {DUI_T("height"), DUI_T("26")}, {DUI_T("valign"), DUI_T("center")}});
    pUpdateRow->AddItem(pUpdateBox2);

    auto* pEditTaskName = ui::Create<ui::RichEdit>(this, {{DUI_T("font"), DUI_T("system_14")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("normal_text_color"), DUI_T("black")}, {DUI_T("multi_line"), DUI_T("false")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("text_padding"), DUI_T("2,0,0,0")}, {DUI_T("prompt_mode"), DUI_T("true")}, {DUI_T("prompt_text"), DUI_T("New Task Name")}, {DUI_T("prompt_color"), DUI_T("splitline_level1")}, {DUI_T("auto_hscroll"), DUI_T("true")}});
    pEditTaskName->SetName(DUI_T("edit_task_name"));
    pEditTaskName->SetBkColor(DUI_T("white"));
    pUpdateBox2->AddItem(pEditTaskName);

    auto* pUpdateBtnRow = ui::Create<ui::Box>(this, {{DUI_T("height"), DUI_T("30")}, {DUI_T("child_margin"), DUI_T("10")}, {DUI_T("width"), DUI_T("stretch")}});
    pSettings->AddItem(pUpdateBtnRow);

    auto* pUpdateBtn = ui::Create<ui::Button>(this, {{DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    pUpdateBtn->SetClass(DUI_T("btn_global_blue_80x30"));
    pUpdateBtn->SetName(DUI_T("btn_update"));
    pUpdateBtn->SetText(DUI_T("Update Data"));
    pUpdateBtnRow->AddItem(pUpdateBtn);

    auto* pSplit4 = ui::Create<ui::Control>(this, {{DUI_T("height"), DUI_T("1")}});
    pSplit4->SetBkColor(DUI_T("splitline_level1"));
    pSettings->AddItem(pSplit4);

    // Delete data
    auto* pDeleteRow = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("30")}, {DUI_T("child_margin"), DUI_T("10")}});
    pSettings->AddItem(pDeleteRow);

    auto* pDeleteLabel = ui::Create<ui::Label>(this, {{DUI_T("font"), DUI_T("system_bold_14")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("valign"), DUI_T("center")}});
    pDeleteLabel->SetText(DUI_T("Delete Data:"));
    pDeleteRow->AddItem(pDeleteLabel);

    auto* pDeleteBox = ui::Create<ui::Box>(this, {{DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("splitline_level1")}, {DUI_T("height"), DUI_T("26")}, {DUI_T("valign"), DUI_T("center")}});
    pDeleteRow->AddItem(pDeleteBox);

    auto* pEditDelete = ui::Create<ui::RichEdit>(this, {{DUI_T("number_only"), DUI_T("true")}, {DUI_T("min_number"), DUI_T("0")}, {DUI_T("font"), DUI_T("system_14")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("text_padding"), DUI_T("2,0,0,0")}, {DUI_T("normal_text_color"), DUI_T("black")}, {DUI_T("multi_line"), DUI_T("false")}, {DUI_T("prompt_mode"), DUI_T("true")}, {DUI_T("prompt_text"), DUI_T("Index (subscript)")}, {DUI_T("prompt_color"), DUI_T("splitline_level1")}, {DUI_T("auto_hscroll"), DUI_T("true")}});
    pEditDelete->SetName(DUI_T("edit_delete"));
    pEditDelete->SetBkColor(DUI_T("white"));
    pDeleteBox->AddItem(pEditDelete);

    auto* pDeleteBtn = ui::Create<ui::Button>(this, {{DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    pDeleteBtn->SetClass(DUI_T("btn_global_red_80x30"));
    pDeleteBtn->SetName(DUI_T("btn_delete"));
    pDeleteBtn->SetText(DUI_T("Delete Data"));
    pDeleteRow->AddItem(pDeleteBtn);

    // Virtual list on the right
    auto* pListArea = ui::Create<ui::Box>(this, {});
    pMain->AddItem(pListArea);

    auto* pList = ui::Create<ui::VirtualVTileListBox>(this, {{DUI_T("frame_selection"), DUI_T("true")}, {DUI_T("select_none_when_click_blank"), DUI_T("true")}, {DUI_T("select_like_list_ctrl"), DUI_T("true")}, {DUI_T("multi_select"), DUI_T("true")}, {DUI_T("scroll_select"), DUI_T("false")}, {DUI_T("item_size"), DUI_T("240,64")}, {DUI_T("auto_calc_item_size"), DUI_T("false")}, {DUI_T("vscrollbar"), DUI_T("true")}, {DUI_T("hscrollbar"), DUI_T("true")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("splitline_level1")}, {DUI_T("padding"), DUI_T("1,1,1,1")}});
    pList->SetClass(DUI_T("list"));
    pList->SetName(DUI_T("list"));
    pList->SetBkColor(DUI_T("white"));
    pListArea->AddItem(pList);

    AttachBox(pRoot);

    // Initialize control pointers
    m_pEditColumn = ui::Find<ui::RichEdit>(this, DUI_T("edit_column"));
    m_pEditTotal = ui::Find<ui::RichEdit>(this, DUI_T("edit_total"));
    m_pTileList = ui::Find<ui::VirtualListBox>(this, DUI_T("list"));
    m_pOptionColumnFix = ui::Find<ui::Option>(this, DUI_T("option_column_fix"));
    m_pEditUpdate = ui::Find<ui::RichEdit>(this, DUI_T("edit_update"));
    m_pEditTaskName = ui::Find<ui::RichEdit>(this, DUI_T("edit_task_name"));
    m_pEditDelete = ui::Find<ui::RichEdit>(this, DUI_T("edit_delete"));
    m_pEditChildMarginX = ui::Find<ui::RichEdit>(this, DUI_T("edit_child_margin_x"));
    m_pEditChildMarginY = ui::Find<ui::RichEdit>(this, DUI_T("edit_child_margin_y"));

    // Set the data provider
    m_pDataProvider = new DataProvider;
    m_pTileList->SetDataProvider(m_pDataProvider);

    const ui::LayoutType layoutType = m_pTileList->GetLayout()->GetLayoutType();
    if ((layoutType != ui::LayoutType::VirtualHTileLayout) &&
        (layoutType != ui::LayoutType::VirtualVTileLayout)){
        if (m_pEditColumn != nullptr) {
            m_pEditColumn->SetEnabled(false);
            m_pEditColumn->SetText(DUI_T("  "));
        }
    }

    if (layoutType == ui::LayoutType::VirtualHTileLayout) {
        m_pOptionColumnFix->SetText(DUI_T("Fixed Rows"));
    }
    else if (layoutType == ui::LayoutType::VirtualVTileLayout) {
        m_pOptionColumnFix->SetText(DUI_T("Fixed Columns"));
    }
    else {
        m_pOptionColumnFix->SetEnabled(false);
        if (auto* pControl = ui::Find<ui::Control>(this, DUI_T("option_column_auto"))) {
            pControl->SetEnabled(false);
        }
        if (auto* pControl = ui::Find<ui::Control>(this, DUI_T("label_column_row"))) {
            pControl->SetEnabled(false);
        }
    }

    pOptionAlign1 = ui::Find<ui::Option>(this, DUI_T("option_align1"));
    pOptionAlign2 = ui::Find<ui::Option>(this, DUI_T("option_align2"));
    pOptionAlign3 = ui::Find<ui::Option>(this, DUI_T("option_align3"));
    if ((pOptionAlign1 != nullptr) && (pOptionAlign2 != nullptr) && (pOptionAlign3 != nullptr)) {
        if ((layoutType == ui::LayoutType::VirtualHTileLayout) || (layoutType == ui::LayoutType::VirtualHLayout)) {
            pOptionAlign1->SetText(DUI_T("Align Top (top)"));
            pOptionAlign2->SetText(DUI_T("Align Center (center)"));
            pOptionAlign3->SetText(DUI_T("Align Bottom (bottom)"));
            pOptionAlign2->Selected(true, true);
        }
        else {
            pOptionAlign1->SetText(DUI_T("Align Left (left)"));
            pOptionAlign2->SetText(DUI_T("Align Center (center)"));
            pOptionAlign3->SetText(DUI_T("Align Right (right)"));
            pOptionAlign2->Selected(true, true);
        }
    }

    pListType = ui::Find<ui::Label>(this, DUI_T("list_box_type"));
    if (pListType != nullptr) {
        if (m_pTileList->GetType() == DUI_CTR_VIRTUAL_HLISTBOX) {
            pListType->SetText(DUI_T("VirtualHListBox"));
        }
        else if (m_pTileList->GetType() == DUI_CTR_VIRTUAL_VLISTBOX) {
            pListType->SetText(DUI_T("VirtualVListBox"));
        }
        else if (m_pTileList->GetType() == DUI_CTR_VIRTUAL_HTILE_LISTBOX) {
            pListType->SetText(DUI_T("VirtualHTileListBox"));
        }
        else if (m_pTileList->GetType() == DUI_CTR_VIRTUAL_VTILE_LISTBOX) {
            pListType->SetText(DUI_T("VirtualVTileListBox"));
        }
    }

    if (layoutType == ui::LayoutType::VirtualHLayout) {
        if (m_pEditChildMarginY != nullptr) {
            m_pEditChildMarginY->SetEnabled(false);
        }
        if (auto* pControl = ui::Find<ui::Control>(this, DUI_T("label_child_margin_y"))) {
            pControl->SetEnabled(false);
        }
    }
    else if (layoutType == ui::LayoutType::VirtualVLayout) {
        if (m_pEditChildMarginX != nullptr) {
            m_pEditChildMarginX->SetEnabled(false);
        }
        if (auto* pControl = ui::Find<ui::Control>(this, DUI_T("label_child_margin_x"))) {
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
    ui::Option* pOptionFix = ui::Find<ui::Option>(this, DUI_T("option_column_fix"));
    if (pOptionFix != nullptr) {
        pOptionFix->AttachSelect([this](const ui::EventArgs&) {
            if (auto* pEditColumn = ui::Find<ui::Control>(this, DUI_T("edit_column"))) {
                pEditColumn->SetVisible(true);
            }
            return true;
        });
        pOptionFix->AttachUnSelect([this](const ui::EventArgs&) {
            if (auto* pEditColumn = ui::Find<ui::Control>(this, DUI_T("edit_column"))) {
                pEditColumn->SetVisible(false);
            }
            return true;
        });
    }

    GetRoot()->AttachBubbledEvent(ui::kEventClick, UiBind(&MainForm::OnClicked, this, std::placeholders::_1), 0);

    ui::Option* pOptionAlign1 = ui::Find<ui::Option>(this, DUI_T("option_align1"));
    ui::Option* pOptionAlign2 = ui::Find<ui::Option>(this, DUI_T("option_align2"));
    ui::Option* pOptionAlign3 = ui::Find<ui::Option>(this, DUI_T("option_align3"));
    if ((pOptionAlign1 != nullptr) && (pOptionAlign2 != nullptr) && (pOptionAlign3 != nullptr)) {
        ui::LayoutType layoutType = m_pTileList->GetLayout()->GetLayoutType();
        if ((layoutType == ui::LayoutType::VirtualHTileLayout) || (layoutType == ui::LayoutType::VirtualHLayout)) {
            //Horizontal layout
            pOptionAlign1->AttachSelect([this](const ui::EventArgs&) {
                if (m_pTileList) {
                    m_pTileList->SetAttribute(DUI_T("child_valign"), DUI_T("top"));
                    m_pTileList->Refresh();
                }
                return true;
                });
            pOptionAlign2->AttachSelect([this](const ui::EventArgs&) {
                if (m_pTileList) {
                    m_pTileList->SetAttribute(DUI_T("child_valign"), DUI_T("center"));
                    m_pTileList->Refresh();
                }
                return true;
                });
            pOptionAlign3->AttachSelect([this](const ui::EventArgs&) {
                if (m_pTileList) {
                    m_pTileList->SetAttribute(DUI_T("child_valign"), DUI_T("bottom"));
                    m_pTileList->Refresh();
                }
                return true;
                });
        }
        else {
            //Vertical layout
            pOptionAlign1->AttachSelect([this](const ui::EventArgs&) {
                if (m_pTileList) {
                    m_pTileList->SetAttribute(DUI_T("child_halign"), DUI_T("left"));
                    m_pTileList->Refresh();
                }
                return true;
                });
            pOptionAlign2->AttachSelect([this](const ui::EventArgs&) {
                if (m_pTileList) {
                    m_pTileList->SetAttribute(DUI_T("child_halign"), DUI_T("center"));
                    m_pTileList->Refresh();
                }
                return true;
                });
            pOptionAlign3->AttachSelect([this](const ui::EventArgs&) {
                if (m_pTileList) {
                    m_pTileList->SetAttribute(DUI_T("child_halign"), DUI_T("right"));
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
    if (sName == DUI_T("btn_set_total"))
    {
        if (!m_pEditChildMarginX->GetText().empty())    {
            m_pTileList->SetAttribute(DUI_T("child_margin_x"), m_pEditChildMarginX->GetText());
        }
        if (!m_pEditChildMarginY->GetText().empty()) {
            m_pTileList->SetAttribute(DUI_T("child_margin_y"), m_pEditChildMarginY->GetText());
        }
        if (m_pOptionColumnFix->IsSelected()) {
            //Fixed number of columns/rows
            ui::LayoutType layoutType = m_pTileList->GetLayout()->GetLayoutType();
            if (layoutType == ui::LayoutType::VirtualVTileLayout) {
                m_pTileList->SetAttribute(DUI_T("columns"), m_pEditColumn->GetText());
            }
            if (layoutType == ui::LayoutType::VirtualHTileLayout) {
                m_pTileList->SetAttribute(DUI_T("rows"), m_pEditColumn->GetText());
            }            
        }
        else {
            //Auto-calculate the number of columns
            m_pTileList->SetAttribute(DUI_T("width"), DUI_T("stretch"));
            ui::LayoutType layoutType = m_pTileList->GetLayout()->GetLayoutType();
            if (layoutType == ui::LayoutType::VirtualVTileLayout) {
                m_pTileList->SetAttribute(DUI_T("columns"), DUI_T("auto"));
            }
            if (layoutType == ui::LayoutType::VirtualHTileLayout) {
                m_pTileList->SetAttribute(DUI_T("rows"), DUI_T("auto"));
            }
        }

        int nTotal = ui::StringUtil::StringToInt32(m_pEditTotal->GetText().c_str());
        if (nTotal > 0) {
            m_pDataProvider->SetTotal(nTotal);
        }
    }
    else if (sName == DUI_T("btn_update")) {
        size_t nIndex = (size_t)ui::StringUtil::StringToInt32(m_pEditUpdate->GetText().c_str());
        ASSERT(nIndex < m_pDataProvider->GetElementCount());
        m_pDataProvider->ChangeTaskName(nIndex, m_pEditTaskName->GetText());
    }
    else if (sName == DUI_T("btn_delete")) {
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
        sInfo += DUI_T(" ");
    }
    if (args.eventType == ui::kEventSelect) {
        size_t nNewItemIndex = (size_t)args.wParam;
        size_t nOldItemIndex = (size_t)args.lParam;
        size_t nNewElementID = pListBox->GetDisplayItemElementIndex(nNewItemIndex);
        if (nOldItemIndex != ui::Box::InvalidIndex) {
            size_t nOldElementID = pListBox->GetDisplayItemElementIndex(nOldItemIndex);
            sInfo += ui::StringUtil::Printf(DUI_T("NewItemIndex=%zu, NewElementID=%zu; OldItemIndex=%zu, OldElementID=%zu"),
                                            nNewItemIndex, nNewElementID, nOldItemIndex, nOldElementID);
        }
        else {
            sInfo += ui::StringUtil::Printf(DUI_T("NewItemIndex=%zu, NewElementID=%zu"), nNewItemIndex, nNewElementID);
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
            sInfo += DUI_T("no params");
        }
        else {
            size_t nCalcElementID = pListBox->GetDisplayItemElementIndex(nItemIndex);
            ASSERT(nElementID == nCalcElementID);
            sInfo += ui::StringUtil::Printf(DUI_T("ItemIndex=%zu, ElementID=%zu"), nItemIndex, nElementID);
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
        size_t nElementID = (size_t)args.lParam;
        if (nItemIndex == ui::Box::InvalidIndex) {
            sInfo += DUI_T("no params");
        }
        else {
            size_t nCalcElementID = pListBox->GetDisplayItemElementIndex(nItemIndex);
            ASSERT(nElementID == nCalcElementID);
            sInfo += ui::StringUtil::Printf(DUI_T("ItemIndex=%zu, ElementID=%zu"), nItemIndex, nElementID);
        }
    }
    else if (args.eventType == ui::kEventElementFilled) {
        size_t nItemIndex = (size_t)args.wParam;
        size_t nElementID = (size_t)args.lParam;
        size_t nCalcElementID = pListBox->GetDisplayItemElementIndex(nItemIndex);
        ASSERT(nElementID == nCalcElementID);
        sInfo += ui::StringUtil::Printf(DUI_T("ItemIndex=%zu, ElementID=%zu, ListBoxItem: 0x%p"), nItemIndex, nElementID, args.pEventData);
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
