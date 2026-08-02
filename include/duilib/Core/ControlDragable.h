#ifndef UI_CORE_CONTROL_DRAGABLE_H_
#define UI_CORE_CONTROL_DRAGABLE_H_

#include "duilib/Core/Control.h"
#include "duilib/Core/Box.h"
#include "duilib/Core/GlobalManager.h"
#include "duilib/Core/DragWindow.h"
#include "duilib/Box/VBox.h"
#include "duilib/Box/HBox.h"
#include "duilib/Box/ScrollBox.h"
#include "duilib/Utils/StringUtil.h"
#include "duilib/Core/WindowCreateParam.h"

#ifdef DUILIB_BUILD_FOR_SDL
    #include "duilib/Core/DragWindowFilter_SDL.h"
#endif

// The shortest pixel distance for a drag operation
#define DRAG_OPERATION_MIN_PT 5

namespace ui
{
/** Main function: supports adjusting the order of child controls in the same Box by dragging
*/
template<typename T = Control>
class ControlDragableT: public T
{
    typedef T BaseClass;
public:
    explicit ControlDragableT(Window* pWindow);
    virtual ~ControlDragableT() override;

    /// Override the parent class methods to provide customized functionality; please refer to the parent class declarations
    virtual DString GetType() const override;    
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;

    /** Set whether dragging to change the control order is supported
    */
    void SetEnableDragOrder(bool bEnable);

    /** Get whether dragging to change the control order is supported
    */
    virtual bool IsEnableDragOrder() const;

    /** Set the alpha of the control when dragging to reorder
    */
    void SetDragAlpha(uint8_t nAlpha);

    /** Get the alpha of the control when dragging to reorder
    */
    uint8_t GetDragAlpha() const;

    /** Set whether the drag-out operation is supported
    */
    void SetEnableDragOut(bool bEnable);

    /** Get whether the drag-out operation is supported
    */
    virtual bool IsEnableDragOut() const;

    /** Whether currently in a drag-to-reorder operation
    */
    bool IsInDraggingOrder() const;

    /** Whether currently in a drag-out operation
    */
    bool IsInDraggingOut() const;

    /** Cancel the drag operation
    */
    void CancelDragOperation();

protected:
    /** @name Member functions related to dragging
    * @{ */

    virtual bool ButtonDown(const EventArgs& msg) override;
    virtual bool ButtonUp(const EventArgs& msg) override;
    virtual bool MouseMove(const EventArgs& msg) override;
    virtual bool OnWindowKillFocus(const EventArgs& msg) override;// The window to which the control belongs lost focus
    virtual void HandleEvent(const EventArgs& msg) override;

protected:
    /** The state of the child control
    */
    struct ItemStatus
    {
        Control* m_pItem = nullptr;
        UiRect m_rcPos;
        uint8_t m_nPaintOrder = 0;
        size_t m_index = Box::InvalidIndex;
    };

    /** Adjust the position of each control according to the mouse position (part of the drag operation)
    * @param [in] pt The current mouse position
    * @param [in] ptMouseDown The position when the mouse button was pressed down
    * @param [in] rcItemList The list of child controls
    */
    virtual void AdjustItemPos(const UiPoint& pt, const UiPoint& ptMouseDown,
                               const std::vector<ItemStatus>& rcItemList) const;

    /** Swap the positions of two controls to complete the order adjustment
    * @param [in] pt The current mouse position
    * @param [in] rcItemList The list of child controls
    * @param [in] nOldItemIndex The original item index
    * @param [in] nNewItemIndex The latest item index
    * @return Returns true if the order was adjusted; otherwise returns false
    */
    virtual bool AdjustItemOrders(const UiPoint& pt,
                                  const std::vector<ItemStatus>& rcItemList,
                                  size_t& nOldItemIndex,
                                  size_t& nNewItemIndex);

    /** The event fired when the control position dragging is finished (within the same container)
    * @param [in] nOldItemIndex The original item index
    * @param [in] nNewItemIndex The latest item index
    */
    virtual void OnItemOrdersChanged(size_t nOldItemIndex, size_t nNewItemIndex);

    /** Clear the drag state and restore the original state
    */
    virtual void ClearDragStatus();

    /** Get the valid range of the area where the items are located
    */
    virtual bool GetItemsValidRect(UiRect& itemsValidRect) const;

    /** @} */

protected:
    /** @name Member variables related to dragging between different Boxes in the same window
    * @{ */

    /** The drag and drop event
    * @param [in] ptMouse The current mouse position
    * @return Returns true if a target control to drop into was matched; returns false if no target control position was found
    */
    virtual bool OnDragOutMouseOver(Box* pTargetBox, Box* pOldTargetBox, const UiPoint& ptMouse);

    /** Create the follow window displayed during the drag-out operation
    */
    virtual DragWindow* CreateDragWindow();

    /** The target position indicator control
    * @param [in] pTargetBox The target Box control interface to be dropped into
    */
    virtual Control* CreateDestControl(Box* pTargetBox);

    /** Generate the bitmap of the control being dragged out
    */
    virtual std::shared_ptr<IBitmap> CreateDragoutImage();

    /** Determine whether the mouse position is within the rectangle of the control
    */
    virtual bool IsPtInControlRect(Control* pControl, const UiPoint& pt) const;

    /** The event fired when the drag of the control's container is finished (within the same container or between different containers)
    * @param [in] pOldBox The interface of the container the control originally belonged to
    * @param [in] nOldItemIndex The original item index
    * @param [in] pNewBox The interface of the container the control now belongs to
    * @param [in] nNewItemIndex The latest item index
    */
    virtual void OnItemBoxChanged(Box* pOldBox, size_t nOldItemIndex, 
                                  Box* pNewBox, size_t nNewItemIndex);

