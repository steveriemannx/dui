#include "dui/Control/Flyout.h"
#include "dui/Core/Box.h"
#include "dui/Core/Control.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/WindowCreateParam.h"
#include "dui/Core/WindowMessage.h"
#include "dui/Core/Keyboard.h"
#include "dui/Core/Callback.h"

namespace ui
{

//Static member initialization
Flyout* Flyout::s_pActiveFlyout = nullptr;
const int32_t Flyout::kDetectIntervalMs = 50;

Flyout::Flyout(Window* pParentWindow) :
    m_pParentWindow(pParentWindow),
    m_placement(Placement::Bottom),
    m_actualPlacement(Placement::Bottom),
    m_bAutoDismiss(true),
    m_bNoFocus(true),
    m_nGap(6),
    m_bAllowFlip(true),
    m_bOpen(false),
    m_bClosing(false),
    m_nDetectTaskId(0),
    m_bPointerDownLast(false),
    m_bEscapeDownLast(false)
{
    if (m_pParentWindow != nullptr) {
        m_skinFolder = m_pParentWindow->GetResourcePath().ToString();
    }
}

Flyout::~Flyout()
{
    StopDetectTimer();
    //Fallback: make sure the active flyout pointer does not dangle after destruction
        //(the normal close path already clears it in DoClose/OnFinalMessage)
    if (s_pActiveFlyout == this) {
        s_pActiveFlyout = nullptr;
    }
}

void Flyout::SetSkinFolder(const std::string& skinFolder)
{
    m_skinFolder = skinFolder;
}

bool Flyout::ShowAt(Control* pAnchor, const std::string& xmlFile, Placement placement)
{
    ASSERT(pAnchor != nullptr);
    ASSERT(!xmlFile.empty());
    if ((pAnchor == nullptr) || xmlFile.empty()) {
        return false;
    }
    Window* pAnchorWindow = pAnchor->GetWindow();
    ASSERT(pAnchorWindow != nullptr);
    if (pAnchorWindow == nullptr) {
        return false;
    }

    //Only one active flyout is kept at a time: close the existing one first
    if ((s_pActiveFlyout != nullptr) && (s_pActiveFlyout != this)) {
        s_pActiveFlyout->DoClose(CloseReason::kManual);
    }

    m_pAnchor = pAnchor;
    m_xml = xmlFile;
    m_placement = placement;
    m_actualPlacement = placement;
    m_bClosing = false;
    m_bOpen = false;

    //In no-focus mode, remember whether the parent window was in the foreground before the flyout was
    //created: creating the flyout window may make it the foreground window unexpectedly, which would make
    //the parent window lose its active state and the keyboard focus
    const bool bParentWasForeground = (m_pParentWindow != nullptr) && m_pParentWindow->IsWindowForeground();

    //Set the initial window position to the anchor top-left corner (physical pixels), so the first show does not flicker at the default position
    UiRect rcAnchorInit;
    GetAnchorScreenRect(rcAnchorInit, true);
    WindowCreateParam createWndParam;
    createWndParam.m_dwStyle = kWS_POPUP;
    createWndParam.m_dwExStyle = kWS_EX_LAYERED;
    if (m_bNoFocus) {
        //No focus: the parent window stays active after the flyout is shown
        createWndParam.m_dwExStyle |= kWS_EX_NOACTIVATE;
    }
    if (m_pParentWindow == nullptr) {
        //Without a parent window, behave as a tool window so it does not show up in the taskbar or the Alt+Tab list
        createWndParam.m_dwExStyle |= kWS_EX_TOOLWINDOW;
    }
    createWndParam.m_nX = rcAnchorInit.left;
    createWndParam.m_nY = rcAnchorInit.top;

    if (!CreateWnd(m_pParentWindow, createWndParam)) {
        //Creation failed (usually a wrong XML resource path): destroy the object and return, so nothing leaks.
        //Note: if this object already was the active flyout (ShowAt called repeatedly and an earlier ShowAt
        //succeeded), deleting it directly would leave a dangling static pointer, so clear that first.
        if (s_pActiveFlyout == this) {
            s_pActiveFlyout = nullptr;
        }
        delete this;
        return false;
    }

    //Compute the anchor position
    PositionAroundAnchor();

    if (!m_bNoFocus) {
        //Show the window and give it the focus
        ShowWindow(ui::kSW_SHOW_NORMAL);

        //Make it a topmost window
        SetWindowAlwaysOnTop(true);

        SetWindowForeground();
        KeepParentActive();
    }
    else {
        //Show the window without giving it the focus
        ShowWindow(ui::kSW_SHOW_NA);

        //Make it a topmost window
        SetWindowAlwaysOnTop(true);

        if (bParentWasForeground && (m_pParentWindow != nullptr) && !m_pParentWindow->IsWindowForeground()) {
            //In no-focus mode the flyout window may become the foreground window right after creation,
            //which makes the parent window lose its active state and the keyboard focus (for example an
            //auto-complete flyout would no longer receive typed input); explicitly restore the foreground
            //state and the input focus of the parent window here
            m_pParentWindow->SetWindowForeground();
            m_pParentWindow->SetWindowFocus();
        }
    }

    m_bOpen = true;
    s_pActiveFlyout = this;

    if (m_bNoFocus) {
        StartDetectTimer();
    }

    //Fire the opened callbacks
    std::vector<FlyoutEvent> callbacks = m_openedCallbacks;
    for (const FlyoutEvent& callback : callbacks) {
        callback(CloseReason::kManual);
    }
    return true;
}

void Flyout::Dismiss()
{
    DoClose(CloseReason::kManual);
}

void Flyout::SetAutoDismiss(bool bAutoDismiss)
{
    m_bAutoDismiss = bAutoDismiss;
}

bool Flyout::IsAutoDismiss() const
{
    return m_bAutoDismiss;
}

void Flyout::SetNoFocus(bool bNoFocus)
{
    m_bNoFocus = bNoFocus;
}

bool Flyout::IsNoFocus() const
{
    return m_bNoFocus;
}

void Flyout::SetGap(int32_t nGap)
{
    m_nGap = nGap;
}

int32_t Flyout::GetGap() const
{
    return m_nGap;
}

void Flyout::SetAllowFlip(bool bAllowFlip)
{
    m_bAllowFlip = bAllowFlip;
}

bool Flyout::IsAllowFlip() const
{
    return m_bAllowFlip;
}

Control* Flyout::GetAnchor() const
{
    return m_pAnchor.get();
}

Flyout::Placement Flyout::GetPlacement() const
{
    return m_placement;
}

bool Flyout::IsOpen() const
{
    return m_bOpen;
}

void Flyout::AttachOpened(const FlyoutEvent& callback)
{
    if (callback) {
        m_openedCallbacks.push_back(callback);
    }
}

void Flyout::AttachClosed(const FlyoutEvent& callback)
{
    if (callback) {
        m_closedCallbacks.push_back(callback);
    }
}

Flyout* Flyout::GetActiveFlyout()
{
    return s_pActiveFlyout;
}

void Flyout::DismissActive()
{
    if (s_pActiveFlyout != nullptr) {
        s_pActiveFlyout->Dismiss();
    }
}

std::string Flyout::GetSkinFolder()
{
    return m_skinFolder.c_str();
}

std::string Flyout::GetSkinFile()
{
    return m_xml.c_str();
}

void Flyout::OnInitWindow()
{
    BaseClass::OnInitWindow();
}

void Flyout::OnFinalMessage()
{
    StopDetectTimer();
    m_bOpen = false;
    if (s_pActiveFlyout == this) {
        s_pActiveFlyout = nullptr;
    }

    //The modeless window base class deletes this object automatically after OnFinalMessage returns
    BaseClass::OnFinalMessage();
}

LRESULT Flyout::OnKillFocusMsg(WindowBase* pSetFocusWindow, const NativeMsg& nativeMsg, bool& bHandled)
{
    LRESULT lResult = BaseClass::OnKillFocusMsg(pSetFocusWindow, nativeMsg, bHandled);
    //In focus mode: close as soon as the focus goes to a window other than this one
    //In no-focus mode (m_bNoFocus) the flyout does not hold the keyboard focus, so receiving a kill-focus
    //message is a normal state change (for example when the parent window becomes the foreground window
    //again) and the flyout must not close; outside clicks are detected by the timer in OnDetectTick
    if (!m_bNoFocus && m_bAutoDismiss && (pSetFocusWindow != this)) {
        DoClose(CloseReason::kClickOutside);
    }
    return lResult;
}

LRESULT Flyout::OnKeyDownMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_bAutoDismiss && (vkCode == kVK_ESCAPE)) {
        bHandled = true;
        DoClose(CloseReason::kEscape);
        return 0;
    }
    return BaseClass::OnKeyDownMsg(vkCode, modifierKey, nativeMsg, bHandled);
}

