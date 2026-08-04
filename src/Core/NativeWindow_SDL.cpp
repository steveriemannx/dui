#include "dui/Core/NativeWindow_SDL.h"
#include "dui/Core/MessageLoop_SDL.h"
#include "dui/Core/WindowDropTarget_SDL.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Utils/StringUtil.h"
#include "dui/Utils/StringConvert.h"
#include "dui/Utils/FileUtil.h"
#include "dui/Utils/PerformanceUtil.h"

#ifdef DUI_BUILD_FOR_SDL

#include <SDL3/SDL.h>

#if defined DUI_BUILD_FOR_MACOS
    #include "dui/Core/SDL_MacOS.h"
#elif defined (DUI_BUILD_FOR_LINUX) || defined (DUI_BUILD_FOR_FREEBSD)
    #include "dui/Core/SDL_Linux.h"
#endif

/** Actively paint the window
*/
#define WM_USER_PAINT_MSG (SDL_EVENT_USER + 3)

/** Actively trigger the window's Hover message
*/
#define WM_USER_HOVER_MSG (SDL_EVENT_USER + 4)

namespace ui {

//Mapping between the window pointer and the SDL window ID, used to relay messages
std::unordered_map<SDL_WindowID, NativeWindow_SDL*> NativeWindow_SDL::s_windowIDMap;

void NativeWindow_SDL::SetWindowFromID(SDL_WindowID id, NativeWindow_SDL* pNativeWindow)
{
    ASSERT(id != 0);
    if (id == 0) {
        return;
    }
    ASSERT(pNativeWindow != nullptr);
    if (pNativeWindow == nullptr) {
        return;
    }
    ASSERT(pNativeWindow->m_sdlWindow != nullptr);
    if (pNativeWindow->m_sdlWindow == nullptr) {
        return;
    }
    ASSERT(SDL_GetWindowFromID(id) == pNativeWindow->m_sdlWindow);
    if (SDL_GetWindowFromID(id) != pNativeWindow->m_sdlWindow) {
        return;
    }
    s_windowIDMap[id] = pNativeWindow;
}

void NativeWindow_SDL::ClearWindowFromID(SDL_WindowID id, NativeWindow_SDL* pNativeWindow)
{
    ASSERT(id != 0);
    NativeWindow_SDL* pOldNativeWindow = nullptr;
    auto iter = s_windowIDMap.find(id);
    if (iter != s_windowIDMap.end()) {
        pOldNativeWindow = iter->second;
        s_windowIDMap.erase(iter);
    }
    if (pNativeWindow != nullptr) {
        ASSERT_UNUSED_VARIABLE(pNativeWindow == pOldNativeWindow);
    }
}

uint32_t NativeWindow_SDL::GetModifiers(SDL_Keymod keymod)
{
    uint32_t modifierKey = 0;
    if (keymod & SDL_KMOD_CTRL) {
        modifierKey |= ModifierKey::kControl;
    }
    if (keymod & SDL_KMOD_SHIFT) {
        modifierKey |= ModifierKey::kShift;
    }
    if (keymod & SDL_KMOD_ALT) {
        modifierKey |= ModifierKey::kAlt;
    }
    if (keymod & SDL_KMOD_GUI) {
        modifierKey |= ModifierKey::kWin;
    }
    return modifierKey;
}

NativeWindow_SDL* NativeWindow_SDL::GetWindowFromID(SDL_WindowID id)
{
    ASSERT(id != 0);
    auto iter = s_windowIDMap.find(id);
    if (iter != s_windowIDMap.end()) {
        return iter->second;
    }
    return nullptr;
}

uint32_t NativeWindow_SDL::GetHoverMsgId()
{
    return WM_USER_HOVER_MSG;
}

SDL_WindowID NativeWindow_SDL::GetWindowIdFromEvent(const SDL_Event& sdlEvent)
{
    SDL_Window* sdlWindow = SDL_GetWindowFromEvent(&sdlEvent);
    if (sdlWindow != nullptr) {
        //Prefer to use the SDL API to get the window ID
        return SDL_GetWindowID(sdlWindow);
    }

    if ((sdlEvent.type >= SDL_EVENT_WINDOW_FIRST) && (sdlEvent.type <= SDL_EVENT_WINDOW_LAST)) {
        //Window event
        return sdlEvent.window.windowID;
    }

    SDL_WindowID windowID = 0;
    switch (sdlEvent.type) {
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
        //Keyboard event
        windowID = sdlEvent.key.windowID;
        break;
    case SDL_EVENT_TEXT_EDITING:
        //Keyboard event
        windowID = sdlEvent.edit.windowID;
        break;
    case SDL_EVENT_TEXT_INPUT:
        //Keyboard event
        windowID = sdlEvent.text.windowID;
        break;
    case SDL_EVENT_MOUSE_MOTION:
        //Mouse event
        windowID = sdlEvent.motion.windowID;
        break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
        //Mouse event
        windowID = sdlEvent.button.windowID;
        break;
    case SDL_EVENT_MOUSE_WHEEL:
        //Mouse event
        windowID = sdlEvent.wheel.windowID;
        break;
    case SDL_EVENT_DROP_BEGIN:
    case SDL_EVENT_DROP_POSITION:
    case SDL_EVENT_DROP_TEXT:
    case SDL_EVENT_DROP_FILE:
    case SDL_EVENT_DROP_COMPLETE:
        //Drag-and-drop event
        windowID = sdlEvent.drop.windowID;
        break;
    default:
        if ((sdlEvent.type > SDL_EVENT_USER) && (sdlEvent.type < SDL_EVENT_LAST)) {
            //User-defined message
            windowID = sdlEvent.user.windowID;
        }
        break;
    }

    return windowID;
}

/** Drawing helper class
*/
class NativeWindowRenderPaint :
    public IRenderPaint
{
public:
    NativeWindow_SDL* m_pNativeWindow = nullptr;
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
            m_pOwner->OnNativePaintMsg(rcPaint, m_nativeMsg, m_bHandled);
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

    /** Get the area that the UI needs to draw, to implement partial drawing
    * @param [out] rcUpdate Returns the rectangular range of the area to be drawn
    * @return Returns true if partial drawing is supported, returns false if partial drawing is not supported
    */
    virtual bool GetUpdateRect(UiRect& rcUpdate) const override
    {
        rcUpdate = m_pNativeWindow->GetUpdateRect();
        return !rcUpdate.IsEmpty();
    }
};

void NativeWindow_SDL::CheckWindowSnap(SDL_Window* window)
{
    // Check whether the window is maximized or minimized
    if ((window == nullptr) || IsChildWindow()) {
        return;
    }
    SDL_WindowFlags flags = SDL_GetWindowFlags(window);
    if ((flags & SDL_WINDOW_MAXIMIZED) || (flags & SDL_WINDOW_MINIMIZED) || (flags & SDL_WINDOW_FULLSCREEN) ) {
        return;
    }

    // Get the display information of the monitor where the window is located
    SDL_DisplayID displayID = SDL_GetDisplayForWindow(window);
    SDL_Rect displayBounds;
    SDL_GetDisplayUsableBounds(displayID, &displayBounds);

    // Get DPI information
    float fDisplayContentScale = SDL_GetDisplayContentScale(displayID);
    if (fDisplayContentScale < 1.0f) {
        fDisplayContentScale = 1.0f;
    }
    const int snapThreshold = static_cast<int>(3 * fDisplayContentScale + 0.5f);

    // Get the window position and size
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    if (!SDL_GetWindowPosition(window, &x, &y)) {
        return;
    }
    if (!SDL_GetWindowSize(window, &w, &h)) {
        return;
    }

    // Snap detection logic
    bool bLeftSnap = (std::abs(x - displayBounds.x) < snapThreshold);
    bool bRightSnap = (std::abs((x + w) - (displayBounds.x + displayBounds.w)) < snapThreshold);
    bool bTopSnap = (std::abs(y - displayBounds.y) < snapThreshold);
    bool bBottomSnap = (std::abs((y + h) - (displayBounds.y + displayBounds.h)) < snapThreshold);

    // Call the callback function to notify the snap state
    if (m_pOwner != nullptr) {
        m_pOwner->OnNativeWindowPosSnapped(bLeftSnap, bRightSnap, bTopSnap, bBottomSnap);
    }
}

bool NativeWindow_SDL::OnSDLWindowEvent(const SDL_Event& sdlEvent)
{
    INativeWindow* pOwner = m_pOwner;
    ASSERT(pOwner != nullptr);
    if (pOwner == nullptr) {
        return false;
    }
    //Interface lifetime flag
    std::weak_ptr<WeakFlag> ownerFlag = pOwner->GetWeakFlag();

    //Messages are first forwarded to the filter (all messages)
    bool bHandled = false;
    LRESULT lResult = pOwner->OnNativeWindowMessage(sdlEvent.type, (WPARAM)&sdlEvent, 0, bHandled);
    if (bHandled || ownerFlag.expired()) {
        return true;
    }

    if (sdlEvent.type != SDL_EVENT_WINDOW_DESTROYED) {
        if (m_sdlWindow == nullptr) {
            return false;
        }
    }

    //Dispatch messages to the handler functions
    switch (sdlEvent.type) {
    case SDL_EVENT_WINDOW_SHOWN:
        {
            lResult = pOwner->OnNativeShowWindowMsg(true, NativeMsg(SDL_EVENT_WINDOW_SHOWN, 0, 0), bHandled);
        }
        break;
    case SDL_EVENT_WINDOW_HIDDEN:
        {
            lResult = pOwner->OnNativeShowWindowMsg(false, NativeMsg(SDL_EVENT_WINDOW_HIDDEN, 0, 0), bHandled);
        }
        break;
    case SDL_EVENT_WINDOW_RESIZED:
        {
            //The window size has changed
            WindowSizeType sizeType = WindowSizeType::kSIZE_RESTORED;
            SDL_WindowFlags nFlags = SDL_GetWindowFlags(m_sdlWindow);
            if (nFlags & SDL_WINDOW_FULLSCREEN) {
                sizeType = WindowSizeType::kSIZE_MAXIMIZED;
            }
            else if (nFlags & SDL_WINDOW_MAXIMIZED) {
                sizeType = WindowSizeType::kSIZE_MAXIMIZED;
            }
            else if (nFlags & SDL_WINDOW_MINIMIZED) {
                sizeType = WindowSizeType::kSIZE_MINIMIZED;
            }
            UiSize newWindowSize;
            newWindowSize.cx = sdlEvent.window.data1;
            newWindowSize.cy = sdlEvent.window.data2;
            lResult = pOwner->OnNativeSizeMsg(sizeType, newWindowSize, NativeMsg(SDL_EVENT_WINDOW_RESIZED, 0, 0), bHandled);
            if (!ownerFlag.expired()) {
                CheckWindowSnap(m_sdlWindow);
            }            
        }
        break;
    case SDL_EVENT_WINDOW_MINIMIZED:
        {
            //The window is minimized
            WindowSizeType sizeType = WindowSizeType::kSIZE_MINIMIZED;
            UiSize newWindowSize;
            SDL_GetWindowSize(m_sdlWindow, &newWindowSize.cx, &newWindowSize.cy);            
            lResult = pOwner->OnNativeSizeMsg(sizeType, newWindowSize, NativeMsg(SDL_EVENT_WINDOW_MINIMIZED, 0, 0), bHandled);
        }
        break;
    case SDL_EVENT_WINDOW_MAXIMIZED:
        {
            //The window is maximized
            WindowSizeType sizeType = WindowSizeType::kSIZE_MAXIMIZED;
            UiSize newWindowSize;
            SDL_GetWindowSize(m_sdlWindow, &newWindowSize.cx, &newWindowSize.cy);
            lResult = pOwner->OnNativeSizeMsg(sizeType, newWindowSize, NativeMsg(SDL_EVENT_WINDOW_MAXIMIZED, 0, 0), bHandled);
        }
        break;
    case SDL_EVENT_WINDOW_RESTORED:
        {
            //The window is restored
            WindowSizeType sizeType = WindowSizeType::kSIZE_RESTORED;
            UiSize newWindowSize;
            SDL_GetWindowSize(m_sdlWindow, &newWindowSize.cx, &newWindowSize.cy);
            lResult = pOwner->OnNativeSizeMsg(sizeType, newWindowSize, NativeMsg(SDL_EVENT_WINDOW_RESTORED, 0, 0), bHandled);
        }
        break;
    case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
        {
            //The DPI of the window display has changed, triggering the DPI change event
            float fNewDisplayScale = SDL_GetWindowDisplayScale(m_sdlWindow);
            float fNewPixelDensity = SDL_GetWindowPixelDensity(m_sdlWindow);
#ifdef DUI_HDPI_TEST_PIXEL_DENSITY
            //TEST ONLY
            fNewPixelDensity = DUI_HDPI_TEST_PIXEL_DENSITY;
#endif
            uint32_t nOldDisplayScale = pOwner->OnNativeGetDpi().GetDisplayScaleFactor();
            pOwner->OnNativeProcessDisplayScaleChangedMsg(fNewDisplayScale, fNewPixelDensity);
            uint32_t nNewDisplayScale = pOwner->OnNativeGetDpi().GetDisplayScaleFactor();

            if (!ownerFlag.expired() && (nOldDisplayScale != nNewDisplayScale)) {
                //The UI display scale has changed
                m_ptLastMousePos = pOwner->OnNativeGetDpi().GetScalePoint(m_ptLastMousePos, nOldDisplayScale);

                //The window size needs to be adjusted according to the DPI ratio to avoid an imbalanced UI display scale
                if ((nNewDisplayScale != 0) && (nOldDisplayScale != 0)) {
                    int w = 0;
                    int h = 0;
                    if (SDL_GetWindowSize(m_sdlWindow, &w, &h)) {
                        w = pOwner->OnNativeGetDpi().MulDiv(w, (int)nNewDisplayScale, (int)nOldDisplayScale);
                        h = pOwner->OnNativeGetDpi().MulDiv(h, (int)nNewDisplayScale, (int)nOldDisplayScale);
                        SDL_SetWindowSize(m_sdlWindow, w, h);
                    }
                }
            }
            if (!ownerFlag.expired()) {
                pOwner->OnNativeDisplayScaleChangedMsg(fNewDisplayScale, fNewPixelDensity);
            }
        }
        break;
    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        {
            if (GlobalManager::Instance().Dpi().IsPixelDensityEnabled()) {
                //The pixel density of the window has changed, triggering the DPI change event
                float fNewDisplayScale = SDL_GetWindowDisplayScale(m_sdlWindow);
                float fNewPixelDensity = SDL_GetWindowPixelDensity(m_sdlWindow);
#ifdef DUI_HDPI_TEST_PIXEL_DENSITY
                //TEST ONLY
                fNewPixelDensity = DUI_HDPI_TEST_PIXEL_DENSITY;
#endif
                uint32_t nOldDisplayScale = pOwner->OnNativeGetDpi().GetDisplayScaleFactor();
                pOwner->OnNativeProcessDisplayScaleChangedMsg(fNewDisplayScale, fNewPixelDensity);
                if (!ownerFlag.expired() && (nOldDisplayScale != pOwner->OnNativeGetDpi().GetDisplayScaleFactor())) {
                    m_ptLastMousePos = pOwner->OnNativeGetDpi().GetScalePoint(m_ptLastMousePos, nOldDisplayScale);
                }                
                if (!ownerFlag.expired()) {
                    pOwner->OnNativeDisplayScaleChangedMsg(fNewDisplayScale, fNewPixelDensity);
                }
            }
        }
        break;
    case SDL_EVENT_WINDOW_MOVED:
        {
            //The window has moved
            UiPoint ptTopLeft;
            ptTopLeft.x = sdlEvent.window.data1;
            ptTopLeft.y = sdlEvent.window.data2;
            lResult = pOwner->OnNativeMoveMsg(ptTopLeft, NativeMsg(SDL_EVENT_WINDOW_MOVED, 0, 0), bHandled);
            if (!ownerFlag.expired()) {
                CheckWindowSnap(m_sdlWindow);
            }
        }
        break;
    case SDL_EVENT_WINDOW_EXPOSED:
        //Asynchronous window painting message: messages generated by the system have already been painted synchronously, so no repainting is done here
#if defined (DUI_BUILD_FOR_LINUX) || defined (DUI_BUILD_FOR_FREEBSD)
        if (!m_bInitWindowPosFlag) {
            m_bInitWindowPosFlag = true;
            if (IsVideoDriverWayland()) {
                uint32_t uFlags = WindowPosFlags::kSWP_NOZORDER;
                if ((m_ptInitWindow.x == kCW_USEDEFAULT) || (m_ptInitWindow.y == kCW_USEDEFAULT)) {
                    uFlags |= WindowPosFlags::kSWP_NOMOVE;
                }
                SetWindowPos(nullptr, InsertAfterFlag::kHWND_DEFAULT,
                             m_ptInitWindow.x, m_ptInitWindow.y,
                             m_szInitWindow.cx, m_szInitWindow.cy,
                             uFlags);
            }
        }
#endif
        break;
    case WM_USER_PAINT_MSG:
        //Proactively initiated window painting message
        PaintWindow(false);
        break;
    case WM_USER_HOVER_MSG:
        //Proactively triggered Hover message
        {
            UiPoint pt;
            GetCursorPos(pt);
            ScreenToClient(pt);
            lResult = pOwner->OnNativeMouseHoverMsg(pt, 0, NativeMsg(WM_USER_HOVER_MSG, 0, 0), bHandled);
        }
        break;
    case SDL_EVENT_WINDOW_MOUSE_ENTER:
        //No processing needed; Windows does not have this message
        break;
    case SDL_EVENT_WINDOW_MOUSE_LEAVE:
        {
            //The mouse has left the window
            lResult = pOwner->OnNativeMouseLeaveMsg(NativeMsg(SDL_EVENT_WINDOW_MOUSE_LEAVE, 0, 0), bHandled);
        }
        break;
    case SDL_EVENT_MOUSE_MOTION:
        {
            UiPoint pt;
            pt.x = (int32_t)sdlEvent.motion.x;
            pt.y = (int32_t)sdlEvent.motion.y;
            pOwner->OnNativeGetDpi().WindowSizeToClientSize(pt);
            uint32_t modifierKey = GetModifiers(SDL_GetModState());
            lResult = pOwner->OnNativeMouseMoveMsg(pt, modifierKey, false, NativeMsg(SDL_EVENT_MOUSE_MOTION, 0, 0), bHandled);

            //Since SDL has no WM_SETCURSOR message, trigger the set-cursor message when the mouse moves
            if (!ownerFlag.expired()) {
                bool bNativeHandled = false;
                pOwner->OnNativeSetCursorMsg(NativeMsg(SDL_EVENT_MOUSE_MOTION, 0, 0), bNativeHandled);
            }
        }
        break;
    case SDL_EVENT_MOUSE_WHEEL:
        {
#ifdef DUI_BUILD_FOR_WIN
            #define WHEEL_DELTA     120
#else
            //TODO: confirm what value other platforms should use
            #define WHEEL_DELTA     120
#endif
            int32_t wheelDelta = (int32_t)(sdlEvent.wheel.y * WHEEL_DELTA);
            pOwner->OnNativeGetDpi().WindowSizeToClientSize(wheelDelta);
            UiPoint pt;
            pt.x = (int32_t)sdlEvent.wheel.mouse_x;
            pt.y = (int32_t)sdlEvent.wheel.mouse_y;
            pOwner->OnNativeGetDpi().WindowSizeToClientSize(pt);
            uint32_t modifierKey = GetModifiers(SDL_GetModState());
            lResult = pOwner->OnNativeMouseWheelMsg(wheelDelta, pt, modifierKey, NativeMsg(SDL_EVENT_MOUSE_WHEEL, 0, 0), bHandled);
        }
        break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        {
            UiPoint pt;
            pt.x = (int32_t)sdlEvent.button.x;
            pt.y = (int32_t)sdlEvent.button.y;
            pOwner->OnNativeGetDpi().WindowSizeToClientSize(pt);

#if defined (DUI_BUILD_FOR_MACOS)
            //MacOS platform: when a CEF child window exists, after clicking the page first and then the main UI, the pt value given by SDL is incorrect, so it is corrected
            UiPoint mousePt;
            GetCursorPos(mousePt);
            ScreenToClient(mousePt);
            if ((mousePt.x != pt.x) || (mousePt.y != pt.y)) {
                pt.x = mousePt.x;
                pt.y = mousePt.y;
            }
#endif

            uint32_t modifierKey = GetModifiers(SDL_GetModState());
            if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
                //Left mouse button
                lResult = pOwner->OnNativeMouseLButtonDownMsg(pt, modifierKey, NativeMsg(SDL_EVENT_MOUSE_BUTTON_DOWN, 0, 0), bHandled);
            }
            else if (sdlEvent.button.button == SDL_BUTTON_RIGHT) {
                //Right mouse button
                lResult = pOwner->OnNativeMouseRButtonDownMsg(pt, modifierKey, NativeMsg(SDL_EVENT_MOUSE_BUTTON_DOWN, 0, 0), bHandled);
            }
            else if (sdlEvent.button.button == SDL_BUTTON_MIDDLE) {
                //Middle mouse button
                lResult = pOwner->OnNativeMouseMButtonDownMsg(pt, modifierKey, NativeMsg(SDL_EVENT_MOUSE_BUTTON_DOWN, 0, 0), bHandled);
            }
        }
        break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            UiPoint pt;
            pt.x = (int32_t)sdlEvent.button.x;
            pt.y = (int32_t)sdlEvent.button.y;
            pOwner->OnNativeGetDpi().WindowSizeToClientSize(pt);
            uint32_t modifierKey = GetModifiers(SDL_GetModState());
            bool bDoubleClick = (sdlEvent.button.clicks == 2) ? true : false;//Whether it is a double click
            if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
                //Left mouse button: first trigger the left-button up message, then the left-button double-click message, to avoid losing the left-button up message
                lResult = pOwner->OnNativeMouseLButtonUpMsg(pt, modifierKey, NativeMsg(SDL_EVENT_MOUSE_BUTTON_UP, 0, 0), bHandled);
                if (!bHandled && bDoubleClick && !ownerFlag.expired()) {
                    lResult = pOwner->OnNativeMouseLButtonDbClickMsg(pt, modifierKey, NativeMsg(SDL_EVENT_MOUSE_BUTTON_UP, 0, 0), bHandled);
                }
            }
            else if (sdlEvent.button.button == SDL_BUTTON_RIGHT) {
                //Right mouse button: first trigger the right-button up message, then the right-button double-click message, to avoid losing the right-button up message
                lResult = pOwner->OnNativeMouseRButtonUpMsg(pt, modifierKey, NativeMsg(SDL_EVENT_MOUSE_BUTTON_UP, 0, 0), bHandled);
                if (!bHandled && !ownerFlag.expired() && (sdlEvent.button.clicks == 1)) {
                    //Simulate Windows and trigger a Context Menu event
                    bool bMenuHandled = false;
                    pOwner->OnNativeContextMenuMsg(pt, NativeMsg(0, 0, 0), bMenuHandled);
                }
                if (!bHandled && bDoubleClick && !ownerFlag.expired()) {
                    lResult = pOwner->OnNativeMouseRButtonDbClickMsg(pt, modifierKey, NativeMsg(SDL_EVENT_MOUSE_BUTTON_UP, 0, 0), bHandled);
                }
            }
            else if (sdlEvent.button.button == SDL_BUTTON_MIDDLE) {
                //Middle mouse button: first trigger the middle-button up message, then the middle-button double-click message, to avoid losing the middle-button up message
                lResult = pOwner->OnNativeMouseMButtonUpMsg(pt, modifierKey, NativeMsg(SDL_EVENT_MOUSE_BUTTON_UP, 0, 0), bHandled);
                if (!bHandled && bDoubleClick && !ownerFlag.expired()) {
                    lResult = pOwner->OnNativeMouseMButtonDbClickMsg(pt, modifierKey, NativeMsg(SDL_EVENT_MOUSE_BUTTON_UP, 0, 0), bHandled);
                }
            }
        }
        break;
    case SDL_EVENT_WINDOW_FOCUS_GAINED:
        {
            //Get keyboard input focus
            INativeWindow* pLostFocusWindow = nullptr;//This parameter cannot be obtained (no impact for now; the code currently does not use this parameter)
            lResult = pOwner->OnNativeSetFocusMsg(pLostFocusWindow, NativeMsg(SDL_EVENT_WINDOW_FOCUS_GAINED, 0, 0), bHandled);
        }
        break;
    case SDL_EVENT_WINDOW_FOCUS_LOST:
        {
            INativeWindow* pSetFocusWindow = nullptr;//This parameter cannot be obtained directly, only indirectly (this parameter is used a lot in the code and needs to be obtained)
            SDL_Window* pKeyboardFocus = SDL_GetKeyboardFocus();
            if (pKeyboardFocus != nullptr) {
                SDL_WindowID id = SDL_GetWindowID(pKeyboardFocus);
                if (id != 0) {
                    NativeWindow_SDL* pNativeWindow = GetWindowFromID(id);
                    if ((pNativeWindow != nullptr) && (pNativeWindow != this)) {
                        pSetFocusWindow = pNativeWindow->m_pOwner;
                    }
                }
            }
            lResult = pOwner->OnNativeKillFocusMsg(pSetFocusWindow, NativeMsg(SDL_EVENT_WINDOW_FOCUS_LOST, 0, 0), bHandled);
        }
        break;
    case SDL_EVENT_KEY_DOWN:
        {            
            VirtualKeyCode vkCode = Keycode::GetVirtualKeyCode(sdlEvent.key.key);
            uint32_t modifierKey = GetModifiers(sdlEvent.key.mod);
            lResult = pOwner->OnNativeKeyDownMsg(vkCode, modifierKey, NativeMsg(SDL_EVENT_KEY_DOWN, SDL_EVENT_KEY_DOWN, (WPARAM)(SDL_KeyboardEvent*)&sdlEvent.key), bHandled);
        }
        break;
    case SDL_EVENT_KEY_UP:
        {
            VirtualKeyCode vkCode = Keycode::GetVirtualKeyCode(sdlEvent.key.key);
            uint32_t modifierKey = GetModifiers(sdlEvent.key.mod);
            lResult = pOwner->OnNativeKeyUpMsg(vkCode, modifierKey, NativeMsg(SDL_EVENT_KEY_UP, SDL_EVENT_KEY_UP, (WPARAM)(SDL_KeyboardEvent*)&sdlEvent.key), bHandled);
        }
        break;
    case SDL_EVENT_TEXT_INPUT:
        {
            //Equivalent to the WM_CHAR message on Windows
            if (sdlEvent.text.text != nullptr) {
                //The text is UTF-8 encoded
                DStringW textW = StringConvert::UTF8ToWString(sdlEvent.text.text);
                if (!textW.empty()) {
                    //Convert to a WM_CHAR event; when there are multiple characters, pass them through NativeMsg
                    VirtualKeyCode vkCode = VirtualKeyCode::kVK_None;
                    uint32_t modifierKey = GetModifiers(SDL_GetModState());
                    NativeMsg nativeMsg(SDL_EVENT_TEXT_INPUT, (WPARAM)textW.c_str(), (LPARAM)textW.size());//wParam: the address of the entire string, lParam: the number of characters
                    lResult = m_pOwner->OnNativeCharMsg(vkCode, modifierKey, nativeMsg, bHandled);
                }
            }
        }
        break;
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        {
            //Callback for the close event
            bHandled = false;
            pOwner->OnNativeWindowCloseMsg((uint32_t)m_closeParam, NativeMsg(SDL_EVENT_WINDOW_CLOSE_REQUESTED, 0, 0), bHandled);
            if (bHandled && !ownerFlag.expired()) {
                //The close event was intercepted; restore the state before closing
                m_bCloseing = false;
            }
            else if(!ownerFlag.expired()) {
                //PreClose event
                pOwner->OnNativePreCloseWindow();

                //Close the window
                if (!ownerFlag.expired()) {
                    ClearNativeWindow();
                }
            }
        }
        break;
    case SDL_EVENT_WINDOW_DESTROYED:
        {
            //The window has been destroyed
            SDL_WindowID windowID = NativeWindow_SDL::GetWindowIdFromEvent(sdlEvent);
            ClearWindowFromID(windowID, this);

            //The window has been closed (after closing)
            pOwner->OnNativePostCloseWindow();
            if (!ownerFlag.expired()) {
                OnFinalMessage();
            }
        }
        break;
    case SDL_EVENT_DROP_BEGIN:
        {
            bHandled = true;
            if (m_pWindowDropTarget != nullptr) {
                m_pWindowDropTarget->OnDropBegin();
            }
        }
        break;
    case SDL_EVENT_DROP_POSITION:
        {
            bHandled = true;
            if (m_pWindowDropTarget != nullptr) {
                UiPoint pt((uint32_t)sdlEvent.drop.x, (uint32_t)sdlEvent.drop.y);
                pOwner->OnNativeGetDpi().WindowSizeToClientSize(pt);
                m_pWindowDropTarget->OnDropPosition(pt);
            }
        }
        break;
    case SDL_EVENT_DROP_TEXT:
        {
            bHandled = true;
            if (m_pWindowDropTarget != nullptr) {
                DStringA dropText;
                if (sdlEvent.drop.data != nullptr) {
                    dropText = sdlEvent.drop.data;
                }
                m_pWindowDropTarget->OnDropText(dropText);
            }
        }
        break;
    case SDL_EVENT_DROP_FILE:
        {
            bHandled = true;
            if (m_pWindowDropTarget != nullptr) {
                DStringA dropSource;
                DStringA dropFile;
                if (sdlEvent.drop.data != nullptr) {
                    dropFile = sdlEvent.drop.data;
                }
                if (sdlEvent.drop.source != nullptr) {
                    dropSource = sdlEvent.drop.source;
                }
                m_pWindowDropTarget->OnDropFile(dropSource, dropFile);
            }
        }
        break;
    case SDL_EVENT_DROP_COMPLETE:
        {
            bHandled = true;
            if (m_pWindowDropTarget != nullptr) {
                m_pWindowDropTarget->OnDropComplete();
            }
        }
        break;
    default:
        break;
    }
    UNUSED_VARIABLE(lResult);
    return true;
}

