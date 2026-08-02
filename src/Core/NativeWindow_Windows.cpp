#include "duilib/Core/NativeWindow_Windows.h"
#include "duilib/Utils/StringConvert.h"
#include "duilib/Utils/FileUtil.h"
#include "duilib/Core/GlobalManager.h"

#if defined (DUILIB_BUILD_FOR_WIN) && !defined (DUILIB_BUILD_FOR_SDL)

#include "duilib/Utils/ApiWrapper_Windows.h"
#include "duilib/Utils/InlineHook_Windows.h"
#include "duilib/Core/WindowDropTarget_Windows.h"
#include "duilib/Core/ControlDropTargetImpl_Windows.h"

#include <CommCtrl.h>
#include <Olectl.h>
#include <VersionHelpers.h>

namespace ui {

//Function to determine whether it is Windows 11
static bool UiIsWindows11OrGreater()
{
    OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0 };
    DWORDLONG const dwlConditionMask = VerSetConditionMask(
        VerSetConditionMask(
            VerSetConditionMask(
                0, VER_MAJORVERSION, VER_GREATER_EQUAL),
            VER_MINORVERSION, VER_GREATER_EQUAL),
        VER_BUILDNUMBER, VER_GREATER_EQUAL);

    osvi.dwMajorVersion = 10;
    osvi.dwMinorVersion = 0;
    osvi.dwBuildNumber = 22000; //Need to distinguish by the Build version number

    return ::VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER, dwlConditionMask) != FALSE;
}

//Timer ID for delayed display of the system menu
#define UI_SYS_MEMU_TIMER_ID 711

NativeWindow_Windows::NativeWindow_Windows(INativeWindow* pOwner):
    m_pOwner(pOwner),
    m_hWnd(nullptr),
    m_hParentWnd(nullptr),
    m_hDcPaint(nullptr),
    m_bIsLayeredWindow(false),
    m_nLayeredWindowAlpha(255),
    m_nLayeredWindowOpacity(255),
    m_bUseSystemCaption(false),
    m_bCloseing(false),
    m_closeParam(kWindowCloseNormal),
    m_bFakeModal(false),
    m_bFullscreen(false),
    m_bFullscreenExiting(false),
    m_dwLastStyle(0),
    m_ptLastMousePos(-1, -1),
    m_pfnOldWndProc(nullptr),
    m_bEnableDragDrop(true),
    m_bDoModal(false),
    m_bCloseByEsc(false),
    m_bCloseByEnter(false),
    m_bSnapLayoutMenu(false),
    m_bEnableSysMenu(true),
    m_bNCLButtonDownOnMaxButton(false),
    m_nSysMenuTimerId(0),
    m_hImc(nullptr),
    m_pWindowDropTarget(nullptr),
    m_nWindowDpiScaleFactor(100),
    m_bChildWindow(false),
    m_pDataObj(nullptr)
{
    ASSERT(m_pOwner != nullptr);
    m_rcLastWindowPlacement = { sizeof(WINDOWPLACEMENT), };

    //Windows 11 and newer versions support showing the snap layout menu (disabled by default; on the latest Win11, it triggers NC drawing, showing the content drawn by the system, which does not look good)
    /*if (UiIsWindows11OrGreater()) {
        m_bSnapLayoutMenu = true;
    }*/
}

NativeWindow_Windows::~NativeWindow_Windows()
{
    ASSERT(m_pfnOldWndProc == nullptr);
    ASSERT(m_hWnd == nullptr);
    ClearNativeWindow();
}

