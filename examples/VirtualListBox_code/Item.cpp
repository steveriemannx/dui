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
    //Build the subtree bottom-up, matching how item.xml is loaded: a container is
    //attached only after its children are populated, and the item root's own
    //class/height attributes are applied last.
    auto* pRow = ui::Create<ui::HBox>(pWindow, {{"mouse_enabled", "false"}, {"padding", "10,5,10,5"}});

    // Build the child controls, but keep the member pointers null until
    // InitSubControls() finds them, exactly like the item.xml-loaded Item:
    // this is what triggers the one-time icon/progress/delete binding setup.
    ui::Control* pImageControl = new ui::Control(pWindow);
    pImageControl->SetName("control_img");
    pImageControl->SetAttribute("width", "auto");
    pImageControl->SetAttribute("height", "auto");
    pImageControl->SetAttribute("margin", "0,0,10,0");
    pImageControl->SetAttribute("mouse_enabled", "false");
    pRow->AddItem(pImageControl);

    ui::VBox* pRight = new ui::VBox(pWindow);
    pRight->SetAttribute("margin", "0,3,0,5");
    pRight->SetAttribute("mouse_enabled", "false");

    ui::HBox* pTitleRow = new ui::HBox(pWindow);
    pTitleRow->SetAttribute("height", "auto");
    pTitleRow->SetAttribute("mouse_enabled", "false");

    ui::Label* pTitleLabel = new ui::Label(pWindow);
    pTitleLabel->SetName("label_title");
    pTitleLabel->SetAttribute("width", "stretch");
    pTitleLabel->SetAttribute("mouse_enabled", "false");
    pTitleRow->AddItem(pTitleLabel);

    ui::Button* pDelBtn = new ui::Button(pWindow);
    pDelBtn->SetClass("btn_recycle");
    pDelBtn->SetName("btn_del");
    pDelBtn->SetAttribute("width", "auto");
    pDelBtn->SetAttribute("height", "auto");
    pDelBtn->SetToolTipText("Delete");
    pTitleRow->AddItem(pDelBtn);

    pRight->AddItem(pTitleRow);

    ui::Control* pStretch = new ui::Control(pWindow);
    pStretch->SetAttribute("height", "stretch");
    pStretch->SetAttribute("mouse_enabled", "false");
    pRight->AddItem(pStretch);

    ui::Progress* pProgressControl = new ui::Progress(pWindow);
    pProgressControl->SetClass("progress_horizontal_blue");
    pProgressControl->SetName("progress");
    pProgressControl->SetAttribute("value", "30");
    pProgressControl->SetAttribute("mouse_enabled", "false");
    pRight->AddItem(pProgressControl);

    // Attach the completed right column and row, then apply the root item style.
    pRow->AddItem(pRight);
    AddItem(pRow);

    SetClass("listitem");
    SetAttribute("height", "auto");
}


Item::~Item()
= default;

void Item::InitSubControls(const std::string& img, const std::string& title, size_t nDataIndex)
{
    // Find the controls under Item
    if (m_pImageControl == nullptr) {
        m_pImageControl = dynamic_cast<ui::Control*>(FindSubControl("control_img"));
        m_pTitleLabel = dynamic_cast<ui::Label*>(FindSubControl("label_title"));
        m_pProgressControl = dynamic_cast<ui::Progress*>(FindSubControl("progress"));
        m_pDelBtn = dynamic_cast<ui::Button*>(FindSubControl("btn_del"));
        // Simulate the progress bar value
        t_time = std::chrono::steady_clock::now().time_since_epoch().count() / 1000;
        m_pProgressControl->SetValue((double)(t_time % 100));
        // Set the icon and task name
        m_pImageControl->SetBkImage(img);
        // Bind the delete-task handler
        m_pDelBtn->AttachClick(UiBind(&Item::OnRemove, this, std::placeholders::_1));
    }
    m_pTitleLabel->SetText(ui::StringUtil::Printf("%s %d%%", title.c_str(), t_time % 100));
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
