#include "duilib/Control/Menu.h"
#include "duilib/Control/MenuListBox.h"
#include "duilib/Control/MenuBar.h"
#include "duilib/Core/Keyboard.h"
#include "duilib/Core/WindowCreateParam.h"
#include "duilib/Utils/FilePathUtil.h"

namespace ui {

//TODO: Centralized management of static objects
ContextMenuObserver& Menu::GetMenuObserver()
{
    static ContextMenuObserver s_context_menu_observer;
    return s_context_menu_observer;
}

//Managed class for second-level or multi-level submenus
class SubMenu: public ui::ListBoxItem
{
public:
    explicit SubMenu(Window* pWindow):
        ListBoxItem(pWindow)
    {
    }
};

ui::Control* Menu::CreateControl(const DString& pstrClass)
{
    if (pstrClass == DUI_CTR_MENU_ITEM){
        return new MenuItem(this);
    }
    else if (pstrClass == DUI_CTR_SUB_MENU) {
        return new SubMenu(this);
    }
    else if (pstrClass == DUI_CTR_MENU_LISTBOX) {
        return new MenuListBox(this);
    }
    return nullptr;
}

bool Menu::Receive(ContextMenuParam param)
{
    switch (param.wParam)
    {
    case MenuCloseType::eMenuCloseAll:
        CloseMenu();
        break;
        case MenuCloseType::eMenuCloseThis:
        {
            Window* pParentWindow = GetParentWindow();
            while (pParentWindow != nullptr) {
                if (pParentWindow == param.pWindow) {
                    CloseMenu();
                    break;
                }
                pParentWindow = pParentWindow->GetParentWindow();
            }
        }
        break;
    default:
        break;
    }

    return true;
}

Menu::Menu(Window* pParentWindow, Control* pRelatedControl, MenuBar* pMenuBar):
    m_pParentWindow(pParentWindow),
    m_pRelatedControl(pRelatedControl),
    m_pMenuBar(pMenuBar),
    m_menuPoint({ 0, 0 }),
    m_popupPosType(MenuPopupPosType::RIGHT_TOP),
    m_noFocus(false),
    m_pOwner(nullptr),
    m_pListBox(nullptr)
{
    m_skinFolder = DString(_T("public/menu/"));
    m_submenuXml = DString(_T("submenu.xml"));
    m_submenuNodeName = DString(_T("submenu"));
}

void Menu::SetSkinFolder(const DString& skinFolder)
{
    m_skinFolder = skinFolder;
}

void Menu::SetSubMenuXml(const DString& submenuXml, const DString& submenuNodeName)
{
    m_submenuXml = submenuXml;
    m_submenuNodeName = submenuNodeName;
}

void Menu::ShowMenu(const DString& xml, const UiPoint& point, MenuPopupPosType popupPosType, bool noFocus, MenuItem* pOwner)
{
    m_menuPoint = point;
    m_popupPosType = popupPosType;

    if (xml.empty()) {
        //Pure-code mode: no XML template, all menu items are added by code
        m_skinFolder = _T("");
    }
    m_xml = xml;
    m_noFocus = noFocus;
    m_pOwner = pOwner;

    Menu::GetMenuObserver().AddReceiver(this);
    WindowCreateParam createWndParam;
    createWndParam.m_dwStyle = kWS_POPUP;
    createWndParam.m_dwExStyle = kWS_EX_TOPMOST | kWS_EX_LAYERED;
    if (m_pParentWindow == nullptr) {
        //When the parent window is empty, set the ToolWindow attribute to avoid appearing in the taskbar
        createWndParam.m_dwExStyle |= kWS_EX_TOOLWINDOW;
    }
    //Set the initial position to avoid a black screen when the menu is first displayed
    createWndParam.m_nX = point.x;
    createWndParam.m_nY = point.y;
    CreateWnd(m_pParentWindow, createWndParam);

    if (m_pMenuBar != nullptr) {
        //If displayed in the MenuBar, set padding.top to 0 to avoid the top shadow affecting the mouse-slide menu switching function
        UiPadding rcShadowCorner = GetShadowCorner();
        rcShadowCorner.top = 0;
        SetShadowCorner(rcShadowCorner);
    }
    
    bool bShown = false;
    if (m_pOwner) {
        bShown = ResizeSubMenu();
    }
    else {
        bShown = ResizeMenu();
    }
    if (!bShown) {
        if (noFocus) {
            ShowWindow(kSW_SHOW_NA);
        }
        else {
            ShowWindow(kSW_SHOW_NORMAL);
        }
    }
    KeepParentActive();
    //Fix the width of menu items to keep them consistent
    UpdateWindow();
    ListBox* pLayoutListBox = Menu::GetLayoutListBox();
    if (pLayoutListBox != nullptr) {
        std::vector<MenuItem*> allMenuItems;
        const size_t nItemCount = pLayoutListBox->GetItemCount();
        for (size_t i = 0; i < nItemCount; ++i) {
            MenuItem* pMenuItem = dynamic_cast<MenuItem*>(pLayoutListBox->GetItemAt(i));
            if (pMenuItem != nullptr) {
                allMenuItems.push_back(pMenuItem);
            }
        }
        int32_t nMaxWidth = 0;
        for (auto pMenuItem : allMenuItems) {
            if (pMenuItem == nullptr) {
                continue;
            }
            if (pMenuItem->GetFixedWidth().IsInt32()) {
                nMaxWidth = std::max(nMaxWidth, pMenuItem->GetFixedWidth().GetInt32());
            }
            else if (pMenuItem->GetFixedWidth().IsAuto()) {
                nMaxWidth = std::max(nMaxWidth, pMenuItem->GetWidth());
            }
        }
        if (nMaxWidth > 0) {
            for (auto pMenuItem : allMenuItems) {
                if (pMenuItem == nullptr) {
                    continue;
                }
                if (pMenuItem->GetFixedWidth().IsAuto() || pMenuItem->GetFixedWidth().IsInt32()) {
                    pMenuItem->SetFixedWidth(UiFixedInt(nMaxWidth), true, false);
                }
            }
        }        
    }
}

void Menu::CloseMenu()
{
    //Close immediately to avoid mutual interference during continuous operations
    CloseWnd();
}

void Menu::DetachOwner()
{
    if (m_pOwner != nullptr) {
        ListBox* pLayoutListBox = Menu::GetLayoutListBox();
        if (pLayoutListBox != nullptr) {
            pLayoutListBox->SelectItem(Box::InvalidIndex, false, false);
        }

        //Disconnect the association of the nodes that were added to the Layout in OnInitWindow
        std::vector<Control*> submenuControls;
        MenuItem::GetAllSubMenuControls(m_pOwner, submenuControls);
        for (auto pItem : submenuControls) {
            if (pItem != nullptr) {
                pItem->SetWindow(nullptr);
                pItem->SetParent(nullptr);
            }
        }

        if (pLayoutListBox != nullptr) {
            pLayoutListBox->RemoveAllItems();
        }
        m_pOwner->m_pSubWindow = nullptr;
        m_pOwner->Invalidate();
        m_pOwner = nullptr;
    }
}

DString Menu::GetSkinFolder()
{
    return m_skinFolder.c_str();
}

DString Menu::GetSkinFile() 
{
    return m_xml.c_str();
}

LRESULT Menu::OnKillFocusMsg(WindowBase* pSetFocusWindow, const NativeMsg& nativeMsg, bool& bHandled)
{
    LRESULT lResult = BaseClass::OnKillFocusMsg(pSetFocusWindow, nativeMsg, bHandled);
    bHandled = true;
    bool bInMenuWindowList = false;
    if (pSetFocusWindow != nullptr) {
        ContextMenuObserver::Iterator<bool, ContextMenuParam> iterator(GetMenuObserver());
        ReceiverImplBase<bool, ContextMenuParam>* pReceiver = iterator.next();
        while (pReceiver != nullptr) {
            Menu* pContextMenu = dynamic_cast<Menu*>(pReceiver);
            if ((pContextMenu != nullptr) && (pContextMenu == pSetFocusWindow)) {
                bInMenuWindowList = true;
                break;
            }
            pReceiver = iterator.next();
        }
    }
    if (!bInMenuWindowList) {
        ContextMenuParam param;
        param.pWindow = this;
        param.wParam = MenuCloseType::eMenuCloseAll;
        GetMenuObserver().RBroadcast(param);
        return 0;
    }
    return lResult;
}

LRESULT Menu::OnKeyDownMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    if (vkCode == kVK_ESCAPE) {
        bHandled = true;
        CloseMenu();
    }
    else if (vkCode == kVK_LEFT) {
        if (m_pOwner != nullptr) {
            //Close the submenu
            bHandled = true;
            CloseMenu();
        }
        else {
            //Intercept this event and notify the MenuBar
            bHandled = true;
            if (m_pMenuBar != nullptr) {
                m_pMenuBar->OnMenuKeyDownMsg(this, vkCode, modifierKey);
            }
        }
    }
    else if (vkCode == kVK_RIGHT) {        
        ListBox* pLayoutListBox = Menu::GetLayoutListBox();
        if (pLayoutListBox != nullptr) {
            size_t index = pLayoutListBox->GetCurSel();
            MenuItem* pItem = dynamic_cast<MenuItem*>(pLayoutListBox->GetItemAt(index));
            if (pItem != nullptr) {
                if (pItem->CheckSubMenuItem()) {
                    //The submenu was expanded
                    bHandled = true;
                }
            }
        }
        if (!bHandled) {
            //Intercept this event and notify the MenuBar
            bHandled = true;
            if (m_pMenuBar != nullptr) {
                m_pMenuBar->OnMenuKeyDownMsg(this, vkCode, modifierKey);
            }
        }
    }
    else if (vkCode == kVK_RETURN || vkCode == kVK_SPACE) {
        bHandled = true;
        ListBox* pLayoutListBox = Menu::GetLayoutListBox();
        if (pLayoutListBox != nullptr) {
            size_t index = pLayoutListBox->GetCurSel();
            MenuItem* pItem = dynamic_cast<MenuItem*>(pLayoutListBox->GetItemAt(index));
            if (pItem != nullptr) {
                if (!pItem->CheckSubMenuItem()) {
                    ContextMenuParam param;
                    param.pWindow = this;
                    param.wParam = MenuCloseType::eMenuCloseAll;
                    //When Enter is pressed, activate the currently selected menu item
                    pItem->Activate(nullptr);
                    Menu::GetMenuObserver().RBroadcast(param);
                }
            }
        }
    }
    else if (vkCode == kVK_DOWN || vkCode == kVK_UP) {
        bHandled = true;
        //Support switching the current menu item with the keyboard up/down keys
        ListBox* pLayoutListBox = Menu::GetLayoutListBox();
        if ((pLayoutListBox != nullptr) && (pLayoutListBox->GetItemCount() > 0)) {
            //By default, select the menu item currently in the hot state to support keyboard operation
            size_t nCurSel = pLayoutListBox->GetCurSel();
            if (!Box::IsValidItemIndex(nCurSel)) {
                bool bFoundItem = false;
                for (size_t nIndex = 0; nIndex < pLayoutListBox->GetItemCount(); ++nIndex) {
                    MenuItem* pItem = dynamic_cast<MenuItem*>(pLayoutListBox->GetItemAt(nIndex));
                    if ((pItem != nullptr) && pItem->IsVisible() && pItem->IsEnabled()) {
                        if (pItem->GetState() == ControlStateType::kControlStateHot) {
                            pLayoutListBox->SelectItem(nIndex, false, false);
                            bFoundItem = true;
                            break;
                        }
                    }
                }
                if (!bFoundItem) {
                    //If no menu item in the Hot state is found
                    if (vkCode == kVK_DOWN) {
                        //Select the first one
                        for (size_t nIndex = 0; nIndex < pLayoutListBox->GetItemCount(); ++nIndex) {
                            MenuItem* pItem = dynamic_cast<MenuItem*>(pLayoutListBox->GetItemAt(nIndex));
                            if ((pItem != nullptr) && pItem->IsVisible() && pItem->IsEnabled()) {
                                pLayoutListBox->SelectItem(nIndex, false, false);
                                break;
                            }
                        }
                    }
                    else {
                        //Select the last one
                        int32_t nIndex = (int32_t)pLayoutListBox->GetItemCount() - 1;
                        for (; nIndex >= 0; --nIndex) {
                            MenuItem* pItem = dynamic_cast<MenuItem*>(pLayoutListBox->GetItemAt(nIndex));
                            if ((pItem != nullptr) && pItem->IsVisible() && pItem->IsEnabled()) {
                                pLayoutListBox->SelectItem(nIndex, false, false);
                                break;
                            }
                        }
                    }
                }
            }
            else {
                //Control the selection of the next menu item
                size_t nStartItemIndex = 0;
                std::vector<size_t> validMenuItemIndexList;
                for (size_t nIndex = 0; nIndex < pLayoutListBox->GetItemCount(); ++nIndex) {
                    MenuItem* pItem = dynamic_cast<MenuItem*>(pLayoutListBox->GetItemAt(nIndex));
                    if ((pItem != nullptr) && pItem->IsVisible() && pItem->IsEnabled()) {                        
                        if (nCurSel == nIndex) {
                            nStartItemIndex = validMenuItemIndexList.size();
                        }
                        validMenuItemIndexList.push_back(nIndex);
                    }
                }

                if (vkCode == kVK_DOWN) {
                    size_t nNextMenuItemIndex = nStartItemIndex + 1;
                    if (nNextMenuItemIndex >= validMenuItemIndexList.size()) {
                        nNextMenuItemIndex = 0;
                    }
                    if (nNextMenuItemIndex < validMenuItemIndexList.size()) {
                        pLayoutListBox->SelectItem(validMenuItemIndexList[nNextMenuItemIndex], false, false);
                    }
                }
                else {
                    size_t nNextMenuItemIndex = nStartItemIndex - 1;
                    if (nStartItemIndex == 0) {
                        nNextMenuItemIndex = validMenuItemIndexList.size() - 1;
                    }
                    if (nNextMenuItemIndex >= validMenuItemIndexList.size()) {
                        nNextMenuItemIndex = 0;
                    }
                    if (nNextMenuItemIndex < validMenuItemIndexList.size()) {
                        pLayoutListBox->SelectItem(validMenuItemIndexList[nNextMenuItemIndex], false, false);
                    }
                }
            }
        }
    }
    if (bHandled) {
        //Already handled
        return 0;
    }
    else {
        return BaseClass::OnKeyDownMsg(vkCode, modifierKey, nativeMsg, bHandled);
    }
}

