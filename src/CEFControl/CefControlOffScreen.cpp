#include "dui/CEFControl/CefControlOffScreen.h"

#ifdef DUI_BUILD_FOR_CEF

#include "dui/Core/Window.h"

#include "dui/CEFControl/CefManager.h"
#include "dui/CEFControl/internal/CefBrowserHandler.h"
#include "dui/CEFControl/internal/CefMemoryBlock.h"

#include "dui/Core/GlobalManager.h"
#include "dui/Core/Box.h"

#include <cstdlib>
#include <cmath>
#include <functional>

#if defined (DUI_BUILD_FOR_WIN)
    #include "dui/CEFControl/internal/Windows/util_win.h"
    #include "dui/CEFControl/internal/Windows/osr_ime_handler_win.h"
#endif

#if defined (DUI_BUILD_FOR_WAYLAND)
#endif

namespace ui {

CefControlOffScreen::CefControlOffScreen(Window* pWindow) :
    CefControl(pWindow),
    m_bHasFocusNode(false),
    m_bFocusNodeEditable(false),
    m_bInGotFocusEvent(false)
{
    m_pCefMemData = std::make_unique<CefMemoryBlock>();
    m_pCefPopupMemData = std::make_unique<CefMemoryBlock>();
}

CefControlOffScreen::~CefControlOffScreen(void)
{
    DoCloseAllBrowsers(true);
    if (m_pBrowserHandler.get()) {
        m_pBrowserHandler->SetHostWindow(nullptr);
        m_pBrowserHandler->SetHandlerDelegate(nullptr);
    }
}

void CefControlOffScreen::OnPaint(CefRefPtr<CefBrowser> /*browser*/, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height)
{
    ASSERT(CefCurrentlyOn(TID_UI));
    //Only off-screen rendering uses this paint interface

if ((buffer == nullptr) || (width <= 0) || (height <= 0) ){
        return;
    }

    std::vector<UiRect> dirtyRectList;
    for (const CefRect& rect : dirtyRects) {
        dirtyRectList.push_back(UiRect(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height));
    }
    if (type == PET_VIEW) {
        //Paint data of the page
        m_pCefMemData->Init(buffer, dirtyRectList, width, height);
    }
    else if (type == PET_POPUP) {
        ////Paint data of the page popup window
        m_pCefPopupMemData->Init(buffer, dirtyRectList, width, height);
    }

    //Call Invalidate on the UI thread to trigger painting
    GlobalManager::Instance().Thread().PostTask(kThreadUI, UiBind(&CefControlOffScreen::Invalidate, this));
}

void CefControlOffScreen::ClientToControl(UiPoint& pt)
{
    auto offset = GetScrollOffsetInScrollBox();
    pt.x = pt.x + offset.x - GetRect().left;
    pt.y = pt.y + offset.y - GetRect().top;

    //The value passed back is the raw value without DPI scaling (96 DPI); CEF then applies DPI scaling internally
    Dpi().UnscaleInt(pt.x);//TODO
    Dpi().UnscaleInt(pt.y);
}

void CefControlOffScreen::OnPopupShow(CefRefPtr<CefBrowser> browser, bool show)
{
    ASSERT(CefCurrentlyOn(TID_UI));
    if (!show) {
        // When the popup window is hidden, refresh the popup area
        m_rectPopup.Set(0, 0, 0, 0);
        if (browser->GetHost() != nullptr) {
            browser->GetHost()->Invalidate(PET_VIEW);
        }
    }
}

void CefControlOffScreen::OnPopupSize(CefRefPtr<CefBrowser> /*browser*/, const CefRect& rect)
{
    ASSERT(CefCurrentlyOn(TID_UI));
if ((rect.width <= 0) || (rect.height <= 0) ){
        return;
    }
    m_rectPopup = rect;
}

void CefControlOffScreen::Init()
{
    GlobalManager::Instance().AssertUIThread();
    if (m_pBrowserHandler.get() == nullptr) {
        m_pBrowserHandler = new CefBrowserHandler;
        m_pBrowserHandler->SetHostWindow(GetWindow());
        m_pBrowserHandler->SetHandlerDelegate(this);

        //Create the Browser object asynchronously to avoid blocking the parsing and display speed of the main interface
        GlobalManager::Instance().Thread().PostTask(ui::kThreadUI, UiBind(&CefControlOffScreen::ReCreateBrowser, this));
    }

    if (!m_jsBridge.get()) {
        m_jsBridge.reset(new CefJSBridge);
    }
#if defined(DUI_BUILD_FOR_MACOS)
    RegisterImeCaretUpdater();
#endif
    BaseClass::Init();
}

void CefControlOffScreen::ReCreateBrowser()
{
    GlobalManager::Instance().AssertUIThread();
    Window* pWindow = GetWindow();
    ASSERT(pWindow != nullptr);
    if (pWindow == nullptr) {
        return;
    }
    ASSERT(pWindow->IsWindow());
    ASSERT(m_pBrowserHandler != nullptr);
    if (m_pBrowserHandler == nullptr) {
        return;
    }

    if (m_pBrowserHandler->GetBrowser() != nullptr) {
        //Already created, do not create the Browser object again
        return;
    }

    // Use windowless mode for off-screen rendering
    CefWindowInfo window_info;
#if CEF_VERSION_MAJOR > 109
    //This parameter must be explicitly initialized and cannot use the default value
    window_info.runtime_style = CEF_RUNTIME_STYLE_ALLOY;
#endif

#ifdef DUI_BUILD_FOR_WIN
    HWND hWnd = pWindow->NativeWnd()->GetHWND();
    window_info.SetAsWindowless(hWnd);
    if (::GetWindowLongPtr(hWnd, GWL_EXSTYLE) & WS_EX_NOACTIVATE) {
        // Don't activate the browser window on creation.
        window_info.ex_style |= WS_EX_NOACTIVATE;
    }
#elif defined (DUI_BUILD_FOR_LINUX) || defined (DUI_BUILD_FOR_FREEBSD)
    CefWindowHandle hParenWindow = (CefWindowHandle)pWindow->NativeWnd()->GetX11WindowNumber();
if (pWindow->NativeWnd()->IsVideoDriverWayland() ){
        hParenWindow = (CefWindowHandle)pWindow->NativeWnd()->GetWaylandDisplayPointer();
    }
    window_info.SetAsWindowless(hParenWindow);
#elif defined DUI_BUILD_FOR_MACOS
    window_info.SetAsWindowless(pWindow->NativeWnd()->GetNSView());
#endif
    CefBrowserSettings browser_settings;
    // Background color: set to white (if not set, it is transparent; if the web page does not set a background color, the background is displayed as black)
    browser_settings.background_color = CefColorSetARGB(255, 255, 255, 255);
    //browser_settings.file_access_from_file_urls = STATE_ENABLED;
    //browser_settings.universal_access_from_file_urls = STATE_ENABLED;
    CefString url = GetInitURL();//The URL to load immediately after creation succeeds
    CefBrowserHost::CreateBrowser(window_info, m_pBrowserHandler, url, browser_settings, nullptr, nullptr);
}

void CefControlOffScreen::SetPos(UiRect rc)
{
    GlobalManager::Instance().AssertUIThread();
    BaseClass::SetPos(rc);

    if (m_pBrowserHandler.get()) {
        m_pBrowserHandler->SetViewRect(UiRect(rc.left, rc.top, rc.right, rc.bottom));
    }
}

void CefControlOffScreen::OnSetVisible(bool bChanged)
{
    GlobalManager::Instance().AssertUIThread();
    BaseClass::OnSetVisible(bChanged);
    if (m_pBrowserHandler.get() && m_pBrowserHandler->GetBrowserHost().get()) {
        m_pBrowserHandler->GetBrowserHost()->WasHidden(!IsVisible());
    }
}

void CefControlOffScreen::Paint(IRender* pRender, const UiRect& rcPaint)
{
    GlobalManager::Instance().AssertUIThread();
    BaseClass::Paint(pRender, rcPaint);
if ((pRender == nullptr) || (m_pBrowserHandler == nullptr) || (m_pBrowserHandler->GetBrowser() == nullptr) ){
        return;
    }

if (m_pCefMemData->IsValid() ){
        // Draw the bitmap of the cef PET_VIEW type
        const UiRect rect = GetRect();
        bool bRectValid = true;
if ((m_pCefMemData->GetWidth() != rect.Width() || (m_pCefMemData->GetHeight() != rect.Height()) )){
            bRectValid = false;
        }

if (!rcPaint.IsEmpty() ){
            if (bRectValid) {
                //The areas match, exactly the same size as the web page content
                m_pCefMemData->PaintData(pRender, rect);
            }
            else {
                //If the areas do not match, draw according to the actual size of the image data, then trigger a draw event again (to avoid drawing beyond the control boundary and covering other controls)
                UiRect rc = rect;
                rc.right = std::min(rc.left + m_pCefMemData->GetWidth(), rect.right);
                rc.bottom = std::min(rc.top + m_pCefMemData->GetHeight(), rect.bottom);
                m_pCefMemData->PaintData(pRender, rc);
                m_pBrowserHandler->SetViewRect(rect);
            }
        }

        // Draw the bitmap of the cef PET_POPUP type
if (!m_rectPopup.IsEmpty() && m_pCefPopupMemData->IsValid() ){
            // If the popup window position is outside the control range, correct it to be within the control range; only draw the popup window within the control range
            UiRect dcPaint = rect;
            dcPaint.left += Dpi().GetScaleInt(m_rectPopup.x);
            dcPaint.top += Dpi().GetScaleInt(m_rectPopup.y);
            dcPaint.right = std::min(dcPaint.left + m_pCefPopupMemData->GetWidth(), rect.right);
            dcPaint.bottom = std::min(dcPaint.top + m_pCefPopupMemData->GetHeight(), rect.bottom);
if (!rcPaint.IsEmpty() ){
                m_pCefPopupMemData->PaintData(pRender, dcPaint);
            }
        }
    }
}

void CefControlOffScreen::SetWindow(Window* pWindow)
{
    GlobalManager::Instance().AssertUIThread();
    BaseClass::SetWindow(pWindow);
    if (m_pBrowserHandler) {
        m_pBrowserHandler->SetHostWindow(pWindow);
        m_pBrowserHandler->SetHandlerDelegate(this);
    }    
}

void CefControlOffScreen::AdaptDpiScale(CefMouseEvent& mouse_event)
{
if (CefManager::GetInstance()->IsEnableOffScreenRendering() ){
        // In off-screen rendering mode, the raw width and height need to be passed, because CEF internally performs further DPI adaptation
        if (GetWindow() != nullptr) {
            GetWindow()->Dpi().UnscaleInt(mouse_event.x);
            GetWindow()->Dpi().UnscaleInt(mouse_event.y);
        }
    }
}

#ifdef DUI_BUILD_FOR_WAYLAND

// Convert the CEF cursor type to the dui standard cursor type (only part of the cursor types are supported)
static CursorType CefCursorTypeToUiCursor(cef_cursor_type_t cefCursor)
{
    switch (cefCursor) {
    case CT_POINTER:           return CursorType::kCursorArrow;          // Pointer -> standard arrow
    case CT_CROSS:             return CursorType::kCursorCross;          // Crosshair cursor -> crosshair
    case CT_HAND:              return CursorType::kCursorHand;           // Hand cursor -> hand
    case CT_IBEAM:             return CursorType::kCursorIBeam;          // Text cursor -> I-beam cursor
    case CT_WAIT:              return CursorType::kCursorWait;           // Wait cursor -> hourglass
    //case CT_HELP:              return IDC_HELP;           // Help cursor -> help arrow

        // Direction adjustment cursors
    case CT_EASTRESIZE:        return CursorType::kCursorSizeWE;         // East resize -> horizontal resize
    case CT_NORTHRESIZE:       return CursorType::kCursorSizeNS;         // North resize -> vertical resize
    case CT_NORTHEASTRESIZE:   return CursorType::kCursorSizeNESW;       // Northeast resize
    case CT_NORTHWESTRESIZE:   return CursorType::kCursorSizeNWSE;       // Northwest resize
    case CT_SOUTHRESIZE:       return CursorType::kCursorSizeNS;         // South resize -> vertical resize
    case CT_SOUTHEASTRESIZE:   return CursorType::kCursorSizeNWSE;       // Southeast resize
    case CT_SOUTHWESTRESIZE:   return CursorType::kCursorSizeNESW;       // Southwest resize
    case CT_WESTRESIZE:        return CursorType::kCursorSizeWE;         // West resize -> horizontal resize

        // Bidirectional adjustment cursors
    case CT_NORTHSOUTHRESIZE:  return CursorType::kCursorSizeNS;          // North-south resize -> vertical resize
    case CT_EASTWESTRESIZE:    return CursorType::kCursorSizeWE;          // East-west resize -> horizontal resize
    case CT_NORTHEASTSOUTHWESTRESIZE: return CursorType::kCursorSizeNESW; // Northeast-southwest resize
    case CT_NORTHWESTSOUTHEASTRESIZE: return CursorType::kCursorSizeNWSE; // Northwest-southeast resize

        // Other mappable types
    case CT_COLUMNRESIZE:      return CursorType::kCursorSizeWE;         // Column resize -> horizontal resize
    case CT_ROWRESIZE:         return CursorType::kCursorSizeNS;         // Row resize -> vertical resize
    case CT_MOVE:              return CursorType::kCursorSizeAll;        // Move -> four-direction resize
    case CT_PROGRESS:          return CursorType::kCursorProgress;       // Progress -> application startup cursor
    case CT_NODROP:            return CursorType::kCursorNo;             // No drop -> forbidden cursor
    case CT_NOTALLOWED:        return CursorType::kCursorNo;             // Not allowed -> forbidden cursor
    case CT_COPY:              return CursorType::kCursorArrow;          // Copy -> standard arrow (customizable)

        // The following types have no direct Windows standard cursor equivalent; return nothing
    case CT_MIDDLEPANNING:
    case CT_EASTPANNING:
    case CT_NORTHPANNING:
    case CT_NORTHEASTPANNING:
    case CT_NORTHWESTPANNING:
    case CT_SOUTHPANNING:
    case CT_SOUTHEASTPANNING:
    case CT_SOUTHWESTPANNING:
    case CT_WESTPANNING:
    case CT_VERTICALTEXT:
    case CT_CELL:
    case CT_CONTEXTMENU:
    case CT_ALIAS:
    case CT_NONE:
    case CT_ZOOMIN:
    case CT_ZOOMOUT:
    case CT_GRAB:
    case CT_GRABBING:
    case CT_MIDDLE_PANNING_VERTICAL:
    case CT_MIDDLE_PANNING_HORIZONTAL:
    case CT_CUSTOM:
    case CT_DND_NONE:
    case CT_DND_MOVE:
    case CT_DND_COPY:
    case CT_DND_LINK:
    //case CT_NUM_VALUES:
    default:
        break;
    }
    return CursorType::kCursorArrow;
}

#endif //DUI_BUILD_FOR_WAYLAND

void CefControlOffScreen::OnCursorChange(cef_cursor_type_t type)
{
#ifdef DUI_BUILD_FOR_WAYLAND
    CursorType uiCursorType = CefCursorTypeToUiCursor(type);
    SetCursorType(uiCursorType);
#else
    (void)type;
#endif
}

bool CefControlOffScreen::OnSetCursor(const EventArgs& msg)
{
#ifdef DUI_BUILD_FOR_WAYLAND
    // When using native backend, the cursor needs to be set
    return BaseClass::OnSetCursor(msg);
#else
    // In off-screen rendering, the control itself does not handle the cursor; the CEF module handles it internally, otherwise the mouse cursor in Cef would be affected
    (void)msg;
    return true;
#endif
}

bool CefControlOffScreen::OnCaptureChanged(const EventArgs& /*msg*/)
{
    CefRefPtr<CefBrowserHost> host;
    if (m_pBrowserHandler != nullptr) {
        host = m_pBrowserHandler->GetBrowserHost();
    }
    if (host != nullptr) {
        host->SendCaptureLostEvent();
    }
    return false;
}

int32_t CefControlOffScreen::GetCefMouseModifiers(const EventArgs& /*msg*/) const
{
    int32_t modifiers = 0;
if (Keyboard::IsKeyDown(kVK_CONTROL) ){
        modifiers |= EVENTFLAG_CONTROL_DOWN;
    }
if (Keyboard::IsKeyDown(kVK_SHIFT) ){
        modifiers |= EVENTFLAG_SHIFT_DOWN;
    }
if (Keyboard::IsKeyDown(kVK_MENU) ){
        modifiers |= EVENTFLAG_ALT_DOWN;
    }
if (Keyboard::IsKeyDown(kVK_LBUTTON) ){
        modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
    }
if (Keyboard::IsKeyDown(kVK_MBUTTON) ){
        modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
    }
if (Keyboard::IsKeyDown(kVK_RBUTTON) ){
        modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;
    }

    // Low bit set from GetKeyState indicates "toggled".
if (Keyboard::IsKeyDown(kVK_NUMLOCK) ){
        modifiers |= EVENTFLAG_NUM_LOCK_ON;
    }
if (Keyboard::IsKeyDown(kVK_CAPITAL) ){
        modifiers |= EVENTFLAG_CAPS_LOCK_ON;
    }
    return modifiers;
}

bool CefControlOffScreen::MouseMove(const EventArgs& msg)
{
    bool bRet = BaseClass::MouseMove(msg);
    CefRefPtr<CefBrowserHost> host;
    if (m_pBrowserHandler != nullptr) {
        host = m_pBrowserHandler->GetBrowserHost();
    }
    Window* pWindow = GetWindow();
if ((pWindow != nullptr) && (host != nullptr) ){
        UiPoint pt = msg.ptMouse;
        pt.Offset(GetScrollOffsetInScrollBox());
if (!GetRect().ContainsPt(pt) && !pWindow->IsCaptured() ){
            return false;
        }

        CefMouseEvent mouse_event;
        mouse_event.x = pt.x - GetRect().left;
        mouse_event.y = pt.y - GetRect().top;
        mouse_event.modifiers = GetCefMouseModifiers(msg);
        AdaptDpiScale(mouse_event);
        host->SendMouseMoveEvent(mouse_event, false);
    }
    return bRet;
}

bool CefControlOffScreen::MouseLeave(const EventArgs& msg)
{
    bool bRet = BaseClass::MouseLeave(msg);
    CefRefPtr<CefBrowserHost> host;
    if (m_pBrowserHandler != nullptr) {
        host = m_pBrowserHandler->GetBrowserHost();
    }
    if (host == nullptr) {
        return bRet;
    }

    UiPoint pt = msg.ptMouse;
    pt.Offset(GetScrollOffsetInScrollBox());
if (!GetRect().ContainsPt(pt) ){
        return bRet;
    }

    CefMouseEvent mouse_event;
    mouse_event.x = pt.x - GetRect().left;
    mouse_event.y = pt.y - GetRect().top;
    mouse_event.modifiers = GetCefMouseModifiers(msg);
    AdaptDpiScale(mouse_event);
    host->SendMouseMoveEvent(mouse_event, true);
    return bRet;
}

bool CefControlOffScreen::MouseWheel(const EventArgs& msg)
{
    bool bRet = BaseClass::MouseWheel(msg);
    CefRefPtr<CefBrowserHost> host;
    if (m_pBrowserHandler != nullptr) {
        host = m_pBrowserHandler->GetBrowserHost();
    }
    if (host == nullptr) {
        return bRet;
    }
    Window* pWindow = GetWindow();
    if (pWindow == nullptr) {
        return bRet;
    }

    UiPoint pt = msg.ptMouse;
    Window* pScrolledWnd = pWindow->WindowFromPoint(pt);
    if (pScrolledWnd != pWindow) {
        return bRet;
    }

    pt.Offset(GetScrollOffsetInScrollBox());
if (!GetRect().ContainsPt(pt) ){
        return bRet;
    }

    int delta = msg.eventData;
    CefMouseEvent mouse_event;
    mouse_event.x = pt.x - GetRect().left;
    mouse_event.y = pt.y - GetRect().top;
    mouse_event.modifiers = GetCefMouseModifiers(msg);
    AdaptDpiScale(mouse_event);
    bool bShiftDown = Keyboard::IsKeyDown(kVK_SHIFT);
    host->SendMouseWheelEvent(mouse_event, bShiftDown ? delta : 0, !bShiftDown ? delta : 0);
    return bRet;
}

bool CefControlOffScreen::ButtonDown(const EventArgs& msg)
{
    bool bRet = BaseClass::ButtonDown(msg);
    SendButtonDownEvent(msg);
    return bRet;
}

bool CefControlOffScreen::ButtonUp(const EventArgs& msg)
{
    bool bRet = BaseClass::ButtonUp(msg);
    SendButtonUpEvent(msg);
    return bRet;
}

bool CefControlOffScreen::ButtonDoubleClick(const EventArgs& msg)
{
    bool bRet = BaseClass::ButtonDoubleClick(msg);
    SendButtonDoubleClickEvent(msg);
    return bRet;
}

bool CefControlOffScreen::RButtonDown(const EventArgs& msg)
{
    bool bRet = BaseClass::RButtonDown(msg);
    SendButtonDownEvent(msg);
    return bRet;
}

bool CefControlOffScreen::RButtonUp(const EventArgs& msg)
{
    bool bRet = BaseClass::RButtonUp(msg);
    SendButtonUpEvent(msg);
    return bRet;
}

bool CefControlOffScreen::RButtonDoubleClick(const EventArgs& msg)
{
    bool bRet = BaseClass::RButtonDoubleClick(msg);
    SendButtonDoubleClickEvent(msg);
    return bRet;
}

bool CefControlOffScreen::MButtonDown(const EventArgs& msg)
{
    bool bRet = BaseClass::MButtonDown(msg);
    SendButtonDownEvent(msg);
    return bRet;
}

bool CefControlOffScreen::MButtonUp(const EventArgs& msg)
{
    bool bRet = BaseClass::MButtonUp(msg);
    SendButtonUpEvent(msg);
    return bRet;
}

bool CefControlOffScreen::MButtonDoubleClick(const EventArgs& msg)
{
    bool bRet = BaseClass::MButtonDoubleClick(msg);
    SendButtonDoubleClickEvent(msg);
    return bRet;
}

void CefControlOffScreen::SendButtonDownEvent(const EventArgs& msg)
{
    CefRefPtr<CefBrowserHost> host;
    if (m_pBrowserHandler != nullptr) {
        host = m_pBrowserHandler->GetBrowserHost();
    }
    if (host == nullptr) {
        return;
    }

    UiPoint pt = msg.ptMouse;
    pt.Offset(GetScrollOffsetInScrollBox());
if (!GetRect().ContainsPt(pt) ){
        return;
    }

    SetFocus();
    CefMouseEvent mouse_event;
    mouse_event.x = pt.x - GetRect().left;
    mouse_event.y = pt.y - GetRect().top;
    mouse_event.modifiers = GetCefMouseModifiers(msg);

    CefBrowserHost::MouseButtonType btnType =
        (msg.eventType == kEventMouseButtonDown ? MBT_LEFT : (
            msg.eventType == kEventMouseRButtonDown ? MBT_RIGHT : MBT_MIDDLE));
    AdaptDpiScale(mouse_event);
    host->SendMouseClickEvent(mouse_event, btnType, false, 1);
}

void CefControlOffScreen::SendButtonUpEvent(const EventArgs& msg)
{
    CefRefPtr<CefBrowserHost> host;
    if (m_pBrowserHandler != nullptr) {
        host = m_pBrowserHandler->GetBrowserHost();
    }
    if (host == nullptr) {
        return;
    }

    UiPoint pt = msg.ptMouse;
    pt.Offset(GetScrollOffsetInScrollBox());
if (!GetRect().ContainsPt(pt) && !GetWindow()->IsCaptured() ){
        return;
    }

    CefMouseEvent mouse_event;
    mouse_event.x = pt.x - GetRect().left;
    mouse_event.y = pt.y - GetRect().top;
    mouse_event.modifiers = GetCefMouseModifiers(msg);
    AdaptDpiScale(mouse_event);
    CefBrowserHost::MouseButtonType btnType =
        (msg.eventType == kEventMouseButtonUp ? MBT_LEFT : (
            msg.eventType == kEventMouseRButtonUp ? MBT_RIGHT : MBT_MIDDLE));
    host->SendMouseClickEvent(mouse_event, btnType, true, 1);
}

void CefControlOffScreen::SendButtonDoubleClickEvent(const EventArgs& msg)
{
    CefRefPtr<CefBrowserHost> host;
    if (m_pBrowserHandler != nullptr) {
        host = m_pBrowserHandler->GetBrowserHost();
    }
    if (host == nullptr) {
        return;
    }

    UiPoint pt = msg.ptMouse;
    pt.Offset(GetScrollOffsetInScrollBox());
if (!GetRect().ContainsPt(pt) ){
        return;
    }

    CefMouseEvent mouse_event;
    mouse_event.x = pt.x - GetRect().left;
    mouse_event.y = pt.y - GetRect().top;
    mouse_event.modifiers = GetCefMouseModifiers(msg);
    AdaptDpiScale(mouse_event);
    CefBrowserHost::MouseButtonType btnType =
        (msg.eventType == kEventMouseDoubleClick ? MBT_LEFT : (
            msg.eventType == kEventMouseRDoubleClick ? MBT_RIGHT : MBT_MIDDLE));
    host->SendMouseClickEvent(mouse_event, btnType, true, 2);
}

void CefControlOffScreen::OnGotFocus()
{
if (!IsVisible() || !IsEnabled() ){
        return;
    }
if (!IsFocused() ){
        // Avoid the appearance of double-focus controls
        m_bInGotFocusEvent = true;
        SetFocus();
        m_bInGotFocusEvent = false;
    }
}

bool CefControlOffScreen::OnSetFocus(const EventArgs& /*msg*/)
{
    // Do not call the base class method (the base class method would close the input method)
    if (GetState() == kControlStateNormal) {
        SetState(kControlStateHot);
    }

    // Set the input method related properties
    if (m_bHasFocusNode) {
        OnFocusedNodeChanged(m_bFocusNodeEditable, m_focusNodeRect);
    }
    else {
        Window* pWindow = GetWindow();
        if (pWindow != nullptr) {
            pWindow->NativeWnd()->SetTextInputArea(nullptr, 0);
            pWindow->NativeWnd()->SetImeOpenStatus(false);
        }
    }
    Invalidate();

    if (!m_bInGotFocusEvent) {
        // Avoid calling CefBrowserHost::SetFocus in the OnGotFocus callback function, which can easily cause an infinite loop
        CefRefPtr<CefBrowserHost> browserHost = GetCefBrowserHost();
        if (browserHost != nullptr) {
            browserHost->SetFocus(true);
        }
    }
    return true;
}

bool CefControlOffScreen::OnKillFocus(const EventArgs& msg)
{
    CefRefPtr<CefBrowserHost> browserHost = GetCefBrowserHost();
    if (browserHost != nullptr) {
        browserHost->SetFocus(false);
    }
    return BaseClass::OnKillFocus(msg);
}

bool CefControlOffScreen::OnChar(const EventArgs& msg)
{
    bool bRet = BaseClass::OnChar(msg);
#if defined (DUI_BUILD_FOR_WIN)
    bool bHandled = false;
    if (msg.modifierKey & ModifierKey::kIsSystemKey) {
        SendKeyEvent(WM_SYSCHAR, msg.wParam, msg.lParam, bHandled);
    }
    else {
        SendKeyEvent(WM_CHAR, msg.wParam, msg.lParam, bHandled);
    }
    return bRet || bHandled;
#elif defined(DUI_BUILD_FOR_MACOS)
    // insertText: on macOS commits an ongoing IME composition. CEF expects
    // ImeCommitText() in that case; sending only KEYEVENT_CHAR leaves the old
    // pinyin composition visible after the committed Chinese text.
    if (m_bImeComposition && (msg.wParam != 0) && (msg.lParam > 0)) {
        CefRefPtr<CefBrowserHost> host = GetCefBrowserHost();
        if (host != nullptr) {
            std::wstring text = (std::wstring::value_type*)msg.wParam;
            CefString commitText(text);
            host->ImeCommitText(commitText, CefRange::InvalidRange(), 0);
            m_bImeComposition = false;
            m_imeMarkedText.clear();
            // Chromium updates selectionStart after the commit has been
            // processed. Querying immediately still returns the old caret.
            GlobalManager::Instance().Thread().PostDelayedTask(
                kThreadUI,
                ToWeakCallback([this]() { QueryImeCaretFromJS(); }),
                50);
        }
        return true;
    }

    SendKeyEvent(msg, KEYEVENT_CHAR);
    return bRet;
#else
    SendKeyEvent(msg, KEYEVENT_CHAR);
    return bRet;
#endif
}

bool CefControlOffScreen::OnKeyDown(const EventArgs& msg)
{
    bool bRet = BaseClass::OnKeyDown(msg);
#if defined (DUI_BUILD_FOR_WIN)
    bool bHandled = false;
    if (msg.modifierKey & ModifierKey::kIsSystemKey) {
        SendKeyEvent(WM_SYSKEYDOWN, msg.wParam, msg.lParam, bHandled);
    }
    else {
        SendKeyEvent(WM_KEYDOWN, msg.wParam, msg.lParam, bHandled);
    }
    return bRet || bHandled;
#elif defined(DUI_BUILD_FOR_MACOS)
    // During an active IME composition, Backspace is handled by the system
    // updating the marked text (which we forward with ImeSetComposition).
    // Sending KEYEVENT_KEYDOWN as well makes CEF delete/redraw twice and
    // causes flicker.
    if (m_bImeComposition && (msg.vkCode == kVK_BACK)) {
        return bRet;
    }
    SendKeyEvent(msg, KEYEVENT_KEYDOWN);
    return bRet;
#else
    SendKeyEvent(msg, KEYEVENT_KEYDOWN);
    return bRet;
#endif
}

bool CefControlOffScreen::OnKeyUp(const EventArgs& msg)
{
    bool bRet = BaseClass::OnKeyUp(msg);
#if defined (DUI_BUILD_FOR_WIN)
    bool bHandled = false;
    if (msg.modifierKey & ModifierKey::kIsSystemKey) {
        SendKeyEvent(WM_SYSKEYUP, msg.wParam, msg.lParam, bHandled);
    }
    else {
        SendKeyEvent(WM_KEYUP, msg.wParam, msg.lParam, bHandled);
    }
    return bRet || bHandled;
#elif defined(DUI_BUILD_FOR_MACOS)
    // See OnKeyDown: avoid double-handling Backspace during composition.
    if (m_bImeComposition && (msg.vkCode == kVK_BACK)) {
        return bRet;
    }
    SendKeyEvent(msg, KEYEVENT_KEYUP);
    return bRet;
#else
    SendKeyEvent(msg, KEYEVENT_KEYUP);
    return bRet;
#endif
}

bool CefControlOffScreen::IsCefOSR() const
{
    return true;
}

bool CefControlOffScreen::IsCefOsrImeMode() const
{
    // In the 64-bit off-screen rendering mode of version 109, libcef.dll crashes internally when entering text with the input method; the reason is unknown, so the input method functionality is currently disabled (side effect: the candidate box position of the Chinese input method is incorrect)
#if defined (_WIN64)
    return (CEF_VERSION_MAJOR > 109) ? true : false;
#else
    return true;
#endif    
}

bool CefControlOffScreen::OnImeSetContext(const EventArgs& msg)
{
#if defined (DUI_BUILD_FOR_WIN)
if (IsCefOsrImeMode() ){
        OnIMESetContext(WM_IME_SETCONTEXT, msg.wParam, msg.lParam);
    }
#else
    UNUSED_VARIABLE(msg);
#endif
    return false;
}

bool CefControlOffScreen::OnImeStartComposition(const EventArgs& /*msg*/)
{
#if defined (DUI_BUILD_FOR_WIN)
if (IsCefOsrImeMode() ){
        OnIMEStartComposition();
    }
#elif defined(DUI_BUILD_FOR_MACOS)
    m_bImeComposition = true;
    m_imeMarkedText.clear();
    // The focused-node callback only provides the whole HTML input bounds.
    // Refresh the actual selection caret before the first composition update
    // so the first candidate window does not use the input's bottom edge.
    QueryImeCaretFromJS();
#endif
    return false;
}

bool CefControlOffScreen::OnImeComposition(const EventArgs& msg)
{
#if defined (DUI_BUILD_FOR_WIN)
if (IsCefOsrImeMode() ){
        OnIMEComposition(WM_IME_COMPOSITION, msg.wParam, msg.lParam);
    }
#elif defined(DUI_BUILD_FOR_MACOS)
    // macOS delivers the marked/composition text through OnNativeMarkedText:
    // wParam points to the whole string and lParam is its length. Forward it to
    // CEF so the pinyin letters and the composition underline are rendered.
    if ((msg.wParam != 0) && (msg.lParam > 0)) {
        m_bImeComposition = true;
        std::wstring text = (std::wstring::value_type*)msg.wParam;
        CefRefPtr<CefBrowser> browser;
        if (m_pBrowserHandler != nullptr) {
            browser = m_pBrowserHandler->GetBrowser();
        }
        if ((browser != nullptr) && (browser->GetHost() != nullptr)) {
            std::vector<CefCompositionUnderline> underlines;
            CefCompositionUnderline underline;
            underline.range = CefRange(0, static_cast<uint32_t>(text.size()));
            underline.color = CefColorSetARGB(255, 0, 0, 0);
            underline.background_color = CefColorSetARGB(0, 0, 0, 0);
            underline.thick = 0;
            underline.style = CEF_CUS_SOLID;
            underlines.push_back(underline);

            // InvalidRange tells CEF to replace the current marked text; the
            // selection is an empty range at the end, matching how a normal
            // IME composition cursor behaves while typing/deleting pinyin.
            CefString compositionText(text);
            browser->GetHost()->ImeSetComposition(
                compositionText,
                underlines,
                CefRange::InvalidRange(),
                CefRange(static_cast<uint32_t>(text.size()),
                         static_cast<uint32_t>(text.size())));
            m_imeMarkedText = text;
        }
    }
#else
    UNUSED_VARIABLE(msg);
#endif
    return false;
}

bool CefControlOffScreen::OnImeEndComposition(const EventArgs& /*msg*/)
{
#if defined (DUI_BUILD_FOR_WIN)
if (IsCefOsrImeMode() ){
        OnIMECancelCompositionEvent();
    }
#elif defined(DUI_BUILD_FOR_MACOS)
    m_bImeComposition = false;
    m_imeMarkedText.clear();
    CefRefPtr<CefBrowser> browser;
    if (m_pBrowserHandler != nullptr) {
        browser = m_pBrowserHandler->GetBrowser();
    }
    if ((browser != nullptr) && (browser->GetHost() != nullptr)) {
        // This path is reached when the marked text becomes empty (Backspace
        // deleted all pinyin). It must cancel the composition, not finish it;
        // finishing would commit the remaining composition as real text.
        browser->GetHost()->ImeCancelComposition();
    }

    // Composition ended (for example after committing with Space): reset the
    // stored input area so the IME does not keep using the old composition
    // caret position.
    if (m_bHasFocusNode) {
        OnFocusedNodeChanged(m_bFocusNodeEditable, m_focusNodeRect);
    }
    else {
        Window* pWindow = GetWindow();
        if (pWindow != nullptr) {
            pWindow->NativeWnd()->SetTextInputArea(nullptr, 0);
        }
    }

    // Ask the page for the real caret position (e.g. after a Space commit)
    // and update the input area to that exact position.
    QueryImeCaretFromJS();
    GlobalManager::Instance().Thread().PostDelayedTask(
        kThreadUI,
        ToWeakCallback([this]() { QueryImeCaretFromJS(); }),
        50);
#endif
    return false;
}

#if defined (DUI_BUILD_FOR_WAYLAND)

/** Get the key flags
*/
static uint32_t GetCefModifiers(Native_Keymod mod)
{
    uint32_t modifiers = 0;
    if (mod & Native_KMOD_CTRL) {
        modifiers |= EVENTFLAG_CONTROL_DOWN;
    }
    if (mod & Native_KMOD_SHIFT) {
        modifiers |= EVENTFLAG_SHIFT_DOWN;
    }
    if (mod & Native_KMOD_ALT) {
        modifiers |= EVENTFLAG_ALT_DOWN;
    }

    if (mod & Native_KMOD_CAPS) {
        modifiers |= EVENTFLAG_CAPS_LOCK_ON;
    }
    if (mod & Native_KMOD_NUM) {
        modifiers |= EVENTFLAG_NUM_LOCK_ON;
    }
    return modifiers;
}

//Linux platform
void CefControlOffScreen::SendKeyEvent(const EventArgs& msg, cef_key_event_type_t type)
{
if (!IsVisible() || !IsEnabled() ){
        return;
    }
    CefRefPtr<CefBrowserHost> host;
    if (m_pBrowserHandler != nullptr) {
        host = m_pBrowserHandler->GetBrowserHost();
    }
    if (host == nullptr) {
        return;
    }

    Native_EventType eventType = (Native_EventType)msg.wParam;
    if (type == KEYEVENT_KEYDOWN) {
        ASSERT(eventType == Native_EVENT_KEY_DOWN);
        ASSERT(msg.lParam != 0);
if ((eventType != Native_EVENT_KEY_DOWN) || (msg.lParam == 0) ){
            return;
        }
    }
    else if (type == KEYEVENT_KEYUP) {
        ASSERT(eventType == Native_EVENT_KEY_UP);
        ASSERT(msg.lParam != 0);
if ((eventType != Native_EVENT_KEY_UP) || (msg.lParam == 0) ){
            return;
        }
    }
if ((type == KEYEVENT_KEYDOWN) || (type == KEYEVENT_KEYUP) ){
        Native_KeyboardEvent* key = (Native_KeyboardEvent*)msg.lParam;
        CefKeyEvent event;
        event.type = (type == KEYEVENT_KEYDOWN) ? KEYEVENT_KEYDOWN : KEYEVENT_KEYUP;
        event.windows_key_code = msg.vkCode;
        event.native_key_code = key->scancode;
        event.is_system_key = false;
        event.modifiers = GetCefModifiers(key->mod);

        host->SendKeyEvent(event);
    }
    else if (type == KEYEVENT_CHAR) {
        ASSERT(msg.eventData == Native_EVENT_TEXT_INPUT);
        ASSERT(msg.vkCode == kVK_None);
if ((msg.eventData == Native_EVENT_TEXT_INPUT) && (msg.wParam != 0) && (msg.lParam > 0) ){
            // The currently entered character or string (for example, when entering Chinese, the candidate word is entered at once, unlike the Windows SDK which enters character by character)
            std::wstring text = (std::wstring::value_type*)msg.wParam;
            CefKeyEvent event;
            event.type = KEYEVENT_CHAR;
            event.modifiers = GetCefModifiers(Native_GetModState());
            size_t nCharCount = text.size();
            for (size_t nCharIndex = 0; nCharIndex < nCharCount; ++nCharIndex) {
                event.character = text[nCharIndex];

                //The following values need to be set on the Windows platform
                event.unmodified_character = event.character;
                event.windows_key_code = event.character;
                host->SendKeyEvent(event);
            }
        }
    }
}
#endif

#if defined (DUI_BUILD_FOR_X11)
static uint32_t GetCefX11Modifiers(uint32_t modifierKey)
{
    uint32_t modifiers = 0;
    if (modifierKey & ModifierKey::kControl) modifiers |= EVENTFLAG_CONTROL_DOWN;
    if (modifierKey & ModifierKey::kShift) modifiers |= EVENTFLAG_SHIFT_DOWN;
    if (modifierKey & ModifierKey::kAlt) modifiers |= EVENTFLAG_ALT_DOWN;
    return modifiers;
}

void CefControlOffScreen::SendKeyEvent(const EventArgs& msg, cef_key_event_type_t type)
{
    if (!IsVisible() || !IsEnabled()) return;
    CefRefPtr<CefBrowserHost> host = GetCefBrowserHost();
    if (host == nullptr) return;

    CefKeyEvent event;
    event.type = type;
    event.windows_key_code = static_cast<int>(msg.vkCode);
    event.native_key_code = static_cast<int>(msg.vkCode);
    event.modifiers = GetCefX11Modifiers(msg.modifierKey);
    if (type == KEYEVENT_CHAR) {
        if (msg.wParam == 0 || msg.lParam <= 0) return;
        const std::wstring::value_type* text =
            reinterpret_cast<const std::wstring::value_type*>(msg.wParam);
        event.character = static_cast<char16_t>(text[0]);
        event.unmodified_character = event.character;
    }
    host->SendKeyEvent(event);
}
#endif

#if defined(DUI_BUILD_FOR_MACOS)
// macOS native: forward keyboard messages to BrowserHost without native backend.
static uint16_t GetNativeKeyCodeFromDui(VirtualKeyCode vkCode)
{
    // Reverse of GetVirtualKeyCodeFromNativeKeyCode() in Keycode_MacOS.cpp.
    // CEF on macOS uses native_key_code for several editing keys (notably
    // Backspace/Delete); leaving it 0 makes those keys fail in the webpage.
    switch (vkCode) {
    case kVK_A: return 0x00;
    case kVK_S: return 0x01;
    case kVK_D: return 0x02;
    case kVK_F: return 0x03;
    case kVK_H: return 0x04;
    case kVK_G: return 0x05;
    case kVK_Z: return 0x06;
    case kVK_X: return 0x07;
    case kVK_C: return 0x08;
    case kVK_V: return 0x09;
    case kVK_B: return 0x0B;
    case kVK_Q: return 0x0C;
    case kVK_W: return 0x0D;
    case kVK_E: return 0x0E;
    case kVK_R: return 0x0F;
    case kVK_Y: return 0x10;
    case kVK_T: return 0x11;
    case kVK_1: return 0x12;
    case kVK_2: return 0x13;
    case kVK_3: return 0x14;
    case kVK_4: return 0x15;
    case kVK_6: return 0x16;
    case kVK_5: return 0x17;
    case kVK_OEM_PLUS: return 0x18;
    case kVK_9: return 0x19;
    case kVK_7: return 0x1A;
    case kVK_OEM_MINUS: return 0x1B;
    case kVK_8: return 0x1C;
    case kVK_0: return 0x1D;
    case kVK_OEM_6: return 0x1E;
    case kVK_O: return 0x1F;
    case kVK_U: return 0x20;
    case kVK_OEM_4: return 0x21;
    case kVK_I: return 0x22;
    case kVK_P: return 0x23;
    case kVK_RETURN: return 0x24;
    case kVK_L: return 0x25;
    case kVK_J: return 0x26;
    case kVK_OEM_7: return 0x27;
    case kVK_K: return 0x28;
    case kVK_OEM_1: return 0x29;
    case kVK_OEM_5: return 0x2A;
    case kVK_OEM_COMMA: return 0x2B;
    case kVK_OEM_2: return 0x2C;
    case kVK_N: return 0x2D;
    case kVK_M: return 0x2E;
    case kVK_OEM_PERIOD: return 0x2F;
    case kVK_TAB: return 0x30;
    case kVK_SPACE: return 0x31;
    case kVK_OEM_3: return 0x32;
    case kVK_BACK: return 0x33;
    case kVK_ESCAPE: return 0x35;
    case kVK_LWIN: return 0x37;
    case kVK_SHIFT: return 0x38;
    case kVK_CAPITAL: return 0x39;
    case kVK_MENU: return 0x3A;
    case kVK_CONTROL: return 0x3B;
    case kVK_RSHIFT: return 0x3C;
    case kVK_RMENU: return 0x3D;
    case kVK_RCONTROL: return 0x3E;
    case kVK_DECIMAL: return 0x41;
    case kVK_MULTIPLY: return 0x43;
    case kVK_ADD: return 0x45;
    case kVK_DIVIDE: return 0x4B;
    case kVK_SEPARATOR: return 0x4C;
    case kVK_SUBTRACT: return 0x4E;
    case kVK_NUMPAD0: return 0x52;
    case kVK_NUMPAD1: return 0x53;
    case kVK_NUMPAD2: return 0x54;
    case kVK_NUMPAD3: return 0x55;
    case kVK_NUMPAD4: return 0x56;
    case kVK_NUMPAD5: return 0x57;
    case kVK_NUMPAD6: return 0x58;
    case kVK_NUMPAD7: return 0x59;
    case kVK_F5: return 0x60;
    case kVK_F6: return 0x61;
    case kVK_F7: return 0x62;
    case kVK_F3: return 0x63;
    case kVK_F8: return 0x64;
    case kVK_F9: return 0x65;
    case kVK_F11: return 0x67;
    case kVK_F13: return 0x69;
    case kVK_F16: return 0x6A;
    case kVK_F14: return 0x6B;
    case kVK_F10: return 0x6D;
    case kVK_F12: return 0x6F;
    case kVK_F15: return 0x71;
    case kVK_HOME: return 0x73;
    case kVK_PRIOR: return 0x74;
    case kVK_DELETE: return 0x75;
    case kVK_F4: return 0x76;
    case kVK_END: return 0x77;
    case kVK_F2: return 0x78;
    case kVK_NEXT: return 0x79;
    case kVK_F1: return 0x7A;
    case kVK_LEFT: return 0x7B;
    case kVK_RIGHT: return 0x7C;
    case kVK_DOWN: return 0x7D;
    case kVK_UP: return 0x7E;
    default: return 0;
    }
}

static uint32_t GetCefModifiersFromDui(uint32_t modifierKey)
{
    uint32_t modifiers = 0;
    if (modifierKey & ModifierKey::kShift) {
        modifiers |= EVENTFLAG_SHIFT_DOWN;
    }
    if (modifierKey & ModifierKey::kControl) {
        modifiers |= EVENTFLAG_CONTROL_DOWN;
    }
    if (modifierKey & ModifierKey::kAlt) {
        modifiers |= EVENTFLAG_ALT_DOWN;
    }
    if (modifierKey & ModifierKey::kWin) {
        modifiers |= EVENTFLAG_COMMAND_DOWN;
    }
    return modifiers;
}

void CefControlOffScreen::SendKeyEvent(const EventArgs& msg, cef_key_event_type_t type)
{
    if (!IsVisible() || !IsEnabled()) {
        return;
    }
    CefRefPtr<CefBrowserHost> host;
    if (m_pBrowserHandler != nullptr) {
        host = m_pBrowserHandler->GetBrowserHost();
    }
    if (host == nullptr) {
        return;
    }

    if (type == KEYEVENT_CHAR) {
        // On macOS the native text input arrives through OnNativeInsertText:
        // wParam points to the whole UTF-16 string and lParam is its length.
        // CEF needs each character delivered as a separate KEYEVENT_CHAR.
        if ((msg.wParam != 0) && (msg.lParam > 0)) {
            std::wstring text = (std::wstring::value_type*)msg.wParam;
            const size_t nCharCount = text.size();
            for (size_t nCharIndex = 0; nCharIndex < nCharCount; ++nCharIndex) {
                CefKeyEvent event;
                event.type = KEYEVENT_CHAR;
                event.character = text[nCharIndex];
                event.unmodified_character = event.character;
                event.windows_key_code = event.character;
                event.native_key_code = 0;
                event.is_system_key = false;
                event.modifiers = GetCefModifiersFromDui(msg.modifierKey);
                host->SendKeyEvent(event);
            }
        }
        return;
    }

    CefKeyEvent event;
    event.type = type;
    event.windows_key_code = msg.vkCode;
    event.native_key_code = GetNativeKeyCodeFromDui(msg.vkCode);
    event.is_system_key = false;
    event.modifiers = GetCefModifiersFromDui(msg.modifierKey);

    // macOS CEF also uses the character fields for editing keys. In particular
    // Backspace must carry NSDeleteCharacter (0x7F), otherwise CEF treats it as
    // "cancel whole composition" instead of deleting one pinyin letter.
    switch (msg.vkCode) {
    case kVK_BACK:
        event.character = 0x7F;
        event.unmodified_character = 0x7F;
        break;
    case kVK_DELETE:
        event.character = 0xF728; // NSDeleteFunctionKey
        event.unmodified_character = 0xF728;
        break;
    case kVK_RETURN:
        event.character = L'\r';
        event.unmodified_character = L'\r';
        break;
    case kVK_TAB:
        event.character = L'\t';
        event.unmodified_character = L'\t';
        break;
    case kVK_ESCAPE:
        event.character = 0x1B;
        event.unmodified_character = 0x1B;
        break;
    case kVK_SPACE:
        event.character = L' ';
        event.unmodified_character = L' ';
        break;
    default:
        if ((msg.vkCode >= 'A') && (msg.vkCode <= 'Z')) {
            event.character = (char16_t)msg.vkCode;
            event.unmodified_character = (char16_t)msg.vkCode;
        }
        else if ((msg.vkCode >= '0') && (msg.vkCode <= '9')) {
            event.character = (char16_t)msg.vkCode;
            event.unmodified_character = (char16_t)msg.vkCode;
        }
        break;
    }

    host->SendKeyEvent(event);
}
#endif

#if defined(DUI_BUILD_FOR_MACOS)
void CefControlOffScreen::RegisterImeCaretUpdater()
{
    if (!m_jsBridge.get()) {
        return;
    }

    m_jsBridge->RegisterCppFunc(
        "__duiUpdateImeCaret",
        [this](const std::string& params, ui::ReportResultFunction callback) {
            OnUpdateImeCaretFromJS(params);
            if (callback) {
                callback(true, "{}");
            }
        },
        nullptr,
        true);
}

void CefControlOffScreen::OnUpdateImeCaretFromJS(const std::string& params)
{
    // The page sends either "x:123.4,y:456.7" or a JSON object like
    // {"x":123.4,"y":456.7} (CSS pixels / DIPs).
    int32_t nX = 0;
    int32_t nY = 0;
    auto ParseNumberAfterKey = [&params](const char* key1, const char* key2) -> double {
        size_t nPos = params.find(key1);
        if (nPos != std::string::npos) {
            return std::atof(params.c_str() + nPos + std::char_traits<char>::length(key1));
        }
        nPos = params.find(key2);
        if (nPos != std::string::npos) {
            return std::atof(params.c_str() + nPos + std::char_traits<char>::length(key2));
        }
        return 0;
    };
    nX = static_cast<int32_t>(std::lround(ParseNumberAfterKey("\"x\":", "x:")));
    nY = static_cast<int32_t>(std::lround(ParseNumberAfterKey("\"y\":", "y:")));

    // CSS pixels from the page are CEF view DIPs; convert to dui physical pixels.
    Dpi().ScaleInt(nX);
    Dpi().ScaleInt(nY);

    UiRect rcControl = GetRect();
    UiPoint scrollOffset = GetScrollOffsetInScrollBox();
    rcControl.Offset(-scrollOffset.x, -scrollOffset.y);

    UiRect inputRect;
    inputRect.left = rcControl.left + nX;
    inputRect.top = rcControl.top + nY;
    inputRect.right = inputRect.left + 2;
    inputRect.bottom = inputRect.top;

    Window* pWindow = GetWindow();
    if (pWindow != nullptr) {
        pWindow->NativeWnd()->SetTextInputArea(&inputRect, 0);
    }
}

void CefControlOffScreen::QueryImeCaretFromJS()
{
    if (m_pBrowserHandler == nullptr) {
        return;
    }
    CefRefPtr<CefBrowser> browser = m_pBrowserHandler->GetBrowser();
    if (!browser) {
        return;
    }
    CefRefPtr<CefFrame> frame = browser->GetFocusedFrame();
    if (!frame) {
        frame = browser->GetMainFrame();
    }
    if (!frame) {
        return;
    }

    const std::string script = R"(
        (() => {
            const el = document.activeElement;
            if (!el || (el.tagName !== 'INPUT' && el.tagName !== 'TEXTAREA')) return;
            if (typeof NimCefWebInstance === 'undefined' || !NimCefWebInstance.call) return;

            const start = el.selectionStart || 0;
            const beforeText = (el.value || '').substring(0, start);
            const cs = getComputedStyle(el);

            // Mirror the text before the caret to measure the caret's x offset.
            const mirror = document.createElement('div');
            mirror.style.position = 'absolute';
            mirror.style.visibility = 'hidden';
            mirror.style.whiteSpace = 'pre';
            mirror.style.font = cs.font;
            mirror.style.letterSpacing = cs.letterSpacing;
            mirror.textContent = beforeText;
            document.body.appendChild(mirror);

            const r = el.getBoundingClientRect();
            const mr = mirror.getBoundingClientRect();
            // The mirror starts at the page origin. Add the input element's
            // viewport position and text insets so the result is the actual
            // caret position, including horizontal scrolling.
            const x = r.left + parseFloat(cs.borderLeftWidth || '0') +
                      parseFloat(cs.paddingLeft || '0') + mr.width - el.scrollLeft;
            const y = r.bottom;
            document.body.removeChild(mirror);

            NimCefWebInstance.call('__duiUpdateImeCaret', 'x:' + x + ',y:' + y, (e, result) => {});
        })();
    )";

