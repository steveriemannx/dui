#ifndef UI_BOX_SCROLLBOX_H_
#define UI_BOX_SCROLLBOX_H_

#include "dui/Layout/HLayout.h"
#include "dui/Layout/VLayout.h"
#include "dui/Layout/HFlowLayout.h"
#include "dui/Layout/VFlowLayout.h"
#include "dui/Layout/HTileLayout.h"
#include "dui/Layout/VTileLayout.h"
#include "dui/Core/ScrollBar.h"
#include "dui/Core/Box.h"

namespace ui 
{
 /** A container with a vertical or horizontal scroll bar, so that the container can hold more content
 *   By changing the layout, six subclasses are formed: HScrollBox/VScrollBox/HFlowScrollBox/VFlowScrollBox/HTileScrollBox/VTileScrollBox
 */
class DUI_API ScrollBox : public Box
{
    typedef Box BaseClass;
public:
    explicit ScrollBox(Window* pWindow, Layout* pLayout = new Layout);
    ScrollBox(const ScrollBox& r) = delete;
    ScrollBox& operator=(const ScrollBox& r) = delete;
    virtual ~ScrollBox() override;

    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& pstrName, const DString& pstrValue) override;
    virtual void SetPos(UiRect rc) override;
    virtual void HandleEvent(const EventArgs& msg) override;
    virtual bool MouseEnter(const EventArgs& msg) override;
    virtual bool MouseLeave(const EventArgs& msg) override;
    virtual void PaintChild(IRender* pRender, const UiRect& rcPaint) override;
    virtual void SetParent(Box* pParent) override;
    virtual void SetWindow(Window* pWindow) override;
    virtual Control* FindControl(FINDCONTROLPROC Proc, void* pProcData,
                                 uint32_t uFlags, const UiPoint& ptMouse,
                                 const UiPoint& scrollPos = UiPoint()) override;
    virtual void ClearImageCache() override;

    /** When the DPI changes, update the control size and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] nNewDpiScale The new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

    /** Get the real offset of the scroll bar, represented by a 32-bit integer value (the virtual table uses the virtual scroll bar position)
    *   If ScrollVirtualOffset is set, this function subtracts the virtual offset from the scroll bar position;
    *   If ScrollVirtualOffset is not set, this function returns the same result as UiSize64 GetScrollPos(), but checks whether it is out of bounds;
    *   This function exists to support virtual tables (VirtualScrollBox) with large amounts of data, avoiding UiRect out-of-bounds issues.
    */
    UiSize GetScrollOffset() const;

    /** Notification interface for ScrollOffset value changes
    * @param [in] oldScrollOffset The old value
    * @param [in] newScrollOffset The new value
    */
    virtual void OnScrollOffsetChanged(const UiSize& /*oldScrollOffset*/, const UiSize& /*newScrollOffset*/) {}

    /** Get the scroll bar position (cx represents the horizontal scroll bar position, cy represents the vertical scroll bar position)
     */
    virtual UiSize64 GetScrollPos() const;

    /** Get the scroll bar range (cx represents the horizontal scroll bar range, cy represents the vertical scroll bar range)
     */
    virtual UiSize64 GetScrollRange() const;

    /** Set the scroll bar position
     * @param[in] szPos The position data to set (cx represents the horizontal scroll bar position, cy represents the vertical scroll bar position)
     */
    virtual void SetScrollPos(UiSize64 szPos);

    /** Set the Y-axis coordinate of the scroll bar
     * @param[in] y The Y-axis coordinate value to set
     */
    virtual void SetScrollPosY(int64_t y);
    
    /** Set the X-axis coordinate of the scroll bar
     * @param[in] x The X-axis coordinate value to set
     */
    virtual void SetScrollPosX(int64_t x);
    
    /** Scroll the scroll bar up
     * @param[in] deltaValue The scroll distance, default is DUI_NOSET_VALUE
     */
    virtual void LineUp(int32_t deltaValue = DUI_NOSET_VALUE);
    
    /** Scroll the scroll bar down
     * @param[in] deltaValue The scroll distance, default is DUI_NOSET_VALUE
     */
    virtual void LineDown(int32_t deltaValue = DUI_NOSET_VALUE);
    
    /** Scroll the scroll bar left
     * @param[in] deltaValue The scroll distance, default is DUI_NOSET_VALUE
     */
    virtual void LineLeft(int32_t deltaValue = DUI_NOSET_VALUE);
    
    /** Scroll the scroll bar right
     * @param[in] deltaValue The scroll distance, default is DUI_NOSET_VALUE
     */
    virtual void LineRight(int32_t deltaValue = DUI_NOSET_VALUE);
    
    /** Scroll up by a page-size distance
     */
    virtual void PageUp();

    /** Scroll down by a page-size distance
     */
    virtual void PageDown();

    /** Go back to the top of the scroll bar
     */
    virtual void HomeUp();

    /** Scroll to the bottom position
     * @param[in] arrange Whether to reset the scroll bar position, default is true
     */
    virtual void EndDown(bool arrange = true);

