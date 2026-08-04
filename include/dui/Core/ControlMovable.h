#ifndef UI_CORE_CONTROL_MOVABLE_H_
#define UI_CORE_CONTROL_MOVABLE_H_

#include "dui/Core/Control.h"
#include "dui/Core/Box.h"
#include "dui/Box/VBox.h"
#include "dui/Box/HBox.h"
#include "dui/Utils/StringUtil.h"
#include "dui/Utils/AttributeUtil.h"

namespace ui
{
/** Main function: supports adjusting the position of the control by pressing the left mouse button
*/
template<typename T = Control>
class ControlMovableT: public T
{
    typedef T BaseClass;
public:
    explicit ControlMovableT(Window* pWindow);
    virtual ~ControlMovableT() override;

    /// Override the parent class methods to provide customized features, please refer to the parent class declarations
    virtual DString GetType() const override;    
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;

    /** Set whether the control position can be changed by mouse dragging
    */
    void SetEnableMovePos(bool bEnable);

    /** Get whether the control position can be changed by mouse dragging
    */
    bool IsEnableMovePos() const;

    /** Set whether to adjust the control position of the parent container
    * @param [in] bMoveParentPos true means adjust the parent container position, false means adjust this control's own position
    */
    void SetMoveParentPos(bool bMoveParentPos);

    /** Get whether to adjust the control position of the parent container
    */
    bool IsMoveParentPos() const;

    /** Set the transparency of the control when dragging to adjust the position (0 - 255)
    */
    void SetMovePosAlpha(uint8_t nAlpha);

    /** Get the transparency of the control when dragging to adjust the position
    */
    uint8_t GetMovePosAlpha() const;

    /** Set the outer margin of the control's movable rectangle (the surrounding area defined by the margin is not click-draggable, only the center area is draggable, not DPI scaled)
    */
    void SetNonDraggableMargin(const UiMargin& rcNonDraggableMargin);

    /** Get the outer margin of the control's movable rectangle (the surrounding area defined by the margin is not click-draggable, only the center area is draggable, not DPI scaled)
    */
    const UiMargin& GetNonDraggableMargin() const;

    /** Set the border range of the control's movable rectangle (the surrounding area is click-draggable, but the center area is not draggable, not DPI scaled)
    */
    void SetDraggableBorder(const UiPadding& rcDraggableBorder);

    /** Get the border range of the control's movable rectangle (the surrounding area is click-draggable, but the center area is not draggable, not DPI scaled)
    */
    const UiPadding& GetDraggableBorder() const;

    /** Set the height reserved within the parent container when moving horizontally, to avoid the control overflowing the parent container (not DPI scaled)
    */
    void SetMovePosReserveWidth(int32_t nReserveWidth);

    /** Get the height reserved within the parent container when moving horizontally, to avoid the control overflowing the parent container (not DPI scaled)
    */
    int32_t GetMovePosReserveWidth() const;

    /** Set the width reserved within the parent container when moving vertically, to avoid the control overflowing the parent container (not DPI scaled)
    */
    void SetMovePosReserveHeight(int32_t nReserveHeight);

    /** Get the width reserved within the parent container when moving vertically, to avoid the control overflowing the parent container (not DPI scaled)
    */
    int32_t GetMovePosReserveHeight() const;

    /** Set whether to ensure the control stays within the parent container when moving
    */
    void SetMovePosKeepWithinParent(bool bKeepWithinParent);

    /** Get whether to ensure the control stays within the parent container when moving
    */
    bool IsMovePosKeepWithinParent() const;

public:
    /** Whether currently in a drag position adjustment operation
    */
    bool IsMovingPos() const;

    /** Cancel the drag position adjustment operation
    */
    void CancelMovePosOperation();

protected:
    /** @name Drag related member functions
    * @{ */

    virtual bool ButtonDown(const EventArgs& msg) override;
    virtual bool ButtonUp(const EventArgs& msg) override;
    virtual bool MouseMove(const EventArgs& msg) override;
    virtual bool OnWindowKillFocus(const EventArgs& msg) override;//The window to which the control belongs has lost focus
    virtual void HandleEvent(const EventArgs& msg) override;

protected:
    /** Clear the drag position adjustment status and restore the original status
    */
    virtual void ClearMovePosStatus();

    /** Limit the control within the parent container range, to avoid completely overflowing the parent container
    * @param [in] pControl The interface of the control
    * @param [in] xOffset The movement distance in the X direction
    * @param [in] yOffset The movement distance in the Y direction
    * @param [in] rcNewPos The new position of the control
    */
    void AdjustRectInParentBox(Control* pControl, int32_t xOffset, int32_t yOffset, UiRect& rcNewPos) const;

