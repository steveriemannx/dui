#include "dui/Control/ChildWindowImpl.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/Window.h"
#include "dui/Core/Control.h"

namespace ui
{
ChildWindowImpl::ChildWindowImpl(ChildWindowEvents* pChildWindowEvents):
    m_pChildWindowEvents(pChildWindowEvents)
{
}

ChildWindowImpl::~ChildWindowImpl()
{
}

void ChildWindowImpl::SetChildWindowVisible(bool bVisible)
{
    if (IsWindowVisible() != bVisible) {
        ShowWindow(bVisible ? ShowWindowCommands::kSW_SHOW : ShowWindowCommands::kSW_HIDE);
    }
}

void ChildWindowImpl::SetChildWindowEnabled(bool bEnabled)
{
    if (IsWindowEnabled() != bEnabled) {
        EnableWindow(bEnabled);
    }
}

void ChildWindowImpl::SetChildWindowEvents(ChildWindowEvents* pChildWindowEvents)
{
    m_pChildWindowEvents = pChildWindowEvents;
}

void ChildWindowImpl::PreInitWindow()
{
    //Empty implementation
}

void ChildWindowImpl::OnInitWindow()
{
    //Empty implementation
}

void ChildWindowImpl::PostInitWindow()
{
    //Empty implementation
}

void ChildWindowImpl::OnInitLayout()
{
    //Empty implementation
}

void ChildWindowImpl::PreCloseWindow()
{
    //Empty implementation
}

void ChildWindowImpl::PostCloseWindow()
{
    //Empty implementation
}

void ChildWindowImpl::FinalMessage()
{
    //Empty implementation
}

void ChildWindowImpl::NotifyWindowEnterFullscreen()
{
    //Empty implementation
    ASSERT(0);
}

void ChildWindowImpl::NotifyWindowExitFullscreen()
{
    //Empty implementation
    ASSERT(0);
}

// ------------------------------ Pure virtual functions related to window property changes ------------------------------
void ChildWindowImpl::OnUseSystemCaptionBarChanged()
{
    //Empty implementation
}

bool ChildWindowImpl::OnPreparePaint()
{
    //Empty implementation, by default returns true to continue drawing
    return true;
}

void ChildWindowImpl::OnLayeredWindowChanged()
{
    //Empty implementation
}

void ChildWindowImpl::OnWindowAlphaChanged()
{
    //Empty implementation
}

void ChildWindowImpl::OnWindowEnterFullscreen()
{
    //Empty implementation
    ASSERT(0);
}

void ChildWindowImpl::OnWindowExitFullscreen()
{
    //Empty implementation
    ASSERT(0);
}

void ChildWindowImpl::OnWindowDisplayScaleChanged(uint32_t nOldScaleFactor, uint32_t nNewScaleFactor)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnWindowDisplayScaleChanged(nOldScaleFactor, nNewScaleFactor);
    }
}

void ChildWindowImpl::GetShadowCorner(UiPadding& rcShadow) const
{
    //Empty implementation, clears the output parameters
    rcShadow = UiPadding();
}

void ChildWindowImpl::GetCurrentShadowCorner(UiPadding& rcShadow) const
{
    //Empty implementation, clears the output parameters
    rcShadow = UiPadding();
}

bool ChildWindowImpl::IsPtInCaptionBarControl(const UiPoint& /*pt*/) const
{
    //Empty implementation, by default returns false
    return false;
}

bool ChildWindowImpl::HasMinMaxBox(bool& bMinimizeBox, bool& bMaximizeBox) const
{
    //Empty implementation, by default set to false
    bMinimizeBox = false;
    bMaximizeBox = false;
    return false;
}

bool ChildWindowImpl::IsPtInMaximizeRestoreButton(const UiPoint& /*pt*/) const
{
    //Empty implementation, by default returns false
    return false;
}

void ChildWindowImpl::GetCreateWindowAttributes(WindowCreateAttributes& createAttributes)
{
    //Empty implementation, clears the output parameters
    createAttributes = WindowCreateAttributes();
}

