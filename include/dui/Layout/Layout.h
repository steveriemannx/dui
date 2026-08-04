#ifndef UI_LAYOUT_LAYOUT_H_
#define UI_LAYOUT_LAYOUT_H_

#include "dui/Core/UiTypes.h"

namespace ui 
{
class Box;
class Control;
class DpiManager;

/** Layout type of a control
*/
enum class LayoutType
{
    FloatLayout,            // Floating layout

    HLayout,                // Horizontal layout
    VLayout,                // Vertical layout
    HTileLayout,            // Horizontal tile layout
    VTileLayout,            // Vertical tile layout
    HFlowLayout,            // Horizontal flow layout
    VFlowLayout,            // Vertical flow layout

    VirtualHLayout,         // Virtual horizontal layout
    VirtualVLayout,         // Virtual vertical layout
    VirtualHTileLayout,     // Virtual horizontal tile layout
    VirtualVTileLayout,     // Virtual vertical tile layout

    GridLayout,             // Grid layout

    ListCtrlReportLayout    // Report mode layout of the ListCtrl control (internal use only)
};

/** Base class of the layout manager
 *  The layout type of this class is Float layout: child controls are stacked in sequence at the top-left coordinates of the area (equivalent to treating all of them as Float controls)
 *  Horizontal alignment: none; child controls are arranged according to the alignment specified by each child control
 *  Vertical alignment: none; child controls are arranged according to the alignment specified by each child control
 */
class DUI_API Layout
{
public:
    Layout();
    Layout(const Layout& r) = delete;
    Layout& operator=(const Layout& r) = delete;
    virtual ~Layout() = default;

    /** Layout type
    */
    virtual LayoutType GetLayoutType() const { return LayoutType::FloatLayout; }

    /** Set layout attributes
     * @param [in] strName The attribute name to set
     * @param [in] strValue The attribute value to set
     * @param [in] dpiManager DPI management interface
     * @return true on success, false if the attribute does not exist
     */
    virtual bool SetAttribute(const DString& strName, 
                              const DString& strValue,
                              const DpiManager& dpiManager);

    /** When the DPI changes, update the control sizes and layout
     * @param [in] nOldDpiScale The old DPI scale percentage
     * @param [in] dpiManager The DPI scale manager
     */
    virtual void ChangeDpiScale(const DpiManager& dpiManager, uint32_t nOldDpiScale);

    /** Whether it is a vertical layout
    */
    virtual bool IsVLayout() const 
    {
        LayoutType type = GetLayoutType();
        return (type == LayoutType::VLayout) ||
               (type == LayoutType::VFlowLayout) ||
               (type == LayoutType::VTileLayout) ||
               (type == LayoutType::VirtualVLayout) ||
               (type == LayoutType::VirtualVTileLayout) ||
               (type == LayoutType::ListCtrlReportLayout);
    }

    /** Whether it is a horizontal layout
    */
    virtual bool IsHLayout() const 
    {
        LayoutType type = GetLayoutType();
        return (type == LayoutType::HLayout) ||
               (type == LayoutType::HFlowLayout) ||
               (type == LayoutType::HTileLayout) ||
               (type == LayoutType::VirtualHLayout) ||
               (type == LayoutType::VirtualHTileLayout);
    }

    /** Whether it is a tile layout (may be a vertical or horizontal layout)
    */
    virtual bool IsTileLayout() const
    {
        LayoutType type = GetLayoutType();
        return (type == LayoutType::VTileLayout) ||
               (type == LayoutType::HTileLayout) ||
               (type == LayoutType::VirtualVTileLayout) ||
               (type == LayoutType::VirtualHTileLayout);
    }

    /** Arrange the positions and sizes of all internal child controls according to the layout strategy
     * @param [in] items List of child controls
     * @param [in] rc Position and size information of the current container, including padding but not margin
     * @param [in] bEstimateOnly true means only estimate without adjusting control positions; false means adjust control positions
     * @return The final width and height of the arranged layout, including the Box container's padding but not its margin (used when the container supports a scroll bar)
     */
    virtual UiSize64 ArrangeChildren(const std::vector<Control*>& items, UiRect rc, bool bEstimateOnly = false);

    /** Estimate the container layout size from the sizes of the internal child controls (used to evaluate controls whose width or height is "auto"; stretch-type child controls are not counted in the size estimation)
     * @param [in] items List of child controls
     * @param [in] szAvailable The container's available width and height, including the padding allocated to the container but not its margin
     * @return The final layout size (width and height) after arrangement, including the Box container's padding but not its margin;
     */
    virtual UiSize64 EstimateLayoutSize(const std::vector<Control*>& items, UiSize szAvailable);