LRESULT Menu::OnContextMenuMsg(const UiPoint& /*pt*/, const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    bHandled = true;
    return 0;
}

LRESULT Menu::OnMouseRButtonDownMsg(const UiPoint& /*pt*/, uint32_t /*modifierKey*/, const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    bHandled = true;
    return 0;
}

LRESULT Menu::OnMouseRButtonUpMsg(const UiPoint& /*pt*/, uint32_t /*modifierKey*/, const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    bHandled = true;
    return 0;
}

LRESULT Menu::OnMouseRButtonDbClickMsg(const UiPoint& /*pt*/, uint32_t /*modifierKey*/, const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    bHandled = true;
    return 0;
}

bool Menu::ResizeMenu()
{
    ui::Control* pRoot = GetRoot();
    ASSERT(pRoot != nullptr);
    if (pRoot == nullptr) {
        return false;
    }
    //Use the screen where the click is located as the primary screen
    ui::UiRect rcWork;
    GetMonitorWorkRect(m_menuPoint, rcWork);
    Dpi().WindowSizeToClientSize(rcWork);
    Dpi().WindowSizeToClientSize(m_menuPoint);

    ui::UiSize szMenuWindow = { rcWork.Width(), rcWork.Height()};
    UiEstSize estSize = pRoot->EstimateSize(szMenuWindow);   //The size returned here includes the size of the shadow
    if (estSize.cx.IsInt32()) {
        szMenuWindow.cx = estSize.cx.GetInt32();
    }
    if (estSize.cy.IsInt32()) {
        szMenuWindow.cy = estSize.cy.GetInt32();
    }

    UiPadding rcShadowCorner = pRoot->GetPadding(); //Area occupied by the window shadow
    ui::UiSize szMenuClient = szMenuWindow;
    szMenuClient.cx -= rcShadowCorner.left + rcShadowCorner.right;
    szMenuClient.cy -= rcShadowCorner.top + rcShadowCorner.bottom; //Remove the shadow window here, i.e., the visually effective area for the user

    ui::UiPoint point(m_menuPoint);  //There is a bug here: since the coordinate point is contained within the window, it will directly trigger mouseenter, causing the submenu to appear; offset by 1 pixel
    if (static_cast<int>(m_popupPosType) & static_cast<int>(eMenuAlignment_Right)) {
        point.x += -szMenuWindow.cx + rcShadowCorner.right + rcShadowCorner.left;
        point.x -= 1;
    }
    else if (static_cast<int>(m_popupPosType) & static_cast<int>(eMenuAlignment_Left)) {
        point.x += 1;
    }
    if (static_cast<int>(m_popupPosType) & static_cast<int>(eMenuAlignment_Bottom))    {
        point.y += -szMenuWindow.cy + rcShadowCorner.bottom + rcShadowCorner.top;
        point.y += 1;
    }
    else if (static_cast<int>(m_popupPosType) & static_cast<int>(eMenuAlignment_Top)) {
        point.y += 1;
    }
    if (static_cast<int>(m_popupPosType) & static_cast<int>(eMenuAlignment_Intelligent)) {
        if (point.x < rcWork.left) {
            point.x = rcWork.left;
        }
        else if (point.x + szMenuClient.cx> rcWork.right) {
            point.x = rcWork.right - szMenuClient.cx;
        }
        if (point.y < rcWork.top) {
            point.y = rcWork.top ;
        }
        else if (point.y + szMenuClient.cy > rcWork.bottom) {
            point.y = rcWork.bottom - szMenuClient.cy;
        }
    }

    int32_t x = point.x - rcShadowCorner.left;
    int32_t y = point.y - rcShadowCorner.top;
    Dpi().ClientSizeToWindowSize(x);
    Dpi().ClientSizeToWindowSize(y);
    Dpi().ClientSizeToWindowSize(szMenuWindow);
    Dpi().ClientSizeToWindowSize(m_menuPoint);
    SetWindowPos(InsertAfterWnd(InsertAfterFlag::kHWND_TOPMOST),
                 x, y,
                 szMenuWindow.cx, szMenuWindow.cy,
                 kSWP_SHOWWINDOW | (m_noFocus ? kSWP_NOACTIVATE : 0));

    if (!m_noFocus) {
        SetWindowForeground();
        ListBox* pLayoutListBox = Menu::GetLayoutListBox();
        SetFocusControl(pLayoutListBox);
    }
    return true;
}

