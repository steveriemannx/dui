#ifndef UI_CORE_PLACE_HOLDER_H_
#define UI_CORE_PLACE_HOLDER_H_

#include "duilib/Core/Callback.h"
#include "duilib/Core/UiTypes.h"

namespace ui 
{
    class Box;
    class Window;
    class DpiManager;

/** Basic wrapper for the position, size, and appearance of a control
*/
class DUILIB_API PlaceHolder : public virtual SupportWeakCallback
{
public:
    explicit PlaceHolder(Window* pWindow);
    PlaceHolder(const PlaceHolder& r) = delete;
    PlaceHolder& operator=(const PlaceHolder& r) = delete;
    virtual ~PlaceHolder() override;

    /** Control type
    */
    virtual DString GetType() const;

    /** Get the control name, corresponding to the name attribute in xml
     */
    DString GetName() const;

    /** Get the control name, corresponding to the name attribute in xml
     * @return Returns the control name (UTF8 encoded)
     */
    std::string GetUTF8Name() const;

    /** Set the control name; setting it in memory will not write it to the xml
     * @param [in] strName The name to set
     */
    void SetName(const DString& strName);

    /** Set the control name; setting it in memory will not write it to the xml (UTF8 encoded)
     * @param [in] strName The name to set
     */
    void SetUTF8Name(const std::string& strName);

    /** Determine whether the control names are equal
    */
    bool IsNameEquals(const DString& name) const;

    /** Determine whether it has a name
    */
    bool HasName() const;

    /** Get the ancestor container pointer by name
    * @param [in] strName The name of the ancestor container to get
    */
    Box* GetAncestor(const DString& strName);

    /** Set the container's owning window
     * @param [in] pParent The parent container pointer
     */
    virtual void SetParent(Box* pParent);

    /** Set the container's owning window
     * @param [in] pWindow The window pointer
     */
    virtual void SetWindow(Window* pWindow);

    /** Get the parent container pointer
    */
    Box* GetParent() const { return m_pParent; }

    /** Get the associated window pointer
     * @return Returns the pointer to the associated window
     */
    Window* GetWindow() const { return m_pWindow; }

    /** Initialization function (this Init function is called when the control is added to a parent control)
     */
    virtual void Init();

    /** Whether it has already been initialized
    */
    bool IsInited() const;

public:
    /** Set whether the control is visible
     */
    void SetVisible(bool bVisible);

    /** Determine whether it is visible
     */
    bool IsVisible() const;

    /** Set whether ancestor controls are visible
    */
    void SetAncestorVisible(bool bAncestorVisible);

    /**@ Determine whether the ancestors are visible
     */
    bool IsAncestorVisible() const;

    /** Set the enabled state of the control
     * @param [in] bEnable When true, the control is enabled; when false, the control is disabled and unavailable
     */
    void SetEnabled(bool bEnable);

    /** Check whether the control is enabled
     * @return The enabled state of the control; returns true if enabled, otherwise false
     */
    bool IsEnabled() const;

    /** Set the enabled state of ancestor controls
     * @param [in] bEnable When true, the control is enabled; when false, the control is disabled and unavailable
     */
    void SetAncestorEnabled(bool bAncestorEnable);

    /** Check whether ancestor controls are enabled
     * @return The enabled state of the control; returns true if enabled, otherwise false
     */
    bool IsAncestorEnabled() const;

    /** Set whether the control responds to mouse events
     * @param [in] bEnable When true, responds to mouse events; when false, does not respond to mouse events
     */
    void SetMouseEnabled(bool bEnable);

    /** Check whether the control responds to mouse events
     * @return Returns whether the control responds to mouse events; true responds to mouse events, false does not
     */
    bool IsMouseEnabled() const { return m_bMouseEnabled; }

    /** Set whether the control responds to keyboard events
     * @param [in] bEnable When true, responds to keyboard events; when false, does not respond to keyboard events
     */
    void SetKeyboardEnabled(bool bEnable);

    /** Check whether the control responds to keyboard events
     * @return Returns whether the control responds to keyboard events; true responds to keyboard events, false does not respond to keyboard events
     */
    bool IsKeyboardEnabled() const { return m_bKeyboardEnabled; }

    /** Set whether the control is floating
     * @param [in] bFloat Set to true to float, false to not float
     */
    void SetFloat(bool bFloat);

    /** Determine whether the control is in a floating state, corresponding to the float attribute in xml
     */
    bool IsFloat() const { return m_bFloat; }

    /** Get the relative position to the parent control (only valid when the control is a floating control)
    * @return If the returned cx and cy are both INT32_MIN, the value is invalid; other values are valid
    */
    UiSize GetFloatPos() const;