    /** @} */

private:
    /** The drag position adjustment operation executed when the mouse moves
    * @return Returns true if in a drag position adjustment operation, returns false if the drag position adjustment operation is not executed
    */
    bool MovePosMouseMove(const EventArgs& msg);

    /** The operation executed when the mouse button is released
    */
    void MovePosMouseUp(const EventArgs& msg);

    /** Move the control to the mouse position
    */
    void MoveControlToPoint(const UiPoint& ptMouse);

    /** Get the interface of the control to be adjusted
    */
    Control* GetMovePosControl();

    /** Whether the current click point is in the draggable area
    */
    bool IsInDraggableArea(const UiPoint& ptMouse) const;

private:
    /** Whether dragging to change the control position is supported (feature switch)
    */
    bool m_bEnableMovePos;

    /** Whether to adjust the control position of the parent container
    */
    bool m_bMoveParentPos;

    /** Whether the left mouse button is pressed
    */
    bool m_bMouseDown;

    /** Whether in a drag position adjustment operation
    */
    bool m_bMovingPos;

    /** When moving the control, ensure the control stays within the parent container
    */
    bool m_bKeepWithinParent;

    /** The transparency of the control when dragging to adjust the position
    */
    uint8_t m_nMovePosAlpha;

    /** The original transparency of the control
    */
    uint8_t m_nOldAlpha;

    /** The mouse position when the mouse button is pressed
    */
    UiPoint m_ptMouseDown;

    /** The control rectangle area when the mouse button is pressed
    */
    UiRect m_rcMouseDown;

    /** The outer margin of the control's movable rectangle (the surrounding area defined by the margin is not click-draggable, only the center area is draggable, not DPI scaled)
    */
    UiMargin m_rcNonDraggableMargin;

    /** The border range of the control's movable rectangle (the surrounding area is click-draggable, but the center area is not draggable, not DPI scaled)
    */
    UiPadding m_rcDraggableBorder;

    /** The height reserved within the parent container when moving horizontally, to avoid the control completely overflowing the parent container (not DPI scaled)
    */
    int32_t m_nMovePosReserveWidth;