bool NativeWindow_Windows::CreateWnd(NativeWindow_Windows* pParentWindow,
                                     const WindowCreateParam& createParam,
                                     const WindowCreateAttributes& createAttributes)
{
    ASSERT(m_hWnd == nullptr);
    if (m_hWnd != nullptr) {
        return false;
    }
    ASSERT(!createParam.m_className.empty());
    if (createParam.m_className.empty()) {
        return false;
    }

    //Register the window class
    HMODULE hModule = GetResModuleHandle();
    DString className = StringConvert::TToLocal(createParam.m_className);
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = createParam.m_dwClassStyle;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.lpfnWndProc = NativeWindow_Windows::__WndProc;
    wc.hInstance = hModule;
    wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = className.c_str();
    wc.hIcon = nullptr;
    wc.hIconSm = nullptr;

    ATOM ret = ::RegisterClassEx(&wc);
    bool bRet = (ret != 0 || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
    ASSERT(bRet);
    if (!bRet) {
        return false;
    }

    //Unregister the ATOM when the module exits
    GlobalManager::Instance().AddAtExitFunction([className, hModule]() {
        ::UnregisterClassW(className.c_str(), hModule);
        });

    //Save the parameters
    m_createParam = createParam;

    //Set the default style
    if (m_createParam.m_dwStyle == 0) {
        m_createParam.m_dwStyle = kWS_OVERLAPPEDWINDOW;
    }

    //Synchronize the Window attributes in the XML file and carry these attributes when creating the window
    SyncCreateWindowAttributes(createAttributes);

    if (m_createParam.m_bCenterWindow) {
        //When centering the window, calculate the starting position of the window to avoid the window position changing when the window pops up
        int32_t xPos = 0;
        int32_t yPos = 0;
        HWND hCenterWindow = nullptr;
        if (pParentWindow != nullptr) {
            hCenterWindow = pParentWindow->GetHWND();
        }
        if (CalculateCenterWindowPos(hCenterWindow, xPos, yPos)) {
            m_createParam.m_nX = xPos;
            m_createParam.m_nY = yPos;
        }
    }

    //Parent window handle
    m_hParentWnd = pParentWindow != nullptr ? pParentWindow->GetHWND() : nullptr;

    //Window title
    DString windowTitle = StringConvert::TToLocal(m_createParam.m_windowTitle);
    HWND hWnd = ::CreateWindowEx(m_createParam.m_dwExStyle,
                                 className.c_str(),
                                 windowTitle.c_str(),
                                 m_createParam.m_dwStyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                                 m_createParam.m_nX, m_createParam.m_nY, m_createParam.m_nWidth, m_createParam.m_nHeight,
                                 m_hParentWnd, nullptr, GetResModuleHandle(), this);
    ASSERT(::IsWindow(hWnd));
    ASSERT(hWnd == m_hWnd);
    if (hWnd != m_hWnd) {
        m_hWnd = hWnd;
    }
    ASSERT(m_hWnd != nullptr);
    if (m_hWnd == nullptr) {
        m_hParentWnd = nullptr;
        return false;
    }
    if (IsLayeredWindow() && IsWindowVisible()) {
        //For layered windows, drawing must be triggered manually; otherwise the window may not draw after creation
        UiRect rcClient;
        GetClientRect(rcClient);
        Invalidate(rcClient);
    }
    return (m_hWnd != nullptr);
}

//Allow text input in the DoModal interface
#ifdef DUILIB_ENABLE_INLINE_HOOK

/** The singleton object of the Hook function
*/
class HookIsDialogMessage: public InlineHook
{
public:
    HookIsDialogMessage() = default;
    ~HookIsDialogMessage() = default;
    HookIsDialogMessage(const HookIsDialogMessage&) = delete;
    HookIsDialogMessage& operator = (const HookIsDialogMessage&) = delete;


    static HookIsDialogMessage& Instance()
    {
        static HookIsDialogMessage self;
        return self;
    }
};


/** The type of the target function
*/
typedef BOOL(WINAPI* PfnIsDialogMessage)(_In_ HWND hDlg, _In_ LPMSG lpMsg);

/** The replaced function
*/
static BOOL WINAPI IsDialogMessageDuiLib(_In_ HWND hDlg, _In_ LPMSG lpMsg)
{
    // Call the original function (via the trampoline)
    BOOL bRet = FALSE;
    if ((lpMsg != nullptr) && (lpMsg->message == WM_CHAR)) {
        //Do not treat WM_CHAR as a dialog message
        return bRet;
    }
    auto original = HookIsDialogMessage::Instance().GetTrampoline<PfnIsDialogMessage>();
    if (original) {
#if defined (_MSC_VER)
        __try {
            bRet = original(hDlg, lpMsg);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            bRet = false;
        }
#else
        bRet = original(hDlg, lpMsg);
#endif
    }
    return bRet;
}

#endif //DUILIB_ENABLE_INLINE_HOOK

int32_t NativeWindow_Windows::DoModal(NativeWindow_Windows* pParentWindow,
                                      const WindowCreateParam& createParam,
                                      const WindowCreateAttributes& createAttributes,
                                      bool bCloseByEsc, bool bCloseByEnter)
{
    ASSERT(m_hWnd == nullptr);
    if (m_hWnd != nullptr) {
        return -1;
    }

    //Save the parameters
    m_createParam = createParam;
    m_bCloseByEsc = bCloseByEsc;
    m_bCloseByEnter = bCloseByEnter;

    //Set the default style
    if (m_createParam.m_dwStyle == 0) {
        m_createParam.m_dwStyle = kWS_POPUPWINDOW;
    }

    //Synchronize the Window attributes in the XML file and carry these attributes when creating the window
    SyncCreateWindowAttributes(createAttributes);

    if (m_createParam.m_bCenterWindow) {
        //When centering the window, calculate the starting position of the window to avoid the window position changing when the window pops up
        int32_t xPos = 0;
        int32_t yPos = 0;
        HWND hCenterWindow = nullptr;
        if (pParentWindow != nullptr) {
            hCenterWindow = pParentWindow->GetHWND();
        }
        if (CalculateCenterWindowPos(hCenterWindow, xPos, yPos)) {
            m_createParam.m_nX = xPos;
            m_createParam.m_nY = yPos;
        }
    }

    //The position and size of the window
    short x = 0;
    short y = 0;
    short cx = 0;
    short cy = 0;

    if (m_createParam.m_nX != kCW_USEDEFAULT) {
        x = (short)m_createParam.m_nX;
    }
    if (m_createParam.m_nY != kCW_USEDEFAULT) {
        y = (short)m_createParam.m_nY;
    }

    // Create the dialog resource structure (the dialog initial state is visible)
    DLGTEMPLATE dlgTemplate = {
        WS_VISIBLE | m_createParam.m_dwStyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        m_createParam.m_dwExStyle,
        0,
        x, y, cx, cy
    };

    // Add the dialog buttons to the resource
    constexpr const size_t nDataSize = sizeof(DLGTEMPLATE) + 32;
    HGLOBAL hResource = ::GlobalAlloc(GMEM_MOVEABLE, nDataSize);
    if (hResource == nullptr) {
        return -1;
    }
    LPDLGTEMPLATE lpDialogTemplate = (LPDLGTEMPLATE)::GlobalLock(hResource);
    if (lpDialogTemplate == nullptr) {
        ::GlobalFree(hResource);
        return -1;
    }
    ::memset(lpDialogTemplate, 0, nDataSize);
    *lpDialogTemplate = dlgTemplate;

    // Show the dialog
    HWND hParentWnd = nullptr;
    if (pParentWindow != nullptr) {
        hParentWnd = pParentWindow->GetHWND();
    }
    else {
        hParentWnd = ::GetActiveWindow();
    }
    //Mark as the modal dialog state
    m_bDoModal = true;

#ifdef DUILIB_ENABLE_INLINE_HOOK
    //Handle IsDialogMessage to support text input in RichEdit controls
    {
        FARPROC targetFunc = nullptr;
        HMODULE hModule = ::GetModuleHandle(_T("User32.dll"));
        if (hModule != nullptr) {
#if defined(UNICODE) || defined(_UNICODE)
            targetFunc = ::GetProcAddress(hModule, "IsDialogMessageW");
#else
            targetFunc = ::GetProcAddress(hModule, "IsDialogMessageA");
#endif
        }
        if (targetFunc != nullptr) {
            HookIsDialogMessage::Instance().Install((void*)targetFunc, (void*)IsDialogMessageDuiLib);
        }
        HookIsDialogMessage::Instance().Install((void*)::IsDialogMessage, (void*)IsDialogMessageDuiLib);
    }
#endif //DUILIB_ENABLE_INLINE_HOOK

    //Show the modal dialog
    INT_PTR nRet = ::DialogBoxIndirectParam(GetResModuleHandle(), (LPDLGTEMPLATE)lpDialogTemplate, hParentWnd, NativeWindow_Windows::__DialogProc, (LPARAM)this);
    // Clean up resources
    ::GlobalUnlock(hResource);
    ::GlobalFree(hResource);
    if (nRet != -1) {
        nRet = m_closeParam;
    }

#ifdef DUILIB_ENABLE_INLINE_HOOK
    HookIsDialogMessage::Instance().Uninstall();
#endif

    return (int32_t)nRet;
}

bool NativeWindow_Windows::CreateChildWnd(NativeWindow_Windows* pParentWindow, int32_t nX, int32_t nY, int32_t nWidth, int32_t nHeight)
{
    ASSERT(m_hWnd == nullptr);
    if (m_hWnd != nullptr) {
        return false;
    }
    ASSERT(pParentWindow != nullptr);
    if (pParentWindow == nullptr) {
        return false;
    }

    //Register the window class
    HMODULE hModule = GetResModuleHandle();
    DString className = StringConvert::TToLocal(pParentWindow->m_createParam.m_className);
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = pParentWindow->m_createParam.m_dwClassStyle;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.lpfnWndProc = NativeWindow_Windows::__WndProc;
    wc.hInstance = hModule;
    wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = className.c_str();
    wc.hIcon = nullptr;
    wc.hIconSm = nullptr;

    ATOM ret = ::RegisterClassEx(&wc);
    bool bRet = (ret != 0 || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
    ASSERT(bRet);
    if (!bRet) {
        return false;
    }

    //Unregister the ATOM when the module exits
    GlobalManager::Instance().AddAtExitFunction([className, hModule]() {
        ::UnregisterClassW(className.c_str(), hModule);
        });

    //Save the parameters
    m_createParam.m_className = pParentWindow->m_createParam.m_className;
    m_createParam.m_dwClassStyle = pParentWindow->m_createParam.m_dwClassStyle;

    m_createParam.m_nX = nX;
    m_createParam.m_nY = nY;
    m_createParam.m_nWidth = nWidth;
    m_createParam.m_nHeight = nHeight;
    m_createParam.m_dwExStyle = 0;
    m_createParam.m_dwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    if (pParentWindow->IsLayeredWindow()) {
        //When the parent window is a layered window, the child window also needs the layered window attribute, otherwise the drawing flow will have problems
        if (::IsWindows8OrGreater()) {
            // Windows 8: top-level windows and child windows support the WS_EX_LAYERED style.
            // Previous Windows versions only support WS_EX_LAYERED for top-level windows.
            m_createParam.m_dwExStyle = WS_EX_LAYERED;
        }
    }

    //Parent window handle
    m_hParentWnd = pParentWindow->GetHWND();
    m_bChildWindow = true;

    //Window title
    DString windowTitle = StringConvert::TToLocal(m_createParam.m_windowTitle);
    HWND hWnd = ::CreateWindowEx(m_createParam.m_dwExStyle,
                                 className.c_str(),
                                 windowTitle.c_str(),
                                 m_createParam.m_dwStyle,
                                 m_createParam.m_nX, m_createParam.m_nY, m_createParam.m_nWidth, m_createParam.m_nHeight,
                                 m_hParentWnd, nullptr, GetResModuleHandle(), this);
    ASSERT(::IsWindow(hWnd));
    ASSERT(hWnd == m_hWnd);
    if (hWnd != m_hWnd) {
        m_hWnd = hWnd;
    }
    ASSERT(m_hWnd != nullptr);
    if (m_hWnd == nullptr) {
        m_hParentWnd = nullptr;
        return false;
    }    
    if (IsLayeredWindow() && IsWindowVisible()) {
        //For layered windows, drawing must be triggered manually; otherwise the window may not draw after creation
        UiRect rcClient;
        GetClientRect(rcClient);
        Invalidate(rcClient);
    }    
    return true;
}

bool NativeWindow_Windows::IsChildWindow() const
{
    return m_bChildWindow;
}

bool NativeWindow_Windows::SetParentWindow(NativeWindow_Windows* pParentWindow)
{
    ASSERT((pParentWindow != nullptr) && pParentWindow->IsWindow());
    if ((pParentWindow == nullptr) || !pParentWindow->IsWindow()) {
        return false;
    }
    if (!IsWindow()) {
        return false;
    }
    ::SetParent(GetHWND(), pParentWindow->GetHWND());
    return true;
}

void NativeWindow_Windows::SyncCreateWindowAttributes(const WindowCreateAttributes& createAttributes)
{
    m_bUseSystemCaption = false;
    if (createAttributes.m_bUseSystemCaptionDefined && createAttributes.m_bUseSystemCaption) {
        //Use the system title bar
        if (m_createParam.m_dwStyle & WS_POPUP) {
            //Popup window
            m_createParam.m_dwStyle |= (WS_CAPTION | WS_SYSMENU);
        }
        else {
            m_createParam.m_dwStyle |= (WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
        }
        m_bUseSystemCaption = true;
    }

    //Initialize the layered window attributes
    m_bIsLayeredWindow = false;
    if (createAttributes.m_bIsLayeredWindowDefined) {
        if (createAttributes.m_bIsLayeredWindow) {
            m_bIsLayeredWindow = true;
            m_createParam.m_dwExStyle |= WS_EX_LAYERED;
        }
        else {
            m_createParam.m_dwExStyle &= ~WS_EX_LAYERED;
        }
    }
    else if (m_createParam.m_dwExStyle & WS_EX_LAYERED) {
        m_bIsLayeredWindow = true;
    }

    //If the system title bar is used, disable the layered window
    if (createAttributes.m_bUseSystemCaptionDefined && createAttributes.m_bUseSystemCaption) {
        m_bIsLayeredWindow = false;
        m_createParam.m_dwExStyle &= ~WS_EX_LAYERED;
    }

    //If opacity is set, set it as a layered window
    if (createAttributes.m_bLayeredWindowOpacityDefined && (createAttributes.m_nLayeredWindowOpacity != 255)) {
        m_createParam.m_dwExStyle |= WS_EX_LAYERED;
        m_bIsLayeredWindow = true;
    }

    if (createAttributes.m_bInitSizeDefined) {
        if (createAttributes.m_szInitSize.cx > 0) {
            m_createParam.m_nWidth = createAttributes.m_szInitSize.cx;
        }
        if (createAttributes.m_szInitSize.cy > 0) {
            m_createParam.m_nHeight = createAttributes.m_szInitSize.cy;
        }
    }
}

LRESULT NativeWindow_Windows::OnCreateMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    bHandled = false;
    //Initialize
    InitNativeWindow();

    if (m_pOwner != nullptr) {
        std::weak_ptr<WeakFlag> windowFlag = m_pOwner->GetWeakFlag();
        m_pOwner->OnNativeCreateWndMsg(false, NativeMsg(uMsg, wParam, lParam), bHandled);
        if (windowFlag.expired()) {
            return 0;
        }
    }

    //Update the style of the maximize/minimize buttons
    UpdateMinMaxBoxStyle();

    if (m_createParam.m_bCenterWindow) {
        //Center the window after creation (the initial position set for the popup window does not take effect; it needs to be set after the window is created, without affecting the effect)
        CenterWindow();
    }
    return 0;
}

LRESULT NativeWindow_Windows::OnInitDialogMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    //Initialize
    InitNativeWindow();

    if (m_pOwner != nullptr) {
        std::weak_ptr<WeakFlag> windowFlag = m_pOwner->GetWeakFlag();
        m_pOwner->OnNativeCreateWndMsg(true, NativeMsg(uMsg, wParam, lParam), bHandled);
        if (windowFlag.expired()) {
            return 0;
        }
    }

    //Update the style of the maximize/minimize buttons
    UpdateMinMaxBoxStyle();

    if (m_createParam.m_bCenterWindow) {
        //Center the window after creation (the initial position set for the popup window does not take effect; it needs to be set after the window is created, without affecting the effect)
        CenterWindow();
    }

    //Mark as already handled; no longer call the window default processing function
    bHandled = true;
    return TRUE;
}

void NativeWindow_Windows::InitNativeWindow()
{
    HWND hWnd = m_hWnd;
    if (!::IsWindow(hWnd)) {
        return;
    }

    //Check and update the layered window attribute
    m_bIsLayeredWindow = false;
    if (m_createParam.m_dwExStyle & WS_EX_LAYERED) {
        m_bIsLayeredWindow = true;
    }
    bool bChanged = false;
    SetLayeredWindowStyle(m_bIsLayeredWindow, bChanged);

    //Initialize the window-related DC
    ASSERT(m_hDcPaint == nullptr);
    m_hDcPaint = ::GetDC(hWnd);

    //Register to accept Touch messages
    RegisterTouchWindowWrapper(hWnd, 0);

    if (!m_createParam.m_windowTitle.empty()) {
        DString windowTitle = StringConvert::TToLocal(m_createParam.m_windowTitle);
        ::SetWindowText(hWnd, windowTitle.c_str());
    }

    //Associate the drag-and-drop operation
    SetEnableDragDrop(IsEnableDragDrop());

    //Record the DPI scaling ratio of the window
    uint32_t uDPI = GetDpiForWnd(hWnd);
    if (uDPI != 0) {
        m_nWindowDpiScaleFactor = (uint32_t)::MulDiv((int32_t)uDPI, 100, 96);
    }
    else {
        m_nWindowDpiScaleFactor = 0;
    }
}

void NativeWindow_Windows::ClearNativeWindow()
{
    //Unregister the touch messages
    HWND hWnd = GetHWND();
    if (hWnd != nullptr) {
        UnregisterTouchWindowWrapper(hWnd);
    }
    //Unregister the hotkeys
    std::vector<int32_t> hotKeyIds = m_hotKeyIds;
    for (int32_t id : hotKeyIds) {
        UnregisterHotKey(id);
    }
    if (m_hDcPaint != nullptr) {
        ::ReleaseDC(m_hWnd, m_hDcPaint);
        m_hDcPaint = nullptr;
    }
    if (m_hImc != nullptr) {
        ::ImmAssociateContext(m_hWnd, m_hImc);
        m_hImc = nullptr;
    }
    if (m_pWindowDropTarget != nullptr) {
        m_pWindowDropTarget->UnregisterDragDrop();
        m_pWindowDropTarget->Release();
        m_pWindowDropTarget = nullptr;
    }
    m_hWnd = nullptr;
}

HWND NativeWindow_Windows::GetHWND() const
{
    return m_hWnd;
}

void* NativeWindow_Windows::GetWindowHandle() const
{
    return m_hWnd;
}

bool NativeWindow_Windows::IsWindow() const
{
    return (m_hWnd != nullptr) && ::IsWindow(m_hWnd);
}

HMODULE NativeWindow_Windows::GetResModuleHandle() const
{
    HMODULE hResModule = (HMODULE)GlobalManager::Instance().GetPlatformData();
    if (hResModule == nullptr) {
        hResModule = ::GetModuleHandle(nullptr);
    }
    return hResModule;
}

HDC NativeWindow_Windows::GetPaintDC() const
{
    return m_hDcPaint;
}

void NativeWindow_Windows::CloseWnd(int32_t nRet)
{
    StopSysMenuTimer();
    m_bCloseing = true;
    ASSERT(::IsWindow(m_hWnd));
    if (!::IsWindow(m_hWnd)) {
        return;
    }
    ::PostMessage(m_hWnd, WM_CLOSE, (WPARAM)nRet, 0L);
}

void NativeWindow_Windows::Close()
{
    StopSysMenuTimer();
    m_bCloseing = true;
    ASSERT(::IsWindow(m_hWnd));
    if (!::IsWindow(m_hWnd)) {
        return;
    }
    ::SendMessage(m_hWnd, WM_CLOSE, 0L, 0L);
}

bool NativeWindow_Windows::IsClosingWnd() const
{
    return m_bCloseing;
}

int32_t NativeWindow_Windows::GetCloseParam() const
{
    return m_closeParam;
}

bool NativeWindow_Windows::SetLayeredWindow(bool bIsLayeredWindow, bool bRedraw)
{
    m_bIsLayeredWindow = bIsLayeredWindow;
    bool bChanged = false;
    SetLayeredWindowStyle(bIsLayeredWindow, bChanged);
    if (bRedraw && bChanged && IsWindow()) {
        // Force the window to repaint
        ::RedrawWindow(m_hWnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
        ::SetWindowPos(m_hWnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }
    return true;
}

bool NativeWindow_Windows::SetLayeredWindowStyle(bool bIsLayeredWindow, bool& bChanged) const
{
    bChanged = false;
    if (::IsWindow(m_hWnd)) {
        LONG dwExStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
        LONG dwOldExStyle = dwExStyle;
        if (bIsLayeredWindow) {
            dwExStyle |= WS_EX_LAYERED;
        }
        else {
            dwExStyle &= ~WS_EX_LAYERED;
        }
        if (dwOldExStyle != dwExStyle) {
            bChanged = true;
            ::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle);
            dwExStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
        }
        if (bIsLayeredWindow) {
            return (dwExStyle & WS_EX_LAYERED) ? true : false;
        }
        else {
            return (dwExStyle & WS_EX_LAYERED) ? false : true;
        }
    }
    return false;
}

bool NativeWindow_Windows::IsLayeredWindow() const
{
#if _DEBUG
    if (::IsWindow(m_hWnd)) {
        LONG dwExStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
        bool bIsLayeredWindow = (dwExStyle & WS_EX_LAYERED) ? true : false;
        ASSERT(bIsLayeredWindow == m_bIsLayeredWindow);
    }
#endif // _DEBUG
    return m_bIsLayeredWindow;
}

void NativeWindow_Windows::UpdateMinMaxBoxStyle() const
{
    //Update the style of the maximize/minimize buttons
    bool bMinimizeBox = false;
    bool bMaximizeBox = false;
    if (!IsUseSystemCaption() && (m_pOwner != nullptr) && m_pOwner->OnNativeHasMinMaxBox(bMinimizeBox, bMaximizeBox)) {
        UINT oldStyleValue = (UINT)::GetWindowLong(GetHWND(), GWL_STYLE);
        UINT newStyleValue = oldStyleValue;
        if (bMinimizeBox) {
            newStyleValue |= WS_MINIMIZEBOX;
        }
        else {
            newStyleValue &= ~WS_MINIMIZEBOX;
        }
        if (bMaximizeBox) {
            newStyleValue |= WS_MAXIMIZEBOX;
        }
        else {
            newStyleValue &= ~WS_MAXIMIZEBOX;
        }
        if (newStyleValue != oldStyleValue) {
            ::SetWindowLong(GetHWND(), GWL_STYLE, newStyleValue);
        }
    }
}

void NativeWindow_Windows::SetLayeredWindowAlpha(int32_t nAlpha)
{
    ASSERT(nAlpha >= 0 && nAlpha <= 255);
    if ((nAlpha < 0) || (nAlpha > 255)) {
        return;
    }
    m_nLayeredWindowAlpha = static_cast<uint8_t>(nAlpha);
}

uint8_t NativeWindow_Windows::GetLayeredWindowAlpha() const
{
    return m_nLayeredWindowAlpha;
}

void NativeWindow_Windows::SetLayeredWindowOpacity(int32_t nAlpha)
{
    ASSERT(nAlpha >= 0 && nAlpha <= 255);
    if ((nAlpha < 0) || (nAlpha > 255)) {
        return;
    }
    m_nLayeredWindowOpacity = static_cast<uint8_t>(nAlpha);
    if (m_nLayeredWindowOpacity == 255) {
        COLORREF crKey = 0;
        BYTE bAlpha = 0;
        DWORD dwFlags = LWA_ALPHA | LWA_COLORKEY;
        bool bAttributes = ::GetLayeredWindowAttributes(m_hWnd, &crKey, &bAlpha, &dwFlags) != FALSE;
        if (bAttributes) {
            bool bRet = ::SetLayeredWindowAttributes(m_hWnd, 0, m_nLayeredWindowOpacity, LWA_ALPHA) != FALSE;
            ASSERT_UNUSED_VARIABLE(bRet);
        }
    }
    else {
        //It must be set as a layered window first, then the setting can succeed
        SetLayeredWindow(true, false);
        bool bRet = ::SetLayeredWindowAttributes(m_hWnd, 0, m_nLayeredWindowOpacity, LWA_ALPHA) != FALSE;
        ASSERT_UNUSED_VARIABLE(bRet);
    }
}

uint8_t NativeWindow_Windows::GetLayeredWindowOpacity() const
{
    return m_nLayeredWindowOpacity;
}

void NativeWindow_Windows::SetUseSystemCaption(bool bUseSystemCaption)
{
    ASSERT(!IsChildWindow());
    if (IsChildWindow()) {
        return;
    }
    m_bUseSystemCaption = bUseSystemCaption;
    if (IsUseSystemCaption()) {
        //Use the system default title bar; the title bar style needs to be added
        bool bChanged = false;
        if (IsWindow()) {
            UINT oldStyleValue = (UINT)::GetWindowLong(GetHWND(), GWL_STYLE);
            UINT newStyleValue = oldStyleValue;
            if (oldStyleValue & WS_POPUP) {
                //Popup window
                newStyleValue |= (WS_CAPTION | WS_SYSMENU);
            }
            else {
                newStyleValue |= (WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);                
            }
            if (newStyleValue != oldStyleValue) {
                ::SetWindowLong(GetHWND(), GWL_STYLE, newStyleValue);
                bChanged = true; 
            }
        }
        //Disable the layered window
        if (IsLayeredWindow()) {
            bChanged = true;
            SetLayeredWindow(false, false);
        }
        if (bChanged) {
            // Force the window to repaint
            ::SetWindowPos(GetHWND(), nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
            //Reactivate the non-client area drawing of the window
            if (IsWindowForeground()) {
                KeepParentActive();
            }            
        }        
    }
    m_pOwner->OnNativeUseSystemCaptionBarChanged();
}

bool NativeWindow_Windows::IsUseSystemCaption() const
{
    return m_bUseSystemCaption;
}

bool NativeWindow_Windows::ShowWindow(ShowWindowCommands nCmdShow)
{
    ASSERT(::IsWindow(m_hWnd));
    if (!::IsWindow(m_hWnd)) {
        return false;
    }
    if (m_bFullscreen) {
        //Exit fullscreen first
        ExitFullscreen();
    }
    if (IsChildWindow()) {
        //Child window: only show and hide are supported
        if (nCmdShow != kSW_HIDE) {
            nCmdShow = kSW_SHOW;
        }
    }
    bool bRet = false;
    int nWindowCmdShow = SW_SHOWNORMAL;
    switch(nCmdShow)
    {
    case kSW_HIDE:
        nWindowCmdShow = SW_HIDE;
        break;
    case kSW_SHOW_NORMAL:
        nWindowCmdShow = SW_SHOWNORMAL;
        break;
    case kSW_SHOW_MINIMIZED:
        nWindowCmdShow = SW_SHOWMINIMIZED;
        break;
    case kSW_SHOW_MAXIMIZED:
        nWindowCmdShow = SW_SHOWMAXIMIZED;
        break;
    case kSW_SHOW_NOACTIVATE:
        nWindowCmdShow = SW_SHOWNOACTIVATE;
        break;
    case kSW_SHOW:
        nWindowCmdShow = SW_SHOW;
        break;
    case kSW_MINIMIZE:
        nWindowCmdShow = SW_MINIMIZE;
        break;
    case kSW_SHOW_MIN_NOACTIVE:
        nWindowCmdShow = SW_SHOWMINNOACTIVE;
        break;
    case kSW_SHOW_NA:
        nWindowCmdShow = SW_SHOWNA;
        break;
    case kSW_RESTORE:
        nWindowCmdShow = SW_RESTORE;
        break;
    default:
        ASSERT(false);
        break;
    }
    bRet = ::ShowWindow(m_hWnd, nWindowCmdShow) != FALSE;
    if (IsLayeredWindow() && IsWindowVisible()) {
        //For layered windows, drawing must be triggered manually; otherwise the window may not draw after creation
        UiRect rcClient;
        GetClientRect(rcClient);
        Invalidate(rcClient);
    }
    return bRet;
}

void NativeWindow_Windows::ShowModalFake(NativeWindow_Windows* pParentWindow)
{
    ASSERT(::IsWindow(m_hWnd));
    ASSERT(!IsChildWindow());
    if (IsChildWindow()) {
        return;
    }
    ASSERT((pParentWindow != nullptr) && (pParentWindow->GetHWND() != nullptr));
    if (pParentWindow != nullptr) {
        auto hOwnerWnd = GetWindowOwner();
        ASSERT(::IsWindow(hOwnerWnd));
        ASSERT_UNUSED_VARIABLE(hOwnerWnd == pParentWindow->GetHWND());
        if (pParentWindow != nullptr) {
            pParentWindow->EnableWindow(false);
        }
    }
    ShowWindow(kSW_SHOW_NORMAL);
    m_bFakeModal = true;
}

void NativeWindow_Windows::OnCloseModalFake(NativeWindow_Windows* pParentWindow)
{
    if (IsFakeModal()) {
        if (pParentWindow != nullptr) {
            pParentWindow->EnableWindow(true);
            pParentWindow->SetWindowFocus();
        }
        m_bFakeModal = false;
    }
    if (IsWindowFocused()) {
        SetOwnerWindowFocus();
    }
}

bool NativeWindow_Windows::IsFakeModal() const
{
    return m_bFakeModal;
}

bool NativeWindow_Windows::IsDoModal() const
{
    return m_bDoModal;
}

void NativeWindow_Windows::CenterWindow()
{
    ASSERT(IsWindow());
    if (!IsWindow()) {
        return;
    }
    ASSERT((::GetWindowLong(m_hWnd, GWL_STYLE) & WS_CHILD) == 0);
    HWND hCenterWindow = ::GetParent(m_hWnd);
    if (hCenterWindow == nullptr) {
        if ((m_hParentWnd != nullptr) && ::IsWindow(m_hParentWnd)) {
            hCenterWindow = m_hParentWnd;
        }
    }
    int32_t xPos = 0;
    int32_t yPos = 0;
    if (CalculateCenterWindowPos(hCenterWindow, xPos, yPos)) {
        ::SetWindowPos(m_hWnd, nullptr, xPos, yPos, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

bool NativeWindow_Windows::CalculateCenterWindowPos(HWND hCenterWindow, int32_t& xPos, int32_t& yPos) const
{
    if (!::IsWindow(hCenterWindow)) {
        hCenterWindow = nullptr;
    }
    //The width and height of the current window
    int32_t nWindowWidth = 0;
    int32_t nWindowHeight = 0;
    if (IsWindow()) {
        UiRect rcDlg;
        GetWindowRect(rcDlg);
        nWindowWidth = rcDlg.Width();
        nWindowHeight = rcDlg.Height();
    }
    else {
        if ((m_createParam.m_nWidth <= 0) || (m_createParam.m_nHeight <= 0)) {
            //The current window width is unknown, so it cannot be calculated
            return false;
        }
        nWindowWidth = m_createParam.m_nWidth;
        nWindowHeight = m_createParam.m_nHeight;
    }

    UiRect rcArea;
    UiRect rcCenter;
    UiRect rcMonitor;
    GetMonitorRect(hCenterWindow != nullptr ? hCenterWindow : GetHWND(), rcMonitor, rcArea);
    if (hCenterWindow == nullptr) {
        rcCenter = rcArea;
    }
    else if (::IsIconic(hCenterWindow)) {
        rcCenter = rcArea;
    }
    else {
        GetWindowRect(hCenterWindow, rcCenter);
    }

    //Leave a gap at the edge of the screen to avoid window snapping
    UINT dpi = 96;
    GetDpiForWindowWrapper(hCenterWindow != nullptr ? hCenterWindow : GetHWND(), dpi);
    const int32_t snapThreshold = MulDiv(3, dpi, 96);

    // Find dialog's upper left based on rcCenter
    int32_t xLeft = rcCenter.CenterX() - nWindowWidth / 2;
    int32_t yTop = rcCenter.CenterY() - nWindowHeight / 2;

    // The dialog is outside the screen, move it inside
    if (xLeft < rcArea.left) {
        xLeft = rcArea.left + snapThreshold;
    }
    else if (xLeft + nWindowWidth > rcArea.right) {
        xLeft = rcArea.right - nWindowWidth - snapThreshold;
    }
    if (yTop < rcArea.top) {
        yTop = rcArea.top + snapThreshold;
    }
    else if (yTop + nWindowHeight > rcArea.bottom) {
        yTop = rcArea.bottom - nWindowHeight - snapThreshold;
    }
    xPos = xLeft;
    yPos = yTop;
    return true;
}

void NativeWindow_Windows::SetWindowAlwaysOnTop(bool bOnTop)
{
    ASSERT(IsWindow() && !IsChildWindow());
    if (!IsWindow() || IsChildWindow()) {
        return;
    }
    if (bOnTop) {
        ::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
    else {
        ::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
}

bool NativeWindow_Windows::IsWindowAlwaysOnTop() const
{
    ASSERT(IsWindow());
    if (!IsWindow()) {
        return false;
    }
    LONG dwExStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
    return (dwExStyle & WS_EX_TOPMOST) ? true : false;
}

bool NativeWindow_Windows::SetWindowForeground()
{
    ASSERT(::IsWindow(m_hWnd));
    if (::GetForegroundWindow() != m_hWnd) {
        ::SetForegroundWindow(m_hWnd);
    }
    return ::GetForegroundWindow() == m_hWnd;
}

bool NativeWindow_Windows::IsWindowForeground() const
{
    return ::IsWindow(m_hWnd) && (m_hWnd == ::GetForegroundWindow());
}

bool NativeWindow_Windows::SetWindowFocus()
{
    ASSERT(::IsWindow(m_hWnd));
    if (::GetFocus() != m_hWnd) {
        ::SetFocus(m_hWnd);
    }
    return ::GetFocus() == m_hWnd;
}

bool NativeWindow_Windows::KillWindowFocus()
{
    ASSERT(::IsWindow(m_hWnd));
    if (::GetFocus() == m_hWnd) {
        ::SetFocus(nullptr);
    }
    return ::GetFocus() != m_hWnd;
}

bool NativeWindow_Windows::IsWindowFocused() const
{
    return ::IsWindow(m_hWnd) && (m_hWnd == ::GetFocus());
}

bool NativeWindow_Windows::SetOwnerWindowFocus()
{
    HWND hwndParent = GetWindowOwner();
    if (hwndParent != nullptr) {
        ::SetFocus(hwndParent);
        return ::GetFocus() == hwndParent;
    }
    return false;
}

void NativeWindow_Windows::CheckSetWindowFocus()
{
    if (::GetFocus() != m_hWnd) {
        ::SetFocus(m_hWnd);
    }
}

LRESULT NativeWindow_Windows::PostMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ASSERT(::IsWindow(m_hWnd));
    return ::PostMessage(m_hWnd, uMsg, wParam, lParam);
}

void NativeWindow_Windows::PostQuitMsg(int32_t nExitCode)
{
    ::PostQuitMessage(nExitCode);
}

bool NativeWindow_Windows::EnterFullscreen()
{
    ASSERT(::IsWindow(m_hWnd));
    if (!::IsWindow(m_hWnd)) {
        return false;
    }
    if (IsWindowMinimized() || IsChildWindow()) {
        //Minimized windows and child windows are not allowed to activate fullscreen
        return false;
    }
    if (m_bFullscreen) {
        return true;
    }
    m_bFullscreen = true;
    m_bFullscreenExiting = false;

    //Save the window style
    m_dwLastStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);

    //Save the window size and position information
    m_rcLastWindowPlacement.length = sizeof(WINDOWPLACEMENT);
    ::GetWindowPlacement(m_hWnd, &m_rcLastWindowPlacement);

    UiRect rcMonitor;
    GetMonitorRect(rcMonitor);

    // Remove the title bar and border
    DWORD dwFullscreenStyle = (m_dwLastStyle | WS_VISIBLE | WS_POPUP | WS_MAXIMIZE) & ~WS_CAPTION & ~WS_BORDER & ~WS_THICKFRAME & ~WS_DLGFRAME;
    ::SetWindowLongPtr(m_hWnd, GWL_STYLE, dwFullscreenStyle);
    ::SetWindowPos(m_hWnd, nullptr, rcMonitor.left, rcMonitor.top, rcMonitor.Width(), rcMonitor.Height(), SWP_FRAMECHANGED); // Set the position and size
    
    m_pOwner->OnNativeWindowEnterFullscreen();
    return true;
}

bool NativeWindow_Windows::ExitFullscreen()
{
    ASSERT(::IsWindow(m_hWnd));
    if (!::IsWindow(m_hWnd)) {
        return false;
    }
    if (!m_bFullscreen) {
        return false;
    }
    if (m_bFullscreenExiting) {
        return false;
    }
    m_bFullscreenExiting = true; //Avoid repeatedly entering the exit process
    
    //Restore the window style
    if (m_dwLastStyle != 0) {
        ::SetWindowLong(m_hWnd, GWL_STYLE, m_dwLastStyle);
        m_dwLastStyle = 0;
    }

    //Restore the window position/size information
    ::SetWindowPlacement(m_hWnd, &m_rcLastWindowPlacement);

    m_bFullscreen = false;
    m_bFullscreenExiting = false;

    //Trigger the position and size change event
    ::SetWindowPos(m_hWnd, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);

    m_pOwner->OnNativeWindowExitFullscreen();
    return true;
}

bool NativeWindow_Windows::IsWindowMaximized() const
{
    return ::IsWindow(m_hWnd) && ::IsZoomed(m_hWnd);
}

bool NativeWindow_Windows::IsWindowMinimized() const
{
    return ::IsWindow(m_hWnd) && ::IsIconic(m_hWnd);
}

bool NativeWindow_Windows::IsWindowFullscreen() const
{
    return m_bFullscreen;
}

bool NativeWindow_Windows::EnableWindow(bool bEnable)
{
    return ::EnableWindow(m_hWnd, bEnable ? TRUE : false) != FALSE;
}

bool NativeWindow_Windows::IsWindowEnabled() const
{
    return ::IsWindow(m_hWnd) && ::IsWindowEnabled(m_hWnd) != FALSE;
}

bool NativeWindow_Windows::IsWindowVisible() const
{
    return ::IsWindow(m_hWnd) && ::IsWindowVisible(m_hWnd) != FALSE;
}

bool NativeWindow_Windows::SetWindowPos(const NativeWindow_Windows* pInsertAfterWindow,
                                        InsertAfterFlag insertAfterFlag,
                                        int32_t X, int32_t Y, int32_t cx, int32_t cy,
                                        uint32_t uFlags)
{
    ASSERT(::IsWindow(m_hWnd));
    HWND hWndInsertAfter = HWND_TOP;
    if (!(uFlags & kSWP_NOZORDER)) {
        if (pInsertAfterWindow != nullptr) {
            hWndInsertAfter = pInsertAfterWindow->GetHWND();
        }
        else {
            hWndInsertAfter = (HWND)insertAfterFlag;
        }
    }
    bool bOldVisible = ::IsWindowVisible(m_hWnd);
    bool bRet = ::SetWindowPos(m_hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags) != FALSE;
    if ((kSWP_SHOWWINDOW & uFlags) && !bOldVisible && IsLayeredWindow()) {
        //For layered windows, drawing must be triggered manually; otherwise the window may not draw after creation
        UiRect rcClient;
        GetClientRect(rcClient);
        Invalidate(rcClient);
    }
    return bRet;
}

bool NativeWindow_Windows::MoveWindow(int32_t X, int32_t Y, int32_t nWidth, int32_t nHeight, bool bRepaint)
{
    ASSERT(::IsWindow(m_hWnd));
    return ::MoveWindow(m_hWnd, X, Y, nWidth, nHeight, bRepaint ? TRUE : FALSE) != FALSE;
}

bool NativeWindow_Windows::SetWindowIcon(const FilePath& iconFilePath)
{
    if (SetWindowIconByIcoFile(iconFilePath)) {
        return true;
    }
    std::vector<uint8_t> fileData;
    bool bRet = FileUtil::ReadFileData(iconFilePath, fileData);
    ASSERT(bRet);
    if (bRet) {
        bRet = SetWindowIcon(fileData, iconFilePath.ToString());
    }
    return bRet;
}

bool NativeWindow_Windows::SetWindowIconByIcoFile(const FilePath& iconFilePath)
{
    ASSERT(::IsWindow(m_hWnd));
    if (!::IsWindow(m_hWnd)) {
        return false;
    }

    uint32_t uDpiScaleFactor = m_pOwner->OnNativeGetDpi().GetDisplayScaleFactor();
    uint32_t uDpi = m_pOwner->OnNativeGetDpi().MulDiv(uDpiScaleFactor, 96u, 100u);

    //Large icon
    int32_t cxIcon = GetSystemMetricsForDpiWrapper(SM_CXICON, uDpi);
    int32_t cyIcon = GetSystemMetricsForDpiWrapper(SM_CYICON, uDpi);
    HICON hIcon = (HICON)::LoadImage(nullptr, iconFilePath.NativePath().c_str(), IMAGE_ICON, cxIcon, cyIcon, LR_DEFAULTCOLOR | LR_LOADFROMFILE | LR_SHARED);
    if (StringUtil::IsEqualNoCase(iconFilePath.GetFileExtension(), _T(".ico"))) {
        ASSERT(hIcon != nullptr);
    }    
    if (hIcon != nullptr) {
        ::SendMessage(m_hWnd, WM_SETICON, (WPARAM)TRUE, (LPARAM)hIcon);
    }
    else {
        return false;
    }

    //Small icon
    cxIcon = GetSystemMetricsForDpiWrapper(SM_CXSMICON, uDpi);
    cyIcon = GetSystemMetricsForDpiWrapper(SM_CYSMICON, uDpi);
    hIcon = (HICON)::LoadImage(nullptr, iconFilePath.NativePath().c_str(), IMAGE_ICON, cxIcon, cyIcon, LR_DEFAULTCOLOR | LR_LOADFROMFILE | LR_SHARED);
    if (StringUtil::IsEqualNoCase(iconFilePath.GetFileExtension(), _T(".ico"))) {
        ASSERT(hIcon != nullptr);
    }
    if (hIcon != nullptr) {
        ::SendMessage(m_hWnd, WM_SETICON, (WPARAM)FALSE, (LPARAM)hIcon);
    }
    else {
        return false;
    }
    return true;
}

bool NativeWindow_Windows::SetWindowIcon(const std::vector<uint8_t>& iconFileData, const DString& iconFileName)
{
    uint32_t uDpiScaleFactor = m_pOwner->OnNativeGetDpi().GetDisplayScaleFactor();
    HICON hSmallIcon = nullptr;
    HICON hBigIcon = nullptr;
    if (CreateIconsFromData(iconFileData, iconFileName, uDpiScaleFactor, &hSmallIcon, &hBigIcon)) {
        ::SendMessage(m_hWnd, WM_SETICON, (WPARAM)TRUE, (LPARAM)hBigIcon);
        ::SendMessage(m_hWnd, WM_SETICON, (WPARAM)FALSE, (LPARAM)hSmallIcon);
        return true;
    }
    return false;
}

void NativeWindow_Windows::SetText(const DString& strText)
{
    ASSERT(::IsWindow(m_hWnd));
#ifdef DUILIB_UNICODE
    ::SetWindowText(m_hWnd, strText.c_str());
#else
    //strText is UTF-8 encoded
    DString localText = StringConvert::TToLocal(strText);
    ::SetWindowText(m_hWnd, localText.c_str());
#endif
}

DString NativeWindow_Windows::GetText() const
{
    ASSERT(::IsWindow(m_hWnd));
    DString text;
    int nLen = ::GetWindowTextLength(m_hWnd);
    if (nLen > 0) {
        std::vector<TCHAR> szText;
        szText.resize((size_t)nLen + 2);
        memset(szText.data(), 0, szText.size() * sizeof(TCHAR));
        ::GetWindowText(m_hWnd, szText.data(), (int)szText.size() - 1);
        DString localText = szText.data();
        text = StringConvert::LocalToT(localText);
    }
    return text;
}

void NativeWindow_Windows::SetWindowMaximumSize(const UiSize& szMaxWindow)
{
    m_szMaxWindow = szMaxWindow;
    if (m_szMaxWindow.cx < 0) {
        m_szMaxWindow.cx = 0;
    }
    if (m_szMaxWindow.cy < 0) {
        m_szMaxWindow.cy = 0;
    }
}

const UiSize& NativeWindow_Windows::GetWindowMaximumSize() const
{
    return m_szMaxWindow;
}

void NativeWindow_Windows::SetWindowMinimumSize(const UiSize& szMinWindow)
{
    m_szMinWindow = szMinWindow;
    if (m_szMinWindow.cx < 0) {
        m_szMinWindow.cx = 0;
    }
    if (m_szMinWindow.cy < 0) {
        m_szMinWindow.cy = 0;
    }
}

const UiSize& NativeWindow_Windows::GetWindowMinimumSize() const
{
    return m_szMinWindow;
}

void NativeWindow_Windows::SetCapture()
{
    if (::GetCapture() != m_hWnd) {
        ::SetCapture(m_hWnd);
    }
}

void NativeWindow_Windows::ReleaseCapture()
{
    if (::GetCapture() == m_hWnd) {
        ::ReleaseCapture();
    }
}

bool NativeWindow_Windows::IsCaptured() const
{
    return ::GetCapture() == m_hWnd;
}

bool NativeWindow_Windows::SetWindowRoundRectRgn(const UiRect& rcWnd, float rx, float ry, bool bRedraw)
{
    IRender* pRender = m_pOwner->OnNativeGetRender();
    if (!IsChildWindow()) {
        ASSERT(pRender != nullptr);
    }
    if (pRender != nullptr) {
        return pRender->SetWindowRoundRectRgn(rcWnd, rx, ry, bRedraw);
    }
    return false;
}

bool NativeWindow_Windows::SetWindowRectRgn(const UiRect& rcWnd, bool bRedraw)
{
    IRender* pRender = m_pOwner->OnNativeGetRender();
    if (!IsChildWindow()) {
        ASSERT(pRender != nullptr);
    }
    if (pRender != nullptr) {
        return pRender->SetWindowRectRgn(rcWnd, bRedraw);
    }
    return false;
}

void NativeWindow_Windows::ClearWindowRgn(bool bRedraw)
{
    IRender* pRender = m_pOwner->OnNativeGetRender();
    if (!IsChildWindow()) {
        ASSERT(pRender != nullptr);
    }
    if (pRender != nullptr) {
        pRender->ClearWindowRgn(bRedraw);
    }
}

void NativeWindow_Windows::Invalidate(const UiRect& rcItem)
{
    RECT rc = { rcItem.left, rcItem.top, rcItem.right, rcItem.bottom };
    ::InvalidateRect(m_hWnd, &rc, FALSE);
    // Invalidating a layered window will not trigger a WM_PAINT message,
    // thus we have to post WM_PAINT by ourselves.
    if (IsLayeredWindow()) {
        ::PostMessage(m_hWnd, WM_PAINT, 0, 0);
    }
}

bool NativeWindow_Windows::UpdateWindow() const
{
    bool bRet = false;
    if ((m_hWnd != nullptr) && ::IsWindow(m_hWnd)) {
        bRet = ::UpdateWindow(m_hWnd) != FALSE;
    }
    return bRet;
}

/** The helper class for drawing
*/
class NativeWindowRenderPaint:
    public IRenderPaint
{
public:
    NativeWindow_Windows* m_pNativeWindow = nullptr;
    INativeWindow* m_pOwner = nullptr;
    NativeMsg m_nativeMsg;
    bool m_bHandled = false;

public:
    /** Complete the drawing through the callback interface
    * @param [in] rcPaint The area to be drawn (client area coordinates)
    */
    virtual bool DoPaint(const UiRect& rcPaint) override
    {
        if (m_pOwner != nullptr) {
            NativeMsg nativeMsg = NativeMsg(WM_PAINT, (WPARAM)m_pNativeWindow->GetHWND(), 0);
            m_pOwner->OnNativePaintMsg(rcPaint, nativeMsg, m_bHandled);
            return true;
        }
        return false;
    }

    /** Callback interface to get the transparency value of the current window
    */
    virtual uint8_t GetLayeredWindowAlpha() override
    {
        return m_pNativeWindow->GetLayeredWindowAlpha();
    }

    /** Get the area that the UI needs to draw, to achieve partial drawing
    * @param [out] rcUpdate Returns the rectangular range of the area to be drawn
    * @return Returns true if partial drawing is supported; returns false if partial drawing is not supported
    */
    virtual bool GetUpdateRect(UiRect& rcUpdate) const override
    {
        RECT rectUpdate = { 0, };
        if (::GetUpdateRect(m_pNativeWindow->GetHWND(), &rectUpdate, FALSE)) {
            rcUpdate.left = rectUpdate.left;
            rcUpdate.top = rectUpdate.top;
            rcUpdate.right = rectUpdate.right;
            rcUpdate.bottom = rectUpdate.bottom;
        }
        else {
            rcUpdate.Clear();
        }
        return !rcUpdate.IsEmpty();
    }
};

LRESULT NativeWindow_Windows::OnPaintMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    //Callback to prepare the drawing function
    LRESULT lResult = 0;
    bHandled = false;
    bool bPaint = m_pOwner->OnNativePreparePaint();
    RECT rectUpdate = { 0, };
    if (!::GetUpdateRect(m_hWnd, &rectUpdate, FALSE)) {
        bPaint = false;
    }
    if (bPaint) {
        if (IsChildWindow()) {
            //Child window mode: drawing is completely handled by the application layer
            if (m_pOwner != nullptr) {
                UiRect rcPaint(rectUpdate.left, rectUpdate.top, rectUpdate.right, rectUpdate.bottom);
                NativeMsg nativeMsg = NativeMsg(WM_PAINT, (WPARAM)m_hWnd, 0);
                m_pOwner->OnNativePaintMsg(rcPaint, nativeMsg, bHandled);
                bPaint = true;
            }
        }
        else {
            //Normal mode: the drawing flow is managed internally
            IRender* pRender = m_pOwner->OnNativeGetRender();
            ASSERT(pRender != nullptr);
            if (pRender != nullptr) {
                NativeWindowRenderPaint renderPaint;
                renderPaint.m_pNativeWindow = this;
                renderPaint.m_pOwner = m_pOwner;
                renderPaint.m_nativeMsg = NativeMsg(uMsg, wParam, lParam);
                renderPaint.m_bHandled = bHandled;
                bPaint = pRender->PaintAndSwapBuffers(&renderPaint);
                bHandled = renderPaint.m_bHandled;
            }
        }
    }
    if (!bPaint) {
        PAINTSTRUCT ps = { 0, };
        ::BeginPaint(m_hWnd, &ps);
        ::EndPaint(m_hWnd, &ps);
    }
    return lResult;
}

void NativeWindow_Windows::KeepParentActive()
{
    HWND hWndParent = GetHWND();
    if (::IsWindow(hWndParent)) {
        while (::GetParent(hWndParent) != nullptr) {
            hWndParent = ::GetParent(hWndParent);
        }
    }
    if (::IsWindow(hWndParent)) {
        ::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
    }
}

void NativeWindow_Windows::GetClientRect(UiRect& rcClient) const
{
    RECT rc = { 0, 0, 0, 0 };
    ::GetClientRect(m_hWnd, &rc);
    rcClient = UiRect(rc.left, rc.top, rc.right, rc.bottom);
}

void NativeWindow_Windows::GetWindowRect(UiRect& rcWindow) const
{
    RECT rc = { 0, 0, 0, 0 };
    ::GetWindowRect(m_hWnd, &rc);
    rcWindow = UiRect(rc.left, rc.top, rc.right, rc.bottom);
}

void NativeWindow_Windows::GetWindowRect(HWND hWnd, UiRect& rcWindow) const
{
    RECT rc = { 0, 0, 0, 0 };
    ::GetWindowRect(hWnd, &rc);
    rcWindow = UiRect(rc.left, rc.top, rc.right, rc.bottom);
}

void NativeWindow_Windows::ScreenToClient(UiPoint& pt) const
{
    POINT ptClient = { pt.x, pt.y };
    ::ScreenToClient(m_hWnd, &ptClient);
    pt = UiPoint(ptClient.x, ptClient.y);
}

void NativeWindow_Windows::ClientToScreen(UiPoint& pt) const
{
    POINT ptClient = { pt.x, pt.y };
    ::ClientToScreen(m_hWnd, &ptClient);
    pt = UiPoint(ptClient.x, ptClient.y);
}

void NativeWindow_Windows::GetCursorPos(UiPoint& pt) const
{
    POINT ptPos;
    ::GetCursorPos(&ptPos);
    pt = { ptPos.x, ptPos.y };
}

bool NativeWindow_Windows::GetMonitorRect(UiRect& rcMonitor) const
{
    UiRect rcWork;
    return GetMonitorRect(m_hWnd, rcMonitor, rcWork);
}

bool NativeWindow_Windows::GetMonitorRect(HWND hWnd, UiRect& rcMonitor, UiRect& rcWork) const
{
    rcMonitor.Clear();
    rcWork.Clear();
    HMONITOR hMonitor = nullptr;
    if (::IsWindow(hWnd)) {
        hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    }
    else {
        hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
    }
    ASSERT(hMonitor != nullptr);
    if (hMonitor == nullptr) {
        return false;
    }
    MONITORINFO oMonitor = { 0, };
    oMonitor.cbSize = sizeof(oMonitor);
    if (::GetMonitorInfo(hMonitor, &oMonitor)) {
        rcWork = UiRect(oMonitor.rcWork.left, oMonitor.rcWork.top,
                        oMonitor.rcWork.right, oMonitor.rcWork.bottom);
        rcMonitor = UiRect(oMonitor.rcMonitor.left, oMonitor.rcMonitor.top,
                           oMonitor.rcMonitor.right, oMonitor.rcMonitor.bottom);
        return true;
    }
    else {
        ASSERT(!"NativeWindow_Windows::GetMonitorWorkRect failed!");
        return false;
    }
}

bool NativeWindow_Windows::GetMonitorWorkRect(UiRect& rcWork) const
{
    UiRect rcMonitor;
    return GetMonitorRect(m_hWnd, rcMonitor, rcWork);
}

bool NativeWindow_Windows::GetPrimaryMonitorWorkRect(UiRect& rcWork)
{
    rcWork.Clear();
    HMONITOR hMonitor = ::MonitorFromPoint({ INT32_MIN, INT32_MIN }, MONITOR_DEFAULTTOPRIMARY);
    ASSERT(hMonitor != nullptr);
    if (hMonitor == nullptr) {
        return false;
    }
    MONITORINFO oMonitor = { 0, };
    oMonitor.cbSize = sizeof(oMonitor);
    if (::GetMonitorInfo(hMonitor, &oMonitor)) {
        rcWork = UiRect(oMonitor.rcWork.left, oMonitor.rcWork.top,
                        oMonitor.rcWork.right, oMonitor.rcWork.bottom);
        return true;
    }
    else {
        ASSERT(!"NativeWindow_Windows::GetPrimaryMonitorWorkRect failed!");
        return false;
    }
}

bool NativeWindow_Windows::GetMonitorWorkRect(const UiPoint& pt, UiRect& rcWork) const
{
    rcWork.Clear();
    HMONITOR hMonitor = ::MonitorFromPoint({ pt.x, pt.y }, MONITOR_DEFAULTTONEAREST);
    ASSERT(hMonitor != nullptr);
    if (hMonitor == nullptr) {
        return false;
    }
    MONITORINFO oMonitor = { 0, };
    oMonitor.cbSize = sizeof(oMonitor);
    if (::GetMonitorInfo(hMonitor, &oMonitor)) {
        rcWork = UiRect(oMonitor.rcWork.left, oMonitor.rcWork.top,
                        oMonitor.rcWork.right, oMonitor.rcWork.bottom);
        return true;
    }
    else {
        ASSERT(!"NativeWindow_Windows::GetMonitorWorkRect failed!");
        return false;
    }
}

const UiPoint& NativeWindow_Windows::GetLastMousePos() const
{
    return m_ptLastMousePos;
}

void NativeWindow_Windows::SetLastMousePos(const UiPoint& pt)
{
    m_ptLastMousePos = pt;
}

bool NativeWindow_Windows::GetModifiers(UINT message, WPARAM wParam, LPARAM lParam, uint32_t& modifierKey) const
{
    //Logic changes need to be synchronized with the function: Window::IsKeyDown
    bool bRet = true;
    modifierKey = ModifierKey::kNone;
    switch (message) {
    case WM_SYSCHAR:
    case WM_CHAR:
        if (0 == (lParam & (1 << 30))) {
            modifierKey |= ModifierKey::kFirstPress;
        }
        if (lParam & (1 << 29)) {
            modifierKey |= ModifierKey::kAlt;
        }
        break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (0 == (lParam & (1 << 30))) {
            modifierKey |= ModifierKey::kFirstPress;
        }
        if (lParam & (1 << 29)) {
            modifierKey |= ModifierKey::kAlt;
        }
        break;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        if (lParam & (1 << 29)) {
            modifierKey |= ModifierKey::kAlt;
        }
        break;
    case WM_MOUSEWHEEL:
        {
            WORD fwKeys = GET_KEYSTATE_WPARAM(wParam);
            if (fwKeys & MK_CONTROL) {
                modifierKey |= ModifierKey::kControl;
            }
            if (fwKeys & MK_SHIFT) {
                modifierKey |= ModifierKey::kShift;
            }
        }
        break;
    case WM_MOUSEHOVER:
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MBUTTONDBLCLK:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    case WM_XBUTTONDBLCLK:
        if (wParam & MK_CONTROL) {
            modifierKey |= ModifierKey::kControl;
        }
        if (wParam & MK_SHIFT) {
            modifierKey |= ModifierKey::kShift;
        }
        break;
    case WM_HOTKEY:
        {
            uint16_t nMod = (uint16_t)LOWORD(lParam);
            if (nMod & MOD_ALT) {
                modifierKey |= ModifierKey::kAlt;
            }
            else if (nMod & MOD_CONTROL) {
                modifierKey |= ModifierKey::kControl;
            }
            else if (nMod & MOD_SHIFT) {
                modifierKey |= ModifierKey::kShift;
            }
            else if (nMod & MOD_WIN) {
                modifierKey |= ModifierKey::kWin;
            }
        }
        break;
    default:
        bRet = false;
        break;
    }
    if ((message == WM_SYSCHAR) || (message == WM_SYSKEYDOWN) || (message == WM_SYSKEYUP)) {
        modifierKey |= ModifierKey::kIsSystemKey;
    }
    ASSERT(bRet);
    return bRet;
}


int32_t NativeWindow_Windows::SetWindowHotKey(uint8_t wVirtualKeyCode, uint8_t wModifiers)
{
    ASSERT(IsWindow());
    return (int32_t)::SendMessage(GetHWND(), WM_SETHOTKEY, MAKEWORD(wVirtualKeyCode, wModifiers), 0);
}

bool NativeWindow_Windows::GetWindowHotKey(uint8_t& wVirtualKeyCode, uint8_t& wModifiers) const
{
    ASSERT(IsWindow());
    DWORD dw = (DWORD)::SendMessage(GetHWND(), HKM_GETHOTKEY, 0, 0L);
    wVirtualKeyCode = LOBYTE(LOWORD(dw));
    wModifiers = HIBYTE(LOWORD(dw));
    return dw != 0;
}

bool NativeWindow_Windows::RegisterHotKey(uint8_t wVirtualKeyCode, uint8_t wModifiers, int32_t id)
{
    ASSERT(IsWindow());
    if (wVirtualKeyCode != 0) {
        UINT fsModifiers = 0;
        if (wModifiers & HOTKEYF_ALT)     fsModifiers |= MOD_ALT;
        if (wModifiers & HOTKEYF_CONTROL) fsModifiers |= MOD_CONTROL;
        if (wModifiers & HOTKEYF_SHIFT)   fsModifiers |= MOD_SHIFT;
        if (wModifiers & HOTKEYF_EXT)     fsModifiers |= MOD_WIN;

#ifndef MOD_NOREPEAT
        if (::IsWindows7OrGreater()) {
            fsModifiers |= 0x4000;
        }
#else
        fsModifiers |= MOD_NOREPEAT;
#endif

        LRESULT lResult = ::RegisterHotKey(this->GetHWND(), id, fsModifiers, wVirtualKeyCode);
        ASSERT(lResult != 0);
        if (lResult != 0) {
            auto iter = std::find(m_hotKeyIds.begin(), m_hotKeyIds.end(), id);
            if (iter != m_hotKeyIds.end()) {
                m_hotKeyIds.erase(iter);
            }
            m_hotKeyIds.push_back(id);
            return true;
        }
    }
    return false;
}

bool NativeWindow_Windows::UnregisterHotKey(int32_t id)
{
    ASSERT(IsWindow());
    auto iter = std::find(m_hotKeyIds.begin(), m_hotKeyIds.end(), id);
    if (iter != m_hotKeyIds.end()) {
        m_hotKeyIds.erase(iter);
    }
    return ::UnregisterHotKey(GetHWND(), id);
}

/** The property name of the window handle
*/
static const DStringW::value_type* sPropName  = L"DuiLibWindow";     // Property name (for pointer validation)
static const DStringW::value_type* sPropName2 = L"DuiLibWindow2";    // Property name (process ID)

LRESULT CALLBACK NativeWindow_Windows::__WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    NativeWindow_Windows* pThis = nullptr;
    if (uMsg == WM_NCCREATE) {
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pThis = static_cast<NativeWindow_Windows*>(lpcs->lpCreateParams);
        if (pThis != nullptr) {
            pThis->m_hWnd = hWnd;
        }
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pThis));
        ::SetPropW(hWnd, sPropName, (HANDLE)pThis);
        ::SetPropW(hWnd, sPropName2, (HANDLE)(size_t)::GetCurrentProcessId());
    }
    else {
        pThis = reinterpret_cast<NativeWindow_Windows*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
#ifdef _DEBUG
        //Verify whether they are consistent
        ASSERT((NativeWindow_Windows*)::GetPropW(hWnd, sPropName) == pThis);
        if (pThis != nullptr) {
            ASSERT(::GetPropW(hWnd, sPropName2) == (HANDLE)(size_t)::GetCurrentProcessId());
        }        
#endif
        if (uMsg == WM_NCDESTROY && pThis != nullptr) {            
            LRESULT lRes = ::DefWindowProc(hWnd, uMsg, wParam, lParam);
            ::SetWindowLongPtr(pThis->m_hWnd, GWLP_USERDATA, 0L);
            ::SetPropW(hWnd, sPropName, nullptr);
            ::SetPropW(hWnd, sPropName2, nullptr);
            ASSERT(hWnd == pThis->GetHWND());
            pThis->OnFinalMessage();
            return lRes;
        }
    }

    if (pThis != nullptr) {
        ASSERT(hWnd == pThis->GetHWND());
        return pThis->WindowMessageProc(uMsg, wParam, lParam);
    }
    else {
        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}


INT_PTR CALLBACK NativeWindow_Windows::__DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_INITDIALOG) {
        //This is the first message of the dialog
        NativeWindow_Windows* pThis = reinterpret_cast<NativeWindow_Windows*>(lParam);
        ASSERT(pThis != nullptr);
        if (pThis != nullptr) {
            ASSERT(pThis->m_hWnd == nullptr);
            pThis->m_hWnd = hWnd;            
            ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pThis));
            ::SetPropW(hWnd, sPropName, (HANDLE)pThis);
            ::SetPropW(hWnd, sPropName2, (HANDLE)(size_t)::GetCurrentProcessId());

            ASSERT(pThis->m_pfnOldWndProc == nullptr);

            //Replace the window processing function of the dialog
            pThis->m_pfnOldWndProc = (WNDPROC)::GetWindowLongPtr(hWnd, GWLP_WNDPROC);
            ::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LPARAM)(WNDPROC)NativeWindow_Windows::__DialogWndProc);

            //Dispatch this message
            pThis->WindowMessageProc(uMsg, wParam, lParam);
            ::SetFocus(hWnd);
            return TRUE;
        }
    }
    else if (uMsg == WM_COMMAND) {
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            NativeWindow_Windows* pThis = reinterpret_cast<NativeWindow_Windows*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
#ifdef _DEBUG
            //Verify whether they are consistent
            ASSERT((NativeWindow_Windows*)::GetPropW(hWnd, sPropName) == pThis);
            if (pThis != nullptr) {
                ASSERT(::GetPropW(hWnd, sPropName2) == (HANDLE)(size_t)::GetCurrentProcessId());
            }
#endif
            ASSERT(pThis != nullptr);
            if (pThis != nullptr) {
                if (pThis->m_bCloseByEsc && (LOWORD(wParam) == IDCANCEL)) {
                    pThis->CloseWnd(kWindowCloseCancel);
                }
                else if (pThis->m_bCloseByEnter && (LOWORD(wParam) == IDOK)) {
                    pThis->CloseWnd(kWindowCloseOK);
                }
            }
            return TRUE;
        }
    }    
    return FALSE;
}