static bool SDLCALL OnNativeWindowExposedEvent(void* userdata, SDL_Event* event)
{
    //Window painting event: paint synchronously to avoid a black screen
    if ((userdata != nullptr) && (event != nullptr)) {
        SDL_EventType eventType = (SDL_EventType)event->type;
        if (eventType == SDL_EVENT_WINDOW_EXPOSED) {
            NativeWindow_SDL* pNativeWindow = (NativeWindow_SDL*)userdata;
            if ((SDL_Window*)pNativeWindow->GetWindowHandle() == SDL_GetWindowFromEvent(event)) {
                pNativeWindow->PaintWindow(true);
            }
        }
    }
    return true;
}

NativeWindow_SDL::NativeWindow_SDL(INativeWindow* pOwner):
    m_pOwner(pOwner),
    m_sdlWindow(nullptr),
    m_sdlRenderer(nullptr),
    m_bIsLayeredWindow(false),
    m_nLayeredWindowAlpha(255),
    m_nLayeredWindowOpacity(255),
    m_bUseSystemCaption(false),
    m_bMouseCapture(false),
    m_bCloseing(false),
    m_bChildWindow(false),
    m_closeParam(kWindowCloseNormal),
    m_bEnableDragDrop(true),
    m_bFakeModal(false),
    m_bDoModal(false),
    m_bFullscreen(false),
    m_bFullscreenExiting(false),
    m_bFullscreenMaximized(false),
    m_ptLastMousePos(-1, -1),
    m_bInitWindowPosFlag(false)
{
    ASSERT(m_pOwner != nullptr);    
}

NativeWindow_SDL::~NativeWindow_SDL()
{
    //Remove the watchers for synchronous window painting (make sure they are removed to avoid exceptions)
    SDL_RemoveEventWatch(OnNativeWindowExposedEvent, this);

    ASSERT(m_sdlWindow == nullptr);
    ClearNativeWindow();
}