void Flyout::OnWindowDisplayScaleChanged(uint32_t nOldScaleFactor, uint32_t nNewScaleFactor)
{
    BaseClass::OnWindowDisplayScaleChanged(nOldScaleFactor, nNewScaleFactor);
    //After a DPI change both the anchor position and the window size have to be recomputed; closing the flyout is the safer choice
    DoClose(CloseReason::kAnchorLost);
}

bool Flyout::GetAnchorScreenRect(UiRect& rcAnchor, bool bPhysical) const
{
    rcAnchor.Clear();
    Control* pAnchor = m_pAnchor.get();
    if (pAnchor == nullptr) {
        return false;
    }
    Window* pAnchorWindow = pAnchor->GetWindow();
    if (pAnchorWindow == nullptr) {
        return false;
    }

    //Layout rectangle of the anchor control in the window client content coordinate system (DIP)
    rcAnchor = pAnchor->GetPos();
    //Subtract the scroll offsets accumulated by the enclosing scroll containers to get the visual client
    //coordinates of the window (GetPos returns content coordinates; visual = content - scroll offset, which
    //matches the convention used by Menu/MenuBar)
    UiPoint ptScrollOffset = pAnchor->GetScrollOffsetInScrollBox();
    rcAnchor.Offset(-ptScrollOffset.x, -ptScrollOffset.y);

    if (bPhysical) {
        //DIP -> physical pixels -> screen coordinates, matching the physical coordinate system of GetCursorPos/GetWindowRect
        pAnchorWindow->Dpi().ClientSizeToWindowSize(rcAnchor);
        pAnchorWindow->ClientToScreen(rcAnchor);
    }
    else {
        //First add the window origin to the DIP value (physical pixels), then scale the whole rectangle back to DIP (same convention as Menu)
        pAnchorWindow->ClientToScreen(rcAnchor);
        pAnchorWindow->Dpi().WindowSizeToClientSize(rcAnchor);
    }
    return true;
}

