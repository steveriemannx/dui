#include "Item.h"
#include "DataProvider.h"
#include <chrono>

Item::Item(ui::Window* pWindow):
    ui::ListBoxItem(pWindow),
    m_pImageControl(nullptr),
    m_pTitleLabel(nullptr),
    m_pProgressControl(nullptr),
    m_pDelBtn(nullptr),
    t_time(0),
    m_nDataIndex(0)
{
    //The item layout is built with pure code (corresponding to the item.xml template; the template XML is no longer loaded)
    SetClass(_T("listitem"));
    SetAttribute(_T("height"), _T("auto"));

    ui::HBox* pRow = new ui::HBox(pWindow);
    pRow->SetAttribute(_T("mouse_enabled"), _T("false"));
    pRow->SetAttribute(_T("padding"), _T("10,5,10,5"));
    AddItem(pRow);

    m_pImageControl = new ui::Control(pWindow);
    m_pImageControl->SetName(_T("control_img"));
    m_pImageControl->SetAttribute(_T("width"), _T("auto"));
    m_pImageControl->SetAttribute(_T("height"), _T("auto"));
    m_pImageControl->SetAttribute(_T("margin"), _T("0,0,10,0"));
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
= default;

void Item::InitSubControls(const DString& img, const DString& title, size_t nDataIndex)
{
    // Find the controls under Item
    if (m_pImageControl == nullptr) {
        m_pImageControl = dynamic_cast<ui::Control*>(FindSubControl(_T("control_img")));
        m_pTitleLabel = dynamic_cast<ui::Label*>(FindSubControl(_T("label_title")));
        m_pProgressControl = dynamic_cast<ui::Progress*>(FindSubControl(_T("progress")));
        m_pDelBtn = dynamic_cast<ui::Button*>(FindSubControl(_T("btn_del")));
        // Simulate the progress bar value
        t_time = std::chrono::steady_clock::now().time_since_epoch().count() / 1000;
        m_pProgressControl->SetValue((double)(t_time % 100));
        // Set the icon and task name
        m_pImageControl->SetBkImage(img);
        // Bind the delete-task handler
        m_pDelBtn->AttachClick(UiBind(&Item::OnRemove, this, std::placeholders::_1));
    }
    m_pTitleLabel->SetText(ui::StringUtil::Printf(_T("%s %d%%"), title.c_str(), t_time % 100));
    m_nDataIndex = nDataIndex;
}


bool Item::OnRemove(const ui::EventArgs& args)
{
    // When removing, only the data needs to be deleted; do not remove the UI elements
    ui::VirtualListBox* pTileBox = dynamic_cast<ui::VirtualListBox*>(GetOwner());
    ASSERT(pTileBox != nullptr);
    if (pTileBox != nullptr) {
        DataProvider* pProvider = dynamic_cast<DataProvider*>(pTileBox->GetDataProvider());
        ASSERT(pProvider != nullptr);
        if (pProvider != nullptr) {
            pProvider->RemoveTask(m_nDataIndex);
            return true;
        }
    }
    return false;    
}
