#include "MainForm.h"
#include "DataProvider.h"

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

void MainForm::GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs)
{
    //Corresponding to the <Window> attributes of main.xml
    attrs.m_bInitSizeDefined = true;
    attrs.m_szInitSize.cx = 1000;
    attrs.m_szInitSize.cy = 600;
    attrs.m_bShadowAttached = true;
    attrs.m_bShadowAttachedDefined = true;
    attrs.m_bIsLayeredWindow = true;
    attrs.m_bIsLayeredWindowDefined = true;
    attrs.m_rcSizeBox = ui::UiRect(4, 4, 4, 4);
    attrs.m_bSizeBoxDefined = true;
    attrs.m_rcCaption = ui::UiRect(0, 0, 0, 36);
    attrs.m_bCaptionDefined = true;
    BaseClass::GetCreateWindowAttributes(attrs);
}

void MainForm::BuildUI()
{
    // Corresponding to the main.xml layout
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
    pTitle->SetText(_T("Virtual List (VirtualHTileListBox | VirtualVTileListBox | VirtualHListBox | VirtualVListBox)"));
    pTitle->SetAttribute(_T("font"), _T("system_14"));
    pTitle->SetAttribute(_T("valign"), _T("center"));
    pTitle->SetAttribute(_T("margin"), _T("8"));
    pTitle->SetAttribute(_T("mouse_enabled"), _T("false"));
    pCaption->AddItem(pTitle);

    ui::Control* pSpacer = new ui::Control(this);
    pSpacer->SetAttribute(_T("mouse_enabled"), _T("false"));
    pCaption->AddItem(pSpacer);

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

    // Virtual list type display
    ui::HBox* pTypeRow = new ui::HBox(this);
    pTypeRow->SetAttribute(_T("height"), _T("30"));
    pRoot->AddItem(pTypeRow);

    ui::Label* pTypeLabel = new ui::Label(this);
    pTypeLabel->SetText(_T("Current virtual list container type:"));
    pTypeLabel->SetAttribute(_T("font"), _T("system_14"));
    pTypeLabel->SetAttribute(_T("valign"), _T("center"));
    pTypeLabel->SetAttribute(_T("margin"), _T("8,0,8,0"));
    pTypeRow->AddItem(pTypeLabel);

    ui::Label* pListType = new ui::Label(this);
    pListType->SetName(_T("list_box_type"));
    pListType->SetText(_T("VirtualHTileListBox"));
    pListType->SetAttribute(_T("font"), _T("system_14"));
    pListType->SetAttribute(_T("valign"), _T("center"));
    pTypeRow->AddItem(pListType);

    // Main content area
    ui::HBox* pMain = new ui::HBox(this);
    pMain->SetAttribute(_T("child_margin"), _T("5"));
    pMain->SetAttribute(_T("padding"), _T("5,5,5,10"));
    pRoot->AddItem(pMain);

    // Left settings panel
    ui::VBox* pSettings = new ui::VBox(this);
    pSettings->SetAttribute(_T("width"), _T("300"));
    pSettings->SetAttribute(_T("border_size"), _T("1"));
    pSettings->SetAttribute(_T("border_color"), _T("splitline_level1"));
    pSettings->SetAttribute(_T("padding"), _T("5,10,5,10"));
    pSettings->SetAttribute(_T("child_margin"), _T("10"));
    pMain->AddItem(pSettings);

    // Row/column settings
    ui::VBox* pColumnGroup = new ui::VBox(this);
    pColumnGroup->SetAttribute(_T("height"), _T("80"));
    pSettings->AddItem(pColumnGroup);

    ui::Label* pColumnLabel = new ui::Label(this);
    pColumnLabel->SetName(_T("label_column_row"));
    pColumnLabel->SetAttribute(_T("font"), _T("system_bold_14"));
    pColumnLabel->SetText(_T("Rows/Columns:"));
    pColumnLabel->SetAttribute(_T("width"), _T("auto"));
    pColumnLabel->SetAttribute(_T("height"), _T("auto"));
    pColumnLabel->SetAttribute(_T("valign"), _T("center"));
    pColumnGroup->AddItem(pColumnLabel);

    ui::Option* pOptionColumnAuto = new ui::Option(this);
    pOptionColumnAuto->SetClass(_T("option_1"));
    pOptionColumnAuto->SetName(_T("option_column_auto"));
    pOptionColumnAuto->SetAttribute(_T("group"), _T("option_group"));
    pOptionColumnAuto->SetText(_T("Auto Calculate"));
    pOptionColumnAuto->SetAttribute(_T("height"), _T("28"));
    pOptionColumnAuto->SetAttribute(_T("margin"), _T("80,0,0,0"));
    pOptionColumnAuto->Selected(true);
    pColumnGroup->AddItem(pOptionColumnAuto);

    ui::HBox* pColumnFixRow = new ui::HBox(this);
    pColumnFixRow->SetAttribute(_T("height"), _T("30"));
    pColumnGroup->AddItem(pColumnFixRow);

    ui::Option* pOptionColumnFix = new ui::Option(this);
    pOptionColumnFix->SetClass(_T("option_1"));
    pOptionColumnFix->SetName(_T("option_column_fix"));
    pOptionColumnFix->SetAttribute(_T("group"), _T("option_group"));
    pOptionColumnFix->SetText(_T("Fixed Columns"));
    pOptionColumnFix->SetAttribute(_T("margin"), _T("80,0,0,0"));
    pColumnFixRow->AddItem(pOptionColumnFix);

    ui::RichEdit* pEditColumn = new ui::RichEdit(this);
    pEditColumn->SetClass(_T("simple rich_edit_spin"));
    pEditColumn->SetName(_T("edit_column"));
    pEditColumn->SetText(_T("3"));
    pEditColumn->SetAttribute(_T("min_number"), _T("1"));
    pEditColumn->SetAttribute(_T("max_number"), _T("12"));
    pEditColumn->SetVisible(false);
    pEditColumn->SetAttribute(_T("text_padding"), _T("1,0,8,0"));
    pEditColumn->SetAttribute(_T("width"), _T("50"));
    pEditColumn->SetAttribute(_T("height"), _T("28"));
    pEditColumn->SetAttribute(_T("margin"), _T("6,0,0,0"));
    pEditColumn->SetAttribute(_T("default_context_menu"), _T("true"));
    pEditColumn->SetAttribute(_T("number_only"), _T("true"));
    pEditColumn->SetAttribute(_T("limit_text"), _T("3"));
    pEditColumn->SetAttribute(_T("valign"), _T("center"));
    pEditColumn->SetAttribute(_T("text_align"), _T("vcenter"));
    pEditColumn->SetBkColor(_T("white"));
    pEditColumn->SetAttribute(_T("border_size"), _T("1"));
    pEditColumn->SetAttribute(_T("border_color"), _T("light_gray"));
    pColumnFixRow->AddItem(pEditColumn);

    ui::Control* pSplit1 = new ui::Control(this);
    pSplit1->SetAttribute(_T("height"), _T("1"));
    pSplit1->SetBkColor(_T("splitline_level1"));
    pSettings->AddItem(pSplit1);

    // Alignment
    ui::VBox* pAlignGroup = new ui::VBox(this);
    pAlignGroup->SetAttribute(_T("height"), _T("auto"));
    pSettings->AddItem(pAlignGroup);

    ui::Label* pAlignLabel = new ui::Label(this);
    pAlignLabel->SetAttribute(_T("font"), _T("system_bold_14"));
    pAlignLabel->SetText(_T("Alignment:"));
    pAlignLabel->SetAttribute(_T("width"), _T("auto"));
    pAlignLabel->SetAttribute(_T("height"), _T("auto"));
    pAlignLabel->SetAttribute(_T("valign"), _T("center"));
    pAlignGroup->AddItem(pAlignLabel);

    ui::Option* pOptionAlign1 = new ui::Option(this);
    pOptionAlign1->SetClass(_T("option_1"));
    pOptionAlign1->SetName(_T("option_align1"));
    pOptionAlign1->SetAttribute(_T("group"), _T("option_group_align"));
    pOptionAlign1->SetText(_T("Align Left (left)"));
    pOptionAlign1->SetAttribute(_T("height"), _T("28"));
    pOptionAlign1->SetAttribute(_T("margin"), _T("80,0,0,0"));
    pOptionAlign1->Selected(true);
    pAlignGroup->AddItem(pOptionAlign1);

    ui::Option* pOptionAlign2 = new ui::Option(this);
    pOptionAlign2->SetClass(_T("option_1"));
    pOptionAlign2->SetName(_T("option_align2"));
    pOptionAlign2->SetAttribute(_T("group"), _T("option_group_align"));
    pOptionAlign2->SetText(_T("Align Center (center)"));
    pOptionAlign2->SetAttribute(_T("height"), _T("28"));
    pOptionAlign2->SetAttribute(_T("margin"), _T("80,0,0,0"));
    pAlignGroup->AddItem(pOptionAlign2);

    ui::Option* pOptionAlign3 = new ui::Option(this);
    pOptionAlign3->SetClass(_T("option_1"));
    pOptionAlign3->SetName(_T("option_align3"));
    pOptionAlign3->SetAttribute(_T("group"), _T("option_group_align"));
    pOptionAlign3->SetText(_T("Align Right (right)"));
    pOptionAlign3->SetAttribute(_T("height"), _T("28"));
    pOptionAlign3->SetAttribute(_T("margin"), _T("80,0,0,0"));
    pAlignGroup->AddItem(pOptionAlign3);

    ui::Control* pSplit2 = new ui::Control(this);
    pSplit2->SetAttribute(_T("height"), _T("1"));
    pSplit2->SetBkColor(_T("splitline_level1"));
    pSettings->AddItem(pSplit2);

    // Child spacing
    ui::VBox* pMarginGroup = new ui::VBox(this);
    pMarginGroup->SetAttribute(_T("height"), _T("auto"));
    pSettings->AddItem(pMarginGroup);

    ui::HBox* pMarginXRow = new ui::HBox(this);
    pMarginXRow->SetAttribute(_T("height"), _T("30"));
    pMarginXRow->SetAttribute(_T("child_margin"), _T("10"));
    pMarginGroup->AddItem(pMarginXRow);

    ui::Label* pMarginXLabel = new ui::Label(this);
    pMarginXLabel->SetName(_T("label_child_margin_x"));
    pMarginXLabel->SetAttribute(_T("font"), _T("system_bold_14"));
    pMarginXLabel->SetText(_T("Child Margin X:"));
    pMarginXLabel->SetAttribute(_T("width"), _T("auto"));
    pMarginXLabel->SetAttribute(_T("height"), _T("auto"));
    pMarginXLabel->SetAttribute(_T("valign"), _T("center"));
    pMarginXRow->AddItem(pMarginXLabel);

    ui::Box* pMarginXBox = new ui::Box(this);
    pMarginXBox->SetAttribute(_T("border_size"), _T("1"));
    pMarginXBox->SetAttribute(_T("border_color"), _T("splitline_level1"));
    pMarginXBox->SetAttribute(_T("width"), _T("90"));
    pMarginXBox->SetAttribute(_T("height"), _T("26"));
    pMarginXBox->SetAttribute(_T("valign"), _T("center"));
    pMarginXRow->AddItem(pMarginXBox);

    ui::RichEdit* pEditMarginX = new ui::RichEdit(this);
    pEditMarginX->SetName(_T("edit_child_margin_x"));
    pEditMarginX->SetBkColor(_T("white"));
    pEditMarginX->SetAttribute(_T("font"), _T("system_14"));
    pEditMarginX->SetAttribute(_T("width"), _T("stretch"));
    pEditMarginX->SetAttribute(_T("height"), _T("stretch"));
    pEditMarginX->SetAttribute(_T("valign"), _T("center"));
    pEditMarginX->SetAttribute(_T("normal_text_color"), _T("black"));
    pEditMarginX->SetAttribute(_T("multi_line"), _T("false"));
    pEditMarginX->SetAttribute(_T("prompt_mode"), _T("true"));
    pEditMarginX->SetAttribute(_T("prompt_text"), _T("Child Margin X"));
    pEditMarginX->SetAttribute(_T("text_align"), _T("vcenter"));
    pEditMarginX->SetAttribute(_T("text_padding"), _T("2,0,0,0"));
    pEditMarginX->SetAttribute(_T("prompt_color"), _T("splitline_level1"));
    pEditMarginX->SetAttribute(_T("auto_hscroll"), _T("true"));
    pMarginXBox->AddItem(pEditMarginX);

    ui::HBox* pMarginYRow = new ui::HBox(this);
    pMarginYRow->SetAttribute(_T("height"), _T("30"));
    pMarginYRow->SetAttribute(_T("child_margin"), _T("10"));
    pMarginGroup->AddItem(pMarginYRow);

    ui::Label* pMarginYLabel = new ui::Label(this);
    pMarginYLabel->SetName(_T("label_child_margin_y"));
    pMarginYLabel->SetAttribute(_T("font"), _T("system_bold_14"));
    pMarginYLabel->SetText(_T("Child Margin Y:"));
    pMarginYLabel->SetAttribute(_T("width"), _T("auto"));
    pMarginYLabel->SetAttribute(_T("height"), _T("auto"));
    pMarginYLabel->SetAttribute(_T("valign"), _T("center"));
    pMarginYRow->AddItem(pMarginYLabel);

    ui::Box* pMarginYBox = new ui::Box(this);
    pMarginYBox->SetAttribute(_T("border_size"), _T("1"));
    pMarginYBox->SetAttribute(_T("border_color"), _T("splitline_level1"));
    pMarginYBox->SetAttribute(_T("width"), _T("90"));
    pMarginYBox->SetAttribute(_T("height"), _T("26"));
    pMarginYBox->SetAttribute(_T("valign"), _T("center"));
    pMarginYRow->AddItem(pMarginYBox);

    ui::RichEdit* pEditMarginY = new ui::RichEdit(this);
    pEditMarginY->SetName(_T("edit_child_margin_y"));
    pEditMarginY->SetBkColor(_T("white"));
    pEditMarginY->SetAttribute(_T("font"), _T("system_14"));
    pEditMarginY->SetAttribute(_T("width"), _T("stretch"));
    pEditMarginY->SetAttribute(_T("height"), _T("stretch"));
    pEditMarginY->SetAttribute(_T("valign"), _T("center"));
    pEditMarginY->SetAttribute(_T("normal_text_color"), _T("black"));
    pEditMarginY->SetAttribute(_T("multi_line"), _T("false"));
    pEditMarginY->SetAttribute(_T("prompt_mode"), _T("true"));
    pEditMarginY->SetAttribute(_T("prompt_text"), _T("Child Margin Y"));
    pEditMarginY->SetAttribute(_T("text_align"), _T("vcenter"));
    pEditMarginY->SetAttribute(_T("text_padding"), _T("2,0,0,0"));
    pEditMarginY->SetAttribute(_T("prompt_color"), _T("splitline_level1"));
    pEditMarginY->SetAttribute(_T("auto_hscroll"), _T("true"));
    pMarginYBox->AddItem(pEditMarginY);

    // Total data
    ui::HBox* pTotalRow = new ui::HBox(this);
    pTotalRow->SetAttribute(_T("height"), _T("30"));
    pTotalRow->SetAttribute(_T("child_margin"), _T("10"));
    pSettings->AddItem(pTotalRow);

    ui::Label* pTotalLabel = new ui::Label(this);
    pTotalLabel->SetAttribute(_T("font"), _T("system_bold_14"));
    pTotalLabel->SetText(_T("Total Data:"));
    pTotalLabel->SetAttribute(_T("width"), _T("auto"));
    pTotalLabel->SetAttribute(_T("height"), _T("auto"));
    pTotalLabel->SetAttribute(_T("valign"), _T("center"));
    pTotalRow->AddItem(pTotalLabel);

    ui::Box* pTotalBox = new ui::Box(this);
    pTotalBox->SetAttribute(_T("border_size"), _T("1"));
    pTotalBox->SetAttribute(_T("border_color"), _T("splitline_level1"));
    pTotalBox->SetAttribute(_T("width"), _T("180"));
    pTotalBox->SetAttribute(_T("height"), _T("26"));
    pTotalBox->SetAttribute(_T("valign"), _T("center"));
    pTotalRow->AddItem(pTotalBox);

    ui::RichEdit* pEditTotal = new ui::RichEdit(this);
    pEditTotal->SetName(_T("edit_total"));
    pEditTotal->SetBkColor(_T("white"));
    pEditTotal->SetText(_T("10000"));
    pEditTotal->SetAttribute(_T("number_only"), _T("true"));
    pEditTotal->SetAttribute(_T("min_number"), _T("1"));
    pEditTotal->SetAttribute(_T("font"), _T("system_14"));
    pEditTotal->SetAttribute(_T("width"), _T("stretch"));
    pEditTotal->SetAttribute(_T("height"), _T("stretch"));
    pEditTotal->SetAttribute(_T("valign"), _T("center"));
    pEditTotal->SetAttribute(_T("normal_text_color"), _T("black"));
    pEditTotal->SetAttribute(_T("multi_line"), _T("false"));
    pEditTotal->SetAttribute(_T("prompt_mode"), _T("true"));
    pEditTotal->SetAttribute(_T("prompt_text"), _T("Total Data"));
    pEditTotal->SetAttribute(_T("text_align"), _T("vcenter"));
    pEditTotal->SetAttribute(_T("text_padding"), _T("2,0,0,0"));
    pEditTotal->SetAttribute(_T("prompt_color"), _T("splitline_level1"));
    pEditTotal->SetAttribute(_T("auto_hscroll"), _T("true"));
    pTotalBox->AddItem(pEditTotal);

    // Change/set buttons
    ui::Box* pSetTotalRow = new ui::Box(this);
    pSetTotalRow->SetAttribute(_T("height"), _T("30"));
    pSetTotalRow->SetAttribute(_T("child_margin"), _T("10"));
    pSetTotalRow->SetAttribute(_T("width"), _T("stretch"));
    pSettings->AddItem(pSetTotalRow);

    ui::Button* pSetTotalBtn = new ui::Button(this);
    pSetTotalBtn->SetClass(_T("btn_global_blue_80x30"));
    pSetTotalBtn->SetAttribute(_T("halign"), _T("center"));
    pSetTotalBtn->SetAttribute(_T("valign"), _T("center"));
    pSetTotalBtn->SetName(_T("btn_set_total"));
    pSetTotalBtn->SetText(_T("Apply/Set"));
    pSetTotalRow->AddItem(pSetTotalBtn);

    ui::Control* pSplit3 = new ui::Control(this);
    pSplit3->SetAttribute(_T("height"), _T("1"));
    pSplit3->SetBkColor(_T("splitline_level1"));
    pSettings->AddItem(pSplit3);

    // Modify data
    ui::HBox* pUpdateRow = new ui::HBox(this);
    pUpdateRow->SetAttribute(_T("height"), _T("30"));
    pUpdateRow->SetAttribute(_T("child_margin"), _T("10"));
    pSettings->AddItem(pUpdateRow);

    ui::Label* pUpdateLabel = new ui::Label(this);
    pUpdateLabel->SetAttribute(_T("font"), _T("system_bold_14"));
    pUpdateLabel->SetText(_T("Modify Data:"));
    pUpdateLabel->SetAttribute(_T("width"), _T("auto"));
    pUpdateLabel->SetAttribute(_T("height"), _T("auto"));
    pUpdateLabel->SetAttribute(_T("valign"), _T("center"));
    pUpdateRow->AddItem(pUpdateLabel);

    ui::Box* pUpdateBox1 = new ui::Box(this);
    pUpdateBox1->SetAttribute(_T("border_size"), _T("1"));
    pUpdateBox1->SetAttribute(_T("border_color"), _T("splitline_level1"));
    pUpdateBox1->SetAttribute(_T("height"), _T("26"));
    pUpdateBox1->SetAttribute(_T("valign"), _T("center"));
    pUpdateRow->AddItem(pUpdateBox1);

    ui::RichEdit* pEditUpdate = new ui::RichEdit(this);
    pEditUpdate->SetName(_T("edit_update"));
    pEditUpdate->SetAttribute(_T("number_only"), _T("true"));
    pEditUpdate->SetAttribute(_T("min_number"), _T("0"));
    pEditUpdate->SetBkColor(_T("white"));
    pEditUpdate->SetAttribute(_T("font"), _T("system_14"));
    pEditUpdate->SetAttribute(_T("width"), _T("stretch"));
    pEditUpdate->SetAttribute(_T("height"), _T("stretch"));
    pEditUpdate->SetAttribute(_T("valign"), _T("center"));
    pEditUpdate->SetAttribute(_T("normal_text_color"), _T("black"));
    pEditUpdate->SetAttribute(_T("multi_line"), _T("false"));
    pEditUpdate->SetAttribute(_T("text_align"), _T("vcenter"));
    pEditUpdate->SetAttribute(_T("text_padding"), _T("2,0,0,0"));
    pEditUpdate->SetAttribute(_T("prompt_mode"), _T("true"));
    pEditUpdate->SetAttribute(_T("prompt_text"), _T("Index (subscript)"));
    pEditUpdate->SetAttribute(_T("prompt_color"), _T("splitline_level1"));
    pEditUpdate->SetAttribute(_T("auto_hscroll"), _T("true"));
    pUpdateBox1->AddItem(pEditUpdate);

    ui::Box* pUpdateBox2 = new ui::Box(this);
    pUpdateBox2->SetAttribute(_T("border_size"), _T("1"));
    pUpdateBox2->SetAttribute(_T("border_color"), _T("splitline_level1"));
    pUpdateBox2->SetAttribute(_T("height"), _T("26"));
    pUpdateBox2->SetAttribute(_T("valign"), _T("center"));
    pUpdateRow->AddItem(pUpdateBox2);

    ui::RichEdit* pEditTaskName = new ui::RichEdit(this);
    pEditTaskName->SetName(_T("edit_task_name"));
    pEditTaskName->SetBkColor(_T("white"));
    pEditTaskName->SetAttribute(_T("font"), _T("system_14"));
    pEditTaskName->SetAttribute(_T("width"), _T("stretch"));
    pEditTaskName->SetAttribute(_T("height"), _T("stretch"));
    pEditTaskName->SetAttribute(_T("valign"), _T("center"));
    pEditTaskName->SetAttribute(_T("normal_text_color"), _T("black"));
    pEditTaskName->SetAttribute(_T("multi_line"), _T("false"));
    pEditTaskName->SetAttribute(_T("text_align"), _T("vcenter"));
    pEditTaskName->SetAttribute(_T("text_padding"), _T("2,0,0,0"));
    pEditTaskName->SetAttribute(_T("prompt_mode"), _T("true"));
    pEditTaskName->SetAttribute(_T("prompt_text"), _T("New Task Name"));
    pEditTaskName->SetAttribute(_T("prompt_color"), _T("splitline_level1"));
    pEditTaskName->SetAttribute(_T("auto_hscroll"), _T("true"));
    pUpdateBox2->AddItem(pEditTaskName);

    ui::Box* pUpdateBtnRow = new ui::Box(this);
    pUpdateBtnRow->SetAttribute(_T("height"), _T("30"));
    pUpdateBtnRow->SetAttribute(_T("child_margin"), _T("10"));
    pUpdateBtnRow->SetAttribute(_T("width"), _T("stretch"));
    pSettings->AddItem(pUpdateBtnRow);

    ui::Button* pUpdateBtn = new ui::Button(this);
    pUpdateBtn->SetClass(_T("btn_global_blue_80x30"));
    pUpdateBtn->SetAttribute(_T("halign"), _T("center"));
    pUpdateBtn->SetAttribute(_T("valign"), _T("center"));
    pUpdateBtn->SetName(_T("btn_update"));
    pUpdateBtn->SetText(_T("Update Data"));
    pUpdateBtnRow->AddItem(pUpdateBtn);

    ui::Control* pSplit4 = new ui::Control(this);
    pSplit4->SetAttribute(_T("height"), _T("1"));
    pSplit4->SetBkColor(_T("splitline_level1"));
    pSettings->AddItem(pSplit4);

    // Delete data
    ui::HBox* pDeleteRow = new ui::HBox(this);
    pDeleteRow->SetAttribute(_T("height"), _T("30"));
    pDeleteRow->SetAttribute(_T("child_margin"), _T("10"));
    pSettings->AddItem(pDeleteRow);

    ui::Label* pDeleteLabel = new ui::Label(this);
    pDeleteLabel->SetAttribute(_T("font"), _T("system_bold_14"));
    pDeleteLabel->SetText(_T("Delete Data:"));
    pDeleteLabel->SetAttribute(_T("width"), _T("auto"));
    pDeleteLabel->SetAttribute(_T("height"), _T("auto"));
    pDeleteLabel->SetAttribute(_T("valign"), _T("center"));
    pDeleteRow->AddItem(pDeleteLabel);

    ui::Box* pDeleteBox = new ui::Box(this);
    pDeleteBox->SetAttribute(_T("border_size"), _T("1"));
    pDeleteBox->SetAttribute(_T("border_color"), _T("splitline_level1"));
    pDeleteBox->SetAttribute(_T("height"), _T("26"));
    pDeleteBox->SetAttribute(_T("valign"), _T("center"));
    pDeleteRow->AddItem(pDeleteBox);

    ui::RichEdit* pEditDelete = new ui::RichEdit(this);
    pEditDelete->SetName(_T("edit_delete"));
    pEditDelete->SetAttribute(_T("number_only"), _T("true"));
    pEditDelete->SetAttribute(_T("min_number"), _T("0"));
    pEditDelete->SetBkColor(_T("white"));
    pEditDelete->SetAttribute(_T("font"), _T("system_14"));
    pEditDelete->SetAttribute(_T("width"), _T("stretch"));
    pEditDelete->SetAttribute(_T("height"), _T("stretch"));
    pEditDelete->SetAttribute(_T("valign"), _T("center"));
    pEditDelete->SetAttribute(_T("text_align"), _T("vcenter"));
    pEditDelete->SetAttribute(_T("text_padding"), _T("2,0,0,0"));
    pEditDelete->SetAttribute(_T("normal_text_color"), _T("black"));
    pEditDelete->SetAttribute(_T("multi_line"), _T("false"));
    pEditDelete->SetAttribute(_T("prompt_mode"), _T("true"));
    pEditDelete->SetAttribute(_T("prompt_text"), _T("Index (subscript)"));
    pEditDelete->SetAttribute(_T("prompt_color"), _T("splitline_level1"));
    pEditDelete->SetAttribute(_T("auto_hscroll"), _T("true"));
    pDeleteBox->AddItem(pEditDelete);

    ui::Button* pDeleteBtn = new ui::Button(this);
    pDeleteBtn->SetClass(_T("btn_global_red_80x30"));
    pDeleteBtn->SetAttribute(_T("halign"), _T("center"));
    pDeleteBtn->SetAttribute(_T("valign"), _T("center"));
    pDeleteBtn->SetName(_T("btn_delete"));
    pDeleteBtn->SetText(_T("Delete Data"));
    pDeleteRow->AddItem(pDeleteBtn);

    // Virtual list on the right
    ui::Box* pListArea = new ui::Box(this);
    pMain->AddItem(pListArea);

    ui::VirtualVTileListBox* pList = new ui::VirtualVTileListBox(this);
    pList->SetClass(_T("list"));
    pList->SetName(_T("list"));
    pList->SetBkColor(_T("white"));
    pList->SetAttribute(_T("frame_selection"), _T("true"));
    pList->SetAttribute(_T("select_none_when_click_blank"), _T("true"));
    pList->SetAttribute(_T("select_like_list_ctrl"), _T("true"));
    pList->SetAttribute(_T("multi_select"), _T("true"));
    pList->SetAttribute(_T("scroll_select"), _T("false"));
    pList->SetAttribute(_T("item_size"), _T("240,64"));
    pList->SetAttribute(_T("auto_calc_item_size"), _T("false"));
    pList->SetAttribute(_T("vscrollbar"), _T("true"));
    pList->SetAttribute(_T("hscrollbar"), _T("true"));
    pList->SetAttribute(_T("border_size"), _T("1"));
    pList->SetAttribute(_T("border_color"), _T("splitline_level1"));
    pList->SetAttribute(_T("padding"), _T("1,1,1,1"));
    pListArea->AddItem(pList);

    AttachBox(pRoot);
}