LRESULT CALLBACK NativeWindow_Windows::__DialogWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    NativeWindow_Windows* pThis = reinterpret_cast<NativeWindow_Windows*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
#ifdef _DEBUG
    //Verify whether they are consistent
    ASSERT((NativeWindow_Windows*)::GetPropW(hWnd, sPropName) == pThis);
    if (pThis != nullptr) {
        ASSERT(::GetPropW(hWnd, sPropName2) == (HANDLE)(size_t)::GetCurrentProcessId());
    }
#endif
    ASSERT(pThis != nullptr);
    if (uMsg == WM_NCDESTROY && pThis != nullptr) {
        if (pThis->m_pfnOldWndProc != nullptr) {
            ::SetWindowLongPtr((hWnd), GWLP_WNDPROC, (LPARAM)(WNDPROC)(pThis->m_pfnOldWndProc));
            pThis->m_pfnOldWndProc = nullptr;
        }
        LRESULT lRes = ::DefWindowProc(hWnd, uMsg, wParam, lParam);
        ::SetWindowLongPtr(pThis->m_hWnd, GWLP_USERDATA, 0L);
        ::SetPropW(hWnd, sPropName, nullptr);
        ::SetPropW(hWnd, sPropName2, nullptr);
        ASSERT(hWnd == pThis->GetHWND());
        pThis->OnFinalMessage();
        return lRes;
    }
    else if (pThis != nullptr) {
        ASSERT(hWnd == pThis->GetHWND());
        return pThis->WindowMessageProc(uMsg, wParam, lParam);
    }
    return ::DefDlgProc(hWnd, uMsg, wParam, lParam);
}