    /** Scroll left by a page-size distance
     */
    virtual void PageLeft();

    /** Scroll right by a page-size distance
     */
    virtual void PageRight();

    /** Scroll to the far left
     */
    virtual void HomeLeft();

    /** Scroll to the far right
     */
    virtual void EndRight();

    /** Touch scroll up (responds to WM_TOUCH message)
     * @param[in] deltaValue The scroll distance
     */
    virtual void TouchUp(int32_t deltaValue);

    /** Touch scroll down (responds to WM_TOUCH message)
     * @param[in] deltaValue The scroll distance
     */
    virtual void TouchDown(int32_t deltaValue);

    /** Enable the scroll bar
     * @param[in] bEnableVertical Whether to enable the vertical scroll bar, default is true
     * @param[in] bEnableHorizontal Whether to enable the horizontal scroll bar, default is true
     */
    virtual void EnableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = false);

    /** Get the object pointer of the vertical scroll bar
     */
    ScrollBar* GetVScrollBar() const;

    /** Get the object pointer of the horizontal scroll bar
     */
    ScrollBar* GetHScrollBar() const;

    /** Check whether the vertical scroll bar is valid
     */
    bool IsVScrollBarValid() const;

    /** Check whether the horizontal scroll bar is valid
     */
    bool IsHScrollBarValid() const;

    /** Whether it is already at the bottom
     */
    bool IsAtEnd() const;

    /** Whether it is locked to the bottom
     */
    bool IsHoldEnd() const;

    /** Set whether the scroll bar is always locked to the bottom position
     * @param[in] bHoldEnd Set to true to lock, false to not lock
     */
    void SetHoldEnd(bool bHoldEnd);
    
    /** Get the scroll step of the vertical scroll bar
     */
    int32_t GetVerScrollUnitPixels() const;

    /** Set the scroll step of the vertical scroll bar
     * @param [in] nUnitPixels The step to set
     * @param [in] bNeedDpiScale Whether DPI scaling is needed
     */
    void SetVerScrollUnitPixels(int32_t nUnitPixels, bool bNeedDpiScale);

    /** Get the scroll step of the horizontal scroll bar
    */
    int32_t GetHorScrollUnitPixels() const;

    /** Set the scroll step of the horizontal scroll bar
    * @param [in] nUnitPixels The step to set
    * @param [in] bNeedDpiScale Whether DPI scaling is needed
    */
    void SetHorScrollUnitPixels(int32_t nUnitPixels, bool bNeedDpiScale);

    /** Get whether the scroll bar of the container floats above the child controls
     * @return Returns true if floating, otherwise false
     */
    bool GetScrollBarFloat() const;

    /** Set whether the scroll bar of the container floats above the child controls
     * @param[in] bScrollBarFloat true means floating, false means not floating over the controls
     */
    void SetScrollBarFloat(bool bScrollBarFloat);

    /** Get whether the scroll bar of the container is displayed on the left
     * @return Returns true if on the left, false if on the right
     */
    bool IsVScrollBarAtLeft() const;

    /** Set whether the scroll bar of the container is displayed on the left
    * @param[in] bAtLeft true means on the left, false means on the right
    */
    void SetVScrollBarAtLeft(bool bAtLeft);

    /** Get the outer margin of the scroll bar
     */
    const UiPadding& GetScrollBarPadding() const;

    /** Set the outer margin of the scroll bar, so that the scroll bar does not occupy the whole container
     * @param [in] rcScrollBarPadding The margin to set
     * @param [in] bNeedDpiScale Whether DPI scaling is supported
     */
    void SetScrollBarPadding(UiPadding rcScrollBarPadding, bool bNeedDpiScale);

    /** Listen for the scroll bar position change event
     * @param [in] callback The callback function notified after a change
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachScrollPosChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventScrollPosChanged, callback, callbackID); }

public:
    /** Get the virtual offset of the scroll bar (cx represents the virtual offset of the horizontal scroll bar, cy represents the virtual offset of the vertical scroll bar)
     */
    const UiSize64& GetScrollVirtualOffset() const;

    /** Set the virtual offset of the scroll bar
     * @param[in] szPos The position data to set (cx represents the horizontal scroll bar position, cy represents the vertical scroll bar position)
     */
    void SetScrollVirtualOffset(const UiSize64& szOffset);

    /** Set the Y-axis virtual offset of the scroll bar
     */
    void SetScrollVirtualOffsetY(int64_t yOffset);

    /** Set the X-axis virtual offset of the scroll bar
     */
    void SetScrollVirtualOffsetX(int64_t xOffset);

protected:
    /** Adjust/estimate the position and size of the child controls
     * @param [in] rc Current position information; when called externally, the inner margin does not need to be subtracted
     * @param [in] bEstimateOnly true means only estimating without adjusting the control position, false means adjusting the control position
     * @return Returns the required size, including the ScrollBox's own inner margin, excluding the outer margin
     */
    virtual UiSize64 CalcRequiredSize(const UiRect& rc, bool bEstimateOnly);

    /** Set the mouse enabled state event
    * @param [in] bChanged true means the state has changed, false means the state has not changed
    */
    virtual void OnSetMouseEnabled(bool bChanged) override;