void MainForm::OnInitWindow()
{
    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);

    // Pure-code-built UI (corresponding to the main.xml layout)
    BuildUI();

    // Manually wire up the <Event> tags in the XML:
    // option_column_fix select → edit_column visible; unselect → hidden
    ui::Option* pOptionFix = dynamic_cast<ui::Option*>(FindControl(_T("option_column_fix")));
    if (pOptionFix != nullptr) {
        pOptionFix->AttachSelect([this](const ui::EventArgs&) {
            ui::Control* pEditColumn = FindControl(_T("edit_column"));
            if (pEditColumn != nullptr) {
                pEditColumn->SetVisible(true);
            }
            return true;
        });
        pOptionFix->AttachUnSelect([this](const ui::EventArgs&) {
            ui::Control* pEditColumn = FindControl(_T("edit_column"));
            if (pEditColumn != nullptr) {
                pEditColumn->SetVisible(false);
            }
            return true;
        });
    }

    m_pEditColumn = dynamic_cast<ui::RichEdit*>(FindControl(_T("edit_column")));
    m_pEditTotal = dynamic_cast<ui::RichEdit*>(FindControl(_T("edit_total")));
    m_pTileList = dynamic_cast<ui::VirtualListBox*>(FindControl(_T("list")));
    m_pOptionColumnFix = dynamic_cast<ui::Option*>(FindControl(_T("option_column_fix")));
    m_pEditUpdate = dynamic_cast<ui::RichEdit*>(FindControl(_T("edit_update")));
    m_pEditTaskName = dynamic_cast<ui::RichEdit*>(FindControl(_T("edit_task_name")));
    m_pEditDelete = dynamic_cast<ui::RichEdit*>(FindControl(_T("edit_delete")));
    m_pEditChildMarginX = dynamic_cast<ui::RichEdit*>(FindControl(_T("edit_child_margin_x")));
    m_pEditChildMarginY = dynamic_cast<ui::RichEdit*>(FindControl(_T("edit_child_margin_y")));

    GetRoot()->AttachBubbledEvent(ui::kEventClick, UiBind(&MainForm::OnClicked, this, std::placeholders::_1), 0);

    // Set the data provider
    m_pDataProvider = new DataProvider;
    m_pTileList->SetDataProvider(m_pDataProvider);

    const ui::LayoutType layoutType = m_pTileList->GetLayout()->GetLayoutType();
    if ((layoutType != ui::LayoutType::VirtualHTileLayout) &&
        (layoutType != ui::LayoutType::VirtualVTileLayout)){
        //Hide the column setting; other layouts do not support setting columns
        if (m_pEditColumn != nullptr) {
            m_pEditColumn->SetEnabled(false);
            m_pEditColumn->SetText(_T("  "));
        }
    }

    if (layoutType == ui::LayoutType::VirtualHTileLayout) {
        m_pOptionColumnFix->SetText(_T("Fixed Rows"));
    }
    else if (layoutType == ui::LayoutType::VirtualVTileLayout) {
        m_pOptionColumnFix->SetText(_T("Fixed Columns"));
    }
    else {
        m_pOptionColumnFix->SetEnabled(false);
        ui::Control* pControl = FindControl(_T("option_column_auto"));
        if (pControl) {
            pControl->SetEnabled(false);
        }
        pControl = FindControl(_T("label_column_row"));
        if (pControl) {
            pControl->SetEnabled(false);
        }
    }

    ui::Option* pOptionAlign1 = dynamic_cast<ui::Option*>(FindControl(_T("option_align1")));
    ui::Option* pOptionAlign2 = dynamic_cast<ui::Option*>(FindControl(_T("option_align2")));
    ui::Option* pOptionAlign3 = dynamic_cast<ui::Option*>(FindControl(_T("option_align3")));
    if ((pOptionAlign1 != nullptr) && (pOptionAlign2 != nullptr) && (pOptionAlign3 != nullptr)) {
        if ((layoutType == ui::LayoutType::VirtualHTileLayout) || (layoutType == ui::LayoutType::VirtualHLayout)) {
            //Horizontal layout
            pOptionAlign1->SetText(_T("Align Top (top)"));
            pOptionAlign2->SetText(_T("Align Center (center)"));
            pOptionAlign3->SetText(_T("Align Bottom (bottom)"));
            pOptionAlign2->Selected(true, true);

            //When the alignment option changes, dynamically adjust the control alignment
            pOptionAlign1->AttachSelect([this](const ui::EventArgs&) {
                if (m_pTileList) {
                    m_pTileList->SetAttribute(_T("child_valign"), _T("top"));
                    m_pTileList->Refresh();
                }
                return true;
                });
            pOptionAlign2->AttachSelect([this](const ui::EventArgs&) {
                if (m_pTileList) {
                    m_pTileList->SetAttribute(_T("child_valign"), _T("center"));
                    m_pTileList->Refresh();
                }
                return true;
                });
            pOptionAlign3->AttachSelect([this](const ui::EventArgs&) {
                if (m_pTileList) {
                    m_pTileList->SetAttribute(_T("child_valign"), _T("bottom"));
                    m_pTileList->Refresh();
                }
                return true;
                });
        }
        else {
            //Vertical layout
            pOptionAlign1->SetText(_T("Align Left (left)"));
            pOptionAlign2->SetText(_T("Align Center (center)"));
            pOptionAlign3->SetText(_T("Align Right (right)"));
            pOptionAlign2->Selected(true, true);

            //When the alignment option changes, dynamically adjust the control alignment
            pOptionAlign1->AttachSelect([this](const ui::EventArgs&) {
                if (m_pTileList) {
                    m_pTileList->SetAttribute(_T("child_halign"), _T("left"));
                    m_pTileList->Refresh();
                }
                return true;
                });
            pOptionAlign2->AttachSelect([this](const ui::EventArgs&) {
                if (m_pTileList) {
                    m_pTileList->SetAttribute(_T("child_halign"), _T("center"));
                    m_pTileList->Refresh();
                }
                return true;
                });
            pOptionAlign3->AttachSelect([this](const ui::EventArgs&) {
                if (m_pTileList) {
                    m_pTileList->SetAttribute(_T("child_halign"), _T("right"));
                    m_pTileList->Refresh();
                }
                return true;
                });
        }
    }
    //Show the current container type and update the default attributes
    ui::Label* pListType = dynamic_cast<ui::Label*>(FindControl(_T("list_box_type")));
    if (pListType != nullptr) {
        if (m_pTileList->GetType() == DUI_CTR_VIRTUAL_HLISTBOX) {
            pListType->SetText(_T("VirtualHListBox"));
        }
        else if (m_pTileList->GetType() == DUI_CTR_VIRTUAL_VLISTBOX) {
            pListType->SetText(_T("VirtualVListBox"));
        }
        else if (m_pTileList->GetType() == DUI_CTR_VIRTUAL_HTILE_LISTBOX) {
            pListType->SetText(_T("VirtualHTileListBox"));
        }
        else if (m_pTileList->GetType() == DUI_CTR_VIRTUAL_VTILE_LISTBOX) {
            pListType->SetText(_T("VirtualVTileListBox"));
        }
    }

    //Update the state of the child spacing
    if (layoutType == ui::LayoutType::VirtualHLayout) {
        if (m_pEditChildMarginY != nullptr) {
            m_pEditChildMarginY->SetEnabled(false);
        }
        ui::Control* pControl = FindControl(_T("label_child_margin_y"));
        if (pControl) {
            pControl->SetEnabled(false);
        }
    }
    else if (layoutType == ui::LayoutType::VirtualVLayout) {
        if (m_pEditChildMarginX != nullptr) {
            m_pEditChildMarginX->SetEnabled(false);
        }
        ui::Control* pControl = FindControl(_T("label_child_margin_x"));
        if (pControl) {
            pControl->SetEnabled(false);
        }
    }

    //Set the initial data
    if ((m_pDataProvider != nullptr) && (m_pEditTotal != nullptr)) {
        int nTotal = ui::StringUtil::StringToInt32(m_pEditTotal->GetText().c_str());
        if (nTotal > 0) {
            m_pDataProvider->SetTotal(nTotal);
        }
    }

    //Test the virtual list events
    TestVirtualListBoxEvents(m_pTileList);
}