bool Menu::ResizeSubMenu()
{
    ASSERT(m_pOwner != nullptr);
    if (m_pOwner == nullptr) {
        return false;
    }
    ASSERT(m_pOwner->GetWindow() != nullptr);

    // Position the popup window in absolute space
    UiRect rcOwner = m_pOwner->GetPos();
    UiRect rc = rcOwner;
   
    UiPadding rcCorner = GetCurrentShadowCorner();
    UiRect rcWindow;
    m_pOwner->GetWindow()->GetWindowRect(rcWindow);
    Dpi().WindowSizeToClientSize(rcWindow);

    UiRect rcClient;
    GetClientRect(rcClient);
    rcClient.Deflate(rcCorner);
    int32_t cxFixed = rcClient.Width();
    int32_t cyFixed = rcClient.Height();
    rcClient.Inflate(rcCorner);
    if (rcClient.Width() < (rcCorner.left + rcCorner.right)) {
        //The window size has not taken effect yet, so it needs to be estimated
        Box* pRoot = GetRoot();
        if (pRoot != nullptr) {
            UiSize maxSize(999999, 999999);
            UiEstSize estSize = pRoot->EstimateSize(maxSize);
            if (!estSize.cx.IsStretch() && !estSize.cy.IsStretch()) {
                UiSize needSize = MakeSize(estSize);
                if (needSize.cx < pRoot->GetMinWidth()) {
                    needSize.cx = pRoot->GetMinWidth();
                }
                if (needSize.cx > pRoot->GetMaxWidth()) {
                    needSize.cx = pRoot->GetMaxWidth();
                }
                if (needSize.cy < pRoot->GetMinHeight()) {
                    needSize.cy = pRoot->GetMinHeight();
                }
                if (needSize.cy > pRoot->GetMaxHeight()) {
                    needSize.cy = pRoot->GetMaxHeight();
                }
                cxFixed = needSize.cx - rcCorner.left - rcCorner.right;
                cyFixed = needSize.cy - rcCorner.top - rcCorner.bottom;
            }
        }
    }

    //Remove the shadow
    rcWindow.Deflate(rcCorner);

    m_pOwner->GetWindow()->ClientToScreen(rc);
    Dpi().WindowSizeToClientSize(rc);
   
    rc.left = rcWindow.right;
    rc.right = rc.left + cxFixed;
    rc.bottom = rc.top + cyFixed;

    bool bReachBottom = false;
    bool bReachRight = false;

    UiRect rcPreWindow;
    ContextMenuObserver::Iterator<bool, ContextMenuParam> iterator(GetMenuObserver());
    ReceiverImplBase<bool, ContextMenuParam>* pReceiver = iterator.next();
    while (pReceiver != nullptr) {
        Menu* pContextMenu = dynamic_cast<Menu*>(pReceiver);
        if (pContextMenu != nullptr) {
            pContextMenu->GetWindowRect(rcPreWindow);  //The shadow needs to be subtracted
            Dpi().WindowSizeToClientSize(rcPreWindow);

            bReachRight = (rcPreWindow.left + rcCorner.left) >= rcWindow.right;
            bReachBottom = (rcPreWindow.top + rcCorner.top) >= rcWindow.bottom;
            if (pContextMenu->GetWindowHandle() == m_pOwner->GetWindow()->GetWindowHandle()
                || bReachBottom || bReachRight) {
                break;
            }
        }
        pReceiver = iterator.next();
    }
    if (bReachBottom) {
        rc.bottom = rcWindow.top;
        rc.top = rc.bottom - cyFixed;
    }

    if (bReachRight) {
        rc.right = rcWindow.left;
        rc.left = rc.right - cxFixed;
    }

    UiRect rcWork;
    GetMonitorWorkRect(m_menuPoint, rcWork);
    Dpi().WindowSizeToClientSize(rcWork);

    if (rc.bottom > rcWork.bottom) {
        rc.bottom = rc.top;
        rc.top = rc.bottom - cyFixed;
    }

    if (rc.right > rcWork.right) {
        rc.right = rcWindow.left;
        rc.left = rc.right - cxFixed;
    }

    if (rc.top < rcWork.top) {
        rc.top = rcOwner.top;
        rc.bottom = rc.top + cyFixed;
    }

    if (rc.left < rcWork.left) {
        rc.left = rcWindow.right;
        rc.right = rc.left + cxFixed;
    }

    //Adjust the window position and show the window, but do not adjust the window size
    int32_t nNewWidth = rc.Width() + rcCorner.left + rcCorner.right;
    int32_t nNewHeight = rc.Height() + rcCorner.top + rcCorner.bottom;
    ASSERT(nNewWidth == rcClient.Width());
    ASSERT(nNewHeight == rcClient.Height());

    int32_t x = rc.left - rcCorner.left;
    int32_t y = rc.top - rcCorner.top;
    Dpi().ClientSizeToWindowSize(x);
    Dpi().ClientSizeToWindowSize(y);
    Dpi().ClientSizeToWindowSize(nNewWidth);
    Dpi().ClientSizeToWindowSize(nNewHeight);
    SetWindowPos(InsertAfterWnd(InsertAfterFlag::kHWND_TOPMOST),
                 x, y,
                 nNewWidth, nNewHeight,
                 kSWP_SHOWWINDOW | kSWP_NOSIZE | (m_noFocus ? kSWP_NOACTIVATE : 0));

    if (!m_noFocus) {
        SetWindowForeground();
        SetFocusControl(Menu::GetLayoutListBox());
    }
    return true;
}

