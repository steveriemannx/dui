#ifndef UI_CORE_BOX_H_
#define UI_CORE_BOX_H_

#include "duilib/Layout/Layout.h"
#include "duilib/Core/Control.h"

namespace ui 
{
/** Container base class
*/
class DUILIB_API Box : public Control
{
    typedef Control BaseClass;
public:
    explicit Box(Window* pWindow, Layout* pLayout = new Layout());
    Box(const Box& r) = delete;
    Box& operator=(const Box& r) = delete;
    virtual ~Box() override;

public:
    /** Invalid item index
    */
    static constexpr auto InvalidIndex{ static_cast<size_t>(-1) };

    /** @brief Whether it is a valid child control index
    */
    static bool IsValidItemIndex(size_t index) { return index != Box::InvalidIndex; }

public:
    /// Override the parent class interface to provide customized functionality. For details of the methods, see the Control control
    virtual DString GetType() const override;
    virtual void SetParent(Box* pParent) override;
    virtual void SetWindow(Window* pWindow) override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;
    virtual void PaintChild(IRender* pRender, const UiRect& rcPaint) override;
    virtual void PaintFocusRect(IRender* pRender) override;
    virtual Control* FindControl(FINDCONTROLPROC Proc, void* pProcData, uint32_t uFlags,
                                 const UiPoint& ptMouse = UiPoint(),
                                 const UiPoint& scrollPos = UiPoint()) override;
    virtual void ClearImageCache() override;
    virtual uint32_t GetControlFlags() const override;

    /** Set the control position (subclass can change the behavior)
     * @param [in] rc The rectangle information to set, including the inner padding, not including the outer margin
     */
    virtual void SetPos(UiRect rc) override;

    /** Get the actual usable rectangle of the control
    *@return The actual usable area of the control, i.e. the area after subtracting Padding from GetPos
    */
    virtual UiRect GetPosWithoutPadding() const;

    /** Get the actual usable rectangle of the control
    *@return The actual usable area of the control, i.e. the area after subtracting Padding from GetRect
    */
    UiRect GetRectWithoutPadding() const;

    /** Calculate the control size (width and height)
        If an image is set and either width or height is set to auto, the final size will be calculated based on the image size and the text size
     *  @param [in] szAvailable The available size, not including the inner padding and the outer margin
     *  @return The estimated size of the control, including the inner padding (Box), not including the outer margin
     */
    virtual UiEstSize EstimateSize(UiSize szAvailable) override;

    /** DPI changed, update the control size and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] nNewDpiScale The new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

    /** Language changed, refresh the UI text display related content
    */
    virtual void OnLanguageChanged() override;

public:
    /** @name Methods related to operating child controls (items)
    * @{
    */
    /** Get the number of child controls
     */
    virtual size_t GetItemCount() const;

    /** Find the specified control by index
     * @param[in] iIndex The control index
     */
    virtual Control* GetItemAt(size_t iIndex) const;

    /** Get the index by the control pointer
     * @param[in] pControl The control pointer
     * @return The index of the control pointed to by pControl, or Box::InvalidIndex if not found
     */
    virtual size_t GetItemIndex(Control* pControl) const;

    /** Set the control index (the control positions will be rearranged internally)
     * @param[in] pControl The control pointer
     * @param[in] iIndex The index value to set
     * @return Returns true on success, false on failure
     */
    virtual bool SetItemIndex(Control* pControl, size_t iIndex);

    /** Add a control to the container
     * @param[in] pControl The control pointer
     * @return Returns true if added successfully, false if failed
     */
    virtual bool AddItem(Control* pControl);

    /** Add a control at the specified position
     * @param[in] pControl The control pointer
     * @param[in] iIndex Insert the control after this index
     * @return Returns true if added successfully, false if failed
     */
    virtual bool AddItemAt(Control* pControl, size_t iIndex);

    /** Remove a control from the container by control pointer
     * @param[in] pControl The pointer of the control
     * @return Returns true if removed successfully, false if failed (the control may not exist)
     */
    virtual bool RemoveItem(Control* pControl);

    /** Remove a control from the container by control index
     * @param[in] iIndex The index of the control to remove
     * @return Returns true if removed successfully, false if failed (the index is too small or exceeds the total number of child controls)
     */
    virtual bool RemoveItemAt(size_t iIndex);

    /** Remove all child controls
     */
    virtual void RemoveAllItems();