bool MainForm::OnClicked(const ui::EventArgs& args)
{
    auto sName = args.GetSender()->GetName();
    if (sName == _T("btn_set_total"))
    {
        if (!m_pEditChildMarginX->GetText().empty())    {
            m_pTileList->SetAttribute(_T("child_margin_x"), m_pEditChildMarginX->GetText());
        }
        if (!m_pEditChildMarginY->GetText().empty()) {
            m_pTileList->SetAttribute(_T("child_margin_y"), m_pEditChildMarginY->GetText());
        }
        if (m_pOptionColumnFix->IsSelected()) {
            //Fixed number of columns/rows
            ui::LayoutType layoutType = m_pTileList->GetLayout()->GetLayoutType();
            if (layoutType == ui::LayoutType::VirtualVTileLayout) {
                m_pTileList->SetAttribute(_T("columns"), m_pEditColumn->GetText());
            }
            if (layoutType == ui::LayoutType::VirtualHTileLayout) {
                m_pTileList->SetAttribute(_T("rows"), m_pEditColumn->GetText());
            }            
        }
        else {
            //Auto-calculate the number of columns
            m_pTileList->SetAttribute(_T("width"), _T("stretch"));
            ui::LayoutType layoutType = m_pTileList->GetLayout()->GetLayoutType();
            if (layoutType == ui::LayoutType::VirtualVTileLayout) {
                m_pTileList->SetAttribute(_T("columns"), _T("auto"));
            }
            if (layoutType == ui::LayoutType::VirtualHTileLayout) {
                m_pTileList->SetAttribute(_T("rows"), _T("auto"));
            }
        }

        int nTotal = ui::StringUtil::StringToInt32(m_pEditTotal->GetText().c_str());
        if (nTotal > 0) {
            m_pDataProvider->SetTotal(nTotal);
        }
    }
    else if (sName == _T("btn_update")) {
        size_t nIndex = (size_t)ui::StringUtil::StringToInt32(m_pEditUpdate->GetText().c_str());
        ASSERT(nIndex < m_pDataProvider->GetElementCount());
        m_pDataProvider->ChangeTaskName(nIndex, m_pEditTaskName->GetText());
    }
    else if (sName == _T("btn_delete")) {
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
        sInfo += _T(" ");
    }
    if (args.eventType == ui::kEventSelect) {
        size_t nNewItemIndex = (size_t)args.wParam;
        size_t nOldItemIndex = (size_t)args.lParam;
        size_t nNewElementID = pListBox->GetDisplayItemElementIndex(nNewItemIndex);
        if (nOldItemIndex != ui::Box::InvalidIndex) {
            size_t nOldElementID = pListBox->GetDisplayItemElementIndex(nOldItemIndex);
            sInfo += ui::StringUtil::Printf(_T("NewItemIndex=%zu, NewElementID=%zu; OldItemIndex=%zu, OldElementID=%zu"),
                                            nNewItemIndex, nNewElementID, nOldItemIndex, nOldElementID);
        }
        else {
            sInfo += ui::StringUtil::Printf(_T("NewItemIndex=%zu, NewElementID=%zu"), nNewItemIndex, nNewElementID);
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
            sInfo += _T("no params");
        }
        else {
            size_t nCalcElementID = pListBox->GetDisplayItemElementIndex(nItemIndex);
            ASSERT(nElementID == nCalcElementID);
            sInfo += ui::StringUtil::Printf(_T("ItemIndex=%zu, ElementID=%zu"), nItemIndex, nElementID);
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
        size_t nElementID = (size_t)args.lParam;
        if (nItemIndex == ui::Box::InvalidIndex) {
            sInfo += _T("no params");
        }
        else {
            size_t nCalcElementID = pListBox->GetDisplayItemElementIndex(nItemIndex);
            ASSERT(nElementID == nCalcElementID);
            sInfo += ui::StringUtil::Printf(_T("ItemIndex=%zu, ElementID=%zu"), nItemIndex, nElementID);
        }
    }
    else if (args.eventType == ui::kEventElementFilled) {
        size_t nItemIndex = (size_t)args.wParam;
        size_t nElementID = (size_t)args.lParam;
        size_t nCalcElementID = pListBox->GetDisplayItemElementIndex(nItemIndex);
        ASSERT(nElementID == nCalcElementID);
        sInfo += ui::StringUtil::Printf(_T("ItemIndex=%zu, ElementID=%zu, ListBoxItem: 0x%p"), nItemIndex, nElementID, args.pEventData);
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