void Menu::PreInitWindow()
{
    BaseClass::PreInitWindow();
    if (m_xml.empty() && (GetRoot() == nullptr)) {
        //Pure-code mode: when there is no XML template, build the root node layout (consistent with the MenuListBox in the XML template)
        MenuListBox* pListBox = new MenuListBox(this);
        pListBox->SetClass(_T("menu"));
        pListBox->SetAttribute(_T("name"), _T("main_menu"));
        AttachBox(pListBox);
    }
}

void Menu::PostInitWindow()
{
    ASSERT(m_pListBox == nullptr);
    if (m_pOwner != nullptr) {
        m_pListBox = dynamic_cast<ui::ListBox*>(FindControl(m_submenuNodeName.c_str()));
        if (m_pListBox == nullptr) {
            //Pure-code mode: when the submenu XML template does not exist, the root node is the ListBox
            m_pListBox = dynamic_cast<ui::ListBox*>(GetRoot());
        }
        ASSERT(m_pListBox != nullptr);
        if (m_pListBox == nullptr) {
            return;
        }
        //Set not to automatically destroy Child objects (because they are copied from the owner, resources are shared, and the Owner manages the lifetime of the objects)
        m_pListBox->SetAutoDestroyChild(false);

        //Get the controls that need to be drawn for the submenu items and add them to the Layout
        std::vector<Control*> submenuControls;
        MenuItem::GetAllSubMenuControls(m_pOwner, submenuControls);
        for (auto pControl : submenuControls) {
            if (pControl != nullptr) {
                m_pListBox->AddItem(pControl);
                continue;
            }
        }
    }
    else {
        m_pListBox = dynamic_cast<ui::ListBox*>(GetRoot());
        if (m_pListBox == nullptr) {
            //Allow an outer layer of shadow
            if ((GetRoot() != nullptr) && (GetRoot()->GetItemCount() > 0)) {
                m_pListBox = dynamic_cast<ui::ListBox*>(GetRoot()->GetItemAt(0));
            }
        }
        ASSERT(m_pListBox != nullptr);
    }

    //After the menu is displayed, put the related control into the Push state (asynchronous)
    if (m_pRelatedControl != nullptr) {
        m_pRelatedControl->SetState(kControlStatePushed);
    }

    //The base class implementation function needs to be called last
    BaseClass::PostInitWindow();
}