    /** @} */

private:
    /** The drag-out operation executed when the mouse moves
    * @return Returns true if in a drag-out operation, false if no drag-out operation was executed
    */
    bool DragOutMouseMove(const EventArgs& msg);

    /** The drag-out operation executed when the mouse button is released
    * @return Returns true if the drag-out operation was executed, false if not
    */
    bool DragOutMouseUp(const EventArgs& msg);

    /** The drag-to-reorder operation executed when the mouse button is pressed down
    */
    void DragOrderMouseDown(const EventArgs& msg);

    /** The drag-to-reorder operation executed when the mouse moves
    * @return Returns true if in a drag-to-reorder operation, false if no drag-to-reorder operation was executed
    */
    bool DragOrderMouseMove(const EventArgs& msg);

    /** The drag-out operation executed when the mouse button is released
    */
    void DragOrderMouseUp(const EventArgs& msg);

private:
    /** @name Member variables related to dragging within the same Box in the same window
    * @{ */

    /** Whether dragging to change the order is supported (feature switch)
    */
    bool m_bEnableDragOrder;

    /** Whether the left mouse button is pressed down
    */
    bool m_bMouseDown;

    /** Whether currently in a drag-to-reorder operation
    */
    bool m_bInDraggingOrder;

    /** The alpha of the control when dragging to reorder
    */
    uint8_t m_nDragAlpha;

    /** The original alpha
    */
    uint8_t m_nOldAlpha;

    /** The mouse position when the mouse button was pressed down
    */
    UiPoint m_ptMouseDown;

    /** The rectangle area of the control when the mouse button was pressed down
    */
    UiRect m_rcMouseDown;

    /** The position of each control in the parent container when the mouse button was pressed down
    */
    std::vector<ItemStatus> m_rcItemList;

    /** @} */

    /** @name Member variables related to dragging between different Boxes in the same window
    * @{ */

    /** The target Box
    */
    Box* m_pTargetBox;

    /** The target position indicator control
    */
    Control* m_pDestControl;

    /** The state window of the drag
    */
    DragWindow* m_pDragWindow;

#ifdef DUILIB_BUILD_FOR_SDL
    /** The message filter of the drag window
    */
    std::unique_ptr<IUIMessageFilter> m_pDragWindowFilter;
#endif

    /** The bitmap of the control being dragged out
    */
    std::shared_ptr<IBitmap> m_pDragImage;

    /** Whether the drag-out operation is supported (feature switch)
    */
    bool m_bEnableDragOut;

    /** Whether a drag-out operation is currently being executed
    */
    bool m_bDraggingOut;

