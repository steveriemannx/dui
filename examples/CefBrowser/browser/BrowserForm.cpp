#include "BrowserForm.h"
#include "App.h"
#include "browser/BrowserBox.h"
#include "browser/BrowserManager.h"
#include "browser/DragDropManager.h"
#include <chrono>

#ifdef DUILIB_BUILD_FOR_SDL
    #include <iostream>
#endif

using namespace ui;

namespace
{
    // Reason the window received the WM_CLOSE message
    enum CloseReason
    {
        kDefaultClose    = 10,  // Closing the window by right-clicking the taskbar, pressing Alt+F4, and other normal reasons
        kBrowserBoxClose = 11   // The last browser box was closed, which caused the window to close
    };

    // Width and height of the drag image
    const int kDragImageWidth = 400;
    const int kDragImageHeight = 300;
}

BrowserForm::BrowserForm()
{
    m_pEditUrl = nullptr;
    m_pTabCtrl = nullptr;
    m_pBorwserBoxTab = nullptr;
    m_pActiveBrowserBox = nullptr;
    m_bButtonDown = false;
    m_bDragState = false;
}

BrowserForm::~BrowserForm()
{
    m_pEditUrl = nullptr;
    m_pTabCtrl = nullptr;
    m_pBorwserBoxTab = nullptr;
    m_pActiveBrowserBox = nullptr;
}

DString BrowserForm::GetSkinFolder()
{
    return _T("cef_browser");
}

DString BrowserForm::GetSkinFile()
{
    return _T("cef_browser.xml");
}

/** Layout management of the title bar area
*/
class TitleBarHLayout : public HLayout
{
public:
    TitleBarHLayout() = default;
    virtual ~TitleBarHLayout() override = default;

public:
    /** Adjust the position and size of all child controls according to the layout strategy
     * @param [in] items the list of child controls
     * @param [in] rc the current position and size information of the container, including padding but not margin
     * @param [in] bEstimateOnly true means only evaluate without adjusting control positions, false means adjust control positions
     * @return the final layout width and height information after arrangement, including the padding of the Box container but not the margin of the Box container itself (use this return value when the container supports scroll bars)
     */
    virtual UiSize64 ArrangeChildren(const std::vector<Control*>& items, UiRect rc, bool bEstimateOnly = false) override
    {
        UiSize64 szSize = HLayout::ArrangeChildren(items, rc, bEstimateOnly);
        ASSERT(items.empty() || items.size() == 5);
        if (items.size() != 5) {
            return szSize;
        }
        //Fixed structure, modify the width of the tab bar after validation
        int64_t nTotalWidth = 0;
        for (Control* pControl : items) {
            if (pControl == nullptr) {
                continue;
            }
            ui::UiMargin margin = pControl->GetMargin();
            nTotalWidth += (pControl->GetRect().Width() + margin.left + margin.right);
        }
        ASSERT(szSize.cx == nTotalWidth);
        if (szSize.cx != nTotalWidth) {
            return szSize;
        }
        ASSERT(rc.Width() == nTotalWidth);
        if (rc.Width() != nTotalWidth) {
            return szSize;
        }

        //Validate the TabCtrl control of the tab bar
        ui::TabCtrl* pTabCtrl = nullptr;
        ui::Control* pItem = items[2];
        if (pItem != nullptr) {
            pTabCtrl = dynamic_cast<ui::TabCtrl*>(pItem);            
        }
        ASSERT(pTabCtrl != nullptr);
        if (pTabCtrl == nullptr) {
            return szSize;
        }
        ASSERT(pTabCtrl->GetFixedWidth().IsStretch());
        if (!pTabCtrl->GetFixedWidth().IsStretch()) {
            return szSize;
        }

        //Equal to the total width: calculate the actual total width of the tab items
        int32_t nTabItemTotalWidth = 0;
        UiRect rcTabCtrl = pItem->GetPos();
        const size_t nTabItemCount = pTabCtrl->GetItemCount();
        for (size_t nTabItem = 0; nTabItem < nTabItemCount; ++nTabItem) {
            Control* pTabItem = pTabCtrl->GetItemAt(nTabItem);
            if ((pTabItem == nullptr) || !pTabItem->IsVisible()) {
                continue;
            }
            ui::UiMargin rcTabItemMargin = pTabItem->GetMargin();
            nTabItemTotalWidth += (pTabItem->GetPos().Width() + rcTabItemMargin.left + rcTabItemMargin.right);
        }
        if (nTabItemTotalWidth < rcTabCtrl.Width()) {
            int32_t nItemDiff = rcTabCtrl.Width() - nTabItemTotalWidth;
            //Tab item control: shorten the length
            rcTabCtrl.right -= nItemDiff;
            if (!bEstimateOnly) {
                pTabCtrl->SetPos(rcTabCtrl);
            }

            //New button control: move to the left
            Control* pItem = items[items.size() - 2];
            if (pItem != nullptr) {
                UiRect rcItem = pItem->GetPos();
                rcItem.Offset(-nItemDiff, 0);
                if (!bEstimateOnly) {
                    pItem->SetPos(rcItem);
                }
            }
            //Last control: increase the width
            pItem = items[items.size() - 1];
            if (pItem != nullptr) {
                UiRect rcItem = pItem->GetPos();
                rcItem.left -= nItemDiff;
                if (!bEstimateOnly) {
                    pItem->SetPos(rcItem);
                }
            }
        }

        //Validate again at the end
        if (!bEstimateOnly) {
            nTotalWidth = 0;
            for (Control* pControl : items) {
                if (pControl == nullptr) {
                    continue;
                }
                ui::UiMargin margin = pControl->GetMargin();
                nTotalWidth += (pControl->GetRect().Width() + margin.left + margin.right);
            }
            ASSERT(rc.Width() == nTotalWidth);
        }
        return szSize;
    }
};