    frame->ExecuteJavaScript(script, frame->GetURL(), 0);
}
#endif

#if defined (DUI_BUILD_FOR_WIN)
static int LogicalToDevice(int value, float device_scale_factor)
{
    float scaled_val = static_cast<float>(value) * device_scale_factor;
    return static_cast<int>(std::floor(scaled_val));
}

static CefRect LogicalToDevice(const CefRect& value, float device_scale_factor)
{
    return CefRect(LogicalToDevice(value.x, device_scale_factor),
                   LogicalToDevice(value.y, device_scale_factor),
                   LogicalToDevice(value.width, device_scale_factor),
                   LogicalToDevice(value.height, device_scale_factor));
}
#endif

void CefControlOffScreen::OnImeCompositionRangeChanged(CefRefPtr<CefBrowser> /*browser*/, const CefRange& selected_range, const std::vector<CefRect>& character_bounds)
{
#if defined (DUI_BUILD_FOR_WIN)
    CefCurrentlyOn(TID_UI);
    if (m_imeHandler != nullptr) {
        float device_scale_factor = Dpi().GetDisplayScale();
        // Convert from view coordinates to device coordinates.
        CefRenderHandler::RectList device_bounds;
        CefRenderHandler::RectList::const_iterator it = character_bounds.begin();
        for (; it != character_bounds.end(); ++it) {
            CefRect value = LogicalToDevice(*it, device_scale_factor);
            value.x += GetRect().left;
            value.y += GetRect().top;
            device_bounds.push_back(value);
        }
        m_imeHandler->ChangeCompositionRange(selected_range, device_bounds);
    }
#elif defined(DUI_BUILD_FOR_MACOS)
    CefCurrentlyOn(TID_UI);
    // CEF may deliver one final composition-bounds callback after Space has
    // committed the text. Those bounds still describe the old marked text and
    // must not overwrite the caret position obtained from the DOM.
    if (!m_bImeComposition) {
        QueryImeCaretFromJS();
        return;
    }
    // CEF reports the per-character bounds of the active composition. Use the
    // caret/selection position so the IME candidate window follows the cursor
    // as the pinyin changes, instead of staying at the focused node's top-left.
    if (!character_bounds.empty()) {
        // selected_range.to is the caret boundary within the composition (the
        // end of the selected range; an empty selection has from == to).
        // When the caret is inside the composition, its x position is exactly
        // the left edge of the character at that index; when it is at the end,
        // it is the right edge of the last character. This lets us align the
        // candidate window with the real caret without magic offsets.
        size_t nIndex = 0;
        int32_t nCaretX = 0;
        if (static_cast<size_t>(selected_range.to) < character_bounds.size()) {
            nIndex = static_cast<size_t>(selected_range.to);
            nCaretX = character_bounds[nIndex].x;
        }
        else {
            nIndex = character_bounds.size() - 1;
            const CefRect& rc = character_bounds[nIndex];
            nCaretX = rc.x + rc.width;
        }

        const CefRect& rc = character_bounds[nIndex];

        UiRect rcControl = GetRect();
        UiPoint scrollOffset = GetScrollOffsetInScrollBox();
        rcControl.Offset(-scrollOffset.x, -scrollOffset.y);

        int32_t nX = nCaretX;
        int32_t nY = rc.y;
        int32_t nWidth = rc.width;
        int32_t nHeight = rc.height;
        Dpi().ScaleInt(nX);
        Dpi().ScaleInt(nY);
        Dpi().ScaleInt(nWidth);
        Dpi().ScaleInt(nHeight);

        UiRect inputRect;
        inputRect.left = rcControl.left + nX;
        inputRect.right = inputRect.left + nWidth;
        // Keep the actual composition character rectangle. AppKit uses the
        // bottom edge of firstRectForCharacterRange as the candidate origin.
        inputRect.top = rcControl.top + nY;
        inputRect.bottom = inputRect.top + nHeight;

        Window* pWindow = GetWindow();
        if (pWindow != nullptr) {
            pWindow->NativeWnd()->SetTextInputArea(&inputRect, 0);
        }
    }
    else if (m_bHasFocusNode) {
        // Composition cleared (for example after committing with Space):
        // don't keep the old composition caret position around.
        OnFocusedNodeChanged(m_bFocusNodeEditable, m_focusNodeRect);
    }
#else    
    UNUSED_VARIABLE(selected_range);
    UNUSED_VARIABLE(character_bounds);
#endif
}