    /** @} */
};

template<typename T>
ControlDragableT<T>::ControlDragableT(Window* pWindow):
    T(pWindow),
    m_bEnableDragOrder(true),
    m_bEnableDragOut(true),
    m_bInDraggingOrder(false),
    m_bMouseDown(false),
    m_nDragAlpha(216),
    m_nOldAlpha(255),    
    m_pTargetBox(nullptr),
    m_pDestControl(nullptr),
    m_pDragWindow(nullptr),
    m_bDraggingOut(false)
{
}

template<typename T>
ControlDragableT<T>::~ControlDragableT()
{
}

template<typename T>
inline DString ControlDragableT<T>::GetType() const { return DUI_CTR_CONTROL_DRAGABLE; }

template<>
inline DString ControlDragableT<Box>::GetType() const { return DUI_CTR_BOX_DRAGABLE; }

template<>
inline DString ControlDragableT<HBox>::GetType() const { return DUI_CTR_HBOX_DRAGABLE; }

template<>
inline DString ControlDragableT<VBox>::GetType() const { return DUI_CTR_VBOX_DRAGABLE; }

template<typename T>
void ControlDragableT<T>::SetAttribute(const DString& strName, const DString& strValue)
{
    if (strName == _T("drag_order")) {
        // Whether dragging to adjust the order is supported (within the same container)
        SetEnableDragOrder(strValue == _T("true"));
    }
    else if (strName == _T("drag_alpha")) {
        SetDragAlpha((uint8_t)StringUtil::StringToInt32(strValue));
    }
    else if (strName == _T("drag_out")) {
        // Whether the drag-out operation is supported (between different containers in the same window)
        SetEnableDragOut(strValue == _T("true"));
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

template<typename T>
void ControlDragableT<T>::SetEnableDragOrder(bool bEnable)
{
    m_bEnableDragOrder = bEnable;
}

template<typename T>
bool ControlDragableT<T>::IsEnableDragOrder() const
{
    return m_bEnableDragOrder;
}

template<typename T>
void ControlDragableT<T>::SetDragAlpha(uint8_t nAlpha)
{
    m_nDragAlpha = nAlpha;
}

template<typename T>
uint8_t ControlDragableT<T>::GetDragAlpha() const
{
    return m_nDragAlpha;
}

template<typename T>
void ControlDragableT<T>::SetEnableDragOut(bool bEnable)
{
    m_bEnableDragOut = bEnable;
}

template<typename T>
bool ControlDragableT<T>::IsEnableDragOut() const
{
    return m_bEnableDragOut;
}

template<typename T>
bool ControlDragableT<T>::IsInDraggingOrder() const
{
    return m_bInDraggingOrder;
}

template<typename T>
bool ControlDragableT<T>::IsInDraggingOut() const
{
    return m_bDraggingOut;
}

template<typename T>
void ControlDragableT<T>::CancelDragOperation()
{
    ClearDragStatus();
}

template<typename T>
bool ControlDragableT<T>::ButtonDown(const EventArgs& msg)
{
    m_bMouseDown = false;
    bool bRet = BaseClass::ButtonDown(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    if (!IsEnableDragOrder() && !IsEnableDragOut()) {
        return bRet;
    }
    const Box* pParent = this->GetParent();
    if (pParent == nullptr) {
        return bRet;
    }
    const Layout* pLayout = pParent->GetLayout();
    if ((pLayout == nullptr) || (!pLayout->IsHLayout() && !pLayout->IsVLayout())) {
        return bRet;
    }
    UiPoint pt(msg.ptMouse);
    pt.Offset(this->GetScrollOffsetInScrollBox());

    m_bMouseDown = true;
    m_ptMouseDown = pt;
    m_rcMouseDown = this->GetRect();

    DragOrderMouseDown(msg); 
    return bRet;
}

template<typename T>
bool ControlDragableT<T>::MouseMove(const EventArgs& msg)
{
    bool bRet = BaseClass::MouseMove(msg);
    if (m_bMouseDown) {
        m_bDraggingOut = DragOutMouseMove(msg);
        if (!m_bDraggingOut) {
            DragOrderMouseMove(msg);
        }
    }
    return bRet;
}

template<typename T>
bool ControlDragableT<T>::ButtonUp(const EventArgs& msg)
{
    bool bRet = BaseClass::ButtonUp(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    if (!DragOutMouseUp(msg)) {
        DragOrderMouseUp(msg);
    }
    ClearDragStatus();
    return bRet;
}

template<typename T>
bool ControlDragableT<T>::OnWindowKillFocus(const EventArgs& msg)
{
    bool bRet = BaseClass::OnWindowKillFocus(msg);
    ClearDragStatus();
    return bRet;
}

template<typename T>
void ControlDragableT<T>::HandleEvent(const EventArgs& msg)
{
    BaseClass::HandleEvent(msg);
    if ((msg.eventType == kEventMouseClickChanged) ||
        (msg.eventType == kEventMouseClickEsc) ||
        (msg.eventType == kEventMouseRButtonDown)) {
        // Cancel the drag-to-reorder or drag-out operation
        ClearDragStatus();
    }
}

template<typename T>
bool ControlDragableT<T>::GetItemsValidRect(UiRect& itemsValidRect) const
{
    const Box* pParent = this->GetParent();
    ASSERT(pParent != nullptr);
    if (pParent == nullptr) {
        return false;
    }

    UiRect boxRect = pParent->GetRect();// The area of the parent container
    UiPoint offsetParent = pParent->GetScrollOffsetInScrollBox();
    boxRect.Offset(-offsetParent.x, -offsetParent.y);// Convert to the client area coordinates of the window
    UiRect rcItemRects = boxRect;// The total area occupied by the items
    for (size_t nIndex = 0; nIndex < m_rcItemList.size(); ++nIndex) {
        const ItemStatus& itemStatus = m_rcItemList[nIndex];
        if ((itemStatus.m_pItem != nullptr) && !itemStatus.m_pItem->IsFloat()) {
            UiRect rcPos = itemStatus.m_rcPos;
            UiPoint offsetItem = itemStatus.m_pItem->GetScrollOffsetInScrollBox();
            rcPos.Offset(-offsetItem.x, -offsetItem.y);// Convert to the client area coordinates of the window
            if (rcPos.left > rcItemRects.left) {
                rcItemRects.left = rcPos.left;
            }
            if (rcPos.top > rcItemRects.top) {
                rcItemRects.top = rcPos.top;
            }
            break;
        }
    }
    for (int32_t nIndex = (int32_t)m_rcItemList.size() - 1; nIndex >= 0; --nIndex) {
        const ItemStatus& itemStatus = m_rcItemList[nIndex];
        if ((itemStatus.m_pItem != nullptr) && !itemStatus.m_pItem->IsFloat()) {
            UiRect rcPos = itemStatus.m_rcPos;
            UiPoint offsetItem = itemStatus.m_pItem->GetScrollOffsetInScrollBox();
            rcPos.Offset(-offsetItem.x, -offsetItem.y);// Convert to the client area coordinates of the window
            if (rcPos.right < rcItemRects.right) {
                rcItemRects.right = rcPos.right;
            }
            if (rcPos.bottom < rcItemRects.bottom) {
                rcItemRects.bottom = rcPos.bottom;
            }
            break;
        }
    }

    UiPoint offset = this->GetScrollOffsetInScrollBox();
    rcItemRects.Offset(offset);// Convert to the control position coordinates
    itemsValidRect = rcItemRects;
    return true;
}

template<typename T>
void ControlDragableT<T>::AdjustItemPos(const UiPoint& pt, const UiPoint& ptMouseDown,
                                        const std::vector<ItemStatus>& rcItemList) const
{
    Control* pMouseItem = nullptr;
    size_t nMouseItemIndex = Box::InvalidIndex;
    size_t nMouseDownItemIndex = Box::InvalidIndex;
    for (const ItemStatus& itemStatus : rcItemList) {
        if (itemStatus.m_rcPos.ContainsPt(pt)) {
            pMouseItem = itemStatus.m_pItem;
            nMouseItemIndex = itemStatus.m_index;
        }
        if (itemStatus.m_pItem == this) {
            nMouseDownItemIndex = itemStatus.m_index;
        }
    }
    if ((pMouseItem == nullptr) ||
        (nMouseItemIndex == Box::InvalidIndex) ||
        (nMouseDownItemIndex == Box::InvalidIndex)) {
        return;
    }
    ControlDragableT<T>* pItem = dynamic_cast<ControlDragableT<T>*>(pMouseItem);
    if (pItem == nullptr) {
        // The mouse is not on a draggable control
        return;
    }
    if (!pItem->IsEnableDragOrder()) {
        // The current item is fixed; reordering is not allowed
        return;
    }

    const size_t itemCount = rcItemList.size();
    int32_t xOffset = pt.x - ptMouseDown.x;
    if (pMouseItem == this) {
        // Current mouse position: at its own position, restore the actual positions of all controls
        for (const ItemStatus& item : rcItemList) {
            if (item.m_pItem == this) {
                continue;
            }
            if (item.m_pItem->GetRect() != item.m_rcPos) {
                item.m_pItem->SetPos(item.m_rcPos);
            }
        }
    }
    else if (xOffset < 0) {
        // Current mouse position: to the left of the press point, move the controls to the right
        for (size_t index = 0; index < itemCount; ++index) {
            const ItemStatus& item = rcItemList[index];
            if ((item.m_index >= nMouseItemIndex) && (item.m_index < nMouseDownItemIndex)) {
                // Move to the right
                if ((index + 1) < itemCount) {
                    const ItemStatus& nextItem = rcItemList[index + 1];
                    item.m_pItem->SetPos(nextItem.m_rcPos);
                }
                else {
                    if (item.m_pItem->GetRect() != item.m_rcPos) {
                        item.m_pItem->SetPos(item.m_rcPos);
                    }
                }
            }
            else {
                // Restore the original position
                if (item.m_pItem->GetRect() != item.m_rcPos) {
                    item.m_pItem->SetPos(item.m_rcPos);
                }
            }
        }
    }
    else {
        // Current mouse position: to the right of the press point, move the controls to the left
        for (size_t index = 0; index < itemCount; ++index) {
            const ItemStatus& item = rcItemList[index];
            if ((item.m_index > nMouseDownItemIndex) && (item.m_index <= nMouseItemIndex)) {
                // Move to the left
                if ((index - 1) < itemCount) {
                    const ItemStatus& nextItem = rcItemList[index - 1];
                    item.m_pItem->SetPos(nextItem.m_rcPos);
                }
                else {
                    if (item.m_pItem->GetRect() != item.m_rcPos) {
                        item.m_pItem->SetPos(item.m_rcPos);
                    }
                }
            }
            else {
                // Restore the original position
                if (item.m_pItem->GetRect() != item.m_rcPos) {
                    item.m_pItem->SetPos(item.m_rcPos);
                }
            }
        }
    }
}

template<typename T>
bool ControlDragableT<T>::AdjustItemOrders(const UiPoint& pt,
                                           const std::vector<ItemStatus>& rcItemList,
                                           size_t& nOldItemIndex,
                                           size_t& nNewItemIndex)
{
    nOldItemIndex = Box::InvalidIndex;
    nNewItemIndex = Box::InvalidIndex;
    Box* pParent = this->GetParent();
    if (pParent == nullptr) {
        return false;
    }

    bool bOrderChanged = false;
    const size_t itemCount = pParent->GetItemCount();
    size_t nMouseItemIndex = Box::InvalidIndex;
    size_t nCurrentItemIndex = Box::InvalidIndex;
    for (const ItemStatus& itemStatus : rcItemList) {
        if (itemStatus.m_rcPos.ContainsPt(pt)) {
            nMouseItemIndex = itemStatus.m_index;
            const ControlDragableT<T>* pItem = dynamic_cast<const ControlDragableT<T>*>(itemStatus.m_pItem);
            if ((pItem != nullptr) && !pItem->IsEnableDragOrder()) {
                // The current control is fixed; reordering is not allowed
                nMouseItemIndex = Box::InvalidIndex;
            }
        }
        if ((itemStatus.m_pItem == this) && !itemStatus.m_rcPos.ContainsPt(pt)) {
            nCurrentItemIndex = itemStatus.m_index;
        }
    }
    if ((nMouseItemIndex < itemCount) && (nCurrentItemIndex < itemCount)) {
        // Adjust the position of the control
        pParent->SetItemIndex(this, nMouseItemIndex);
        nOldItemIndex = nCurrentItemIndex;
        nNewItemIndex = nMouseItemIndex;
        bOrderChanged = true;
    }
    return bOrderChanged;
}

template<typename T>
void ControlDragableT<T>::OnItemOrdersChanged(size_t /*nOldItemIndex*/, size_t /*nNewItemIndex*/)
{
}

template<typename T>
void ControlDragableT<T>::ClearDragStatus()
{
    // Restore the state of the drag-out operation
    if ((m_pDestControl != nullptr) && (m_pTargetBox != nullptr)) {
        if (m_pTargetBox->RemoveItem(m_pDestControl)) {
            m_pDestControl = nullptr;
        }
    }
    if (m_pDragWindow != nullptr) {
#ifdef DUILIB_BUILD_FOR_SDL
        if (m_pDragWindowFilter != nullptr) {
            m_pDragWindow->RemoveMessageFilter(m_pDragWindowFilter.get());
        }
#endif
        if (!m_pDragWindow->IsClosingWnd()) {
            m_pDragWindow->SetDragImage(nullptr);
            m_pDragWindow->CloseWnd();
        }
        m_pDragWindow->Release();
        m_pDragWindow = nullptr;

#ifdef DUILIB_BUILD_FOR_SDL
        m_pDragWindowFilter.reset();
#endif
    }

    const bool bInDraggingOut = IsInDraggingOut();
    m_pDragImage.reset();
    m_pTargetBox = nullptr;
    m_bDraggingOut = false;

    if (bInDraggingOut) {
        // Only restore visibility if the drag-out operation was executed, to avoid side effects
        this->SetVisible(true);
    }

    // Restore the state of the drag-to-reorder operation
    if (m_bInDraggingOrder) {
        this->SetAlpha(m_nOldAlpha);
        m_nOldAlpha = 255;
        m_bInDraggingOrder = false;
    }
    m_bMouseDown = false;
    if (!m_rcItemList.empty()) {
        // Restore the paint order of the child controls
        for (const ItemStatus& itemStatus : m_rcItemList) {
            if ((itemStatus.m_pItem != nullptr) &&
                (itemStatus.m_nPaintOrder != itemStatus.m_pItem->GetPaintOrder())) {
                itemStatus.m_pItem->SetPaintOrder(itemStatus.m_nPaintOrder);
            }
        }
        m_rcItemList.clear();
        // Repaint the parent control
        Control* pParent = this->GetParent();
        if (pParent != nullptr) {
            pParent->Invalidate();
            pParent->SetPos(pParent->GetPos());
        }
    }
}

template<typename T>
bool ControlDragableT<T>::DragOutMouseUp(const EventArgs& msg)
{
    if (!IsInDraggingOut() || !IsEnableDragOut() || (m_pTargetBox == nullptr)) {
        return false;
    }
    // The drag-out operation of the control
    Box* pParent = this->GetParent();
    UiPoint pt(msg.ptMouse);
    pt.Offset(m_pTargetBox->GetScrollOffsetInScrollBox());
    if ((pParent == nullptr) || !m_pTargetBox->GetRect().ContainsPt(pt)) {
        return false;
    }
    size_t nIndex = Box::InvalidIndex;
    if (m_pDestControl != nullptr) {
        nIndex = m_pTargetBox->GetItemIndex(m_pDestControl);
    }
    if (nIndex == Box::InvalidIndex) {
        const size_t nCount = m_pTargetBox->GetItemCount();
        for (size_t nItem = 0; nItem < nCount; ++nItem) {
            Control* pControl = m_pTargetBox->GetItemAt(nItem);
            if ((pControl == nullptr) || !pControl->IsVisible() || pControl->IsFloat()) {
                continue;
            }
            if (IsPtInControlRect(pControl, pt)) {
                nIndex = nItem;
                break;
            }
        }
    }
    if (nIndex < m_pTargetBox->GetItemCount()) {
        if (m_pDestControl != nullptr) {
            m_pTargetBox->RemoveItem(m_pDestControl);
            m_pDestControl = nullptr;
        }

        Box* pOldBox = pParent;
        size_t nOldItemIndex = pParent->GetItemIndex(this);
        Box* pNewBox = m_pTargetBox;
        size_t nNewItemIndex = nIndex;

        bool bAutoDestroyChild = pParent->IsAutoDestroyChild();
        pParent->SetAutoDestroyChild(false);
        pParent->RemoveItem(this);
        pParent->SetAutoDestroyChild(bAutoDestroyChild);

        this->SetVisible(true);
        m_pTargetBox->AddItem(this);
        m_pTargetBox->SetItemIndex(this, nIndex);
        m_pTargetBox = nullptr;

        // Fire the event
        OnItemBoxChanged(pOldBox, nOldItemIndex, pNewBox, nNewItemIndex);
        return true;
    }
    return false;
}

template<typename T>
void ControlDragableT<T>::OnItemBoxChanged(Box* /*pOldBox*/, size_t /*nOldItemIndex*/,
                                           Box* /*pNewBox*/, size_t /*nNewItemIndex*/)
{
}

template<typename T>
void ControlDragableT<T>::DragOrderMouseUp(const EventArgs& msg)
{
    if (!IsEnableDragOrder() || !IsInDraggingOrder()) {
        return;
    }
    // Within the control: drag to change the order
    UiPoint pt(msg.ptMouse);
    pt.Offset(this->GetScrollOffsetInScrollBox());
    UiRect itemsValidRect;
    if (GetItemsValidRect(itemsValidRect)) {
        if (pt.x < itemsValidRect.left) {
            pt.x = itemsValidRect.left;
        }
        if (pt.x >= itemsValidRect.right) {
            pt.x = itemsValidRect.right - 1;
        }
        if (pt.y < itemsValidRect.top) {
            pt.y = itemsValidRect.top;
        }
        if (pt.y >= itemsValidRect.bottom) {
            pt.y = itemsValidRect.bottom - 1;
        }
    }
    size_t nOldItemIndex = Box::InvalidIndex;
    size_t nNewItemIndex = Box::InvalidIndex;
    bool bOrderChanged = AdjustItemOrders(pt, m_rcItemList, nOldItemIndex, nNewItemIndex);
    ClearDragStatus();
    if (bOrderChanged) {
        // Fire the item order swap event
        OnItemOrdersChanged(nOldItemIndex, nNewItemIndex);
    }
}

template<typename T>
bool ControlDragableT<T>::OnDragOutMouseOver(Box* pTargetBox, Box* pOldTargetBox, const UiPoint& ptMouse)
{
    bool bDropped = false;
    if ((pTargetBox == nullptr) || (pTargetBox->GetLayout() == nullptr)) {
        return bDropped;
    }
    if ((pOldTargetBox != nullptr) && (pOldTargetBox != pTargetBox) && (m_pDestControl != nullptr)) {
        // Remove from the old one
        size_t nIndex = pOldTargetBox->GetItemIndex(m_pDestControl);
        if (nIndex != Box::InvalidIndex) {
            if (pOldTargetBox->RemoveItem(m_pDestControl)) {
                m_pDestControl = nullptr;
            }
        }
    }

    UiPoint pt(ptMouse);
    pt.Offset(pTargetBox->GetScrollOffsetInScrollBox());
    const size_t nCount = pTargetBox->GetItemCount();
    for (size_t nIndex = 0; nIndex < nCount; ++nIndex) {
        Control* pControl = pTargetBox->GetItemAt(nIndex);
        if ((pControl == nullptr) || !pControl->IsVisible() || pControl->IsFloat()) {
            continue;
        }
        if ((pControl == m_pDestControl) && (IsPtInControlRect(pControl, pt))) {
            // The mouse is on the indicator control
            bDropped = true;
            break;
        }
        if (IsPtInControlRect(pControl, pt)) {
            bDropped = true;
            if (m_pDestControl == nullptr) {
                m_pDestControl = CreateDestControl(pTargetBox);
            }
            if (m_pDestControl == nullptr) {
                break;
            }
            size_t nOldIndex = pTargetBox->GetItemIndex(m_pDestControl);
            if (nOldIndex == Box::InvalidIndex) {
                pTargetBox->AddItem(m_pDestControl);
            }
            UiRect rc = pControl->GetRect();
            if (pTargetBox->GetLayout()->IsHLayout()) {
                size_t nNewIndex = nIndex;
                if ((pt.x - rc.left) > rc.Width() / 2) {
                    if (nNewIndex < (pTargetBox->GetItemCount() - 1)) {
                        nNewIndex += 1;
                    }
                    if (nOldIndex != nNewIndex) {
                        pTargetBox->SetItemIndex(m_pDestControl, nNewIndex);
                    }
                }
                else {
                    if (nOldIndex != (nNewIndex - 1)) {
                        pTargetBox->SetItemIndex(m_pDestControl, nNewIndex);
                    }
                }
            }
            else {
                size_t nNewIndex = nIndex;
                if ((pt.y - rc.top) > rc.Height() / 2) {
                    if (nNewIndex < (pTargetBox->GetItemCount() - 1)) {
                        nNewIndex += 1;
                    }
                    if (nOldIndex != nNewIndex) {
                        pTargetBox->SetItemIndex(m_pDestControl, nNewIndex);
                    }
                }
                else {
                    if (nOldIndex != (nNewIndex - 1)) {
                        pTargetBox->SetItemIndex(m_pDestControl, nNewIndex);
                    }
                }
            }
            break;
        }
    }
    if (!bDropped && pTargetBox->GetRect().ContainsPt(pt)) {
        bDropped = true;
        if (m_pDestControl == nullptr) {
            m_pDestControl = CreateDestControl(pTargetBox);
        }
        size_t nOldIndex = pTargetBox->GetItemIndex(m_pDestControl);
        if (nOldIndex == Box::InvalidIndex) {
            pTargetBox->AddItem(m_pDestControl);
        }
    }
    return bDropped;
}

template<typename T>
DragWindow* ControlDragableT<T>::CreateDragWindow()
{
    return new DragWindow;
}

template<typename T>
Control* ControlDragableT<T>::CreateDestControl(Box* pTargetBox)
{
    Control* pDestControl = new Control(this->GetWindow());
    pDestControl->SetAttribute(_T("bkcolor"), _T("#FF5D6B99"));
    pDestControl->SetAttribute(_T("valign"), _T("center"));
    pDestControl->SetAttribute(_T("halign"), _T("center"));

    Layout* pLayout = nullptr;
    if (pTargetBox != nullptr) {
        pLayout = pTargetBox->GetLayout();
    }
    bool bInited = false;
    if ((pLayout != nullptr) && pLayout->IsTileLayout()) {
        UiSize szItem;
        szItem.cx = this->GetFixedWidth().GetInt32();
        szItem.cy = this->GetFixedHeight().GetInt32();
        if ((szItem.cx > 0) && (szItem.cy > 0)) {
            pDestControl->SetAlpha(128);
            pDestControl->SetFixedHeight(UiFixedInt(szItem.cy), false, false);
            pDestControl->SetFixedWidth(UiFixedInt(szItem.cx), false, false);
            bInited = true;
        }
    }
    if (!bInited) {        
        if ((pLayout != nullptr) && pLayout->IsVLayout()) {
            pDestControl->SetAttribute(_T("height"), _T("4"));
            pDestControl->SetAttribute(_T("width"), _T("80%"));
        }
        else {
            pDestControl->SetAttribute(_T("width"), _T("4"));
            pDestControl->SetAttribute(_T("height"), _T("80%"));
        }
    }
    return pDestControl;
}

template<typename T>
std::shared_ptr<IBitmap> ControlDragableT<T>::CreateDragoutImage()
{
    UiRect rc = this->GetRect();
    if (rc.IsEmpty()) {
        return nullptr;
    }
    std::unique_ptr<IRender> render;
    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    ASSERT(pRenderFactory != nullptr);
    if (pRenderFactory != nullptr) {
        ASSERT(this->GetWindow() != nullptr);
        IRenderDpiPtr spRenderDpi;
        if (this->GetWindow() != nullptr) {
            spRenderDpi = this->GetWindow()->GetRenderDpi();
        }
        render.reset(pRenderFactory->CreateRender(spRenderDpi));
    }
    ASSERT(render != nullptr);
    if(render == nullptr) {
        return nullptr;
    }
    const int32_t kDragImageWidth = rc.Width();
    const int32_t kDragImageHeight = rc.Height();
    if (render->Resize(kDragImageWidth, kDragImageHeight)) {
        int32_t destWidth = 0;
        int32_t destHeight = 0;
        float scale = (float)rc.Width() / (float)rc.Height();
        if (scale >= 1.0) {
            destWidth = kDragImageWidth;
            destHeight = (int32_t)(kDragImageWidth * (float)rc.Height() / (float)rc.Width());
        }
        else {
            destHeight = kDragImageHeight;
            destWidth = (int32_t)(kDragImageHeight * (float)rc.Width() / (float)rc.Height());
        }
        render->AlphaBlend((kDragImageWidth - destWidth) / 2, 0, destWidth, destHeight,
            this->GetWindow()->GetRender(),
            rc.left, rc.top, rc.Width(), rc.Height());
    }
    std::shared_ptr<IBitmap> pDragImage(render->MakeImageSnapshot());
    return pDragImage;
}

template<typename T>
bool ControlDragableT<T>::IsPtInControlRect(Control* pControl, const UiPoint& pt) const
{
    if (pControl == nullptr) {
        return false;
    }
    UiRect rc = pControl->GetRect();
    if (rc.ContainsPt(pt)) {
        return true;
    }
    UiMargin rcMargin = pControl->GetMargin();
    rc.Inflate(rcMargin);
    if (rc.ContainsPt(pt)) {
        return true;
    }
    Layout* pLayout = nullptr;
    const Box* pBox = pControl->GetParent();
    if (pBox != nullptr) {
        pLayout = pBox->GetLayout();
    }
    if (pLayout != nullptr) {
        int32_t nChildMarginX = pLayout->GetChildMarginX() / 2;
        int32_t nChildMarginY = pLayout->GetChildMarginY() / 2;
        if ((nChildMarginX > 0) || (nChildMarginY > 0)) {
            rc = pControl->GetRect();
            rc.left -= nChildMarginX;
            rc.right += nChildMarginX;
            rc.top -= nChildMarginY;
            rc.bottom += nChildMarginY;
            if (rc.ContainsPt(pt)) {
                return true;
            }
        }
    }
    return false;
}

template<typename T>
void ControlDragableT<T>::DragOrderMouseDown(const EventArgs& /*msg*/)
{
    m_rcItemList.clear();
    if (!IsEnableDragOrder()) {
        return;
    }
    const Box* pParent = this->GetParent();
    if (pParent == nullptr) {
        return;
    }
    // Order adjustment within the control: record the original position information of each control
    size_t nItemCount = pParent->GetItemCount();
    for (size_t index = 0; index < nItemCount; ++index) {
        ItemStatus itemStatus;
        itemStatus.m_index = index;
        itemStatus.m_pItem = pParent->GetItemAt(index);
        if (itemStatus.m_pItem != nullptr) {
            itemStatus.m_rcPos = itemStatus.m_pItem->GetRect();
            itemStatus.m_nPaintOrder = itemStatus.m_pItem->GetPaintOrder();
            m_rcItemList.push_back(itemStatus);
        }
        if (itemStatus.m_pItem == this) {
            // The current control's paint order needs to be last to avoid being obscured by other controls
            itemStatus.m_pItem->SetPaintOrder(255);
            ASSERT(itemStatus.m_rcPos.ContainsPt(m_ptMouseDown));
        }
    }
}

template<typename T>
bool ControlDragableT<T>::DragOrderMouseMove(const EventArgs& msg)
{
    bool bRet = false;
    if (!IsEnableDragOrder()) {
        // The control is forbidden to drag-to-reorder
        return bRet;
    }
    const Box* pParent = this->GetParent();
    if (pParent == nullptr) {
        return bRet;
    }
    const Layout* pLayout = pParent->GetLayout();
    if ((pLayout == nullptr) || pLayout->IsTileLayout() || (!pLayout->IsHLayout() && !pLayout->IsVLayout())) {
        return bRet;
    }
    UiPoint pt(msg.ptMouse);
    pt.Offset(this->GetScrollOffsetInScrollBox());
    UiPoint ptNow = pt;
    UiRect itemsValidRect;
    if (GetItemsValidRect(itemsValidRect)) {
        if (pt.x < itemsValidRect.left) {
            pt.x = itemsValidRect.left;
        }
        if (pt.x >= itemsValidRect.right) {
            pt.x = itemsValidRect.right - 1;
        }
        if (pt.y < itemsValidRect.top) {
            pt.y = itemsValidRect.top;
        }
        if (pt.y >= itemsValidRect.bottom) {
            pt.y = itemsValidRect.bottom - 1;
        }
    }
    if (pLayout->IsHLayout()) {
        int32_t xOffset = pt.x - m_ptMouseDown.x;
        if (std::abs(xOffset) < this->Dpi().GetScaleInt(DRAG_OPERATION_MIN_PT)) {
            return bRet;
        }
        // Adjust the positions of the other controls
        AdjustItemPos(pt, m_ptMouseDown, m_rcItemList);

        UiRect rect = m_rcMouseDown;
        rect.left += xOffset;
        rect.right += xOffset;

        this->SetPos(rect);

        ScrollBox* pScrollBox = dynamic_cast<ScrollBox*>(this->GetParent());
        if ((pScrollBox != nullptr) && (pScrollBox->GetScrollRange().cx > 0)) {
            if (ptNow.x <= itemsValidRect.left) {
                // Scroll to the left
                pScrollBox->LineLeft();
            }
            else if (ptNow.x >= itemsValidRect.right) {
                // Scroll to the right
                pScrollBox->LineRight();
            }
        }
    }
    else {
        int32_t yOffset = pt.y - m_ptMouseDown.y;
        if (std::abs(yOffset) < this->Dpi().GetScaleInt(DRAG_OPERATION_MIN_PT)) {
            return bRet;
        }
        // Adjust the positions of the other controls
        AdjustItemPos(pt, m_ptMouseDown, m_rcItemList);

        UiRect rect = m_rcMouseDown;
        rect.top += yOffset;
        rect.bottom += yOffset;
        this->SetPos(rect);

        ScrollBox* pScrollBox = dynamic_cast<ScrollBox*>(this->GetParent());
        if ((pScrollBox != nullptr) && (pScrollBox->GetScrollRange().cy > 0)) {
            if (ptNow.y <= itemsValidRect.top) {
                // Scroll up
                pScrollBox->LineUp();
            }
            else if (ptNow.y >= itemsValidRect.bottom) {
                // Scroll down
                pScrollBox->LineDown();
            }
        }
    }

    if (!m_bInDraggingOrder) {
        bRet = true;
        m_bInDraggingOrder = true;
        m_nOldAlpha = this->GetAlpha();
        // Set the translucent effect
        this->SetAlpha(m_nDragAlpha);
    }
    return bRet;
}

template<typename T>
bool ControlDragableT<T>::DragOutMouseMove(const EventArgs& msg)
{
    bool bRet = false;
    if (!IsEnableDragOut()) {
        // The control is forbidden to be dragged out
        return bRet;
    }
    const Window* pWindow = this->GetWindow();
    const Box* pParent = this->GetParent();
    if ((pParent == nullptr) || (pWindow == nullptr)) {
        return bRet;
    }
    if (pParent->GetDragOutId() == 0) {
        // The parent container does not support the drag-out operation
        return bRet;
    }
    const Layout* pLayout = pParent->GetLayout();
    if ((pLayout == nullptr) || (!pLayout->IsHLayout() && !pLayout->IsVLayout())) {
        return bRet;
    }
    UiPoint pt(msg.ptMouse);
    pt.Offset(this->GetScrollOffsetInScrollBox());
    if (!IsInDraggingOut()) {
        int32_t xOffset = pt.x - m_ptMouseDown.x;
        int32_t yOffset = pt.y - m_ptMouseDown.y;
        int32_t nMinOffset = this->Dpi().GetScaleInt(DRAG_OPERATION_MIN_PT);
        if ((std::abs(xOffset) < nMinOffset) && (std::abs(yOffset) < nMinOffset)) {
            return bRet;
        }
    }

    std::shared_ptr<IBitmap> pDragImage = m_pDragImage;
    if (pDragImage == nullptr) {
        pDragImage = CreateDragoutImage();
        m_pDragImage = pDragImage;
    }

    bool bDropped = false;
    Box* pOldTargetBox = m_pTargetBox;
    m_pTargetBox = pWindow->FindDroppableBox(msg.ptMouse, pParent->GetDragOutId());
    if (m_pTargetBox != nullptr) {
        bDropped = OnDragOutMouseOver(m_pTargetBox, pOldTargetBox, msg.ptMouse);
    }
    if (!bDropped) {
        if ((m_pDestControl != nullptr) && (m_pTargetBox != nullptr)) {
            if (m_pTargetBox->RemoveItem(m_pDestControl)) {
                m_pDestControl = nullptr;
            }
        }
        if ((m_pDestControl != nullptr) && (pOldTargetBox != nullptr)) {
            if (pOldTargetBox->RemoveItem(m_pDestControl)) {
                m_pDestControl = nullptr;
            }
        }
        if (m_pDestControl != nullptr) {
            delete m_pDestControl;
            m_pDestControl = nullptr;
        }
    }
    if (!bDropped) {
        m_pTargetBox = nullptr;
    }

    // Drag out of the parent container
    if ((m_pDragWindow == nullptr) || m_pDragWindow->IsClosingWnd()) {
        if (m_pDragWindow != nullptr) {
#ifdef DUILIB_BUILD_FOR_SDL
            if (m_pDragWindowFilter != nullptr) {
                m_pDragWindow->RemoveMessageFilter(m_pDragWindowFilter.get());
            }
#endif
            m_pDragWindow->Release();
            m_pDragWindow = nullptr;

#ifdef DUILIB_BUILD_FOR_SDL
            m_pDragWindowFilter.reset();
#endif
        }
        m_pDragWindow = CreateDragWindow();
        ASSERT(m_pDragWindow != nullptr);
        if (m_pDragWindow != nullptr) {
            m_pDragWindow->AddRef();

            WindowCreateParam createWndParam;
            createWndParam.m_dwStyle = kWS_POPUP;
            createWndParam.m_dwExStyle = kWS_EX_LAYERED | kWS_EX_NOACTIVATE | kWS_EX_TRANSPARENT;
            m_pDragWindow->CreateWnd(this->GetWindow(), createWndParam);
            ASSERT(m_pDragWindow->IsWindow());
            if (m_pDragWindow->IsWindow()) {
                m_pDragWindow->AddRef();
            }
            m_pDragWindow->SetDragImage(pDragImage);
            m_pDragWindow->ShowWindow(kSW_SHOW_NA);

#ifdef DUILIB_BUILD_FOR_SDL
            m_pDragWindowFilter = std::make_unique<DragWindowFilter>(this->GetWindow(), m_pDragWindow);
            m_pDragWindow->AddMessageFilter(m_pDragWindowFilter.get());
            // Alleviate the black screen phenomenon during display
            m_pDragWindow->InvalidateAll();
#endif
        }
    }
    if (m_pDragWindow != nullptr) {
        m_pDragWindow->AdjustPos();
    }
    if (this->IsVisible()) {
        this->SetVisible(false);
    }    
    bRet = true;
    return bRet;
}

typedef ControlDragableT<Control> ControlDragable;
typedef ControlDragableT<Box> BoxDragable;
typedef ControlDragableT<HBox> HBoxDragable;
typedef ControlDragableT<VBox> VBoxDragable;

}

#endif // UI_CORE_CONTROL_DRAGABLE_H_