void BrowserForm::OnInitWindow()
{
    App::Instance().AddMainWindow(this);
    AttachWindowSetFocusMsg([this](const ui::EventArgs&) {
        App::Instance().SetActiveMainWindow(this);
        return true;
        });

    GetRoot()->AttachBubbledEvent(ui::kEventClick, UiBind(&BrowserForm::OnClicked, this, std::placeholders::_1), 0);

    m_pEditUrl = static_cast<RichEdit*>(FindControl(_T("edit_url")));
    m_pEditUrl->AttachReturn(UiBind(&BrowserForm::OnReturn, this, std::placeholders::_1));
    if (m_pEditUrl != nullptr) {
        //When the mouse clicks into the address bar, select all the text
        m_pEditUrl->SetSelAllOnFocus(true);
    }

    //Replace the layout of the title bar
    HBox* pTitleBar = static_cast<HBox*>(FindControl(_T("title_bar")));
    if (pTitleBar != nullptr) {
        TitleBarHLayout* pNewLayout = new TitleBarHLayout;
        Layout* pOldLayout = pTitleBar->ResetLayout(pNewLayout);
        if (pOldLayout != nullptr) {
            //Copy the layout attributes
            pNewLayout->SetChildMarginX(pOldLayout->GetChildMarginX());
            pNewLayout->SetChildMarginY(pOldLayout->GetChildMarginY());
            pTitleBar->FreeLayout(pOldLayout);
        }
    }

    m_pTabCtrl = static_cast<TabCtrl*>(FindControl(_T("tab_ctrl")));
    m_pBorwserBoxTab = static_cast<TabBox*>(FindControl(_T("browser_box_tab")));

    if (m_pTabCtrl != nullptr) {
        m_pTabCtrl->AttachSelect(UiBind(&BrowserForm::OnTabItemSelected, this, std::placeholders::_1));
    }

    //Set the state of the buttons
    Control* pButton = FindControl(_T("btn_back"));
    if (pButton != nullptr) {
        pButton->SetEnabled(false);
    }
    pButton = FindControl(_T("btn_forward"));
    if (pButton != nullptr) {
        pButton->SetEnabled(false);
    }
    pButton = FindControl(_T("btn_refresh"));
    if (pButton != nullptr) {
        pButton->SetVisible(true);
    }
    pButton = FindControl(_T("btn_stop"));
    if (pButton != nullptr) {
        pButton->SetVisible(false);
    }

#ifdef DUILIB_BUILD_FOR_SDL
    //Show basic SDL information
    DString driverName = GetVideoDriverName();
    DString renderName = GetWindowRenderName();
    DString logMsg = ui::StringUtil::Printf(_T("[SDL: VideoDriver:\"%s\", RenderName:\"%s\"]"), driverName.c_str(), renderName.c_str());
    std::cout << logMsg << std::endl;
#endif
}

void BrowserForm::OnPreCloseWindow()
{
    //When closing the window, first close all Browser objects associated with this window
    ui::CefManager::GetInstance()->ProcessWindowCloseEvent(this);
}

void BrowserForm::OnCloseWindow()
{
    App::Instance().RemoveMainWindow(this);
    // Use m_pTabCtrl to determine the total number of browser boxes; the total obtained by browser_box_tab_ is inaccurate
    int browser_box_count = GetBoxCount();
    for (int i = 0; i < browser_box_count; i++) {
        Control* pBoxItem = m_pBorwserBoxTab->GetItemAt(i);
        ASSERT(nullptr != pBoxItem);
        if (nullptr == pBoxItem) {
            continue;
        }

        BrowserBox* pBrowserBox = dynamic_cast<BrowserBox*>(pBoxItem);
        if (nullptr != pBrowserBox) {
            pBrowserBox->UninitBrowserBox();
        }
    }
}