void CefControlOffScreen::OnFocusedNodeChanged(CefRefPtr<CefBrowser> /*browser*/,
                                               CefRefPtr<CefFrame> /*frame*/,
                                               CefDOMNode::Type /*type*/,
                                               bool /*bText*/,
                                               bool bEditable,
                                               const CefRect& nodeRect)
{
    OnFocusedNodeChanged(bEditable, nodeRect);
}

void CefControlOffScreen::OnFocusedNodeChanged(bool bEditable, const CefRect& nodeRect)
{
    m_bHasFocusNode = true;
    m_bFocusNodeEditable = bEditable;
    m_focusNodeRect = nodeRect;

if (!IsVisible() || !IsEnabled() || !IsFocused() ){
        return;
    }
    Window* pWindow = GetWindow();
    if (pWindow == nullptr) {
        return;
    }

    // If inside a text edit box, open the input method; otherwise close it
    bool bEnableIME = bEditable;
    pWindow->NativeWnd()->SetImeOpenStatus(bEnableIME);
    if (bEnableIME) {
        UiRect rc = GetRect();
        UiPoint scrollOffset = GetScrollOffsetInScrollBox();
        rc.Offset(-scrollOffset.x, -scrollOffset.y);

        // CefDOMNode bounds are in CEF view DIPs. The dui control geometry is
        // in backing pixels when pixel density is enabled, so convert the
        // complete node rectangle before combining the two coordinate spaces.
        CefRect deviceNodeRect = nodeRect;
        Dpi().ScaleInt(deviceNodeRect.x);
        Dpi().ScaleInt(deviceNodeRect.y);
        Dpi().ScaleInt(deviceNodeRect.width);
        Dpi().ScaleInt(deviceNodeRect.height);

        UiRect inputRect;
        inputRect.left = rc.left + deviceNodeRect.x;
        inputRect.right = inputRect.left + deviceNodeRect.width;
        // Use the bottom edge of the focused input element so the macOS IME
        // candidate bar is placed below the input instead of on top of it.
        inputRect.top = rc.top + deviceNodeRect.y + deviceNodeRect.height;
        inputRect.bottom = inputRect.top;

        pWindow->NativeWnd()->SetTextInputArea(&inputRect, 0);
#if defined(DUI_BUILD_FOR_MACOS)
        // Resolve the initial position to the actual DOM caret. This is
        // asynchronous, but avoids keeping the whole input element as the
        // IME anchor after the first focus event.
        QueryImeCaretFromJS();
#endif
    }
    else {
        pWindow->NativeWnd()->SetTextInputArea(nullptr, 0);
    }
}

