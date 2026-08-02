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