LRESULT NativeWindow_Windows::WindowMessageProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = 0;
    HWND hWnd = m_hWnd;
    INativeWindow* pOwner = m_pOwner;
    if (pOwner == nullptr) {
        lResult = CallDefaultWindowProc(uMsg, wParam, lParam);
        return lResult;
    }
    //The lifecycle flag of the interface
    std::weak_ptr<WeakFlag> ownerFlag = pOwner->GetWeakFlag();

    //The message is first passed to the filter (all messages)
    bool bHandled = false;
    if (!bHandled && !ownerFlag.expired()) {
        lResult = pOwner->OnNativeWindowMessage(uMsg, wParam, lParam, bHandled);
    }

    if (m_bDoModal && (uMsg == WM_KEYDOWN) && (wParam == VK_ESCAPE)) {
        //Modal dialog: close when the ESC key is pressed
        CloseWnd(kWindowCloseCancel);
        return 0;
    }

    //Third priority: messages handled internally; after handling, they are no longer dispatched
    if (!bHandled && !ownerFlag.expired()) {
        lResult = ProcessInternalMessage(uMsg, wParam, lParam, bHandled);
    }

    //Fourth priority: internal processing functions; prioritize ensuring their own functions work correctly
    if (!bHandled && !ownerFlag.expired()) {
        lResult = ProcessWindowMessage(uMsg, wParam, lParam, bHandled);
    }

    const bool bWindowCloseMsg = (uMsg == WM_CLOSE) || ((uMsg == WM_SYSCOMMAND) && (GET_SC_WPARAM(wParam) == SC_CLOSE));
    bool bWindowClosed = false;
    if (!bHandled && bWindowCloseMsg && !ownerFlag.expired()) {
        //The window is about to close (before closing)
        StopSysMenuTimer();

        //Preserve the exit parameter of the window close
        if (uMsg == WM_CLOSE) {
            m_closeParam = (int32_t)wParam;
        }

        bWindowClosed = true;
        pOwner->OnNativePreCloseWindow();
    }
    else if (bHandled && bWindowCloseMsg && !ownerFlag.expired()) {
        //Restore the state before closing
        m_bCloseing = false;
        m_closeParam = kWindowCloseNormal;
    }

    //Fifth priority: the system default window function
    if (!bHandled && !ownerFlag.expired() && ::IsWindow(hWnd)) {
        if (bWindowClosed && m_bDoModal) {
            //Modal dialog
            ::EndDialog(hWnd, wParam);
            lResult = 0;
            bHandled = true;
        }
        else {
            lResult = CallDefaultWindowProc(uMsg, wParam, lParam);
        }        
    }

    if (!bHandled && !ownerFlag.expired() && (uMsg == WM_DESTROY)) {
        //The window has been closed (after closing)
        pOwner->OnNativePostCloseWindow();
    }
    return lResult;
}