std::shared_ptr<IBitmap> CefControlOffScreen::MakeImageSnapshot()
{
if ((m_pCefMemData == nullptr) || (GetWindow() == nullptr)){
        return nullptr;
    }
    std::unique_ptr<IRender> render;
    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    ASSERT(pRenderFactory != nullptr);
    if (pRenderFactory != nullptr) {
        render.reset(pRenderFactory->CreateRender(GetWindow()->GetRenderDpi()));
    }
if ((render != nullptr) && m_pCefMemData->MakeImageSnapshot(render.get()) ){
        return std::shared_ptr<IBitmap>(render->MakeImageSnapshot());
    }
    return nullptr;
}

#if defined (DUI_BUILD_FOR_WIN)

LRESULT CefControlOffScreen::SendKeyEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    CefRefPtr<CefBrowserHost> host;
    if (m_pBrowserHandler != nullptr) {
        host = m_pBrowserHandler->GetBrowserHost();
    }
    if (host == nullptr) {
        return 0;
    }

    CefKeyEvent event;
    event.windows_key_code = static_cast<int>(wParam);
    event.native_key_code = static_cast<int>(lParam);
    event.is_system_key = uMsg == WM_SYSCHAR || uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP;

    if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) {
        event.type = KEYEVENT_RAWKEYDOWN;
    }
    else if (uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP) {
        event.type = KEYEVENT_KEYUP;
    }
    else {
        event.type = KEYEVENT_CHAR;
    }
    event.modifiers = client::GetCefKeyboardModifiers(wParam, lParam);

    host->SendKeyEvent(event);
    bHandled = true;
    return 0;
}