    /** The width reserved within the parent container when moving vertically, to avoid the control completely overflowing the parent container (not DPI scaled)
    */
    int32_t m_nMovePosReserveHeight;
};

template<typename T>
ControlMovableT<T>::ControlMovableT(Window* pWindow):
    T(pWindow),
    m_bEnableMovePos(true),
    m_bMoveParentPos(false),
    m_bMovingPos(false),
    m_bMouseDown(false),
    m_bKeepWithinParent(false),
    m_nMovePosAlpha(216),
    m_nOldAlpha(255),
    m_nMovePosReserveWidth(20),
    m_nMovePosReserveHeight(20)
{
    //Should be a floating control by default, and save the control position
    this->SetKeepFloatPos(true);
    this->SetFloat(true);
}

template<typename T>
ControlMovableT<T>::~ControlMovableT()
{
}

template<typename T>
inline DString ControlMovableT<T>::GetType() const { return DUI_CTR_CONTROL_MOVABLE; }

template<>
inline DString ControlMovableT<Box>::GetType() const { return DUI_CTR_BOX_MOVABLE; }

template<>
inline DString ControlMovableT<HBox>::GetType() const { return DUI_CTR_HBOX_MOVABLE; }

template<>
inline DString ControlMovableT<VBox>::GetType() const { return DUI_CTR_VBOX_MOVABLE; }

template<typename T>
void ControlMovableT<T>::SetAttribute(const DString& strName, const DString& strValue)
{
    if (strName == _T("enable_move_pos")) {
        SetEnableMovePos(strValue == _T("true"));
    }
    else if (strName == _T("move_parent_pos")) {
        SetMoveParentPos(strValue == _T("true"));
    }
    else if (strName == _T("move_pos_alpha")) {
        SetMovePosAlpha((uint8_t)StringUtil::StringToInt32(strValue));
    }
    else if (strName == _T("move_pos_non_draggable_margin")) {
        UiMargin rcNonDraggableMargin;
        AttributeUtil::ParseMarginValue(strValue.c_str(), rcNonDraggableMargin);
        SetNonDraggableMargin(rcNonDraggableMargin);
    }
    else if (strName == _T("move_pos_draggable_border")) {
        UiPadding rcDraggableBorder;
        AttributeUtil::ParsePaddingValue(strValue.c_str(), rcDraggableBorder);
        SetDraggableBorder(rcDraggableBorder);
    }
    else if (strName == _T("move_pos_reserve_width")) {
        SetMovePosReserveWidth(StringUtil::StringToInt32(strValue));
    }
    else if (strName == _T("move_pos_reserve_height")) {
        SetMovePosReserveHeight(StringUtil::StringToInt32(strValue));
    }
    else if (strName == _T("move_pos_keep_within_parent")) {
        SetMovePosKeepWithinParent(strValue == _T("true"));
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

template<typename T>
void ControlMovableT<T>::SetEnableMovePos(bool bEnable)
{
    m_bEnableMovePos = bEnable;
}

template<typename T>
bool ControlMovableT<T>::IsEnableMovePos() const
{
    return m_bEnableMovePos;
}

template<typename T>
void ControlMovableT<T>::SetMoveParentPos(bool bMoveParentPos)
{
    m_bMoveParentPos = bMoveParentPos;
}

template<typename T>
bool ControlMovableT<T>::IsMoveParentPos() const
{
    return m_bMoveParentPos;
}

template<typename T>
void ControlMovableT<T>::SetMovePosAlpha(uint8_t nAlpha)
{
    m_nMovePosAlpha = nAlpha;
}

template<typename T>
uint8_t ControlMovableT<T>::GetMovePosAlpha() const
{
    return m_nMovePosAlpha;
}

template<typename T>
void ControlMovableT<T>::SetNonDraggableMargin(const UiMargin& rcNonDraggableMargin)
{
    m_rcNonDraggableMargin = rcNonDraggableMargin;
    m_rcNonDraggableMargin.left = std::max(m_rcNonDraggableMargin.left, 0);
    m_rcNonDraggableMargin.top = std::max(m_rcNonDraggableMargin.top, 0);
    m_rcNonDraggableMargin.right = std::max(m_rcNonDraggableMargin.right, 0);
    m_rcNonDraggableMargin.bottom = std::max(m_rcNonDraggableMargin.bottom, 0);
}

template<typename T>
const UiMargin& ControlMovableT<T>::GetNonDraggableMargin() const
{
    return m_rcNonDraggableMargin;
}

template<typename T>
void ControlMovableT<T>::SetDraggableBorder(const UiPadding& rcDraggableBorder)
{
    m_rcDraggableBorder = rcDraggableBorder;
    m_rcDraggableBorder.left = std::max(m_rcDraggableBorder.left, 0);
    m_rcDraggableBorder.top = std::max(m_rcDraggableBorder.top, 0);
    m_rcDraggableBorder.right = std::max(m_rcDraggableBorder.right, 0);
    m_rcDraggableBorder.bottom = std::max(m_rcDraggableBorder.bottom, 0);
}

template<typename T>
const UiPadding& ControlMovableT<T>::GetDraggableBorder() const
{
    return m_rcDraggableBorder;
}

template<typename T>
void ControlMovableT<T>::SetMovePosReserveWidth(int32_t nReserveWidth)
{
    m_nMovePosReserveWidth = nReserveWidth;
    if (m_nMovePosReserveWidth < 0) {
        m_nMovePosReserveWidth = 0;
    }
}

template<typename T>
int32_t ControlMovableT<T>::GetMovePosReserveWidth() const
{
    return m_nMovePosReserveWidth;
}

template<typename T>
void ControlMovableT<T>::SetMovePosReserveHeight(int32_t nReserveHeight)
{
    m_nMovePosReserveHeight = nReserveHeight;
    if (m_nMovePosReserveHeight < 0) {
        m_nMovePosReserveHeight = 0;
    }
}

template<typename T>
int32_t ControlMovableT<T>::GetMovePosReserveHeight() const
{
    return m_nMovePosReserveHeight;
}

template<typename T>
void ControlMovableT<T>::SetMovePosKeepWithinParent(bool bKeepWithinParent)
{
    m_bKeepWithinParent = bKeepWithinParent;
}

template<typename T>
bool ControlMovableT<T>::IsMovePosKeepWithinParent() const
{
    return m_bKeepWithinParent;
}

template<typename T>
bool ControlMovableT<T>::IsMovingPos() const
{
    return m_bMovingPos;
}

template<typename T>
void ControlMovableT<T>::CancelMovePosOperation()
{
    ClearMovePosStatus();
}

template<typename T>
Control* ControlMovableT<T>::GetMovePosControl()
{
    if (IsMoveParentPos() && (this->GetParent() != nullptr)) {
        return this->GetParent();
    }
    return this;
}

template<typename T>
bool ControlMovableT<T>::IsInDraggableArea(const UiPoint& ptMouse) const
{
    UiPoint pt(ptMouse);
    pt.Offset(this->GetScrollOffsetInScrollBox());
    UiRect rect = this->GetRect();
    if (!rect.ContainsPt(pt)) {
        return false;
    }
    if (!m_rcNonDraggableMargin.IsEmpty() || !m_rcNonDraggableMargin.IsEmpty()) {
        if (!m_rcNonDraggableMargin.IsEmpty()) {
            UiRect rc = rect;
            UiMargin rcMargin = m_rcNonDraggableMargin;
            this->Dpi().ScaleMargin(rcMargin);
            rc.Deflate(rcMargin);
            rc.Validate();
            if (rc.ContainsPt(pt)) {
                return true;
            }
        }
        if (!m_rcDraggableBorder.IsEmpty()) {
            UiRect rc = rect;
            UiPadding rcPadding = m_rcDraggableBorder;
            this->Dpi().ScalePadding(rcPadding);
            rc.Deflate(rcPadding);
            rc.Validate();
            if (rc.ContainsPt(pt)) {
                return false;
            }
            return true;
        }        
        return false;
    }
    else {
        return true;
    }
}

template<typename T>
bool ControlMovableT<T>::ButtonDown(const EventArgs& msg)
{
    m_bMouseDown = false;
    bool bRet = BaseClass::ButtonDown(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    if (!IsEnableMovePos()) {
        //Feature is disabled
        return bRet;
    }

    if (!IsInDraggableArea(msg.ptMouse)) {
        //The click position is not in the draggable area
        return bRet;
    }

    UiPoint pt(msg.ptMouse);
    pt.Offset(this->GetScrollOffsetInScrollBox());

    m_bMouseDown = true;
    m_ptMouseDown = pt;

    m_rcMouseDown = GetMovePosControl()->GetRect();
    return bRet;
}

template<typename T>
bool ControlMovableT<T>::MouseMove(const EventArgs& msg)
{
    bool bRet = BaseClass::MouseMove(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    if (m_bMouseDown) {
        MovePosMouseMove(msg);
    }
    return bRet;
}

template<typename T>
bool ControlMovableT<T>::ButtonUp(const EventArgs& msg)
{
    bool bRet = BaseClass::ButtonUp(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    if (m_bMouseDown) {
        MovePosMouseUp(msg);
    }
    ClearMovePosStatus();
    return bRet;
}

template<typename T>
bool ControlMovableT<T>::OnWindowKillFocus(const EventArgs& msg)
{
    bool bRet = BaseClass::OnWindowKillFocus(msg);
    ClearMovePosStatus();
    return bRet;
}

template<typename T>
void ControlMovableT<T>::HandleEvent(const EventArgs& msg)
{
    BaseClass::HandleEvent(msg);
    if (msg.IsSenderExpired()) {
        return;
    }
    if ((msg.eventType == kEventMouseClickChanged) ||
        (msg.eventType == kEventMouseClickEsc) ||
        (msg.eventType == kEventMouseRButtonDown)) {
        //Cancel the operation
        ClearMovePosStatus();
    }
}

template<typename T>
void ControlMovableT<T>::ClearMovePosStatus()
{
    //Restore the status
    if (m_bMovingPos) {
        Control* pControl = GetMovePosControl();
        pControl->SetAlpha(m_nOldAlpha);
        m_nOldAlpha = 255;
        m_bMovingPos = false;
    }
    m_bMouseDown = false;
}

template<typename T>
bool ControlMovableT<T>::MovePosMouseMove(const EventArgs& msg)
{
    if (!IsEnableMovePos()) {
        //The control is not allowed to be dragged to adjust its size
        return false;
    }
    if (!m_bMouseDown) {
        return false;
    }

    //Move the control to the mouse position
    MoveControlToPoint(msg.ptMouse);

    if (!m_bMovingPos) {
        Control* pControl = GetMovePosControl();
        pControl->SetKeepFloatPos(true);
        if (!pControl->IsFloat()) {
            pControl->SetFloat(true);
            MoveControlToPoint(msg.ptMouse);
        }

        m_bMovingPos = true;
        m_nOldAlpha = pControl->GetAlpha();
        //Set the semi-transparent effect
        pControl->SetAlpha(m_nMovePosAlpha);
    }
    return m_bMovingPos;
}

template<typename T>
void ControlMovableT<T>::MovePosMouseUp(const EventArgs& msg)
{
    if (!IsEnableMovePos() || !IsMovingPos()) {
        return;
    }
    MoveControlToPoint(msg.ptMouse);
    ClearMovePosStatus();
}

template<typename T>
void ControlMovableT<T>::MoveControlToPoint(const UiPoint& ptMouse)
{
    UiPoint pt(ptMouse);
    pt.Offset(this->GetScrollOffsetInScrollBox());

    int32_t xOffset = pt.x - m_ptMouseDown.x;
    int32_t yOffset = pt.y - m_ptMouseDown.y;

    Control* pControl = GetMovePosControl();
    UiRect rcNewPos = m_rcMouseDown;
    rcNewPos.left += xOffset;
    rcNewPos.right += xOffset;

    rcNewPos.top += yOffset;
    rcNewPos.bottom += yOffset;

    //Limit the control within the parent container range, to avoid completely overflowing the parent container
    AdjustRectInParentBox(pControl, xOffset, yOffset, rcNewPos);

    //Adjust the position of the control
    pControl->SetPos(rcNewPos);
}

template<typename T>
void ControlMovableT<T>::AdjustRectInParentBox(Control* pControl, int32_t xOffset, int32_t yOffset, UiRect& rcNewPos) const
{
    Control* pParent = nullptr;
    if (pControl != nullptr) {
        pParent = pControl->GetParent();
    }
    if (pParent == nullptr) {
        return;
    }
    UiRect rcParent = pParent->GetRect();
    rcParent.Deflate(pParent->GetPadding());// Subtract the padding

    if (IsMovePosKeepWithinParent()) {
        //Completely limit the control within the parent container range, to avoid overflowing
        if (xOffset < 0) {
            //Move to the left
            if (rcNewPos.left < rcParent.left) {
                int32_t offset = rcParent.left - rcNewPos.left;
                rcNewPos.Offset(offset, 0);
            }
        }
        else if (xOffset > 0) {
            //Move to the right
            if (rcNewPos.right > rcParent.right) {
                int32_t offset = rcNewPos.right - rcParent.right;
                rcNewPos.Offset(-offset, 0);
            }
        }
        if (yOffset < 0) {
            //Move up
            if (rcNewPos.top < rcParent.top) {
                int32_t offset = rcParent.top - rcNewPos.top;
                rcNewPos.Offset(0, offset);
            }
        }
        else if (yOffset > 0) {
            //Move down
            if (rcNewPos.bottom > rcParent.bottom) {
                int32_t offset = rcNewPos.bottom - rcParent.bottom;
                rcNewPos.Offset(0, -offset);
            }
        }
    }
    else {
        //Partial overflow is allowed, but complete overflow is not
        int32_t nReserveValueX = GetMovePosReserveWidth();
        this->Dpi().ScaleInt(nReserveValueX);
        if (nReserveValueX > 0) {
            if (xOffset < 0) {
                //Move to the left
                if (rcNewPos.right < (rcParent.left + nReserveValueX)) {
                    int32_t offset = (rcParent.left + nReserveValueX) - rcNewPos.right;
                    rcNewPos.Offset(offset, 0);
                }
            }
            else if (xOffset > 0) {
                //Move to the right
                if (rcNewPos.left > (rcParent.right - nReserveValueX)) {
                    int32_t offset = rcNewPos.left - (rcParent.right - nReserveValueX);
                    rcNewPos.Offset(-offset, 0);
                }
            }
        }

        int32_t nReserveValueY = GetMovePosReserveHeight();
        this->Dpi().ScaleInt(nReserveValueY);
        if (yOffset < 0) {
            //Move up
            if (rcNewPos.bottom < (rcParent.top + nReserveValueY)) {
                int32_t offset = (rcParent.top + nReserveValueY) - rcNewPos.bottom;
                rcNewPos.Offset(0, offset);
            }
        }
        else if (yOffset > 0) {
            //Move down
            if (rcNewPos.top > (rcParent.bottom - nReserveValueY)) {
                int32_t offset = rcNewPos.top - (rcParent.bottom - nReserveValueY);
                rcNewPos.Offset(0, -offset);
            }
        }
    }
}

typedef ControlMovableT<Control> ControlMovable;
typedef ControlMovableT<Box> BoxMovable;
typedef ControlMovableT<HBox> HBoxMovable;
typedef ControlMovableT<VBox> VBoxMovable;

}

#endif // UI_CORE_CONTROL_MOVABLE_H_