LRESULT NativeWindow_Windows::CallDefaultWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (m_pfnOldWndProc != nullptr) {
        return ::CallWindowProc(m_pfnOldWndProc, m_hWnd, uMsg, wParam, lParam);
    }
    return ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}

LRESULT NativeWindow_Windows::ProcessInternalMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    LRESULT lResult = 0;
    bHandled = false;
    bool bInternalMsg = true;
    switch (uMsg)
    {
    case WM_NCACTIVATE:         lResult = OnNcActivateMsg(uMsg, wParam, lParam, bHandled); break;
    case WM_NCCALCSIZE:         lResult = OnNcCalcSizeMsg(uMsg, wParam, lParam, bHandled); break;
    case WM_NCHITTEST:          lResult = OnNcHitTestMsg(uMsg, wParam, lParam, bHandled); break;

    case WM_GETMINMAXINFO:      lResult = OnGetMinMaxInfoMsg(uMsg, wParam, lParam, bHandled); break;
    case WM_ERASEBKGND:         lResult = OnEraseBkGndMsg(uMsg, wParam, lParam, bHandled); break;
    case WM_DISPLAYCHANGE:      lResult = OnDisplayChangedMsg(uMsg, wParam, lParam, bHandled); break;
    case WM_DPICHANGED:         lResult = OnDpiChangedMsg(uMsg, wParam, lParam, bHandled); break;
    case WM_WINDOWPOSCHANGING:  lResult = OnWindowPosChangingMsg(uMsg, wParam, lParam, bHandled); break;

    case WM_NOTIFY:             lResult = OnNotifyMsg(uMsg, wParam, lParam, bHandled); break;
    case WM_COMMAND:            lResult = OnCommandMsg(uMsg, wParam, lParam, bHandled); break;
    case WM_CTLCOLOREDIT:       lResult = OnCtlColorMsgs(uMsg, wParam, lParam, bHandled); break;
    case WM_CTLCOLORSTATIC:     lResult = OnCtlColorMsgs(uMsg, wParam, lParam, bHandled); break;
    case WM_TOUCH:              lResult = OnTouchMsg(uMsg, wParam, lParam, bHandled); break;
    case WM_POINTERDOWN:
    case WM_POINTERUP:
    case WM_POINTERUPDATE:
    case WM_POINTERLEAVE:
    case WM_POINTERWHEEL:
    case WM_POINTERCAPTURECHANGED:
        lResult = OnPointerMsgs(uMsg, wParam, lParam, bHandled);
        break;

    case WM_CREATE:     lResult = OnCreateMsg(uMsg, wParam, lParam, bHandled); break;
    case WM_INITDIALOG: lResult = OnInitDialogMsg(uMsg, wParam, lParam, bHandled); break;

    case WM_TIMER:
        {
            if (wParam == m_nSysMenuTimerId) {
                //The timer for the delayed display of the system menu is triggered
                ::KillTimer(m_hWnd, m_nSysMenuTimerId);
                m_nSysMenuTimerId = 0;

                POINT pt;
                ::GetCursorPos(&pt);
                ShowWindowSysMenu(m_hWnd, pt);
                bHandled = true;
            }
        }
        break;
    default:
        bInternalMsg = false;
        break;
    }//end of switch

    if (bInternalMsg && !bHandled) {
        //Call the window function, then stop passing this message
        bHandled = true;
        lResult = CallDefaultWindowProc(uMsg, wParam, lParam);
    }
    return lResult;
}

