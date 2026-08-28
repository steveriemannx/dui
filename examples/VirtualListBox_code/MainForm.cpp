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
    //Corresponding to the <Window> attributes of main.xml:
    // size="75%,75%" and min_size="750,500" (min size is set in PreInitWindow)
    ui::UiRect rcWork;
    ui::WindowBase::GetPrimaryMonitorWorkRect(rcWork);
    attrs.m_bInitSizeDefined = true;
    attrs.m_szInitSize.cx = (int32_t)(rcWork.Width() * 0.75f);
    attrs.m_szInitSize.cy = (int32_t)(rcWork.Height() * 0.75f);
    attrs.m_bShadowAttached = true;
    attrs.m_bShadowAttachedDefined = true;
    attrs.m_bIsLayeredWindow = true;
    attrs.m_bIsLayeredWindowDefined = true;
    attrs.m_rcSizeBox = ui::UiRect(4, 4, 4, 4);
    attrs.m_bSizeBoxDefined = true;
    attrs.m_rcCaption = ui::UiRect(0, 0, 0, 36);
    attrs.m_bCaptionDefined = true;

    // WindowBuilder clamps the initial size to the XML min_size at parse time.
    if (attrs.m_szInitSize.cx < 750) {
        attrs.m_szInitSize.cx = 750;
    }
    if (attrs.m_szInitSize.cy < 500) {
        attrs.m_szInitSize.cy = 500;
    }

    BaseClass::GetCreateWindowAttributes(attrs);
}

void MainForm::PreInitWindow()
{
    BaseClass::PreInitWindow();

    // No layout XML is loaded, so Window::ParseWindowXml cannot establish the
    // window resource sub-path; set it explicitly so image paths resolve from
    // the "virtual_list_box" folder.
    SetResourcePath(ui::FilePath(_T("virtual_list_box")));
    SetWindowMinimumSize(ui::UiSize(750, 500), true);
}