void BrowserForm::OnLoadingStateChange(BrowserBox* pBrowserBox)
{
    if (m_pActiveBrowserBox != pBrowserBox) {
        return;
    }
    ui::CefControl* pCefCcontrol = m_pActiveBrowserBox->GetCefControl();
    if ((pCefCcontrol == nullptr) || (pCefCcontrol->GetCefBrowser() == nullptr)) {
        return;
    }
    bool isLoading = pCefCcontrol->GetCefBrowser()->IsLoading();
    bool canGoBack = pCefCcontrol->GetCefBrowser()->CanGoBack();
    bool canGoForward = pCefCcontrol->GetCefBrowser()->CanGoForward();
    Control* pButton = FindControl(_T("btn_back"));
    if (pButton != nullptr) {
        pButton->SetEnabled(canGoBack);
    }
    pButton = FindControl(_T("btn_forward"));
    if (pButton != nullptr) {
        pButton->SetEnabled(canGoForward);
    }
    pButton = FindControl(_T("btn_refresh"));
    if (pButton != nullptr) {
        pButton->SetVisible(!isLoading);
    }
    pButton = FindControl(_T("btn_stop"));
    if (pButton != nullptr) {
        pButton->SetVisible(isLoading);
    }
}

LRESULT BrowserForm::OnKeyDownMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    if ((vkCode == VirtualKeyCode::kVK_TAB) && ui::Keyboard::IsKeyDown(VirtualKeyCode::kVK_CONTROL)) {
        // Handle the Ctrl+Tab shortcut: switch tabs
        bHandled = true;
        size_t nNextItem = m_pTabCtrl->GetCurSel();
        nNextItem = (nNextItem + 1) % m_pTabCtrl->GetItemCount();
        m_pTabCtrl->SelectItem(nNextItem, true, true);
    }
    else if ((vkCode == VirtualKeyCode::kVK_ESCAPE) && ui::Keyboard::IsKeyDown(VirtualKeyCode::kVK_LBUTTON)) {
        //When the ESC key is pressed, cancel the tab drag-out
        if (DragDropManager::GetInstance()->IsDragingBorwserBox()) {
            DragDropManager::GetInstance()->EndDragBorwserBox(false);
        }
    }
    else if (vkCode == ui::kVK_F11) {
        if (ui::CefManager::GetInstance()->IsEnableF11()) {
            //Enter fullscreen or exit fullscreen for the page
            if (IsWindowFullscreen() && (GetFullscreenControl() != nullptr)) {
                bHandled = true;
                ExitControlFullscreen();
            }
            else {
                //Display the current page in fullscreen
                bHandled = true;
                ShowCurrentPageFullscreen();
            }
        }
    }
    else if (vkCode == ui::kVK_F12) {
        if (ui::CefManager::GetInstance()->IsEnableF12()) {
            //Show or hide the developer tools
            bHandled = true;
            SwitchShowDevTools();
        }
    }
    if (bHandled) {
        return 0;
    }
    return BaseClass::OnKeyDownMsg(vkCode, modifierKey, nativeMsg, bHandled);
}

void BrowserForm::SwitchShowDevTools()
{
    ui::CefControl* pCefControl = nullptr;
    if (m_pActiveBrowserBox != nullptr) {
        pCefControl = m_pActiveBrowserBox->GetCefControl();
    }
    if (pCefControl != nullptr) {
        if (pCefControl->IsAttachedDevTools()) {
            pCefControl->DettachDevTools();
        }
        else {
            pCefControl->AttachDevTools();
        }
    }
}

void BrowserForm::ShowCurrentPageFullscreen()
{
    ui::CefControl* pCefControl = nullptr;
    if (m_pActiveBrowserBox != nullptr) {
        pCefControl = m_pActiveBrowserBox->GetCefControl();
    }
    if (pCefControl != nullptr) {
        SetFullscreenControl(pCefControl);
    }
}

LRESULT BrowserForm::OnWindowCloseMsg(uint32_t wParam, const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    int browser_count = GetBoxCount();
    if (browser_count > 0 && nullptr != m_pActiveBrowserBox) {
        // If there is only one browser box, close it directly
        if (1 == browser_count) {
            CloseBox(m_pActiveBrowserBox->GetBrowserId());
        }        
        else {
            // If there are multiple browser boxes
            while (GetBoxCount() > 0) {
                Control* pTabItem = m_pTabCtrl->GetItemAt(0);
                ASSERT(nullptr != pTabItem);
                if (nullptr == pTabItem) {
                    break;
                }
                CloseBox(pTabItem->GetUTF8Name());
            }
        }
    }
    return BaseClass::OnWindowCloseMsg(wParam, nativeMsg, bHandled);
}