INativeWindow* NativeWindow_Windows::WindowBaseFromPoint(const UiPoint& pt, bool bIgnoreChildWindow)
{
    NativeWindow_Windows* pWindow = nullptr;
    HWND hWndPt = ::WindowFromPoint({ pt.x, pt.y });
    std::vector<HWND> hwndList;
    if (::IsWindow(hWndPt)) {
        hwndList.push_back(hWndPt);
        if (bIgnoreChildWindow) {
            //Get the parent window list
            HWND hParent = ::GetParent(hWndPt);
            while ((hParent != nullptr) && ::IsWindow(hParent)) {
                hwndList.push_back(hParent);
                hParent = ::GetParent(hParent);
            }
        }
    }

    for (HWND hWnd : hwndList) {
        if (hWnd == m_hWnd) {
            pWindow = this;
        }
        else {
            pWindow = reinterpret_cast<NativeWindow_Windows*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
            if (pWindow != nullptr) {
                if (::GetPropW(hWnd, sPropName2) != (HANDLE)(size_t)::GetCurrentProcessId()) {
                    //Validation failed: not a duilib window
                    pWindow = nullptr;
                }
                else if ((NativeWindow_Windows*)::GetPropW(hWnd, sPropName) != pWindow) {
                    //Validation failed: not a duilib window
                    pWindow = nullptr;
                }
                else if (pWindow->m_hWnd != hWnd) {
                    pWindow = nullptr;
                }
            }
        }
        if (pWindow != nullptr) {
            break;
        }
    }
    INativeWindow* pNativeWindow = nullptr;
    if (pWindow != nullptr) {
        pNativeWindow = pWindow->m_pOwner;
    }
    return pNativeWindow;
}

LRESULT NativeWindow_Windows::OnNcActivateMsg(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& bHandled)
{
    ASSERT_UNUSED_VARIABLE(uMsg == WM_NCACTIVATE);
    if (IsUseSystemCaption()) {
        bHandled = false;
        return 0;
    }

    LRESULT lResult = 0;
    if (IsWindowMinimized()) {
        bHandled = false;
    }
    else {
        //MSDN: when the wParam parameter is FALSE, the application should return TRUE to instruct the system to continue with the default processing
        bHandled = true;
        lResult = (wParam == FALSE) ? TRUE : FALSE;
    }
    return lResult;
}

LRESULT NativeWindow_Windows::OnNcCalcSizeMsg(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, bool& bHandled)
{
    ASSERT_UNUSED_VARIABLE(uMsg == WM_NCCALCSIZE);
    if (IsUseSystemCaption()) {
        bHandled = false;
        return 0;
    }

    //Intercept this message so that the system does not process it
    bHandled = true;
    return 0;
}

LRESULT NativeWindow_Windows::OnNcHitTestMsg(UINT uMsg, WPARAM /*wParam*/, LPARAM lParam, bool& bHandled)
{
    ASSERT_UNUSED_VARIABLE(uMsg == WM_NCHITTEST);
    if (IsUseSystemCaption()) {
        bHandled = false;
        return 0;
    }

    bHandled = true;
    if (IsWindowFullscreen()) {
        //In fullscreen state, the entire window is in the work area
        return HTCLIENT;
    }

    UiPoint pt;
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);
    ScreenToClient(pt);

    UiRect rcClient;
    GetClientRect(rcClient);

    //Client area, excluding the shadow area
    UiPadding rcCorner;
    m_pOwner->OnNativeGetShadowCorner(rcCorner);
    rcClient.Deflate(rcCorner);

    if (!IsWindowMaximized()) {
        //Not maximized state
        UiRect rcSizeBox = m_pOwner->OnNativeGetSizeBox();
        if (pt.y < rcClient.top + rcSizeBox.top) {
            if (pt.y >= rcClient.top) {
                if (pt.x < (rcClient.left + rcSizeBox.left) && pt.x >= rcClient.left) {
                    return HTTOPLEFT;//In the top-left corner of the window border
                }
                else if (pt.x > (rcClient.right - rcSizeBox.right) && pt.x <= rcClient.right) {
                    return HTTOPRIGHT;//In the top-right corner of the window border
                }
                else {
                    return HTTOP;//In the upper horizontal border of the window
                }
            }
            else {
                return HTCLIENT;//In the work area
            }
        }
        else if (pt.y > rcClient.bottom - rcSizeBox.bottom) {
            if (pt.y <= rcClient.bottom) {
                if (pt.x < (rcClient.left + rcSizeBox.left) && pt.x >= rcClient.left) {
                    return HTBOTTOMLEFT;//In the bottom-left corner of the window border
                }
                else if (pt.x > (rcClient.right - rcSizeBox.right) && pt.x <= rcClient.right) {
                    return HTBOTTOMRIGHT;//In the bottom-right corner of the window border
                }
                else {
                    return HTBOTTOM;//In the lower horizontal border of the window
                }
            }
            else {
                return HTCLIENT;//In the work area
            }
        }

        if (pt.x < rcClient.left + rcSizeBox.left) {
            if (pt.x >= rcClient.left) {
                return HTLEFT;//In the left border of the window
            }
            else {
                return HTCLIENT;//In the work area
            }
        }
        if (pt.x > rcClient.right - rcSizeBox.right) {
            if (pt.x <= rcClient.right) {
                return HTRIGHT;//In the right border of the window
            }
            else {
                return HTCLIENT;//In the work area
            }
        }
    }

    UiRect rcCaption;
    m_pOwner->OnNativeGetCaptionRect(rcCaption);
    //The rectangular range of the title bar area
    UiRect rcCaptionRect;
    rcCaptionRect.left = rcClient.left + rcCaption.left;
    rcCaptionRect.right = rcClient.right - rcCaption.right;
    rcCaptionRect.top = rcClient.top + rcCaption.top;
    rcCaptionRect.bottom = rcClient.top + rcCaption.bottom;
    if (rcCaptionRect.ContainsPt(pt)) {
        //In the title bar range
        UiRect sysMenuRect;
        m_pOwner->OnNativeGetSysMenuRect(sysMenuRect);
        sysMenuRect.Offset(rcClient.left, rcClient.top);
        sysMenuRect.Intersect(rcCaptionRect);
        if (!sysMenuRect.IsEmpty()) {
            if (sysMenuRect.ContainsPt(pt)) {                
                return HTSYSMENU;//In the system menu rectangle area
            }
        }

        //Whether the snap layout menu is supported
        bool bPtInMaximizeRestoreButton = false;        
        if (IsEnableSnapLayoutMenu()) {
            bPtInMaximizeRestoreButton = m_pOwner->OnNativeIsPtInMaximizeRestoreButton(pt);
        }
        if (bPtInMaximizeRestoreButton) {
            return HTMAXBUTTON; //On the maximize or restore button, show the snap layout menu
        }
        else if (m_pOwner->OnNativeIsPtInCaptionBarControl(pt)) {
            return HTCLIENT;//In the work area (controls placed on the title bar are treated as the work area)
        }
        else {
            return HTCAPTION;//In the title bar
        }
    }
    //Others: in the work area
    return HTCLIENT;
}

LRESULT NativeWindow_Windows::OnGetMinMaxInfoMsg(UINT uMsg, WPARAM /*wParam*/, LPARAM lParam, bool& bHandled)
{
    ASSERT_UNUSED_VARIABLE(uMsg == WM_GETMINMAXINFO);
    bHandled = false;
    LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
    UiRect rcWork;
    UiRect rcMonitor;
    GetMonitorRect(m_hWnd, rcMonitor, rcWork);
    rcWork.Offset(-rcMonitor.left, -rcMonitor.top);

    //When maximized, the maximum area of the current screen is set by default
    lpMMI->ptMaxPosition.x = rcWork.left;
    lpMMI->ptMaxPosition.y = rcWork.top;
    lpMMI->ptMaxSize.x = rcWork.Width();
    lpMMI->ptMaxSize.y = rcWork.Height();

    if (GetWindowMaximumSize().cx != 0) {
        lpMMI->ptMaxTrackSize.x = GetWindowMaximumSize().cx;
    }
    if (GetWindowMaximumSize().cy != 0) {
        lpMMI->ptMaxTrackSize.y = GetWindowMaximumSize().cy;
    }

    UiPadding rcShadow;
    m_pOwner->OnNativeGetShadowCorner(rcShadow);
    UiSize minSize = GetWindowMinimumSize();
    minSize.cx = std::max(minSize.cx, rcShadow.left + rcShadow.right + m_pOwner->OnNativeGetDpi().GetScaleInt(16));
    minSize.cy = std::max(minSize.cy, rcShadow.top + rcShadow.bottom + m_pOwner->OnNativeGetDpi().GetScaleInt(16));

    if (minSize.cx > 0) {
        lpMMI->ptMinTrackSize.x = minSize.cx;
    }
    if (minSize.cy > 0) {
        lpMMI->ptMinTrackSize.y = minSize.cy;
    }
    return 0;
}

LRESULT NativeWindow_Windows::OnEraseBkGndMsg(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, bool& bHandled)
{
    ASSERT_UNUSED_VARIABLE(uMsg == WM_ERASEBKGND);
    bHandled = true;
    return 1;
}

LRESULT NativeWindow_Windows::OnDisplayChangedMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    ASSERT_UNUSED_VARIABLE(uMsg == WM_DISPLAYCHANGE);
    bHandled = false;
    // Parse the WM_DISPLAYCHANGE message parameters
    int32_t nColorDepth = (int)wParam;                  // Color depth (bits per pixel)
    int32_t nScreenWidth = LOWORD(lParam);              // Screen horizontal resolution (width)
    int32_t nScreenHeight = HIWORD(lParam);             // Screen vertical resolution (height)
    m_pOwner->OnNativeDisplayResolutionChangedMsg(nColorDepth, nScreenWidth, nScreenHeight);
    return 0;
}

LRESULT NativeWindow_Windows::OnDpiChangedMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    ASSERT_UNUSED_VARIABLE(uMsg == WM_DPICHANGED);
    bHandled = false;
    std::weak_ptr<WeakFlag> windowFlag = m_pOwner->GetWeakFlag();

    //The display DPI of the window changes; trigger the DPI change event
    uint32_t nNewDPI = HIWORD(wParam);
    uint32_t nOldWindowDpiScaleFactor = m_nWindowDpiScaleFactor;
    m_nWindowDpiScaleFactor = DpiManager::MulDiv(nNewDPI, 100u, 96u);
    float fNewDisplayScale = (float)DpiManager::MulDiv(nNewDPI, 100u, 96u) / 100.0f;
    float fNewPixelDensity = 1.0f;

    UiRect rcNewWindow;
    const RECT* prcNewWindow = (RECT*)lParam;
    if (prcNewWindow != nullptr) {
        rcNewWindow.left = prcNewWindow->left;
        rcNewWindow.top = prcNewWindow->top;
        rcNewWindow.right = prcNewWindow->right;
        rcNewWindow.bottom = prcNewWindow->bottom;
    }
    bool bDisplayScaleChanged = false;
    uint32_t nOldDisplayScale = m_pOwner->OnNativeGetDpi().GetDisplayScaleFactor();
    m_pOwner->OnNativeProcessDisplayScaleChangedMsg(fNewDisplayScale, fNewPixelDensity);
    if (windowFlag.expired()) {
        return 0;
    }
    if (nOldDisplayScale != m_pOwner->OnNativeGetDpi().GetDisplayScaleFactor()) {
        bDisplayScaleChanged = true;
        m_ptLastMousePos = m_pOwner->OnNativeGetDpi().GetScalePoint(m_ptLastMousePos, nOldDisplayScale);
    }
    //Update the position and size of the window
    if (!rcNewWindow.IsEmpty()) {
        uint32_t nNewDisplayScale = DpiManager::MulDiv(nNewDPI, 100u, 96u);
        if (!bDisplayScaleChanged && (nNewDisplayScale != 0) && (nOldWindowDpiScaleFactor != 0) && (nNewDisplayScale != nOldWindowDpiScaleFactor)) {
            //If the DPI change message is not handled, keep the original size (needs to be done asynchronously)
            UiRect rcWindow = rcNewWindow;            
            rcWindow.right = rcWindow.left + DpiManager::MulDiv((uint32_t)rcNewWindow.Width(), nOldWindowDpiScaleFactor, nNewDisplayScale);
            rcWindow.bottom = rcWindow.top + DpiManager::MulDiv((uint32_t)rcNewWindow.Height(), nOldWindowDpiScaleFactor, nNewDisplayScale);
            HWND hWnd = GetHWND();
            GlobalManager::Instance().Thread().PostTask(ui::kThreadUI, [hWnd, rcWindow]() {
                    ::SetWindowPos(hWnd, nullptr,
                                   rcWindow.left, rcWindow.top, rcWindow.Width(), rcWindow.Height(),
                                   SWP_NOZORDER | SWP_NOACTIVATE);
                    return true;
                });
            bHandled = true;
        }
        else {
            SetWindowPos(nullptr, InsertAfterFlag::kHWND_DEFAULT,
                         rcNewWindow.left, rcNewWindow.top, rcNewWindow.Width(), rcNewWindow.Height(),
                         SWP_NOZORDER | SWP_NOACTIVATE);
        }
        if (!windowFlag.expired()) {
            float fRealDisplayScale = m_pOwner->OnNativeGetDpi().GetDisplayScaleFactor() / 100.0f;
            m_pOwner->OnNativeDisplayScaleChangedMsg(fRealDisplayScale, fNewPixelDensity);
        }
    }
    return 0;
}

LRESULT NativeWindow_Windows::OnWindowPosChangingMsg(UINT uMsg, WPARAM /*wParam*/, LPARAM lParam, bool& bHandled)
{
    ASSERT_UNUSED_VARIABLE(uMsg == WM_WINDOWPOSCHANGING);
    bHandled = false;
    if (IsWindowMaximized()) {
        //Maximized state
        LPWINDOWPOS lpPos = (LPWINDOWPOS)lParam;
        if (lpPos->flags & SWP_FRAMECHANGED) // The first maximize, not the WINDOWPOSCHANGE triggered after maximizing
        {
            POINT pt = { 0, 0 };
            HMONITOR hMontorPrimary = ::MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
            HMONITOR hMonitorTo = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY);

            // First maximize the window, then minimize it, then restore; at this time, the HMONITOR obtained by MonitorFromWindow is inaccurate
            // If the position from GetWindowRect is incorrect (the position info obtained when minimized is -38000), use the position in the normal state to get the HMONITOR
            UiRect rc;
            GetWindowRect(rc);
            if (rc.left < -10000 && rc.top < -10000 && rc.bottom < -10000 && rc.right < -10000) {
                WINDOWPLACEMENT wp = { sizeof(WINDOWPLACEMENT) };
                ::GetWindowPlacement(m_hWnd, &wp);
                hMonitorTo = ::MonitorFromRect(&wp.rcNormalPosition, MONITOR_DEFAULTTOPRIMARY);
            }
            if (hMonitorTo != hMontorPrimary) {
                // Solve the problem that borderless windows are not maximized correctly on dual monitors (when the secondary monitor resolution is larger than the primary monitor)
                MONITORINFO  miTo;
                ZeroMemory(&miTo, sizeof(miTo));
                miTo.cbSize = sizeof(MONITORINFO);
                ::GetMonitorInfo(hMonitorTo, &miTo);

                lpPos->x = miTo.rcWork.left;
                lpPos->y = miTo.rcWork.top;
                lpPos->cx = miTo.rcWork.right - miTo.rcWork.left;
                lpPos->cy = miTo.rcWork.bottom - miTo.rcWork.top;
            }
        }
    }
    return 0;
}