bool Flyout::IsPointInAnchor(const UiPoint& ptScreen) const
{
    UiRect rcAnchor;
    if (!GetAnchorScreenRect(rcAnchor, true)) {
        return false;
    }
    return rcAnchor.ContainsPt(ptScreen);
}

void Flyout::PositionAroundAnchor()
{
    Box* pRoot = GetRoot();
    ASSERT(pRoot != nullptr);
    if (pRoot == nullptr) {
        return;
    }
    Control* pAnchor = m_pAnchor.get();
    if (pAnchor == nullptr) {
        return;
    }

    //Anchor rectangle in screen coordinates (DIP)
    UiRect rcAnchor;
    if (!GetAnchorScreenRect(rcAnchor, false)) {
        return;
    }

    //Use the work area of the monitor that contains the anchor center as the boundary (DIP)
    UiPoint ptAnchorCenter((rcAnchor.left + rcAnchor.right) / 2, (rcAnchor.top + rcAnchor.bottom) / 2);
    Dpi().ClientSizeToWindowSize(ptAnchorCenter);
    UiRect rcWork;
    if (!GetMonitorWorkRect(ptAnchorCenter, rcWork)) {
        return;
    }
    Dpi().WindowSizeToClientSize(rcWork);

    //Estimate the window size (the returned value includes the shadow area)
    UiSize szAvailable(rcWork.Width(), rcWork.Height());
    UiEstSize estSize = pRoot->EstimateSize(szAvailable);
    UiSize szWindow = szAvailable;
    if (estSize.cx.IsInt32()) {
        szWindow.cx = estSize.cx.GetInt32();
    }
    if (estSize.cy.IsInt32()) {
        szWindow.cy = estSize.cy.GetInt32();
    }
    if (szWindow.cx > rcWork.Width()) {
        szWindow.cx = rcWork.Width();
    }
    if (szWindow.cy > rcWork.Height()) {
        szWindow.cy = rcWork.Height();
    }

    //Edge area occupied by the window shadow; it has to be subtracted from the visual area
    UiPadding rcShadow = pRoot->GetPadding();
    int32_t nClientWidth = szWindow.cx - rcShadow.left - rcShadow.right;
    int32_t nClientHeight = szWindow.cy - rcShadow.top - rcShadow.bottom;

    const int32_t nGap = Dpi().GetScaleInt(m_nGap);

    //Compute the top-left corner of the visual area for the preferred direction (screen DIP coordinates)
    Placement actual = m_placement;
    int32_t nClientX = rcAnchor.left;
    int32_t nClientY = rcAnchor.top;
    switch (m_placement)
    {
    case Placement::Bottom:
        nClientX = rcAnchor.left;
        nClientY = rcAnchor.bottom + nGap;
        break;
    case Placement::BottomEnd:
        nClientX = rcAnchor.right - nClientWidth;
        nClientY = rcAnchor.bottom + nGap;
        break;
    case Placement::Top:
        nClientX = rcAnchor.left;
        nClientY = rcAnchor.top - nGap - nClientHeight;
        break;
    case Placement::TopEnd:
        nClientX = rcAnchor.right - nClientWidth;
        nClientY = rcAnchor.top - nGap - nClientHeight;
        break;
    case Placement::Right:
        nClientX = rcAnchor.right + nGap;
        nClientY = rcAnchor.top;
        break;
    case Placement::RightEnd:
        nClientX = rcAnchor.right + nGap;
        nClientY = rcAnchor.bottom - nClientHeight;
        break;
    case Placement::Left:
        nClientX = rcAnchor.left - nGap - nClientWidth;
        nClientY = rcAnchor.top;
        break;
    case Placement::LeftEnd:
        nClientX = rcAnchor.left - nGap - nClientWidth;
        nClientY = rcAnchor.bottom - nClientHeight;
        break;
    default:
        break;
    }

    //Flip to the opposite side when there is not enough space
    if (m_bAllowFlip) {
        const bool bBottomSide = (m_placement == Placement::Bottom) || (m_placement == Placement::BottomEnd);
        const bool bTopSide = (m_placement == Placement::Top) || (m_placement == Placement::TopEnd);
        const bool bRightSide = (m_placement == Placement::Right) || (m_placement == Placement::RightEnd);
        const bool bLeftSide = (m_placement == Placement::Left) || (m_placement == Placement::LeftEnd);

        if (bBottomSide && (nClientY + nClientHeight > rcWork.bottom)) {
            //Not enough room below: try flipping above
            int32_t nFlippedY = rcAnchor.top - nGap - nClientHeight;
            if (nFlippedY >= rcWork.top) {
                nClientY = nFlippedY;
                actual = (m_placement == Placement::BottomEnd) ? Placement::TopEnd : Placement::Top;
            }
        }
        else if (bTopSide && (nClientY < rcWork.top)) {
            //Not enough room above: try flipping below
            int32_t nFlippedY = rcAnchor.bottom + nGap;
            if (nFlippedY + nClientHeight <= rcWork.bottom) {
                nClientY = nFlippedY;
                actual = (m_placement == Placement::TopEnd) ? Placement::BottomEnd : Placement::Bottom;
            }
        }
        else if (bRightSide && (nClientX + nClientWidth > rcWork.right)) {
            //Not enough room on the right: try flipping to the left
            int32_t nFlippedX = rcAnchor.left - nGap - nClientWidth;
            if (nFlippedX >= rcWork.left) {
                nClientX = nFlippedX;
                actual = (m_placement == Placement::RightEnd) ? Placement::LeftEnd : Placement::Left;
            }
        }
        else if (bLeftSide && (nClientX < rcWork.left)) {
            //Not enough room on the left: try flipping to the right
            int32_t nFlippedX = rcAnchor.right + nGap;
            if (nFlippedX + nClientWidth <= rcWork.right) {
                nClientX = nFlippedX;
                actual = (m_placement == Placement::LeftEnd) ? Placement::RightEnd : Placement::Right;
            }
        }
    }
    m_actualPlacement = actual;

    //If the flyout still overflows the work area after flipping, clamp it into the work area
    if (nClientX < rcWork.left) {
        nClientX = rcWork.left;
    }
    else if (nClientX + nClientWidth > rcWork.right) {
        nClientX = rcWork.right - nClientWidth;
    }
    if (nClientY < rcWork.top) {
        nClientY = rcWork.top;
    }
    else if (nClientY + nClientHeight > rcWork.bottom) {
        nClientY = rcWork.bottom - nClientHeight;
    }

    //The window position has to include the shadow padding, and is converted from DIP back to physical pixels
    int32_t nWindowX = nClientX - rcShadow.left;
    int32_t nWindowY = nClientY - rcShadow.top;
    Dpi().ClientSizeToWindowSize(nWindowX);
    Dpi().ClientSizeToWindowSize(nWindowY);
    UiSize szWindowPhysical = szWindow;
    Dpi().ClientSizeToWindowSize(szWindowPhysical);

    SetWindowPos(InsertAfterWnd(),
                 nWindowX, nWindowY,
                 szWindowPhysical.cx, szWindowPhysical.cy,
                 kSWP_NOZORDER | kSWP_NOACTIVATE);
}

