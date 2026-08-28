#include "MainForm.h"
#include "Item.h"
#include "generated_ui.inc"  // Auto-generated: InitMain() + RegisterEmbeddedClasses()

MainForm::MainForm() :
    m_pListBox(nullptr),
    m_pEditColumn(nullptr),
    m_pEditTotal(nullptr),
    m_pEditUpdate(nullptr)
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
    return _T("");
}

void MainForm::GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs)
{
    attrs.m_bInitSizeDefined = true;
    attrs.m_szInitSize.cx = 900;
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

void MainForm::OnInitWindow()
{
    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);

    // Auto-register classes with embedded images (generated from global.xml)
    RegisterEmbeddedClasses(this);

    // Build-time generated code: InitMain() from main.xml
    InitMain(this);

    // Wire up caption button click handlers
    auto wireBtn = [this](const DString& name) {
        ui::Button* btn = dynamic_cast<ui::Button*>(FindControl(name));
        if (btn) btn->AttachClick(ui::UiBind(&MainForm::OnButtonClick, this, std::placeholders::_1));
    };
    wireBtn(_T("closebtn"));
    wireBtn(_T("minbtn"));
    wireBtn(_T("maxbtn"));
    wireBtn(_T("restorebtn"));

    m_pEditColumn = dynamic_cast<ui::RichEdit*>(FindControl(_T("edit_column")));
    m_pEditTotal  = dynamic_cast<ui::RichEdit*>(FindControl(_T("edit_total")));
    m_pListBox    = dynamic_cast<ui::VirtualListBox*>(FindControl(_T("list")));
    m_pEditUpdate = dynamic_cast<ui::RichEdit*>(FindControl(_T("edit_update")));

    InitListBox();

    BaseClass::OnInitWindow();
}

void MainForm::InitListBox()
{
    if (!m_pListBox) return;

    const int kCount = 50;
    for (int i = 0; i < kCount; i++) {
        DString title = ui::StringUtil::Printf(_T("Item #%02d"), i + 1);
        Item* pItem = new Item(m_pListBox->GetWindow());
        pItem->SetFixedHeight(ui::UiFixedInt(64), true, true);
        pItem->InitSubControls(_T("icon.png"), title, i);
        m_pListBox->AddItem(pItem);
    }

    UpdateCounts();
}

void MainForm::UpdateCounts()
{
    if (m_pEditTotal) {
        m_pEditTotal->SetText(ui::StringUtil::Printf(_T("%d"), m_pListBox ? (int)m_pListBox->GetItemCount() : 0));
    }
    if (m_pEditColumn) m_pEditColumn->SetText(_T("2"));
    if (m_pEditUpdate) m_pEditUpdate->SetText(_T("build-time"));
}

bool MainForm::OnButtonClick(const ui::EventArgs& msg)
{
    ui::Control* pSender = msg.GetSender();
    if (pSender == nullptr) return false;
    DString sName = pSender->GetName();
    if (sName == DUI_CTR_BUTTON_CLOSE) {
        CloseWnd();
    } else if (sName == DUI_CTR_BUTTON_MIN) {
        ShowWindow(ui::kSW_MINIMIZE);
    } else if (sName == DUI_CTR_BUTTON_MAX) {
        ShowWindow(ui::kSW_SHOW_MAXIMIZED);
    } else if (sName == DUI_CTR_BUTTON_RESTORE) {
        ShowWindow(ui::kSW_RESTORE);
    }
    return true;
}