LRESULT NativeWindow_Windows::OnNotifyMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    ASSERT_UNUSED_VARIABLE(uMsg == WM_NOTIFY);
    bHandled = false;
    LPNMHDR lpNMHDR = (LPNMHDR)lParam;
    if (lpNMHDR != nullptr) {
        bHandled = true;
        return ::SendMessage(lpNMHDR->hwndFrom, OCM__BASE + uMsg, wParam, lParam);
    }
    return 0;
}

LRESULT NativeWindow_Windows::OnCommandMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    ASSERT_UNUSED_VARIABLE(uMsg == WM_COMMAND);
    bHandled = false;
    if (lParam == 0) {
        return 0;
    }
    HWND hWndChild = (HWND)lParam;
    bHandled = true;
    return ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
}

LRESULT NativeWindow_Windows::OnCtlColorMsgs(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    ASSERT_UNUSED_VARIABLE(uMsg == WM_CTLCOLOREDIT || uMsg == WM_CTLCOLORSTATIC);
    bHandled = false;
    // Refer To: http://msdn.microsoft.com/en-us/library/bb761691(v=vs.85).aspx
    // Read-only or disabled edit controls do not send the WM_CTLCOLOREDIT message; instead, they send the WM_CTLCOLORSTATIC message.
    if (lParam == 0) {
        return 0;
    }
    HWND hWndChild = (HWND)lParam;
    bHandled = true;
    return ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
}

LRESULT NativeWindow_Windows::OnTouchMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    ASSERT_UNUSED_VARIABLE(uMsg == WM_TOUCH);
    LRESULT lResult = 0;
    bHandled = false;
    unsigned int nNumInputs = LOWORD(wParam);
    if (nNumInputs < 1) {
        nNumInputs = 1;
    }
    TOUCHINPUT* pInputs = new TOUCHINPUT[nNumInputs];
    // Only care about the first touch position
    if (!GetTouchInputInfoWrapper((HTOUCHINPUT)lParam, nNumInputs, pInputs, sizeof(TOUCHINPUT))) {
        delete[] pInputs;
        return lResult;
    }
    else {
        CloseTouchInputHandleWrapper((HTOUCHINPUT)lParam);
        if (pInputs[0].dwID == 0) {
            return lResult;
        }
    }
    //Get the coordinates of the touch point and convert them to client area coordinates in the window
    UiPoint pt = { TOUCH_COORD_TO_PIXEL(pInputs[0].x) , TOUCH_COORD_TO_PIXEL(pInputs[0].y) };
    ScreenToClient(pt);

    DWORD dwFlags = pInputs[0].dwFlags;
    delete[] pInputs;
    pInputs = nullptr;

    if (dwFlags & TOUCHEVENTF_DOWN) {        
        lResult = m_pOwner->OnNativeMouseLButtonDownMsg(pt, 0, NativeMsg(WM_LBUTTONDOWN, 0, MAKELPARAM(pt.x, pt.y)), bHandled);
    }
    else if (dwFlags & TOUCHEVENTF_MOVE) {
        std::weak_ptr<WeakFlag> windowFlag = m_pOwner->GetWeakFlag();
        UiPoint lastMousePos = m_ptLastMousePos;
        lResult = m_pOwner->OnNativeMouseMoveMsg(pt, 0, false, NativeMsg(WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y)), bHandled);
        int wheelDelta = pt.y - lastMousePos.y;
        if ((wheelDelta != 0) && !windowFlag.expired()) {
            //Trigger the wheel function
            lResult = m_pOwner->OnNativeMouseWheelMsg(wheelDelta, pt, 0, NativeMsg(WM_MOUSEWHEEL, MAKEWPARAM(0, wheelDelta), MAKELPARAM(pt.x, pt.y)), bHandled);
        }
    }
    else if (dwFlags & TOUCHEVENTF_UP) {
        lResult = m_pOwner->OnNativeMouseLButtonUpMsg(pt, 0, NativeMsg(WM_LBUTTONUP, 0, MAKELPARAM(pt.x, pt.y)), bHandled);
    }
    return lResult;
}

LRESULT NativeWindow_Windows::OnPointerMsgs(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    ASSERT_UNUSED_VARIABLE( uMsg == WM_POINTERDOWN ||
                            uMsg == WM_POINTERUP ||
                            uMsg == WM_POINTERUPDATE ||
                            uMsg == WM_POINTERLEAVE ||
                            uMsg == WM_POINTERCAPTURECHANGED ||
                            uMsg == WM_POINTERWHEEL);

    LRESULT lResult = 0;
    bHandled = false;
    // Only care about the first touch point
    if (!IS_POINTER_PRIMARY_WPARAM(wParam)) {
        bHandled = true;
        return lResult;
    }
    //Get the pointer position and convert the screen coordinates to window client area coordinates
    UiPoint pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    ScreenToClient(pt);
    switch (uMsg)
    {
    case WM_POINTERDOWN:
        lResult = m_pOwner->OnNativeMouseLButtonDownMsg(pt, 0, NativeMsg(WM_LBUTTONDOWN, 0, MAKELPARAM(pt.x, pt.y)), bHandled);
        bHandled = true;
        break;
    case WM_POINTERUPDATE:
        lResult = m_pOwner->OnNativeMouseMoveMsg(pt, 0, false, NativeMsg(WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y)), bHandled);
        bHandled = true;
        break;
    case WM_POINTERUP:
        lResult = m_pOwner->OnNativeMouseLButtonUpMsg(pt, 0, NativeMsg(WM_LBUTTONUP, 0, MAKELPARAM(pt.x, pt.y)), bHandled);
        bHandled = true;
        break;
    case WM_POINTERWHEEL:
    {
        int32_t wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        lResult = m_pOwner->OnNativeMouseWheelMsg(wheelDelta, pt, 0, NativeMsg(WM_MOUSEWHEEL, MAKEWPARAM(0, wheelDelta), MAKELPARAM(pt.x, pt.y)), bHandled);
        bHandled = true;
    }
    break;
    case WM_POINTERLEAVE:
        lResult = m_pOwner->OnNativeMouseLeaveMsg(NativeMsg(WM_MOUSELEAVE, 0, 0), bHandled);
        break;
    case WM_POINTERCAPTURECHANGED:
        lResult = m_pOwner->OnNativeCaptureChangedMsg(NativeMsg(WM_CAPTURECHANGED, 0, 0), bHandled);
        //If bHandled is not set, the program will convert it to a WM_BUTTON class message
        bHandled = true;
        break;
    default:
        break;
    }
    return 0;
}

void NativeWindow_Windows::CheckWindowSnap(HWND hWnd)
{
    if (::IsZoomed(hWnd) || ::IsIconic(hWnd) || IsChildWindow()) {
        //When maximized/minimized/child window, do not process
        return;
    }
    // Get the monitor information where the window is located
    HMONITOR hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { 0, };
    mi.cbSize = sizeof(MONITORINFO);
    ::GetMonitorInfo(hMonitor, &mi);

    // DPI adaptation
    UINT dpi = 96;
    GetDpiForWindowWrapper(hWnd, dpi);
    const int32_t snapThreshold = MulDiv(3, dpi, 96);

    // Get the actual coordinates of the window
    RECT rcWindow = { 0 };
    ::GetWindowRect(hWnd, &rcWindow);

    // Snap detection logic
    bool bLeftSnap = (std::abs(rcWindow.left - mi.rcWork.left) < snapThreshold);
    bool bRightSnap = (std::abs(rcWindow.right - mi.rcWork.right) < snapThreshold);
    bool bTopSnap = (std::abs(rcWindow.top - mi.rcWork.top) < snapThreshold);
    bool bBottomSnap = (std::abs(rcWindow.bottom - mi.rcWork.bottom) < snapThreshold);

    m_pOwner->OnNativeWindowPosSnapped(bLeftSnap, bRightSnap, bTopSnap, bBottomSnap);
}