void MainForm::BuildUI()
{
    // Corresponding to the main.xml layout
    auto* pRoot = ui::Create<ui::VBox>(this, {});
    pRoot->SetBkColor(_T("bk_wnd_darkcolor"));

    // Title bar area
    auto* pCaption = ui::Create<ui::HBox>(this, {{_T("name"), _T("window_caption_bar")}, {_T("width"), _T("stretch")}, {_T("height"), _T("36")}});
    pCaption->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    auto* pTitle = ui::Create<ui::Label>(this, {{_T("font"), _T("system_14")}, {_T("valign"), _T("center")}, {_T("margin"), _T("8")}, {_T("mouse_enabled"), _T("false")}});
    pTitle->SetText(_T("Virtual List (VirtualHTileListBox | VirtualVTileListBox | VirtualHListBox | VirtualVListBox)"));
    pCaption->AddItem(pTitle);

    auto* pSpacer = ui::Create<ui::Control>(this, {{_T("mouse_enabled"), _T("false")}});
    pCaption->AddItem(pSpacer);

    auto* pMinBtn = ui::Create<ui::Button>(this, {{_T("height"), _T("32")}, {_T("width"), _T("40")}, {_T("margin"), _T("0,2,0,2")}});
    pMinBtn->SetClass(_T("btn_wnd_min_11"));
    pMinBtn->SetName(_T("minbtn"));
    pMinBtn->SetToolTipText(_T("Minimize"));
    pCaption->AddItem(pMinBtn);

    auto* pMaxBox = ui::Create<ui::Box>(this, {{_T("height"), _T("stretch")}, {_T("width"), _T("40")}, {_T("margin"), _T("0,2,0,2")}});
    pCaption->AddItem(pMaxBox);

    auto* pMaxBtn = ui::Create<ui::Button>(this, {{_T("height"), _T("32")}, {_T("width"), _T("stretch")}});
    pMaxBtn->SetClass(_T("btn_wnd_max_11"));
    pMaxBtn->SetName(_T("maxbtn"));
    pMaxBtn->SetToolTipText(_T("Maximize"));
    pMaxBox->AddItem(pMaxBtn);

    auto* pRestoreBtn = ui::Create<ui::Button>(this, {{_T("height"), _T("32")}, {_T("width"), _T("stretch")}});
    pRestoreBtn->SetClass(_T("btn_wnd_restore_11"));
    pRestoreBtn->SetName(_T("restorebtn"));
    pRestoreBtn->SetVisible(false);
    pRestoreBtn->SetToolTipText(_T("Restore"));
    pMaxBox->AddItem(pRestoreBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{_T("height"), _T("stretch")}, {_T("width"), _T("40")}, {_T("margin"), _T("0,0,0,2")}});
    pCloseBtn->SetClass(_T("btn_wnd_close_11"));
    pCloseBtn->SetName(_T("closebtn"));
    pCloseBtn->SetToolTipText(_T("Close"));
    pCaption->AddItem(pCloseBtn);

    // Virtual list type display
    auto* pTypeRow = ui::Create<ui::HBox>(this, {{_T("height"), _T("30")}});
    pRoot->AddItem(pTypeRow);

    auto* pTypeLabel = ui::Create<ui::Label>(this, {{_T("font"), _T("system_14")}, {_T("valign"), _T("center")}, {_T("margin"), _T("8,0,8,0")}});
    pTypeLabel->SetText(_T("Current virtual list container type:"));
    pTypeRow->AddItem(pTypeLabel);

    auto* pListType = ui::Create<ui::Label>(this, {{_T("font"), _T("system_14")}, {_T("valign"), _T("center")}});
    pListType->SetName(_T("list_box_type"));
    pListType->SetText(_T("VirtualHTileListBox"));
    pTypeRow->AddItem(pListType);

    // Main content area
    auto* pMain = ui::Create<ui::HBox>(this, {{_T("child_margin"), _T("5")}, {_T("padding"), _T("5,5,5,10")}});
    pRoot->AddItem(pMain);

    // Left settings panel
    auto* pSettings = ui::Create<ui::VBox>(this, {{_T("width"), _T("300")}, {_T("border_size"), _T("1")}, {_T("border_color"), _T("splitline_level1")}, {_T("padding"), _T("5,10,5,10")}, {_T("child_margin"), _T("10")}});
    pMain->AddItem(pSettings);

    // Row/column settings
    auto* pColumnGroup = ui::Create<ui::VBox>(this, {{_T("height"), _T("80")}});
    pSettings->AddItem(pColumnGroup);

    auto* pColumnLabel = ui::Create<ui::Label>(this, {{_T("font"), _T("system_bold_14")}, {_T("width"), _T("auto")}, {_T("height"), _T("auto")}, {_T("valign"), _T("center")}});
    pColumnLabel->SetName(_T("label_column_row"));
    pColumnLabel->SetText(_T("Rows/Columns:"));
    pColumnGroup->AddItem(pColumnLabel);

    auto* pOptionColumnAuto = ui::Create<ui::Option>(this, {{_T("group"), _T("option_group")}, {_T("height"), _T("28")}, {_T("margin"), _T("80,0,0,0")}});
    pOptionColumnAuto->SetClass(_T("option_1"));
    pOptionColumnAuto->SetName(_T("option_column_auto"));
    pOptionColumnAuto->SetText(_T("Auto Calculate"));
    pOptionColumnAuto->Selected(true);
    pColumnGroup->AddItem(pOptionColumnAuto);

    auto* pColumnFixRow = ui::Create<ui::HBox>(this, {{_T("height"), _T("30")}});
    pColumnGroup->AddItem(pColumnFixRow);

    auto* pOptionColumnFix = ui::Create<ui::Option>(this, {{_T("group"), _T("option_group")}, {_T("margin"), _T("80,0,0,0")}});
    pOptionColumnFix->SetClass(_T("option_1"));
    pOptionColumnFix->SetName(_T("option_column_fix"));
    pOptionColumnFix->SetText(_T("Fixed Columns"));
    pColumnFixRow->AddItem(pOptionColumnFix);

    auto* pEditColumn = ui::Create<ui::RichEdit>(this, {{_T("min_number"), _T("1")}, {_T("max_number"), _T("12")}, {_T("text_padding"), _T("1,0,8,0")}, {_T("width"), _T("50")}, {_T("height"), _T("28")}, {_T("margin"), _T("6,0,0,0")}, {_T("default_context_menu"), _T("true")}, {_T("number_only"), _T("true")}, {_T("limit_text"), _T("3")}, {_T("valign"), _T("center")}, {_T("text_align"), _T("vcenter")}, {_T("border_size"), _T("1")}, {_T("border_color"), _T("light_gray")}});
    pEditColumn->SetClass(_T("simple rich_edit_spin"));
    pEditColumn->SetName(_T("edit_column"));
    pEditColumn->SetText(_T("3"));
    pEditColumn->SetVisible(false);
    pEditColumn->SetBkColor(_T("white"));
    pColumnFixRow->AddItem(pEditColumn);

    auto* pSplit1 = ui::Create<ui::Control>(this, {{_T("height"), _T("1")}});
    pSplit1->SetBkColor(_T("splitline_level1"));
    pSettings->AddItem(pSplit1);

    // Alignment
    auto* pAlignGroup = ui::Create<ui::VBox>(this, {{_T("height"), _T("auto")}});
    pSettings->AddItem(pAlignGroup);

    auto* pAlignLabel = ui::Create<ui::Label>(this, {{_T("font"), _T("system_bold_14")}, {_T("width"), _T("auto")}, {_T("height"), _T("auto")}, {_T("valign"), _T("center")}});
    pAlignLabel->SetText(_T("Alignment:"));
    pAlignGroup->AddItem(pAlignLabel);

    auto* pOptionAlign1 = ui::Create<ui::Option>(this, {{_T("group"), _T("option_group_align")}, {_T("height"), _T("28")}, {_T("margin"), _T("80,0,0,0")}});
    pOptionAlign1->SetClass(_T("option_1"));
    pOptionAlign1->SetName(_T("option_align1"));
    pOptionAlign1->SetText(_T("Align Left (left)"));
    pOptionAlign1->Selected(true);
    pAlignGroup->AddItem(pOptionAlign1);

    auto* pOptionAlign2 = ui::Create<ui::Option>(this, {{_T("group"), _T("option_group_align")}, {_T("height"), _T("28")}, {_T("margin"), _T("80,0,0,0")}});
    pOptionAlign2->SetClass(_T("option_1"));
    pOptionAlign2->SetName(_T("option_align2"));
    pOptionAlign2->SetText(_T("Align Center (center)"));
    pAlignGroup->AddItem(pOptionAlign2);

    auto* pOptionAlign3 = ui::Create<ui::Option>(this, {{_T("group"), _T("option_group_align")}, {_T("height"), _T("28")}, {_T("margin"), _T("80,0,0,0")}});
    pOptionAlign3->SetClass(_T("option_1"));
    pOptionAlign3->SetName(_T("option_align3"));
    pOptionAlign3->SetText(_T("Align Right (right)"));
    pAlignGroup->AddItem(pOptionAlign3);

    auto* pSplit2 = ui::Create<ui::Control>(this, {{_T("height"), _T("1")}});
    pSplit2->SetBkColor(_T("splitline_level1"));
    pSettings->AddItem(pSplit2);

    // Child spacing
    auto* pMarginGroup = ui::Create<ui::VBox>(this, {{_T("height"), _T("auto")}});
    pSettings->AddItem(pMarginGroup);

    auto* pMarginXRow = ui::Create<ui::HBox>(this, {{_T("height"), _T("30")}, {_T("child_margin"), _T("10")}});
    pMarginGroup->AddItem(pMarginXRow);

    auto* pMarginXLabel = ui::Create<ui::Label>(this, {{_T("font"), _T("system_bold_14")}, {_T("width"), _T("auto")}, {_T("height"), _T("auto")}, {_T("valign"), _T("center")}});
    pMarginXLabel->SetName(_T("label_child_margin_x"));
    pMarginXLabel->SetText(_T("Child Margin X:"));
    pMarginXRow->AddItem(pMarginXLabel);

    auto* pMarginXBox = ui::Create<ui::Box>(this, {{_T("border_size"), _T("1")}, {_T("border_color"), _T("splitline_level1")}, {_T("width"), _T("90")}, {_T("height"), _T("26")}, {_T("valign"), _T("center")}});
    pMarginXRow->AddItem(pMarginXBox);

    auto* pEditMarginX = ui::Create<ui::RichEdit>(this, {{_T("font"), _T("system_14")}, {_T("width"), _T("stretch")}, {_T("height"), _T("stretch")}, {_T("valign"), _T("center")}, {_T("normal_text_color"), _T("black")}, {_T("multi_line"), _T("false")}, {_T("prompt_mode"), _T("true")}, {_T("prompt_text"), _T("Child Margin X")}, {_T("text_align"), _T("vcenter")}, {_T("text_padding"), _T("2,0,0,0")}, {_T("prompt_color"), _T("splitline_level1")}, {_T("auto_hscroll"), _T("true")}});
    pEditMarginX->SetName(_T("edit_child_margin_x"));
    pEditMarginX->SetBkColor(_T("white"));
    pMarginXBox->AddItem(pEditMarginX);

    auto* pMarginYRow = ui::Create<ui::HBox>(this, {{_T("height"), _T("30")}, {_T("child_margin"), _T("10")}});
    pMarginGroup->AddItem(pMarginYRow);

    auto* pMarginYLabel = ui::Create<ui::Label>(this, {{_T("font"), _T("system_bold_14")}, {_T("width"), _T("auto")}, {_T("height"), _T("auto")}, {_T("valign"), _T("center")}});
    pMarginYLabel->SetName(_T("label_child_margin_y"));
    pMarginYLabel->SetText(_T("Child Margin Y:"));
    pMarginYRow->AddItem(pMarginYLabel);

    auto* pMarginYBox = ui::Create<ui::Box>(this, {{_T("border_size"), _T("1")}, {_T("border_color"), _T("splitline_level1")}, {_T("width"), _T("90")}, {_T("height"), _T("26")}, {_T("valign"), _T("center")}});
    pMarginYRow->AddItem(pMarginYBox);

    auto* pEditMarginY = ui::Create<ui::RichEdit>(this, {{_T("font"), _T("system_14")}, {_T("width"), _T("stretch")}, {_T("height"), _T("stretch")}, {_T("valign"), _T("center")}, {_T("normal_text_color"), _T("black")}, {_T("multi_line"), _T("false")}, {_T("prompt_mode"), _T("true")}, {_T("prompt_text"), _T("Child Margin Y")}, {_T("text_align"), _T("vcenter")}, {_T("text_padding"), _T("2,0,0,0")}, {_T("prompt_color"), _T("splitline_level1")}, {_T("auto_hscroll"), _T("true")}});
    pEditMarginY->SetName(_T("edit_child_margin_y"));
    pEditMarginY->SetBkColor(_T("white"));
    pMarginYBox->AddItem(pEditMarginY);

    // Total data
    auto* pTotalRow = ui::Create<ui::HBox>(this, {{_T("height"), _T("30")}, {_T("child_margin"), _T("10")}});
    pSettings->AddItem(pTotalRow);

    auto* pTotalLabel = ui::Create<ui::Label>(this, {{_T("font"), _T("system_bold_14")}, {_T("width"), _T("auto")}, {_T("height"), _T("auto")}, {_T("valign"), _T("center")}});
    pTotalLabel->SetText(_T("Total Data:"));
    pTotalRow->AddItem(pTotalLabel);

    auto* pTotalBox = ui::Create<ui::Box>(this, {{_T("border_size"), _T("1")}, {_T("border_color"), _T("splitline_level1")}, {_T("width"), _T("180")}, {_T("height"), _T("26")}, {_T("valign"), _T("center")}});
    pTotalRow->AddItem(pTotalBox);

    auto* pEditTotal = ui::Create<ui::RichEdit>(this, {{_T("number_only"), _T("true")}, {_T("min_number"), _T("1")}, {_T("font"), _T("system_14")}, {_T("width"), _T("stretch")}, {_T("height"), _T("stretch")}, {_T("valign"), _T("center")}, {_T("normal_text_color"), _T("black")}, {_T("multi_line"), _T("false")}, {_T("prompt_mode"), _T("true")}, {_T("prompt_text"), _T("Total Data")}, {_T("text_align"), _T("vcenter")}, {_T("text_padding"), _T("2,0,0,0")}, {_T("prompt_color"), _T("splitline_level1")}, {_T("auto_hscroll"), _T("true")}});
    pEditTotal->SetName(_T("edit_total"));
    pEditTotal->SetBkColor(_T("white"));
    pEditTotal->SetText(_T("10000"));
    pTotalBox->AddItem(pEditTotal);

    // Change/set buttons
    auto* pSetTotalRow = ui::Create<ui::Box>(this, {{_T("height"), _T("30")}, {_T("child_margin"), _T("10")}, {_T("width"), _T("stretch")}});
    pSettings->AddItem(pSetTotalRow);

    auto* pSetTotalBtn = ui::Create<ui::Button>(this, {{_T("halign"), _T("center")}, {_T("valign"), _T("center")}});
    pSetTotalBtn->SetClass(_T("btn_global_blue_80x30"));
    pSetTotalBtn->SetName(_T("btn_set_total"));
    pSetTotalBtn->SetText(_T("Apply/Set"));
    pSetTotalRow->AddItem(pSetTotalBtn);

    auto* pSplit3 = ui::Create<ui::Control>(this, {{_T("height"), _T("1")}});
    pSplit3->SetBkColor(_T("splitline_level1"));
    pSettings->AddItem(pSplit3);

    // Modify data
    auto* pUpdateRow = ui::Create<ui::HBox>(this, {{_T("height"), _T("30")}, {_T("child_margin"), _T("10")}});
    pSettings->AddItem(pUpdateRow);

    auto* pUpdateLabel = ui::Create<ui::Label>(this, {{_T("font"), _T("system_bold_14")}, {_T("width"), _T("auto")}, {_T("height"), _T("auto")}, {_T("valign"), _T("center")}});
    pUpdateLabel->SetText(_T("Modify Data:"));
    pUpdateRow->AddItem(pUpdateLabel);

    auto* pUpdateBox1 = ui::Create<ui::Box>(this, {{_T("border_size"), _T("1")}, {_T("border_color"), _T("splitline_level1")}, {_T("height"), _T("26")}, {_T("valign"), _T("center")}});
    pUpdateRow->AddItem(pUpdateBox1);

    auto* pEditUpdate = ui::Create<ui::RichEdit>(this, {{_T("number_only"), _T("true")}, {_T("min_number"), _T("0")}, {_T("font"), _T("system_14")}, {_T("width"), _T("stretch")}, {_T("height"), _T("stretch")}, {_T("valign"), _T("center")}, {_T("normal_text_color"), _T("black")}, {_T("multi_line"), _T("false")}, {_T("text_align"), _T("vcenter")}, {_T("text_padding"), _T("2,0,0,0")}, {_T("prompt_mode"), _T("true")}, {_T("prompt_text"), _T("Index (subscript)")}, {_T("prompt_color"), _T("splitline_level1")}, {_T("auto_hscroll"), _T("true")}});
    pEditUpdate->SetName(_T("edit_update"));
    pEditUpdate->SetBkColor(_T("white"));
    pUpdateBox1->AddItem(pEditUpdate);

    auto* pUpdateBox2 = ui::Create<ui::Box>(this, {{_T("border_size"), _T("1")}, {_T("border_color"), _T("splitline_level1")}, {_T("height"), _T("26")}, {_T("valign"), _T("center")}});
    pUpdateRow->AddItem(pUpdateBox2);

    auto* pEditTaskName = ui::Create<ui::RichEdit>(this, {{_T("font"), _T("system_14")}, {_T("width"), _T("stretch")}, {_T("height"), _T("stretch")}, {_T("valign"), _T("center")}, {_T("normal_text_color"), _T("black")}, {_T("multi_line"), _T("false")}, {_T("text_align"), _T("vcenter")}, {_T("text_padding"), _T("2,0,0,0")}, {_T("prompt_mode"), _T("true")}, {_T("prompt_text"), _T("New Task Name")}, {_T("prompt_color"), _T("splitline_level1")}, {_T("auto_hscroll"), _T("true")}});
    pEditTaskName->SetName(_T("edit_task_name"));
    pEditTaskName->SetBkColor(_T("white"));
    pUpdateBox2->AddItem(pEditTaskName);

    auto* pUpdateBtnRow = ui::Create<ui::Box>(this, {{_T("height"), _T("30")}, {_T("child_margin"), _T("10")}, {_T("width"), _T("stretch")}});
    pSettings->AddItem(pUpdateBtnRow);

    auto* pUpdateBtn = ui::Create<ui::Button>(this, {{_T("halign"), _T("center")}, {_T("valign"), _T("center")}});
    pUpdateBtn->SetClass(_T("btn_global_blue_80x30"));
    pUpdateBtn->SetName(_T("btn_update"));
    pUpdateBtn->SetText(_T("Update Data"));
    pUpdateBtnRow->AddItem(pUpdateBtn);

    auto* pSplit4 = ui::Create<ui::Control>(this, {{_T("height"), _T("1")}});
    pSplit4->SetBkColor(_T("splitline_level1"));
    pSettings->AddItem(pSplit4);

    // Delete data
    auto* pDeleteRow = ui::Create<ui::HBox>(this, {{_T("height"), _T("30")}, {_T("child_margin"), _T("10")}});
    pSettings->AddItem(pDeleteRow);

    auto* pDeleteLabel = ui::Create<ui::Label>(this, {{_T("font"), _T("system_bold_14")}, {_T("width"), _T("auto")}, {_T("height"), _T("auto")}, {_T("valign"), _T("center")}});
    pDeleteLabel->SetText(_T("Delete Data:"));
    pDeleteRow->AddItem(pDeleteLabel);

    auto* pDeleteBox = ui::Create<ui::Box>(this, {{_T("border_size"), _T("1")}, {_T("border_color"), _T("splitline_level1")}, {_T("height"), _T("26")}, {_T("valign"), _T("center")}});
    pDeleteRow->AddItem(pDeleteBox);

    auto* pEditDelete = ui::Create<ui::RichEdit>(this, {{_T("number_only"), _T("true")}, {_T("min_number"), _T("0")}, {_T("font"), _T("system_14")}, {_T("width"), _T("stretch")}, {_T("height"), _T("stretch")}, {_T("valign"), _T("center")}, {_T("text_align"), _T("vcenter")}, {_T("text_padding"), _T("2,0,0,0")}, {_T("normal_text_color"), _T("black")}, {_T("multi_line"), _T("false")}, {_T("prompt_mode"), _T("true")}, {_T("prompt_text"), _T("Index (subscript)")}, {_T("prompt_color"), _T("splitline_level1")}, {_T("auto_hscroll"), _T("true")}});
    pEditDelete->SetName(_T("edit_delete"));
    pEditDelete->SetBkColor(_T("white"));
    pDeleteBox->AddItem(pEditDelete);

    auto* pDeleteBtn = ui::Create<ui::Button>(this, {{_T("halign"), _T("center")}, {_T("valign"), _T("center")}});
    pDeleteBtn->SetClass(_T("btn_global_red_80x30"));
    pDeleteBtn->SetName(_T("btn_delete"));
    pDeleteBtn->SetText(_T("Delete Data"));
    pDeleteRow->AddItem(pDeleteBtn);

    // Virtual list on the right
    auto* pListArea = ui::Create<ui::Box>(this, {});
    pMain->AddItem(pListArea);

    auto* pList = ui::Create<ui::VirtualVTileListBox>(this, {{_T("frame_selection"), _T("true")}, {_T("select_none_when_click_blank"), _T("true")}, {_T("select_like_list_ctrl"), _T("true")}, {_T("multi_select"), _T("true")}, {_T("scroll_select"), _T("false")}, {_T("item_size"), _T("240,64")}, {_T("auto_calc_item_size"), _T("false")}, {_T("vscrollbar"), _T("true")}, {_T("hscrollbar"), _T("true")}, {_T("border_size"), _T("1")}, {_T("border_color"), _T("splitline_level1")}, {_T("padding"), _T("1,1,1,1")}});
    pList->SetClass(_T("list"));
    pList->SetName(_T("list"));
    pList->SetBkColor(_T("white"));
    pListArea->AddItem(pList);

    AttachBox(pRoot);
}


void MainForm::OnInitWindow()
{
    // Use the OS-provided system shadow on all platforms.
    SetShadowAttached(true);
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);

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
    BaseClass::OnInitWindow();
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

