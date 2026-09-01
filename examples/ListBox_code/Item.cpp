#include "Item.h"

Item::Item(ui::Window* pWindow):
    ui::ControlDragableT<ui::ListBoxItem>(pWindow),
    m_pListBox(nullptr),
    m_pImageControl(nullptr),
    m_pTitleLabel(nullptr),
    m_pProgressControl(nullptr),
    m_pDelBtn(nullptr)
{
    //Build the subtree bottom-up, matching how item.xml is loaded: a container is
    //attached only after its children are populated, and the item root's own
    //class/height attributes are applied last (FillBoxWithCache does the same).
    auto* pRow = ui::Create<ui::HBox>(pWindow, {{"height", "auto"}, {"mouse_enabled", "false"}, {"padding", "18,5,10,5"}});

    m_pImageControl = new ui::Control(pWindow);
    m_pImageControl->SetName("control_img");
    m_pImageControl->SetAttribute("width", "auto");
    m_pImageControl->SetAttribute("height", "auto");
    m_pImageControl->SetAttribute("margin", "0,0,10");
    m_pImageControl->SetAttribute("mouse_enabled", "false");
    pRow->AddItem(m_pImageControl);

    auto* pRight = ui::Create<ui::VBox>(pWindow, {{"margin", "0,3,0,5"}, {"mouse_enabled", "false"}});

    auto* pTitleRow = ui::Create<ui::HBox>(pWindow, {{"height", "auto"}, {"mouse_enabled", "false"}});

    m_pTitleLabel = new ui::Label(pWindow);
    m_pTitleLabel->SetName("label_title");
    m_pTitleLabel->SetAttribute("width", "stretch");
    m_pTitleLabel->SetAttribute("mouse_enabled", "false");
    pTitleRow->AddItem(m_pTitleLabel);

    m_pDelBtn = new ui::Button(pWindow);
    m_pDelBtn->SetClass("btn_recycle");
    m_pDelBtn->SetName("btn_del");
    m_pDelBtn->SetAttribute("width", "auto");
    m_pDelBtn->SetAttribute("height", "auto");
    m_pDelBtn->SetToolTipText("Delete");
    m_pDelBtn->SetAttribute("margin", "0,0,4,0");
    pTitleRow->AddItem(m_pDelBtn);

    pRight->AddItem(pTitleRow);

    auto* pStretch = ui::Create<ui::Control>(pWindow, {{"height", "stretch"}, {"mouse_enabled", "false"}});
    pRight->AddItem(pStretch);

    m_pProgressControl = new ui::Progress(pWindow);
    m_pProgressControl->SetClass("progress_horizontal_blue");
    m_pProgressControl->SetName("progress");
    m_pProgressControl->SetAttribute("value", "30");
    m_pProgressControl->SetAttribute("mouse_enabled", "false");
    pRight->AddItem(m_pProgressControl);

    // Attach the completed right column and row, then apply the root item style.
    pRow->AddItem(pRight);
    AddItem(pRow);

    SetClass("list_box_item_test list_box_item_checkbox_1");
    SetAttribute("height", "auto");
}

Item::~Item()
{
}

void Item::InitSubControls(const DString& img, const DString& title)
{
    // Find the controls under Item (the child controls are already built in code in the constructor; the names match the template)
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
