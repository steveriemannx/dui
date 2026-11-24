#ifndef UI_CONTROL_LIST_CTRL_REPORT_VIEW_H_
#define UI_CONTROL_LIST_CTRL_REPORT_VIEW_H_

#include "dui/Control/ListCtrlDefs.h"

namespace ui
{
//Contains classes: ListCtrlReportView / ListCtrlReportLayout

/** List data display and layout functionality (Report type)
 *  The basic data structure of the ReportView view:
 *  <ListCtrlReportView>
 *      <ListCtrlItem> <ListCtrlSubItem/> ... <ListCtrlSubItem/> </ListCtrlItem>
 *      ...
 *      <ListCtrlItem> <ListCtrlSubItem/> ... <ListCtrlSubItem/> </ListCtrlItem>
 *  </ListCtrlReportView>
 */
class ListCtrl;
class ListCtrlData;
class ListCtrlSubItem;
class ListCtrlItem;
struct ListCtrlItemData;
class DUI_API ListCtrlReportView : public ListCtrlView, public IListCtrlView
{
    typedef ListCtrlView BaseClass;
    friend class ListCtrlReportLayout;
public:
    explicit ListCtrlReportView(Window* pWindow);
    virtual ~ListCtrlReportView() override;

    virtual std::string GetType() const override { return "ListCtrlReportView"; }
    virtual void HandleEvent(const EventArgs& msg) override;

    /** Mouse event (forwarded from a child control): intercepted in cell-selection mode to implement cell
    *  selection and range extension
    * @param [in] msg The content of the mouse event
    * @return Returns true if it has been handled as a cell selection, so row selection is not triggered
    */
    virtual bool OnListBoxItemMouseEvent(const EventArgs& msg) override;

    /** Keyboard event (forwarded from a child control): intercepted while the drop-down candidate popup is open,
    *  to consume Enter/ESC/Up/Down first
    * @param [in] msg The content of the keyboard event
    * @return Returns true if the key has been consumed, so the sub-item does not handle it
    */
    virtual bool OnListBoxItemKeyEvent(const EventArgs& msg) override;

    /** Set the ListCtrl control interface
    */
    void SetListCtrl(ListCtrl* pListCtrl);

    /** Set the data provider object
    * @param[in] pProvider The data provider object
    */
    virtual void SetDataProvider(VirtualListBoxElement* pProvider) override;

    /** Refresh the interface to keep data and display in sync
    * @param [in] bSync true means synchronous refresh, immediately redrawing the current view; false means asynchronous refresh, where the refresh action is an asynchronous redraw that marks the dirty region and lets the system redraw the interface
    */
    virtual void Refresh(bool bSync = false) override;

    /** Get the width of the list control (the sum of the total widths of all Header columns)
    */
    int32_t GetListCtrlWidth() const;

    /** Set the index of the top element
    */
    void SetTopElementIndex(size_t nTopElementIndex);

    /** Get the index of the top element
    */
    virtual size_t GetTopElementIndex() const override;

    /** Set the list of currently displayed data items, in order from top to bottom
    * @param [in] itemIndexList The index list of the currently displayed data items
    */
    void SetDisplayDataItems(const std::vector<size_t>& itemIndexList);

    /** Get the list of currently displayed data items, in order from top to bottom
    * @param [in] itemIndexList The index list of the currently displayed data items
    */
    virtual void GetDisplayDataItems(std::vector<size_t>& itemIndexList) const override;

    /** Determine whether a data item is visible
    * @param [in] itemIndex The index of the data item
    */
    virtual bool IsDataItemDisplay(size_t itemIndex) const override;

    /** Ensure the data item is visible
    * @param [in] itemIndex The index of the data item
    * @param [in] bToTop Whether to ensure it is at the very top
    */
    virtual bool EnsureDataItemVisible(size_t itemIndex, bool bToTop) override;

public:
    /** Get the index of the first element within the visible range
    * @param [in] nScrollPosY The current Y scroll bar position
    * @return Returns the index of the element
    */
    size_t GetTopDataItemIndex(int64_t nScrollPosY) const;

    /** Get the height of the specified element (row height)
    * @param [in] itemIndex The index of the data item
    */
    int32_t GetDataItemHeight(size_t itemIndex) const;

    struct ShowItemInfo
    {
        size_t nItemIndex;      //Element index
        int32_t nItemHeight;    //Height of the element
    };

