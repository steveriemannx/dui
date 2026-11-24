#ifndef UI_CONTROL_TIMELINE_H_
#define UI_CONTROL_TIMELINE_H_

#include "dui/Core/Control.h"
#include <vector>
#include <functional>

namespace ui {

/** Timeline node status
*/
enum class TimelineNodeStatus
{
    kPending = 0,     // Pending/not started (gray)
    kInProgress = 1,  // In progress (accent color)
    kCompleted = 2,   // Completed (green)
    kFailed = 3,      // Failed (red)
    kCustom = 4       // Custom color (uses TimelineItem::customColor)
};

/** Timeline connector line style
*/
enum class TimelineLineStyle
{
    kSolid = 0,   // Solid
    kDashed = 1   // Dashed
};

/** Timeline control (inherits from Control, custom painted)
 *
 *  Vertical timeline: the nodes and the status connector line are on the left, the content (time/title/body) on the right; used to show logs, task flows and approval flows.
 *  Data-driven rendering, with support for normal/reverse order, node status colors, connector line style, node click events, empty data and a loading state.
 *
 *  XML attributes:
 *  - item_time / item_title / item_content / item_status / item_color: data bindings, multiple items separated by '|' (see the documentation)
 *  - item_time_id / item_title_id / item_content_id: multi-language data bindings, multiple STRIDs separated by '|' (refreshed automatically on a language change)
 *  - node_color_pending / node_color_inprogress / node_color_completed / node_color_failed: node color of each status
 *  - line_color: connector line color; line_style: connector line style (solid/dashed)
 *  - scrollbar_track_color: scroll bar track color, defaults to the semantic color bg_scrollbar_normal
 *  - scrollbar_thumb_color: scroll bar thumb color, defaults to the semantic color bg_scrollbar_thumb_normal
 *  - auto_hide_scrollbar: whether the scroll bar is shown only while the mouse hovers, defaults to true
 *  - reverse: whether to display in reverse order, defaults to false (normal order)
 *  - node_size: node diameter (pixels, DPI scaled), defaults to 12
 *  - item_spacing: spacing between adjacent items (pixels, DPI scaled), defaults to 24
 *  - empty_text / empty_text_id: empty data hint text / multi-language ID, empty by default
 *  - loading: whether the loading state is shown, defaults to false
 */
class DUI_API Timeline : public Control
{
    typedef Control BaseClass;

public:
    /** Timeline item
    */
    struct Item
    {
        std::string time;         // Time/date text
        std::string title;        // Title
        std::string content;      // Body content (supports line breaks)
        TimelineNodeStatus status = TimelineNodeStatus::kPending;
        std::string customColor;  // Custom node color (used when status == kCustom)
    };

    /** Custom item paint callback: returning true means the item was painted by the callback (the default painting is skipped)
    *   @param pRender The render interface
    *   @param item The item data
    *   @param rcItem The whole item area (already includes the scroll offset)
    *   @param rcNode The rectangle in which the node center lies
    *   @param index The item index
    */
    typedef std::function<bool(IRender* pRender, const Item& item, const UiRect& rcItem,
                               const UiRect& rcNode, size_t index)> ItemDrawCallback;

public:
    explicit Timeline(Window* pWindow);
    Timeline(const Timeline& r) = delete;
    Timeline& operator=(const Timeline& r) = delete;
    ~Timeline() override;

    /** Get the control type
    */
    std::string GetType() const override;

    /** Set an XML attribute
    */
    void SetAttribute(const std::string& strName, const std::string& strValue) override;

    /** Paint the timeline content
    */
    void PaintStateImages(IRender* pRender) override;

    /** Refresh the multi-language data bindings when the language changes
    */
    void OnLanguageChanged() override;

    // Mouse interaction: node click, scroll bar dragging, wheel scrolling
    bool ButtonDown(const EventArgs& msg) override;
    bool ButtonUp(const EventArgs& msg) override;
    bool MouseMove(const EventArgs& msg) override;
    bool MouseWheel(const EventArgs& msg) override;
    bool MouseEnter(const EventArgs& msg) override;
    bool MouseLeave(const EventArgs& msg) override;

public:
    /** Append an item
    */
    void AddItem(const Item& item);

    /** Append an item (convenience overload)
    */
    void AddItem(const std::string& time, const std::string& title, const std::string& content,
                 TimelineNodeStatus status = TimelineNodeStatus::kCompleted);