void CefControlOffScreen::OnIMEStartComposition()
{
    HWND hWnd = nullptr;
    if (GetWindow() != nullptr) {
        ASSERT(GetWindow()->IsWindow());
        hWnd = (HWND)GetWindow()->GetWindowHandle();
    }
    ASSERT(hWnd != nullptr);
    if (hWnd == nullptr) {
        return;
    }
if ((m_imeHandler == nullptr) || (m_imeHandler->GetHandlerHWND() != hWnd) ){
        //Create the IME manager
        m_imeHandler = std::make_unique<client::OsrImeHandlerWin>(hWnd);        
    }
    if (m_imeHandler) {
        m_imeHandler->SetInputLanguage();
        m_imeHandler->CreateImeWindow();
        m_imeHandler->MoveImeWindow();
        m_imeHandler->ResetComposition();
    }
}

void CefControlOffScreen::OnIMESetContext(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (GetWindow() == nullptr) {
        return;
    }
    HWND hWnd = (HWND)GetWindow()->GetWindowHandle();
    // We handle the IME Composition Window ourselves (but let the IME Candidates
    // Window be handled by IME through DefWindowProc(), so clear the
    // ISC_SHOWUICOMPOSITIONWINDOW flag:
    lParam &= ~ISC_SHOWUICOMPOSITIONWINDOW;
    ::DefWindowProc(hWnd, message, wParam, lParam);

    // Create Caret Window if required
if ((m_imeHandler == nullptr) || (m_imeHandler->GetHandlerHWND() != hWnd) ){
        //Create the IME manager
        m_imeHandler = std::make_unique<client::OsrImeHandlerWin>(hWnd);
        m_imeHandler->SetInputLanguage();
    }
    if (m_imeHandler) {
        m_imeHandler->CreateImeWindow();
        m_imeHandler->MoveImeWindow();
    }
}