    /** Get the data to be displayed: pinned-to-top items first, and in order
    @param [in] nScrollPosY The current Y scroll bar position
    @param [in] maxCount The maximum number of records to take (including pinned-to-top and non-pinned-to-top)
    @param [out] itemIndexList Returns the indices of the elements to display (non-pinned-to-top)
    @param [out] atTopItemIndexList Returns the indices of the elements to display (pinned-to-top)
    @param [out] nPrevItemHeights The total height of all elements before the first visible element (not including pinned-to-top elements)
    */
    void GetDataItemsToShow(int64_t nScrollPosY, size_t maxCount, 
                            std::vector<ShowItemInfo>& itemIndexList,
                            std::vector<ShowItemInfo>& atTopItemIndexList,
                            int64_t& nPrevItemHeights) const;

    /** Get the maximum number of data items that can be displayed in a region of the specified height
    @param [in] nScrollPosY The current Y scroll bar position
    @param [in] nRectHeight The region height
    @param [out] pItemIndexList Returns the indices of the elements that can be displayed
    @param [out] pAtTopItemIndexList The indices of the pinned-to-top items
    @return Returns the number of data items that can be displayed
    */
    int32_t GetMaxDataItemsToShow(int64_t nScrollPosY, int32_t nRectHeight, 
                                  std::vector<size_t>* pItemIndexList = nullptr,
                                  std::vector<size_t>* pAtTopItemIndexList = nullptr) const;

    /** Get the total height of the display position of the specified element
    * @param [in] itemIndex The index of the data item
    * @parma [in] bIncludeAtTops Whether to include the height of pinned-to-top elements
    * @return The total height of the display position, not including the element itself
    */
    int64_t GetDataItemTotalHeights(size_t itemIndex, bool bIncludeAtTops) const;

    /** Listen for the Report view data item UI element fill event (virtual list, fills the data of a row)
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: The index of the associated UI container sub-item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it means no sub-item is associated
     *   lParam: The data item index corresponding to the associated UI container sub-item index, valid range: [0, GetDataItemCount()); if the value is Box::InvalidIndex, it means no data item is associated
     *   pEventData: The UI control interface pointer of the associated UI container sub-item, type: ListCtrlItem* pointer, data is filled by row
     *   pExtraData: The associated cell pointer, type: ListCtrlSubItem* pointer; if it is not nullptr, only this cell was filled; if it is nullptr, all cells of this row were filled
     */
    void AttachReportViewItemFilled(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventReportViewItemFilled, callback, callbackID); }

public:
    /** Whether it is normal mode (all row heights are the default row height, no hidden rows, no pinned-to-top rows)
    */
    bool IsNormalMode() const;

    /** Set the UI control indices to be pinned to top
    */
    void SetAtTopControlIndex(const std::vector<size_t>& atTopControlList);

    /** Adjust the number of UI controls to ensure enough data is displayed
    */
    void AjustItemCount();

public:
    /** Width of the horizontal grid line
    * @param [in] nLineWidth The width of the grid line; if it is 0, the horizontal grid line is not displayed
    * @param [in] bNeedDpiScale If true, the width needs to be DPI-adaptive
    */
    void SetRowGridLineWidth(int32_t nLineWidth, bool bNeedDpiScale);
    int32_t GetRowGridLineWidth() const;

    /** Color of the horizontal grid line
    * @param [in] color The color of the horizontal grid line
    */
    void SetRowGridLineColor(const std::string& color);
    std::string GetRowGridLineColor() const;

    /** Width of the vertical grid line
    * @param [in] nLineWidth The width of the grid line; if it is 0, the vertical grid line is not displayed
    * @param [in] bNeedDpiScale If true, the width needs to be DPI-adaptive
    */
    void SetColumnGridLineWidth(int32_t nLineWidth, bool bNeedDpiScale);
    int32_t GetColumnGridLineWidth() const;

    /** Color of the vertical grid line
    * @param [in] color The color of the vertical grid line
    */
    void SetColumnGridLineColor(const std::string& color);
    std::string GetColumnGridLineColor() const;

    /** Adjust the column widths: the 1st value in the array is the column index, and the 2nd value is the column width
    */
    void AdjustSubItemWidth(const std::map<size_t, int32_t>& subItemWidths);

    /** Refresh the data display of the specified cell (per-cell refresh)
    * @param [in] nElementIndex The index of the data element, valid range: [0, GetDataItemCount())
    * @param [in] nColumnId The column ID
    * @return Returns true when the update succeeded; returns false when a parameter is invalid or the cell is currently not in the visible range
    */
    bool RefreshSubItemData(size_t nElementIndex, size_t nColumnId);

protected:
    /** Draw child controls
    */
    virtual void PaintChild(IRender* pRender, const UiRect& rcPaint) override;

