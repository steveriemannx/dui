#ifndef UI_CONTROL_SPLIT_H_
#define UI_CONTROL_SPLIT_H_

#include "duilib/Core/Control.h"
#include "duilib/Core/Box.h"

namespace ui
{

/** Split bar control, the width or height of the left/right or top/bottom controls can be changed by dragging the split bar
*   Usage: if placed in a horizontal layout (HLayout), drag left and right
*        if placed in a vertical layout (VLayout), drag up and down
*   Note: if both controls are set to stretch type, the split bar cannot work properly.
*/
template<typename InheritType = Control>
class SplitTemplate : public InheritType
{
    typedef InheritType BaseClass;
public:
    explicit SplitTemplate(Window* pWindow);

    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;
 
    /** Whether it can be dragged
    */
    bool IsSplitDragValid();

    /** Stop dragging
    */
    void StopSplitDrag();

    /** Set whether adjusting its width is allowed when there is only one control
    */
    void SetEnableSplitSingle(bool bEnableSplitSingle);

    /** Get whether adjusting its width is allowed when there is only one control
    */
    bool IsEnableSplitSingle() const;

    /** Listen for the drag event
     * @param [in] callback The callback function triggered when selected
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     */
    void AttachSplitDraged(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventSplitDraged, callback, callbackID); }

protected:
    virtual bool MouseEnter(const EventArgs& msg) override;
    virtual bool ButtonDown(const EventArgs& msg) override;
    virtual bool ButtonUp(const EventArgs& msg) override;
    virtual bool MouseMove(const EventArgs& msg) override;

private:
    //Whether the container is a vertical layout
    bool IsVLayout(Control* pControl) const;

    //Whether the container is a horizontal layout
    bool IsHLayout(Control* pControl) const;

    /** Adjust the width or height of one of the two controls (pFirst)
    * @param [in] bHLayout true means horizontal layout, false means vertical layout
    * @param [in] nTotal The total width or height of the two controls
    * @param [in] nOffset The mouse width or height offset
    * @param [in] pFirst The first control interface, this function will adjust the width or height of the control corresponding to this interface
    * @param [in] nFirstFixedInt The original width or height value of the first control (recorded when the mouse button is pressed)
    * @param [in] pSecond The second control interface, read its minimum value to avoid not leaving enough minimum space when adjusting the first control
    * @return Returns the width or height of the first control
    */
    int32_t CalculateControlPos(bool bHLayout, const int32_t nTotal, const int32_t nOffset,
                                Control* pFirst, const UiFixedInt& nFirstFixedInt,
                                const Control* pSecond) const;

    /** Only adjust the size of one control
    */
    void OnMouseMoveSingle(const EventArgs& msg);

private:
    //Control interface of the left side (left/right drag) or top side (up/down drag)
    Control* m_pLeftTop;

    //Control interface of the right side (left/right drag) or bottom side (up/down drag)
    Control* m_pRightBottom;

    //Width (left/right drag) or height (up/down drag) of the control on the left side (left/right drag) or top side (up/down drag)
    UiFixedInt m_nLeftUpFixedValue;

    //Width (left/right drag) or height (up/down drag) of the control on the right side (left/right drag) or bottom side (up/down drag)
    UiFixedInt m_nRightBottomFixedValue;

    //Mouse position when dragging starts
    UiPoint m_ptStart;

    //Layout direction: true means horizontal layout, false means vertical layout
    bool m_bHLayout;

    /** Whether adjusting its width is allowed when there is only one control
    */
    bool m_bEnableSplitSingle;
};

template<typename InheritType>
SplitTemplate<InheritType>::SplitTemplate(Window* pWindow):
    InheritType(pWindow),
    m_pLeftTop(nullptr),
    m_pRightBottom(nullptr),
    m_bHLayout(true),
    m_bEnableSplitSingle(false)
{
    this->SetMouseEnabled(true);
}

template<typename InheritType>
inline DString SplitTemplate<InheritType>::GetType() const { return DUI_CTR_SPLIT; }

template<>
inline DString SplitTemplate<Box>::GetType() const { return DUI_CTR_SPLITBOX; }