    /** Set whether to keep the floating control's position relative to the parent control unchanged when the parent control's position and size are adjusted
    */
    void SetKeepFloatPos(bool bKeepFloatPos);

    /** Get whether the floating control's position relative to the parent control is kept unchanged when the parent control's position and size are adjusted
    */
    bool IsKeepFloatPos() const;

public:
    /** Get the width and height set for the control; both include padding but not margin
    */
    const UiFixedSize& GetFixedSize() const;

    /** Get the set width (includes padding, excludes margin), corresponding to the width attribute in xml; if not set, the default is stretch
     */
    const UiFixedInt& GetFixedWidth() const;

    /** Get the fixed height (includes padding, excludes margin), corresponding to the height attribute in xml; if not set, the default is stretch
     */
    const UiFixedInt& GetFixedHeight() const;

    /** Set the width of the control
     * @param [in] cx The width to set (includes padding, excludes margin)
     * @param [in] bArrange Whether to re-arrange, defaults to true
     * @param [in] bNeedDpiScale Whether DPI scaling applies, defaults to true
     */
    void SetFixedWidth(UiFixedInt cx, bool bArrange, bool bNeedDpiScale);

    /** Set the height of the control
     * @param [in] cy The fixed height to set (includes padding, excludes margin)
     * @param [in] bArrange Whether to re-arrange, defaults to true
     * @param [in] bNeedDpiScale Whether DPI scaling applies, defaults to true
     */
    void SetFixedHeight(UiFixedInt cy, bool bArrange, bool bNeedDpiScale);

public:
    /** Determine whether the size needs to be re-evaluated
    * @param [in] szAvailable The size of the area rectangle when estimating
    */
    bool IsReEstimateSize(const UiSize& szAvailable) const;

    /** Set whether the size needs to be re-evaluated
    * @param [in] bReEstimateSize Whether the size needs to be re-estimated
    */
    void SetReEstimateSize(bool bReEstimateSize);

    /** Get the estimated size of the control (length and width), equivalent to the cached value after the EstimateSize function estimation
    */
    UiEstSize GetEstimateSize() const;

    /** Set the estimated size of the control (length and width), equivalent to the cached value after the EstimateSize function estimation
    *@param [in] szEstimateSize The estimation result, saved as a cache
    *@param [in] szAvailable The size of the area rectangle when estimating
    */
    void SetEstimateSize(const UiEstSize& szEstimateSize, const UiSize& szAvailable);

public:
    /** Get the minimum width
    * @return Returns the minimum width (includes padding, excludes margin) (return value >= 0)
    */
    int32_t GetMinWidth() const;

    /**@brief Set the minimum width
     * @param [in] cx The minimum width to set (includes padding, excludes margin) (value >= 0)
     * @param [in] bNeedDpiScale Whether DPI adaptation is needed
     */
    void SetMinWidth(int32_t cx, bool bNeedDpiScale);

    /** Get the maximum width
    * @return Returns the maximum width (includes padding, excludes margin) (return value >= 0)
    */
    int32_t GetMaxWidth() const;

    /** Set the maximum width
     * @param [in] cx The maximum width to set (includes padding, excludes margin) (value >= 0)
     * @param [in] bNeedDpiScale Whether DPI adaptation is needed
     */
    void SetMaxWidth(int32_t cx, bool bNeedDpiScale);

    /** Get the minimum height
    * @return Returns the minimum height (includes padding, excludes margin) (return value >= 0)
    */
    int32_t GetMinHeight() const;

    /** Set the minimum height
     * @param [in] cy The minimum height to set (includes padding, excludes margin) (value >= 0)
     * @param [in] bNeedDpiScale Whether DPI adaptation is needed
     */
    void SetMinHeight(int32_t cy, bool bNeedDpiScale);

    /** Get the maximum height
    * @return Returns the maximum height (includes padding, excludes margin) (return value >= 0)
    */
    int32_t GetMaxHeight() const;

    /** Set the maximum height
     * @param[in] cy The maximum height to set (includes padding, excludes margin) (value >= 0)
     * @param [in] bNeedDpiScale Whether DPI adaptation is needed
     */
    void SetMaxHeight(int32_t cy, bool bNeedDpiScale);

    /** Get the actual width (includes padding, excludes margin)
     */
    int32_t GetWidth() const { return m_uiRect.Width(); }

    /** Get the actual height (includes padding, excludes margin)
     */
    int32_t GetHeight() const { return m_uiRect.Height(); }

    /** Get the horizontal alignment type, see the HorAlignType enum
     */
    HorAlignType GetHorAlignType() const;

    /** Set the horizontal alignment type
     * @param [in] horAlignType The alignment type to set, see the HorAlignType enum
     */
    void SetHorAlignType(HorAlignType horAlignType);