    /** Find a child control
    */
    virtual Control* FindControl(FINDCONTROLPROC Proc, void* pProcData, uint32_t uFlags,
                                 const UiPoint& ptMouse = UiPoint(),
                                 const UiPoint& scrollPos = UiPoint()) override;

    /** Get the current scroll bar offset (overrides ScrollBox::GetScrollOffset)
    *  While the frozen-column hit-test traversal is running (m_bFrozenHitTestPass is true), the horizontal
    *  component is counted as 0: frozen columns are fixed at the left of the viewport and do not move with
    *  horizontal scrolling, so their coordinate space has a horizontal offset of 0, which makes the
    *  GetScrollOffsetInScrollBox() of child controls consistent with the display position of frozen columns
    * @return Returns the scroll bar offset
    */
    virtual UiSize GetScrollOffset() const override;

    /** Get the scroll offset of the coordinate space that a child control (including descendant controls) belongs to
    *  (overrides ScrollBox::GetScrollOffsetForChild)
    *  Child controls inside the frozen-column area (cells of data rows, header cells and their descendants) are
    *  fixed at the left of the viewport and do not move with horizontal scrolling, so their horizontal offset is
    *  counted as 0; other child controls (such as the whole-row ListCtrlItem) keep the default behaviour
    * @param [in] pChild The child control whose coordinates are converted (may be a descendant control)
    * @return Returns the scroll offset corresponding to the coordinate space of the child control
    */
    virtual UiSize GetScrollOffsetForChild(const PlaceHolder* pChild) const override;

    /** Create a data item
    * @return Returns the pointer to the created data item
    */
    virtual Control* CreateDataItem() override;

    /** Fill the specified data item
    * @param [in] pControl The pointer to the data item control
    * @param [in] nElementIndex The index ID of the data element, range: [0, GetElementCount())
    * @param [in] itemData The data item (the attributes representing the row)
    * @param [in] subItemList The data sub-items (representing the data of each column; the 1st is the column ID, and the 2nd is the column data)
    */
    virtual bool FillDataItem(Control* pControl,
                              size_t nElementIndex,
                              const ListCtrlItemData& itemData,
                              const std::vector<ListCtrlSubItemData2Pair>& subItemList) override;


    /** Get the maximum width of a column
    * @param [in] subItemList The data sub-items (representing the data of each column)
    * @return Returns the maximum width of the column, which is the value after DPI adaptation; returns -1 on failure
    */
    virtual int32_t GetMaxDataItemWidth(const std::vector<const ListCtrlSubItemData2*>& subItemList) override;

    /** Calculate how many sub-items are displayed on this page
    * @param [in] bIsHorizontal Whether the current layout is a horizontal layout
    * @param [out] nColumns Returns the number of columns
    * @param [out] nRows Returns the number of rows
    * @return Returns the number of records displayed in the visible region
    */
    virtual size_t GetDisplayItemCount(bool bIsHorizontal, size_t& nColumns, size_t& nRows) const override;

    /** Determine whether a data element is selectable
    * @param [in] nElementIndex The element index, valid range: [0, GetElementCount())
    */
    virtual bool IsSelectableElement(size_t nElementIndex) const override;

    /** Get the next selectable data element
    * @param [in] nElementIndex The element index, valid range: [0, GetElementCount())
    * @param [in] bForward true means searching forward, false means searching backward
    */
    virtual size_t FindSelectableElement(size_t nElementIndex, bool bForward) const override;

    /** The selection state change event of a sub-item, used for state synchronization
    * @param [in] iIndex The ID of the sub-item, range: [0, GetItemCount())
    * @param [in] pListBoxItem The associated list item interface
    */
    virtual void OnItemSelectedChanged(size_t iIndex, IListBoxItem* pListBoxItem) override;

    /** The check state change event of a sub-item, used for state synchronization
    * @param [in] iIndex The ID of the sub-item, range: [0, GetItemCount())
    * @param [in] pListBoxItem The associated list item interface
    */
    virtual void OnItemCheckedChanged(size_t iIndex, IListBoxItem* pListBoxItem) override;

    /** The selection state has changed
    */
    virtual void OnSelectStatusChanged() override;

    /** Get the scroll amount of the scroll view
    */
    virtual void GetScrollDeltaValue(int32_t& nHScrollValue, int32_t& nVScrollValue) const override;