bool BrowserForm::OnClicked(const ui::EventArgs& arg )
{
    DString name = arg.GetSender()->GetName();
    if (name == _T("btn_close")) {
        if (m_pActiveBrowserBox != nullptr) {
            CloseBox(m_pActiveBrowserBox->GetBrowserId());
        }
    }
    else if (name == _T("btn_add")) {
        BrowserManager::GetInstance()->CreateBorwserBox(this, "", _T(""));
    }
    else if (m_pActiveBrowserBox) {
        auto cef_control = m_pActiveBrowserBox->GetCefControl();
        if (!cef_control) {
            return true;
        }

        if (name == _T("btn_back")) {
            cef_control->GoBack();
        }
        else if (name == _T("btn_forward")) {
            cef_control->GoForward();
        }
        else if (name == _T("btn_refresh")) {
            cef_control->Refresh();
        }
        else if (name == _T("btn_stop")) {
            cef_control->StopLoad();
        }
    }

    return true;
}

bool BrowserForm::OnReturn(const ui::EventArgs& arg)
{
    if (m_pEditUrl != nullptr) {
        if (m_pEditUrl != nullptr) {
            DString url = m_pEditUrl->GetText();
            if (!url.empty()) {
                if ((m_pActiveBrowserBox != nullptr) && (m_pActiveBrowserBox->GetCefControl())) {
                    m_pActiveBrowserBox->GetCefControl()->LoadURL(url);
                }
            }
        }
    }
    return true;
}

void BrowserForm::OpenLinkUrl(const DString& url, bool bInNewWindow)
{
    if (ui::GlobalManager::Instance().IsInUIThread()) {
        std::string id = BrowserManager::GetInstance()->CreateBrowserID();
        if (bInNewWindow) {
            BrowserManager::GetInstance()->CreateBorwserBox(nullptr, id, url);
        }
        else {
            BrowserManager::GetInstance()->CreateBorwserBox(this, id, url);
        }
    }
    else {
        //Forward to the UI thread for processing
        ui::GlobalManager::Instance().Thread().PostTask(ui::kThreadUI, UiBind(&BrowserForm::OpenLinkUrl, this, url, bInNewWindow));
    }
}

BrowserBox* BrowserForm::CreateBrowserBox(ui::Window* pWindow, std::string browserId)
{
    return new BrowserBox(pWindow, browserId);
}

BrowserBox* BrowserForm::CreateBox(const std::string& browserId, DString url)
{
    ASSERT(ui::GlobalManager::Instance().IsInUIThread());
    DString id = ui::StringConvert::UTF8ToT(browserId);
    if (nullptr != FindTabItem(id)) {
        ASSERT(0);
        return nullptr;
    }
    if (nullptr != FindBox(id)) {
        ASSERT(0);
        return nullptr;
    }

    TabCtrlItem* pTabItem = new TabCtrlItem(m_pTabCtrl->GetWindow());
    GlobalManager::Instance().FillBoxWithCache(pTabItem, ui::FilePath(_T("cef_browser/tab_item.xml")));
    m_pTabCtrl->AddItemAt(pTabItem, GetBoxCount());
    pTabItem->SetUTF8Name(browserId);
    ui::Button* btn_item_close = pTabItem->GetCloseButton();
    ASSERT(btn_item_close != nullptr);
    if (btn_item_close != nullptr) {
        btn_item_close->AttachClick(UiBind(&BrowserForm::OnTabItemClose, this, std::placeholders::_1, browserId));
    }

    BrowserBox* pBrowserBox = CreateBrowserBox(m_pBorwserBoxTab->GetWindow(), browserId);
    m_pBorwserBoxTab->AddItem(pBrowserBox);
    GlobalManager::Instance().FillBoxWithCache(pBrowserBox, ui::FilePath(_T("cef_browser/browser_box.xml")), nullptr);
    pBrowserBox->SetName(id);
    pBrowserBox->InitBrowserBox(url);

    if (GetBoxCount() <= 1) {
        m_pActiveBrowserBox = pBrowserBox;
    }

    // Switch to the new browser box
    // If pTabItem is in the hidden state, the selection event cannot be triggered properly, so switch directly to the target browser box here
    pTabItem->Selected(true, false, 0);
    ChangeToBox(id);

    OnCreateNewTabPage(pTabItem, pBrowserBox);
    return pBrowserBox;
}