    /** When inside a ScrollBox, whether to pre-calculate the actual area size and then lay out the child controls according to the actual area size
     *  For some layouts, when inside a ScrollBox, the layout must be done according to the actual area size rather than the display area (factors: whether there are stretch-type child controls, horizontal and vertical alignment)
     * @return true means that when inside a ScrollBox, the actual area size is pre-calculated and the child controls are then laid out according to the actual area size
     *         false means that when inside a ScrollBox, no special handling is needed and the child controls are laid out directly
     */
    virtual bool LayoutByActualAreaSize() const;

public:
    /** Set the owner container interface
    */
    void SetOwner(Box* pOwner);

    /** Get the owner container interface
    */
    Box* GetOwner() const { return m_pOwner; }

    /** Get the extra margin between child controls (X-axis direction)
     * @return The extra spacing value (already DPI-scaled)
     */
    int32_t GetChildMarginX() const { return m_nChildMarginX; }

    /** Get the extra margin between child controls (Y-axis direction)
     * @return The extra spacing value (already DPI-scaled)
     */
    int32_t GetChildMarginY() const { return m_nChildMarginY; }

    /** Set the extra margin between child controls (X-axis direction)
     * @param [in] nMarginX The margin value to set (already DPI-scaled)
     */
    void SetChildMarginX(int32_t nMarginX);

    /** Set the extra margin between child controls (Y-axis direction)
     * @param [in] nMarginY The margin value to set (already DPI-scaled)
     */
    void SetChildMarginY(int32_t nMarginY);

    /** Set the extra margin between child controls (both the X-axis and Y-axis directions to the same value)
     * @param [in] nMargin The margin value to set (already DPI-scaled)
     */
    void SetChildMargin(int32_t nMargin);

    /** Get the horizontal alignment of the child controls
     */
    HorAlignType GetChildHAlignType() const { return m_hChildAlignType; }

    /** Set the horizontal alignment of the child controls
     * @param [in] hAlignType The alignment to set
     */
    void SetChildHAlignType(HorAlignType hAlignType);

    /** Get the vertical alignment of the child controls
     */
    VerAlignType GetChildVAlignType() const { return m_vChildAlignType; }

    /** Set the vertical alignment of the child controls
     * @param [in] vAlignType The alignment to set
     */
    void SetChildVAlignType(VerAlignType vAlignType);

public:
    /** Remove the padding from the area and ensure the rc area is valid
    */
    void DeflatePadding(UiRect& rc) const;

public:
    /** Set the coordinate information in the floating state
     * @param [in] pControl Control handle
     * @param [in] rcContainer The rect area to set, excluding the container's padding
     * @return The final size information of the control (width and height)
     */
    static UiSize64 SetFloatPos(Control* pControl, const UiRect& rcContainer);

protected:
    /** Check whether the configured width and height are correct; give an assertion if an error is found
    */
    void CheckConfig(const std::vector<Control*>& items);

    /** Calculate the layout position of the control according to the alignment specified by the control
    * @param [in] pControl The control interface
    * @param [in] rcContainer The rect of the target container, including the control's margin and padding
    * @param [in] childSize The size of the control pControl (width and height), including the padding; the control size will not be re-calculated internally
    * @return The position and size of the control, excluding the margin but including the padding
              The returned value can be used with pControl->SetPos(rect) to adjust the control position;
    */
    static UiRect GetFloatPos(const Control* pControl, UiRect rcContainer, UiSize childSize);

private:
    /** Set the coordinate information in the floating state
     * @param [in] pControl Control handle
     * @param [in] rcContainer The rect area to set, excluding the container's padding
     * @param [in] bEstimateOnly true means only estimate without adjusting the control position; false means adjust the control position
     * @return The final size information of the control (width and height)
     */
    static UiSize64 SetFloatPosInternal(Control* pControl, const UiRect& rcContainer, bool bEstimateOnly);

private:
    // The owner Box object
    Box* m_pOwner;

    // Extra margin between child controls: X-axis direction
    uint16_t m_nChildMarginX;

    // Extra margin between child controls: Y-axis direction
    uint16_t m_nChildMarginY;

    // Horizontal alignment of the child controls
    HorAlignType m_hChildAlignType;

    // Vertical alignment of the child controls
    VerAlignType m_vChildAlignType;
};

} // namespace ui

#endif // UI_LAYOUT_LAYOUT_H_