    /** A mouse frame selection operation has been performed
    * @param [in] left The X coordinate left value of the frame selection
    * @param [in] right The X coordinate right value of the frame selection
    * @param [in] top The Y coordinate top value of the frame selection
    * @param [in] bottom The Y coordinate bottom value of the frame selection
    * @return Returns true if there is a selection change, otherwise returns false
    */
    virtual bool OnFrameSelection(int64_t left, int64_t right, int64_t top, int64_t bottom) override;

    /** Whether it is currently allowed to paint the "row selected" background color (the row selected background
    *  is a visual exclusive to row-selection mode)
    *  Returns false in cell-selection mode: then no matter which entry point changes the row selection state
    *  (mouse/keyboard/API), the UI never shows a "row selected" visual, so the row selectable state is invisible
    *  in cell-selection mode
    */
    virtual bool CanPaintSelectedColors(bool bHasStateImages) const override;

    /** The left mouse button was clicked on a blank area of the view
    * @return Returns true if there is a selection change, otherwise returns false
    */
    virtual bool OnLButtonClickedBlank() override;

    /** The right mouse button was clicked on a blank area of the view
    * @return Returns true if there is a selection change, otherwise returns false
    */
    virtual bool OnRButtonClickedBlank() override;

    /** The Check state change of a data item (column level)
    * @param [in] nElementIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] nColumnId The column ID
    * @param [in] bChecked Whether it is checked
    */
    void OnSubItemColumnChecked(size_t nElementIndex, size_t nColumnId, bool bChecked);

    /** Determine whether a child control (including descendant controls) belongs to the "frozen column" area
    *  The check walks up to the cell that owns the control (cells of data rows are ListCtrlSubItem, header cells are
    *  ListCtrlHeaderItem), then converts the cell's column ID into a column index and compares it with the frozen
    *  column count; controls without an owning cell, such as a whole-row ListCtrlItem, return false
    * @param [in] pChild The child control to check (may be a descendant control)
    * @return Returns true if the control belongs to the frozen column area, otherwise returns false
    */
    bool IsChildInFrozenColumn(const PlaceHolder* pChild) const;

protected:
    /** The data content changed; the displayed data must be reloaded in the event. Subclasses can override this
    *  function to intercept the automatic UI refresh
    */
    virtual void OnModelDataChanged(size_t nStartElementIndex, size_t nEndElementIndex) override;

    /** The data item count changed; the displayed data must be reloaded in the event. Subclasses can override this
    *  function to intercept the automatic UI refresh
    */
    virtual void OnModelCountChanged() override;

private:
    /** Clear the element index binding of all data rows
    *  After clearing, these row controls must refill their data on the next layout (fill on demand: rows whose
    *  binding is still valid are not refilled).
    *  Used when "the binding between data elements and UI controls becomes invalid": structural refresh (sorting,
    *  inserting/deleting rows, row height, pinned-to-top rows, hidden rows, etc.), to avoid showing stale data
    *  when controls are reused
    */
    void InvalidateRowElementIndexes();

    /** Add a CheckBox to a cell and attach the check/uncheck events
    * @param [in] pSubItem The cell control
    * @param [in] nColumnId The column ID
    * @param [in] nElementIndex The index of the data item
    * @param [in] bChecked The checked state
    */
    void AddSubItemCheckBox(ListCtrlSubItem* pSubItem, size_t nColumnId, size_t nElementIndex, bool bChecked);

    /** Remove the CheckBox from a cell and detach the check/uncheck events
    */
    void RemoveSubItemCheckBox(ListCtrlSubItem* pSubItem);

    /** Draw grid lines
    */
    void PaintGridLines(IRender* pRender);

    /** Mouse drag-selection in cell-selection mode: select the cells inside the drag rectangle
    *  Note: the drag rectangle is passed in by ListBoxHelper (view client coordinates, the same convention as
    *        row mode); Ctrl+drag (m_bCellDragAdditive) uses additive semantics and only adds cells
    * @param [in] left The left X coordinate of the drag rectangle
    * @param [in] right The right X coordinate of the drag rectangle
    * @param [in] top The top Y coordinate of the drag rectangle
    * @param [in] bottom The bottom Y coordinate of the drag rectangle
    * @return Returns true if there is a selection change, otherwise returns false
    */
    bool OnFrameSelectionForCells(int64_t left, int64_t right, int64_t top, int64_t bottom);