bool BrowserForm::CloseBox(const std::string& browserId)
{
    if (browserId.empty()) {
        return false;
    }

    DString id = ui::StringConvert::UTF8ToT(browserId);

    bool bRet = false;
    // Remove the corresponding item from the left session list
    TabCtrlItem* pTabItem = FindTabItem(id);
    if (nullptr != pTabItem) {
        m_pTabCtrl->RemoveItem(pTabItem);
        m_pTabCtrl->ArrangeAncestor();
        bRet = true;
    }

    // Find the browser box in the browser list and remove it
    BrowserBox* pBrowserBox = FindBox(id);
    ASSERT(pBrowserBox != nullptr);
    if (pBrowserBox != nullptr) {
        OnCloseTabPage(pBrowserBox);
        pBrowserBox->UninitBrowserBox();
        // Remove the box immediately if the number of browser boxes is greater than 1, otherwise do not remove it
        // If the last browser box is removed here immediately, the interface will turn black when the window closes because there is no control left
        // This browser box will be removed automatically when the window closes
        if (m_pBorwserBoxTab->GetItemCount() > 1) {
            m_pBorwserBoxTab->RemoveItem(pBrowserBox);
            if (m_pActiveBrowserBox == pBrowserBox) {
                m_pActiveBrowserBox = nullptr;
                size_t nSelItem = m_pBorwserBoxTab->GetCurSel();
                if (nSelItem != Box::InvalidIndex) {
                    m_pActiveBrowserBox = dynamic_cast<BrowserBox*>(m_pBorwserBoxTab->GetItemAt(nSelItem));
                }                
            }
        }
        else {
            m_pActiveBrowserBox = nullptr;
        }
    }
    
    if (GetBoxCount() == 0) {
        // When all browser boxes are closed, close the browser window
        this->CloseWnd(kBrowserBoxClose);
    }
    else {
        if (m_pActiveBrowserBox != nullptr) {
            //Select the new tab
            std::string newId = m_pActiveBrowserBox->GetBrowserId();
            SetActiveBox(newId);
        }
    }
    return bRet;
}

bool BrowserForm::AttachBox(BrowserBox* pBrowserBox)
{
    if (nullptr == pBrowserBox) {
        return false;
    }

    DString id = ui::StringConvert::UTF8ToT(pBrowserBox->GetBrowserId());
    if (nullptr != FindTabItem(id)) {
        ASSERT(0);
        return false;
    }
    if (nullptr != FindBox(id)) {
        ASSERT(0);
        return false;
    }

    TabCtrlItem* pTabItem = new TabCtrlItem(m_pTabCtrl->GetWindow());
    GlobalManager::Instance().FillBoxWithCache(pTabItem, ui::FilePath(_T("cef_browser/tab_item.xml")));
    m_pTabCtrl->AddItemAt(pTabItem, GetBoxCount());
    pTabItem->SetUTF8Name(pBrowserBox->GetBrowserId());
    pTabItem->SetTitle(pBrowserBox->GetTitle());
    ui::Button* btn_item_close = pTabItem->GetCloseButton();
    ASSERT(btn_item_close != nullptr);
    if (btn_item_close != nullptr) {
        btn_item_close->AttachClick(UiBind(&BrowserForm::OnTabItemClose, this, std::placeholders::_1, pBrowserBox->GetBrowserId()));
    }

    // When the pBrowserBox browser box control created by another form is added to the container control in another form
    // The AddItem function will update the m_pWindow of all child controls in pBrowserBox to the new form pointer
    m_pBorwserBoxTab->AddItem(pBrowserBox);

    if (GetBoxCount() <= 1) {
        m_pActiveBrowserBox = pBrowserBox;
    }

    // Switch to the new browser box
    // If pTabItem is in the hidden state, the selection event cannot be triggered properly, so switch directly to the target browser box here
    pTabItem->Selected(true, false, 0);
    ChangeToBox(id);

    //Re-download the website icon
    ui::CefControl* pCefControl = pBrowserBox->GetCefControl();
    if (pCefControl != nullptr) {
        pCefControl->ReDownloadFavIcon();
    }

    OnCreateNewTabPage(pTabItem, pBrowserBox);
    return true;
}

bool BrowserForm::DetachBox(BrowserBox* pBrowserBox)
{
    if (pBrowserBox == nullptr) {
        return false;
    }

    DString id = ui::StringConvert::UTF8ToT(pBrowserBox->GetBrowserId());

    // Remove the corresponding item from the top tab bar
    TabCtrlItem* pTabItem = FindTabItem(id);
    if (pTabItem == nullptr) {
        return false;
    }

    m_pTabCtrl->RemoveItem(pTabItem);
    m_pTabCtrl->ArrangeAncestor();
    OnCloseTabPage(pBrowserBox);

    // Find the browser box in the Tab browser box list on the right and remove it
    // pBrowserBox cannot be deleted here
    bool auto_destroy = m_pBorwserBoxTab->IsAutoDestroyChild();
    m_pBorwserBoxTab->SetAutoDestroyChild(false);
    if (!m_pBorwserBoxTab->RemoveItem(pBrowserBox)) {
        m_pBorwserBoxTab->SetAutoDestroyChild(auto_destroy);
        return false;
    }
    m_pBorwserBoxTab->SetAutoDestroyChild(auto_destroy);
    if (m_pActiveBrowserBox == pBrowserBox) {
        m_pActiveBrowserBox = nullptr;
    }

    // When all browser boxes are closed, close the browser window
    if (GetBoxCount() == 0) {
        this->CloseWnd(kBrowserBoxClose);
    }
    return true;
}

BrowserBox* BrowserForm::GetSelectedBox()
{
    return m_pActiveBrowserBox;
}

