#include "AppItemUi.h"
#include "AppDb.h"

AppItemUi* AppItemUi::Create(const AppItem& item, Box* p)
{
    ASSERT(p != nullptr);
    if (p == nullptr) {
        return nullptr;
    }
    ASSERT(p->GetWindow() != nullptr);
    AppItemUi* uiItem = new AppItemUi(p->GetWindow());
    uiItem->SetAppdata(item, false);
    // The Item template is built in code (app_item.xml is no longer loaded)
    return uiItem;
}

AppItemUi::AppItemUi(ui::Window* pWindow) :
    ui::ControlDragableT<ui::VBox>(pWindow)
{
    // Build the item layout in pure code (corresponding to the app_item.xml template)
    SetAttribute("height", "90");
    SetAttribute("width", "90");
    SetAttribute("mousechild", "false");
    SetBkColor("white");

    m_app_icon = new ui::Control(pWindow);
    m_app_icon->SetName("app_icon");
    m_app_icon->SetAttribute("halign", "center");
    m_app_icon->SetAttribute("width", "35");
    m_app_icon->SetAttribute("height", "35");
    m_app_icon->SetAttribute("margin", "0,10");
    AddItem(m_app_icon);

    m_app_name = new ui::Label(pWindow);
    m_app_name->SetName("app_name");
    m_app_name->SetAttribute("font", "system_14");
    m_app_name->SetAttribute("halign", "center");
    m_app_name->SetAttribute("margin", "0,5");
    AddItem(m_app_name);
}

void AppItemUi::OnInit()
{
    if (IsInited()) {
        return;
    }
    BaseClass::OnInit();
    m_app_icon = static_cast<ui::Control*>(FindSubControl("app_icon"));
    if (m_app_icon) {
        m_app_icon->SetBkImage(m_app_data.m_icon);
    }
    m_app_name = static_cast<ui::Label*>(FindSubControl("app_name"));
    if (m_app_name) {
        m_app_name->SetText(m_app_data.m_name);
    }

    // Bind events
}

void AppItemUi::SetAppdata(const AppItem& item, bool refresh)
{
    m_app_data = item;
    if (refresh) {
        if (m_app_icon) {
            m_app_icon->SetBkImage(m_app_data.m_icon);
        }
        if (m_app_name) {
            m_app_name->SetText(m_app_data.m_name);
        }
    }
}

ui::Control* AppItemUi::CreateDestControl(Box* pTargetBox)
{
    Control* pControl = BaseClass::CreateDestControl(pTargetBox);
    if (pControl != nullptr) {
        pControl->SetBkImage("file='move_control/1.png'");
    }
    return pControl;
}

void AppItemUi::OnItemBoxChanged(Box* /*pOldBox*/, size_t /*nOldItemIndex*/,
                                 Box* pNewBox, size_t /*nNewItemIndex*/)
{
    if (pNewBox == m_pFrequentBox) {
        // Frequent apps
        m_app_data.m_isFrequent = true;
    }
    else {
        // My apps
        m_app_data.m_isFrequent = false;
    }
    AppDb::GetInstance().SaveToDb(m_app_data);
}

void AppItemUi::SetFrequentBox(ui::Box* pFrequentBox)
{
    m_pFrequentBox = pFrequentBox;
}
