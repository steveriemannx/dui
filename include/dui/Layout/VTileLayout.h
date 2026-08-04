#ifndef UI_LAYOUT_VTILE_LAYOUT_H_
#define UI_LAYOUT_VTILE_LAYOUT_H_

#include "dui/Layout/Layout.h"

namespace ui 
{

/** Tile layout (vertical)
 *  Horizontal alignment: centered by default
 *  Vertical alignment: top-aligned, with controls arranged in sequence
 *  In a tile layout, the vertical alignment specified by a child control itself does not take effect
 */
class DUI_API VTileLayout : public Layout
{
    typedef Layout BaseClass;
public:
    VTileLayout();

    /** Layout type
    */
    virtual LayoutType GetLayoutType() const override { return LayoutType::VTileLayout; }

    /** Arrange the positions and sizes of all internal child controls according to the layout strategy
     * @param [in] items List of child controls
     * @param [in] rc Position and size information of the current container, including padding but not margin
     * @param [in] bEstimateOnly true means only estimate without adjusting control positions; false means adjust control positions
     * @return The final width and height of the arranged layout, including the Box container's padding but not its margin (used when the container supports a scroll bar)
     */
    virtual UiSize64 ArrangeChildren(const std::vector<Control*>& items, UiRect rc, bool bEstimateOnly = false) override;

    /** Estimate the container layout size from the sizes of the internal child controls (used to evaluate controls whose width or height is "auto"; stretch-type child controls are not counted in the size estimation)
     * @param [in] items List of child controls
     * @param [in] szAvailable The container's available width and height, including the padding allocated to the container but not its margin
     * @return The final layout size (width and height) after arrangement, including the Box container's padding but not its margin;
     */
    virtual UiSize64 EstimateLayoutSize(const std::vector<Control*>& items, UiSize szAvailable) override;
    
    /** Set layout attributes
     * @param [in] strName The attribute name to set
     * @param [in] strValue The attribute value to set
     * @param [in] dpiManager DPI management interface
     * @return true on success, false if the attribute does not exist
     */
    virtual bool SetAttribute(const DString& strName, 
                              const DString& strValue,
                              const DpiManager& dpiManager) override;

    /** When the DPI changes, update the control sizes and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] dpiManager The DPI scale manager
    */
    virtual void ChangeDpiScale(const DpiManager& dpiManager, uint32_t nOldDpiScale) override;

public:
    /** Get the child item size; the width and height include the control's outer and inner margins
     */
    const UiSize& GetItemSize() const;

    /** Set the child item size
     * @param[in] szItem The child item size data; the width and height include the control's outer and inner margins
     * @param [in] bArrange Whether to re-arrange when it changes
     */
    void SetItemSize(UiSize szItem, bool bArrange = true);

    /** Get the number of columns
     */
    int32_t GetColumns() const;

    /** Set how many columns of data to display
     * @param[in] nCols The number of columns to display
     */
    void SetColumns(int32_t nCols);

    /** Set whether to auto-calculate the number of columns
     */
    void SetAutoCalcColumns(bool bAutoCalcColumns);

    /** Check whether the number of columns is auto-calculated
     */
    bool IsAutoCalcColumns() const;

    /** When the control content exceeds the boundary, scale it down proportionally so that the content is fully displayed within the tile area
    */
    void SetScaleDown(bool bScaleDown);

    /** Check whether to scale down proportionally when the boundary is exceeded
    */
    bool IsScaleDown() const;

    /** Set whether to auto-calculate the width of child items (effective only when fixed columns are set)
    */
    void SetAutoCalcItemWidth(bool bAutoCalcItemWidth);

    /** Whether to auto-calculate the width of child items
    */
    bool IsAutoCalcItemWidth() const;

    /** Whether the current layout is a free layout
    */
    bool IsFreeLayout() const;

    /** Calculate the width of child items
    */
    bool AutoCalcItemWidth(int32_t nColumns, int32_t nMarginX, int32_t szAvailable, int32_t& nItemWidth) const;

private:
    /** Unhandled child control interfaces and their width/height information
    */
    struct ItemSizeInfo
    {
        Control* pControl = nullptr; // Child control interface
        int32_t cx = 0;              // Width of the child control
        int32_t cy = 0;              // Height of the child control
    };

    /** Get the available width and height for size estimation
    * @param [in] pControl Control interface
    * @param [in] szItem The width and height of each tile control (configured value)
    * @param [in] rc The available area rect of the container holding the tile controls
    * @return The estimated size of the control (width and height)
    */
    static UiSize CalcEstimateSize(Control* pControl, const UiSize& szItem, const UiRect& rc);

