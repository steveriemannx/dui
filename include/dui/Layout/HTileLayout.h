#ifndef UI_LAYOUT_HTILE_LAYOUT_H_
#define UI_LAYOUT_HTILE_LAYOUT_H_

#include "dui/Layout/Layout.h"

namespace ui 
{

/** Tile layout (horizontal)
 *  Horizontal alignment: left-aligned, controls arranged in sequence
 *  Vertical alignment: center-aligned by default
 *  In a tile layout, the alignment specified by the child controls themselves does not take effect
 */
class DUI_API HTileLayout : public Layout
{
    typedef Layout BaseClass;
public:
    HTileLayout();

    /** Layout type
    */
    virtual LayoutType GetLayoutType() const override { return LayoutType::HTileLayout; }

    /** Arrange the positions and sizes of all child controls according to the layout strategy
     * @param [in] items list of child controls
     * @param [in] rc position and size info of the current container, including padding but not margins
     * @param [in] bEstimateOnly true means only estimate without adjusting control positions; false means adjust control positions
     * @return the width and height of the final layout after arrangement, including the padding of the Box container, but not the margins of the Box container itself (used when the container supports a scroll bar)
     */
    virtual UiSize64 ArrangeChildren(const std::vector<Control*>& items, UiRect rc, bool bEstimateOnly = false) override;

    /** Estimate the container layout size based on the sizes of the internal child controls (used to evaluate the size of controls whose width or height is "auto"; stretch-type child controls are not included in the size estimation)
     * @param [in] items list of child controls
     * @param [in] szAvailable available width and height of the container, including the padding allocated to the container, but not the margins allocated to the container
     * @return the size info (width and height) of the final layout after arrangement, including the padding of the Box container itself, but not the margins of the Box container itself;
     */
    virtual UiSize64 EstimateLayoutSize(const std::vector<Control*>& items, UiSize szAvailable) override;
    
    /** Set layout attributes
     * @param [in] strName name of the attribute to set
     * @param [in] strValue value of the attribute to set
     * @param [in] dpiManager DPI management interface
     * @return true if set successfully, false if the attribute does not exist
     */
    virtual bool SetAttribute(const DString& strName, 
                              const DString& strValue,
                              const DpiManager& dpiManager) override;

    /** DPI changed; update control size and layout
    * @param [in] nOldDpiScale old DPI scale percentage
    * @param [in] dpiManager DPI scale manager
    */
    virtual void ChangeDpiScale(const DpiManager& dpiManager, uint32_t nOldDpiScale) override;

public:
    /** Get the child item size; the width and height include the control's margins and padding
     */
    const UiSize& GetItemSize() const;

    /** Set the child item size
     * @param[in] szItem child item size data; the width and height include the control's margins and padding
     * @param [in] bArrange whether to rearrange when the size changes
     */
    void SetItemSize(UiSize szItem, bool bArrange = true);

    /** Get the number of rows
     */
    int32_t GetRows() const;

    /** Set how many rows of data to display
     * @param[in] nRows the number of rows to display
     */
    void SetRows(int32_t nRows);

    /** Set whether to auto-calculate the number of rows
     */
    void SetAutoCalcRows(bool bAutoCalcRows);

    /** Check whether the number of rows is auto-calculated
     */
    bool IsAutoCalcRows() const;

    /** When the control content exceeds the boundary, scale it down proportionally so that the content is fully displayed within the tile area
    */
    void SetScaleDown(bool bScaleDown);

    /** Check whether to scale down proportionally when the content exceeds the boundary
    */
    bool IsScaleDown() const;

    /** Set whether to auto-calculate the height of child items (only effective when a fixed number of rows is set)
    */
    void SetAutoCalcItemHeight(bool bAutoCalcItemHeight);

    /** Whether to auto-calculate the height of child items
    */
    bool IsAutoCalcItemHeight() const;

    /** Whether the current layout is a free layout
    */
    bool IsFreeLayout() const;

    /** Calculate the height of child items
    */
    bool AutoCalcItemHeight(int32_t nRows, int32_t nMarginY, int32_t szAvailable, int32_t& nItemHeight) const;

private:
    /** Unprocessed child control interface and its width/height info
    */
    struct ItemSizeInfo
    {
        Control* pControl = nullptr; // Child control interface
        int32_t cx = 0;              // Width of the child control
        int32_t cy = 0;              // Height of the child control
    };

    /** Get the available width and height used for size estimation
    * @param [in] pControl control interface
    * @param [in] szItem width and height of each tile control (configured value)
    * @param [in] rc available area rect of the container holding the tile controls
    * @return the estimated size of the control (width and height)
    */
    static UiSize CalcEstimateSize(Control* pControl, const UiSize& szItem, const UiRect& rc);

