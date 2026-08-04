#ifndef UI_LAYOUT_GRIDLAYOUT_H_
#define UI_LAYOUT_GRIDLAYOUT_H_

#include "dui/Layout/Layout.h"
#include <unordered_map>

namespace ui 
{
/** Grid layout
 */
class DUI_API GridLayout : public Layout
{
    typedef Layout BaseClass;
public:
    GridLayout();

public:
    /** Get the number of rows (0 means auto-calculation)
     */
    int32_t GetRows() const;

    /** Set the number of rows (0 means auto-calculation)
     * @param [in] nRows number of rows
     */
    void SetRows(int32_t nRows);

    /** Get the number of columns (0 means auto-calculation)
     */
    int32_t GetColumns() const;

    /** Set the number of columns (0 means auto-calculation)
     * @param [in] nCols number of columns
     */
    void SetColumns(int32_t nCols);

    /** Set the grid cell width (0 means auto-calculation)
    */
    void SetGridWidth(int32_t nGridWidth, bool bNeedDpiScale);

    /** Get the grid cell width (0 means auto-calculation)
    * @return returns the grid width (already DPI-scaled)
    */
    int32_t GetGridWidth() const;

    /** Set the grid cell height (0 means auto-calculation)
    */
    void SetGridHeight(int32_t nGridHeight, bool bNeedDpiScale);

    /** Get the grid cell height (0 means auto-calculation)
    * @return returns the grid height (already DPI-scaled)
    */
    int32_t GetGridHeight() const;

    /** Whether to scale down proportionally when the control content exceeds the boundary
     *   true  use the child control's size; if it exceeds the grid size, scale it down proportionally so that the control content is fully displayed within the grid
     *   false ignore the child control's own size; the child control size matches the grid size
     */
    void SetScaleDown(bool bScaleDown);

    /** Determine whether to scale down proportionally when exceeding the boundary
     *@return return value true  use the child control's size; if it exceeds the grid size, scale it down proportionally so that the control content is fully displayed within the grid
     *              false ignore the child control's own size; the child control size matches the grid size
     */
    bool IsScaleDown() const;

public:
    /** Layout type
    */
    virtual LayoutType GetLayoutType() const override { return LayoutType::GridLayout; }

    /** Set layout attributes
     * @param [in] strName attribute name to set
     * @param [in] strValue attribute value to set
     * @param [in] dpiManager DPI management interface
     * @return true on success, false if the attribute does not exist
     */
    virtual bool SetAttribute(const DString& strName, 
                              const DString& strValue,
                              const DpiManager& dpiManager) override;

    /** Update control size and layout when DPI changes
     * @param [in] nOldDpiScale old DPI scale percentage
     * @param [in] dpiManager DPI scale manager
     */
    virtual void ChangeDpiScale(const DpiManager& dpiManager, uint32_t nOldDpiScale) override;

    /** Adjust the position and size of all child controls according to the layout strategy
     * @param [in] items child control list
     * @param [in] rc current container position and size info, including the padding, but not the margin
     * @param [in] bEstimateOnly true means only estimate without adjusting control positions, false means adjust control positions
     * @return returns the width and height info of the final layout after arrangement, including the Box container padding, but not the Box container's own margin (use this return value when the container supports a scrollbar)
     */
    virtual UiSize64 ArrangeChildren(const std::vector<Control*>& items, UiRect rc, bool bEstimateOnly = false) override;

    /** Estimate the container layout size based on the size of the internal child controls (used to evaluate the size of controls with "auto" width or height; stretch-type child controls are not counted in the size estimation)
     * @param [in] items child control list
     * @param [in] szAvailable available width and height of the container, including the padding allocated to the container, but not the margin allocated to the container
     * @return returns the size info (width and height) of the final layout after arrangement, including the Box container's own padding, but not the Box container's own margin;
     */
    virtual UiSize64 EstimateLayoutSize(const std::vector<Control*>& items, UiSize szAvailable) override;

private:
    /** Adjust the position info of all internal controls
     * @param [in] items control list
     * @param [in] rc current container position info, including the padding, but not the margin
     * @param [in] bEstimateOnly true means only estimate without adjusting control positions, false means adjust control positions
     * @return returns the width and height info of the final box after arrangement, including the Owner Box padding, but not the margin
     */
    UiSize64 ArrangeChildInternal(const std::vector<Control*>& items, UiRect rc, bool bEstimateOnly) const;