    /** Get the basic parameter: the number of tile columns
    * @param [in] normalItems List of child controls
    * @param [in] rc The rect of the container holding the tile controls
    * @param [in] tileWidth The width of each tile control (configured value)
    * @param [in] childMarginX The X-axis spacing of the child controls
    * @param [in] childMarginY The Y-axis spacing of the child controls
    * @param [out] nColumns Returns the total number of columns
    */
    static void CalcTileColumns(const std::vector<ItemSizeInfo>& normalItems, const UiRect& rc,
                                int32_t tileWidth, int32_t childMarginX, int32_t childMarginY,
                                int32_t& nColumns);

    /** Estimate the size of a floating control
    */
    static UiSize64 EstimateFloatSize(Control* pControl, const UiRect& rc);

    /** Handle floating child controls and return the list of unhandled child controls
    * @param [in] items List of child controls
    * @param [in] rect The available outer rect size
    * @param [in] szItem The width and height of each tile control (configured value)
    * @param [in] bEstimateOnly If true, only the area is calculated and control positions are not adjusted; if false, control positions are adjusted.
    * @param [out] normalItems Returns the list of unhandled child controls and their size information
    * @return The area width and height occupied by the floating controls
    */
    static UiSize64 ArrangeFloatChild(const std::vector<Control*>& items,
                                      const UiRect& rc,
                                      const UiSize& szItem,
                                      bool bEstimateOnly,
                                      std::vector<ItemSizeInfo>& normalItems);

    /** Get the basic parameter: tile height. During layout arrangement, the height of each row is calculated when the row starts
    * @param [in] normalItems List of child controls
    * @param [in] iterBegin The iterator at the start of the child controls
    * @param [in] nColumns Total number of columns
    * @param [in] szItem The width and height of the tile controls (configured value)
    * @return The height value, including the margin values Margin.top + Margin.bottom
    */
    static int32_t CalcTileRowHeight(const std::vector<ItemSizeInfo>& normalItems,
                                     const std::vector<ItemSizeInfo>::const_iterator iterBegin,
                                      int32_t nColumns,
                                     const UiSize& szItem);

    /** Calculate the display coordinates and size of a tile control
    * @param [in] itemSizeInfo The tile control interface and its size information
    * @param [in] tileWidth The configured tile control width
    * @param [in] tileHeight The configured tile control height (taken as the row height)
    * @param [in] ptTile The top-left coordinates of the current tile control
    * @param [in] bScaleDown When the control content exceeds the boundary, scale it down proportionally so that the content is fully displayed within the tile area
    * @param [out] szTilePos The display coordinates, width and height of the tile control
    * @return The target area size of the tile control (width and height), including the control's outer margins
    */
    static UiSize CalcTilePosition(const ItemSizeInfo& itemSizeInfo,
                                   int32_t tileWidth,
                                   int32_t tileHeight,
                                   const UiPoint& ptTile,
                                   bool bScaleDown, 
                                   UiRect& szTilePos);

private:
    /** Internal implementation for laying out child controls (default alignment: centered)
    * @param [in] items List of child controls
    * @param [in] rect The available rect size, including the padding allocated to the control but not its margin
    * @param [in] bEstimateOnly If true, only the area is calculated and control positions are not adjusted; if false, control positions are adjusted.
    * @param [in] inColumnWidths The width value of each column; may be empty
    * @param [out] outColumnWidths The width value of each column used in this layout
    * @return The width and height of the area
    */
    UiSize64 ArrangeChildNormal(const std::vector<Control*>& items, 
                                UiRect rect,
                                bool bEstimateOnly,
                                const std::vector<int32_t>& inColumnWidths,
                                std::vector<int32_t>& outColumnWidths) const;

    /** Arrange controls using a free layout (no fixed column count; make full use of the display space to show as much content as possible) (default alignment: centered)
    * @param [in] items List of child controls
    * @param [in] rect The available rect size, including the padding allocated to the control but not its margin
    * @param [in] bEstimateOnly If true, only the area is calculated and control positions are not adjusted; if false, control positions are adjusted.
    * @return The width and height of the area
    */
    UiSize64 ArrangeChildFreeLayout(const std::vector<Control*>& items, 
                                    UiRect rect, 
                                    bool bEstimateOnly) const;

private:
    // Child item size; the width and height include the control's outer and inner margins
    UiSize m_szItem;

    // Number of columns of data to display
    int32_t m_nColumns;

    // Auto-calculate the number of columns
    bool m_bAutoCalcColumns;

    // When the control content exceeds the boundary, scale it down proportionally so that it is fully displayed within the tile area
    bool m_bScaleDown;

    // Whether to auto-calculate the width of child items (adapts to the overall width of the parent control; effective only when fixed columns are set)
    bool m_bAutoCalcItemWidth;
};

} // namespace ui

#endif // UI_LAYOUT_VTILE_LAYOUT_H_