void BrowserForm::SetActiveBox(const std::string& browserId)
{
    if (browserId.empty()) {
        return;
    }

    if (IsWindowMinimized()) {
        ShowWindow(kSW_RESTORE);
    }
    else {
        ShowWindow(kSW_SHOW);
    }

    // Find the browser box item to activate in the session list on the left of the window
    DString id = ui::StringConvert::UTF8ToT(browserId);
    TabCtrlItem* pTabItem = FindTabItem(id);
    if (nullptr == pTabItem) {
        return;
    }

    // If pTabItem is in the hidden state, the selection event cannot be triggered properly, so switch directly to the target browser box here
    pTabItem->Selected(true, false, 0);
    ChangeToBox(id);
}

bool BrowserForm::IsActiveBox(const BrowserBox* pBrowserBox)
{
    ASSERT(nullptr != pBrowserBox);
    return (pBrowserBox == m_pActiveBrowserBox && IsWindowForeground() && !IsWindowMinimized() && IsWindowVisible());
}

bool BrowserForm::IsActiveBox(const DString& browserId)
{
    ASSERT(!browserId.empty());
    return (IsWindowForeground() && !IsWindowMinimized() && IsWindowVisible() && FindBox(browserId) == m_pActiveBrowserBox);
}

int32_t BrowserForm::GetBoxCount() const
{
    int32_t nBoxCount = 0;
    if (m_pTabCtrl != nullptr) {
        nBoxCount = (int32_t)m_pTabCtrl->GetItemCount();
    }
    return nBoxCount;
}

bool BrowserForm::OnTabItemSelected(const ui::EventArgs& param)
{
    if (kEventSelect == param.eventType) {
        ASSERT(param.GetSender() == m_pTabCtrl);
        if (m_pTabCtrl != nullptr) {
            // If a top tab is clicked, find the corresponding browser box in the Tab below and select it
            Control* pSelectedItem = m_pTabCtrl->GetItemAt(m_pTabCtrl->GetCurSel());
            ASSERT(pSelectedItem != nullptr);
            if (pSelectedItem != nullptr) {
                DString session_id = pSelectedItem->GetName();
                ChangeToBox(session_id);
            }
        }
    }
    return true;
}

bool BrowserForm::OnTabItemClose(const ui::EventArgs& param, const std::string& browserId)
{
    CloseBox(browserId);
    return true;
}

BrowserBox* BrowserForm::FindBox(const DString& browserId)
{
    for (int i = 0; i < (int)m_pBorwserBoxTab->GetItemCount(); i++) {
        Control *pBoxItem = m_pBorwserBoxTab->GetItemAt(i);
        ASSERT(nullptr != pBoxItem);
        if (nullptr == pBoxItem) {
            return nullptr;
        }

        if (pBoxItem->GetName() == browserId) {
            return dynamic_cast<BrowserBox*>(pBoxItem);
        }
    }
    return nullptr;
}

TabCtrlItem* BrowserForm::FindTabItem(const DString& browserId)
{
    for (int i = 0; i < GetBoxCount(); i++) {
        Control *pTabItem = m_pTabCtrl->GetItemAt(i);
        ASSERT(nullptr != pTabItem);
        if (nullptr == pTabItem) {
            return nullptr;
        }

        if (pTabItem->GetName() == browserId) {
            return dynamic_cast<TabCtrlItem*>(pTabItem);
        }
    }
    return nullptr;
}

void BrowserForm::SetTabItemName(const DString& browserId, const DString& name)
{
    TabCtrlItem* pTabItem = FindTabItem(browserId);
    if (nullptr != pTabItem) {
        pTabItem->SetTitle(name);
    }
}

void BrowserForm::SetURL(const std::string& browserId, const DString& url)
{
    if ((m_pEditUrl != nullptr) && (m_pActiveBrowserBox != nullptr) && (m_pActiveBrowserBox->GetBrowserId() == browserId)) {
        m_pEditUrl->SetText(url);
    }
}

bool BrowserForm::ChangeToBox(const DString& browserId)
{
    if (browserId.empty()) {
        return false;
    }

    BrowserBox* pBoxItem = FindBox(browserId);
    if (nullptr == pBoxItem) {
        return false;
    }
    pBoxItem->SetPos(pBoxItem->GetPos());
    m_pBorwserBoxTab->SelectItem(pBoxItem);
    pBoxItem->SetFocus();
    m_pActiveBrowserBox = pBoxItem;

    // Update the taskbar icon and title according to the currently active browser box
    DStringW urlW = m_pActiveBrowserBox->GetCefControl()->GetURL();
    m_pEditUrl->SetText(urlW);
    OnLoadingStateChange(m_pActiveBrowserBox);
    return true;
}