private:
    /** Set the position and size
    * @param [in] rc The rect range passed in externally
    * @param [in] bScrollProcess true means an internal recursive call, false means an external call
    */
    void DoSetPos(UiRect rc, bool bScrollProcess);

    /** Set the position and size (internal logic implementation)
    * @param [in] rc The rect range passed in externally
    * @param [in] bScrollProcess true means an internal recursive call, false means an external call
    */
    void SetPosInternally(const UiRect& rc, bool bScrollProcess);

    /** Adjust/estimate the position and size of the child controls
     * @param [in] rc Current position information; when called externally, the inner margin does not need to be subtracted
     * @param [in] bEstimateOnly true means only estimating without adjusting the control position, false means adjusting the control position
     * @return Returns the required size, including the ScrollBox's own inner margin, excluding the outer margin
     */
    UiSize64 DoArrangeChildren(const UiRect& rc, bool bEstimateOnly);

    /** Set the position of the vertical scroll bar
    */
    void ProcessVScrollBar(UiRect rcScrollBarPos, int64_t nScrollRange, bool bShowHScrollBar, bool& bNeedResetPos);

    /** Set the position of the horizontal scroll bar
     */
    void ProcessHScrollBar(UiRect rcScrollBarPos, int64_t nScrollRange, bool bShowVScrollBar, bool& bNeedResetPos);

    /** Check whether to show the horizontal scroll bar
    */
    bool NeedShowHScrollBar(UiRect rcBox, int64_t cxRequired,
                            UiRect& rcScrollBarPos, int64_t& nScrollRange) const;

    /** Check whether to show the vertical scroll bar
    */
    bool NeedShowVScrollBar(UiRect rcBox, int64_t cyRequired,
                            UiRect& rcScrollBarPos, int64_t& nScrollRange) const;

private:
    //Vertical scroll bar interface
    std::unique_ptr<ScrollBar> m_pVScrollBar;

    //Horizontal scroll bar interface
    std::unique_ptr<ScrollBar> m_pHScrollBar;

    //Virtual offset of the scroll bar
    UiSize64 m_scrollVirtualOffset;

private:
    //Outer margin of the scroll bar
    UiPadding m_rcScrollBarPadding;

    //Scroll step of the vertical scroll bar
    int32_t m_nVScrollUnitPixels;

    //Scroll step of the horizontal scroll bar
    int32_t m_nHScrollUnitPixels;

    //Whether it is locked to the bottom
    bool m_bHoldEnd;

    //Whether the scroll bar of the container floats above the child controls
    bool m_bScrollBarFloat;

    //Whether the scroll bar of the container is displayed on the left
    bool m_bVScrollBarAtLeft;
};

/** ScrollBox with horizontal layout
*/
class DUI_API HScrollBox : public ScrollBox
{
public:
    explicit HScrollBox(Window* pWindow) :
        ScrollBox(pWindow, new HLayout)
    {
    }

    virtual DString GetType() const override { return DUI_CTR_HSCROLLBOX; }
};

/** ScrollBox with vertical layout
*/
class DUI_API VScrollBox : public ScrollBox
{
public:
    explicit VScrollBox(Window* pWindow) :
        ScrollBox(pWindow, new VLayout)
    {
    }

    virtual DString GetType() const override { return DUI_CTR_VSCROLLBOX; }
};

/** ScrollBox with horizontal flow layout
*/
class DUI_API HFlowScrollBox : public ScrollBox
{
public:
    explicit HFlowScrollBox(Window* pWindow):
        ScrollBox(pWindow, new HFlowLayout)
    {
    }

    virtual DString GetType() const override { return DUI_CTR_HFLOW_SCROLLBOX; }
};

/** ScrollBox with vertical flow layout
*/
class DUI_API VFlowScrollBox : public ScrollBox
{
public:
    explicit VFlowScrollBox(Window* pWindow):
        ScrollBox(pWindow, new VFlowLayout)
    {
    }

    virtual DString GetType() const override { return DUI_CTR_VFLOW_SCROLLBOX; }
};

/** ScrollBox with tile layout (horizontal layout)
*/
class DUI_API HTileScrollBox : public ScrollBox
{
public:
    explicit HTileScrollBox(Window* pWindow) :
        ScrollBox(pWindow, new HTileLayout)
    {
    }

    virtual DString GetType() const override { return DUI_CTR_HTILE_SCROLLBOX; }
};

/** ScrollBox with tile layout (vertical layout)
*/
class DUI_API VTileScrollBox : public ScrollBox
{
public:
    explicit VTileScrollBox(Window* pWindow) :
        ScrollBox(pWindow, new VTileLayout)
    {
    }

    virtual DString GetType() const override { return DUI_CTR_VTILE_SCROLLBOX; }
};

} // namespace ui

#endif // UI_BOX_SCROLLBOX_H_