Control* Menu::GetRelatedControl() const
{
    return m_pRelatedControl.get();
}

ListBox* Menu::GetLayoutListBox() const
{
    return m_pListBox.get();
}

void Menu::OnMenuItemActivated(const DString& menuName, int32_t nMenuLevel,
                               const DString& itemName, size_t nItemIndex)
{
    Menu* pParentMenu = nullptr;
    if (GetParentWindow() != nullptr) {
        pParentMenu = dynamic_cast<Menu*>(GetParentWindow());
    }
    if (pParentMenu != nullptr) {
        pParentMenu->OnMenuItemActivated(menuName, nMenuLevel + 1, itemName, nItemIndex);
    }
    else {
        //It is already the top-level menu
        m_pActiveMenuItem = std::make_unique<ActiveMenuItem>();
        m_pActiveMenuItem->m_itemIndex = nItemIndex;
        m_pActiveMenuItem->m_itemName = itemName;
        m_pActiveMenuItem->m_menuLevel = nMenuLevel;
        m_pActiveMenuItem->m_menuName = menuName;
    }
}

void Menu::AttachMenuItemActivated(MenuItemActivatedEvent callback)
{
    if (callback != nullptr) {
        m_callbackList.push_back(callback);
    }
}

void Menu::ClearMenuItemActivated()
{
    m_callbackList.clear();
}

void Menu::OnFinalMessage()
{
    //Send the callback to notify of the already selected event
    if ((m_pActiveMenuItem != nullptr) && !m_callbackList.empty()) {
        ActiveMenuItem activeData = *m_pActiveMenuItem;
        std::vector<MenuItemActivatedEvent> callbackList(m_callbackList);
        for (MenuItemActivatedEvent callback : callbackList) {
            if (callback) {
                callback(activeData.m_menuName, activeData.m_menuLevel,
                         activeData.m_itemName, activeData.m_itemIndex);
            }
        }
    }
    BaseClass::OnFinalMessage();
}

void Menu::OnCloseWindow()
{
    RemoveObserver();
    DetachOwner();

    if (m_pRelatedControl != nullptr) {
        //Restore the state of the related control
        UiPoint pt;
        GetCursorPos(pt);
        m_pRelatedControl->ScreenToClient(pt);
        pt.Offset(m_pRelatedControl->GetScrollOffsetInScrollBox());
        if (m_pRelatedControl->GetRect().ContainsPt(pt)) {
            if (m_pRelatedControl->GetState() != ui::kControlStateHot) {
                m_pRelatedControl->SetState(ui::kControlStateHot);
            }            
        }
        else {
            if (m_pRelatedControl->GetState() != ui::kControlStateNormal) {
                m_pRelatedControl->SetState(ui::kControlStateNormal);
            }
        }
    }
    BaseClass::OnCloseWindow();
}

bool Menu::AddMenuItem(MenuItem* pMenuItem)
{
    //Currently, only the first-level menu can access this interface
    ASSERT(m_pOwner == nullptr);
    ListBox* pLayoutListBox = Menu::GetLayoutListBox();
    ASSERT(pLayoutListBox != nullptr);
    if (pLayoutListBox != nullptr) {
        return pLayoutListBox->AddItem(pMenuItem);
    }
    return false;
}

bool Menu::AddMenuControl(Control* pControl)
{
    //Pure-code mode: add normal controls (such as separator lines, custom rows) to the menu
    ASSERT(pControl != nullptr);
    if (pControl == nullptr) {
        return false;
    }
    ListBox* pLayoutListBox = Menu::GetLayoutListBox();
    ASSERT(pLayoutListBox != nullptr);
    if (pLayoutListBox != nullptr) {
        return pLayoutListBox->AddItem(pControl);
    }
    return false;
}