void CefControlOffScreen::OnIMEComposition(UINT /*message*/, WPARAM /*wParam*/, LPARAM lParam)
{
    ASSERT(m_imeHandler != nullptr);
    CefRefPtr<CefBrowser> browser;
    if (m_pBrowserHandler != nullptr) {
        browser = m_pBrowserHandler->GetBrowser();
    }
if ((browser != nullptr) && (browser->GetHost() != nullptr) && (m_imeHandler != nullptr) ){
        CefString cTextStr;
if (m_imeHandler->GetResult(lParam, cTextStr) ){
            // Send the text to the browser. The |replacement_range| and
            // |relative_cursor_pos| params are not used on Windows, so provide
            // default invalid values.
            browser->GetHost()->ImeCommitText(cTextStr, CefRange((std::numeric_limits<uint32_t>::max)(), (std::numeric_limits<uint32_t>::max)()), 0);
            m_imeHandler->ResetComposition();
            // Continue reading the composition string - Japanese IMEs send both
            // GCS_RESULTSTR and GCS_COMPSTR.
        }

        std::vector<CefCompositionUnderline> underlines;
        int composition_start = 0;

        if (m_imeHandler->GetComposition(lParam, cTextStr, underlines,
            composition_start)) {
            // Send the composition string to the browser. The |replacement_range|
            // param is not used on Windows, so provide a default invalid value.
            browser->GetHost()->ImeSetComposition(
                cTextStr, underlines, CefRange((std::numeric_limits<uint32_t>::max)(), (std::numeric_limits<uint32_t>::max)()),
                CefRange(composition_start, static_cast<int>(composition_start + cTextStr.length())));   // TEST-C

            // Update the Candidate Window position. The cursor is at the end so
            // subtract 1. This is safe because IMM32 does not support non-zero-width
            // in a composition. Also,  negative values are safely ignored in
            // MoveImeWindow
            m_imeHandler->UpdateCaretPosition(composition_start - 1);
        }
        else {
            OnIMECancelCompositionEvent();
        }
    }
}

void CefControlOffScreen::OnIMECancelCompositionEvent()
{
    CefRefPtr<CefBrowser> browser;
    if (m_pBrowserHandler != nullptr) {
        browser = m_pBrowserHandler->GetBrowser();
    }
if ((browser != nullptr) && (browser->GetHost() != nullptr) ){
        browser->GetHost()->ImeCancelComposition();
    }
    if (m_imeHandler != nullptr) {
        m_imeHandler->ResetComposition();
        m_imeHandler->DestroyImeWindow();
    }
}


#endif //defined (DUI_BUILD_FOR_WIN)

} //namespace ui

#endif //DUI_BUILD_FOR_CEF