    /** Hit test: map the mouse position (window client coordinates) to a cell (row display index + column ID)
    * @param [in] ptMouse The mouse position (window client coordinates)
    * @param [out] nRowIndex The display index of the row containing the hit cell; Box::InvalidIndex when nothing is hit
    * @param [out] nColumnId The column ID of the hit cell; Box::InvalidIndex when nothing is hit
    * @param [out] ppItem The data item (ListCtrlItem) containing the hit cell; nullptr when nothing is hit
    * @return Returns whether a cell was hit
    */
    bool HitTestCell(const UiPoint& ptMouse, size_t& nRowIndex, size_t& nColumnId, ListCtrlItem** ppItem = nullptr) const;

    /** Convert the mouse position to "sub-item control coordinates" (the same coordinate system as ListCtrlSubItem::GetRect())
    *  The horizontal part follows the frozen-column convention: when the mouse is inside the frozen area, frozen columns
    *  do not move with horizontal scrolling, so the horizontal scroll amount is counted as 0 (consistent with the
    *  frozen-column handling of the paint pass and FindControl, avoiding a mismatch between the hit column and the
    *  visual after horizontal scrolling)
    * @param [in] ptMouse The mouse position (window client coordinates)
    * @param [in] scrollOffsetInBox The scroll offset inside the scroll box (the same value as ListCtrlItem::GetSubItem)
    * @return Returns the converted coordinates
    */
    UiPoint MouseToCellSpace(const UiPoint& ptMouse, const UiPoint& scrollOffsetInBox) const;

    /** Determine whether the mouse point falls inside a "pinned-to-top row" (a non-selectable row)
    *  Pinned-to-top rows are fixed at the top and cover the normal rows scrolling underneath; when the mouse is
    *  over them it must not pass through and select the cells below
    * @param [in] ptMouse The mouse position (window client coordinates)
    * @return Returns whether the point falls inside a pinned-to-top row
    */
    bool IsPointOnAtTopItem(const UiPoint& ptMouse) const;

    /** Hit-test the list item containing the "pinned-to-top row" (a non-selectable row)
    * @param [in] ptMouse The mouse position (window client coordinates)
    * @return Returns the pinned-to-top list item hit by the mouse point; nullptr when nothing is hit
    */
    ListCtrlItem* HitTestTopItem(const UiPoint& ptMouse) const;

    /** Determine whether the mouse point (window client coordinates) falls on the "CheckBox icon of the list item itself"
    *  List items mean data rows (ListCtrlItem) and the header (ListCtrlHeader); their CheckBox is drawn by the control
    *  itself, and this is used in cell-selection mode to decide whether the mouse event should be let through so that
    *  the item completes the check/uncheck itself
    * @param [in] pSender The control that the mouse event belongs to (that is, EventArgs::GetSender())
    * @param [in] ptMouse The mouse position (window client coordinates)
    * @return Returns true if the mouse point falls on the CheckBox icon of the list item itself
    */
    bool IsMouseOnItemCheckBox(const Control* pSender, const UiPoint& ptMouse) const;

    /** Calculate the rectangle occupied by "the header and the pinned-to-top rows" (these rows are fixed at the top and
    *  cover the normal rows scrolling underneath)
    * @param [out] rcTopFixed Returns the rectangle occupied by the header and the pinned-to-top rows
    * @return Whether a header or pinned-to-top rows exist
    */
    bool GetTopFixedItemsRect(UiRect& rcTopFixed) const;

    /** Fill the data of a cell into the UI control (shared by FillDataItem and RefreshSubItemData, so the fill logic
    *  stays consistent)
    * @param [in] pSubItem The target sub-item control
    * @param [in] pStorage The cell data; may be null
    * @param [in] defaultSubItem The default attribute sub-item control
    * @param [in] pDefaultTextColors The cached array of default-state text colors (length kControlStateCount),
    *             calculated once per row by the caller and reused by all cells of that row, avoiding a repeated
    *             lookup for every cell
    * @param [in] nColumnId The column ID
    * @param [in] nElementIndex The index of the data element
    */
    void FillSubItemData(ListCtrlSubItem* pSubItem,
                         const ListCtrlSubItemData2* pStorage,
                         const ListCtrlSubItem& defaultSubItem,
                         const std::string* pDefaultTextColors,
                         size_t nColumnId,
                         size_t nElementIndex);