bool Menu::AddMenuItemAt(MenuItem* pMenuItem, size_t iIndex)
{
    //Currently, only the first-level menu can access this interface
    ASSERT(m_pOwner == nullptr);
    ListBox* pLayoutListBox = Menu::GetLayoutListBox();
    ASSERT(pLayoutListBox != nullptr);
    if (pLayoutListBox == nullptr) {
        return false;
    }
    
    size_t itemIndex = 0;
    MenuItem* pElementUI = nullptr;
    const size_t count = pLayoutListBox->GetItemCount();
    for (size_t i = 0; i < count; ++i) {
        Control* pControl = pLayoutListBox->GetItemAt(i);
        pElementUI = dynamic_cast<MenuItem*>(pControl);
        if (pElementUI != nullptr) {
            if (itemIndex == iIndex) {
                return pLayoutListBox->AddItemAt(pMenuItem, i);
            }
            ++itemIndex;
        }
        pElementUI = nullptr;
    }
    return false;
}

bool Menu::RemoveMenuItem(MenuItem* pMenuItem)
{
    //Currently, only the first-level menu can access this interface
    ASSERT(m_pOwner == nullptr);
    ListBox* pLayoutListBox = Menu::GetLayoutListBox();
    ASSERT(pLayoutListBox != nullptr);
    MenuItem* pElementUI = nullptr;
    if (pLayoutListBox != nullptr) {
        const size_t count = pLayoutListBox->GetItemCount();
        for (size_t i = 0; i < count; ++i) {
            pElementUI = dynamic_cast<MenuItem*>(pLayoutListBox->GetItemAt(i));
            if (pMenuItem == pElementUI) {
                pLayoutListBox->RemoveItemAt(i);
            }
            pElementUI = nullptr;
        }
    }
    return false;
}

bool Menu::RemoveMenuItemAt(size_t iIndex)
{
    //Currently, only the first-level menu can access this interface
    ASSERT(m_pOwner == nullptr);
    MenuItem* pMenuElementUI = GetMenuItemAt(iIndex);
    if (pMenuElementUI != nullptr) {
        return RemoveMenuItem(pMenuElementUI);
    }
    return false;
}

size_t Menu::GetMenuItemCount() const
{
    //Currently, only the first-level menu can access this interface
    ASSERT(m_pOwner == nullptr);
    ListBox* pLayoutListBox = Menu::GetLayoutListBox();
    if (pLayoutListBox == nullptr) {
        return 0;
    }
    size_t itemCount = 0;
    const size_t count = pLayoutListBox->GetItemCount();
    for (size_t i = 0; i < count; ++i) {
        if (dynamic_cast<MenuItem*>(pLayoutListBox->GetItemAt(i)) != nullptr) {
            ++itemCount;
        }
    }
    return itemCount;
}

MenuItem* Menu::GetMenuItemAt(size_t iIndex) const
{
    //Currently, only the first-level menu can access this interface
    ASSERT(m_pOwner == nullptr);
    ListBox* pLayoutListBox = Menu::GetLayoutListBox();
    ASSERT(pLayoutListBox != nullptr);
    if (pLayoutListBox == nullptr) {
        return nullptr;
    }
    size_t itemIndex = 0;
    MenuItem* pElementUI = nullptr;
    const size_t count = pLayoutListBox->GetItemCount();
    for (size_t i = 0; i < count; ++i) {
        Control* pControl = pLayoutListBox->GetItemAt(i);
        pElementUI = dynamic_cast<MenuItem*>(pControl);
        if (pElementUI != nullptr) {
            if (itemIndex == iIndex) {
                break;
            }
            ++itemIndex;
        }
        pElementUI = nullptr;
    }
    return pElementUI;
}

MenuItem* Menu::GetMenuItemByName(const DString& name) const
{
    //Currently, only the first-level menu can access this interface
    ASSERT(m_pOwner == nullptr);
    ListBox* pLayoutListBox = Menu::GetLayoutListBox();
    ASSERT(pLayoutListBox != nullptr);
    MenuItem* pElementUI = nullptr;
    if (pLayoutListBox != nullptr) {
        const size_t count = pLayoutListBox->GetItemCount();
        for (size_t i = 0; i < count; ++i) {
            pElementUI = dynamic_cast<MenuItem*>(pLayoutListBox->GetItemAt(i));
            if ((pElementUI != nullptr) && (pElementUI->IsNameEquals(name))) {
                break;
            }
            pElementUI = nullptr;
        }
    }
    return pElementUI;
}

MenuItem::MenuItem(Window* pWindow):
    ListBoxItem(pWindow),
    m_pSubWindow(nullptr)
{
    //On menu elements, do not let child controls respond to mouse messages
    SetMouseChildEnabled(false);
}

void MenuItem::GetAllSubMenuItem(const MenuItem* pParentElementUI,
                                       std::vector<MenuItem*>& submenuItems)
{
    submenuItems.clear();
    ASSERT(pParentElementUI != nullptr);
    if (pParentElementUI == nullptr) {
        return;
    }
    const size_t itemCount = pParentElementUI->GetItemCount();
    for (size_t i = 0; i < itemCount; ++i) {
        Control* pControl = pParentElementUI->GetItemAt(i);
        MenuItem* menuElementUI = dynamic_cast<MenuItem*>(pControl);
        if (menuElementUI != nullptr) {
            submenuItems.push_back(menuElementUI);
            continue;
        }

        menuElementUI = nullptr;
        SubMenu* subMenu = dynamic_cast<SubMenu*>(pControl);
        if (subMenu != nullptr) {
            const size_t count = subMenu->GetItemCount();
            for (size_t j = 0; j < count; ++j) {
                menuElementUI = dynamic_cast<MenuItem*>(subMenu->GetItemAt(j));
                if (menuElementUI != nullptr) {
                    submenuItems.push_back(menuElementUI);
                    continue;
                }
            }
        }
    }
}