    /** Insert an item at the specified position
    */
    bool InsertItem(size_t index, const Item& item);

    /** Remove the specified item
    */
    bool RemoveItemAt(size_t index);

    /** Remove all items
    */
    void ClearItems();

    /** Set all items at once (replaces everything)
    */
    void SetItems(const std::vector<Item>& items);

    /** Get the item count
    */
    size_t GetItemCount() const { return m_items.size(); }

    /** Get the specified item
    */
    const Item* GetItemAt(size_t index) const;

    /** Get all items
    */
    const std::vector<Item>& GetItems() const { return m_items; }

    /** Set the node color of the specified status (semantic color name or color value)
    */
    void SetNodeColor(TimelineNodeStatus status, const std::string& strColor);

    /** Get the node color of the specified status
    */
    const std::string& GetNodeColor(TimelineNodeStatus status) const;

    /** Set the connector line color
    */
    void SetLineColor(const std::string& strColor);

    /** Get the connector line color
    */
    const std::string& GetLineColor() const { return m_strLineColor; }

    /** Set the scroll bar track color (semantic color name or color value)
    */
    void SetScrollbarTrackColor(const std::string& strColor);

    /** Get the scroll bar track color
    */
    const std::string& GetScrollbarTrackColor() const { return m_strScrollbarTrackColor; }

    /** Set the scroll bar thumb color (semantic color name or color value)
    */
    void SetScrollbarThumbColor(const std::string& strColor);

    /** Get the scroll bar thumb color
    */
    const std::string& GetScrollbarThumbColor() const { return m_strScrollbarThumbColor; }

    /** Set whether the scroll bar is shown only while the mouse hovers
    */
    void SetAutoHideScrollbar(bool bAutoHide);

    /** Get whether the scroll bar is shown only while the mouse hovers
    */
    bool IsAutoHideScrollbar() const { return m_bAutoHideScrollbar; }

    /** Set the connector line style
    */
    void SetLineStyle(TimelineLineStyle style);

    /** Set whether to display in reverse order
    */
    void SetReverse(bool bReverse);

    /** Get whether to display in reverse order
    */
    bool IsReverse() const { return m_bReverse; }

    /** Set the node diameter (pixels, DPI scaling is applied automatically)
    */
    void SetNodeSize(int32_t nSize);

    /** Set the spacing between adjacent items (pixels, DPI scaling is applied automatically)
    */
    void SetItemSpacing(int32_t nSpacing);

    /** Set the empty data hint text
    */
    void SetEmptyText(const std::string& strText);

    /** Set the empty data hint text (multi-language ID, refreshed automatically on a language change)
    */
    void SetEmptyTextId(const std::string& strTextId);

    /** Set whether to show the loading state
    */
    void SetLoading(bool bLoading);

    /** Get whether it is loading
    */
    bool IsLoading() const { return m_bLoading; }

    /** Set the custom item paint callback (overrides the default painting, set it to empty to restore the default)
    */
    void SetItemDrawCallback(const ItemDrawCallback& callback);