bool NativeWindow_SDL::CreateWnd(NativeWindow_SDL* pParentWindow,
                                 const WindowCreateParam& createParam,
                                 const WindowCreateAttributes& createAttributes)
{
    //Get the DiplayMode to avoid hanging when an error occurs at the lower level
    {
        SDL_DisplayID id = SDL_GetPrimaryDisplay();
        const SDL_DisplayMode* mode1 = SDL_GetDesktopDisplayMode(id);
        const SDL_DisplayMode* mode2 = SDL_GetCurrentDisplayMode(id);
        if (mode1 == mode2) {
            id = 0;
        }
    }

    ASSERT(m_sdlWindow == nullptr);
    if (m_sdlWindow != nullptr) {
        return false;
    }
    ASSERT(m_sdlRenderer == nullptr);
    if (m_sdlRenderer != nullptr) {
        return false;
    }
    //Initialize SDL
    if (!MessageLoop_SDL::CheckInitSDL()) {
        return false;
    }

    //Save the parameters
    m_createParam = createParam;

    //Set the default style
    if (m_createParam.m_dwStyle == 0) {
        m_createParam.m_dwStyle = kWS_OVERLAPPEDWINDOW;
    }

    //Create the SDL window and Render
    if (!CreateWindowAndRender(pParentWindow, createAttributes)) {
        return false;
    }

    if (m_pOwner != nullptr) {
        bool bHandled = false;
        m_pOwner->OnNativeCreateWndMsg(false, NativeMsg(0, 0, 0), bHandled);

        bool bMinimizeBox = false;
        bool bMaximizeBox = false;
        if (m_pOwner->OnNativeHasMinMaxBox(bMinimizeBox, bMaximizeBox)) {
            //If there is a maximize button, set the resizable window attribute
            if (bMaximizeBox) {
                SDL_SetWindowResizable(m_sdlWindow, true);
            }
        }
    }
    return true;
}

bool NativeWindow_SDL::CreateWindowAndRender(NativeWindow_SDL* pParentWindow, const WindowCreateAttributes& createAttributes)
{
    //Create the SDL window
    m_sdlWindow = CreateSdlWindow(pParentWindow, createAttributes);
    ASSERT(m_sdlWindow != nullptr);
    if (m_sdlWindow == nullptr) {
        return false;
    }

    //Create the SDL render interface
    m_sdlRenderer = CreateSdlRenderer(createAttributes.m_sdlRenderName);
    ASSERT(m_sdlRenderer != nullptr);
    if (m_sdlRenderer == nullptr) {
        SDL_DestroyWindow(m_sdlWindow);
        m_sdlWindow = nullptr;
        return false;
    }

    //Initialize
    InitNativeWindow();
    return true;
}

SDL_Window* NativeWindow_SDL::CreateSdlWindow(NativeWindow_SDL* pParentWindow, const WindowCreateAttributes& createAttributes)
{
    bool bOpenGL = false;
    bool bSupportTransparent = false;
#ifndef DUI_BUILD_FOR_WIN
    //Linux platform    
    bool bOpenGLES2 = false;    
    QueryRenderProperties(createAttributes.m_sdlRenderName, bOpenGL, bOpenGLES2, bSupportTransparent);
#endif

    //Sync the Window attributes from the XML file and carry them when creating the window
    SyncCreateWindowAttributes(createAttributes, bSupportTransparent);

    if (m_createParam.m_bCenterWindow) {
        //When centering the window, calculate its starting position to avoid the window position changing when it pops up
        int32_t xPos = 0;
        int32_t yPos = 0;
        SDL_Window* pCenterWindow = pParentWindow != nullptr ? pParentWindow->m_sdlWindow : nullptr;
        if (CalculateCenterWindowPos(pCenterWindow, xPos, yPos)) {
            m_createParam.m_nX = xPos;
            m_createParam.m_nY = yPos;
        }
    }

    //Create the properties
    SDL_PropertiesID props = SDL_CreateProperties();
    SetCreateWindowProperties(props, pParentWindow, createAttributes, bOpenGL);

    SDL_Window* pSdlWindow = nullptr;
#ifndef DUI_BUILD_FOR_WIN
    //Linux platform
    std::vector<DString> renderNames;
    GetRenderNameList(createAttributes.m_sdlRenderName, renderNames);
    DString firstRenderName;
    if (!renderNames.empty()) {
        firstRenderName = renderNames.front();
    }
    if (bSupportTransparent && (firstRenderName == _T("opengles2"))) {
        //Set the flag to create a Render that supports transparent windows
        bool bOldValue = SDL_GetHintBoolean(SDL_HINT_VIDEO_FORCE_EGL, false);
        if (!bOldValue) {
            SDL_SetHint(SDL_HINT_VIDEO_FORCE_EGL, "true");
            pSdlWindow = SDL_CreateWindowWithProperties(props);
            SDL_SetHint(SDL_HINT_VIDEO_FORCE_EGL, "false");
        }
        if (pSdlWindow == nullptr) {
            pSdlWindow = SDL_CreateWindowWithProperties(props);
        }
    }
    else {
        pSdlWindow = SDL_CreateWindowWithProperties(props);
    }
#else
    //Windows platform
    pSdlWindow = SDL_CreateWindowWithProperties(props);
#endif

    SDL_DestroyProperties(props);

    if (pSdlWindow != nullptr) {
        if (createAttributes.m_bSizeBoxDefined && !createAttributes.m_rcSizeBox.IsZero()) {
            SDL_SetWindowResizable(pSdlWindow, true);
        }

        //Set up watchers to implement synchronous window painting
        bool bRet = SDL_AddEventWatch(OnNativeWindowExposedEvent, this);
        ASSERT_UNUSED_VARIABLE(bRet);
    }
    return pSdlWindow;
}

int32_t NativeWindow_SDL::DoModal(NativeWindow_SDL* pParentWindow,
                                  const WindowCreateParam& createParam,
                                  const WindowCreateAttributes& createAttributes,
                                  bool bCloseByEsc, bool bCloseByEnter)
{
    ASSERT(m_sdlWindow == nullptr);
    if (m_sdlWindow != nullptr) {
        return -1;
    }
    ASSERT(m_sdlRenderer == nullptr);
    if (m_sdlRenderer != nullptr) {
        return -1;
    }
    //Initialize SDL
    if (!MessageLoop_SDL::CheckInitSDL()) {
        return -1;
    }

    //Save the parameters
    m_createParam = createParam;

    //Set the default style
    if (m_createParam.m_dwStyle == 0) {
        m_createParam.m_dwStyle = kWS_OVERLAPPEDWINDOW;
    }

    //Create the SDL window and Render
    if (!CreateWindowAndRender(pParentWindow, createAttributes)) {
        return false;
    }
    
    //Mark as modal dialog state
    m_bDoModal = true;

    if (m_pOwner != nullptr) {
        bool bHandled = false;
        m_pOwner->OnNativeCreateWndMsg(true, NativeMsg(0, 0, 0), bHandled);

        bool bMinimizeBox = false;
        bool bMaximizeBox = false;
        if (m_pOwner->OnNativeHasMinMaxBox(bMinimizeBox, bMaximizeBox)) {
            //If there is a maximize button, set the resizable window attribute
            if (bMaximizeBox) {
                SDL_SetWindowResizable(m_sdlWindow, true);
            }
        }
    }

    SDL_WindowID currentWindowId = SDL_GetWindowID(m_sdlWindow);
    if (currentWindowId == 0) {
        m_bDoModal = false;
        return 0;
    }

    //Set it as a modal dialog
    if (pParentWindow != nullptr) {
        SDL_SetWindowParent(m_sdlWindow, pParentWindow->m_sdlWindow);
        SDL_SetWindowModal(m_sdlWindow, true);
    }

    //Show the window
    m_bCloseing = false;
    m_closeParam = kWindowCloseNormal;
    SDL_ShowWindow(m_sdlWindow);

    //Enter the internal message loop
    MessageLoop_SDL messageLoop;
    messageLoop.RunDoModal(*this, bCloseByEsc, bCloseByEnter);

    m_bDoModal = false;
    return m_closeParam;
}

bool NativeWindow_SDL::CreateChildWnd(NativeWindow_SDL* pParentWindow, int32_t nX, int32_t nY, int32_t nWidth, int32_t nHeight)
{
    ASSERT(m_sdlWindow == nullptr);
    if (m_sdlWindow != nullptr) {
        return false;
    }
    ASSERT(m_sdlRenderer == nullptr);
    if (m_sdlRenderer != nullptr) {
        return false;
    }
    SDL_Window* sdlParentWindow = nullptr;
    if (pParentWindow != nullptr) {
        sdlParentWindow = pParentWindow->m_sdlWindow;
    }
    ASSERT(sdlParentWindow != nullptr);
    if (sdlParentWindow == nullptr) {
        return false;
    }

    SDL_PropertiesID props = SDL_CreateProperties();

    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, nX);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, nY);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, nWidth);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, nHeight);

    //Parent window
    SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_PARENT_POINTER, sdlParentWindow);

    //Window attributes
    SDL_WindowFlags windowFlags = 0;

    //When created, the window remains hidden; the API must be called to show the window to avoid flicker when creating the window
    windowFlags |= SDL_WINDOW_HIDDEN;

    //Support Hight DPI, see the SDL documentation: docs/README-highdpi.md
    windowFlags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;

    //As long as the system title bar is not used, this attribute must be set, otherwise the window will have a system title bar
    windowFlags |= SDL_WINDOW_BORDERLESS;

    SDL_WindowFlags sdlParentFlag = SDL_GetWindowFlags(sdlParentWindow);
    if (sdlParentFlag & SDL_WINDOW_TRANSPARENT) {
        //Set the transparent attribute (following the parent window); this attribute must be passed in when creating the window and cannot be modified after the window is created
        windowFlags |= SDL_WINDOW_TRANSPARENT;
    }

    //This window does not need focus
    windowFlags |= SDL_WINDOW_NOT_FOCUSABLE; //SDL_WINDOW_INPUT_FOCUS

    //Add the OpenGL flag
    windowFlags |= SDL_WINDOW_OPENGL;

    //This flag cannot be included, otherwise the child window will go behind the main window (the SDL internal implementation does not really use the parent-child window relationship)
    windowFlags &= ~SDL_WINDOW_UTILITY;

#ifdef DUI_BUILD_FOR_WIN
    //Windows platform: associate the SDL window with the externally created child window handle
    SDL_PropertiesID propID = SDL_GetWindowProperties(sdlParentWindow);
    HMODULE hModule = (HMODULE)SDL_GetPointerProperty(propID, SDL_PROP_WINDOW_WIN32_INSTANCE_POINTER, nullptr);
    ASSERT(hModule != nullptr);
    if (hModule == nullptr) {
        hModule = (HMODULE)GlobalManager::Instance().GetPlatformData();
    }
    if (hModule == nullptr) {
        hModule = ::GetModuleHandle(nullptr);
    }
    HWND hChild = nullptr;
    HWND hParent = (HWND)SDL_GetPointerProperty(propID, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
    if (hParent != nullptr) {
        const DString className = _T("dui_child_window");
        WNDCLASS wc = { 0 };
        wc.lpfnWndProc = ::DefWindowProc;
        wc.hInstance = hModule;
        wc.lpszClassName = className.c_str();
        ATOM ret = RegisterClass(&wc);
        ASSERT(ret != 0 || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);

        //Unregister the ATOM when the module exits
        GlobalManager::Instance().AddAtExitFunction([className, hModule]() {
            ::UnregisterClassW(className.c_str(), hModule);
            });

        // Create a Windows child window (WS_CHILD style)
        hChild = ::CreateWindowEx( 0,
                                   className.c_str(),
                                   0,
                                   WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                                   nX, nY, nWidth, nHeight,
                                   hParent,
                                   nullptr,
                                   hModule,
                                   nullptr);
    }
    if (hChild != nullptr) {
        SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_WIN32_HWND_POINTER, hChild);
    }
#endif

    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, windowFlags);

    m_bChildWindow = true;
    m_sdlWindow = SDL_CreateWindowWithProperties(props);
    SDL_DestroyProperties(props);

    if (m_sdlWindow != nullptr) {
        //Create the SDL render interface (this is necessary; otherwise the child window may fail to display properly on Linux)
        m_sdlRenderer = CreateSdlRenderer(_T(""));
        ASSERT(m_sdlRenderer != nullptr);
        if (m_sdlRenderer == nullptr) {
            SDL_DestroyWindow(m_sdlWindow);
            m_sdlWindow = nullptr;
            return false;
        }

        SDL_WindowID id = SDL_GetWindowID(m_sdlWindow);
        SetWindowFromID(id, this);

        //SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "true");

        //Set up watchers to implement synchronous window painting
        bool bRet = SDL_AddEventWatch(OnNativeWindowExposedEvent, this);
        ASSERT_UNUSED_VARIABLE(bRet);

        //Associate drag-and-drop operations
        SetEnableDragDrop(IsEnableDragDrop());

        if (m_pOwner != nullptr) {
            bool bHandled = false;
            m_pOwner->OnNativeCreateWndMsg(false, NativeMsg(0, 0, 0), bHandled);
        }
        return true;
    }
    else {
#ifdef DUI_BUILD_FOR_WIN
        if (hChild != nullptr) {
            ::DestroyWindow(hChild);
            hChild = nullptr;
        }
#endif
        return false;
    }    
}

SDL_Renderer* NativeWindow_SDL::CreateSdlRenderer(const DString& sdlRenderName) const
{
    std::vector<DString> renderNames;
    GetRenderNameList(sdlRenderName, renderNames);

    ASSERT(m_sdlWindow != nullptr);
    if (m_sdlWindow == nullptr) {
        return nullptr;
    }

    SDL_Renderer* sdlRenderer = nullptr;
    for (auto iter = renderNames.begin(); iter != renderNames.end(); ++iter) {
        const DStringA renderName = StringConvert::TToUTF8(*iter);
        sdlRenderer = SDL_CreateRenderer(m_sdlWindow, renderName.c_str());
        if (sdlRenderer != nullptr) {
            break;
        }
    }
    if (sdlRenderer == nullptr) {
        //If creation fails, use the default Render engine
        sdlRenderer = SDL_CreateRenderer(m_sdlWindow, nullptr);
    }
    ASSERT(sdlRenderer != nullptr);
    return sdlRenderer;
}