void MenuItem::GetAllSubMenuControls(const MenuItem* pParentElementUI,
                                           std::vector<Control*>& submenuControls)
{
    submenuControls.clear();
    ASSERT(pParentElementUI != nullptr);
    if (pParentElementUI == nullptr) {
        return;
    }
    const size_t itemCount = pParentElementUI->GetItemCount();
    for (size_t i = 0; i < itemCount; ++i) {
        Control* pControl = pParentElementUI->GetItemAt(i);
        MenuItem* menuElementUI = dynamic_cast<MenuItem*>(pControl);
        if (menuElementUI != nullptr) {
            submenuControls.push_back(menuElementUI);
            continue;
        }

        SubMenu* subMenu = dynamic_cast<SubMenu*>(pControl);
        if (subMenu != nullptr) {
            const size_t count = subMenu->GetItemCount();
            for (size_t j = 0; j < count; ++j) {
                Control* pSubControl = subMenu->GetItemAt(j);
                if (pSubControl != nullptr) {
                    submenuControls.push_back(pSubControl);
                }
            }
        }
    }
}

bool MenuItem::AddSubMenuItem(MenuItem* pMenuItem)
{
    return AddItem(pMenuItem);
}

bool MenuItem::AddSubMenuItemAt(MenuItem* pMenuItem, size_t iIndex)
{
    const size_t subMenuCount = GetSubMenuItemCount();
    ASSERT(iIndex <= subMenuCount);
    if (iIndex > subMenuCount) {
        return false;
    }
    
    size_t itemIndex = 0;
    const size_t itemCount = GetItemCount();
    for (size_t i = 0; i < itemCount; ++i) {
        Control* pControl = GetItemAt(i);
        MenuItem* menuElementUI = dynamic_cast<MenuItem*>(pControl);
        if (menuElementUI != nullptr) {
            if (itemIndex == iIndex) {
                //Matched under the current node
                return AddItemAt(pMenuItem, i);
            }
            ++itemIndex;
            continue;
        }

        menuElementUI = nullptr;
        SubMenu* subMenu = dynamic_cast<SubMenu*>(pControl);
        if (subMenu != nullptr) {
            const size_t count = subMenu->GetItemCount();
            for (size_t j = 0; j < count; ++j) {
                menuElementUI = dynamic_cast<MenuItem*>(subMenu->GetItemAt(j));
                if (menuElementUI != nullptr) {
                    if (itemIndex == iIndex) {
                        //Matched in the SubMenu under the current node
                        return subMenu->AddItemAt(pMenuItem, j);
                    }
                    ++itemIndex;
                    continue;
                }
            }
        }
    }
    //If no match is found, append to the end
    return AddItem(pMenuItem);
}

bool MenuItem::RemoveSubMenuItem(MenuItem* pMenuItem)
{
    const size_t itemCount = GetItemCount();
    for (size_t i = 0; i < itemCount; ++i) {
        Control* pControl = GetItemAt(i);
        MenuItem* menuElementUI = dynamic_cast<MenuItem*>(pControl);
        if (menuElementUI != nullptr) {
            if (pMenuItem == menuElementUI) {
                //Matched under the current node
                return RemoveItemAt(i);
            }
            continue;
        }

        menuElementUI = nullptr;
        SubMenu* subMenu = dynamic_cast<SubMenu*>(pControl);
        if (subMenu != nullptr) {
            const size_t count = subMenu->GetItemCount();
            for (size_t j = 0; j < count; ++j) {
                menuElementUI = dynamic_cast<MenuItem*>(subMenu->GetItemAt(j));
                if (menuElementUI != nullptr) {
                    if (menuElementUI == pMenuItem) {
                        //Matched in the SubMenu under the current node
                        return subMenu->RemoveItemAt(j);
                    }
                    continue;
                }
            }
        }
    }
    return false;
}
bool MenuItem::RemoveSubMenuItemAt(size_t iIndex)
{
    size_t itemIndex = 0;
    const size_t itemCount = GetItemCount();
    for (size_t i = 0; i < itemCount; ++i) {
        Control* pControl = GetItemAt(i);
        MenuItem* menuElementUI = dynamic_cast<MenuItem*>(pControl);
        if (menuElementUI != nullptr) {
            if (itemIndex == iIndex) {
                //Matched under the current node
                return RemoveItemAt(i);
            }
            ++itemIndex;
            continue;
        }

        menuElementUI = nullptr;
        SubMenu* subMenu = dynamic_cast<SubMenu*>(pControl);
        if (subMenu != nullptr) {
            const size_t count = subMenu->GetItemCount();
            for (size_t j = 0; j < count; ++j) {
                menuElementUI = dynamic_cast<MenuItem*>(subMenu->GetItemAt(j));
                if (menuElementUI != nullptr) {
                    if (itemIndex == iIndex) {
                        //Matched in the SubMenu under the current node
                        return subMenu->RemoveItemAt(j);
                    }
                    ++itemIndex;
                    continue;
                }
            }
        }
    }
    return false;
}

void MenuItem::RemoveAllSubMenuItem()
{
    RemoveAllItems();
}

size_t MenuItem::GetSubMenuItemCount() const
{
    std::vector<MenuItem*> submenuItems;
    GetAllSubMenuItem(this, submenuItems);
    return submenuItems.size();
};

MenuItem* MenuItem::GetSubMenuItemAt(size_t iIndex) const
{
    MenuItem* foundItem = nullptr;
    std::vector<MenuItem*> submenuItems;
    GetAllSubMenuItem(this, submenuItems);
    if (iIndex < submenuItems.size()) {
        foundItem = submenuItems.at(iIndex);
    }
    return foundItem;
}

MenuItem* MenuItem::GetSubMenuItemByName(const DString& name) const
{
    std::vector<MenuItem*> submenuItems;
    GetAllSubMenuItem(this, submenuItems);
    MenuItem* subMenuItem = nullptr;
    for (auto item : submenuItems) {
        if ((item != nullptr) && (item->GetName() == name)) {
            subMenuItem = item;
            break;
        }
    }
    return subMenuItem;
}

bool MenuItem::ButtonUp(const ui::EventArgs& msg)
{
    Window* pWindow = GetWindow();
    ASSERT(pWindow != nullptr);
    if (pWindow == nullptr) {
        return false;
    }
    std::weak_ptr<WeakFlag> weakFlag = pWindow->GetWeakFlag();
    bool ret = BaseClass::ButtonUp(msg);
    if (ret && !weakFlag.expired() && !msg.IsSenderExpired()) {
        //Handle here: if there is a submenu, display the submenu
        if (!CheckSubMenuItem()){
            ContextMenuParam param;
            param.pWindow = pWindow;
            param.wParam = MenuCloseType::eMenuCloseAll;
            Menu::GetMenuObserver().RBroadcast(param);
        }
    }
    return ret;
}