// Implementation of non-pure virtual functions
void ChildWindowImpl::OnDisplayScaleChanged(uint32_t /*nOldScaleFactor*/, uint32_t /*nNewScaleFactor*/)
{
    //Empty implementation
}

IRender* ChildWindowImpl::GetRender() const
{
    //Empty implementation, by default returns nullptr
    return nullptr;
}

Control* ChildWindowImpl::OnFindControl(const UiPoint& /*pt*/) const
{
    //Empty implementation
    return nullptr;
}

// ------------------------------ Pure virtual functions related to window message handling ------------------------------
LRESULT ChildWindowImpl::OnWindowMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, bool& bHandled)
{
    //Empty implementation, by default marks the message as unhandled and returns 0
    bHandled = false;
    return 0;
}

void ChildWindowImpl::OnWindowCreateMsg(bool /*bDoModal*/, const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        m_pChildWindowEvents->OnWindowCreateMsg(nativeMsg, bHandled);
    }
}

LRESULT ChildWindowImpl::OnWindowCloseMsg(uint32_t wParam, const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnWindowCloseMsg(wParam, nativeMsg, bHandled);
    }
    return 0;
}

LRESULT ChildWindowImpl::OnShowWindowMsg(bool bShow, const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnShowWindowMsg(bShow, nativeMsg, bHandled);
    }
    return 0;
}

LRESULT ChildWindowImpl::OnPaintMsg(const UiRect& rcPaint, const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnPaintMsg(rcPaint, nativeMsg, bHandled);
    }
    return 0;
}

LRESULT ChildWindowImpl::OnWindowPosChangedMsg(const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnWindowPosChangedMsg(nativeMsg, bHandled);
    }
    return 0;
}

LRESULT ChildWindowImpl::OnSizeMsg(WindowSizeType sizeType, const UiSize& newWindowSize, const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnSizeMsg(sizeType, newWindowSize, nativeMsg, bHandled);
    }
    return 0;
}

LRESULT ChildWindowImpl::OnMoveMsg(const UiPoint& ptTopLeft, const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnMoveMsg(ptTopLeft, nativeMsg, bHandled);
    }
    return 0;
}

LRESULT ChildWindowImpl::OnSetFocusMsg(WindowBase* pLostFocusWindow, const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnSetFocusMsg(pLostFocusWindow, nativeMsg, bHandled);
    }
    return 0;
}

LRESULT ChildWindowImpl::OnKillFocusMsg(WindowBase* pSetFocusWindow, const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnKillFocusMsg(pSetFocusWindow, nativeMsg, bHandled);
    }
    return 0;
}

LRESULT ChildWindowImpl::OnImeSetContextMsg(const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    bHandled = false;
    return 0;
}

LRESULT ChildWindowImpl::OnImeStartCompositionMsg(const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    bHandled = false;
    return 0;
}

LRESULT ChildWindowImpl::OnImeCompositionMsg(const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    bHandled = false;
    return 0;
}

LRESULT ChildWindowImpl::OnImeEndCompositionMsg(const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    bHandled = false;
    return 0;
}

LRESULT ChildWindowImpl::OnSetCursorMsg(const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnSetCursorMsg(nativeMsg, bHandled);
    }
    return 0;
}

LRESULT ChildWindowImpl::OnContextMenuMsg(const UiPoint& /*pt*/, const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    bHandled = false;
    return 0;
}

LRESULT ChildWindowImpl::OnKeyDownMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnKeyDownMsg(vkCode, modifierKey, nativeMsg, bHandled);
    }
    return 0;
}

LRESULT ChildWindowImpl::OnKeyUpMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnKeyUpMsg(vkCode, modifierKey, nativeMsg, bHandled);
    }
    return 0;
}

LRESULT ChildWindowImpl::OnCharMsg(VirtualKeyCode /*vkCode*/, uint32_t /*modifierKey*/, const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    bHandled = false;
    return 0;
}

LRESULT ChildWindowImpl::OnHotKeyMsg(int32_t /*hotkeyId*/, VirtualKeyCode /*vkCode*/, uint32_t /*modifierKey*/, const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    bHandled = false;
    return 0;
}