void NativeWindow_SDL::GetRenderNameList(const DString& externalRenderName, std::vector<DString>& renderNames) const
{
    //The following is the list of available Render names (not every named Render is usable; for example, some do not work, and some do not support features such as semi-transparent windows):
    //Windows platform: "gpu,direct3d11,direct3d12,direct3d,opengl,opengles2,vulkan,software"
    //Linux platform: "gpu,opengl,opengles2,vulkan,software"

    //The list of Render names to create, ordered by priority
    renderNames.clear();
    if (m_sdlRenderer != nullptr) {
        //The current Render has the highest priority
        const char* renderName = SDL_GetRendererName(m_sdlRenderer);
        if (renderName != nullptr) {
            std::string name = renderName;
            renderNames.push_back(StringConvert::UTF8ToT(name));
        }
    }

    if (renderNames.empty()) {
        //If a window already exists, prefer the Render Name of the existing window to avoid windows coexisting with different Render Names
        int32_t nWindowCount = 0;
        SDL_Window** ppSdlWindow = SDL_GetWindows(&nWindowCount);
        if ((nWindowCount > 0) && (ppSdlWindow != nullptr)) {
            for (int32_t nWindow = 0; nWindow < nWindowCount; ++nWindow) {
                SDL_Renderer* pSdlRender = nullptr;
                SDL_Window* pSdlWindow = ppSdlWindow[nWindow];
                if (pSdlWindow != nullptr) {
                    pSdlRender = SDL_GetRenderer(pSdlWindow);                    
                }
                if (pSdlRender != nullptr) {
                    const char* pRenderName = SDL_GetRendererName(pSdlRender);
                    if ((pRenderName != nullptr) && (pRenderName[0] != '\0')) {
                        //As the highest priority Render name
                        DString name = StringConvert::UTF8ToT(std::string(pRenderName));
                        renderNames.push_back(name);
                        break;
                    }
                }
            }
        }
    }

    //The externally set Render name takes priority
    if (!externalRenderName.empty()) {
        std::list<DString> renderNameList = StringUtil::Split(externalRenderName, _T(","));
        for (auto iter = renderNameList.begin(); iter != renderNameList.end(); ++iter) {
            DString name = *iter;
            StringUtil::Trim(name);
            if (!name.empty()) {
                renderNames.push_back(name);
            }
        }
    }

    //Lowest priority: default values, ordered by priority
#ifdef DUI_BUILD_FOR_WIN
    //Note: currently, those supporting transparency on the Windows platform (attribute: SDL_WINDOW_TRANSPARENT) are: "direct3d11", "opengl", "vulkan"
    renderNames.push_back(_T("direct3d11"));
    renderNames.push_back(_T("opengl"));
    renderNames.push_back(_T("vulkan"));
#else
    //Linux platform: currently, those supporting transparency (attribute: SDL_WINDOW_TRANSPARENT) are: "opengles2", "vulkan"
    renderNames.push_back(_T("opengles2"));
    renderNames.push_back(_T("vulkan"));
    renderNames.push_back(_T("opengl"));
#endif

    //Remove those that do not exist
    std::vector<DString> renderDrivers;
    int32_t nRenderCount = SDL_GetNumRenderDrivers();
    for (int32_t nRenderIndex = 0; nRenderIndex < nRenderCount; ++nRenderIndex) {
        const char* renderName = SDL_GetRenderDriver(nRenderIndex);
        if (renderName != nullptr) {
            DString name = StringConvert::UTF8ToT(std::string(renderName));
            if (!name.empty()) {
                renderDrivers.push_back(name);
            }
        }
    }
    auto iter = renderNames.begin();
    while ( iter != renderNames.end()) {
        const DString& name = *iter;
        if (std::find(renderDrivers.begin(), renderDrivers.end(), name) == renderDrivers.end()) {
            //Does not exist, remove it
            iter = renderNames.erase(iter);
        }
        else {
            ++iter;
        }
    }
}

void NativeWindow_SDL::QueryRenderProperties(const DString& externalRenderName, bool& bOpenGL, bool& bOpenGLES2, bool& bSupportTransparent) const
{
    bOpenGL = false;
    bOpenGLES2 = false;
    bSupportTransparent = false;

    if (m_sdlRenderer != nullptr) {
        //The current Render has the highest priority
        const char* renderName = SDL_GetRendererName(m_sdlRenderer);
        if (renderName != nullptr) {
            std::string name = renderName;
            bSupportTransparent = IsRenderSupportTransparent(StringConvert::UTF8ToT(name));
            if (name == "opengles2") {
                bOpenGL = true;
                bOpenGLES2 = true;
            }
            else if (name == "opengl") {
                bOpenGL = true;
            }
            else if (name == "vulkan") {
                //The SDL_WINDOW_OPENGL flag needs to be added, otherwise vulkan cannot work; the reason is unknown
                bOpenGL = true;
            }
            return;
        }
    }

    std::vector<DString> renderNames;
    GetRenderNameList(externalRenderName, renderNames);
    for (auto iter = renderNames.begin(); iter != renderNames.end(); ++iter) {
        const DString& name = *iter;
        if (name.empty()) {
            continue;
        }
        if (name == _T("opengles2")) {
            bOpenGL = true;
            bOpenGLES2 = true;
        }
        else if (name == _T("opengl")) {
            bOpenGL = true;
        }
        else if (name == _T("vulkan")) {
            //The SDL_WINDOW_OPENGL flag needs to be added, otherwise vulkan cannot work; the reason is unknown
            bOpenGL = true;
        }
        if (!bSupportTransparent) {
            bSupportTransparent = IsRenderSupportTransparent(name);
        }
        //Only take the first one
        break;
    }
}

bool NativeWindow_SDL::IsRenderSupportTransparent(const DString& renderName) const
{
    bool bSupportTransparent = false;
#ifdef DUI_BUILD_FOR_WIN
    if (renderName == _T("direct3d11")) {
        bSupportTransparent = true;
    }
    else if (renderName == _T("opengl")) {
        bSupportTransparent = true;
    }
    else if (renderName == _T("vulkan")) {
        bSupportTransparent = true;
    }
#else
    if (renderName == _T("opengles2")) {
        bSupportTransparent = true;
    }
    else if (renderName == _T("opengl")) {
        bSupportTransparent = true;
    }
    else if (renderName == _T("vulkan")) {
        bSupportTransparent = true;
    }
#endif
    return bSupportTransparent;
}

void NativeWindow_SDL::SyncCreateWindowAttributes(const WindowCreateAttributes& createAttributes, bool bSupportTransparent)
{
    m_bUseSystemCaption = false;
    if (createAttributes.m_bUseSystemCaptionDefined && createAttributes.m_bUseSystemCaption) {
        //Use the system title bar
        m_bUseSystemCaption = true;
    }
    //Since the Linux system does not support transparency yet, the system title bar is forced to be used
#ifndef DUI_BUILD_FOR_WIN
    if (!bSupportTransparent && createAttributes.m_bUseSystemCaptionDefined) {
        m_bUseSystemCaption = true;
    }
#else
    bSupportTransparent = true;
#endif

    if (IsUseSystemCaption()) {
        //Use the system title bar
        if (m_createParam.m_dwStyle & kWS_POPUP) {
            //Popup window
            m_createParam.m_dwStyle |= (kWS_CAPTION | kWS_SYSMENU);
        }
        else {
            m_createParam.m_dwStyle |= (kWS_CAPTION | kWS_SYSMENU | kWS_MINIMIZEBOX | kWS_MAXIMIZEBOX);
        }
    }

    //Initialize the layered window attributes
    m_bIsLayeredWindow = false;
    if (createAttributes.m_bIsLayeredWindowDefined) {
        if (createAttributes.m_bIsLayeredWindow) {
            m_bIsLayeredWindow = true;
            m_createParam.m_dwExStyle |= kWS_EX_LAYERED;
        }
        else {
            m_createParam.m_dwExStyle &= ~kWS_EX_LAYERED;
        }
    }
    else if (m_createParam.m_dwExStyle & kWS_EX_LAYERED) {
        m_bIsLayeredWindow = true;
    }

    //If the system title bar is used, turn off the layered window
    if (IsUseSystemCaption()) {
        m_bIsLayeredWindow = false;
        m_createParam.m_dwExStyle &= ~kWS_EX_LAYERED;
    }

    //If opacity is set, make it a layered window
    if (createAttributes.m_bLayeredWindowOpacityDefined && (createAttributes.m_nLayeredWindowOpacity != 255)) {
        m_createParam.m_dwExStyle |= kWS_EX_LAYERED;
        m_bIsLayeredWindow = true;
    }
    if (createAttributes.m_bLayeredWindowAlphaDefined && (createAttributes.m_nLayeredWindowAlpha != 255)) {
        m_createParam.m_dwExStyle |= kWS_EX_LAYERED;
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

    //On Linux, only some Renders support transparent windows; on Windows, transparent windows are supported
    if (!bSupportTransparent) {
        m_bIsLayeredWindow = false;
        m_createParam.m_dwExStyle &= ~kWS_EX_LAYERED;
    }
}

void NativeWindow_SDL::SetCreateWindowProperties(SDL_PropertiesID props, NativeWindow_SDL* pParentWindow,
                                                 const WindowCreateAttributes& createAttributes, bool bUseOpenGL)
{
    //Do not automatically exit the message loop when closing the window
    SDL_SetHint(SDL_HINT_QUIT_ON_LAST_WINDOW_CLOSE, "false");

    //This attribute needs to be set in advance so that the window resize function is kept when there is no border
    SDL_SetHint("SDL_BORDERLESS_RESIZABLE_STYLE", "true");

    if (!m_createParam.m_windowTitle.empty()) {
        std::string windowTitle = StringConvert::TToUTF8(m_createParam.m_windowTitle);
        SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, windowTitle.c_str());
    }

    //The position and size of the window
    int32_t x = kCW_USEDEFAULT;
    int32_t y = kCW_USEDEFAULT;
    int32_t cx = kCW_USEDEFAULT;
    int32_t cy = kCW_USEDEFAULT;

    if ((m_createParam.m_nX != kCW_USEDEFAULT) && (m_createParam.m_nY != kCW_USEDEFAULT)) {
        x = m_createParam.m_nX;
        y = m_createParam.m_nY;
    }
    if ((m_createParam.m_nWidth != kCW_USEDEFAULT) && (m_createParam.m_nHeight != kCW_USEDEFAULT)) {
        cx = m_createParam.m_nWidth;
        cy = m_createParam.m_nHeight;
    }
    if (x != kCW_USEDEFAULT) {
        SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, x);
    }
    if (y != kCW_USEDEFAULT) {
        SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, y);
    }
    if (cx != kCW_USEDEFAULT) {
        SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, cx);
    }
    if (cy != kCW_USEDEFAULT) {
        SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, cy);
    }

    m_bInitWindowPosFlag = false;
    m_ptInitWindow.x = x;
    m_ptInitWindow.y = y;
    m_szInitWindow.cx = cx;
    m_szInitWindow.cy = cy;

    //Parent window
    if ((pParentWindow != nullptr) && (pParentWindow->m_sdlWindow != nullptr)) {
        SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_PARENT_POINTER, pParentWindow->m_sdlWindow);
    }

    //Window attributes
    SDL_WindowFlags windowFlags = SDL_WINDOW_INPUT_FOCUS;
    if (createAttributes.m_bSizeBoxDefined && !createAttributes.m_rcSizeBox.IsZero()) {
        windowFlags |= SDL_WINDOW_RESIZABLE;
    }

    //When created, the window remains hidden; the API must be called to show the window to avoid flicker when creating the window
    windowFlags |= SDL_WINDOW_HIDDEN;

    //Support Hight DPI, see the SDL documentation: docs/README-highdpi.md
    windowFlags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;

    if (!IsUseSystemCaption() && IsLayeredWindow()) {
        //Set the transparent attribute; this attribute must be passed in when creating the window and cannot be modified after the window is created
        windowFlags |= SDL_WINDOW_TRANSPARENT;
    }

    //If it is a popup window without a shadow and a title bar, it is borderless by default
    const bool bPopupWindow = m_createParam.m_dwStyle & kWS_POPUP;
    if (!IsUseSystemCaption()) {
        //As long as the system title bar is not used, this attribute must be set, otherwise the window will have a system title bar
        windowFlags |= SDL_WINDOW_BORDERLESS;
    }

    if (bPopupWindow && (m_createParam.m_dwExStyle & kWS_EX_TOOLWINDOW)) {
        windowFlags |= SDL_WINDOW_UTILITY;
    }
    else if (bPopupWindow && (pParentWindow != nullptr)) {
        //SDL window parent-child relationship: no parent-child relationship is set at the system level, so popup windows need this attribute to avoid multiple windows appearing in the taskbar
        //However, since SDL does not use the system parent-child window relationship, after a popup window appears, the main window loses focus and appears inactive in the taskbar, which is a poor experience
        windowFlags |= SDL_WINDOW_UTILITY;
    }
    if (m_createParam.m_dwExStyle & kWS_EX_NOACTIVATE) {
        windowFlags |= SDL_WINDOW_NOT_FOCUSABLE;
    }
    if (m_createParam.m_dwExStyle & kWS_EX_TOOLTIP_WINDOW) {
        //Tooltip window
        windowFlags &= ~SDL_WINDOW_UTILITY;
        windowFlags |= SDL_WINDOW_TOOLTIP;
    }
    if (m_createParam.m_dwExStyle & kWS_EX_TOPMOST) {
        windowFlags |= SDL_WINDOW_ALWAYS_ON_TOP;
    }

#ifdef DUI_BUILD_FOR_WIN
    bUseOpenGL = false;
#endif
    if (bUseOpenGL) {
        //Add the OpenGL flag
        windowFlags |= SDL_WINDOW_OPENGL;
    }

    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, windowFlags);
}

static SDL_HitTestResult SDLCALL NativeWindow_SDL_HitTest(SDL_Window* win,
                                                          const SDL_Point* area,
                                                          void* data)
{
    NativeWindow_SDL* pWindow = (NativeWindow_SDL*)data;
    if (pWindow == nullptr) {
        return SDL_HITTEST_NORMAL;
    }
    return (SDL_HitTestResult)pWindow->SDL_HitTest(win, area, data);
}

int32_t NativeWindow_SDL::SDL_HitTest(SDL_Window* win, const SDL_Point* area, void* data)
{
    if (((NativeWindow_SDL*)data != this) ||
        (area == nullptr) || (win != GetWindowHandle()) ||
        IsUseSystemCaption() || (m_pOwner == nullptr)) {
        return SDL_HITTEST_NORMAL;
    }

    //Fullscreen mode: only return the client area attribute
    if (IsWindowFullscreen()) {
        return SDL_HITTEST_NORMAL;
    }

    UiPoint pt;
    pt.x = area->x;
    pt.y = area->y;
    m_pOwner->OnNativeGetDpi().WindowSizeToClientSize(pt);

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
                    return SDL_HITTEST_RESIZE_TOPLEFT;//In the top-left corner of the window border
                }
                else if (pt.x > (rcClient.right - rcSizeBox.right) && pt.x <= rcClient.right) {
                    return SDL_HITTEST_RESIZE_TOPRIGHT;//In the top-right corner of the window border
                }
                else {
                    return SDL_HITTEST_RESIZE_TOP;//In the top horizontal border of the window
                }
            }
            else {
                return SDL_HITTEST_NORMAL;//In the work area
            }
        }
        else if (pt.y > rcClient.bottom - rcSizeBox.bottom) {
            if (pt.y <= rcClient.bottom) {
                if (pt.x < (rcClient.left + rcSizeBox.left) && pt.x >= rcClient.left) {
                    return SDL_HITTEST_RESIZE_BOTTOMLEFT;//In the bottom-left corner of the window border
                }
                else if (pt.x > (rcClient.right - rcSizeBox.right) && pt.x <= rcClient.right) {
                    return SDL_HITTEST_RESIZE_BOTTOMRIGHT;//In the bottom-right corner of the window border
                }
                else {
                    return SDL_HITTEST_RESIZE_BOTTOM;//In the bottom horizontal border of the window
                }
            }
            else {
                return SDL_HITTEST_NORMAL;//In the work area
            }
        }

        if (pt.x < rcClient.left + rcSizeBox.left) {
            if (pt.x >= rcClient.left) {
                return SDL_HITTEST_RESIZE_LEFT;//In the left border of the window
            }
            else {
                return SDL_HITTEST_NORMAL;//In the work area
            }
        }
        if (pt.x > rcClient.right - rcSizeBox.right) {
            if (pt.x <= rcClient.right) {
                return SDL_HITTEST_RESIZE_RIGHT;//In the right border of the window
            }
            else {
                return SDL_HITTEST_NORMAL;//In the work area
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
        //Within the title bar range (SDL does not support HTSYSMENU and HTMAXBUTTON; the implementation here differs from the Windows default)
        if (m_pOwner->OnNativeIsPtInCaptionBarControl(pt)) {
            return SDL_HITTEST_NORMAL;//In the work area (controls placed on the title bar are treated as part of the work area)
        }
        else {
            return SDL_HITTEST_DRAGGABLE;//In the title bar
        }
    }
    //Otherwise, in the work area
    return SDL_HITTEST_NORMAL;
}