    /** Column layout table: describes the X range of each column (content coordinates, with the viewport left edge as the origin)
    *  It is derived from "the sub-item control geometry of the first visible data row"; when there is no visible data
    *  row it falls back to the header geometry.
    *  This is the single source of truth for "the X range of a column": the frozen-area width, and the later cell
    *  selection / merging / multi-level header capabilities are all derived from it. It is forbidden to accumulate
    *  column widths again in painting, hit testing or grid lines.
    *  Note: this table records columns in "visible order", and the coordinates are content coordinates (the scroll
    *  offset is not subtracted).
    */
    struct ListCtrlColumnLayout
    {
        /** Position information of a single column */
        struct Column
        {
            /** The column ID */
            size_t nColumnId = Box::InvalidIndex;

            /** The column ordinal (starting from 0, in visible order) */
            size_t nColumnIndex = 0;

            /** The X coordinate of the left border of the column (content coordinates, relative to the viewport left) */
            int32_t nXBegin = 0;

            /** The X coordinate of the right border of the column (content coordinates, relative to the viewport left) */
            int32_t nXEnd = 0;

            /** The width of the column */
            int32_t GetWidth() const { return nXEnd - nXBegin; }
        };

        /** Information of each column (arranged by column order) */
        std::vector<Column> m_columns;

        /** Get the column count */
        size_t GetColumnCount() const { return m_columns.size(); }

        /** Get the total width of the first N columns (that is, the right border X coordinate of this group of columns,
        *  relative to the viewport left)
        * @param [in] nColumnCount The column count; if it exceeds the actual column count, the actual column count is
        *            used (consistent with the historical behaviour)
        * @return Returns the width, or 0 when it cannot be calculated
        */
        int32_t GetColumnsWidth(size_t nColumnCount) const
        {
            if (m_columns.empty() || (nColumnCount == 0)) {
                return 0;
            }
            const size_t nIndex = (nColumnCount < m_columns.size()) ? (nColumnCount - 1) : (m_columns.size() - 1);
            const int32_t nWidth = m_columns[nIndex].nXEnd;
            return (nWidth > 0) ? nWidth : 0;
        }
    };

    /** Cell geometry layer: painting / hit testing / grid lines / frozen area / column X ranges are all derived from it
    *  Content coordinates: the coordinate system of a child control's GetRect(), which moves with the scroll offset
    *  Screen coordinates: the coordinate system of the final painting / mouse position (RenderOffset not included)
    *  Frozen columns do not move with horizontal scrolling, so their content coordinates equal their screen coordinates
    *  (this structure generalizes A1's FrozenTransform, of which frozen columns are a special case)
    */
    struct ListCtrlCellGeometry
    {
        /** Whether frozen columns exist */
        bool bHasFrozen = false;

        /** The number of frozen columns */
        size_t nFrozenCount = 0;

        /** The width of the frozen area (content coordinates equal screen coordinates) */
        int32_t nFrozenWidth = 0;

        /** The viewport rectangle (screen coordinates, padding not included) */
        UiRect rcViewport;

        /** The current scroll offset */
        UiSize scrollPos;

        /** The column layout table (the single source of truth for the X ranges of the columns) */
        ListCtrlColumnLayout columnLayout;

        /** The right border of the frozen area (screen X coordinate) */
        int32_t FrozenRight() const { return rcViewport.left + nFrozenWidth; }

        /** Whether the screen X coordinate falls inside the frozen area */
        bool IsFrozenX(int32_t screenX) const { return bHasFrozen && (screenX < FrozenRight()); }

        /** Grid line: derive the screen X from the "content X with the scroll offset already subtracted"
        *  (frozen columns do not move with horizontal scrolling) */
        int32_t GridLineScreenX(int32_t x, bool bFrozenColumn) const
        {
            return bFrozenColumn ? (x + scrollPos.cx) : x;
        }

        /** Paint pass: derive the clip rectangle (screen) + paint rectangle (screen) + window origin offset
        * @param [in] bFrozenPass true means the frozen-column paint pass, false means the scrollable-area paint pass
        * @param [out] rcClip The clip rectangle (screen coordinates)
        * @param [out] rcPaint The paint rectangle (screen coordinates)
        * @param [out] ptOffset The window origin offset
        */
        void GetPaintPass(bool bFrozenPass, UiRect& rcClip, UiRect& rcPaint, UiPoint& ptOffset) const
        {
            rcClip = rcViewport;
            rcPaint = rcViewport;
            if (bHasFrozen) {
                if (bFrozenPass) {
                    //Frozen-column pass: keep only the frozen area at the left of the viewport
                    rcClip.right = FrozenRight();
                    rcPaint.right = FrozenRight();
                }
                else {
                    //Scrollable-area pass: clip away the frozen area, and align the paint area's left border to the
                    //right side of the frozen area (it moves with horizontal scrolling)
                    rcClip.left = FrozenRight();
                    rcPaint.Offset(scrollPos.cx, scrollPos.cy);
                    rcPaint.left = FrozenRight() + scrollPos.cx;
                }
            }
            else {
                rcPaint.Offset(scrollPos.cx, scrollPos.cy);
            }
            //Frozen columns do not move with horizontal scrolling, so the horizontal window origin offset is 0
            ptOffset = bFrozenPass ? UiPoint(0, scrollPos.cy) : UiPoint(scrollPos.cx, scrollPos.cy);
        }
    };