void Flyout::StartDetectTimer()
{
    StopDetectTimer();

#if defined(DUI_BUILD_FOR_WIN)
    //Seed the state with the current key state, so the click that triggered the flyout is not mistaken for a new external press before it is released
    m_bPointerDownLast = (::GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    m_bEscapeDownLast = (::GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0;

    m_nDetectTaskId = GlobalManager::Instance().Thread().PostDelayedTask(
        kThreadUI,
        UiBind(this, [this]() {
            m_nDetectTaskId = 0;
            OnDetectTick();
        }),
        kDetectIntervalMs);
#else
    //Outside click detection in no-focus mode is not supported on non-Windows platforms yet; use focus mode through SetNoFocus(false)
#endif
}

void Flyout::StopDetectTimer()
{
    if (m_nDetectTaskId != 0) {
        GlobalManager::Instance().Thread().CancelTask(m_nDetectTaskId);
        m_nDetectTaskId = 0;
    }
}

void Flyout::OnDetectTick()
{
    if (!m_bOpen || m_bClosing) {
        return;
    }

    //Close when this window or the parent window becomes invalid (destroyed/hidden/minimized)
    if (!IsWindow() || !IsWindowVisible()) {
        DoClose(CloseReason::kAnchorLost);
        return;
    }
    if (m_pParentWindow != nullptr) {
        if (!m_pParentWindow->IsWindow() || !m_pParentWindow->IsWindowVisible() ||
            m_pParentWindow->IsWindowMinimized()) {
            DoClose(CloseReason::kAnchorLost);
            return;
        }
    }
    if (m_pAnchor == nullptr) {
        DoClose(CloseReason::kAnchorLost);
        return;
    }

#if defined(DUI_BUILD_FOR_WIN)
    if (m_bAutoDismiss) {
        //Close on the press edge of any mouse button when the point is inside neither the flyout nor the anchor
        //(the click is not swallowed: the message still goes to the window that was clicked, e.g. another trigger button)
        const bool bPointerDown = ((::GetAsyncKeyState(VK_LBUTTON) |
                                    ::GetAsyncKeyState(VK_RBUTTON) |
                                    ::GetAsyncKeyState(VK_MBUTTON)) & 0x8000) != 0;
        const bool bPointerPressed = bPointerDown && !m_bPointerDownLast;
        m_bPointerDownLast = bPointerDown;
        if (bPointerPressed) {
            UiPoint ptCursor;
            GetCursorPos(ptCursor);

            UiRect rcFlyout;
            GetWindowRect(rcFlyout);
            const bool bInsideFlyout = rcFlyout.ContainsPt(ptCursor);

            if (!bInsideFlyout && !IsPointInAnchor(ptCursor)) {
                DoClose(CloseReason::kClickOutside);
                return;
            }
        }

        //Press edge of the Esc key
        const bool bEscapeDown = (::GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0;
        const bool bEscapePressed = bEscapeDown && !m_bEscapeDownLast;
        m_bEscapeDownLast = bEscapeDown;
        if (bEscapePressed) {
            DoClose(CloseReason::kEscape);
            return;
        }
    }
#endif

    if (m_bOpen && !m_bClosing) {
        m_nDetectTaskId = GlobalManager::Instance().Thread().PostDelayedTask(
            kThreadUI,
            UiBind(this, [this]() {
                m_nDetectTaskId = 0;
                OnDetectTick();
            }),
            kDetectIntervalMs);
    }
}

void Flyout::DoClose(CloseReason reason)
{
    if (m_bClosing) {
        return;
    }
    m_bClosing = true;
    m_bOpen = false;

    StopDetectTimer();

    if (s_pActiveFlyout == this) {
        s_pActiveFlyout = nullptr;
    }

    //Invoke the callbacks before the window is actually closed
    std::vector<FlyoutEvent> callbacks = m_closedCallbacks;
    for (const FlyoutEvent& callback : callbacks) {
        callback(reason);
    }

    //In no-focus mode, clicking the flyout content makes the framework move the native keyboard focus to the
    //flyout window (the "make sure the clicked window has the input focus" logic in Window::OnButtonDown), while
    //closing the window is asynchronous (CloseWnd posts a WM_CLOSE message): if the focus is not handed back to
    //the parent window before closing, the native focus is lost once the flyout is destroyed and the edit control
    //in the parent window still receives no keyboard messages even though it holds the framework focus
    //(which looks like "typing no longer works after clicking a candidate item").
    //Restore the focus only when the flyout itself holds it, the parent window is still the foreground window and
    //does not hold the focus yet, so the focus is not stolen from other windows or from child windows of the
    //parent (such as a native CEF child window).
    if (m_bNoFocus && IsWindowForeground() &&
        (m_pParentWindow != nullptr) && !m_pParentWindow->IsWindowFocused()) {
        if (!m_pParentWindow->IsWindowForeground()) {
            m_pParentWindow->SetWindowForeground();
        }
        m_pParentWindow->SetWindowFocus();
    }

    if (IsWindow()) {
        CloseWnd();
    }
}

} // namespace ui