void NativeWindow_SDL::InitNativeWindow()
{
    ASSERT(m_sdlWindow != nullptr);
    if (m_sdlWindow == nullptr) {
        return;
    }

    SDL_WindowID id = SDL_GetWindowID(m_sdlWindow);
    SetWindowFromID(id, this);

    //Set the Hit Test function
    if (!IsUseSystemCaption()) {
        bool nRet = SDL_SetWindowHitTest(m_sdlWindow, NativeWindow_SDL_HitTest, this);
        ASSERT_UNUSED_VARIABLE(nRet);
    }
    else {
        bool nRet = SDL_SetWindowHitTest(m_sdlWindow, nullptr, nullptr);
        ASSERT_UNUSED_VARIABLE(nRet);
    }

    if (!m_createParam.m_windowTitle.empty()) {
        std::string windowTitle = StringConvert::TToUTF8(m_createParam.m_windowTitle);
        bool nRet = SDL_SetWindowTitle(m_sdlWindow, windowTitle.c_str());
        ASSERT_UNUSED_VARIABLE(nRet);
    }

    if (m_createParam.m_bCenterWindow) {
        if ((m_createParam.m_nWidth <= 0) || (m_createParam.m_nHeight <= 0)) {
            //If the window cannot be centered when creating it, center it after creation
            CenterWindow();
        }
    }

    //Associated drag-and-drop operation
    SetEnableDragDrop(IsEnableDragDrop());
}

void NativeWindow_SDL::ClearNativeWindow()
{
    SDL_Renderer* sdlRenderer = m_sdlRenderer;
    SDL_Window* sdlWindow = m_sdlWindow;
    m_sdlRenderer = nullptr;
    m_sdlWindow = nullptr;

    if (sdlWindow != nullptr) {
        SDL_SetWindowHitTest(sdlWindow, nullptr, nullptr);
        SDL_DestroyWindow(sdlWindow);
        sdlWindow = nullptr;
    }
    if (sdlRenderer != nullptr) {
        SDL_DestroyRenderer(sdlRenderer);
        sdlRenderer = nullptr;
    }

    m_pWindowDropTarget.reset();
}

void* NativeWindow_SDL::GetWindowHandle() const
{
    return m_sdlWindow;
}

DString NativeWindow_SDL::GetVideoDriverName() const
{
    DString driverName;
    const char* name = SDL_GetCurrentVideoDriver();
    if (name != nullptr) {
        driverName = StringConvert::UTF8ToT(std::string(name));
    }
    return driverName;
}

DString NativeWindow_SDL::GetWindowRenderName() const
{
    DString renderName;
    if (m_sdlWindow != nullptr) {
        SDL_Renderer* sdlRenderer = SDL_GetRenderer(m_sdlWindow);
        if (sdlRenderer != nullptr) {
            const char* name = SDL_GetRendererName(sdlRenderer);
            if (name != nullptr) {
                renderName = StringConvert::UTF8ToT(std::string(name));
            }
        }
    }
    return renderName;
}

bool NativeWindow_SDL::IsWindow() const
{
    return (m_sdlWindow != nullptr);
}

bool NativeWindow_SDL::IsChildWindow() const
{
    return m_bChildWindow;
}

bool NativeWindow_SDL::SetParentWindow(NativeWindow_SDL* pParentWindow)
{
    ASSERT((pParentWindow != nullptr) && pParentWindow->IsWindow());
    if ((pParentWindow == nullptr) || !pParentWindow->IsWindow()) {
        return false;
    }
    if (!IsWindow()) {
        return false;
    }
    bool bRet = SDL_SetWindowParent(m_sdlWindow, pParentWindow->m_sdlWindow);
#ifdef DUI_BUILD_FOR_WIN
    ::SetParent(GetHWND(), pParentWindow->GetHWND());
#endif
    return bRet;
}

#ifdef DUI_BUILD_FOR_WIN