    /** Get the basic parameter: the number of tile columns
    * @param [in] normalItems list of child controls
    * @param [in] rc rect of the container holding the tile controls
    * @param [in] tileHeight width of each tile control (configured value)
    * @param [in] childMarginX X-axis spacing of child controls
    * @param [in] childMarginY Y-axis spacing of child controls
    * @param [out] nRows returns the total number of rows
    */
    static void CalcTileRows(const std::vector<ItemSizeInfo>& normalItems, const UiRect& rc,
                             int32_t tileHeight, int32_t childMarginX, int32_t childMarginY,
                             int32_t& nRows);

    /** Estimate the size of a float control
    */
    static UiSize64 EstimateFloatSize(Control* pControl, const UiRect& rc);

    /** Handle float child controls, and return the list of unprocessed child controls
    * @param [in] items list of child controls
    * @param [in] rect available external rect size
    * @param [in] szItem width and height of each tile control (configured value)
    * @param [in] bEstimateOnly if true, only calculate the area without adjusting control positions; if false, adjust control positions
    * @param [out] normalItems returns the list of unprocessed child controls and their size info
    * @return the width and height of the area occupied by the float controls
    */
    static UiSize64 ArrangeFloatChild(const std::vector<Control*>& items,
                                      const UiRect& rc,
                                      const UiSize& szItem,
                                      bool bEstimateOnly,
                                      std::vector<ItemSizeInfo>& normalItems);

    /** Get the basic parameter: tile height; during layout arrangement, when each column starts, calculate the width of this column
    * @param [in] normalItems list of child controls
    * @param [in] iterBegin iterator to the first child control
    * @param [in] nRows total number of rows
    * @param [in] szItem width and height of the tile controls (configured value)
    * @return the height/width value, including the margins Margin.top + Margin.bottom
    */
    static int32_t CalcTileColumnWidth(const std::vector<ItemSizeInfo>& normalItems,
                                       const std::vector<ItemSizeInfo>::const_iterator iterBegin,
                                        int32_t nRows,
                                       const UiSize& szItem);

    /** Calculate the display coordinates and size of a tile control
    * @param [in] itemSizeInfo interface of the tile control, and its size info
    * @param [in] tileWidth configured tile control width
    * @param [in] tileHeight configured tile control height (taken as the row height)
    * @param [in] ptTile coordinates of the top-left corner of the current tile control
    * @param [in] bScaleDown when the control content exceeds the boundary, scale it down proportionally so that the content is fully displayed within the tile area
    * @param [out] szTilePos display coordinates, width and height of the tile control
    * @return the size (width and height) of the target area of the tile control; the width and height include the control's margins
    */
    static UiSize CalcTilePosition(const ItemSizeInfo& itemSizeInfo,
                                   int32_t tileWidth,
                                   int32_t tileHeight,
                                   const UiPoint& ptTile,
                                   bool bScaleDown, 
                                   UiRect& szTilePos);

private:
    /** Internal implementation function for laying out child controls (default alignment: center)
    * @param [in] items list of child controls
    * @param [in] rect available rect size, including the padding allocated to the control, but not the margins allocated to the control
    * @param [in] bEstimateOnly if true, only calculate the area without adjusting control positions; if false, adjust control positions
    * @param [in] inRowHeights height of each row; can be empty
    * @param [out] outRowHeights the row heights used in this layout pass
    * @return the width and height of the area
    */
    UiSize64 ArrangeChildNormal(const std::vector<Control*>& items, 
                                UiRect rect,
                                bool bEstimateOnly,
                                const std::vector<int32_t>& inRowHeights,
                                std::vector<int32_t>& outRowHeights) const;

    /** Arrange controls using free layout (no fixed column count; make full use of the display space to show as much content as possible) (default alignment: center)
    * @param [in] items list of child controls
    * @param [in] rect available rect size, including the padding allocated to the control, but not the margins allocated to the control
    * @param [in] bEstimateOnly if true, only calculate the area without adjusting control positions; if false, adjust control positions
    * @return the width and height of the area
    */
    UiSize64 ArrangeChildFreeLayout(const std::vector<Control*>& items, 
                                    UiRect rect, 
                                    bool bEstimateOnly) const;

private:
    // Child item size; the width and height include the control's margins and padding
    UiSize m_szItem;

    // Number of rows of data to display
    int32_t m_nRows;

    // Auto-calculate the number of rows
    bool m_bAutoCalcRows;

    // When the control content exceeds the boundary, scale it down proportionally so that it is fully displayed within the tile area
    bool m_bScaleDown;

    // Whether to auto-calculate the height of child items (auto-fit to the parent control's overall height; only effective when a fixed number of rows is set)
    bool m_bAutoCalcItemHeight;
};

} // namespace ui

#endif // UI_LAYOUT_HTILE_LAYOUT_H_