    /** Build the cell geometry layer (derived once, reused by painting / hit testing / grid lines)
    */
    ListCtrlCellGeometry BuildCellGeometry() const;

    /** Build the column layout table: derive the X range of each column from the sub-item control geometry of the
    *  first visible data row; when there is no visible data row, fall back to the header geometry
    * @param [in] nViewLeft The X coordinate of the viewport left (the origin of the content coordinates)
    * @return Returns the built column layout table
    */
    ListCtrlColumnLayout BuildColumnLayout(int32_t nViewLeft) const;

    /** Paint the highlight of the cell selection (semi-transparent fill of the selected area + focus frame of the
    *  current cell)
    * @param [in] pRender The render interface
    * @param [in] cellGeometry The cell geometry layer
    * @param [in] bFrozenPass Whether to paint the frozen-column pass
    */
    void PaintCellSelection(IRender* pRender, const ListCtrlCellGeometry& cellGeometry, bool bFrozenPass);

    /** Move the header and pinned-to-top items to the end
    * @param [in,out] items The list of controls to move
    * @param [out] atTopItems Returns the list of pinned-to-top controls
    */
    void MoveTopItemsToLast(std::vector<Control*>& items, std::vector<Control*>& atTopItems) const;

    /** Give the data fill callback of the cell
    * @param [in] nListBoxIndex The index of the UI control container of this row in the interface, range: [0, GetItemCount())
    * @param [in] nElementIndex The index of the data element, valid range: [0, GetDataItemCount())
    * @param [in] pItem The UI container interface of the data item (corresponding to the row)
    * @param [in] pSubItem The UI container interface of the data item cell, corresponding to the cell. If it is nullptr,
    *            all cells of this row have had their data filled; if it is not nullptr, only this cell was filled by this event
    */
    void SendEventReportViewItemFilled(size_t nListBoxIndex, size_t nElementIndex,
                                       ListCtrlItem* pItem, ListCtrlSubItem* pSubItem);

private:
    /** ListCtrl control interface
    */
    ListCtrl* m_pListCtrl;

    /** Data interface
    */
    ListCtrlData* m_pData;

    /** The index of the top element (used for drawing grid lines)
    */
    size_t m_nTopElementIndex;

    /** The currently visible element list
    */
    std::vector<size_t> m_diplayItemIndexList;

    /** The UI control indices pinned to top
    */
    std::vector<size_t> m_atTopControlList;

private:
    /** Width of the horizontal grid line
    */
    int32_t m_nRowGridLineWidth;

    /** Color of the horizontal grid line
    */
    UiString m_rowGridLineColor;

    /** Width of the vertical grid line
    */
    int32_t m_nColumnGridLineWidth;

    /** Color of the vertical grid line
    */
    UiString m_columnGridLineColor;

    /** Frozen column hit-test traversal flag: when true, the horizontal scroll amount this control reports is counted
    *  as 0 (frozen columns do not move with horizontal scrolling)
    */
    bool m_bFrozenHitTestPass;
};

/** Layout management interface of the list data display control
*/
class DUI_API ListCtrlReportLayout : public Layout, public VirtualLayout
{
    typedef Layout BaseClass;
public:
    ListCtrlReportLayout();

    /** Layout type
    */
    virtual LayoutType GetLayoutType() const override { return LayoutType::ListCtrlReportLayout; }

    /** Adjust the position and size of all internal child controls according to the layout policy
     * @param [in] items The list of child controls
     * @param [in] rc The current container position and size information, including padding, but not including margin
     * @param [in] bEstimateOnly true means only estimating without adjusting the control positions, false means adjusting the control positions
     * @return Returns the width and height information of the final layout after arrangement, including the padding of the Box container, but not including the margin of the Box container itself (use this return value when the container supports a scroll bar)
     */
    virtual UiSize64 ArrangeChildren(const std::vector<Control*>& items, UiRect rc, bool bEstimateOnly = false) override;

