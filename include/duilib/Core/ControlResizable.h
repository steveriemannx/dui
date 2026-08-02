#ifndef UI_CORE_CONTROL_RESIZABLE_H_
#define UI_CORE_CONTROL_RESIZABLE_H_

#include "duilib/Core/ControlMovable.h"

namespace ui
{
/** Main function: supports adjusting the size of the control by pressing the left mouse button (while retaining the draggable position feature of the base class)
*/
template<typename T = Control>
class ControlResizableT: public ControlMovableT<T>
{
    typedef ControlMovableT<T> BaseClass;
public:
    explicit ControlResizableT(Window* pWindow);
    virtual ~ControlResizableT() override;

    /// Override the parent class methods to provide customized features, please refer to the parent class declarations
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;

    /** Set whether the control size can be changed by mouse dragging
    */
    void SetEnableResize(bool bEnable);

    /** Get whether the control size can be changed by mouse dragging
    */
    bool IsEnableResize() const;

    /** Get the stretchable range size of the four sides of the control (value not DPI scaled)
    */
    const UiRect& GetSizeBox() const;

    /** Set the stretchable range size of the four sides of the control (value not DPI scaled)
    * @param [in] rcSizeBox The size to set
    */
    void SetSizeBox(const UiRect& rcSizeBox);

    /** Set the minimum width reserved when resizing (not DPI scaled)
    */
    void SetResizeReserveWidth(int32_t nResizeReserveWidth);

    /** Get the minimum width reserved when resizing (not DPI scaled)
    */
    int32_t GetResizeReserveWidth() const;

    /** Set the minimum height reserved when resizing (not DPI scaled)
    */
    void SetResizeReserveHeight(int32_t nResizeReserveHeight);

    /** Get the minimum height reserved when resizing (not DPI scaled)
    */
    int32_t GetResizeReserveHeight() const;

    /** Set whether to ensure the control stays within the parent container when resizing
    */
    void SetResizeKeepWithinParent(bool bKeepWithinParent);

    /** Get whether to ensure the control stays within the parent container when resizing
    */
    bool IsResizeKeepWithinParent() const;

public:
    /** Whether currently in a drag resize operation
    */
    bool IsResizing() const;

    /** Cancel the drag resize operation
    */
    void CancelResizeOperation();

protected:
    /** @name Drag related member functions
    * @{ */

    virtual bool ButtonDown(const EventArgs& msg) override;
    virtual bool ButtonUp(const EventArgs& msg) override;
    virtual bool MouseMove(const EventArgs& msg) override;
    virtual bool OnWindowKillFocus(const EventArgs& msg) override;//The window to which the control belongs has lost focus
    virtual bool OnSetCursor(const EventArgs& msg) override;
    virtual void HandleEvent(const EventArgs& msg) override;

protected:
    /** Clear the drag resize status and restore the original status
    */
    virtual void ClearResizeStatus();

    /** @} */

private:
    /** The type of resizing
    */
    enum class SizeType : uint8_t
    {
        kNone,
        kLeft,
        kTop,
        kRight,
        kBottom,
        kLeftTop,
        kRightTop,
        kLeftBottom,
        kRightBottom
    };

private:
    /** The drag resize operation executed when the mouse moves
    * @return Returns true if in a drag resize operation, returns false if the drag resize operation is not executed
    */
    bool ResizeMouseMove(const EventArgs& msg);

    /** The operation executed when the mouse button is released
    */
    void ResizeMouseUp(const EventArgs& msg);

    /** Determine the resize type according to the mouse position
    */
    SizeType GetCurrentSizeType(const UiPoint& ptMouse) const;

    /** Determine the current cursor style according to the resize type
    */
    CursorType GetCurrentCursorType(SizeType sizeType) const;

    /** Move the control to the mouse position
    */
    bool ResizeControlToPoint(const UiPoint& ptMouse);

    /** Get the interface of the control to be adjusted
    */
    Control* GetResizeControl();

    /** Get the interface of the control to be adjusted
    */
    const Control* GetResizeControl() const;

private:
    /** Whether dragging to change the control size is supported (feature switch)
    */
    bool m_bEnableResize;