    /** Get the vertical alignment type, see the VerAlignType enum
     */
    VerAlignType GetVerAlignType() const;

    /** Set the vertical alignment type
     * @param [in] vorAlignType The alignment type to set, see the VerAlignType enum
     */
    void SetVerAlignType(VerAlignType verAlignType);

    /** Get the margin of the control
     */
    UiMargin GetMargin() const;

    /** Set the margin of the control
     * @param [in] rcMargin The margin information of the control
     * @param [in] bNeedDpiScale Whether the margin adapts to DPI; false means no DPI adaptation
     */
    void SetMargin(UiMargin rcMargin, bool bNeedDpiScale);

    /** Get the padding
     * @return Returns the padding on all four sides (the four Rect parameters represent the padding of each side)
     */
    UiPadding GetPadding() const;

    /** Set the padding
     * @param [in] rcPadding The padding data
     * @param [in] bNeedDpiScale Whether to adapt to DPI
     */
    void SetPadding(UiPadding rcPadding, bool bNeedDpiScale);

    /** Whether the control itself is allowed to apply padding
    * @param [in] bEnable true means the control itself also applies padding (Box and Control);
                          false means the control itself does not apply padding; padding is only used to lay out child controls (in the case of Box)
    */
    void SetEnableControlPadding(bool bEnable);

    /** Determine whether the control itself is allowed to apply padding
    * @return true means the control itself also applies padding (Box and Control);
              false means the control itself does not apply padding; padding is only used to lay out child controls (in the case of Box)
    */
    bool IsEnableControlPadding() const;

    /** Get the control's own padding (returns empty if padding is disabled for the control)
    * @return Returns the padding on all four sides (the four Rect parameters represent the padding of each side)
    */
    UiPadding GetControlPadding() const;

    /** Get the control position (subclasses can change the behavior)
    * @return Returns the rectangular area of the control, including padding and excluding margin
     */
    virtual UiRect GetPos() const { return m_uiRect; }

    /** Set the control position (subclasses can change the behavior)
     * @param [in] rc The rectangular area to set, including padding and excluding margin
     */
    virtual void SetPos(UiRect rc);

    /** Get the control rectangle (simply returns the m_uiRect value), including padding and excluding margin
    */
    const UiRect& GetRect() const { return m_uiRect; }

    /** Set the control rectangle (simply sets the m_uiRect value), including padding and excluding margin
    */
    void SetRect(const UiRect& rc);

    /** Set the cell merge attribute (how many rows it spans), only effective in GridLayout layout
    */
    void SetRowSpan(int32_t rowSpan);

    /** Get the cell merge attribute (how many rows it spans), only effective in GridLayout layout
    * @return Return value >= 1
    */
    int32_t GetRowSpan() const;

    /** Set the cell merge attribute (how many columns it spans), only effective in GridLayout layout
    */
    void SetColumnSpan(int32_t colSpan);

    /** Get the cell merge attribute (how many columns it spans), only effective in GridLayout layout
    * @return Return value >= 1
    */
    int32_t GetColumnSpan() const;

    /** Redraw the control
    */
    virtual void Invalidate();

    /** Redraw part of the control's area
    * @param [in] rc The area that needs to be redrawn
    */
    virtual void InvalidateRect(const UiRect& rc);

    /** Get the drawing expansion area of this control including the box-shadow
    * @return Returns the total area after expanding rc + box-shadow; returns rc if there is no box-shadow
    */
    virtual UiRect GetBoxShadowExpandedRect(const UiRect& rc) const;

    /** Re-arrange the control layout
     */
    virtual void Arrange();

    /** Have the parent container re-arrange its layout
     */
    virtual void ArrangeAncestor();

    /** Based on the control attributes, determine whether the layout needs re-arrangement or only a redraw
    */
    void RelayoutOrRedraw();

    /** Determine whether it has been arranged
     */
    bool IsArranged() const { return m_bIsArranged; }

    /** Set whether it has been arranged
     */
    void SetArranged(bool bArranged);

    /** Set whether to clip the drawing range
    * @param [in] clip Set to true if clipping is needed, otherwise not needed; see the drawing functions
    */
    void SetClip(bool bClip) { m_bClip = bClip; }

    /** Determine whether the drawing range is clipped
    * @return Returns true if needed, false if not needed
    */
    bool IsClip() const { return m_bClip; }

    /** Get the outer scroll offset
     */
    UiPoint GetScrollOffsetInScrollBox() const;

    /** Determine whether two controls are equal or have a parent-child or descendant relationship
     * @param [in] pAncestor A control at the ancestor level
     * @param [in] pChild A control at the descendant level
     * @return Returns true if the two controls are equal or have a parent-child or descendant relationship, otherwise false
     */
    static bool IsControlRelated(const PlaceHolder* pAncestor, const PlaceHolder* pChild);