LRESULT NativeWindow_Windows::ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    LRESULT lResult = 0;
    bHandled = false;
    switch (uMsg)
    {
    case WM_WINDOWPOSCHANGED:
    {
        CheckWindowSnap(m_hWnd);
        static bool bDragWindowContentsEnabled = IsDragWindowContentsEnabled(); //Read only once to avoid affecting performance
        if (!bDragWindowContentsEnabled) {
            // If the "Show window contents while dragging" option is not enabled, the WM_MOVE message will not be sent during window movement,
            // the entire window needs to be redrawn to avoid the problem of the window not drawing when it moves from outside the screen into the screen
            UiRect rc;
            GetClientRect(rc);
            Invalidate(rc);
        }
        lResult = m_pOwner->OnNativeWindowPosChangedMsg(NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_SIZE:
    {        
        WindowSizeType sizeType = static_cast<WindowSizeType>(wParam);
        UiSize newWindowSize;
        newWindowSize.cx = (int)(short)LOWORD(lParam);
        newWindowSize.cy = (int)(short)HIWORD(lParam);
        lResult = m_pOwner->OnNativeSizeMsg(sizeType, newWindowSize, NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_MOVE:
    {
        UiPoint ptTopLeft;
        ptTopLeft.x = (int)(short)LOWORD(lParam);   // horizontal position 
        ptTopLeft.y = (int)(short)HIWORD(lParam);   // vertical position 
        lResult = m_pOwner->OnNativeMoveMsg(ptTopLeft, NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_SHOWWINDOW:
    {
        bool bShow = wParam != FALSE;
        lResult = m_pOwner->OnNativeShowWindowMsg(bShow, NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_PAINT:
    {
        lResult = OnPaintMsg(uMsg, wParam, lParam, bHandled);
        break;
    }
    case WM_SETFOCUS:
    {
        INativeWindow* pLostFocusWindow = nullptr;
        HWND hWnd = (HWND)wParam;
        if ((hWnd != nullptr) && ::IsWindow(hWnd)) {
            NativeWindow_Windows* pThis = reinterpret_cast<NativeWindow_Windows*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
            //Verify whether they are consistent
            if (pThis != nullptr) {
                if ((NativeWindow_Windows*)::GetPropW(hWnd, sPropName) == pThis) {
                    pLostFocusWindow = pThis->m_pOwner;
                }
            }
        }
        lResult = m_pOwner->OnNativeSetFocusMsg(pLostFocusWindow, NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_KILLFOCUS:
    {
        INativeWindow* pSetFocusWindow = nullptr;
        HWND hWnd = (HWND)wParam;
        if ((hWnd != nullptr) && ::IsWindow(hWnd)) {
            NativeWindow_Windows* pThis = reinterpret_cast<NativeWindow_Windows*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
            //Verify whether they are consistent
            if (pThis != nullptr) {
                if ((NativeWindow_Windows*)::GetPropW(hWnd, sPropName) == pThis) {
                    pSetFocusWindow = pThis->m_pOwner;
                }
            }
        }
        lResult = m_pOwner->OnNativeKillFocusMsg(pSetFocusWindow, NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_IME_SETCONTEXT:
    {
        lResult = m_pOwner->OnNativeImeSetContextMsg(NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_IME_STARTCOMPOSITION:
    {
        lResult = m_pOwner->OnNativeImeStartCompositionMsg(NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_IME_COMPOSITION:
    {
        lResult = m_pOwner->OnNativeImeCompositionMsg(NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_IME_ENDCOMPOSITION:
    {
        lResult = m_pOwner->OnNativeImeEndCompositionMsg(NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_SETCURSOR:
    {
        if (LOWORD(lParam) == HTCLIENT) {
            //Only handle setting the cursor of the client area
            lResult = m_pOwner->OnNativeSetCursorMsg(NativeMsg(uMsg, wParam, lParam), bHandled);
        }
        break;
    }
    case WM_CONTEXTMENU:
    {
        UiPoint pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        if ((pt.x != -1) && (pt.y != -1)) {
            ScreenToClient(pt);
        }
        lResult = m_pOwner->OnNativeContextMenuMsg(pt, NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    {
        VirtualKeyCode vkCode = static_cast<VirtualKeyCode>(wParam);
        uint32_t modifierKey = 0;
        GetModifiers(uMsg, wParam, lParam, modifierKey);
        lResult = m_pOwner->OnNativeKeyDownMsg(vkCode, modifierKey, NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        VirtualKeyCode vkCode = static_cast<VirtualKeyCode>(wParam);
        uint32_t modifierKey = 0;
        GetModifiers(uMsg, wParam, lParam, modifierKey);
        lResult = m_pOwner->OnNativeKeyUpMsg(vkCode, modifierKey, NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_CHAR:
    case WM_SYSCHAR:
    {
        VirtualKeyCode vkCode = static_cast<VirtualKeyCode>(wParam);
        uint32_t modifierKey = 0;
        GetModifiers(uMsg, wParam, lParam, modifierKey);
        lResult = m_pOwner->OnNativeCharMsg(vkCode, modifierKey, NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_HOTKEY:
    {
        int32_t hotkeyId = (int32_t)wParam;
        VirtualKeyCode vkCode = static_cast<VirtualKeyCode>((int32_t)(int16_t)HIWORD(lParam));
        uint32_t modifierKey = 0;
        GetModifiers(uMsg, wParam, lParam, modifierKey);
        lResult = m_pOwner->OnNativeHotKeyMsg(hotkeyId, vkCode, modifierKey, NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_MOUSEWHEEL:
    {
        int32_t wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        UiPoint pt;
        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);
        ScreenToClient(pt);
        uint32_t modifierKey = 0;
        GetModifiers(uMsg, wParam, lParam, modifierKey);
        lResult = m_pOwner->OnNativeMouseWheelMsg(wheelDelta, pt, modifierKey, NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_MOUSEMOVE:
    {
        UiPoint pt;
        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);
        uint32_t modifierKey = 0;
        GetModifiers(uMsg, wParam, lParam, modifierKey);
        lResult = m_pOwner->OnNativeMouseMoveMsg(pt, modifierKey, false, NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_MOUSEHOVER:
    {
        UiPoint pt;
        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);
        uint32_t modifierKey = 0;
        GetModifiers(uMsg, wParam, lParam, modifierKey);
        lResult = m_pOwner->OnNativeMouseHoverMsg(pt, modifierKey, NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_MOUSELEAVE:
    {
        lResult = m_pOwner->OnNativeMouseLeaveMsg(NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_LBUTTONDOWN:
    {
        m_bNCLButtonDownOnMaxButton = false;
        UiPoint pt;
        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);
        uint32_t modifierKey = 0;
        GetModifiers(uMsg, wParam, lParam, modifierKey);
        lResult = m_pOwner->OnNativeMouseLButtonDownMsg(pt, modifierKey, NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_LBUTTONUP:
    {
        UiPoint pt;
        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);
        uint32_t modifierKey = 0;
        GetModifiers(uMsg, wParam, lParam, modifierKey);
        lResult = m_pOwner->OnNativeMouseLButtonUpMsg(pt, modifierKey, NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_LBUTTONDBLCLK:
    {
        UiPoint pt;
        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);
        uint32_t modifierKey = 0;
        GetModifiers(uMsg, wParam, lParam, modifierKey);
        lResult = m_pOwner->OnNativeMouseLButtonDbClickMsg(pt, modifierKey, NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_RBUTTONDOWN:
    {
        m_bNCLButtonDownOnMaxButton = false;
        UiPoint pt;
        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);
        uint32_t modifierKey = 0;
        GetModifiers(uMsg, wParam, lParam, modifierKey);
        lResult = m_pOwner->OnNativeMouseRButtonDownMsg(pt, modifierKey, NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_RBUTTONUP:
    {
        UiPoint pt;
        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);
        uint32_t modifierKey = 0;
        GetModifiers(uMsg, wParam, lParam, modifierKey);
        lResult = m_pOwner->OnNativeMouseRButtonUpMsg(pt, modifierKey, NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_RBUTTONDBLCLK:
    {
        UiPoint pt;
        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);
        uint32_t modifierKey = 0;
        GetModifiers(uMsg, wParam, lParam, modifierKey);
        lResult = m_pOwner->OnNativeMouseRButtonDbClickMsg(pt, modifierKey, NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_MBUTTONDOWN:
    {
        UiPoint pt;
        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);
        uint32_t modifierKey = 0;
        GetModifiers(uMsg, wParam, lParam, modifierKey);
        lResult = m_pOwner->OnNativeMouseMButtonDownMsg(pt, modifierKey, NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_MBUTTONUP:
    {
        UiPoint pt;
        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);
        uint32_t modifierKey = 0;
        GetModifiers(uMsg, wParam, lParam, modifierKey);
        lResult = m_pOwner->OnNativeMouseMButtonUpMsg(pt, modifierKey, NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_MBUTTONDBLCLK:
    {
        UiPoint pt;
        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);
        uint32_t modifierKey = 0;
        GetModifiers(uMsg, wParam, lParam, modifierKey);
        lResult = m_pOwner->OnNativeMouseMButtonDbClickMsg(pt, modifierKey, NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_CAPTURECHANGED:
    {
        HWND hWnd = m_hWnd;
        lResult = m_pOwner->OnNativeCaptureChangedMsg(NativeMsg(uMsg, wParam, lParam), bHandled);
        if (::IsWindow(hWnd) && m_bNCLButtonDownOnMaxButton && !IsUseSystemCaption()) {
            //The mouse capture needs to be released, otherwise clicking the maximize button will not respond properly
            if (::GetCapture() == m_hWnd) {
                m_bNCLButtonDownOnMaxButton = false;
                ::ReleaseCapture();
            }
        }
        break;
    }
    case WM_CLOSE:
    {
        lResult = m_pOwner->OnNativeWindowCloseMsg((uint32_t)wParam, NativeMsg(uMsg, wParam, lParam), bHandled);
        break;
    }
    case WM_NCMOUSEMOVE:
    {
        if (!IsUseSystemCaption()) {
            UiPoint pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            ScreenToClient(pt);
            uint32_t modifierKey = 0;
            lResult = m_pOwner->OnNativeMouseMoveMsg(pt, modifierKey, true, NativeMsg(uMsg, wParam, lParam), bHandled);
        }
        break;
    }
    case WM_NCLBUTTONDOWN:
    {
        if (!IsUseSystemCaption() && (wParam == HTMAXBUTTON)) {
            //If the mouse is clicked on the maximize button, intercept this message to prevent Windows from also triggering the maximize/restore command
            m_bNCLButtonDownOnMaxButton = true;
            bHandled = true; 
            UiPoint pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            ScreenToClient(pt);
            uint32_t modifierKey = 0;
            lResult = m_pOwner->OnNativeMouseLButtonDownMsg(pt, modifierKey, NativeMsg(uMsg, wParam, lParam), bHandled);
        }
        else if (!IsUseSystemCaption() && (wParam == HTSYSMENU) && IsEnableSysMenu()) {
            //The mouse is clicked at the window menu position; start the timer to show the system window menu with a delay
            StopSysMenuTimer();            
            m_nSysMenuTimerId = ::SetTimer(m_hWnd, UI_SYS_MEMU_TIMER_ID, 300, nullptr);
        }
        break;
    }
    case WM_NCLBUTTONUP:
    {
        m_bNCLButtonDownOnMaxButton = false;
        if (!IsUseSystemCaption() && (wParam == HTMAXBUTTON)) {
            //If the mouse is clicked on the maximize button, intercept this message to prevent Windows from also triggering the maximize/restore command
            bHandled = true;

            UiPoint pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            ScreenToClient(pt);
            uint32_t modifierKey = 0;
            lResult = m_pOwner->OnNativeMouseLButtonUpMsg(pt, modifierKey, NativeMsg(uMsg, wParam, lParam), bHandled);
        }
        break;
    }
    case WM_NCRBUTTONUP:
    {
        m_bNCLButtonDownOnMaxButton = false;
        bool bEnable = (wParam == HTCAPTION) || (wParam == HTMAXBUTTON) || (wParam == HTSYSMENU);
        if (bEnable && IsEnableSysMenu() && !IsUseSystemCaption()) {
            // Show the system menu
            POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            if (ShowWindowSysMenu(m_hWnd, pt)) {
                bHandled = true;
            }
        }
        break;
    }
    default:
        break;
    }//end of switch
    return lResult;
}

void NativeWindow_Windows::StopSysMenuTimer()
{
    if (m_nSysMenuTimerId != 0) {
        ::KillTimer(m_hWnd, m_nSysMenuTimerId);
        m_nSysMenuTimerId = 0;
    }
}

bool NativeWindow_Windows::ShowWindowSysMenu(HWND hWnd, const POINT& pt) const
{
    HMENU hSysMenu = ::GetSystemMenu(hWnd, FALSE);
    if (hSysMenu == nullptr) {
        return false;
    }
    //Update the menu state
    MENUITEMINFO mii;
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_STATE;
    mii.fType = 0;

    // update the options
    mii.fState = MF_ENABLED;
    SetMenuItemInfo(hSysMenu, SC_RESTORE, FALSE, &mii);
    SetMenuItemInfo(hSysMenu, SC_SIZE, FALSE, &mii);
    SetMenuItemInfo(hSysMenu, SC_MOVE, FALSE, &mii);
    SetMenuItemInfo(hSysMenu, SC_MAXIMIZE, FALSE, &mii);
    SetMenuItemInfo(hSysMenu, SC_MINIMIZE, FALSE, &mii);

    mii.fState = MF_GRAYED;

    WINDOWPLACEMENT wp = { 0, };
    ::GetWindowPlacement(hWnd, &wp);

    switch (wp.showCmd)
    {
    case SW_SHOWMAXIMIZED:
        SetMenuItemInfo(hSysMenu, SC_SIZE, FALSE, &mii);
        SetMenuItemInfo(hSysMenu, SC_MOVE, FALSE, &mii);
        SetMenuItemInfo(hSysMenu, SC_MAXIMIZE, FALSE, &mii);
        SetMenuDefaultItem(hSysMenu, SC_CLOSE, FALSE);
        break;
    case SW_SHOWMINIMIZED:
        SetMenuItemInfo(hSysMenu, SC_MINIMIZE, FALSE, &mii);
        SetMenuDefaultItem(hSysMenu, SC_RESTORE, FALSE);
        break;
    case SW_SHOWNORMAL:
        SetMenuItemInfo(hSysMenu, SC_RESTORE, FALSE, &mii);
        SetMenuDefaultItem(hSysMenu, SC_CLOSE, FALSE);
        break;
    default:
        break;
    }

    UiRect rcSizeBox = m_pOwner->OnNativeGetSizeBox();
    if ((rcSizeBox.left <= 0) && (rcSizeBox.top <= 0) && (rcSizeBox.right <= 0) && (rcSizeBox.bottom <= 0)) {
        //Disable resizing
        SetMenuItemInfo(hSysMenu, SC_SIZE, FALSE, &mii);
    }

    UINT wndStyleValue = (UINT)::GetWindowLong(GetHWND(), GWL_STYLE);
    if (!(wndStyleValue & WS_MINIMIZEBOX)) {
        //Disable minimizing
        SetMenuItemInfo(hSysMenu, SC_MINIMIZE, FALSE, &mii);
    }
    if (!(wndStyleValue & WS_MAXIMIZEBOX)) {
        //Disable maximizing and restoring
        SetMenuItemInfo(hSysMenu, SC_MAXIMIZE, FALSE, &mii);
        SetMenuItemInfo(hSysMenu, SC_RESTORE, FALSE, &mii);
    }

    // Show the system menu at the click position
    int32_t nRet = ::TrackPopupMenu(hSysMenu, TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, 0, hWnd, nullptr);
    if (nRet != 0) {
        ::PostMessage(hWnd, WM_SYSCOMMAND, nRet, 0);
    }
    return true;
}

void NativeWindow_Windows::OnFinalMessage()
{
    if (m_pOwner) {
        m_pOwner->OnNativeFinalMessage();
    }
}

HWND NativeWindow_Windows::GetWindowOwner() const
{
    return ::GetWindow(m_hWnd, GW_OWNER);
}

void NativeWindow_Windows::SetEnableSnapLayoutMenu(bool bEnable)
{
    //Only supported on Windows 11
    if (UiIsWindows11OrGreater()) {
        m_bSnapLayoutMenu = bEnable;
    }
}

bool NativeWindow_Windows::IsEnableSnapLayoutMenu() const
{
    return m_bSnapLayoutMenu;
}

void NativeWindow_Windows::SetEnableSysMenu(bool bEnable)
{
    m_bEnableSysMenu = bEnable;
}

bool NativeWindow_Windows::IsEnableSysMenu() const
{
    return m_bEnableSysMenu;
}

void NativeWindow_Windows::SetImeOpenStatus(bool bOpen)
{
    if (!bOpen) {
        //Disable the input method
        EnableIME(m_hWnd, false);
    }
    else {
        //Enable the input method
        EnableIME(m_hWnd, true);
    }
}

void NativeWindow_Windows::EnableIME(HWND hwnd, bool bEnable)
{
    ASSERT(::IsWindow(hwnd));
    if (!::IsWindow(hwnd)) {
        return;
    }
    if (!bEnable) {
        //Disable
        if (m_hImc == nullptr) {
            m_hImc = ::ImmAssociateContext(hwnd, nullptr);
            ASSERT(m_hImc != nullptr);
        }
    }
    else {
        //Enable
        if (m_hImc != nullptr) {
            HIMC hImc = ::ImmAssociateContext(hwnd, m_hImc);
            m_hImc = nullptr;
            ASSERT_UNUSED_VARIABLE(hImc == nullptr);
        }
        else {
            //Check whether the input method is open and provide an assertion
            HIMC hImc = ::ImmGetContext(hwnd);
            ASSERT(hImc != nullptr);
            if (hImc != nullptr) {                
                if (!::ImmGetOpenStatus(hImc)) {
                    ::ImmSetOpenStatus(hImc, TRUE);
                }
                ASSERT(::ImmGetOpenStatus(hImc));
                ::ImmReleaseContext(hwnd, hImc);
            }
        }
    }
}

void NativeWindow_Windows::SetTextInputArea(const UiRect* /*rect*/, int32_t /*nCursor*/)
{
}

void NativeWindow_Windows::SetEnableDragDrop(bool bEnable)
{
    m_bEnableDragDrop = bEnable;
    if (bEnable && IsWindow()) {
        if (m_pWindowDropTarget == nullptr) {
            m_pWindowDropTarget = new WindowDropTarget(this);
            m_pWindowDropTarget->AddRef();
            m_pWindowDropTarget->RegisterDragDrop();
        }
    }
    else {
        if (m_pWindowDropTarget != nullptr) {
            m_pWindowDropTarget->UnregisterDragDrop();
            m_pWindowDropTarget->Release();
            m_pWindowDropTarget = nullptr;
        }
    }
}

bool NativeWindow_Windows::IsEnableDragDrop() const
{
    return m_bEnableDragDrop;
}

Control* NativeWindow_Windows::FindControl(const UiPoint& pt) const
{
    return m_pOwner->OnNativeFindControl(pt);
}


bool NativeWindow_Windows::NeedCenterWindowAfterCreated() const
{
    return m_createParam.m_bCenterWindow;
}

HRESULT NativeWindow_Windows::OnDragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect, bool& bHandled)
{
    if (pDataObj == nullptr) {
        return S_FALSE;
    }
    ControlDropData_Windows data;
    data.m_pDataObj = pDataObj;
    data.m_grfKeyState = grfKeyState;
    UiPoint ptClient;
    ptClient.x = pt.x;
    ptClient.y = pt.y;
    ScreenToClient(ptClient);
    data.m_ptClientX = ptClient.x;
    data.m_ptClientY = ptClient.y;
    data.m_dwEffect = (pdwEffect != nullptr) ? *pdwEffect : 0;
    data.m_hResult = S_OK;
    data.m_bHandled = false;

    m_textList.clear();
    m_fileList.clear();
    m_pDataObj = pDataObj;

    ControlDropTargetImpl_Windows::ParseWindowsDataObject(pDataObj, m_textList, m_fileList);
    data.m_textList = m_textList;
    data.m_fileList = m_fileList;

    m_pOwner->OnNativeDropEnterMsg(kControlDropTypeWindows, &data);
    bHandled = data.m_bHandled;
    if (pdwEffect != nullptr) {
        *pdwEffect = data.m_dwEffect;
    }
    return data.m_hResult;
}

HRESULT NativeWindow_Windows::OnDragOver(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect, bool& bHandled)
{
    if ((pDataObj == nullptr) || (m_pDataObj != pDataObj)) {
        return S_FALSE;
    }
    ControlDropData_Windows data;
    data.m_pDataObj = pDataObj;
    data.m_grfKeyState = grfKeyState;
    UiPoint ptClient;
    ptClient.x = pt.x;
    ptClient.y = pt.y;
    ScreenToClient(ptClient);
    data.m_ptClientX = ptClient.x;
    data.m_ptClientY = ptClient.y;
    data.m_dwEffect = (pdwEffect != nullptr) ? *pdwEffect : 0;
    data.m_hResult = S_OK;
    data.m_bHandled = false;
    data.m_textList = m_textList;
    data.m_fileList = m_fileList;

    m_pOwner->OnNativeDropOverMsg(kControlDropTypeWindows, &data);
    bHandled = data.m_bHandled;
    if (pdwEffect != nullptr) {
        *pdwEffect = data.m_dwEffect;
    }
    return data.m_hResult;
}

HRESULT NativeWindow_Windows::OnDragLeave()
{
    m_pDataObj = nullptr;
    m_textList.clear();
    m_fileList.clear();
    m_pOwner->OnNativeDropLeaveMsg();
    return S_OK;
}

HRESULT NativeWindow_Windows::OnDrop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect, bool& bHandled)
{
    if ((pDataObj == nullptr) || (m_pDataObj != pDataObj)) {
        return S_FALSE;
    }
    ControlDropData_Windows data;
    data.m_pDataObj = pDataObj;
    data.m_grfKeyState = grfKeyState;
    UiPoint ptClient;
    ptClient.x = pt.x;
    ptClient.y = pt.y;
    ScreenToClient(ptClient);
    data.m_ptClientX = ptClient.x;
    data.m_ptClientY = ptClient.y;
    data.m_dwEffect = (pdwEffect != nullptr) ? *pdwEffect : 0;
    data.m_hResult = S_OK;
    data.m_bHandled = false;
    data.m_textList = m_textList;
    data.m_fileList = m_fileList;

    m_pOwner->OnNativeDropMsg(kControlDropTypeWindows, &data);
    bHandled = data.m_bHandled;
    if (pdwEffect != nullptr) {
        *pdwEffect = data.m_dwEffect;
    }
    return data.m_hResult;
}

} // namespace ui

#endif //DUILIB_BUILD_FOR_WIN
