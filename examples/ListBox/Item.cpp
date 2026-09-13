#include "Item.h"

Item::Item(ui::Window* pWindow):
    ui::ControlDragableT<ui::ListBoxItem>(pWindow),
    m_pListBox(nullptr),
    m_pImageControl(nullptr),
    m_pTitleLabel(nullptr),
    m_pProgressControl(nullptr),
    m_pDelBtn(nullptr)
{
}

Item::~Item()
{
}

void Item::InitSubControls(const std::string& img, const std::string& title)
{
    // Find the controls under Item
    m_pImageControl = dynamic_cast<ui::Control*>(FindSubControl("control_img"));
    m_pTitleLabel = dynamic_cast<ui::Label*>(FindSubControl("label_title"));
    m_pProgressControl = dynamic_cast<ui::Progress*>(FindSubControl("progress"));
    m_pDelBtn = dynamic_cast<ui::Button*>(FindSubControl("btn_del"));

    // Simulate the progress bar value
    int32_t nProgress = std::time(nullptr) % 100;
    m_pProgressControl->SetValue((double)(nProgress));

    // Set the icon and task name
    m_pImageControl->SetBkImage(img);
    m_pTitleLabel->SetText(ui::StringUtil::Printf("%s %d%%", title.c_str(), nProgress));

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