template<typename InheritType>
void SplitTemplate<InheritType>::SetAttribute(const DString& strName, const DString& strValue)
{
    if (strName == _T("enable_split_single")) {
        SetEnableSplitSingle(strValue == _T("true"));
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

template<typename InheritType>
void SplitTemplate<InheritType>::SetEnableSplitSingle(bool bEnableSplitSingle)
{
    m_bEnableSplitSingle = bEnableSplitSingle;
}

template<typename InheritType>
bool SplitTemplate<InheritType>::IsEnableSplitSingle() const
{
    return m_bEnableSplitSingle;
}

template<typename InheritType>
bool SplitTemplate<InheritType>::IsSplitDragValid() 
{
    bool bDragValid = false;
    if (IsEnableSplitSingle()) {
        bDragValid = (m_pLeftTop != nullptr) || (m_pRightBottom != nullptr);
    }
    else {
        bDragValid = (m_pLeftTop != nullptr) && (m_pRightBottom != nullptr);
    }
    return bDragValid;
}

template<typename InheritType>
void SplitTemplate<InheritType>::StopSplitDrag() 
{
    m_pLeftTop = nullptr; 
    m_pRightBottom = nullptr;
}

template<typename InheritType>
bool SplitTemplate<InheritType>::IsVLayout(Control* pControl) const
{
    const Box* pBox = dynamic_cast<Box*>(pControl);
    if (pBox == nullptr) {
        return false;
    }
    const Layout* pLayout = pBox->GetLayout();
    if (pLayout == nullptr) {
        return false;
    }
    return pLayout->IsVLayout();
}

template<typename InheritType>
bool SplitTemplate<InheritType>::IsHLayout(Control* pControl) const
{
    const Box* pBox = dynamic_cast<Box*>(pControl);
    if (pBox == nullptr) {
        return false;
    }
    const Layout* pLayout = pBox->GetLayout();
    if (pLayout == nullptr) {
        return false;
    }
    return pLayout->IsHLayout();
}

template<typename InheritType>
bool SplitTemplate<InheritType>::MouseEnter(const EventArgs& msg)
{
    if (IsHLayout(this->GetParent())) {
        //Horizontal layout
        if (this->GetCursorType() != CursorType::kCursorSizeWE) {
            this->SetCursorType(CursorType::kCursorSizeWE);
        }
    }
    else if (IsVLayout(this->GetParent())) {
        //Vertical layout
        if (this->GetCursorType() != CursorType::kCursorSizeNS) {
            this->SetCursorType(CursorType::kCursorSizeNS);
        }
    }
    return BaseClass::MouseEnter(msg);
}

template<typename InheritType>
bool SplitTemplate<InheritType>::ButtonDown(const EventArgs& msg)
{
    bool bRet = BaseClass::ButtonDown(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    Box* pParent = this->GetParent();
    if (!this->IsEnabled() || (pParent == nullptr)) {
        return bRet;
    }
    size_t nChildCount = pParent->GetItemCount();
    Control* pPrev = nullptr;
    Control* pNext = nullptr;
    Control* pThis = nullptr;
    for (size_t i = 0; i < nChildCount; ++i) {
        Control* pControl = pParent->GetItemAt(i);
        if ((pControl == nullptr) || !pControl->IsVisible() || pControl->IsFloat()) {
            continue;
        }
        if (pThis) {
            pNext = pControl;
            break;
        }
        if (pControl == this) {
            pThis = pControl;
        }
        else {
            pPrev = pControl;
        }
    }
    bool bDragValid = false;
    if (IsEnableSplitSingle()) {
        bDragValid = (pPrev != nullptr) || (pNext != nullptr);
    }
    else {
        bDragValid = (pPrev != nullptr) && (pNext != nullptr);
    }
    if (bDragValid && (pThis != nullptr)) {
        if (IsVLayout(pParent)) {
            m_bHLayout = false;
            m_pLeftTop = pPrev;
            m_pRightBottom = pNext;
            m_ptStart = msg.ptMouse;

            m_nLeftUpFixedValue = UiFixedInt(0);
            if (m_pLeftTop != nullptr) {
                m_nLeftUpFixedValue = m_pLeftTop->GetFixedHeight();
                if (!m_nLeftUpFixedValue.IsStretch()) {
                    m_nLeftUpFixedValue.SetInt32(m_pLeftTop->GetHeight());
                }
            }

            m_nRightBottomFixedValue = UiFixedInt(0);
            if (m_pRightBottom != nullptr) {
                m_nRightBottomFixedValue = m_pRightBottom->GetFixedHeight();
                if (!m_nRightBottomFixedValue.IsStretch()) {
                    m_nRightBottomFixedValue.SetInt32(m_pRightBottom->GetHeight());
                }
            }
        }
        else if (IsHLayout(pParent)) {
            m_bHLayout = true;
            m_pLeftTop = pPrev;
            m_pRightBottom = pNext;
            m_ptStart = msg.ptMouse;

            m_nLeftUpFixedValue = UiFixedInt(0);
            if (m_pLeftTop != nullptr) {
                m_nLeftUpFixedValue = m_pLeftTop->GetFixedWidth();
                if (!m_nLeftUpFixedValue.IsStretch()) {
                    m_nLeftUpFixedValue.SetInt32(m_pLeftTop->GetWidth());
                }
            }
            
            m_nRightBottomFixedValue = UiFixedInt(0);
            if (m_pRightBottom != nullptr) {
                m_nRightBottomFixedValue = m_pRightBottom->GetFixedWidth();
                if (!m_nRightBottomFixedValue.IsStretch()) {
                    m_nRightBottomFixedValue.SetInt32(m_pRightBottom->GetWidth());
                }
            }
        }
    }
    if (!IsSplitDragValid()) {
        StopSplitDrag();
    }
    if ((m_pLeftTop != nullptr) && (m_pRightBottom != nullptr)) {
        //If both controls are stretch type, make the left (top) one non-stretch, allowing the drag operation
        if (m_nLeftUpFixedValue.IsStretch() && m_nRightBottomFixedValue.IsStretch()) {
            if (IsVLayout(pParent)) {
                m_nLeftUpFixedValue.SetInt32(m_pLeftTop->GetHeight());
            }
            else {
                m_nLeftUpFixedValue.SetInt32(m_pLeftTop->GetWidth());
            }
        }
    }
    this->Invalidate();
    return bRet;
}

template<typename InheritType>
bool SplitTemplate<InheritType>::ButtonUp(const EventArgs& msg)
{
    bool bRet = BaseClass::ButtonUp(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    StopSplitDrag();
    return bRet;
}

template<typename InheritType>
int32_t SplitTemplate<InheritType>::CalculateControlPos(bool bHLayout, const int32_t nTotal, const int32_t nOffset,
                                                        Control* pFirst, const UiFixedInt& nFirstFixedInt,
                                                        const Control* pSecond) const
{
    ASSERT((pFirst != nullptr) && (pSecond != nullptr));
    if ((pFirst == nullptr) || (pSecond == nullptr)) {
        return 0;
    }
    int32_t nNewValue = nFirstFixedInt.GetInt32() + nOffset; //New value, can be positive or negative
    if (nNewValue < 0) {
        nNewValue = 0;
    }

    int32_t nMin = 0; //Minimum value
    if (bHLayout) {
        nMin = pFirst->GetMinWidth();
    }
    else {
        nMin = pFirst->GetMinHeight();
    }    
    //Keep no smaller than the minimum value
    if (nNewValue < nMin) {
        nNewValue = nMin;
    }

    int32_t nMax = 0; //Maximum value, and set according to the minimum value of the other side, leaving the minimum space
    if (bHLayout) {
        nMax = pFirst->GetMaxWidth();
        if (pSecond->GetMinWidth() > 0) {
            int32_t nNewMax = nTotal - pSecond->GetMinWidth();
            if ((nNewMax > 0) && (nMax > nNewMax)) {
                nMax = nNewMax;
            }
        }
    }
    else {
        nMax = pFirst->GetMaxHeight();
        if (pSecond->GetMinHeight() > 0) {
            int32_t nNewMax = nTotal - pSecond->GetMinHeight();
            if ((nNewMax > 0) && (nMax > nNewMax)) {
                nMax = nNewMax;
            }
        }
    }
    //Keep no larger than the maximum value
    if (nNewValue > nMax) {
        nNewValue = nMax;
    }
    return nNewValue;    
}

template<typename InheritType>
bool SplitTemplate<InheritType>::MouseMove(const EventArgs& msg)
{
    bool bRet = BaseClass::MouseMove(msg);
    if (!IsSplitDragValid() || !this->IsMouseFocused()) {
        return bRet;
    }

    if ((m_pLeftTop == nullptr) || (m_pRightBottom == nullptr)) {
        //Only one control needs to be resized
        OnMouseMoveSingle(msg);
        return bRet;
    }
    
    int32_t nOffset = 0; //Drag range (width or height offset, can be positive or negative)
    if (m_bHLayout) {
        nOffset = msg.ptMouse.x - m_ptStart.x;
    }
    else {
        nOffset = msg.ptMouse.y - m_ptStart.y;
    }

    //When dragging, must not exceed the total width value or total height value
    if (nOffset > 0) {
        if (!m_nRightBottomFixedValue.IsStretch()) {
            if (nOffset > m_nRightBottomFixedValue.GetInt32()) {
                nOffset = m_nRightBottomFixedValue.GetInt32();
            }
        }
    }
    else if(nOffset < 0){
        if (!m_nLeftUpFixedValue.IsStretch()) {
            if (-nOffset > m_nLeftUpFixedValue.GetInt32()) {
                nOffset = -m_nLeftUpFixedValue.GetInt32();
            }
        }
    }
    else {
        return bRet;
    }
    
    int32_t nTotal = 0; //Total width value or total height value
    if (m_nLeftUpFixedValue.IsInt32() && m_nRightBottomFixedValue.IsInt32()) {
        nTotal = m_nLeftUpFixedValue.GetInt32() + m_nRightBottomFixedValue.GetInt32();
    }
    else {
        if (m_bHLayout) {
            nTotal = m_pLeftTop->GetWidth() + m_pRightBottom->GetWidth();
        }
        else {
            nTotal = m_pLeftTop->GetHeight() + m_pRightBottom->GetHeight();
        }
    }

    //For stretch-type controls, do not adjust, let the parent container adjust automatically (if both controls are stretch type, the split bar cannot work)
    Control* pControl1 = nullptr;
    Control* pControl2 = nullptr;
    int32_t nNewValue1 = 0;
    int32_t nNewValue2 = 0;
    if (!m_nLeftUpFixedValue.IsStretch()) {
        pControl1 = m_pLeftTop;
        nNewValue1 = CalculateControlPos(m_bHLayout, nTotal, nOffset, m_pLeftTop, m_nLeftUpFixedValue, m_pRightBottom);
    }
    else {
        nNewValue1 = -1;
    }
    if (!m_nRightBottomFixedValue.IsStretch()) {
        pControl2 = m_pRightBottom;
        nNewValue2 = CalculateControlPos(m_bHLayout, nTotal, -nOffset, m_pRightBottom, m_nRightBottomFixedValue, m_pLeftTop);
    }
    else {
        nNewValue2 = -1;
    }
    if ((nNewValue1 < 0) && (nNewValue2 < 0)) {
        //Both controls are stretch type, do not adjust
        return bRet;
    }
    else if ((nNewValue1 >= 0) && (nNewValue2 >= 0)) {
        if ((nNewValue1 + nNewValue2) != nTotal) {
            //Exceeds the limit, do not adjust
            return bRet;
        }
    }
    else if (nNewValue1 >= 0) {
        if (nNewValue1 > nTotal) {
            //Exceeds the limit, do not adjust
            return bRet;
        }
    }
    else if (nNewValue2 >= 0) {
        if (nNewValue2 > nTotal) {
            //Exceeds the limit, do not adjust
            return bRet;
        }
    }
    
    bool bAdjusted = false;
    if ((pControl1 != nullptr) && (nNewValue1 >= 0)) {
        bAdjusted = true;
        if (m_bHLayout) {
            pControl1->SetFixedWidth(UiFixedInt(nNewValue1), true, false);
        }
        else {
            pControl1->SetFixedHeight(UiFixedInt(nNewValue1), true, false);
        }
    }
    if ((pControl2 != nullptr) && (nNewValue2 >= 0)) {
        bAdjusted = true;
        if (m_bHLayout) {
            pControl2->SetFixedWidth(UiFixedInt(nNewValue2), true, false);
        }
        else {
            pControl2->SetFixedHeight(UiFixedInt(nNewValue2), true, false);
        }
    }

    if (bAdjusted && ((pControl1 != nullptr) || (pControl2 != nullptr))) {
        this->SendEvent(kEventSplitDraged, (WPARAM)pControl1, (LPARAM)pControl2);
    }
    return bRet;
}

template<typename InheritType>
void SplitTemplate<InheritType>::OnMouseMoveSingle(const EventArgs& msg)
{
    if ((m_pLeftTop == nullptr) && (m_pRightBottom == nullptr)) {
        return;
    }
    if ((m_pLeftTop != nullptr) && (m_pRightBottom != nullptr)) {
        return;
    }
    if ((m_pRightBottom != nullptr) && m_nRightBottomFixedValue.IsStretch()) {
        //The control is stretch type, do not adjust
        return;
    }
    if ((m_pLeftTop != nullptr) && m_nLeftUpFixedValue.IsStretch()) {
        //The control is stretch type, do not adjust
        return;
    }

    int32_t nOffset = 0; //Drag range (width or height offset, can be positive or negative)
    if (m_bHLayout) {
        nOffset = msg.ptMouse.x - m_ptStart.x;
    }
    else {
        nOffset = msg.ptMouse.y - m_ptStart.y;
    }

    //When dragging, must not exceed the total width value or total height value
    if (nOffset > 0) {
        if ((m_pRightBottom != nullptr) && !m_nRightBottomFixedValue.IsStretch()) {
            if (nOffset > m_nRightBottomFixedValue.GetInt32()) {
                nOffset = m_nRightBottomFixedValue.GetInt32();
            }
        }
    }
    else if (nOffset < 0) {
        if ((m_pLeftTop != nullptr) && !m_nLeftUpFixedValue.IsStretch()) {
            if (-nOffset > m_nLeftUpFixedValue.GetInt32()) {
                nOffset = -m_nLeftUpFixedValue.GetInt32();
            }
        }
    }
    else {
        return;
    }

    UiFixedInt nFixedInt;
    Control* pControl = nullptr;
    if (m_pLeftTop != nullptr) {
        pControl = m_pLeftTop;
        nFixedInt = m_nLeftUpFixedValue;
    }
    else if (m_pRightBottom != nullptr) {
        pControl = m_pRightBottom;
        nFixedInt = m_nRightBottomFixedValue;
    }
    if (pControl == nullptr) {
        return;
    }

    int32_t nNewValue = nFixedInt.GetInt32() + nOffset; //New value, can be positive or negative
    if (nNewValue < 0) {
        nNewValue = 0;
    }
    int32_t nMin = 0; //Minimum value
    bool bHLayout = m_bHLayout;
    if (bHLayout) {
        nMin = pControl->GetMinWidth();
    }
    else {
        nMin = pControl->GetMinHeight();
    }
    //Keep no smaller than the minimum value
    if (nNewValue < nMin) {
        nNewValue = nMin;
    }

    int32_t nMax = 0; //Maximum value, and set according to the minimum value of the other side, leaving the minimum space
    if (bHLayout) {
        nMax = pControl->GetMaxWidth();
    }
    else {
        nMax = pControl->GetMaxHeight();
    }
    //Keep no larger than the maximum value
    if (nNewValue > nMax) {
        nNewValue = nMax;
    }

    bool bAdjusted = false;
    if ((pControl != nullptr) && (nNewValue >= 0)) {
        bAdjusted = true;
        if (bHLayout) {
            pControl->SetFixedWidth(UiFixedInt(nNewValue), true, false);
        }
        else {
            pControl->SetFixedHeight(UiFixedInt(nNewValue), true, false);
        }
    }
    if (bAdjusted && (pControl != nullptr)) {
        this->SendEvent(kEventSplitDraged, (WPARAM)pControl, (LPARAM)nullptr);
    }
}

typedef SplitTemplate<Control> Split;
typedef SplitTemplate<Box> SplitBox;

}//name space ui

#endif // UI_CONTROL_SPLIT_H_