LRESULT ChildWindowImpl::OnMouseWheelMsg(int32_t wheelDelta, const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnMouseWheelMsg(wheelDelta, pt, modifierKey, nativeMsg, bHandled);
    }
    return 0;
}

LRESULT ChildWindowImpl::OnMouseMoveMsg(const UiPoint& pt, uint32_t modifierKey, bool /*bFromNC*/, const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnMouseMoveMsg(pt, modifierKey, nativeMsg, bHandled);
    }
    return 0;
}

LRESULT ChildWindowImpl::OnMouseHoverMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnMouseHoverMsg(pt, modifierKey, nativeMsg, bHandled);
    }
    return 0;
}

LRESULT ChildWindowImpl::OnMouseLeaveMsg(const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnMouseLeaveMsg(nativeMsg, bHandled);
    }
    return 0;
}

LRESULT ChildWindowImpl::OnMouseLButtonDownMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnMouseLButtonDownMsg(pt, modifierKey, nativeMsg, bHandled);
    }
    return 0;
}

LRESULT ChildWindowImpl::OnMouseLButtonUpMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnMouseLButtonUpMsg(pt, modifierKey, nativeMsg, bHandled);
    }
    return 0;
}

LRESULT ChildWindowImpl::OnMouseLButtonDbClickMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnMouseLButtonDbClickMsg(pt, modifierKey, nativeMsg, bHandled);
    }
    return 0;
}

LRESULT ChildWindowImpl::OnMouseRButtonDownMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnMouseRButtonDownMsg(pt, modifierKey, nativeMsg, bHandled);
    }
    return 0;
}

LRESULT ChildWindowImpl::OnMouseRButtonUpMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnMouseRButtonUpMsg(pt, modifierKey, nativeMsg, bHandled);
    }
    return 0;
}

LRESULT ChildWindowImpl::OnMouseRButtonDbClickMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnMouseRButtonDbClickMsg(pt, modifierKey, nativeMsg, bHandled);
    }
    return 0;
}

LRESULT ChildWindowImpl::OnMouseMButtonDownMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnMouseMButtonDownMsg(pt, modifierKey, nativeMsg, bHandled);
    }
    return 0;
}

LRESULT ChildWindowImpl::OnMouseMButtonUpMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnMouseMButtonUpMsg(pt, modifierKey, nativeMsg, bHandled);
    }
    return 0;
}

LRESULT ChildWindowImpl::OnMouseMButtonDbClickMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnMouseMButtonDbClickMsg(pt, modifierKey, nativeMsg, bHandled);
    }
    return 0;
}

LRESULT ChildWindowImpl::OnCaptureChangedMsg(const NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_pChildWindowEvents != nullptr) {
        return m_pChildWindowEvents->OnCaptureChangedMsg(nativeMsg, bHandled);
    }
    return 0;
}

void ChildWindowImpl::OnWindowPosSnapped(bool /*bLeftSnap*/, bool /*bRightSnap*/, bool /*bTopSnap*/, bool /*bBottomSnap*/)
{
    //Empty implementation
}

void ChildWindowImpl::OnDropEnterMsg(ControlDropType /*dropType*/, void* /*pDropData*/)
{
    //Empty implementation
}

void ChildWindowImpl::OnDropOverMsg(ControlDropType /*dropType*/, void* /*pDropData*/)
{
    //Empty implementation
}

void ChildWindowImpl::OnDropMsg(ControlDropType /*dropType*/, void* /*pDropData*/)
{
    //Empty implementation
}

void ChildWindowImpl::OnDropLeaveMsg()
{
    //Empty implementation
}

void ChildWindowImpl::OnDisplayResolutionChangedMsg(int32_t /*nColorDepth*/, int32_t /*nScreenWidth*/, int32_t /*nScreenHeight*/)
{
    //Empty implementation
}

void ChildWindowImpl::OnDisplayScaleChangedMsg(float /*fNewDisplayScale*/, float /*fNewPixelDensity*/)
{
    //Empty implementation
}

}//namespace ui