    /** @} */

public:
    /** Find the index of the next selectable control (for list, combo)
     * @param[in] iIndex The index to start searching from
     * @param[in] bForward true to search incrementally, false to search decrementally
     * @return The index of the next selectable control; Box::InvalidIndex means no selectable control
     */
    size_t FindSelectable(size_t iIndex, bool bForward = true) const;

    /**
     * @brief Find the specified child control
     * @param[in] pstrSubControlName The child control name
     * @return Returns the child control pointer
     */
    Control* FindSubControl(const DString& pstrSubControlName);

    /**
     * @brief Whether to destroy automatically
     * @return true to destroy automatically, false not to
     */
    bool IsAutoDestroyChild() const { return m_bAutoDestroyChild; }

    /**
     * @brief Set whether the control is destroyed automatically
     * @param[in] bAuto true to destroy automatically, false not to
     * @return none
     */
    void SetAutoDestroyChild(bool bAuto) { m_bAutoDestroyChild = bAuto; }

    /**
     * @brief Get whether the container responds to mouse operations
     * @return true to respond, false not to
     */
    bool IsMouseChildEnabled() const { return m_bMouseChildEnabled; }

    /**
     * @brief Set whether the container responds to mouse operations
     * @param[in] bEnable Set to true to respond to mouse operations, set to false not to; the default is true
     */
    void SetMouseChildEnabled(bool bEnable) { m_bMouseChildEnabled = bEnable; }

    /**
     * @brief Get the layout object pointer of the container
     * @return Returns the layout object pointer associated with the container
     */
    Layout* GetLayout() const { return m_pLayout; }

    /** Re-associate the layout object, and return the previously associated layout object
     * @param [in] pNewLayout The layout object pointer
     * @return Returns the old layout object; if no longer used, it needs to be released with FreeLayout
     */
    Layout* ResetLayout(Layout* pNewLayout);

    /** Release the layout object
    * @param [in] pLayout The layout object pointer
    */
    void FreeLayout(Layout* pLayout);

public:
    /** Set whether dragging and dropping into this container is supported: if not equal to 0, dropping in is supported, otherwise not (drag in from a container with DragOutId==DropInId)
    */
    void SetDropInId(uint8_t nDropInId);

    /** Get whether dragging and dropping into this container is supported: if not equal to 0, dropping in is supported, otherwise not
    */
    uint8_t GetDropInId() const;

    /** Set whether dragging out of this container is supported: if not equal to 0, dragging out is supported, otherwise not (drag out to a container with DropInId==DragOutId)
    */
    void SetDragOutId(uint8_t nDragOutId);

    /** Get whether dragging out of this container is supported: if not equal to 0, dragging out is supported, otherwise not
    */
    uint8_t GetDragOutId() const;

protected:

    /** Find a control, the child control list is passed in externally
    */
    Control* FindControlInItems(const std::vector<Control*>& items, 
                                FINDCONTROLPROC Proc, void* pProcData,
                                uint32_t uFlags, 
                                const UiPoint& ptMouse, 
                                const UiPoint& scrollPos);

protected:
    /** Set visible state event
    * @param [in] bChanged true means the state changed, false means the state did not change
    */
    virtual void OnSetVisible(bool bChanged) override;

    /** Set enabled state event
    * @param [in] bChanged true means the state changed, false means the state did not change
    */
    virtual void OnSetEnabled(bool bChanged) override;

private:
    /**@brief Add a control at the specified position
     * @param[in] pControl The control pointer
     * @param[in] iIndex Insert the control after this index
     */
    bool DoAddItemAt(Control* pControl, size_t iIndex);

    /**@brief Remove a control from the container by control pointer
     * @param[in] pControl The pointer of the control
     */
    bool DoRemoveItem(Control* pControl);

protected:

    // List of child controls in the container
    std::vector<Control*> m_items;

private:
    // Layout management interface
    Layout* m_pLayout;

    // Whether to automatically delete the item objects (if true: the object will be deleted when removed from m_items; if false, it will not be deleted)
    bool m_bAutoDestroyChild;

    // Whether to allow responding to mouse messages of child controls
    bool m_bMouseChildEnabled;

    // Whether dragging and dropping into this container is supported: if not equal to 0, dropping in is supported, otherwise not (drag in from a container with DragOutId==DropInId)
    uint8_t m_nDropInId;

    // Whether dragging out of this container is supported: if not equal to 0, dragging out is supported, otherwise not (drag out to a container with DropInId==DragOutId)
    uint8_t m_nDragOutId;
};

} // namespace ui

#endif // UI_CORE_BOX_H_