void BrowserForm::NotifyFavicon(const BrowserBox* pBrowserBox, CefRefPtr<CefImage> image)
{
    if (pBrowserBox == nullptr) {
        return;
    }

    DString id = ui::StringConvert::UTF8ToT(pBrowserBox->GetBrowserId());
    TabCtrlItem* pTabItem = FindTabItem(id);
    if (pTabItem == nullptr) {
        return;
    }

    if (image == nullptr) {
        pTabItem->ClearIconData();
    }

    int32_t nWidth = 0;
    int32_t nHeight = 0;    
    CefRefPtr<CefBinaryValue> cefImageData = image->GetAsBitmap(Dpi().GetDisplayScale(), CEF_COLOR_TYPE_BGRA_8888, CEF_ALPHA_TYPE_PREMULTIPLIED, nWidth, nHeight);
    if (cefImageData == nullptr) {
        pTabItem->ClearIconData();
        return;
    }
    size_t nDataSize = cefImageData->GetSize();
    if (nDataSize == 0) {
        pTabItem->ClearIconData();
        return;
    }
    ASSERT((int32_t)nDataSize == nHeight * nWidth * sizeof(uint32_t));
    if ((int32_t)nDataSize != nHeight * nWidth * sizeof(uint32_t)) {
        pTabItem->ClearIconData();
        return;
    }
    std::vector<uint8_t> imageData;
    imageData.resize(nDataSize);
    nDataSize = cefImageData->GetData(imageData.data(), imageData.size(), 0);
    ASSERT(nDataSize == imageData.size());
    if (nDataSize != imageData.size()) {
        pTabItem->ClearIconData();
        return;
    }

    pTabItem->SetIconData(nWidth, nHeight, imageData.data(), (int32_t)imageData.size());
}

void BrowserForm::OnCreateNewTabPage(ui::TabCtrlItem* pTabItem, BrowserBox* pBrowserBox)
{
    if (pTabItem != nullptr) {
        pTabItem->AttachAllEvents(UiBind(&BrowserForm::OnProcessTabItemDrag, this, std::placeholders::_1));
    }
    if ((pBrowserBox != nullptr) && (pBrowserBox->GetCefControl() != nullptr)) {
        if (IsWindowFullscreen() && (GetFullscreenControl() != nullptr)) {
            //Currently in the control fullscreen state, set the control of the new tab as the fullscreen control
            SetFullscreenControl(pBrowserBox->GetCefControl());
        }
    }
}

void BrowserForm::OnCloseTabPage(BrowserBox* pBrowserBox)
{
}

bool BrowserForm::OnBeforeDragBoxCallback(const DString& browserId)
{
    BrowserBox* pBrowserBox = FindBox(browserId);
    if (pBrowserBox != nullptr) {
        pBrowserBox->SetVisible(false);
    }
    else {
        return false;
    }
    m_dragingBrowserId = browserId;

    TabCtrlItem* pTabItem = FindTabItem(browserId);
    if (pTabItem != nullptr) {
        pTabItem->CancelDragOperation();
        pTabItem->SetVisible(false);
    }

    // Find the new browser box to be displayed
    size_t index = pTabItem->GetListBoxIndex();
    if (index > 0) {
        index--;
    }
    else {
        index++;
    }
    TabCtrlItem* new_tab_item = static_cast<TabCtrlItem*>(m_pTabCtrl->GetItemAt(index));
    if (new_tab_item != nullptr) {
        new_tab_item->Selected(true, false, 0);
        ChangeToBox(new_tab_item->GetName());
    }

    //Since the tab is hidden, notify the parent control of the tab to recalculate its position
    if (m_pTabCtrl != nullptr) {
        m_pTabCtrl->ArrangeAncestor();
    }
    return true;
}

void BrowserForm::OnAfterDragBoxCallback(bool bDropSucceed)
{
    DString dragingBrowserId;
    dragingBrowserId.swap(m_dragingBrowserId);
    m_bDragState = false;
    m_bButtonDown = false;
    if (!bDropSucceed && !dragingBrowserId.empty()) {
        BrowserBox* pBrowserBox = FindBox(dragingBrowserId);
        if (pBrowserBox != nullptr) {
            pBrowserBox->SetFadeVisible(true);
        }

        TabCtrlItem* pTabItem = FindTabItem(dragingBrowserId);
        if (pTabItem != nullptr) {
            pTabItem->SetFadeVisible(true);
            pTabItem->Selected(true, false, 0);
            ChangeToBox(dragingBrowserId);
        }

        //Since the tab is hidden, notify the parent control of the tab to recalculate its position
        if (m_pTabCtrl != nullptr) {
            m_pTabCtrl->ArrangeAncestor();
        }
    }
}

LRESULT BrowserForm::OnMouseMoveMsg(const ui::UiPoint& pt, uint32_t modifierKey, bool bFromNC, const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    LRESULT lResult = BaseClass::OnMouseMoveMsg(pt, modifierKey, bFromNC, nativeMsg, bHandled);
    DragDropManager::GetInstance()->UpdateDragFormPos();
    return lResult;
}

