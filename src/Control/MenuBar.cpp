#include "dui/Control/MenuBar.h"
#include "dui/Control/Button.h"

namespace ui
{
class MenuBarButton : public ButtonBox
{
    typedef ButtonBox BaseClass;
public:
    explicit MenuBarButton(MenuBar* pMenuBar);
public:
    //Mouse events
    virtual bool MouseEnter(const EventArgs& msg) override;
    virtual bool MouseLeave(const EventArgs& msg) override;
    virtual bool ButtonDown(const EventArgs& msg) override;
    virtual bool ButtonUp(const EventArgs& msg) override;

private:
    MenuBar* m_pMenuBar;
};

MenuBarButton::MenuBarButton(MenuBar* pMenuBar) :
    ButtonBox(pMenuBar->GetWindow()),
    m_pMenuBar(pMenuBar)
{
    //Use RichText mode
    //SetRichText(true);
}

bool MenuBarButton::MouseEnter(const EventArgs& msg)
{
    bool bRet = BaseClass::MouseEnter(msg);
    m_pMenuBar->OnMenuMouseEnter(this, msg);
    return bRet;
}

bool MenuBarButton::MouseLeave(const EventArgs& msg)
{
    bool bRet = BaseClass::MouseLeave(msg);
    m_pMenuBar->OnMenuMouseLeave(this, msg);
    return bRet;
}

bool MenuBarButton::ButtonDown(const EventArgs& msg)
{
    bool bRet = BaseClass::ButtonDown(msg);
    m_pMenuBar->OnMenuMouseButtonDown(this, msg);
    return bRet;
}

bool MenuBarButton::ButtonUp(const EventArgs& msg)
{
    bool bRet = BaseClass::ButtonUp(msg);
    m_pMenuBar->OnMenuMouseButtonUp(this, msg);
    return bRet;
}

////////////////////////////////////////////////////////////////////////////////
MenuBar::MenuBar(Window* pWindow):
    HBox(pWindow),
    m_nItemDataId(0),
    m_bActiveState(false),
    m_bEnableBtnActive(false)
{
}

DString MenuBar::GetType() const { return DUI_CTR_MENU_BAR; }

void MenuBar::OnInit()
{
    if (IsInited()) {
        return;
    }
    BaseClass::OnInit();

    //Initialize
    for (TopMenuData& menuData : m_topMenuList) {
        menuData.m_nItemDataId = ++m_nItemDataId;
        AddTopMenuToUI(menuData, GetItemCount());
    }
}

void MenuBar::AddTopMenuToUI(const TopMenuData& menuData, size_t nInsertItem)
{
    MenuBarButton* pNewItem = new MenuBarButton(this);
    if (!menuData.m_menuTextId.empty()) {
        pNewItem->SetTextId(menuData.m_menuTextId.c_str());
    }
    else {
        pNewItem->SetText(menuData.m_menuText.c_str());
    }
    bool bAdded = AddItemAt(pNewItem, nInsertItem);
    ASSERT(bAdded);
    if (bAdded) {
        pNewItem->SetUserDataID(menuData.m_nItemDataId);
        if (!menuData.m_menuTextButtonClass.empty()) {
            pNewItem->SetClass(menuData.m_menuTextButtonClass.c_str());
        }
        else {
            //Use the default value
            pNewItem->SetClass(_T("menu_bar_button"));
        }
        if (!menuData.m_menuTextButtonAttributes.empty()) {
            pNewItem->ApplyAttributeList(menuData.m_menuTextButtonAttributes.c_str());
        }
    }

    pNewItem->AttachClick([this, pNewItem](const EventArgs&) {
        //When the button is clicked
        if (m_bEnableBtnActive) {
            ShowPopupMenu(pNewItem);
        }        
        return true;
        });
}

void MenuBar::RemoveTopMenuFromUI(const TopMenuData& menuData)
{
    const size_t nItemCount = GetItemCount();
    for (size_t nItem = 0; nItem < nItemCount; ++nItem) {
        Control* pItem = GetItemAt(nItem);
        if ((pItem != nullptr) && (pItem->GetUserDataID() == menuData.m_nItemDataId)) {
            RemoveItem(pItem);
            break;
        }
    }
}

int32_t MenuBar::AddTopMenu(const DString& menuItemId,
                            const DString& menuText,
                            const DString& menuTextId,
                            const DString& menuXmlPath,
                            const DString& menuTextButtonClass,
                            const DString& menuTextButtonAttributes)
{
    return InsertTopMenu((int32_t)m_topMenuList.size(), menuItemId, menuText, menuTextId, menuXmlPath, menuTextButtonClass, menuTextButtonAttributes);
}

int32_t MenuBar::AddTopMenu(const MenuBarItem& menuBarItem)
{
    return InsertTopMenu((int32_t)m_topMenuList.size(), menuBarItem);
}

int32_t MenuBar::AddTopMenu(const DString& menuItemId,
                            const DString& menuText,
                            const std::function<void(Menu*)>& menuBuilder,
                            const DString& menuTextButtonClass,
                            const DString& menuTextButtonAttributes)
{
    ASSERT(!(menuText.empty()) && (menuBuilder != nullptr));
    if (menuText.empty() || (menuBuilder == nullptr)) {
        return -1;
    }
    int32_t nMenuIndex = (int32_t)m_topMenuList.size();

    TopMenuData menuData;
    menuData.m_menuItemId = menuItemId;
    menuData.m_menuText = menuText;
    menuData.m_menuXmlPath = _T("");
    menuData.m_menuTextButtonClass = menuTextButtonClass;
    menuData.m_menuTextButtonAttributes = menuTextButtonAttributes;
    menuData.m_menuBuilder = menuBuilder;
    if (IsInited()) {
        menuData.m_nItemDataId = ++m_nItemDataId;
    }
    m_topMenuList.push_back(menuData);
    if (IsInited()) {
        //Add to the UI
        AddTopMenuToUI(menuData, nMenuIndex);
    }
    return nMenuIndex;
}

int32_t MenuBar::InsertTopMenu(int32_t nMenuIndex, const MenuBarItem& menuBarItem)
{
    return InsertTopMenu(nMenuIndex,
                         menuBarItem.m_menuItemId,
                         menuBarItem.m_menuText,
                         menuBarItem.m_menuTextId,
                         menuBarItem.m_menuXmlPath,
                         menuBarItem.m_menuTextButtonClass,
                         menuBarItem.m_menuTextButtonAttributes);
}

int32_t MenuBar::InsertTopMenu(int32_t nMenuIndex,
                               const DString& menuItemId,
                               const DString& menuText,
                               const DString& menuTextId,
                               const DString& menuXmlPath,
                               const DString& menuTextButtonClass,
                               const DString& menuTextButtonAttributes)
{
    ASSERT(!(menuText.empty() && menuTextId.empty()) && !menuXmlPath.empty());
    if ((menuText.empty() && menuTextId.empty()) || menuXmlPath.empty()) {
        return -1;
    }
    if ((nMenuIndex < 0) || (nMenuIndex > (int32_t)m_topMenuList.size())) {
        nMenuIndex = (int32_t)m_topMenuList.size();
    }

    TopMenuData menuData;
    menuData.m_menuItemId = menuItemId;
    menuData.m_menuText = menuText;
    menuData.m_menuTextId = menuTextId;
    menuData.m_menuXmlPath = menuXmlPath;
    menuData.m_menuTextButtonClass = menuTextButtonClass;
    menuData.m_menuTextButtonAttributes = menuTextButtonAttributes;
    if (IsInited()) {
        menuData.m_nItemDataId = ++m_nItemDataId;
    }
    int32_t nRetIndex = 0;
    if (nMenuIndex < (int32_t)m_topMenuList.size()) {
        m_topMenuList.insert(m_topMenuList.begin() + nMenuIndex, menuData);
        nRetIndex = nMenuIndex;
    }
    else {
        m_topMenuList.push_back(menuData);
        nRetIndex = (int32_t)m_topMenuList.size() - 1;
    }
    if (IsInited()) {
        //Add to the UI
        AddTopMenuToUI(menuData, nMenuIndex);
    }
    return nRetIndex;
}

bool MenuBar::GetTopMenu(int32_t nMenuIndex, MenuBarItem& menuBarItem)
{
    if ((nMenuIndex >= 0) && (nMenuIndex < (int32_t)m_topMenuList.size())) {
        const TopMenuData& menuData = m_topMenuList[nMenuIndex];
        menuBarItem.m_menuItemId = menuData.m_menuItemId.c_str();
        menuBarItem.m_menuText = menuData.m_menuText.c_str();
        menuBarItem.m_menuTextId = menuData.m_menuTextId.c_str();
        menuBarItem.m_menuXmlPath = menuData.m_menuXmlPath.c_str();
        menuBarItem.m_menuTextButtonClass = menuData.m_menuTextButtonClass.c_str();
        menuBarItem.m_menuTextButtonAttributes = menuData.m_menuTextButtonAttributes.c_str();
        return true;
    }
    return false;
}

bool MenuBar::RemoveTopMenu(int32_t nMenuIndex)
{
    if ((nMenuIndex >= 0) && (nMenuIndex < (int32_t)m_topMenuList.size())) {
        TopMenuData menuData = m_topMenuList[nMenuIndex];        
        m_topMenuList.erase(m_topMenuList.begin() + nMenuIndex);
        //Remove from the UI
        RemoveTopMenuFromUI(menuData);
        return true;
    }
    return false;
}

bool MenuBar::SetActiveTopMenuIndex(int32_t nMenuIndex)
{
    ASSERT((nMenuIndex >= 0) && (nMenuIndex < (int32_t)m_topMenuList.size()));
    if ((nMenuIndex >= 0) && (nMenuIndex < (int32_t)m_topMenuList.size())) {
        const TopMenuData& menuData = m_topMenuList[nMenuIndex];
        for (size_t nIndex = 0; nIndex < GetItemCount(); ++nIndex) {
            MenuBarButton* pItem = dynamic_cast<MenuBarButton*>(GetItemAt(nIndex));
            if ((pItem != nullptr) && pItem->IsVisible() && pItem->IsEnabled()) {
                if (menuData.m_nItemDataId == pItem->GetUserDataID()) {
                    m_bEnableBtnActive = true;
                    pItem->Activate(nullptr);
                    m_bEnableBtnActive = false;
                    return true;
                }
            }
        }
    }
    return false;
}

int32_t MenuBar::GetActiveTopMenuIndex() const
{
    if ((m_pActiveMenu != nullptr) && m_pActiveMenu->IsWindow() && !m_pActiveMenu->IsClosingWnd()) {
        Control* pRelatedControl = m_pActiveMenu->GetRelatedControl();
        if (pRelatedControl != nullptr) {
            MenuBarButton* pItem = dynamic_cast<MenuBarButton*>(pRelatedControl);
            if (pItem != nullptr) {
                for (int32_t nMenuIndex = 0; nMenuIndex < (int32_t)m_topMenuList.size(); ++nMenuIndex) {
                    const TopMenuData& menuData = m_topMenuList[nMenuIndex];
                    if (menuData.m_nItemDataId == pItem->GetUserDataID()) {
                        return nMenuIndex;
                    }
                }
            }
        }
    }
    return -1;
}

void MenuBar::AttachMenuBarItemActivated(MenuBarItemActivatedEvent callback)
{
    if (callback != nullptr) {
        m_callbackList.push_back(callback);
    }
}

void MenuBar::ClearMenuBarItemActivated()
{
    m_callbackList.clear();
}

void MenuBar::CheckShowPopupMenu(MenuBarButton* pButton)
{
    if (HasActivePopupMenu()) {
        bool bPopup = true;
        if ((m_pActiveMenu != nullptr) && (m_pActiveMenu->GetRelatedControl() == pButton)) {
            //Already popped up, no need to pop up again
            bPopup = false;
            if ((pButton != nullptr) && (pButton->GetState() != ui::kControlStatePushed)) {
                pButton->SetState(ui::kControlStatePushed);
            }
        }
        if (bPopup) {
            ShowPopupMenu(pButton);
        }
    }
}

void MenuBar::OnMenuMouseEnter(MenuBarButton* pButton, const EventArgs& /*msg*/)
{
    if (pButton == nullptr) {
        return;
    }
    size_t nItemDataId = pButton->GetUserDataID();
    for (TopMenuData& menuData : m_topMenuList) {
        if (menuData.m_nItemDataId == nItemDataId) {
            if (menuData.m_bMouseEnter) {
                //Already executed, do not repeat the popup menu operation
                return;
            }
            menuData.m_bMouseEnter = true;
        }
        else {
            menuData.m_bMouseEnter = false;
        }
    }

    //Check and pop up the menu
    CheckShowPopupMenu(pButton);
}

void MenuBar::OnMenuMouseLeave(MenuBarButton* pButton, const EventArgs& /*msg*/)
{
    if (HasActivePopupMenu()) {
        if ((m_pActiveMenu != nullptr) && (m_pActiveMenu->GetRelatedControl() == pButton)) {
            //The menu has already popped up, keep the state unchanged
            if ((pButton != nullptr) && (pButton->GetState() != ui::kControlStatePushed)) {
                pButton->SetState(ui::kControlStatePushed);
            }
        }
    }
}

void MenuBar::OnMenuMouseButtonDown(MenuBarButton* pButton, const EventArgs& /*msg*/)
{
    m_bActiveState = !m_bActiveState;//Toggle effect when the mouse button is pressed
    if (m_bActiveState) {
        //Pop up the menu
        ShowPopupMenu(pButton);
    }
    else {
        //Already popped up, hide it
        HidePopupMenu(pButton);
    }
}

void MenuBar::OnMenuMouseButtonUp(MenuBarButton* /*pButton*/, const EventArgs& /*msg*/)
{
}

void MenuBar::ShowPopupMenu(MenuBarButton* pButton)
{
    ASSERT(pButton != nullptr);
    if (pButton == nullptr) {
        return;
    }
    Window* pWindow = GetWindow();
    ASSERT(pWindow != nullptr);
    if (pWindow == nullptr) {
        return;
    }
    TopMenuData topMenuData;
    bool bFoundMenu = false;
    size_t nItemDataId = pButton->GetUserDataID();
    for (const TopMenuData& menuData : m_topMenuList) {
        if (menuData.m_nItemDataId == nItemDataId) {
            bFoundMenu = true;
            topMenuData = menuData;
        }
    }
    ASSERT(bFoundMenu);
    if (!bFoundMenu) {
        return;
    }

    UiPoint point(pButton->GetRect().left, pButton->GetRect().top);
    point.Offset(0, pButton->GetRect().Height());
    ClientToScreen(point);

    //If already expanded, close it
    HidePopupMenu(pButton);

    Menu* pMenu = new ui::Menu(pWindow, pButton, this);//The parent window needs to be set; otherwise, when the menu pops up, the program will become inactive
    m_pActiveMenu = pMenu;

    if (topMenuData.m_menuBuilder) {
        //Pure-code mode: no XML template, menu items are added by the callback function
        pMenu->ShowMenu(_T(""), point);
        topMenuData.m_menuBuilder(pMenu);
    }
    else {
        pMenu->SetSkinFolder(pWindow->GetResourcePath().ToString());
        pMenu->ShowMenu(topMenuData.m_menuXmlPath.c_str(), point);
    }
    pMenu->SetWindowFocus();

    std::weak_ptr<WeakFlag> menuBarFlag = GetWeakFlag();
    //Menu command event response
    DString menuItemId = topMenuData.m_menuItemId.c_str();
    MenuItemActivatedEvent callback = [this, menuBarFlag, menuItemId](const DString& menuName, int32_t nMenuLevel,
                                                                      const DString& itemName, size_t nItemIndex) {
            //Menu command activated, notify the application layer
            if (!menuBarFlag.expired()) {
                DString activeMenuName = menuName;
                int32_t activeMenuLevel = nMenuLevel;
                DString activeItemName = itemName;
                size_t activeItemIndex = nItemIndex;
                std::vector<MenuBarItemActivatedEvent> callbackList(m_callbackList);
                for (MenuBarItemActivatedEvent callback : callbackList) {
                    if (callback) {
                        callback(menuItemId,
                            activeMenuName, activeMenuLevel,
                            activeItemName, activeItemIndex);
                    }
                }
            }
        };
    pMenu->AttachMenuItemActivated(callback);

    //When the menu closes, reset the activation state
    pMenu->AttachWindowCloseMsg([this, pMenu, menuBarFlag](const EventArgs& /*args*/) {
            if (!menuBarFlag.expired() && (m_pActiveMenu == pMenu)) {
                m_bActiveState = false;
            }
            return true;
        });
}

void MenuBar::HidePopupMenu(MenuBarButton* /*pButton*/)
{
    if ((m_pActiveMenu != nullptr) && m_pActiveMenu->IsWindow() && !m_pActiveMenu->IsClosingWnd()) {
        m_pActiveMenu->CloseMenu();        
    }
    m_pActiveMenu = nullptr;
}

bool MenuBar::HasActivePopupMenu() const
{
    if ((m_pActiveMenu != nullptr) && m_pActiveMenu->IsWindow() && !m_pActiveMenu->IsClosingWnd()) {
        return true;
    }
    return false;
}

void MenuBar::OnMenuKeyDownMsg(Menu* pMenu, VirtualKeyCode vkCode, uint32_t /*modifierKey*/)
{
    if ((pMenu == nullptr) || !HasActivePopupMenu()) {
        return;
    }
    size_t nStartItemIndex = 0;
    std::vector<size_t> itemIndexList;
    for (size_t nIndex = 0; nIndex < GetItemCount(); ++nIndex) {
        MenuBarButton* pItem = dynamic_cast<MenuBarButton*>(GetItemAt(nIndex));
        if ((pItem != nullptr) && pItem->IsVisible() && pItem->IsEnabled()) {
            itemIndexList.push_back(nIndex);
            if (pMenu->GetRelatedControl() == pItem) {
                nStartItemIndex = nIndex;
            }
        }
    }
    if (vkCode == kVK_RIGHT) {
        size_t nNextMenuIndex = nStartItemIndex + 1;
        if (nNextMenuIndex >= itemIndexList.size()) {
            nNextMenuIndex = 0;
        }
        if (nNextMenuIndex < itemIndexList.size()) {
            MenuBarButton* pItem = dynamic_cast<MenuBarButton*>(GetItemAt(nNextMenuIndex));
            if (pItem != nullptr) {
                HidePopupMenu(nullptr);
                m_bEnableBtnActive = true;
                pItem->Activate(nullptr);
                m_bEnableBtnActive = false;
            }
        }
    }
    else if (vkCode == kVK_LEFT) {
        size_t nNextMenuIndex = nStartItemIndex - 1;
        if (nStartItemIndex == 0) {
            nNextMenuIndex = itemIndexList.size() - 1;
        }
        if (nNextMenuIndex >= itemIndexList.size()) {
            nNextMenuIndex = 0;
        }
        if (nNextMenuIndex < itemIndexList.size()) {
            MenuBarButton* pItem = dynamic_cast<MenuBarButton*>(GetItemAt(nNextMenuIndex));
            if (pItem != nullptr) {
                HidePopupMenu(nullptr);
                m_bEnableBtnActive = true;
                pItem->Activate(nullptr);
                m_bEnableBtnActive = false;
            }
        }
    }
}

} // namespace ui