bool MenuItem::MouseEnter(const ui::EventArgs& msg)
{
    Window* pWindow = GetWindow();
    ASSERT(pWindow != nullptr);
    if (pWindow == nullptr) {
        return BaseClass::MouseEnter(msg);
    }
    std::weak_ptr<WeakFlag> weakFlag = pWindow->GetWeakFlag();
    bool ret = BaseClass::MouseEnter(msg);
    if (!weakFlag.expired() && IsHotState() && !msg.IsSenderExpired()) {
        //Handle here: if there is a submenu, display the submenu
        if (!CheckSubMenuItem()) {
            ContextMenuParam param;
            param.pWindow = pWindow;
            param.wParam = MenuCloseType::eMenuCloseThis;
            Menu::GetMenuObserver().RBroadcast(param);
            //The previously selected item must be deselected here
            if (!weakFlag.expired() && (GetOwner() != nullptr)) {
                GetOwner()->SelectItem(Box::InvalidIndex, false, false);
            }
        }
    }
    return ret;
}

void MenuItem::PaintChild(ui::IRender* pRender, const ui::UiRect& rcPaint)
{
    UiRect rcTemp;
    if (!UiRect::Intersect(rcTemp, rcPaint, GetRect())) {
        return;
    }

    for (auto item : m_items) {
        Control* pControl = item;
        if (pControl == nullptr) {
            continue;
        }

        //For the content of multi-level menu items, do not draw it
        MenuItem* menuElementUI = dynamic_cast<MenuItem*>(pControl);
        if (menuElementUI != nullptr){
            continue;
        }
        SubMenu* subMenu = dynamic_cast<SubMenu*>(pControl);
        if (subMenu != nullptr) {
            continue;
        }
        
        if (!pControl->IsVisible()) {
            continue;
        }
        pControl->AlphaPaint(pRender, rcPaint);
    }
}

bool MenuItem::CheckSubMenuItem()
{
    bool hasSubMenu = false;
    for (auto item : m_items) {
        MenuItem* subMenuItem = dynamic_cast<MenuItem*>(item);
        if (subMenuItem != nullptr) {
            hasSubMenu = true;
            break;
        }
    }
    if (hasSubMenu) {
        if (GetOwner() != nullptr) {
            GetOwner()->SelectItem(GetListBoxIndex(), true, true);
        }
        if (m_pSubWindow == nullptr) {
            CreateMenuWnd();
        }
        else {
            //The submenu window shown last time has not disappeared yet, so do not show it again
            hasSubMenu = false;
        }
    }
    return hasSubMenu;
}

void MenuItem::CreateMenuWnd()
{
    ASSERT(m_pSubWindow == nullptr);
    if (m_pSubWindow != nullptr) {
        return;
    }

    Window* pWindow = GetWindow();
    m_pSubWindow = new Menu(pWindow, nullptr);
    ContextMenuParam param;
    param.pWindow = pWindow;
    param.wParam = MenuCloseType::eMenuCloseThis;
    Menu::GetMenuObserver().RBroadcast(param);

    //Interface of the parent menu window, used to synchronize configuration information
    Menu* pParentWindow = dynamic_cast<Menu*>(pWindow);
    ASSERT(pParentWindow != nullptr);
    if (pParentWindow != nullptr) {
        const DString skinFolder = pParentWindow->GetSkinFolder();
        m_pSubWindow->SetSkinFolder(skinFolder);
        DString subMenuXml;
        if (pParentWindow->m_xml.empty()) {
            //Pure-code mode: the submenu has no XML template, menu items are added by code
            subMenuXml = _T("");
        }
        else {
            FilePath xmlPath = pParentWindow->GetXmlPath();
            FilePath subXmlFile = FilePath(pParentWindow->m_submenuXml.c_str());
            //Convention: the submenu XML and the parent menu XML file are in the same directory
            if (!xmlPath.IsEmpty()) {
                subXmlFile = FilePathUtil::JoinFilePath(xmlPath, subXmlFile);
            }
            subMenuXml = subXmlFile.ToString();
        }
        m_pSubWindow->SetSubMenuXml(pParentWindow->m_submenuXml.c_str(), pParentWindow->m_submenuNodeName.c_str());

        //Set the top-left coordinate of the submenu window (to avoid a black screen flicker when the submenu pops up)
        UiPoint subMenuPt;
        if (pWindow != nullptr) {
            UiRect rcOwner = GetPos();
            UiRect rc = rcOwner;
            UiPadding rcCorner = pWindow->GetCurrentShadowCorner();
            UiRect rcWindow;
            GetWindow()->GetWindowRect(rcWindow);
            //Remove the shadow
            rcWindow.Deflate(rcCorner);
            GetWindow()->ClientToScreen(rc);
            rc.left = rcWindow.right;
            subMenuPt.x = rc.left - rcCorner.left;
            subMenuPt.y = rc.top - rcCorner.top;
        }
        m_pSubWindow->ShowMenu(subMenuXml, subMenuPt, MenuPopupPosType::RIGHT_BOTTOM, false, this);
    }
}

void MenuItem::Activate(const EventArgs* pMsg)
{
    std::weak_ptr<WeakFlag> weakFlag = GetWeakFlag();
    BaseClass::Activate(pMsg);
    if (weakFlag.expired()) {
        //During event handling, the control has already become invalid
        return;
    }
    DString itemName = GetName();
    size_t nItemIndex = GetListBoxIndex();
    Menu* pMenu = dynamic_cast<Menu*>(GetWindow());
    if (pMenu != nullptr) {
        DString menuName;
        if (pMenu->GetLayoutListBox() != nullptr) {
            menuName = pMenu->GetLayoutListBox()->GetName();
        }
        pMenu->OnMenuItemActivated(menuName, 0, itemName, nItemIndex);
    }
}

} // namespace ui