    /** Item click event callback (EventArgs::wParam is the item index)
    */
    void AttachItemClick(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Get the current scroll offset (pixels)
    */
    int32_t GetScrollPos() const { return m_nScrollPos; }

    /** Set the scroll offset (pixels, clamped automatically)
    */
    void SetScrollPos(int32_t nPos);

    /** Scroll to the specified item
    */
    void ScrollToItem(size_t index);

private:
    /** Compute the content area (excluding the node column width)
    */
    UiRect CalcContentRect(const UiRect& rcChart) const;

    /** Compute the height of a single item (pixels)
    *   @param pRender The render interface
    *   @param item The item
    *   @param nContentWidth The content area width (used to measure the body text wrapping)
    */
    int32_t CalcItemHeight(IRender* pRender, const Item& item, int32_t nContentWidth);

    /** Compute the total height of all items (including the spacing), and cache the item layout
    */
    int32_t CalcTotalHeight(IRender* pRender, const UiRect& rcContent);

    /** Hit test: determine the index of the node hit by the mouse position
    *   @return The index of the hit item, or -1 when nothing is hit
    */
    int32_t HitTestNode(IRender* pRender, const UiPoint& ptMouse);

    /** Update the scroll offset and clamp it
    */
    void UpdateScrollRange(IRender* pRender, const UiRect& rcChart);

    /** Paint a single item
    */
    void PaintItem(IRender* pRender, size_t index, const Item& item, const UiRect& rcItem,
                   const UiRect& rcContent, int32_t nNodeCenterY, bool bIsLast);

    /** Get the X coordinate of the node center (control coordinates)
    */
    int32_t GetNodeCenterX(const UiRect& rcChart) const;

    /** Fire the item click event
    */
    void SendItemClickEvent(size_t index);

    /** Compute the scroll bar thumb rectangle
    */
    UiRect CalcScrollThumbRect(const UiRect& rcChart) const;

    /** Paint the scroll bar (when the content overflows)
    */
    void DrawScrollbar(IRender* pRender, const UiRect& rcChart);

    /** Check whether the mouse hits the scroll bar thumb
    */
    bool HitTestScrollThumb(const UiPoint& ptMouse) const;

    /** Parse an XML data binding field (separated by '|')
    *   @param strValue The field value string
    *   @param nField The field number: 0 = time, 1 = title, 2 = content, 3 = status, 4 = color
    *   @param bTranslate Whether it is multi-language ID mode (each segment is fetched by STRID from the language file)
    */
    void ParseItemsField(const std::string& strValue, int32_t nField, bool bTranslate = false);

    /** Get the node color of an item (resolves the semantic color)
    */
    UiColor GetNodeUiColor(const Item& item) const;

    /** Paint the connector line (solid/dashed)
    */
    void DrawLine(IRender* pRender, int32_t x, int32_t y1, int32_t y2, UiColor color);

    /** Paint text (named DrawTimelineText, to avoid a conflict with the DrawText macro of the Windows SDK)
    */
    void DrawTimelineText(IRender* pRender, const std::string& strText, const UiRect& rc, uint32_t uFormat,
                          const std::string& strColor, const std::string& strFontId);

    /** Measure the text height (supports multi-line wrapping)
    */
    int32_t MeasureTextHeight(IRender* pRender, const std::string& strText, int32_t nWidth, const std::string& strFontId);

private:
    /** Item data
    */
    std::vector<Item> m_items;

    /** Node color of each status (semantic color names, indexed by TimelineNodeStatus)
    */
    std::string m_nodeColors[5];

    /** Connector line color
    */
    std::string m_strLineColor;

    /** Scroll bar track color (semantic color name or color value)
    */
    std::string m_strScrollbarTrackColor;

    /** Scroll bar thumb color (semantic color name or color value)
    */
    std::string m_strScrollbarThumbColor;

    /** Whether the scroll bar is shown only while the mouse hovers
    */
    bool m_bAutoHideScrollbar;

    /** Whether the mouse currently hovers over the control
    */
    bool m_bMouseHover;

    /** Connector line style
    */
    TimelineLineStyle m_lineStyle;

    /** Whether to display in reverse order
    */
    bool m_bReverse;

    /** Node diameter (pixels, the raw value before DPI scaling)
    */
    int32_t m_nNodeSize;

    /** Spacing between adjacent items (pixels, the raw value before DPI scaling)
    */
    int32_t m_nItemSpacing;

    /** Empty data hint text
    */
    std::string m_strEmptyText;

    /** Multi-language ID of the empty data hint text
    */
    std::string m_strEmptyTextId;

    /** Raw multi-language ID strings of the data bindings (item_time_id/item_title_id/item_content_id, re-resolved on a language change)
    */
    std::string m_strItemTimeIds;
    std::string m_strItemTitleIds;
    std::string m_strItemContentIds;

    /** Whether it is loading
    */
    bool m_bLoading;

    /** Current scroll offset (pixels)
    */
    int32_t m_nScrollPos;

    /** Total content height (pixels, the scroll range)
    */
    int32_t m_nContentHeight;

    /** Custom item paint callback
    */
    ItemDrawCallback m_itemDrawCallback;

    /** Whether the scroll bar thumb is being dragged
    */
    bool m_bDraggingScrollbar;

    /** Mouse Y where the scroll bar dragging started (control coordinates)
    */
    int32_t m_dragStartY;

    /** Scroll offset where the scroll bar dragging started
    */
    int32_t m_dragStartScrollPos;

    /** Scroll bar thumb rectangle (cached, used for hit testing and painting)
    */
    UiRect m_rcScrollThumb;
};

} // namespace ui

#endif // UI_CONTROL_TIMELINE_H_