    /** Calculate the actual number of rows and columns
    * @param [in] gridCount total number of cells
    * @param [in,out] rows input the currently set number of rows, output the calculated effective number of rows
    * @param [in,out] cols input the currently set number of columns, output the calculated effective number of columns
    */
    void CalcActualGridSize(int32_t gridCount, int32_t& rows, int32_t& cols) const;

    /** Calculate the width of each column (evenly distributed, after deducting the horizontal spacing)
    * @param [in] contentRect rect range of the content display area (excluding the container padding)
    * @param [in] cols number of columns
    * @return returns the calculated width of each column
    */
    std::vector<int32_t> CalcColumnWidths(const UiRect& contentRect, int32_t cols) const;

    /** Calculate the height of each row (evenly distributed, after deducting the vertical spacing)
    * @param [in] contentRect rect range of the content display area (excluding the container padding)
    * @param [in] rows number of rows
    * @return returns the calculated height of each row
    */
    std::vector<int32_t> CalcRowHeights(const UiRect& contentRect, int32_t rows) const;

    /** Calculate the merged cell areas (handling rowSpan/columnSpan)
    * @param [in] controls list of child controls to be laid out
    * @param [in] rows number of rows
    * @param [in] cols number of columns
    * @param [in] colWidths list of column widths
    * @param [in] rowHeights list of row heights
    * @param [out] cellOccupied marks the cells in the merged range as occupied (2D array)
    * @param [out] ctrlCellRects position and size of the list controls
    */
    bool CalcMergedCellRects(const std::vector<Control*>& controls,
                             int32_t rows, int32_t cols,
                             const std::vector<int32_t>& colWidths,
                             const std::vector<int32_t>& rowHeights,
                             std::vector<std::vector<bool>>& cellOccupied,
                             std::unordered_map<Control*, UiRect>& ctrlCellRects) const;

    /** Calculate the overall size of the grid
    * @param [in] colWidths list of column widths
    * @param [in] rowHeights list of row heights
    * @return returns the size of the grid
    */
    UiSize CalcGridTotalSize(const std::vector<int32_t>& colWidths, const std::vector<int32_t>& rowHeights) const;

    /** Calculate the position of the grid in the container (based on the layout's alignment attributes)
    * @param [in] contentRect rect range of the content display area (excluding the container padding)
    * @param [in] gridSize grid size
    */
    UiRect CalcGridPosition(const UiRect& contentRect, const UiSize& gridSize) const;

    /** Adjust the positions of the merged cells (adding the overall grid offset)
    * @param [in] ctrlCellRects position and size of the list controls
    * @param [in] gridRect grid rect range
    */
    void AdjustMergedCellPositions(std::unordered_map<Control*, UiRect>& ctrlCellRects,
                                   const UiRect& gridRect) const;

    /** Arrange controls into the merged cells (using the controls' own alignment attributes)
    * @param [in] controls list of child controls to be laid out
    * @param [out] ctrlCellRects position and size of the list controls
    */
    void ArrangeControlsInMergedCells(const std::vector<Control*>& controls,
                                      const std::unordered_map<Control*, UiRect>& ctrlCellRects) const;

    /** Calculate the position based on the control's own alignment attributes
    * @param [in] contentRect rect range of the container
    * @param [in] ctrlSize control size
    * @param [in] ctrl control interface
    */
    UiRect CalcControlPosition(const UiRect& contentRect, const UiSize& ctrlSize, Control* ctrl) const;

private:
    /** Number of grid rows (0 means auto-calculation)
    */
    int32_t m_nRows;

    /** Number of grid columns (0 means auto-calculation)
    */
    int32_t m_nColumns;

    /** Grid cell width (0 means auto-calculation)
    */
    int32_t m_nGridWidth;

    /** Grid cell height (0 means auto-calculation)
    */
    int32_t m_nGridHeight;

    /** Whether to scale down proportionally when the control content exceeds the boundary
     *  true  use the child control's size; if it exceeds the grid size, scale it down proportionally so that the control content is fully displayed within the grid
     *  false ignore the child control's own size; the child control size matches the grid size
     */
    bool m_bScaleDown;
};

} // namespace ui

#endif // UI_LAYOUT_GRIDLAYOUT_H_