    /** When resizing the control, ensure the control stays within the parent container
    */
    bool m_bKeepWithinParent;

    /** Whether the left mouse button is pressed
    */
    bool m_bMouseDown;

    /** Whether in a drag resize operation
    */
    bool m_bResizing;

    /** The type of resizing
    */
    SizeType m_sizeType;

    /** The mouse position when the mouse button is pressed
    */
    UiPoint m_ptMouseDown;

    /** The control rectangle area when the mouse button is pressed
    */
    UiRect m_rcMouseDown;

    /** The stretchable range information of the four sides of the control (value not DPI scaled)
    */
    UiRect m_rcSizeBox;

    /** The minimum width reserved when resizing (not DPI scaled)
    */
    int32_t m_nResizeReserveWidth;

    /** The minimum height reserved when resizing (not DPI scaled)
    */
    int32_t m_nResizeReserveHeight;
};

template<typename T>
ControlResizableT<T>::ControlResizableT(Window* pWindow):
    ControlMovableT<T>(pWindow),
    m_bEnableResize(true),
    m_bKeepWithinParent(false),
    m_bResizing(false),
    m_bMouseDown(false),
    m_sizeType(SizeType::kNone),
    m_nResizeReserveWidth(10),
    m_nResizeReserveHeight(10)
{
    //Should be a floating control by default, and save the control position
    this->SetKeepFloatPos(true);
    this->SetFloat(true);

    //Disable the drag control position feature by default
    this->SetEnableMovePos(false);
}

template<typename T>
ControlResizableT<T>::~ControlResizableT()
{
}

template<typename T>
inline DString ControlResizableT<T>::GetType() const { return DUI_CTR_CONTROL_RESIZABLE; }

template<>
inline DString ControlResizableT<Box>::GetType() const { return DUI_CTR_BOX_RESIZABLE; }

template<>
inline DString ControlResizableT<HBox>::GetType() const { return DUI_CTR_HBOX_RESIZABLE; }

template<>
inline DString ControlResizableT<VBox>::GetType() const { return DUI_CTR_VBOX_RESIZABLE; }

template<typename T>
void ControlResizableT<T>::SetAttribute(const DString& strName, const DString& strValue)
{
    if (strName == _T("enable_resize")) {
        SetEnableResize(strValue == _T("true"));
    }
    else if (strName == _T("resize_size_box")) {
        UiRect rcSizeBox;
        AttributeUtil::ParseRectValue(strValue.c_str(), rcSizeBox, false);
        SetSizeBox(rcSizeBox);
    }
    else if (strName == _T("resize_reserve_width")) {
        SetResizeReserveWidth(StringUtil::StringToInt32(strValue));
    }
    else if (strName == _T("resize_reserve_height")) {
        SetResizeReserveHeight(StringUtil::StringToInt32(strValue));
    }
    else if (strName == _T("resize_keep_within_parent")) {
        SetResizeKeepWithinParent(strValue == _T("true"));
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

template<typename T>
void ControlResizableT<T>::SetEnableResize(bool bEnable)
{
    m_bEnableResize = bEnable;
}

template<typename T>
bool ControlResizableT<T>::IsEnableResize() const
{
    return m_bEnableResize;
}

template<typename T>
const UiRect& ControlResizableT<T>::GetSizeBox() const
{
    return m_rcSizeBox;
}

template<typename T>
void ControlResizableT<T>::SetSizeBox(const UiRect& rcSizeBox)
{
    m_rcSizeBox = rcSizeBox;
    m_rcSizeBox.left = std::max(m_rcSizeBox.left, 0);
    m_rcSizeBox.top = std::max(m_rcSizeBox.top, 0);
    m_rcSizeBox.right = std::max(m_rcSizeBox.right, 0);
    m_rcSizeBox.bottom = std::max(m_rcSizeBox.bottom, 0);

    //Sync to the base class: the resizable border part cannot be used to drag the control
    UiMargin rcNonDraggableMargin;
    rcNonDraggableMargin.left = m_rcSizeBox.left;
    rcNonDraggableMargin.top = m_rcSizeBox.top;
    rcNonDraggableMargin.right = m_rcSizeBox.right;
    rcNonDraggableMargin.bottom = m_rcSizeBox.bottom;
    this->SetNonDraggableMargin(rcNonDraggableMargin);
}

template<typename T>
void ControlResizableT<T>::SetResizeReserveWidth(int32_t nResizeReserveWidth)
{
    m_nResizeReserveWidth = nResizeReserveWidth;
    if (m_nResizeReserveWidth < 0) {
        m_nResizeReserveWidth = 0;
    }
}

template<typename T>
int32_t ControlResizableT<T>::GetResizeReserveWidth() const
{
    return m_nResizeReserveWidth;
}

template<typename T>
void ControlResizableT<T>::SetResizeReserveHeight(int32_t nResizeReserveHeight)
{
    m_nResizeReserveHeight = nResizeReserveHeight;
    if (m_nResizeReserveHeight < 0) {
        m_nResizeReserveHeight = 0;
    }
}

template<typename T>
int32_t ControlResizableT<T>::GetResizeReserveHeight() const
{
    return m_nResizeReserveHeight;
}

template<typename T>
void ControlResizableT<T>::SetResizeKeepWithinParent(bool bKeepWithinParent)
{
    m_bKeepWithinParent = bKeepWithinParent;
}

template<typename T>
bool ControlResizableT<T>::IsResizeKeepWithinParent() const
{
    return m_bKeepWithinParent;
}

template<typename T>
bool ControlResizableT<T>::IsResizing() const
{
    return m_bResizing;
}

template<typename T>
void ControlResizableT<T>::CancelResizeOperation()
{
    ClearResizeStatus();
}

template<typename T>
Control* ControlResizableT<T>::GetResizeControl()
{
    return this;
}

template<typename T>
const Control* ControlResizableT<T>::GetResizeControl() const
{
    return this;
}

template<typename T>
bool ControlResizableT<T>::ButtonDown(const EventArgs& msg)
{
    m_bMouseDown = false;
    bool bRet = BaseClass::ButtonDown(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    if (this->IsMovingPos()) {
        //Currently in a control move operation
        ClearResizeStatus();
        return bRet;
    }
    if (!IsEnableResize() || GetSizeBox().IsZero()) {
        //Feature is disabled
        return bRet;
    }

    UiPoint pt(msg.ptMouse);
    pt.Offset(this->GetScrollOffsetInScrollBox());

    m_bMouseDown = true;
    m_ptMouseDown = pt;

    m_rcMouseDown = GetResizeControl()->GetRect();
    return bRet;
}

template<typename T>
bool ControlResizableT<T>::MouseMove(const EventArgs& msg)
{
    bool bRet = BaseClass::MouseMove(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    if (this->IsMovingPos()) {
        //Currently in a control move operation
        ClearResizeStatus();
        return bRet;
    }
    if (!IsEnableResize() || GetSizeBox().IsZero()) {
        //Feature is disabled
        return bRet;
    }
    if (m_bMouseDown) {
        //When the mouse button is pressed, resize the control
        ResizeMouseMove(msg);
    }
    else {
        //When the mouse button is not pressed, change the cursor style and determine the resize type
        m_sizeType = GetCurrentSizeType(msg.ptMouse);
    }
    return bRet;
}

template<typename T>
bool ControlResizableT<T>::ButtonUp(const EventArgs& msg)
{
    bool bRet = BaseClass::ButtonUp(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    if (IsEnableResize() && !GetSizeBox().IsZero() && IsResizing()) {
        ResizeMouseUp(msg);
    }
    ClearResizeStatus();
    return bRet;
}

template<typename T>
bool ControlResizableT<T>::OnWindowKillFocus(const EventArgs& msg)
{
    bool bRet = BaseClass::OnWindowKillFocus(msg);
    ClearResizeStatus();
    return bRet;
}

template<typename T>
bool ControlResizableT<T>::OnSetCursor(const EventArgs& msg)
{
    if (m_sizeType == SizeType::kNone) {
        return BaseClass::OnSetCursor(msg);
    }
    else {
        CursorType cursorType = GetCurrentCursorType(m_sizeType);
        this->SetCursor(cursorType);
        return true;
    }
}

template<typename T>
void ControlResizableT<T>::HandleEvent(const EventArgs& msg)
{
    BaseClass::HandleEvent(msg);
    if (msg.IsSenderExpired()) {
        return;
    }
    if ((msg.eventType == kEventMouseClickChanged) ||
        (msg.eventType == kEventMouseClickEsc) ||
        (msg.eventType == kEventMouseRButtonDown)) {
        //Cancel the operation
        ClearResizeStatus();
    }
}

template<typename T>
void ControlResizableT<T>::ClearResizeStatus()
{
    //Restore the status
    m_bResizing = false;
    m_bMouseDown = false;
    m_sizeType = SizeType::kNone;
}

template<typename T>
bool ControlResizableT<T>::ResizeMouseMove(const EventArgs& msg)
{
    if (!IsEnableResize() || GetSizeBox().IsZero()) {
        //The control is not allowed to be dragged to resize
        return false;
    }
    if (!m_bMouseDown) {
        return false;
    }

    //Move the control to the mouse position
    if (!ResizeControlToPoint(msg.ptMouse)) {
        return false;
    }

    if (!m_bResizing) {
        m_bResizing = true;
    }
    return m_bResizing;
}

template<typename T>
void ControlResizableT<T>::ResizeMouseUp(const EventArgs& msg)
{
    if (!IsEnableResize() || GetSizeBox().IsZero() || !IsResizing()) {
        return;
    }
    ResizeControlToPoint(msg.ptMouse);
    ClearResizeStatus();
}

template<typename T>
typename ControlResizableT<T>::SizeType ControlResizableT<T>::GetCurrentSizeType(const UiPoint& ptMouse) const
{
    UiPoint pt(ptMouse);
    pt.Offset(this->GetScrollOffsetInScrollBox());
    UiRect rcClient = GetResizeControl()->GetRect();
    UiRect rcSizeBox = GetSizeBox();
    this->Dpi().ScaleRect(rcSizeBox);
    if (pt.y < rcClient.top + rcSizeBox.top) {
        if (pt.y >= rcClient.top) {
            if (pt.x < (rcClient.left + rcSizeBox.left) && pt.x >= rcClient.left) {
                return SizeType::kLeftTop;//At the top-left corner of the border
            }
            else if (pt.x > (rcClient.right - rcSizeBox.right) && pt.x <= rcClient.right) {
                return SizeType::kRightTop;//At the top-right corner of the border
            }
            else {
                return SizeType::kTop;//In the top horizontal border
            }
        }
        else {
            return SizeType::kNone;//In the workspace
        }
    }
    else if (pt.y > rcClient.bottom - rcSizeBox.bottom) {
        if (pt.y <= rcClient.bottom) {
            if (pt.x < (rcClient.left + rcSizeBox.left) && pt.x >= rcClient.left) {
                return SizeType::kLeftBottom;//At the bottom-left corner of the border
            }
            else if (pt.x > (rcClient.right - rcSizeBox.right) && pt.x <= rcClient.right) {
                return SizeType::kRightBottom;//At the bottom-right corner of the border
            }
            else {
                return SizeType::kBottom;//In the bottom horizontal border
            }
        }
        else {
            return SizeType::kNone;//In the workspace
        }
    }

    if (pt.x < rcClient.left + rcSizeBox.left) {
        if (pt.x >= rcClient.left) {
            return SizeType::kLeft;//In the left border
        }
        else {
            return SizeType::kNone;//In the workspace
        }
    }
    if (pt.x > rcClient.right - rcSizeBox.right) {
        if (pt.x <= rcClient.right) {
            return SizeType::kRight;//In the right border
        }
        else {
            return SizeType::kNone;//In the workspace
        }
    }
    return SizeType::kNone;//In the workspace
}

template<typename T>
CursorType ControlResizableT<T>::GetCurrentCursorType(SizeType sizeType) const
{
    CursorType cursorType = CursorType::kCursorArrow;
    switch (sizeType) {
    case SizeType::kTop://Top border
        cursorType = CursorType::kCursorSizeNS;
        break;
    case SizeType::kBottom://Bottom border
        cursorType = CursorType::kCursorSizeNS;
        break;
    case SizeType::kLeft://Left border
        cursorType = CursorType::kCursorSizeWE;
        break;
    case SizeType::kRight://Right border
        cursorType = CursorType::kCursorSizeWE;
        break;
    case SizeType::kLeftTop: //Top-left corner
        cursorType = CursorType::kCursorSizeNWSE;
        break;
    case SizeType::kRightTop://Top-right corner
        cursorType = CursorType::kCursorSizeNESW;
        break;
    case SizeType::kLeftBottom://Bottom-left corner
        cursorType = CursorType::kCursorSizeNESW;
        break;
    case SizeType::kRightBottom://Bottom-right corner
        cursorType = CursorType::kCursorSizeNWSE;
        break;
    default:
        break;
    }
    return cursorType;
}

template<typename T>
bool ControlResizableT<T>::ResizeControlToPoint(const UiPoint& ptMouse)
{
    UiPoint pt(ptMouse);
    pt.Offset(this->GetScrollOffsetInScrollBox());

    int32_t xOffset = pt.x - m_ptMouseDown.x;
    int32_t yOffset = pt.y - m_ptMouseDown.y;
    
    Control* pControl = GetResizeControl();
    UiRect rcNewPos = m_rcMouseDown;

    bool bSizeTypeLeft = false;
    bool bSizeTypeTop = false;
    bool bSizeTypeRight = false;
    bool bSizeTypeBottom = false;

    switch (m_sizeType) {
    case SizeType::kTop://Top border
        bSizeTypeTop = true;
        break;
    case SizeType::kBottom://Bottom border
        bSizeTypeBottom = true;
        break;
    case SizeType::kLeft://Left border
        bSizeTypeLeft = true;
        break;
    case SizeType::kRight://Right border
        bSizeTypeRight = true;
        break;
    case SizeType::kLeftTop: //Top-left corner
        bSizeTypeLeft = true;
        bSizeTypeTop = true;
        break;
    case SizeType::kRightTop://Top-right corner
        bSizeTypeRight = true;
        bSizeTypeTop = true;
        break;
    case SizeType::kLeftBottom://Bottom-left corner
        bSizeTypeLeft = true;
        bSizeTypeBottom = true;
        break;
    case SizeType::kRightBottom://Bottom-right corner
        bSizeTypeRight = true;
        bSizeTypeBottom = true;
        break;
    default:
        break;
    }

    int32_t nMinWidth = GetResizeReserveWidth();
    this->Dpi().ScaleInt(nMinWidth);
    nMinWidth = std::max(nMinWidth, pControl->GetMinWidth());
    nMinWidth = std::max(nMinWidth, 0);

    int32_t nMinHeight = GetResizeReserveHeight();
    this->Dpi().ScaleInt(nMinHeight);
    nMinHeight = std::max(nMinHeight, pControl->GetMinHeight());
    nMinHeight = std::max(nMinHeight, 0);

    int32_t nMaxWidth = pControl->GetMaxWidth();
    int32_t nMaxHeight = pControl->GetMaxHeight();
    if (nMaxWidth <= nMinWidth) {
        nMaxWidth = INT32_MAX;
    }
    if (nMaxHeight <= nMinHeight) {
        nMaxHeight = INT32_MAX;
    }

    Control* pParent = nullptr;
    if (IsResizeKeepWithinParent()) {
        pParent = pControl->GetParent();
    }
    UiRect rcParent;
    if (pParent != nullptr) {
        rcParent = pParent->GetRect();
        rcParent.Deflate(pParent->GetPadding());// Subtract the padding
    }

    if (bSizeTypeLeft) {
        //Left side
        rcNewPos.left += xOffset;
        if (rcNewPos.left > rcNewPos.right) {
            rcNewPos.left = rcNewPos.right;
        }
        if (xOffset > 0) {
            //Shrink
            if (rcNewPos.Width() < nMinWidth) {
                rcNewPos.left = rcNewPos.right - nMinWidth;
            }
        }
        else if (xOffset < 0) {
            //Enlarge
            if (rcNewPos.Width() > nMaxWidth) {
                rcNewPos.left = rcNewPos.right - nMaxWidth;
            }
        }

        if (IsResizeKeepWithinParent() && (pParent != nullptr)) {
            //Strictly limit within the parent container, overflowing is not allowed
            if (xOffset < 0) {
                if (rcNewPos.left < rcParent.left) {
                    rcNewPos.left = rcParent.left;
                    rcNewPos.Validate();
                }
            }
        }
    }
    if (bSizeTypeRight) {
        //Right side
        rcNewPos.right += xOffset;
        if (rcNewPos.right < rcNewPos.left) {
            rcNewPos.right = rcNewPos.left;
        }
        if (xOffset > 0) {
            //Enlarge
            if (rcNewPos.Width() > nMaxWidth) {
                rcNewPos.right = rcNewPos.left + nMaxWidth;
            }
        }
        else if (xOffset < 0) {
            //Shrink
            if (rcNewPos.Width() < nMinWidth) {
                rcNewPos.right = rcNewPos.left + nMinWidth;
            }            
        }
        if (IsResizeKeepWithinParent() && (pParent != nullptr)) {
            //Strictly limit within the parent container, overflowing is not allowed
            if (xOffset > 0) {
                if (rcNewPos.right > rcParent.right) {
                    rcNewPos.right = rcParent.right;
                    rcNewPos.Validate();
                }
            }
        }
    }
    if (bSizeTypeTop) {
        //Top side
        rcNewPos.top += yOffset;
        if (rcNewPos.top > rcNewPos.bottom) {
            rcNewPos.top = rcNewPos.bottom;
        }
        if (yOffset > 0) {
            //Shrink
            if (rcNewPos.Height() < nMinHeight) {
                rcNewPos.top = rcNewPos.bottom - nMinHeight;
            }
        }
        else if (yOffset < 0) {
            //Enlarge
            if (rcNewPos.Height() > nMaxHeight) {
                rcNewPos.top = rcNewPos.bottom - nMaxHeight;
            }
        }
        if (IsResizeKeepWithinParent() && (pParent != nullptr)) {
            //Strictly limit within the parent container, overflowing is not allowed
            if (xOffset < 0) {
                if (rcNewPos.top < rcParent.top) {
                    rcNewPos.top = rcParent.top;
                    rcNewPos.Validate();
                }
            }
        }
    }
    if (bSizeTypeBottom) {
        //Bottom side
        rcNewPos.bottom += yOffset;
        if (rcNewPos.bottom < rcNewPos.top) {
            rcNewPos.bottom = rcNewPos.top;
        }
        if (yOffset > 0) {
            //Enlarge
            if (rcNewPos.Height() > nMaxHeight) {
                rcNewPos.bottom = rcNewPos.top + nMaxHeight;
            }
        }
        else if (yOffset < 0) {
            //Shrink
            if (rcNewPos.Height() < nMinHeight) {
                rcNewPos.bottom = rcNewPos.top + nMinHeight;
            }
        }
        if (IsResizeKeepWithinParent() && (pParent != nullptr)) {
            //Strictly limit within the parent container, overflowing is not allowed
            if (xOffset > 0) {
                if (rcNewPos.bottom > rcParent.bottom) {
                    rcNewPos.bottom = rcParent.bottom;
                    rcNewPos.Validate();
                }
            }
        }
    }
    if (rcNewPos != pControl->GetPos()) {
        pControl->SetPos(rcNewPos);
    }    
    return true;
}

typedef ControlResizableT<Control> ControlResizable;
typedef ControlResizableT<Box> BoxResizable;
typedef ControlResizableT<HBox> HBoxResizable;
typedef ControlResizableT<VBox> VBoxResizable;

}

#endif // UI_CORE_CONTROL_RESIZABLE_H_