    /** Estimate the container layout size based on the internal child control sizes (used to evaluate the size of controls whose width or height is "auto"; stretched child controls are not counted in the size estimation)
     * @param [in] items The list of child controls
     * @param [in] szAvailable The available width and height of the container, including the padding assigned to the container, but not including the margin assigned to the container
     * @return Returns the size information (width and height) of the final layout after arrangement, including the padding of the Box container itself, but not including the margin of the Box container itself;
     */
    virtual UiSize64 EstimateLayoutSize(const std::vector<Control*>& items, UiSize szAvailable) override;

public:
    /** Lazy-load and display data
    * @param [in] rc The current container size information; when called externally, the padding needs to be removed first
    */
    virtual void LazyArrangeChild(UiRect rc) const override;

    /** Get the maximum number of real data items to display (i.e., real data items that have corresponding Control objects)
    * @param [in] rc The current container size information; when called externally, the padding needs to be removed first
    */
    virtual size_t AjustMaxItem(UiRect rc) const override;

    /** Get the index of the previous element of the first element within the visible range
    * @param [in] rc The rectangle of the current display region, not including padding
    * @return Returns the index of the element
    */
    virtual size_t GetTopElementIndex(UiRect rc) const override;

    /** Determine whether an element is within the visible range
    * @param[in] iIndex The element index, range: [0, GetElementCount())
    * @param [in] rc The rectangle of the current display region, not including padding
    * @return Returns true if visible, otherwise not visible
    */
    virtual bool IsElementDisplay(UiRect rc, size_t iIndex) const override;

    /** Determine whether re-layout is needed
    */
    virtual bool NeedReArrange() const override;

    /** Get the data element indices of all currently visible controls
    * @param [in] rc The rectangle of the current display region, not including padding
    * @param[out] collection The index list, range: [0, GetElementCount())
    */
    virtual void GetDisplayElements(UiRect rc, std::vector<size_t>& collection) const override;

    /** Make the control within the visible range
    * @param [in] rc The rectangle of the current display region, not including padding
    * @param[in] iIndex The element index, range: [0, GetElementCount())
    * @param[in] bToTop Whether it is at the very top
    */
    virtual void EnsureVisible(UiRect rc, size_t iIndex, bool bToTop) const override;

public:
    /** Set the associated Box interface
    */
    void SetDataView(ListCtrlReportView* pDataView);

private:
    /** Get the associated Box interface
    */
    ListCtrlReportView* GetDataView() const { return m_pDataView; }

    /** Get the height of data items, not including the header
    * @param [in] nCount The number of data items; if it is Box::InvalidIndex, get the total height of all data items
    * @parma [in] bIncludeAtTops Whether to include the height of pinned-to-top elements
    * @return Returns the total height of nCount data items, not including the height of the header
    */
    int64_t GetElementsHeight(size_t nCount, bool bIncludeAtTops) const;

    /** Get the height and width of a data item
    * @param [in] rcWidth The current container width; when called externally, the padding needs to be removed first
    * @param [in] nElementIndex The index of the data element
    * @return Returns the height and width of the data element
    */
    UiSize GetElementSize(int32_t rcWidth, size_t nElementIndex) const;

    /** Get the row width
    */
    int32_t GetItemWidth() const;

    /** Get the row height (currently only supports the case where all rows have equal height)
    */
    int32_t GetItemHeight() const;

    /** Get the height of the header control
    */
    int32_t GetHeaderHeight() const;

    /** Lazy-load and display data (normal mode, same row heights, no hidden items, no pinned-to-top items)
    * @param [in] rc The current container size information; when called externally, the padding needs to be removed first
    */
    void LazyArrangeChildNormal(UiRect rc) const;

    /** Get the data element indices of all currently visible controls
    * @param [in] rc The rectangle of the current display region, not including padding
    * @param[out] collection The index list, range: [0, GetElementCount())
    */
    void GetDisplayElements(UiRect rc, std::vector<size_t>& collection, 
                            std::vector<size_t>* pAtTopItemIndexList) const;

    /** The reserved space at the bottom, ensuring the last data item is easy to view completely when scrolled to the very bottom
    */
    int32_t GetReserveHeight() const;

private:
    /** The associated ListBox interface
    */
    ListCtrlReportView* m_pDataView;

    /** Whether the bottom reserved space is set
    */
    bool m_bReserveSet;
};

}//namespace ui

#endif //UI_CONTROL_LIST_CTRL_REPORT_VIEW_H_
