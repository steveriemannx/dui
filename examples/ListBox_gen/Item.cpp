#include "Item.h"

Item::Item(ui::Window* pWindow):
    ui::ControlDragableT<ui::ListBoxItem>(pWindow),
    m_pListBox(nullptr),
    m_pImageControl(nullptr),
    m_pTitleLabel(nullptr),
    m_pProgressControl(nullptr),
    m_pDelBtn(nullptr)
{
    //The item layout is built with pure code (corresponding to the item.xml template; the template XML is no longer loaded)
    //The Item style classes are registered by ListBoxForm::OnInitWindow via AddClass
    SetClass(_T("list_box_item_test list_box_item_checkbox_1"));
    SetAttribute(_T("height"), _T("auto"));

    ui::HBox* pRow = new ui::HBox(pWindow);
    pRow->SetAttribute(_T("height"), _T("auto"));
    pRow->SetAttribute(_T("mouse_enabled"), _T("false"));
    pRow->SetAttribute(_T("padding"), _T("18,5,10,5"));
    AddItem(pRow);

    m_pImageControl = new ui::Control(pWindow);
    m_pImageControl->SetName(_T("control_img"));
    m_pImageControl->SetAttribute(_T("width"), _T("auto"));
    m_pImageControl->SetAttribute(_T("height"), _T("auto"));
    m_pImageControl->SetAttribute(_T("margin"), _T("0,0,10"));
    m_pImageControl->SetAttribute(_T("mouse_enabled"), _T("false"));
    pRow->AddItem(m_pImageControl);

    ui::VBox* pRight = new ui::VBox(pWindow);
    pRight->SetAttribute(_T("margin"), _T("0,3,0,5"));
    pRight->SetAttribute(_T("mouse_enabled"), _T("false"));
    pRow->AddItem(pRight);

    ui::HBox* pTitleRow = new ui::HBox(pWindow);
    pTitleRow->SetAttribute(_T("height"), _T("auto"));
    pTitleRow->SetAttribute(_T("mouse_enabled"), _T("false"));
    pRight->AddItem(pTitleRow);

    m_pTitleLabel = new ui::Label(pWindow);
    m_pTitleLabel->SetName(_T("label_title"));
    m_pTitleLabel->SetAttribute(_T("width"), _T("stretch"));
    m_pTitleLabel->SetAttribute(_T("mouse_enabled"), _T("false"));
    pTitleRow->AddItem(m_pTitleLabel);

    m_pDelBtn = new ui::Button(pWindow);
    m_pDelBtn->SetClass(_T("btn_recycle"));
    m_pDelBtn->SetName(_T("btn_del"));
    m_pDelBtn->SetAttribute(_T("width"), _T("auto"));
    m_pDelBtn->SetAttribute(_T("height"), _T("auto"));
    m_pDelBtn->SetToolTipText(_T("Delete"));
    m_pDelBtn->SetAttribute(_T("margin"), _T("0,0,4,0"));
    pTitleRow->AddItem(m_pDelBtn);

    ui::Control* pStretch = new ui::Control(pWindow);
    pStretch->SetAttribute(_T("height"), _T("stretch"));
    pStretch->SetAttribute(_T("mouse_enabled"), _T("false"));
    pRight->AddItem(pStretch);

    m_pProgressControl = new ui::Progress(pWindow);
    m_pProgressControl->SetClass(_T("progress_horizontal_blue"));
    m_pProgressControl->SetName(_T("progress"));
    m_pProgressControl->SetAttribute(_T("value"), _T("30"));
    m_pProgressControl->SetAttribute(_T("mouse_enabled"), _T("false"));
    pRight->AddItem(m_pProgressControl);
}

Item::~Item()
{
}

void Item::InitSubControls(const DString& img, const DString& title)
{
    // Find the controls under Item (the child controls are already built in code in the constructor; the names match the template)
    m_pImageControl = dynamic_cast<ui::Control*>(FindSubControl(_T("control_img")));
    m_pTitleLabel = dynamic_cast<ui::Label*>(FindSubControl(_T("label_title")));
    m_pProgressControl = dynamic_cast<ui::Progress*>(FindSubControl(_T("progress")));
    m_pDelBtn = dynamic_cast<ui::Button*>(FindSubControl(_T("btn_del")));

    // Simulate the progress bar value
    int32_t nProgress = std::time(nullptr) % 100;
    m_pProgressControl->SetValue((double)(nProgress));

    // Set the icon and task name
    m_pImageControl->SetBkImage(img);
    m_pTitleLabel->SetText(ui::StringUtil::Printf(_T("%s %d%%"), title.c_str(), nProgress));

    // Bind the delete-task handler
    m_pDelBtn->AttachClick(UiBind(&Item::OnRemove, this, std::placeholders::_1));
}

bool Item::OnRemove(const ui::EventArgs& args)
{
    ui::ListBox* parent = dynamic_cast<ui::ListBox*>(this->GetParent());
    if (parent != nullptr) {
        parent->RemoveItem(this);
    }
    return true;
}