HWND NativeWindow_SDL::GetHWND() const
{
    if (!IsWindow()) {
        return nullptr;
    }
    SDL_PropertiesID propID = SDL_GetWindowProperties(m_sdlWindow);
    HWND hWnd = (HWND)SDL_GetPointerProperty(propID, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
    if (!::IsWindow(hWnd)) {
        hWnd = nullptr;
    }
    return hWnd;
}

HMODULE NativeWindow_SDL::GetResModuleHandle() const
{
    if (!IsWindow()) {
        return nullptr;
    }
    SDL_PropertiesID propID = SDL_GetWindowProperties(m_sdlWindow);
    HMODULE hModule = (HMODULE)SDL_GetPointerProperty(propID, SDL_PROP_WINDOW_WIN32_INSTANCE_POINTER, nullptr);
    ASSERT(hModule != nullptr);
    if (hModule == nullptr) {
        hModule = (HMODULE)GlobalManager::Instance().GetPlatformData();
    }
    if (hModule == nullptr) {
        hModule = ::GetModuleHandle(nullptr);
    }
    return hModule;
}

HDC NativeWindow_SDL::GetPaintDC() const
{
    if (!IsWindow()) {
        return nullptr;
    }
    SDL_PropertiesID propID = SDL_GetWindowProperties(m_sdlWindow);
    HDC hDC = (HDC)SDL_GetPointerProperty(propID, SDL_PROP_WINDOW_WIN32_HDC_POINTER, nullptr);
    ASSERT(hDC != nullptr);
    return hDC;
}

#endif //DUI_BUILD_FOR_WIN

#if defined (DUI_BUILD_FOR_LINUX) || defined (DUI_BUILD_FOR_FREEBSD)
bool NativeWindow_SDL::IsVideoDriverX11() const
{
    DString videoDriverName = StringUtil::MakeLowerString(GetVideoDriverName());
    return videoDriverName == _T("x11");
}

bool NativeWindow_SDL::IsVideoDriverWayland() const
{
    DString videoDriverName = StringUtil::MakeLowerString(GetVideoDriverName());
    return videoDriverName == _T("wayland");
}

size_t NativeWindow_SDL::GetX11DisplayPointer() const
{
    if (!IsWindow()) {
        return 0;
    }
    SDL_PropertiesID propID = SDL_GetWindowProperties(m_sdlWindow);
    size_t nWindowDisplay = (size_t)SDL_GetPointerProperty(propID, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
    return nWindowDisplay;
}

uint64_t NativeWindow_SDL::GetX11ScreenNumber() const
{
    if (!IsWindow()) {
        return 0;
    }
    SDL_PropertiesID propID = SDL_GetWindowProperties(m_sdlWindow);
    uint64_t nScreenNumber = (uint64_t)SDL_GetNumberProperty(propID, SDL_PROP_WINDOW_X11_SCREEN_NUMBER, 0);
    return nScreenNumber;
}

uint64_t NativeWindow_SDL::GetX11WindowNumber() const
{
    if (!IsWindow()) {
        return 0;
    }
    SDL_PropertiesID propID = SDL_GetWindowProperties(m_sdlWindow);
    uint64_t nWindowNumber = (uint64_t)SDL_GetNumberProperty(propID, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
    return nWindowNumber;
}

size_t NativeWindow_SDL::GetWaylandDisplayPointer() const
{
    if (!IsWindow()) {
        return 0;
    }
    SDL_PropertiesID propID = SDL_GetWindowProperties(m_sdlWindow);
    size_t nWaylandDisplay = (size_t)SDL_GetPointerProperty(propID, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr);
    return nWaylandDisplay;
}

#endif

#if defined DUI_BUILD_FOR_MACOS
/** Gets the NSView* pointer
*/
void* NativeWindow_SDL::GetNSView() const
{
    if (!IsWindow()) {
        return nullptr;
    }
    return GetSDLWindowContentView(m_sdlWindow);
}

/** Gets the NSWindow* pointer
*/
void* NativeWindow_SDL::GetNSWindow() const
{
    if (!IsWindow()) {
        return nullptr;
    }
    SDL_PropertiesID propID = ::SDL_GetWindowProperties(m_sdlWindow);
    return ::SDL_GetPointerProperty(propID, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
}
#endif

void NativeWindow_SDL::CloseWnd(int32_t nRet)
{
    m_closeParam = nRet;
    m_bCloseing = true;

    //Close asynchronously
    if (m_sdlWindow != nullptr) {
        SDL_Event sdlEvent;
        sdlEvent.type = SDL_EVENT_WINDOW_CLOSE_REQUESTED;
        sdlEvent.common.timestamp = 0;
        sdlEvent.window.data1 = 0;
        sdlEvent.window.data2 = 0;
        sdlEvent.window.windowID = SDL_GetWindowID(m_sdlWindow);
        bool nRetE = SDL_PushEvent(&sdlEvent);
        ASSERT_UNUSED_VARIABLE(nRetE);
    }
}

void NativeWindow_SDL::Close()
{
    //Close synchronously
    m_bCloseing = true;
    if (m_pOwner == nullptr) {
        return;
    }

    //Calls back the close event
    bool bHandled = false;
    m_pOwner->OnNativeWindowCloseMsg(0, NativeMsg(0, 0, 0), bHandled);
    if (bHandled) {
        //The close event was intercepted
        m_bCloseing = false;
        return;
    }

    //PreClose event
    if (m_pOwner) {
        m_pOwner->OnNativePreCloseWindow();
    }

    //Closes the window
    ClearNativeWindow();
}

bool NativeWindow_SDL::IsClosingWnd() const
{
    return m_bCloseing;
}

int32_t NativeWindow_SDL::GetCloseParam() const
{
    return m_closeParam;
}

bool NativeWindow_SDL::IsLayeredWindow() const
{
    return m_bIsLayeredWindow;
}

void NativeWindow_SDL::SetLayeredWindowAlpha(int32_t nAlpha)
{
    ASSERT(nAlpha >= 0 && nAlpha <= 255);
    if ((nAlpha < 0) || (nAlpha > 255)) {
        return;
    }
    m_nLayeredWindowAlpha = static_cast<uint8_t>(nAlpha);
}

uint8_t NativeWindow_SDL::GetLayeredWindowAlpha() const
{
    return m_nLayeredWindowAlpha;
}

void NativeWindow_SDL::SetLayeredWindowOpacity(int32_t nAlpha)
{
    ASSERT(nAlpha >= 0 && nAlpha <= 255);
    if ((nAlpha < 0) || (nAlpha > 255)) {
        return;
    }
    ASSERT(IsWindow());
    if (!IsWindow()) {
        return;
    }
    m_nLayeredWindowOpacity = static_cast<uint8_t>(nAlpha);
    float opacity = 1.0f;//Completely opaque
    if (m_nLayeredWindowOpacity != 255) {
        //Opacity setting is supported
        opacity = 1.0f * nAlpha / 255.0f;
    }
    bool nRet = SDL_SetWindowOpacity(m_sdlWindow, opacity);
    ASSERT_UNUSED_VARIABLE(nRet);
}

uint8_t NativeWindow_SDL::GetLayeredWindowOpacity() const
{
    return m_nLayeredWindowOpacity;
}

void NativeWindow_SDL::SetUseSystemCaption(bool bUseSystemCaption)
{
    ASSERT(!IsChildWindow());
    if (IsChildWindow()) {
        //In child window mode, the system title bar is not supported
        return;
    }
    m_bUseSystemCaption = bUseSystemCaption;

#ifndef DUI_BUILD_FOR_WIN
    //Currently, only the OpenGLES2 Render supports window translucency on Linux, so if it is not supported, the system title bar is forced to be used
    bool bHasOpenGL = false;
    bool bOpenGLES2 = false;
    bool bSupportTransparent = false;
    QueryRenderProperties(_T(""), bHasOpenGL, bOpenGLES2, bSupportTransparent);
    if (!bSupportTransparent) {
        m_bUseSystemCaption = true;
    }
#endif

    if (IsUseSystemCaption()) {
        //Use the system default title bar; the title bar style needs to be added
        if (IsWindow()) {
            bool nRet = SDL_SetWindowBordered(m_sdlWindow, true);
            ASSERT_UNUSED_VARIABLE(nRet);
        }
        //Turn off the layered window
        if (IsLayeredWindow()) {
            SetLayeredWindow(false, false);
        }

        //Set the Hit Test function to the default
        SDL_SetWindowHitTest(m_sdlWindow, nullptr, nullptr);
    }
    else {
        //This attribute needs to be set in advance so that the window resize function is kept when there is no border
        SDL_SetHint("SDL_BORDERLESS_RESIZABLE_STYLE", "true"); 
        bool nRet = SDL_SetWindowBordered(m_sdlWindow, false);
        ASSERT_UNUSED_VARIABLE(nRet);

        //Set the HitTest function
        nRet = SDL_SetWindowHitTest(m_sdlWindow, NativeWindow_SDL_HitTest, this);
        ASSERT_UNUSED_VARIABLE(nRet);
    }
    m_pOwner->OnNativeUseSystemCaptionBarChanged();
}

bool NativeWindow_SDL::IsUseSystemCaption() const
{
    return m_bUseSystemCaption;
}

bool NativeWindow_SDL::ShowWindow(ShowWindowCommands nCmdShow)
{
    ASSERT(IsWindow());
    if (m_sdlWindow == nullptr) {
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
    bool nRet = false;
    switch(nCmdShow)
    {
    case kSW_HIDE:
        nRet = SDL_HideWindow(m_sdlWindow);
        break;
    case kSW_SHOW:
    case kSW_SHOW_NORMAL:
        {
            const bool bActivate = SDL_GetHintBoolean(SDL_HINT_WINDOW_ACTIVATE_WHEN_SHOWN, true);
            if (!bActivate) {
                SDL_SetHint(SDL_HINT_WINDOW_ACTIVATE_WHEN_SHOWN, "true");
                ASSERT(SDL_GetHintBoolean(SDL_HINT_WINDOW_ACTIVATE_WHEN_SHOWN, true) == true);
            }
            nRet = SDL_ShowWindow(m_sdlWindow);
            if (!bActivate) {
                SDL_SetHint(SDL_HINT_WINDOW_ACTIVATE_WHEN_SHOWN, "false");
                ASSERT(SDL_GetHintBoolean(SDL_HINT_WINDOW_ACTIVATE_WHEN_SHOWN, true) == false);
            }
            //Adjust the Z-Order of the window to avoid the issue of the window sometimes not showing
            SDL_RaiseWindow(m_sdlWindow);
        }        
        break;
    case kSW_SHOW_NA:
    case kSW_SHOW_NOACTIVATE:
        {
            const bool bActivate = SDL_GetHintBoolean(SDL_HINT_WINDOW_ACTIVATE_WHEN_SHOWN, true);
            if (bActivate) {
                SDL_SetHint(SDL_HINT_WINDOW_ACTIVATE_WHEN_SHOWN, "false");
                ASSERT(SDL_GetHintBoolean(SDL_HINT_WINDOW_ACTIVATE_WHEN_SHOWN, true) == false);
            }
            nRet = SDL_ShowWindow(m_sdlWindow);
            if (bActivate) {
                SDL_SetHint(SDL_HINT_WINDOW_ACTIVATE_WHEN_SHOWN, "true");
                ASSERT(SDL_GetHintBoolean(SDL_HINT_WINDOW_ACTIVATE_WHEN_SHOWN, true) == true);
            }
        }
        break;
    case kSW_MINIMIZE:
    case kSW_SHOW_MINIMIZED:
        nRet = SDL_MinimizeWindow(m_sdlWindow);
        break;
    case kSW_SHOW_MAXIMIZED:
        nRet = SDL_MaximizeWindow(m_sdlWindow);
        break;
    case kSW_SHOW_MIN_NOACTIVE:
        //"No Active" is not supported
        nRet = SDL_MinimizeWindow(m_sdlWindow);
        break;
    case kSW_RESTORE:
        nRet = SDL_RestoreWindow(m_sdlWindow);
        break;
    default:
        ASSERT(false);
        break;
    }
    return nRet;
}

void NativeWindow_SDL::ShowModalFake(NativeWindow_SDL* pParentWindow)
{
    ASSERT(IsWindow());
    if (!IsWindow()) {
        return;
    }
    ASSERT(!IsChildWindow());
    if (IsChildWindow()) {
        return;
    }
    if ((pParentWindow != nullptr) && !pParentWindow->IsWindow()) {
        pParentWindow = nullptr;
    }
    if (pParentWindow != nullptr) {
        if (pParentWindow != nullptr) {
            pParentWindow->EnableWindow(false);
        }
    }
    ShowWindow(kSW_SHOW_NORMAL);
    if (pParentWindow != nullptr) {
        bool nRet = SDL_SetWindowParent(m_sdlWindow, pParentWindow->m_sdlWindow);
        ASSERT_UNUSED_VARIABLE(nRet);

        nRet = SDL_SetWindowModal(m_sdlWindow, true);
        ASSERT_UNUSED_VARIABLE(nRet);
        m_bFakeModal = true;
    }    
}

void NativeWindow_SDL::OnCloseModalFake(NativeWindow_SDL* pParentWindow)
{
    if (IsFakeModal()) {
        if (pParentWindow != nullptr) {
            pParentWindow->EnableWindow(true);
            pParentWindow->SetWindowFocus();
        }
        m_bFakeModal = false;
    }
}

bool NativeWindow_SDL::IsFakeModal() const
{
    return m_bFakeModal;
}

bool NativeWindow_SDL::IsDoModal() const
{
    return m_bDoModal;
}

void NativeWindow_SDL::CenterWindow()
{  
    ASSERT(IsWindow());
    SDL_Window* pCenterWindow = SDL_GetWindowParent(m_sdlWindow);
    int32_t xLeft = 0;
    int32_t yTop = 0;
    if (CalculateCenterWindowPos(pCenterWindow, xLeft, yTop)) {
        SetWindowPos(nullptr, InsertAfterFlag(), xLeft, yTop, -1, -1, kSWP_NOSIZE | kSWP_NOZORDER | kSWP_NOACTIVATE);
    }    
}

bool NativeWindow_SDL::CalculateCenterWindowPos(SDL_Window* pCenterWindow, int32_t& xPos, int32_t& yPos) const
{
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
    GetMonitorRect(pCenterWindow != nullptr ? pCenterWindow : m_sdlWindow, rcMonitor, rcArea);
    if (pCenterWindow == nullptr) {
        rcCenter = rcArea;
    }
    else if (SDL_GetWindowFlags(pCenterWindow) & SDL_WINDOW_MINIMIZED) {
        rcCenter = rcArea;
    }
    else {
        GetWindowRect(pCenterWindow, rcCenter);
    }

    // Find dialog's upper left based on rcCenter
    int32_t xLeft = rcCenter.CenterX() - nWindowWidth / 2;
    int32_t yTop = rcCenter.CenterY() - nWindowHeight / 2;

    // The dialog is outside the screen, move it inside
    if (xLeft < rcArea.left) {
        xLeft = rcArea.left;
    }
    else if (xLeft + nWindowWidth > rcArea.right) {
        xLeft = rcArea.right - nWindowWidth;
    }
    if (yTop < rcArea.top) {
        yTop = rcArea.top;
    }
    else if (yTop + nWindowHeight > rcArea.bottom) {
        yTop = rcArea.bottom - nWindowHeight;
    }
    xPos = xLeft;
    yPos = yTop;
    return true;
}

void NativeWindow_SDL::SetWindowAlwaysOnTop(bool bOnTop)
{
    ASSERT(IsWindow() && !IsChildWindow());
    if (!IsWindow() || IsChildWindow()) {
        return;
    }
    bool nRet = SDL_SetWindowAlwaysOnTop(m_sdlWindow, bOnTop ? true : false);
    ASSERT_UNUSED_VARIABLE(nRet);
}

bool NativeWindow_SDL::IsWindowAlwaysOnTop() const
{
    ASSERT(IsWindow());
    if (!IsWindow()) {
        return false;
    }
    SDL_WindowFlags nFlags = SDL_GetWindowFlags(m_sdlWindow);
    return (nFlags & SDL_WINDOW_ALWAYS_ON_TOP) ? true : false;
}

bool NativeWindow_SDL::SetWindowForeground()
{
    ASSERT(IsWindow());
    if (!IsWindow()) {
        return false;
    }
    bool nRet = SDL_RaiseWindow(m_sdlWindow);
    ASSERT(nRet);
//#ifdef _DEBUG
//    Note: sometimes the condition here does not hold; no functional impact has been found so far
//    auto pKeyboardFocus = SDL_GetKeyboardFocus();
//    ASSERT(pKeyboardFocus == m_sdlWindow);
//#endif
    return nRet;
}

bool NativeWindow_SDL::IsWindowForeground() const
{
    if (!IsWindow()) {
        return false;
    }
    return (SDL_GetKeyboardFocus() == m_sdlWindow) ? true : false;
}

bool NativeWindow_SDL::SetWindowFocus()
{
    ASSERT(IsWindow());
    if (!IsWindow()) {
        return false;
    }
    SDL_Window* pKeyboardFocus = SDL_GetKeyboardFocus();
    if (pKeyboardFocus != m_sdlWindow) {
        SetWindowForeground();
        pKeyboardFocus = SDL_GetKeyboardFocus();
    }
    return (pKeyboardFocus == m_sdlWindow) ? true : false;
}

bool NativeWindow_SDL::IsWindowFocused() const
{
    if (!IsWindow()) {
        return false;
    }
    return (SDL_GetKeyboardFocus() == m_sdlWindow) ? true : false;
}

void NativeWindow_SDL::CheckSetWindowFocus()
{
    if (!IsWindowFocused()) {
        SetWindowFocus();
    }
#if defined (DUI_BUILD_FOR_WIN)
    //When a child window exists, the focus window obtained by SDL is problematic and incorrect, so additional checks are needed (this affects RichEdit input)
    HWND hWnd = GetHWND();
    if (::GetFocus() != hWnd) {
        ::SetFocus(hWnd);
    }
#elif defined (DUI_BUILD_FOR_MACOS)
    SetFocus_MacOS(GetNSWindow());
#elif (defined (DUI_BUILD_FOR_LINUX) || defined (DUI_BUILD_FOR_FREEBSD)) && !defined(DUI_BUILD_FOR_WAYLAND)
    SetFocus_Linux(GetX11WindowNumber());
#endif
}

LRESULT NativeWindow_SDL::PostMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    //ASSERT(IsWindow());
    if (!IsWindow()) {
        return -1;
    }

    ASSERT((uMsg > SDL_EVENT_USER) && (uMsg < SDL_EVENT_LAST));
    if ((uMsg <= SDL_EVENT_USER) || (uMsg >= SDL_EVENT_LAST)) {
        return -1;
    }
    
    SDL_Event sdlEvent;
    sdlEvent.type = uMsg;
    sdlEvent.common.timestamp = 0;
    sdlEvent.user.type = uMsg;
    sdlEvent.user.reserved = 0;
    sdlEvent.user.timestamp = 0;
    sdlEvent.user.code = uMsg;
    sdlEvent.user.data1 = (void*)wParam;
    sdlEvent.user.data2 = (void*)lParam;
    sdlEvent.user.windowID = SDL_GetWindowID(m_sdlWindow);
    bool nRet = SDL_PushEvent(&sdlEvent);
    ASSERT(nRet);
    return nRet ? 0 : -1;
}

void NativeWindow_SDL::PostQuitMsg(int32_t /*nExitCode*/)
{
    SDL_Event sdlEvent;
    sdlEvent.type = SDL_EVENT_QUIT;
    sdlEvent.common.timestamp = 0;
    bool nRet = SDL_PushEvent(&sdlEvent);
    ASSERT_UNUSED_VARIABLE(nRet);
}

bool NativeWindow_SDL::EnterFullscreen()
{
    ASSERT(IsWindow());
    if (m_sdlWindow == nullptr) {
        return false;
    }
    if (IsWindowMinimized() || IsChildWindow()) {
        //Minimized or child windows are not allowed to activate fullscreen
        return false;
    }
    if (m_bFullscreen) {
        return true;
    }
    m_bFullscreen = true;
    m_bFullscreenExiting = false;
    m_bFullscreenMaximized = IsWindowMaximized();
    if (m_bFullscreenMaximized) {
        //If the window is maximized, restore it first, then enter fullscreen (because on some platforms exiting fullscreen after entering from maximized state misbehaves); re-maximize when exiting fullscreen
        SDL_RestoreWindow(m_sdlWindow);
    }

    m_lastWindowFlags = ::SDL_GetWindowFlags(m_sdlWindow);
    if (m_lastWindowFlags & SDL_WINDOW_RESIZABLE) {
        //Need to remove the resizable window attribute
        SDL_SetWindowResizable(m_sdlWindow, false);
    }

#if defined (__APPLE__)
    //Set a specific display mode so SDL uses exclusive fullscreen (bypassing
    //Spaces), avoiding the fullscreen stretching animation and ensuring
    //fullscreen works for borderless windows on macOS.
    SDL_DisplayID displayID = SDL_GetDisplayForWindow(m_sdlWindow);
    if (displayID != 0) {
        const SDL_DisplayMode* desktopMode = SDL_GetDesktopDisplayMode(displayID);
        if (desktopMode) {
            SDL_SetWindowFullscreenMode(m_sdlWindow, desktopMode);
        }
    }
#endif

    bool nRet = SDL_SetWindowFullscreen(m_sdlWindow, true);
    ASSERT_UNUSED_VARIABLE(nRet);

    m_pOwner->OnNativeWindowEnterFullscreen();
    return true;
}

bool NativeWindow_SDL::ExitFullscreen()
{
    ASSERT(IsWindow());
    if (m_sdlWindow == nullptr) {
        return false;
    }
    if (!m_bFullscreen) {
        return false;
    }
    if (m_bFullscreenExiting) {
        return false;
    }
    m_bFullscreenExiting = true;//Avoid entering the exit flow repeatedly

    bool nRet = SDL_SetWindowFullscreen(m_sdlWindow, false);
    ASSERT_UNUSED_VARIABLE(nRet);

#if defined (__APPLE__)
    //Clear the exclusive fullscreen mode, restoring default desktop fullscreen behavior (Spaces)
    SDL_SetWindowFullscreenMode(m_sdlWindow, nullptr);
#endif

    if (m_lastWindowFlags & SDL_WINDOW_RESIZABLE) {
        //Need to restore the resizable window attribute
        SDL_SetWindowResizable(m_sdlWindow, true);
    }

    m_bFullscreen = false;
    m_bFullscreenExiting = false;

    if (m_bFullscreenMaximized) {
        m_bFullscreenMaximized = false;
        //If the window is maximized, restore it first, then enter fullscreen (because on some platforms exiting fullscreen after entering from maximized state misbehaves); re-maximize when exiting fullscreen
        SDL_MaximizeWindow(m_sdlWindow);
    }
    m_pOwner->OnNativeWindowExitFullscreen();
    return true;
}

bool NativeWindow_SDL::IsWindowMaximized() const
{
    ASSERT(IsWindow());
    bool bWindowMaximized = false;
    if (m_sdlWindow != nullptr) {
        SDL_WindowFlags nFlags = SDL_GetWindowFlags(m_sdlWindow);
        bWindowMaximized = nFlags & SDL_WINDOW_MAXIMIZED;
    }
    return bWindowMaximized;
}

bool NativeWindow_SDL::IsWindowMinimized() const
{
    ASSERT(IsWindow());
    bool bWindowMinimized = false;
    if (m_sdlWindow != nullptr) {
        SDL_WindowFlags nFlags = SDL_GetWindowFlags(m_sdlWindow);
        bWindowMinimized = nFlags & SDL_WINDOW_MINIMIZED;
    }
    return bWindowMinimized;
}

bool NativeWindow_SDL::IsWindowFullscreen() const
{
    ASSERT(IsWindow());
    bool bFullscreen = false;
    if (m_sdlWindow != nullptr) {
        SDL_WindowFlags nFlags = SDL_GetWindowFlags(m_sdlWindow);
        bFullscreen = nFlags & SDL_WINDOW_FULLSCREEN;
    }
    return bFullscreen;
}

bool NativeWindow_SDL::IsWindowVisible() const
{
    if (!IsWindow()) {
        return false;
    }
    SDL_WindowFlags windowFlags = SDL_GetWindowFlags(m_sdlWindow);
    return (windowFlags & SDL_WINDOW_HIDDEN) ? false : true;
}

bool NativeWindow_SDL::SetWindowPos(const NativeWindow_SDL* pInsertAfterWindow,
                                   InsertAfterFlag insertAfterFlag,
                                   int32_t X, int32_t Y, int32_t cx, int32_t cy,
                                   uint32_t uFlags)
{
    ASSERT(m_sdlWindow != nullptr);
    if (m_sdlWindow == nullptr) {
        return false;
    }
    bool bRet = true;
    bool bModified = false;
    ASSERT(IsWindow());
    if (!(uFlags & kSWP_NOMOVE)) {
        if (!m_bInitWindowPosFlag) {
            m_ptInitWindow.x = X;
            m_ptInitWindow.y = Y;
        }
        //Wayland: this function cannot modify the window position; it is not supported internally
        //Wayland: setting the initial coordinate values of the window is not supported
        bool nRet = SDL_SetWindowPosition(m_sdlWindow, X, Y);
        ASSERT_UNUSED_VARIABLE(nRet);
        if (!nRet) {
            bRet = false;
        }
        else {
            bModified = true;
        }
    }
    if (!(uFlags & kSWP_NOSIZE)) {
        if (!m_bInitWindowPosFlag) {
            m_szInitWindow.cx = cx;
            m_szInitWindow.cy = cy;
        }
        bool nRet = SDL_SetWindowSize(m_sdlWindow, cx, cy);
        ASSERT_UNUSED_VARIABLE(nRet);
        if (!nRet) {
            bRet = false;
        }
        else {
            bModified = true;
        }
    }
    //Sync the window
    if (bModified) {
        SDL_SyncWindow(m_sdlWindow);
        bModified = false;
    }
    if (uFlags & kSWP_HIDEWINDOW) {
        if (!ShowWindow(ShowWindowCommands::kSW_HIDE)) {
            bRet = false;
        }
        else {
            bModified = true;
        }
    }
    if (uFlags & kSWP_SHOWWINDOW) {
        ShowWindowCommands showCommand = ShowWindowCommands::kSW_SHOW;
        if (uFlags & kSWP_NOACTIVATE) {
            showCommand = ShowWindowCommands::kSW_SHOW_NA;
        }
        if (!ShowWindow(showCommand)) {
            bRet = false;
        }
        else {
            bModified = true;
        }
    }
    if (!(uFlags & kSWP_NOZORDER) && (pInsertAfterWindow == nullptr)) {
        //Only the case where pInsertAfterWindow is nullptr is supported
        if (insertAfterFlag == InsertAfterFlag::kHWND_TOPMOST) {
            SDL_SetWindowAlwaysOnTop(m_sdlWindow, true);
        }
        else if (insertAfterFlag == InsertAfterFlag::kHWND_NOTOPMOST) {
            SDL_SetWindowAlwaysOnTop(m_sdlWindow, false);
        }
        else if (insertAfterFlag == InsertAfterFlag::kHWND_TOP) {
            bool bForce = SDL_GetHintBoolean(SDL_HINT_FORCE_RAISEWINDOW, false);
            bool bActivate = SDL_GetHintBoolean(SDL_HINT_WINDOW_ACTIVATE_WHEN_RAISED, true);
            SDL_SetHint(SDL_HINT_FORCE_RAISEWINDOW, "false");
            SDL_SetHint(SDL_HINT_WINDOW_ACTIVATE_WHEN_RAISED, "false");
            SDL_RaiseWindow(m_sdlWindow);
            SDL_SetHint(SDL_HINT_FORCE_RAISEWINDOW, bForce ? "true" : "false");
            SDL_SetHint(SDL_HINT_WINDOW_ACTIVATE_WHEN_RAISED, bActivate ? "true" : "false");
        }
    }
    if (bRet) {
        SDL_SyncWindow(m_sdlWindow);
    }    
    return bRet;
}

bool NativeWindow_SDL::MoveWindow(int32_t X, int32_t Y, int32_t nWidth, int32_t nHeight, bool bRepaint)
{
    ASSERT(IsWindow());
    bool bRet = SDL_SetWindowPosition(m_sdlWindow, X, Y);
    ASSERT_UNUSED_VARIABLE(bRet);
    bool nRet = SDL_SetWindowSize(m_sdlWindow, nWidth, nHeight);
    ASSERT_UNUSED_VARIABLE(nRet);
    if (!nRet) {
        bRet = false;
    }
    SDL_SyncWindow(m_sdlWindow);
    if (bRepaint) {
        UiRect rect;
        GetClientRect(rect);
        Invalidate(rect);
    }
    return nRet;
}

void NativeWindow_SDL::SetText(const DString& strText)
{
    ASSERT(IsWindow());
    //Converts to UTF-8 encoding
    DStringA utf8Text = StringConvert::TToUTF8(strText);
    bool nRet = SDL_SetWindowTitle(m_sdlWindow, utf8Text.c_str());
    ASSERT_UNUSED_VARIABLE(nRet);
}

DString NativeWindow_SDL::GetText() const
{
    DString windowText;
    if (m_sdlWindow != nullptr) {
        const char* szText = SDL_GetWindowTitle(m_sdlWindow);
        if (szText != nullptr) {
            windowText = StringConvert::UTF8ToT(szText);
        }
    }
    return windowText;
}

void NativeWindow_SDL::SetWindowMaximumSize(const UiSize& szMaxWindow)
{
    m_szMaxWindow = szMaxWindow;
    if (m_szMaxWindow.cx < 0) {
        m_szMaxWindow.cx = 0;
    }
    if (m_szMaxWindow.cy < 0) {
        m_szMaxWindow.cy = 0;
    }
    ASSERT(IsWindow());
    if (m_sdlWindow != nullptr) {
        SDL_SetWindowMaximumSize(m_sdlWindow, m_szMaxWindow.cx, m_szMaxWindow.cy);
    }
}

const UiSize& NativeWindow_SDL::GetWindowMaximumSize() const
{
    return m_szMaxWindow;
}

void NativeWindow_SDL::SetWindowMinimumSize(const UiSize& szMinWindow)
{
    m_szMinWindow = szMinWindow;
    if (m_szMinWindow.cx < 0) {
        m_szMinWindow.cx = 0;
    }
    if (m_szMinWindow.cy < 0) {
        m_szMinWindow.cy = 0;
    }
    ASSERT(IsWindow());
    if (m_sdlWindow != nullptr) {
        SDL_SetWindowMinimumSize(m_sdlWindow, m_szMinWindow.cx, m_szMinWindow.cy);
    }
}

const UiSize& NativeWindow_SDL::GetWindowMinimumSize() const
{
    return m_szMinWindow;
}

void NativeWindow_SDL::SetCapture()
{
    //ASSERT(SDL_GetMouseFocus() == m_sdlWindow);
    if (SDL_GetMouseFocus() == m_sdlWindow) {
        bool nRet = SDL_CaptureMouse(true);
        if (!nRet) {
            const char* szErrorMsg = SDL_GetError();
            ASSERT_UNUSED_VARIABLE(nRet);
            UNUSED_VARIABLE(szErrorMsg);
        }        
        if (nRet) {
            m_bMouseCapture = true;
        }
    }
}

void NativeWindow_SDL::ReleaseCapture()
{
    if (m_bMouseCapture) {
        ASSERT(SDL_GetMouseFocus() == m_sdlWindow);
        if (SDL_GetMouseFocus() == m_sdlWindow) {
            bool nRet = SDL_CaptureMouse(false);
            ASSERT_UNUSED_VARIABLE(nRet);
        }
        m_bMouseCapture = false;
    }
}

bool NativeWindow_SDL::IsCaptured() const
{
    return m_bMouseCapture;
}

struct NativeWindowExposedEvent
{
    SDL_Window* m_sdlWindow = nullptr;
    bool m_bFoundExposedEvent = false;
};

static bool SDLCALL FilterNativeWindowExposedEvent(void* userdata, SDL_Event* event)
{
    if ((userdata != nullptr) && (event != nullptr)) {
        NativeWindowExposedEvent* data = (NativeWindowExposedEvent*)userdata;
        if (!data->m_bFoundExposedEvent && (event->type == WM_USER_PAINT_MSG)) {
            if (data->m_sdlWindow == SDL_GetWindowFromEvent(event)) {
                data->m_bFoundExposedEvent = true;
            }
        }
    }
    return true;
}

void NativeWindow_SDL::Invalidate(const UiRect& rcItem)
{
    if (m_rcUpdateRect.IsZero()) {
        m_rcUpdateRect = rcItem;
    }
    else {
        m_rcUpdateRect.Union(rcItem);
    }

    //This feature does not exist yet; only a paint message can be sent to trigger UI drawing
    if (m_sdlWindow != nullptr) {
        NativeWindowExposedEvent data;
        data.m_bFoundExposedEvent = false;
        data.m_sdlWindow = m_sdlWindow;
        SDL_FilterEvents(FilterNativeWindowExposedEvent, &data);
        if (!data.m_bFoundExposedEvent) {
            //If the queue has no paint message for this window, add one; if it already has one, do not add again to avoid redundant painting that hurts performance
            SDL_Event sdlEvent;
            sdlEvent.type = WM_USER_PAINT_MSG;
            sdlEvent.common.timestamp = 0;
            sdlEvent.user.data1 = 0;
            sdlEvent.user.data2 = 0;
            sdlEvent.user.windowID = SDL_GetWindowID(m_sdlWindow);
            bool nRet = SDL_PushEvent(&sdlEvent);
            ASSERT_UNUSED_VARIABLE(nRet);
        }
    }
}

void NativeWindow_SDL::PaintWindow(bool bPaintAll)
{
    PerformanceStat statPerformance(_T("PaintWindow, NativeWindow_SDL::PaintWindow(Total)"));
    if (bPaintAll) {
        //Draw everything
        m_rcUpdateRect.Clear();
    }
    INativeWindow* pOwner = m_pOwner;
    ASSERT(pOwner != nullptr);
    if (pOwner == nullptr) {
        return;
    }
    //Interface lifetime flag
    std::weak_ptr<WeakFlag> ownerFlag = pOwner->GetWeakFlag();
    bool bPaint = pOwner->OnNativePreparePaint();
    if (bPaint && !ownerFlag.expired()) {
        if (IsChildWindow()) {
            //Child window mode: the application layer is fully responsible for drawing
            if (pOwner != nullptr) {
                UiRect rcPaint = GetUpdateRect();
                bool bHandled = false;
                NativeMsg nativeMsg = NativeMsg(SDL_EVENT_WINDOW_EXPOSED, (WPARAM)m_sdlWindow, 0);
                pOwner->OnNativePaintMsg(rcPaint, nativeMsg, bHandled);
            }
        }
        else {
            //Normal mode: the internal drawing pipeline is managed internally
            IRender* pRender = pOwner->OnNativeGetRender();
            ASSERT(pRender != nullptr);
            if ((pRender != nullptr) && !ownerFlag.expired()) {
                NativeWindowRenderPaint renderPaint;
                renderPaint.m_pNativeWindow = this;
                renderPaint.m_pOwner = pOwner;
                renderPaint.m_nativeMsg = NativeMsg(SDL_EVENT_WINDOW_EXPOSED, 0, 0);
                renderPaint.m_bHandled = false;
                bPaint = pRender->PaintAndSwapBuffers(&renderPaint);
            }
        }
    }
    m_rcUpdateRect.Clear();
}

const UiRect& NativeWindow_SDL::GetUpdateRect() const
{
    return m_rcUpdateRect;
}

void NativeWindow_SDL::SetImeOpenStatus(bool bOpen)
{
    if (m_sdlWindow == nullptr) {
        return;
    }    
    if (bOpen) {
        if (!SDL_TextInputActive(m_sdlWindow)) {
            SDL_StartTextInput(m_sdlWindow);
            SDL_SetTextInputArea(m_sdlWindow, nullptr, 0);
        }        
    }
    else {
        SDL_SetTextInputArea(m_sdlWindow, nullptr, 0);
        SDL_StopTextInput(m_sdlWindow);        
    }
}

void NativeWindow_SDL::SetTextInputArea(const UiRect* rect, int32_t nCursor)
{
    if (m_sdlWindow == nullptr) {
        return;
    }
    if (rect == nullptr) {
        SDL_SetTextInputArea(m_sdlWindow, nullptr, nCursor);
    }
    else {
        SDL_Rect sdlRect;
        sdlRect.x = rect->left;
        sdlRect.y = rect->top;
        sdlRect.w = rect->Width();
        sdlRect.h = rect->Height();
        SDL_SetTextInputArea(m_sdlWindow, &sdlRect, nCursor);
    }
}

void NativeWindow_SDL::GetClientRect(UiRect& rcClient) const
{
    rcClient.Clear();
    ASSERT(IsWindow());
    int nWidth = 0;
    int nHeight = 0;
    bool nRet = false;
    if (GlobalManager::Instance().Dpi().IsPixelDensityEnabled()) {
        nRet = SDL_GetWindowSizeInPixels(m_sdlWindow, &nWidth, &nHeight);
    }
    else {
        nRet = SDL_GetWindowSize(m_sdlWindow, &nWidth, &nHeight);
    }
    
    ASSERT(nRet);
    if (nRet) {
        rcClient.left = 0;
        rcClient.top = 0;
        rcClient.right = rcClient.left + nWidth;
        rcClient.bottom = rcClient.top + nHeight;
    }

#ifdef DUI_HDPI_TEST_PIXEL_DENSITY
    //TEST ONLY
    rcClient.right = (int32_t)std::round(rcClient.left + nWidth * DUI_HDPI_TEST_PIXEL_DENSITY);
    rcClient.bottom = (int32_t)std::round(rcClient.top + nHeight * DUI_HDPI_TEST_PIXEL_DENSITY);
#endif
}

void NativeWindow_SDL::GetWindowRect(UiRect& rcWindow) const
{
    GetWindowRect(m_sdlWindow, rcWindow);
}

void NativeWindow_SDL::GetWindowRect(SDL_Window* sdlWindow, UiRect& rcWindow) const
{
    rcWindow.Clear();
    ASSERT(sdlWindow != nullptr);
    if (sdlWindow == nullptr) {
        return;
    }

    //The top-left corner coordinates of the window (screen coordinates)
    int nXPos = 0;
    int nYPos = 0;
    bool bRet = SDL_GetWindowPosition(sdlWindow, &nXPos, &nYPos);
    if (!bRet) {
        //In a pure Wayland environment, this function cannot get the window position
        nXPos = 0;
        nYPos = 0;
    }

    //The border size
    int nTopBorder = 0;
    int nLeftBorder = 0;
    int nBottomBorder = 0;
    int nRightBorder = 0;
    if (bRet) {
        SDL_GetWindowBordersSize(sdlWindow, &nTopBorder, &nLeftBorder, &nBottomBorder, &nRightBorder);
    }    

    //The client area size
    int nWidth = 0;
    int nHeight = 0;
    SDL_GetWindowSize(sdlWindow, &nWidth, &nHeight);

    rcWindow.left = nXPos - nLeftBorder;
    rcWindow.top = nYPos - nTopBorder;
    rcWindow.right = rcWindow.left + nWidth + nLeftBorder + nRightBorder;
    rcWindow.bottom = rcWindow.top + nHeight + nTopBorder + nBottomBorder;

//#if defined (DUI_BUILD_FOR_WIN) && defined (_DEBUG)
//    {
//        HWND hWnd = GetHWND();
//        if (!::IsIconic(hWnd) && ::IsWindowVisible(hWnd)) {
//            //When minimized or hidden, do not compare; the two are different
//            RECT rect = { 0, };
//            ::GetWindowRect(hWnd, &rect);
//            if (rect.left != -32000) {
//                ASSERT(rcWindow.left == rect.left);
//                ASSERT(rcWindow.top == rect.top);
//                ASSERT(rcWindow.right == rect.right);
//                ASSERT(rcWindow.bottom == rect.bottom);
//            }            
//        }
//    }
//#endif
}

void NativeWindow_SDL::ScreenToClient(UiPoint& pt) const
{
    int nXPos = 0;
    int nYPos = 0;
    bool nRet = SDL_GetWindowPosition(m_sdlWindow, &nXPos, &nYPos);
    ASSERT(nRet);
    if (nRet) {
        pt.x -= nXPos;
        pt.y -= nYPos;
    }

    if ((m_pOwner != nullptr) && m_pOwner->OnNativeGetDpi().HasPixelDensity()) {
        m_pOwner->OnNativeGetDpi().WindowSizeToClientSize(pt);
    }
}

void NativeWindow_SDL::ClientToScreen(UiPoint& pt) const
{
    if ((m_pOwner != nullptr) && m_pOwner->OnNativeGetDpi().HasPixelDensity()) {
        m_pOwner->OnNativeGetDpi().ClientSizeToWindowSize(pt);
    }

#if defined (DUI_BUILD_FOR_WIN) && defined (_DEBUG)
    POINT ptWnd = { pt.x, pt.y };
#endif

    int nXPos = 0;
    int nYPos = 0;
    bool nRet = SDL_GetWindowPosition(m_sdlWindow, &nXPos, &nYPos);
    ASSERT(nRet);
    if (nRet) {
        pt.x += nXPos;
        pt.y += nYPos;
    }
#if defined (DUI_BUILD_FOR_WIN) && defined (_DEBUG)
    {
        HWND hWnd = GetHWND();
        ::ClientToScreen(hWnd, &ptWnd);
        ASSERT(ptWnd.x == pt.x);
        ASSERT(ptWnd.y == pt.y);
    }
#endif
}

void NativeWindow_SDL::GetCursorPos(UiPoint& pt) const
{
    float x = 0;
    float y = 0;
    SDL_GetGlobalMouseState(&x, &y);
    pt.x = (int32_t)x;
    pt.y = (int32_t)y;
}

bool NativeWindow_SDL::GetMonitorRect(UiRect& rcMonitor) const
{
    UiRect rcWork;
    return GetMonitorRect(m_sdlWindow, rcMonitor, rcWork);
}

bool NativeWindow_SDL::GetMonitorRect(SDL_Window* sdlWindow, UiRect& rcMonitor, UiRect& rcWork) const
{
    rcMonitor.Clear();
    rcWork.Clear();

    SDL_DisplayID displayID = 0;
    if (sdlWindow != nullptr) {
        displayID = SDL_GetDisplayForWindow(sdlWindow);
    }
    if (displayID == 0) {
        displayID = SDL_GetPrimaryDisplay();
    }
    ASSERT(displayID != 0);
    if (displayID == 0) {
        return false;
    }

    SDL_Rect rect = {0, };
    bool nRet = SDL_GetDisplayBounds(displayID, &rect);
    ASSERT(nRet);
    if (nRet) {
        rcMonitor.left = rect.x;
        rcMonitor.top = rect.y;
        rcMonitor.right = rcMonitor.left + rect.w;
        rcMonitor.bottom = rcMonitor.top + rect.h;
    }

    nRet = SDL_GetDisplayUsableBounds(displayID, &rect);
    ASSERT(nRet);
    if (nRet) {
        rcWork.left = rect.x;
        rcWork.top = rect.y;
        rcWork.right = rcWork.left + rect.w;
        rcWork.bottom = rcWork.top + rect.h;
    }
    return !rcMonitor.IsEmpty() && !rcWork.IsEmpty();
}

bool NativeWindow_SDL::GetMonitorWorkRect(UiRect& rcWork) const
{
    UiRect rcMonitor;
    return GetMonitorRect(m_sdlWindow, rcMonitor, rcWork);
}

bool NativeWindow_SDL::GetPrimaryMonitorWorkRect(UiRect& rcWork)
{
    rcWork.Clear();
    //Initialize SDL
    if (!MessageLoop_SDL::CheckInitSDL()) {
        return false;
    }

    SDL_DisplayID displayID = SDL_GetPrimaryDisplay();
    ASSERT(displayID != 0);
    if (displayID == 0) {
        return false;
    }
    SDL_Rect rect = { 0, };
    bool nRet = SDL_GetDisplayUsableBounds(displayID, &rect);
    ASSERT(nRet);
    if (nRet) {
        rcWork.left = rect.x;
        rcWork.top = rect.y;
        rcWork.right = rcWork.left + rect.w;
        rcWork.bottom = rcWork.top + rect.h;
    }
    return !rcWork.IsEmpty();
}

bool NativeWindow_SDL::GetMonitorWorkRect(const UiPoint& pt, UiRect& rcWork) const
{
    rcWork.Clear();
    SDL_Point point = {pt.x, pt.y};
    SDL_DisplayID displayID = SDL_GetDisplayForPoint(&point);
    if (displayID == 0) {
        displayID = SDL_GetPrimaryDisplay();
    }
    ASSERT(displayID != 0);
    if (displayID == 0) {
        return false;
    }

    SDL_Rect rect = { 0, };
    bool nRet = SDL_GetDisplayUsableBounds(displayID, &rect);
    ASSERT(nRet);
    if (nRet) {
        rcWork.left = rect.x;
        rcWork.top = rect.y;
        rcWork.right = rcWork.left + rect.w;
        rcWork.bottom = rcWork.top + rect.h;
    }
    return !rcWork.IsEmpty();
}

const UiPoint& NativeWindow_SDL::GetLastMousePos() const
{
    return m_ptLastMousePos;
}

void NativeWindow_SDL::SetLastMousePos(const UiPoint& pt)
{
    m_ptLastMousePos = pt;
}

INativeWindow* NativeWindow_SDL::WindowBaseFromPoint(const UiPoint& pt, bool /*bIgnoreChildWindow*/)
{
    SDL_Window* pKeyboardFocus = SDL_GetKeyboardFocus();
    if (pKeyboardFocus != nullptr) {
        SDL_Window* sdlWindow = pKeyboardFocus;
        UiRect rcWindow;
        GetWindowRect(sdlWindow, rcWindow);
        if (rcWindow.ContainsPt(pt)) {
            NativeWindow_SDL* pWindow = GetWindowFromID(SDL_GetWindowID(sdlWindow));
            if ((pWindow != nullptr) && !pWindow->IsClosingWnd()) {
                return pWindow->m_pOwner;
            }
        }
    }
    SDL_Window* pMouseFocus = SDL_GetMouseFocus();
    if (pMouseFocus != nullptr) {
        SDL_Window* sdlWindow = pMouseFocus;
        UiRect rcWindow;
        GetWindowRect(sdlWindow, rcWindow);
        if (rcWindow.ContainsPt(pt)) {
            NativeWindow_SDL* pWindow = GetWindowFromID(SDL_GetWindowID(sdlWindow));
            if ((pWindow != nullptr) && !pWindow->IsClosingWnd()) {
                return pWindow->m_pOwner;
            }
        }
    }

    int nCount = 0;
    SDL_Window* const* pWindowList = SDL_GetWindows(&nCount);
    if ((nCount <= 0) || (pWindowList == nullptr)){
        return nullptr;
    }
    //TODO: the Z-Order issue is not considered
    for (int nIndex = 0; nIndex < nCount; ++nIndex) {
        SDL_Window* sdlWindow = pWindowList[nIndex];
        if (sdlWindow != nullptr) {
            UiRect rcWindow;
            GetWindowRect(sdlWindow, rcWindow);
            if (rcWindow.ContainsPt(pt)) {
                NativeWindow_SDL* pWindow = GetWindowFromID(SDL_GetWindowID(sdlWindow));
                if ((pWindow != nullptr) && !pWindow->IsClosingWnd()) {
                    return pWindow->m_pOwner;
                }
            }
        }
    }
    return nullptr;
}

void NativeWindow_SDL::OnFinalMessage()
{
    //Cancels the watchers of synchronized window drawing
    SDL_RemoveEventWatch(OnNativeWindowExposedEvent, this);
    if (m_pOwner) {
        m_pOwner->OnNativeFinalMessage();
    }
}

bool NativeWindow_SDL::SetWindowIcon(const FilePath& iconFilePath)
{
    std::vector<uint8_t> fileData;
    bool bRet = FileUtil::ReadFileData(iconFilePath, fileData);
    ASSERT(bRet);
    if (bRet) {
        bRet = SetWindowIcon(fileData, iconFilePath.ToString());
    }
    return bRet;
}

bool NativeWindow_SDL::SetWindowIcon(const std::vector<uint8_t>& iconFileData, const DString& iconFileName)
{
    ASSERT(!iconFileData.empty());
    if (iconFileData.empty()) {
        return false;
    }
    ASSERT(m_pOwner != nullptr);
    if (m_pOwner == nullptr) {
        return false;
    }
    ASSERT(IsWindow());
    if (!IsWindow()) {
        return false;
    }
    ImageDecoderFactory& imageDecoders = GlobalManager::Instance().ImageDecoders();
    float fImageSizeScale = (m_pOwner != nullptr) ? m_pOwner->OnNativeGetDpi().GetDisplayScale() : 1.0f;
    ImageDecodeParam decodeParam;
    decodeParam.m_imageFilePath = iconFileName;
    decodeParam.m_fImageSizeScale = fImageSizeScale;
    decodeParam.m_pFileData = std::make_shared<std::vector<uint8_t>>(iconFileData);
    std::shared_ptr<IBitmap> pBitmap = imageDecoders.DecodeImageData(decodeParam);
    if (pBitmap == nullptr) {
        return false;
    }
    uint32_t nWidth = pBitmap->GetWidth();
    uint32_t nHeight = pBitmap->GetHeight();
    if ((nWidth < 1) || (nHeight < 1)) {
        return false;
    }

    void* pPixelBits = pBitmap->LockPixelBits();
    ASSERT(pPixelBits != nullptr);
    if (pPixelBits == nullptr) {
        return false;
    }

#ifdef DUI_BUILD_FOR_WIN
    SDL_PixelFormat format = SDL_PIXELFORMAT_BGRA32;
#else
    SDL_PixelFormat format = SDL_PIXELFORMAT_RGBA32;
#endif
    SDL_Surface* cursorSurface = SDL_CreateSurfaceFrom(pBitmap->GetWidth(), pBitmap->GetHeight(), format, pPixelBits, pBitmap->GetWidth() * sizeof(uint32_t));
    ASSERT(cursorSurface != nullptr);
    if (cursorSurface == nullptr) {
        return false;
    }

    bool nRet = SDL_SetWindowIcon(m_sdlWindow, cursorSurface);
    SDL_DestroySurface(cursorSurface);
    ASSERT(nRet);
    return nRet;
}

void NativeWindow_SDL::SetEnableDragDrop(bool bEnable)
{
    m_bEnableDragDrop = bEnable;
    if (bEnable) {
        m_pWindowDropTarget = std::make_unique<WindowDropTarget>(this);
    }
    else {
        m_pWindowDropTarget.reset();
    }
}

bool NativeWindow_SDL::IsEnableDragDrop() const
{
    return m_bEnableDragDrop;
}

Control* NativeWindow_SDL::FindControl(const UiPoint& pt) const
{
    return m_pOwner->OnNativeFindControl(pt);
}

bool NativeWindow_SDL::NeedCenterWindowAfterCreated() const
{
    return m_createParam.m_bCenterWindow;
}

bool NativeWindow_SDL::GetWindowSize(int32_t* w, int32_t* h) const
{
    if (m_sdlWindow != nullptr) {
        return SDL_GetWindowSize(m_sdlWindow, w, h);
    }
    return false;
}

bool NativeWindow_SDL::GetWindowSizeInPixels(int32_t* w, int32_t* h) const
{
    if (m_sdlWindow != nullptr) {
        return SDL_GetWindowSizeInPixels(m_sdlWindow, w, h);
    }
    return false;
}

float NativeWindow_SDL::GetDisplayContentScale() const
{
    if (m_sdlWindow != nullptr) {
        return SDL_GetDisplayContentScale(SDL_GetDisplayForWindow(m_sdlWindow));
    }
    return 0.0f;    
}

float NativeWindow_SDL::GetWindowDisplayScale() const
{
    if (m_sdlWindow != nullptr) {
        return SDL_GetWindowDisplayScale(m_sdlWindow);
    }
    return 0.0f;
}

float NativeWindow_SDL::GetWindowPixelDensity() const
{
    if (m_sdlWindow != nullptr) {
        return SDL_GetWindowPixelDensity(m_sdlWindow);
    }
    return 0.0f;
}

bool NativeWindow_SDL::SetLayeredWindow(bool bIsLayeredWindow, bool /*bRedraw*/)
{
    //This feature is not supported
    ASSERT_UNUSED_VARIABLE(bIsLayeredWindow == bIsLayeredWindow);
    //m_bIsLayeredWindow = bIsLayeredWindow;
    //The SDL_WINDOW_TRANSPARENT attribute cannot be modified, so this property is not modifiable; the correct attribute is set when the window is created
    return true;
}

bool NativeWindow_SDL::SetWindowRoundRectRgn(const UiRect& rcWnd, float rx, float ry, bool bRedraw)
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

bool NativeWindow_SDL::SetWindowRectRgn(const UiRect& rcWnd, bool bRedraw)
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

void NativeWindow_SDL::ClearWindowRgn(bool bRedraw)
{
    IRender* pRender = m_pOwner->OnNativeGetRender();
    if (!IsChildWindow()) {
        ASSERT(pRender != nullptr);
    }
    if (pRender != nullptr) {
        pRender->ClearWindowRgn(bRedraw);
    }
}

void NativeWindow_SDL::OnDropBegin()
{
    ControlDropData_SDL data;
    data.m_bHandled = false;
    data.m_ptClientX = 0;
    data.m_ptClientY = 0;
    data.m_bTextData = false;
    m_pOwner->OnNativeDropEnterMsg(kControlDropTypeSDL, &data);
}

void NativeWindow_SDL::OnDropPosition(const UiPoint& pt, bool& bHandled)
{
    ControlDropData_SDL data;
    data.m_bHandled = false;
    data.m_ptClientX = pt.x;
    data.m_ptClientY = pt.y;
    data.m_bTextData = false;
    m_pOwner->OnNativeDropOverMsg(kControlDropTypeSDL, &data);
    bHandled = data.m_bHandled;
}

void NativeWindow_SDL::OnDropTexts(const std::vector<DString>& textList, const UiPoint& pt, bool& bHandled)
{
    ControlDropData_SDL data;
    data.m_bHandled = false;
    data.m_ptClientX = pt.x;
    data.m_ptClientY = pt.y;
    data.m_bTextData = true;
    data.m_textList = textList;
    m_pOwner->OnNativeDropMsg(kControlDropTypeSDL, &data);
    bHandled = data.m_bHandled;
}

void NativeWindow_SDL::OnDropFiles(const DString& source, const std::vector<DString>& fileList, const UiPoint& pt, bool& bHandled)
{
    ControlDropData_SDL data;
    data.m_bHandled = false;
    data.m_ptClientX = pt.x;
    data.m_ptClientY = pt.y;
    data.m_bTextData = false;
    data.m_source = source;
    data.m_fileList = fileList;
    m_pOwner->OnNativeDropMsg(kControlDropTypeSDL, &data);
    bHandled = data.m_bHandled;
}

void NativeWindow_SDL::OnDropLeave()
{
    m_pOwner->OnNativeDropLeaveMsg();
}

bool NativeWindow_SDL::KillWindowFocus()
{
    //This feature is not supported
    return false;
}

bool NativeWindow_SDL::EnableWindow(bool /*bEnable*/)
{
    //This feature is not provided
    return true;
}

bool NativeWindow_SDL::IsWindowEnabled() const
{
    //This feature is not provided
    return true;
}

bool NativeWindow_SDL::UpdateWindow() const
{
    //This feature does not exist
    if (m_sdlWindow != nullptr) {
        SDL_SyncWindow(m_sdlWindow);
    }    
    return true;
}

void NativeWindow_SDL::KeepParentActive()
{
    //This feature is not supported
}

int32_t NativeWindow_SDL::SetWindowHotKey(uint8_t /*wVirtualKeyCode*/, uint8_t /*wModifiers*/)
{
    //This feature is not supported
    ASSERT(0);
    return -1;
}

bool NativeWindow_SDL::GetWindowHotKey(uint8_t& /*wVirtualKeyCode*/, uint8_t& /*wModifiers*/) const
{
    //This feature is not supported
    ASSERT(0);
    return false;
}

bool NativeWindow_SDL::RegisterHotKey(uint8_t /*wVirtualKeyCode*/, uint8_t /*wModifiers*/, int32_t /*id*/)
{
    //This feature is not supported
    ASSERT(0);
    return false;
}

bool NativeWindow_SDL::UnregisterHotKey(int32_t /*id*/)
{
    //This feature is not supported
    ASSERT(0);
    return false;
}

LRESULT NativeWindow_SDL::CallDefaultWindowProc(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    //This feature is not supported
    ASSERT(0);
    return -1;
}

void NativeWindow_SDL::SetEnableSnapLayoutMenu(bool /*bEnable*/)
{
    //This feature is not supported
}

bool NativeWindow_SDL::IsEnableSnapLayoutMenu() const
{
    //This feature is not supported
    return false;
}

void NativeWindow_SDL::SetEnableSysMenu(bool /*bEnable*/)
{
    //This feature is not supported
}

bool NativeWindow_SDL::IsEnableSysMenu() const
{
    //This feature is not supported
    return false;
}

} // namespace ui

#endif //DUI_BUILD_FOR_SDL

//
//LRESULT NativeWindow_SDL::ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
//{
//    LRESULT lResult = 0;
//    bHandled = false;
//    switch (uMsg)
//    {
//
//    case WM_IME_STARTCOMPOSITION://not supported
//    {
//        lResult = m_pOwner->OnNativeImeStartCompositionMsg(NativeMsg(uMsg, wParam, lParam), bHandled);
//        break;
//    }
//    case WM_IME_ENDCOMPOSITION://not supported
//    {
//        lResult = m_pOwner->OnNativeImeEndCompositionMsg(NativeMsg(uMsg, wParam, lParam), bHandled);
//        break;
//    }
//    case WM_SETCURSOR://not supported (needs handling)
//    {
//        if (LOWORD(lParam) == HTCLIENT) {
//            //Only handle setting the client area cursor
//            lResult = m_pOwner->OnNativeSetCursorMsg(NativeMsg(uMsg, wParam, lParam), bHandled);
//        }
//        break;
//    }
//    case WM_CONTEXTMENU://not supported
//    {
//        UiPoint pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
//        if ((pt.x != -1) && (pt.y != -1)) {
//            ScreenToClient(pt);
//        }
//        lResult = m_pOwner->OnNativeContextMenuMsg(pt, NativeMsg(uMsg, wParam, lParam), bHandled);
//        break;
//    }
//    case WM_CHAR:
//    {
//        VirtualKeyCode vkCode = static_cast<VirtualKeyCode>(wParam);
//        uint32_t modifierKey = 0;
//        GetModifiers(uMsg, wParam, lParam, modifierKey);
//        lResult = m_pOwner->OnNativeCharMsg(vkCode, modifierKey, NativeMsg(uMsg, wParam, lParam), bHandled);
//        break;
//    }
//    case WM_HOTKEY://not supported
//    {
//        int32_t hotkeyId = (int32_t)wParam;
//        VirtualKeyCode vkCode = static_cast<VirtualKeyCode>((int32_t)(int16_t)HIWORD(lParam));
//        uint32_t modifierKey = 0;
//        GetModifiers(uMsg, wParam, lParam, modifierKey);
//        lResult = m_pOwner->OnNativeHotKeyMsg(hotkeyId, vkCode, modifierKey, NativeMsg(uMsg, wParam, lParam), bHandled);
//        break;
//    }
//    case WM_MOUSEHOVER://not supported
//    {
//        UiPoint pt;
//        pt.x = GET_X_LPARAM(lParam);
//        pt.y = GET_Y_LPARAM(lParam);
//        uint32_t modifierKey = 0;
//        GetModifiers(uMsg, wParam, lParam, modifierKey);
//        lResult = m_pOwner->OnNativeMouseHoverMsg(pt, modifierKey, NativeMsg(uMsg, wParam, lParam), bHandled);
//        break;
//    }
//
//    case WM_CAPTURECHANGED://not supported
//    {
//        lResult = m_pOwner->OnNativeCaptureChangedMsg(NativeMsg(uMsg, wParam, lParam), bHandled);
//        break;
//    }
//    default:
//        break;
//    }//end of switch
//    return lResult;
//}


//SDL source: 
//if (style & WS_POPUP) {
//    window->flags |= SDL_WINDOW_BORDERLESS;
//}
//else {
//    window->flags &= ~SDL_WINDOW_BORDERLESS;
//}
//if (style & WS_THICKFRAME) {
//    window->flags |= SDL_WINDOW_RESIZABLE;
//}
//else {
//    window->flags &= ~SDL_WINDOW_RESIZABLE;
//}