    /** Get the DPI manager corresponding to this window
    */
    const DpiManager& Dpi() const;

protected:
    /** Visibility state (for internal subclasses to override; returns true if visible, false if not visible)
    */
    virtual bool IsVisibleInternal() const { return true; }

    /** Set the visibility state event
    * @param [in] bChanged true means the state has changed, false means the state has not changed
    */
    virtual void OnSetVisible(bool bChanged) { UNUSED_VARIABLE(bChanged); }

    /** Set the enabled state event
    * @param [in] bChanged true means the state has changed, false means the state has not changed
    */
    virtual void OnSetEnabled(bool bChanged) { UNUSED_VARIABLE(bChanged); }

    /** Set the mouse-enabled state event
    * @param [in] bChanged true means the state has changed, false means the state has not changed
    */
    virtual void OnSetMouseEnabled(bool bChanged) { UNUSED_VARIABLE(bChanged); }

    /** Set the keyboard-enabled state
    * @param [in] bChanged true means the state has changed, false means the state has not changed
    */
    virtual void OnSetKeyboardEnabled(bool bChanged) { UNUSED_VARIABLE(bChanged); }

protected:
    /** Re-arrange itself
    */
    virtual void ArrangeSelf();

    /** Execute the initialization event (when each control is initialized, this function is called, and only once)
     *  When this function runs, the value of IsInited() is false; if IsInited() is true, it means the OnInit() function was executed again.
     */
    virtual void OnInit();

private:
    /** Ensure the infrequently used data is created
    */
    void CheckPlaceHolderData();

private:
    /** Infrequently used data
    */
    struct TPlaceHolderData
    {
        //Constructor
        TPlaceHolderData();

        //The relative position to the parent control (only valid when the control is a floating control)
        UiSize m_uiFloatPos;

        //The minimum control size
        UiSize m_cxyMin;

        //The maximum control size
        UiSize m_cxyMax;

        //Cell merge attribute (default spans 1 row), only effective in GridLayout layout
        int16_t m_rowSpan;

        //Cell merge attribute (default spans 1 column), only effective in GridLayout layout
        int16_t m_colSpan;

        //Keep the floating control's position relative to the parent control unchanged when the parent control's position and size are adjusted
        bool m_bKeepFloatPos;
    };

private:
    //Control name, used for operations such as finding controls
    UiString m_sName;

    //The associated window object
    Window* m_pWindow;

    //The parent control object
    Box* m_pParent;

    //The position and size of the control
    UiRect m_uiRect;

    //The control size set externally
    UiFixedSize m_cxyFixed;

    //The result of estimating the control size (only used when the width or height of the control is auto)
    std::unique_ptr<UiEstResult> m_pEstResult;

    //Infrequently used data
    std::unique_ptr<TPlaceHolderData> m_pData;

    //The margin attribute of the control (top, bottom, left, right margins); the margin is the space outside m_uiRect and is not included in m_uiRect
    UiMargin16 m_rcMargin;

    //The padding on all four sides (top, bottom, left, right); the padding is the space inside the control rectangle and is included within it
    UiPadding16 m_rcPadding;

    //The horizontal alignment type of the control (HorAlignType)
    HorAlignType m_horAlignType;

    //The vertical alignment type of the control (VerAlignType)
    VerAlignType m_verAlignType;

    //Whether the size needs to be re-evaluated
    bool m_bReEstimateSize;

    //Whether the control itself is allowed to set padding
    //(Original logic: Control itself has no padding, while Box's Layout has padding, so the background image of the Box itself does not apply padding; only child controls apply padding)
    //This switch defaults to true; the option to turn it off is provided for compatibility with the original logic. For example, the shadow implementation must not enable padding, otherwise the shadow rendering will be abnormal
    bool m_bEnableControlPadding;

    //Whether the control is a floating control
    bool m_bFloat;

    //Whether the layout needs re-arrangement
    bool m_bIsArranged;

    //Whether to clip the drawing range
    bool m_bClip;

    //Whether it is visible
    bool m_bVisible;

    //Whether the ancestor container is visible
    bool m_bAncestorVisible;

    //The Enable state of the control (when false, it does not respond to input messages such as mouse and keyboard)
    bool m_bEnabled;

    //The Enable state of the ancestor container
    bool m_bAncestorEnabled;

    //The Enable state for mouse messages (when false, mouse messages are not responded to)
    bool m_bMouseEnabled;

    //The Enable state for keyboard messages (when false, keyboard messages are not responded to)
    bool m_bKeyboardEnabled;

    //Whether initialization has been completed
    bool m_bInited;
};

} // namespace ui

#endif // UI_CORE_PLACE_HOLDER_H_