LRESULT BrowserForm::OnMouseLButtonUpMsg(const ui::UiPoint& pt, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    LRESULT lResult = BaseClass::OnMouseLButtonUpMsg(pt, modifierKey, nativeMsg, bHandled);
    DragDropManager::GetInstance()->EndDragBorwserBox(true);
    return lResult;
}

LRESULT BrowserForm::OnCaptureChangedMsg(const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    LRESULT lResult = BaseClass::OnCaptureChangedMsg(nativeMsg, bHandled);
    DragDropManager::GetInstance()->EndDragBorwserBox(true);
    return lResult;
}


bool BrowserForm::OnProcessTabItemDrag(const ui::EventArgs& param)
{
    switch (param.eventType)
    {
    case kEventMouseMove:
    {
        if (!m_bButtonDown || m_bDragState || (m_pActiveBrowserBox == nullptr)) {
            break;
        }

        if (!ui::Keyboard::IsKeyDown(ui::VirtualKeyCode::kVK_LBUTTON)) {
            break;
        }

        DString id = ui::StringConvert::UTF8ToT(m_pActiveBrowserBox->GetBrowserId());
        TabCtrlItem* pTabItem = FindTabItem(id);
        if (pTabItem == nullptr) {
            break;
        }

        //When the vertical sliding distance of the mouse exceeds the tab width, start the drag-out operation
        int32_t cy = std::abs(param.ptMouse.y - m_oldDragPoint.y);
        if (cy > pTabItem->GetPos().Height()) {

            m_bDragState = true;

            // Generate a bitmap of the browser box being dragged
            std::shared_ptr<ui::IBitmap> spIBitmap = GenerateWebPageBitmap(m_pActiveBrowserBox->GetCefControl());

            // pt should specify the coordinate relative to the top-left corner (0,0) of the bitmap; here it is set to the top-center point of the bitmap
            ui::UiPoint pt = { kDragImageWidth / 2, 0 };

            if (!DragDropManager::GetInstance()->StartDragBorwserBox(m_pActiveBrowserBox, spIBitmap, pt)) {
                m_bDragState = false;
            }
        }
    }
    break;
    case kEventMouseButtonDown:
        m_oldDragPoint = { param.ptMouse.x, param.ptMouse.y };
        m_bDragState = false;
        m_bButtonDown = true;
        break;
    case kEventMouseButtonUp:
        m_bButtonDown = false;
        break;
    default:
        break;
    }
    return true;
}

std::shared_ptr<ui::IBitmap> BrowserForm::GenerateWebPageBitmap(ui::CefControl* pCefControl)
{
    std::shared_ptr<IBitmap> spBitmap;
    if (pCefControl != nullptr) {
        spBitmap = pCefControl->MakeImageSnapshot();
    }
    if (spBitmap == nullptr) {
        return nullptr;
    }
    if ((spBitmap->GetWidth() < 1) || (spBitmap->GetHeight() < 1)) {
        return nullptr;
    }

    std::unique_ptr<IRender> render;
    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    ASSERT(pRenderFactory != nullptr);
    if (pRenderFactory != nullptr) {
        render.reset(pRenderFactory->CreateRender(GetRenderDpi()));
    }
    ASSERT(render != nullptr);
    if (render->Resize(kDragImageWidth, kDragImageHeight)) {
        int32_t dest_width = 0;
        int32_t dest_height = 0;
        float scale = (float)spBitmap->GetWidth() / (float)spBitmap->GetHeight();
        if (scale >= 1.0) {
            dest_width = kDragImageWidth;
            dest_height = (int32_t)(kDragImageWidth * (float)spBitmap->GetHeight() / (float)spBitmap->GetWidth());
        }
        else {
            dest_height = kDragImageHeight;
            dest_width = (int32_t)(kDragImageHeight * (float)spBitmap->GetWidth() / (float)spBitmap->GetHeight());
        }

        UiRect rcPaint;
        rcPaint.left = 0;
        rcPaint.top = 0;
        rcPaint.right = rcPaint.left + kDragImageWidth;
        rcPaint.bottom = rcPaint.top + kDragImageHeight;

        UiRect rcDest;
        rcDest.left = (kDragImageWidth - dest_width) / 2;
        rcDest.top = 0;
        rcDest.right = rcDest.left + dest_width;
        rcDest.bottom = rcDest.top + dest_height;

        UiRect rcSource;
        rcSource.left = 0;
        rcSource.top = 0;
        rcSource.right = rcSource.left + spBitmap->GetWidth();
        rcSource.bottom = rcSource.top + spBitmap->GetHeight();

        render->DrawImageRect(rcPaint, spBitmap.get(), rcDest, rcSource);
        return std::shared_ptr<ui::IBitmap>(render->MakeImageSnapshot());
    }
    return nullptr;
}
