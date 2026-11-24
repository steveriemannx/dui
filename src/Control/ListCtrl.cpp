#include "dui/Control/ListCtrl.h"
#include "dui/Control/ListCtrlReportView.h"
#include "dui/Control/ListCtrlIconView.h"
#include "dui/Control/ListCtrlListView.h"
#include "dui/Control/RichEdit.h"
#include "dui/Control/DateTime.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/Keyboard.h"
#include "dui/Utils/Clipboard.h"
#include "dui/Utils/FileUtil.h"
#include "dui/Utils/StringConvert.h"
#include "dui/Utils/StringUtil.h"
#include "dui/Control/Calendar.h"
#include "dui/Control/Flyout.h"
#include "dui/Box/ListBox.h"
#include "dui/Box/ListBoxItem.h"
#include <set>
#include <algorithm>

namespace ui
{

/** ListCtrl cell combo candidate flyout (used by the kEditCombo editor)
*   Features:
*   1. Reuses Flyout (does not take the focus by default): the parent window stays active and the keyboard events of the list control keep being dispatched;
*      the up/down arrow keys, Return and Esc are handled in one place by ListCtrl::OnComboKeyDown (shared by row selection mode and cell selection mode)
*   2. Clicking a candidate item commits it immediately (the host is notified through a callback); clicking outside the flyout closes it automatically (treated as abandoning the edit)
*   3. Non-modal window: the object is deleted automatically by the framework after the window is closed, so the caller does not need to free it manually
*/
class ListCtrlComboFlyout : public Flyout
{
    typedef Flyout BaseClass;
public:
    /** Callback invoked when a single candidate item is selected (by mouse click); the parameter is the candidate item index (starting from 0)
    */
    typedef std::function<void(size_t nIndex)> ItemSelectEvent;

    /** Constructor
    * @param [in] pParentWindow The parent window (the anchor control must belong to this window)
    */
    explicit ListCtrlComboFlyout(Window* pParentWindow) :
        Flyout(pParentWindow),
        m_nSelectedIndex(Box::InvalidIndex),
        m_nMaxVisible(8),
        m_nGapDip(4),
        m_pList(nullptr),
        m_pContent(nullptr)
    {
        //Do not take the focus: the parent window stays active and the list control keeps receiving keyboard events
        SetNoFocus(true);
        //Close automatically when clicking outside the flyout
        SetAutoDismiss(true);
        //Always displayed below the anchor, without flipping up or down
        SetAllowFlip(false);
        SetGap(m_nGapDip);
    }
    ListCtrlComboFlyout(const ListCtrlComboFlyout& r) = delete;
    ListCtrlComboFlyout& operator=(const ListCtrlComboFlyout& r) = delete;

    /** Show the candidate flyout below the anchor control (the cell)
    * @param [in] pAnchor The anchor control (the text control of the hit cell)
    * @param [in] items The list of candidate item texts (must not be empty)
    * @param [in] nSelectedIndex The index of the initially selected item
    * @param [in] nWidthDip The width of the flyout (DIP, usually the cell width)
    * @param [in] placement The expected popup placement
    * @return Returns true if it was shown successfully; on failure the object has already been destroyed by the framework
    */
    bool ShowAt(Control* pAnchor, const std::vector<std::string>& items, size_t nSelectedIndex,
                int32_t nWidthDip, Placement placement = Placement::Bottom)
    {
        if ((pAnchor == nullptr) || items.empty() || (nWidthDip <= 0)) {
            delete this;
            return false;
        }
        m_items = items;
        m_nSelectedIndex = nSelectedIndex;
        m_nWidthDip = nWidthDip;

        const int32_t nRows = std::min<int32_t>(static_cast<int32_t>(m_items.size()), m_nMaxVisible);
        const int32_t nHeight = nRows * GetItemHeight();

        //Embedded skin XML: the Window size is the client area size without the shadow, and the framework appends the shadow automatically
        std::string sXml = StringUtil::Printf(
            "<Window size=\"%d,%d\" caption=\"0,0,0,0\" use_system_caption=\"false\" "
            "shadow_type=\"default\" shadow_attached=\"true\" layered_window=\"true\" size_box=\"0,0,0,0\">"
            "<VBox class=\"list_ctrl_combo_flyout\" name=\"list_ctrl_combo_content\" width=\"%d\" height=\"%d\">"
            "<VListBox class=\"list_ctrl_combo_list\" name=\"list_ctrl_combo_list\" "
            "width=\"stretch\" height=\"stretch\"/>"
            "</VBox></Window>",
            m_nWidthDip, nHeight, m_nWidthDip, nHeight);

        return Flyout::ShowAt(pAnchor, sXml, placement);
    }

    /** Get the index of the currently selected item; returns Box::InvalidIndex when nothing is selected
    */
    size_t GetSelectedIndex() const
    {
        if (m_pList != nullptr) {
            return m_pList->GetCurSel();
        }
        return (m_nSelectedIndex < m_items.size()) ? m_nSelectedIndex : Box::InvalidIndex;
    }

    /** Set the currently selected item (does not trigger the selection event)
    */
    void SetSelectedIndex(size_t nIndex)
    {
        if (nIndex >= m_items.size()) {
            return;
        }
        m_nSelectedIndex = nIndex;
        if (m_pList != nullptr) {
            m_pList->SelectItem(nIndex, false, false);
            m_pList->EnsureVisible(nIndex, ListBoxVerVisible::kVisible, ListBoxHorVisible::kVisible);
        }
    }

    /** Move the currently selected item (up/down arrow keys)
    * @param [in] nDelta +1 moves to the next item, -1 moves to the previous item; it stays at the boundary after reaching the first/last item
    */
    void MoveSelection(int32_t nDelta)
    {
        if (m_items.empty()) {
            return;
        }
        size_t nCurrent = GetSelectedIndex();
        if (nCurrent == Box::InvalidIndex) {
            SetSelectedIndex(nDelta > 0 ? 0 : (m_items.size() - 1));
            return;
        }
        int32_t nIndex = static_cast<int32_t>(nCurrent) + nDelta;
        if (nIndex < 0) {
            nIndex = 0;
        }
        if (nIndex >= static_cast<int32_t>(m_items.size())) {
            nIndex = static_cast<int32_t>(m_items.size()) - 1;
        }
        SetSelectedIndex(static_cast<size_t>(nIndex));
    }

    /** Get the list of candidate item texts
    */
    const std::vector<std::string>& GetItems() const { return m_items; }

    /** Listen to the candidate item selected event (mouse click)
    */
    void AttachItemSelect(const ItemSelectEvent& callback) { m_itemSelectCallbacks.push_back(callback); }

public:
    //WindowImplBase interfaces
    virtual void OnInitWindow() override
    {
        BaseClass::OnInitWindow();
        InitControls();
    }

private:
    /** Initialize the internal controls and bind the events
    */
    void InitControls()
    {
        ASSERT(m_pList == nullptr);
        if (m_pList != nullptr) {
            return;
        }
        m_pList = dynamic_cast<VListBox*>(FindControl("list_ctrl_combo_list"));
        ASSERT(m_pList != nullptr);
        if (m_pList == nullptr) {
            return;
        }
        //Content container: its height changes with the number of candidate rows
        m_pContent = dynamic_cast<Box*>(FindControl("list_ctrl_combo_content"));

        //Determine the initial height according to the number of candidate rows (including the container padding), so that no unnecessary scroll bar appears
        ApplyContentHeight();

        //Mouse click on a list item: confirm the selection
        m_pList->AttachClick([this](const EventArgs& msg) {
            const size_t nIndex = static_cast<size_t>(msg.wParam);
            if (nIndex < m_items.size()) {
                std::vector<ItemSelectEvent> callbacks = m_itemSelectCallbacks;
                for (const ItemSelectEvent& callback : callbacks) {
                    callback(nIndex);
                }
            }
            return true;
            });

        //Mouse hover over a list item: sync the highlight (consistent with the keyboard navigation state)
        m_pList->AttachItemMouseEnter([this](const EventArgs& msg) {
            const size_t nIndex = static_cast<size_t>(msg.wParam);
            if (nIndex < m_items.size()) {
                m_pList->SelectItem(nIndex, false, false);
            }
            return true;
            });

        RebuildList();
    }

    /** Build the list content according to m_items
    */
    void RebuildList()
    {
        ASSERT(m_pList != nullptr);
        if (m_pList == nullptr) {
            return;
        }
        m_pList->RemoveAllItems();
        for (const std::string& text : m_items) {
            ListBoxItem* pItem = new ListBoxItem(this);
            pItem->SetClass("list_ctrl_combo_item");
            pItem->SetText(text.c_str());
            pItem->SetFixedHeight(UiFixedInt(GetItemHeight()), true, true);
            m_pList->AddItem(pItem);
        }
        if (m_nSelectedIndex < m_items.size()) {
            m_pList->SelectItem(m_nSelectedIndex, false, false);
            m_pList->EnsureVisible(m_nSelectedIndex, ListBoxVerVisible::kVisible, ListBoxHorVisible::kVisible);
        }
    }

    /** Adjust the height of the content container (and of the root container) according to the current number of candidate rows
    *   Container height = row count * single row height + the container's own padding;
    *   ignoring the padding would leave the list with too little usable height and thus show a scroll bar by mistake
    */
    void ApplyContentHeight()
    {
        if (m_pContent == nullptr) {
            return;
        }
        const int32_t nItemHeight = Dpi().GetScaleInt(GetItemHeight());
        const int32_t nRows = std::min<int32_t>(static_cast<int32_t>(m_items.size()), m_nMaxVisible);

        const UiPadding rcPadding = m_pContent->GetPadding();
        const int32_t nContentHeight = nRows * nItemHeight + rcPadding.top + rcPadding.bottom;
        m_pContent->SetFixedHeight(UiFixedInt(nContentHeight), true, false);

        //The height of the root container (the shadow container) must be updated as well, otherwise the window size does not follow the number of candidate rows
        Box* pRoot = GetRoot();
        if ((pRoot != nullptr) && (pRoot != m_pContent)) {
            const UiPadding rcShadow = pRoot->GetPadding();
            pRoot->SetFixedHeight(UiFixedInt(nContentHeight + rcShadow.top + rcShadow.bottom), true, false);
        }
    }

    /** Calculate the height of a single list item (DIP)
    */
    int32_t GetItemHeight() const { return 28; }

private:
    /** Candidate item text
    */
    std::vector<std::string> m_items;

    /** Index of the currently selected item
    */
    size_t m_nSelectedIndex;

    /** Maximum number of visible rows
    */
    int32_t m_nMaxVisible;

    /** Width of the flyout (DIP)
    */
    int32_t m_nWidthDip;

    /** Spacing between the flyout and the anchor (DIP)
    */
    int32_t m_nGapDip;

    /** Internal candidate list control
    */
    VListBox* m_pList;

    /** Content container of the flyout (its height changes with the number of candidate rows)
    */
    Box* m_pContent;

    /** Candidate item selected callback
    */
    std::vector<ItemSelectEvent> m_itemSelectCallbacks;
};

ListCtrl::ListCtrl(Window* pWindow):
    VBox(pWindow),
    m_pHeaderCtrl(nullptr),
    m_pReportView(nullptr),
    m_pIconView(nullptr),
    m_pListView(nullptr),
    m_bEnableHeaderDragOrder(true),
    m_nFrozenColumnCount(0),
    m_bShowHeaderCtrl(true),
    m_bEnableRefresh(true),
    m_bMultiSelect(true),
    m_selectionMode(ListCtrlSelectionMode::kRow),
    m_bCellDragSelecting(false),
    m_hoverCell(),
    m_bEnableColumnWidthAuto(true),
    m_bAutoCheckSelect(false),
    m_bHeaderShowCheckBox(false),
    m_bDataItemShowCheckBox(false),
    m_listCtrlType(ListCtrlType::Report),
    m_pRichEdit(nullptr),
    m_pDateTime(nullptr),
    m_bEnableCopy(true),
    m_bEnableCsvExport(true),
    m_bEnableItemEdit(true),
    m_nItemHeight(0),
    m_nHeaderHeight(0),
    m_pData(nullptr),
    m_nSortedColumnId(Box::InvalidIndex),
    m_bSortedUp(false),
    m_pComboFlyout(nullptr)
{
    size_t nCount = sizeof(m_imageList) / sizeof(m_imageList[0]);
    for (size_t i = 0; i < nCount; ++i) {
        m_imageList[i] = nullptr;
    }

    m_pData = new ListCtrlData;
    m_pData->SetAutoCheckSelect(IsAutoCheckSelect());

    m_pReportView = new ListCtrlReportView(pWindow);
    m_pReportView->SetListCtrl(this);
    m_pReportView->SetDataProvider(m_pData);

    SetDataItemHeight(32, true);
    SetHeaderHeight(32, true);
}

ListCtrl::~ListCtrl()
{
    if (m_pData != nullptr) {
        delete m_pData;
        m_pData = nullptr;
    }
    if (!IsInited() && (m_pReportView != nullptr)) {
        delete m_pReportView;
        m_pReportView = nullptr;
    }
    if (!IsInited() && (m_pIconView != nullptr)) {
        delete m_pIconView;
        m_pIconView = nullptr;
    }
    if (!IsInited() && (m_pListView != nullptr)) {
        delete m_pListView;
        m_pListView = nullptr;
    }
    if (!IsInited() && (m_pRichEdit != nullptr)) {
        delete m_pRichEdit;
        m_pRichEdit = nullptr;
    }
    if (!IsInited() && (m_pDateTime != nullptr)) {
        delete m_pDateTime;
        m_pDateTime = nullptr;
    }
}

std::string ListCtrl::GetType() const { return DUI_CTR_LISTCTRL; }

void ListCtrl::SetImageList(ListCtrlType type, const ImageListPtr& spImageList)
{
    if (type == ListCtrlType::Report) {
        m_imageList[0] = spImageList;
    }
    else if (type == ListCtrlType::Icon) {
        m_imageList[1] = spImageList;
    }
    else if (type == ListCtrlType::List) {
        m_imageList[2] = spImageList;
    }
    else {
        ASSERT(0);
    }
}

ImageListPtr ListCtrl::GetImageList(ListCtrlType type) const
{
    if (type == ListCtrlType::Report) {
        return m_imageList[0];
    }
    else if (type == ListCtrlType::Icon) {
        return m_imageList[1];
    }
    else if (type == ListCtrlType::List) {
        return m_imageList[2];
    }
    else {
        return nullptr;
    }
}

void ListCtrl::SetAttribute(const std::string& strName, const std::string& strValue)
{
    if (strName == "header_class") {
        SetHeaderClass(strValue);
    }
    else if (strName == "header_item_class") {
        SetHeaderItemClass(strValue);
    }
    else if (strName == "header_split_box_class") {
        SetHeaderSplitBoxClass(strValue);
    }
    else if (strName == "header_split_control_class") {
        SetHeaderSplitControlClass(strValue);
    }
    else if (strName == "enable_header_drag_order") {
        SetEnableHeaderDragOrder(strValue == "true");
    }
    else if (strName == "frozen_column_count") {
        SetFrozenColumnCount((size_t)StringUtil::StringToInt32(strValue));
    }
    else if (strName == "check_box_class") {
        SetCheckBoxClass(strValue);
    }
    else if (strName == "data_item_class") {
        SetDataItemClass(strValue);
    }
    else if (strName == "data_sub_item_class") {
        SetDataSubItemClass(strValue);
    }
    else if (strName == "row_grid_line_width") {
        SetRowGridLineWidth(StringUtil::StringToInt32(strValue), true);
    }
    else if (strName == "row_grid_line_color") {
        SetRowGridLineColor(strValue);
    }
    else if (strName == "column_grid_line_width") {
        SetColumnGridLineWidth(StringUtil::StringToInt32(strValue), true);
    }
    else if (strName == "column_grid_line_color") {
        SetColumnGridLineColor(strValue);
    }
    else if (strName == "report_view_class") {
        SetReportViewClass(strValue);
    }
    else if (strName == "header_height") {
        SetHeaderHeight(StringUtil::StringToInt32(strValue), true);
    }
    else if (strName == "data_item_height") {
        SetDataItemHeight(StringUtil::StringToInt32(strValue), true);
    }
    else if (strName == "show_header") {
        SetHeaderVisible(strValue == "true");
    }
    else if (strName == "multi_select") {
        SetMultiSelect(strValue == "true");
    }
    else if (strName == "selection_mode") {
        if (strValue == "cell") {
            SetSelectionMode(ListCtrlSelectionMode::kCell);
        }
        else if (strValue == "row") {
            SetSelectionMode(ListCtrlSelectionMode::kRow);
        }
    }
    else if (strName == "enable_column_width_auto") {
        SetEnableColumnWidthAuto(strValue == "true");
    }
    else if (strName == "auto_check_select") {
        SetAutoCheckSelect(strValue == "true");
    }
    else if (strName == "show_header_checkbox") {
        SetHeaderShowCheckBox(strValue == "true");
    }
    else if (strName == "show_data_item_checkbox") {
        SetDataItemShowCheckBox(strValue == "true");
    }
    else if (strName == "type") {
        if (strValue == "report") {
            SetListCtrlType(ListCtrlType::Report);
        }
        else if (strValue == "icon") {
            SetListCtrlType(ListCtrlType::Icon);
        }
        else if (strValue == "list") {
            SetListCtrlType(ListCtrlType::List);
        }
    }
    else if (strName == "icon_view_class") {
        SetIconViewClass(strValue);
    }
    else if (strName == "icon_view_item_class") {
        SetIconViewItemClass(strValue);
    }
    else if (strName == "icon_view_item_image_class") {
        SetIconViewItemImageClass(strValue);
    }
    else if (strName == "icon_view_item_label_class") {
        SetIconViewItemLabelClass(strValue);
    }
    else if (strName == "list_view_class") {
        SetListViewClass(strValue);
    }
    else if (strName == "list_view_item_class") {
        SetListViewItemClass(strValue);
    }
    else if (strName == "list_view_item_image_class") {
        SetListViewItemImageClass(strValue);
    }
    else if (strName == "list_view_item_label_class") {
        SetListViewItemLabelClass(strValue);
    }
    else if (strName == "enable_item_edit") {
        SetEnableItemEdit(strValue == "true");
    }
    else if (strName == "enable_copy") {
        //dui has no StringUtil::IsValueTrue; booleans are compared against "true", as in the other dui controls
        SetEnableCopy(strValue == "true");
    }
    else if (strName == "enable_csv_export") {
        //dui has no StringUtil::IsValueTrue; booleans are compared against "true", as in the other dui controls
        SetEnableCsvExport(strValue == "true");
    }
    else if (strName == "list_ctrl_richedit_class") {
        SetRichEditClass(strValue);
    }
    else if (strName == "list_ctrl_datetime_class") {
        SetDateTimeClass(strValue);
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

void ListCtrl::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    if (!Dpi().CheckDisplayScaleFactor(nNewDpiScale)) {
        return;
    }

    //First disable the UI refresh
    bool bOldValue = SetEnableRefresh(false);
    int32_t iValue = GetRowGridLineWidth();
    iValue = Dpi().GetScaleInt(iValue, nOldDpiScale);
    SetRowGridLineWidth(iValue, false);

    iValue = GetColumnGridLineWidth();
    iValue = Dpi().GetScaleInt(iValue, nOldDpiScale);
    SetColumnGridLineWidth(iValue, false);

    if (m_nHeaderHeight > 0) {
        m_nHeaderHeight = Dpi().GetScaleInt(m_nHeaderHeight, nOldDpiScale);
    }

    iValue = GetDataItemHeight();
    iValue = Dpi().GetScaleInt(iValue, nOldDpiScale);
    SetDataItemHeight(iValue, false);

    if (m_pData != nullptr) {
        m_pData->ChangeDpiScale(Dpi(), nOldDpiScale);
    }

    size_t nCount = sizeof(m_imageList) / sizeof(m_imageList[0]);
    for (size_t i = 0; i < nCount; ++i) {
        if (m_imageList[i] != nullptr) {
            m_imageList[i]->ChangeDpiScale(Dpi(), nOldDpiScale);
        }
    }
    BaseClass::ChangeDpiScale(nOldDpiScale, nNewDpiScale);

    //Update the column width
    size_t nColumnCount = GetColumnCount();
    for (size_t columnIndex = 0; columnIndex < nColumnCount; ++columnIndex) {
        int32_t nColumnWidth = GetColumnWidth(columnIndex);
        if (nColumnWidth > 0) {
            SetColumnWidth(columnIndex, nColumnWidth, false);
        }        
    }
    //Refresh the data
    SetEnableRefresh(bOldValue);
    Refresh();
}

void ListCtrl::HandleEvent(const EventArgs& msg)
{
    BaseClass::HandleEvent(msg);
    if ((msg.eventType > kEventKeyBegin) && (msg.eventType < kEventKeyEnd)) {
        //Row selection mode: Ctrl+C copies the selected rows (TSV format), Ctrl+Shift+C copies them in CSV format
        //(in cell selection mode the event is handled and consumed by OnCellKeyDown, so it is not handled again here)
        if ((m_selectionMode == ListCtrlSelectionMode::kRow) &&
            (msg.eventType == kEventKeyDown) &&
            Keyboard::IsKeyDown(kVK_CONTROL) &&
            ((msg.vkCode == 'C') || (msg.vkCode == 'c'))) {
            if (CopySelectionToClipboard(Keyboard::IsKeyDown(kVK_SHIFT))) {
                //Already handled, do not forward it to the view any more
                return;
            }
        }
        OnViewKeyboardEvents(msg);
    }
    else if ((msg.eventType > kEventMouseBegin) && (msg.eventType < kEventMouseEnd)) {
        OnViewMouseEvents(msg);
    }
}

void ListCtrl::OnInit()
{
    if (IsInited()) {
        return;
    }
    BaseClass::OnInit();
    //Initialize the Body
    ASSERT(m_pReportView != nullptr);
    AddItem(m_pReportView);
    m_pReportView->SetClass(GetReportViewClass());

    //Initialize the Header
    ASSERT(m_pHeaderCtrl == nullptr);
    if (m_pHeaderCtrl == nullptr) {
        m_pHeaderCtrl = new ListCtrlHeader(GetWindow());
    }
    m_pHeaderCtrl->SetListCtrl(this);
    // Add the Header to the data view for management, as the first element, and control the display attributes in the Layout implementation
    m_pReportView->AddItem(m_pHeaderCtrl);

    if (!m_headerClass.empty()) {
        m_pHeaderCtrl->SetClass(m_headerClass.c_str());
    }
    m_pHeaderCtrl->SetAutoCheckSelect(false);
    m_pHeaderCtrl->SetShowCheckBox(m_bHeaderShowCheckBox); //Whether to show the CheckBox
    int32_t nHeaderHeight = m_nHeaderHeight;
    if (nHeaderHeight < 0) {
        nHeaderHeight = GetDataItemHeight();
    }
    m_pHeaderCtrl->SetFixedHeight(UiFixedInt(nHeaderHeight), true, false);
    if (!m_bShowHeaderCtrl) {
        SetHeaderVisible(false);
    }

    //Synchronize the single-select and multi-select states
    m_pReportView->SetDataProvider(m_pData);

    //Update the default text attributes
    SetDataSubItemClass(GetDataSubItemClass());

    //Update the default row height
    m_pData->SetDefaultItemHeight(GetDataItemHeight());

    InitReportView();
    DoSetListCtrlType(GetListCtrlType(), true);
}

void ListCtrl::InitReportView()
{
    if (m_pReportView == nullptr) {
        return;
    }
    m_pReportView->SetMultiSelect(IsMultiSelect());

    //Attach events and forward them to the outer layer
    m_pReportView->AttachSelect([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::Report, args);
        return true;
        });
    m_pReportView->AttachSelChanged([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::Report, args);
        return true;
        });
    m_pReportView->AttachItemMouseEnter([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::Report, args);
        return true;
        });
    m_pReportView->AttachItemMouseLeave([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::Report, args);
        return true;
        });
    m_pReportView->AttachDoubleClick([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::Report, args);
        return true;
        });
    m_pReportView->AttachClick([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::Report, args);
        return true;
        });
    m_pReportView->AttachRClick([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::Report, args);
        return true;
        });
    m_pReportView->AttachReturn([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::Report, args);
        return true;
        });
    m_pReportView->AttachKeyDown([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::Report, args);
        return true;
        });
    m_pReportView->AttachKeyUp([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::Report, args);
        return true;
        });

    m_pReportView->AttachReportViewItemFilled([this](const EventArgs& args) {
        EventArgs newMsg = args;
        newMsg.SetSender(this);
        SendEventMsg(newMsg);
        return true;
        });
    m_pReportView->AttachPosChanged([this](const EventArgs&) {
        EventArgs newMsg;
        newMsg.listCtrlType = (int32_t)ListCtrlType::Report;
        newMsg.pEventData = m_pReportView;
        SendEvent(kEventViewPosChanged, newMsg);
        return true;
        });
    m_pReportView->AttachSizeChanged([this](const EventArgs&) {
        EventArgs newMsg;
        newMsg.listCtrlType = (int32_t)ListCtrlType::Report;
        newMsg.pEventData = m_pReportView;
        SendEvent(kEventViewSizeChanged, newMsg);
        return true;
        });
}

void ListCtrl::InitIconView()
{
    if (m_pIconView == nullptr) {
        return;
    }
    m_pIconView->SetMultiSelect(IsMultiSelect());

    //Attach events and forward them to the outer layer
    m_pIconView->AttachSelect([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::Icon, args);
        return true;
        });
    m_pIconView->AttachSelChanged([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::Icon, args);
        return true;
        });
    m_pIconView->AttachItemMouseEnter([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::Icon, args);
        return true;
        });
    m_pIconView->AttachItemMouseLeave([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::Icon, args);
        return true;
        });
    m_pIconView->AttachDoubleClick([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::Icon, args);
        return true;
        });
    m_pIconView->AttachClick([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::Icon, args);
        return true;
        });
    m_pIconView->AttachRClick([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::Icon, args);
        return true;
        });
    m_pIconView->AttachReturn([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::Icon, args);
        return true;
        });
    m_pIconView->AttachKeyDown([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::Icon, args);
        return true;
        });
    m_pIconView->AttachKeyUp([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::Icon, args);
        return true;
        });

    m_pIconView->AttachIconViewItemFilled([this](const EventArgs& args) {
        EventArgs newMsg = args;
        newMsg.listCtrlType = (int32_t)ListCtrlType::Icon;
        newMsg.SetSender(this);
        SendEventMsg(newMsg);
        return true;
        });
    m_pIconView->AttachPosChanged([this](const EventArgs&) {
        EventArgs newMsg;
        newMsg.listCtrlType = (int32_t)ListCtrlType::Icon;
        newMsg.pEventData = m_pIconView;
        SendEvent(kEventViewPosChanged, newMsg);
        return true;
        });
    m_pIconView->AttachSizeChanged([this](const EventArgs&) {
        EventArgs newMsg;
        newMsg.listCtrlType = (int32_t)ListCtrlType::Icon;
        newMsg.pEventData = m_pIconView;
        SendEvent(kEventViewSizeChanged, newMsg);
        return true;
        });
}

void ListCtrl::InitListView()
{
    if (m_pListView == nullptr) {
        return;
    }
    if (m_pIconView != nullptr) {
        m_pIconView->SetMultiSelect(IsMultiSelect());
    }

    //Attach events and forward them to the outer layer
    m_pListView->AttachSelect([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::List, args);
        return true;
        });
    m_pListView->AttachSelChanged([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::List, args);
        return true;
        });
    m_pListView->AttachItemMouseEnter([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::List, args);
        return true;
        });
    m_pListView->AttachItemMouseLeave([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::List, args);
        return true;
        });
    m_pListView->AttachDoubleClick([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::List, args);
        return true;
        });
    m_pListView->AttachClick([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::List, args);
        return true;
        });
    m_pListView->AttachRClick([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::List, args);
        return true;
        });
    m_pListView->AttachReturn([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::List, args);
        return true;
        });
    m_pListView->AttachKeyDown([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::List, args);
        return true;
        });
    m_pListView->AttachKeyUp([this](const EventArgs& args) {
        OnListCtrlViewEvent(ListCtrlType::List, args);
        return true;
        });

    m_pListView->AttachListViewItemFilled([this](const EventArgs& args) {
        EventArgs newMsg = args;
        newMsg.listCtrlType = (int32_t)ListCtrlType::List;
        newMsg.SetSender(this);
        SendEventMsg(newMsg);
        return true;
        });
    m_pListView->AttachPosChanged([this](const EventArgs&) {
        EventArgs newMsg;
        newMsg.listCtrlType = (int32_t)ListCtrlType::List;
        newMsg.pEventData = m_pListView;
        SendEvent(kEventViewPosChanged, newMsg);
        return true;
        });
    m_pListView->AttachSizeChanged([this](const EventArgs&) {
        EventArgs newMsg;
        newMsg.listCtrlType = (int32_t)ListCtrlType::List;
        newMsg.pEventData = m_pListView;
        SendEvent(kEventViewSizeChanged, newMsg);
        return true;
        });
}

void ListCtrl::OnListCtrlViewEvent(ListCtrlType listCtrlType, const EventArgs& args)
{
    EventArgs msg = args;
    msg.SetSender(this);
    msg.listCtrlType = (int32_t)listCtrlType;//List view type
    msg.pEventData = nullptr;

    if (args.eventType != kEventSelChanged) {
        if (listCtrlType == ListCtrlType::Report) {
            size_t nItemIndex = args.wParam;
            Control* pControl = m_pReportView->GetItemAt(nItemIndex);
            ListCtrlItem* pItem = nullptr;
            if (pControl != nullptr) {
                pItem = dynamic_cast<ListCtrlItem*>(pControl);
            }
            if ((pItem == nullptr) && (nItemIndex == 0)) {
                if (dynamic_cast<ListCtrlHeader*>(pControl) != nullptr) {
                    //Do not trigger the event when in the header
                    return;
                }
            }
            if (nItemIndex != Box::InvalidIndex) {
                ASSERT(pItem != nullptr);
            }
            if (pItem != nullptr) {
                ASSERT(pItem->GetDataItemIndex() == (size_t)msg.lParam);
                msg.pEventData = pItem;
            }
        }
        else if (listCtrlType == ListCtrlType::Icon) {
            size_t nItemIndex = args.wParam;
            Control* pControl = m_pIconView->GetItemAt(nItemIndex);
            ListCtrlIconViewItem* pItem = nullptr;
            if (pControl != nullptr) {
                pItem = dynamic_cast<ListCtrlIconViewItem*>(pControl);
            }
            if (nItemIndex != Box::InvalidIndex) {
                ASSERT(pItem != nullptr);
            }
            if (pItem != nullptr) {
                ASSERT(pItem->GetDataItemIndex() == (size_t)msg.lParam);
                msg.pEventData = pItem;
            }
        }
        else if (listCtrlType == ListCtrlType::List) {
            size_t nItemIndex = args.wParam;
            Control* pControl = m_pListView->GetItemAt(nItemIndex);
            ListCtrlListViewItem* pItem = nullptr;
            if (pControl != nullptr) {
                pItem = dynamic_cast<ListCtrlListViewItem*>(pControl);
            }
            if (nItemIndex != Box::InvalidIndex) {
                ASSERT(pItem != nullptr);
            }
            if (pItem != nullptr) {
                ASSERT(pItem->GetDataItemIndex() == (size_t)msg.lParam);
                msg.pEventData = pItem;
            }
        }
        else {
            ASSERT(0);
            return;
        }
    }

    //Trigger the event to the application layer
    SendEventMsg(msg);
}

void ListCtrl::SetListCtrlType(ListCtrlType type)
{
    DoSetListCtrlType(type, false);
}

void ListCtrl::DoSetListCtrlType(ListCtrlType type, bool bInit)
{
    ListCtrlType oldListCtrlType = m_listCtrlType;
    m_listCtrlType = type;
    if (!IsInited()) {
        return;
    }
    void* pEventData = nullptr;
    if (m_listCtrlType == ListCtrlType::Report) {
        //Switch to the Report view
        ASSERT(m_pReportView != nullptr);
        if (m_pReportView != nullptr) {
            m_pReportView->SetDataProvider(m_pData);
            m_pData->SetListView(m_pReportView);
            m_pReportView->SetVisible(true);
            if (m_pReportView->IsMultiSelect() != IsMultiSelect()) {
                m_pReportView->SetMultiSelect(IsMultiSelect());
            }
        }
        if (m_pIconView != nullptr) {
            m_pIconView->SetVisible(false);
            m_pIconView->SetDataProvider(nullptr);            
        }
        if (m_pListView != nullptr) {
            m_pListView->SetVisible(false);
            m_pListView->SetDataProvider(nullptr);
        }
        pEventData = m_pReportView;
    }
    else if (m_listCtrlType == ListCtrlType::Icon) {
        //Switch to the Icon view
        if (m_pIconView == nullptr) {
            m_pIconView = new ListCtrlIconView(GetWindow());
            m_pIconView->SetListCtrl(this);
            AddItem(m_pIconView);
            m_pIconView->SetClass(GetIconViewClass());
            InitIconView();
        }
        m_pIconView->SetDataProvider(m_pData);
        m_pData->SetListView(m_pIconView);
        m_pIconView->SetVisible(true);
        if (m_pIconView->IsMultiSelect() != IsMultiSelect()) {
            m_pIconView->SetMultiSelect(IsMultiSelect());
        }
        if (m_pReportView != nullptr) {
            m_pReportView->SetVisible(false);
            m_pReportView->SetDataProvider(nullptr);
        }
        if (m_pListView != nullptr) {
            m_pListView->SetVisible(false);
            m_pListView->SetDataProvider(nullptr);
        }
        pEventData = m_pIconView;
    }
    else if (m_listCtrlType == ListCtrlType::List) {
        //Switch to the List view
        if (m_pListView == nullptr) {
            m_pListView = new ListCtrlListView(GetWindow());
            m_pListView->SetListCtrl(this);
            AddItem(m_pListView);
            m_pListView->SetClass(GetListViewClass());
            InitListView();
        }
        m_pListView->SetDataProvider(m_pData);
        m_pData->SetListView(m_pListView);
        m_pListView->SetVisible(true);
        if (m_pListView->IsMultiSelect() != IsMultiSelect()) {
            m_pListView->SetMultiSelect(IsMultiSelect());
        }
        if (m_pReportView != nullptr) {
            m_pReportView->SetVisible(false);
            m_pReportView->SetDataProvider(nullptr);
        }
        if (m_pIconView != nullptr) {
            m_pIconView->SetVisible(false);
            m_pIconView->SetDataProvider(nullptr);
        }
        pEventData = m_pListView;
    }

    //Trigger the view type change event
    ListCtrlType newListCtrlType = m_listCtrlType;
    if (bInit || (oldListCtrlType != newListCtrlType)) {
        EventArgs msg;
        msg.wParam = (WPARAM)newListCtrlType;
        msg.lParam = (LPARAM)oldListCtrlType;
        msg.pEventData = pEventData;
        msg.listCtrlType = (int32_t)newListCtrlType;
        SendEvent(kEventViewTypeChanged, msg);
    }
}

ListCtrlType ListCtrl::GetListCtrlType() const
{
    return m_listCtrlType;
}

ListCtrlReportView* ListCtrl::GetReportView() const
{
    return m_pReportView;
}

ListCtrlIconView* ListCtrl::GetIconView() const
{
    return m_pIconView;
}

ListCtrlListView* ListCtrl::GetListView() const
{
    return m_pListView;
}

void ListCtrl::SetHeaderClass(const std::string& className)
{
    m_headerClass = className;
    if (IsInited() && (m_pHeaderCtrl != nullptr)) {
        m_pHeaderCtrl->SetClass(className);
    }
}

void ListCtrl::SetHeaderItemClass(const std::string& className)
{
    m_headerItemClass = className;
}

std::string ListCtrl::GetHeaderItemClass() const
{
    return m_headerItemClass.c_str();
}

void ListCtrl::SetHeaderSplitBoxClass(const std::string& className)
{
    m_headerSplitBoxClass = className;
}

std::string ListCtrl::GetHeaderSplitBoxClass() const
{
    return m_headerSplitBoxClass.c_str();
}

void ListCtrl::SetHeaderSplitControlClass(const std::string& className)
{
    m_headerSplitControlClass = className;
}

std::string ListCtrl::GetHeaderSplitControlClass() const
{
    return m_headerSplitControlClass.c_str();
}

void ListCtrl::SetCheckBoxClass(const std::string& className)
{
    m_checkBoxClass = className;
}

std::string ListCtrl::GetCheckBoxClass() const
{
    return m_checkBoxClass.c_str();
}

void ListCtrl::SetReportViewClass(const std::string& className)
{
    m_reportViewClass = className;
    if (IsInited() && (m_pReportView != nullptr)) {
        m_pReportView->SetClass(className);
    }
}

std::string ListCtrl::GetReportViewClass() const
{
    return m_reportViewClass.c_str();
}

void ListCtrl::SetDataItemClass(const std::string& className)
{
    m_dataItemClass = className;
}

std::string ListCtrl::GetDataItemClass() const
{
    return m_dataItemClass.c_str();
}

void ListCtrl::SetDataSubItemClass(const std::string& className)
{
    m_dataSubItemClass = className;
    if (IsInited() && !className.empty()) {
        ListCtrlSubItem defaultSubItem(GetWindow());
        defaultSubItem.SetClass(className);
        m_pData->SetDefaultTextStyle(defaultSubItem.GetTextStyle());
    }
}

std::string ListCtrl::GetDataSubItemClass() const
{
    return m_dataSubItemClass.c_str();
}

void ListCtrl::SetIconViewClass(const std::string& className)
{
    m_iconViewClass = className;
    if (IsInited() && (m_pIconView != nullptr)) {
        m_pIconView->SetClass(className);
    }
}

std::string ListCtrl::GetIconViewClass() const
{
    return m_iconViewClass.c_str();
}

void ListCtrl::SetIconViewItemClass(const std::string& className)
{
    m_iconViewItemClass = className;
}

std::string ListCtrl::GetIconViewItemClass() const
{
    return m_iconViewItemClass.c_str();
}

void ListCtrl::SetIconViewItemImageClass(const std::string& className)
{
    m_iconViewItemImageClass = className;
}

std::string ListCtrl::GetIconViewItemImageClass() const
{
    return m_iconViewItemImageClass.c_str();
}

void ListCtrl::SetIconViewItemLabelClass(const std::string& className)
{
    m_iconViewItemLabelClass = className;
}

std::string ListCtrl::GetIconViewItemLabelClass() const
{
    return m_iconViewItemLabelClass.c_str();
}

void ListCtrl::SetListViewClass(const std::string& className)
{
    m_listViewClass = className;
    if (IsInited() && (m_pListView != nullptr)) {
        m_pListView->SetClass(className);
    }
}

std::string ListCtrl::GetListViewClass() const
{
    return m_listViewClass.c_str();
}

void ListCtrl::SetListViewItemClass(const std::string& className)
{
    m_listViewItemClass = className;
}

std::string ListCtrl::GetListViewItemClass() const
{
    return m_listViewItemClass.c_str();
}

void ListCtrl::SetListViewItemImageClass(const std::string& className)
{
    m_listViewItemImageClass = className;
}

std::string ListCtrl::GetListViewItemImageClass() const
{
    return m_listViewItemImageClass.c_str();
}

void ListCtrl::SetListViewItemLabelClass(const std::string& className)
{
    m_listViewItemLabelClass = className;
}

std::string ListCtrl::GetListViewItemLabelClass() const
{
    return m_listViewItemLabelClass.c_str();
}

void ListCtrl::SetRichEditClass(const std::string& richEditClass)
{
    if (m_listCtrlRichEditClass != richEditClass) {
        m_listCtrlRichEditClass = richEditClass;
        if (IsInited() && (m_pRichEdit != nullptr)) {
            m_pRichEdit->SetClass(richEditClass);
        }
    }
}

void ListCtrl::SetDateTimeClass(const std::string& dateTimeClass)
{
    if (m_listCtrlDateTimeClass != dateTimeClass) {
        m_listCtrlDateTimeClass = dateTimeClass;
        if (IsInited() && (m_pDateTime != nullptr)) {
            m_pDateTime->SetClass(dateTimeClass);
        }
    }
}

void ListCtrl::SetEnableItemEdit(bool bEnableItemEdit)
{
    m_bEnableItemEdit = bEnableItemEdit;
}

bool ListCtrl::IsEnableItemEdit() const
{
    return m_bEnableItemEdit;
}

void ListCtrl::SetEnableCopy(bool bEnableCopy)
{
    m_bEnableCopy = bEnableCopy;
}

bool ListCtrl::IsEnableCopy() const
{
    return m_bEnableCopy;
}

void ListCtrl::SetEnableCsvExport(bool bEnableCsvExport)
{
    m_bEnableCsvExport = bEnableCsvExport;
}

bool ListCtrl::IsEnableCsvExport() const
{
    return m_bEnableCsvExport;
}

std::string ListCtrl::EscapeText(const std::string& strText, bool bCsvFormat)
{
    if (!bCsvFormat) {
        //TSV format: tabs/returns/newlines inside a cell would break the row/column structure, so they are all replaced with spaces
        std::string strResult = strText;
        for (std::string::value_type& ch : strResult) {
            if ((ch == '\t') || (ch == '\r') || (ch == '\n')) {
                ch = ' ';
            }
        }
        return strResult;
    }

    //CSV format (RFC4180): a field is wrapped in double quotes only when it contains a comma/double quote/return/newline
    const bool bNeedQuote = (strText.find(',') != std::string::npos) ||
                            (strText.find('"') != std::string::npos) ||
                            (strText.find('\r') != std::string::npos) ||
                            (strText.find('\n') != std::string::npos);
    if (!bNeedQuote) {
        return strText;
    }
    std::string strResult;
    strResult += '"';
    for (const std::string::value_type ch : strText) {
        if (ch == '"') {
            //Double quotes inside a field must be doubled
            strResult += '"';
        }
        strResult += ch;
    }
    strResult += '"';
    return strResult;
}

std::string ListCtrl::BuildSelectionText(bool bCsvFormat, bool bSelectedOnly) const
{
    const size_t nColumnCount = GetColumnCount();
    if ((nColumnCount == 0) || (GetDataItemCount() == 0)) {
        return std::string();
    }

    //The columns to output (all valid columns in the current column order; the header row is not included when exporting the whole table)
    std::vector<size_t> columnIdList;
    columnIdList.reserve(nColumnCount);
    for (size_t nColumnIndex = 0; nColumnIndex < nColumnCount; ++nColumnIndex) {
        const size_t nColumnId = GetColumnId(nColumnIndex);
        if (IsValidColumnId(nColumnId)) {
            columnIdList.push_back(nColumnId);
        }
    }
    if (columnIdList.empty()) {
        return std::string();
    }

    //Row output range (inclusive) and cell participation decision
    std::vector<size_t> rowIndexList;          //The rows to output (display index)
    bool bCellMode = false;                    //Whether to output according to the "bounding rectangle" of the cell area
    struct SelectedRect
    {
        size_t nRowBegin;       //Start row (display index)
        size_t nRowEnd;         //End row (display index)
        size_t nColumnBegin;    //Start column (column order)
        size_t nColumnEnd;      //End column (column order)
    };
    std::vector<SelectedRect> rectList;        //The selected areas in kCell mode (already converted to column order)
    size_t nRowBegin = 0;
    size_t nRowEnd = 0;
    size_t nColumnBegin = 0;
    size_t nColumnEnd = 0;

    if (!bSelectedOnly) {
        //Export the whole table: output all data rows
        const size_t nDataItemCount = GetDataItemCount();
        rowIndexList.reserve(nDataItemCount);
        for (size_t nRowIndex = 0; nRowIndex < nDataItemCount; ++nRowIndex) {
            rowIndexList.push_back(nRowIndex);
        }
    }
    else if (m_selectionMode == ListCtrlSelectionMode::kRow) {
        //Row selection mode: output the selected rows
        GetSelectedDataItems(rowIndexList);
        std::sort(rowIndexList.begin(), rowIndexList.end());
        rowIndexList.erase(std::unique(rowIndexList.begin(), rowIndexList.end()), rowIndexList.end());
    }
    else if (m_selectionMode == ListCtrlSelectionMode::kCell) {
        //Cell selection mode: take the bounding rectangle of the union of all selected areas; cells inside the rectangle that are not selected are output as empty strings
        std::vector<ListCtrlCellRange> rangeList;
        GetSelectedCellRanges(rangeList);
        bool bHasRect = false;
        for (const ListCtrlCellRange& range : rangeList) {
            if (!range.IsValid()) {
                continue;
            }
            const size_t nColumnIndexBegin = GetColumnIndex(range.nColumnIdBegin);
            const size_t nColumnIndexEnd = GetColumnIndex(range.nColumnIdEnd);
            if (!Box::IsValidItemIndex(nColumnIndexBegin) || !Box::IsValidItemIndex(nColumnIndexEnd)) {
                continue;
            }
            SelectedRect rect;
            rect.nRowBegin = range.nRowBegin;
            rect.nRowEnd = range.nRowEnd;
            rect.nColumnBegin = (nColumnIndexBegin < nColumnIndexEnd) ? nColumnIndexBegin : nColumnIndexEnd;
            rect.nColumnEnd = (nColumnIndexBegin < nColumnIndexEnd) ? nColumnIndexEnd : nColumnIndexBegin;
            rectList.push_back(rect);

            if (!bHasRect) {
                bHasRect = true;
                nRowBegin = rect.nRowBegin;
                nRowEnd = rect.nRowEnd;
                nColumnBegin = rect.nColumnBegin;
                nColumnEnd = rect.nColumnEnd;
            }
            else {
                if (rect.nRowBegin < nRowBegin) { nRowBegin = rect.nRowBegin; }
                if (rect.nRowEnd > nRowEnd) { nRowEnd = rect.nRowEnd; }
                if (rect.nColumnBegin < nColumnBegin) { nColumnBegin = rect.nColumnBegin; }
                if (rect.nColumnEnd > nColumnEnd) { nColumnEnd = rect.nColumnEnd; }
            }
        }
        if (!bHasRect) {
            return std::string();
        }
        bCellMode = true;
        for (size_t nRowIndex = nRowBegin; nRowIndex <= nRowEnd; ++nRowIndex) {
            rowIndexList.push_back(nRowIndex);
        }
        //The output range of the columns is limited to the bounding rectangle (the column order interval)
        columnIdList.clear();
        for (size_t nColumnIndex = nColumnBegin; (nColumnIndex <= nColumnEnd) && (nColumnIndex < nColumnCount); ++nColumnIndex) {
            const size_t nColumnId = GetColumnId(nColumnIndex);
            if (IsValidColumnId(nColumnId)) {
                columnIdList.push_back(nColumnId);
            }
        }
    }

    if (rowIndexList.empty() || columnIdList.empty()) {
        return std::string();
    }

    //Determine whether a cell falls inside a selected area (used only in kCell mode; columnIndex is the index into columnIdList)
    auto isCellSelected = [&rectList, nColumnBegin](size_t nRowIndex, size_t nColumnIndex) -> bool {
        const size_t nColumnOrder = nColumnBegin + nColumnIndex;
        for (const SelectedRect& rect : rectList) {
            if ((nRowIndex >= rect.nRowBegin) && (nRowIndex <= rect.nRowEnd) &&
                (nColumnOrder >= rect.nColumnBegin) && (nColumnOrder <= rect.nColumnEnd)) {
                return true;
            }
        }
        return false;
    };

    const std::string strCellSep = bCsvFormat ? "," : "\t";
    const std::string strLineSep = "\r\n";
    const size_t nDataItemCount = GetDataItemCount();

    std::string strResult;
    bool bFirstLine = true;
    for (const size_t nRowIndex : rowIndexList) {
        if (nRowIndex >= nDataItemCount) {
            continue;
        }
        std::string strLine;
        for (size_t nColumnIndex = 0; nColumnIndex < columnIdList.size(); ++nColumnIndex) {
            if (nColumnIndex > 0) {
                strLine += strCellSep;
            }
            if (bCellMode && !isCellSelected(nRowIndex, nColumnIndex)) {
                //Unselected cells are output as empty strings, so that the rows and columns stay aligned and the content does not shift
                continue;
            }
            //Note: cell merging (spanning rows/columns, originally C4) was decided not to be implemented after review (it would require an overall upgrade of the layout model of the header and rows),
            //    so the text is fetched cell by cell here
            const std::string strText = GetSubItemTextById(nRowIndex, columnIdList[nColumnIndex]);
            strLine += EscapeText(strText, bCsvFormat);
        }
        if (!bFirstLine) {
            strResult += strLineSep;
        }
        bFirstLine = false;
        strResult += strLine;
    }
    return strResult;
}

bool ListCtrl::CopySelectionToClipboard(bool bCsvFormat)
{
    //When the switch is off, do nothing at all (inert: the edit state is not changed and the data is not modified)
    if (!m_bEnableCopy) {
        return false;
    }

    //Mutually exclusive with the edit state: while editing, commit the edited content first and then copy, so that the edit result is not lost
    LeaveEditMode();

    const std::string strText = BuildSelectionText(bCsvFormat, true);
    if (strText.empty()) {
        return false;
    }
    //On the Windows platform the clipboard is written through the UTF-16 path
    return Clipboard::SetClipboardText(strText);
}

bool ListCtrl::ExportToCsvFile(const std::string& strFilePath, bool bSelectedOnly)
{
    if (!m_bEnableCsvExport) {
        return false;
    }
    if (strFilePath.empty()) {
        return false;
    }
    const std::string strText = BuildSelectionText(true, bSelectedOnly);
    if (strText.empty()) {
        return false;
    }
    //Write the UTF-8 BOM header + the UTF-8 body, so that Excel detects the encoding directly (avoiding garbled Chinese text)
    const std::string& strUtf8Text = StringConvert::TToUTF8(strText);
    std::vector<uint8_t> fileData;
    fileData.reserve(3 + strUtf8Text.size());
    fileData.push_back((uint8_t)0xEF);
    fileData.push_back((uint8_t)0xBB);
    fileData.push_back((uint8_t)0xBF);
    fileData.insert(fileData.end(), strUtf8Text.begin(), strUtf8Text.end());
    return FileUtil::WriteFileData(FilePath(strFilePath), fileData);
}

RichEdit* ListCtrl::GetRichEdit() const
{
    return m_pRichEdit;
}

std::string ListCtrl::GetRichEditClass() const
{
    return m_listCtrlRichEditClass.c_str();
}

std::string ListCtrl::GetDateTimeClass() const
{
    return m_listCtrlDateTimeClass.c_str();
}

void ListCtrl::SetRowGridLineWidth(int32_t nLineWidth, bool bNeedDpiScale)
{
    m_pReportView->SetRowGridLineWidth(nLineWidth, bNeedDpiScale);
}

int32_t ListCtrl::GetRowGridLineWidth() const
{
    return m_pReportView->GetRowGridLineWidth();
}

void ListCtrl::SetRowGridLineColor(const std::string& color)
{
    m_pReportView->SetRowGridLineColor(color);
}

std::string ListCtrl::GetRowGridLineColor() const
{
    return m_pReportView->GetRowGridLineColor();
}

void ListCtrl::SetColumnGridLineWidth(int32_t nLineWidth, bool bNeedDpiScale)
{
    m_pReportView->SetColumnGridLineWidth(nLineWidth, bNeedDpiScale);
}

int32_t ListCtrl::GetColumnGridLineWidth() const
{
    return m_pReportView->GetColumnGridLineWidth();
}

void ListCtrl::SetColumnGridLineColor(const std::string& color)
{
    m_pReportView->SetColumnGridLineColor(color);
}

std::string ListCtrl::GetColumnGridLineColor() const
{
    return m_pReportView->GetColumnGridLineColor();
}

void ListCtrl::SetEnableColumnWidthAuto(bool bEnable)
{
    m_bEnableColumnWidthAuto = bEnable;
}

bool ListCtrl::IsEnableColumnWidthAuto() const
{
    return m_bEnableColumnWidthAuto;
}

ListCtrlHeaderItem* ListCtrl::InsertColumn(int32_t columnIndex, const ListCtrlColumn& columnInfo)
{
    if (m_pHeaderCtrl == nullptr) {
        return nullptr;
    }
    else {
        ListCtrlHeaderItem* pHeaderItem = m_pHeaderCtrl->InsertColumn(columnIndex, columnInfo);
        if (pHeaderItem != nullptr) {
            //Editor type declared per column: record the editor type of this column (stored by column ID, unaffected by column order changes)
            SetColumnEditorTypeById(pHeaderItem->GetColumnId(), columnInfo.nEditorType);
        }
        return pHeaderItem;
    }
}

size_t ListCtrl::GetColumnCount() const
{
    if (m_pHeaderCtrl == nullptr) {
        return 0;
    }
    else {
        return m_pHeaderCtrl->GetColumnCount();
    }
}

int32_t ListCtrl::GetColumnWidth(size_t columnIndex) const
{
    if (m_pHeaderCtrl == nullptr) {
        return 0;
    }
    else {
        return m_pHeaderCtrl->GetColumnWidth(columnIndex);
    }
}

int32_t ListCtrl::GetColumnWidthById(size_t columnId) const
{
    size_t columnIndex = GetColumnIndex(columnId);
    if (Box::IsValidItemIndex(columnIndex)) {
        return GetColumnWidth(columnIndex);
    }
    return 0;
}

bool ListCtrl::SetColumnWidth(size_t columnIndex, int32_t nWidth, bool bNeedDpiScale)
{
    bool bRet = false;
    ASSERT(m_pHeaderCtrl != nullptr);
    if (m_pHeaderCtrl != nullptr) {
        bRet = m_pHeaderCtrl->SetColumnWidth(columnIndex, nWidth, bNeedDpiScale);
    }
    if(bRet) {
        OnColumnWidthChanged(GetColumnId(columnIndex), Box::InvalidIndex);
    }
    return true;
}

bool ListCtrl::SetColumnWidthById(size_t columnId, int32_t nWidth, bool bNeedDpiScale)
{
    size_t columnIndex = GetColumnIndex(columnId);
    if (Box::IsValidItemIndex(columnIndex)) {
        return SetColumnWidth(columnIndex, nWidth, bNeedDpiScale);
    }
    return false;
}

bool ListCtrl::SetColumnWidthAuto(size_t columnIndex)
{
    bool bRet = false;
    size_t nColumnId = GetColumnId(columnIndex);
    if (nColumnId == Box::InvalidIndex) {
        return bRet;
    }
    //Calculate the width of this column
    int32_t nMaxWidth = m_pData->GetMaxColumnWidth(nColumnId);
    if (nMaxWidth > 0) {
        bRet = SetColumnWidth(columnIndex, nMaxWidth, false);
    }
    return bRet;
}

bool ListCtrl::SetColumnWidthAutoById(size_t columnId)
{
    size_t columnIndex = GetColumnIndex(columnId);
    if (Box::IsValidItemIndex(columnIndex)) {
        return SetColumnWidthAuto(columnIndex);
    }
    return false;
}

bool ListCtrl::SetColumnWidth(const std::vector<UiFixedInt>& columnWidthList, bool bNeedDpiScale)
{
    if (columnWidthList.empty()) {
        return false;
    }
    bool bRet = true;
    std::map<size_t, UiFixedInt> stretchWidthMap;
    const size_t nColumnWidthCount = std::min(columnWidthList.size(), GetColumnCount());
    if (nColumnWidthCount != columnWidthList.size()) {
        bRet = false;
    }
    for (size_t nColumnIndex = 0; nColumnIndex < nColumnWidthCount; ++nColumnIndex) {
        const UiFixedInt& fixedInt = columnWidthList[nColumnIndex];
        if (fixedInt.IsInt32()) {
            //Fixed type
            if (fixedInt.GetInt32() < 0) {
                bRet = false;
            }
            else if (!SetColumnWidth(nColumnIndex, fixedInt.GetInt32(), bNeedDpiScale)) {
                bRet = false;
            }
        }
        else if (fixedInt.IsAuto()) {
            //Auto-fit width
            if (!SetColumnWidthAuto(nColumnIndex)) {
                bRet = false;
            }
        }
        else if (fixedInt.IsStretch()) {
            //Stretch type
            stretchWidthMap[nColumnIndex] = fixedInt;
        }
    }

    //Calculate the width of each stretch-type column
    if (!stretchWidthMap.empty()) {
        if ((m_pReportView != nullptr) && (m_pHeaderCtrl != nullptr)) {            
            int32_t nToltalFixedWidth = 0;
            const size_t nColumnCount = m_pHeaderCtrl->GetColumnCount();
            for (size_t nColumnIndex = 0; nColumnIndex < nColumnCount; ++nColumnIndex) {
                if (stretchWidthMap.find(nColumnIndex) == stretchWidthMap.end()) {
                    //Non-stretch-type columns
                    ListCtrlHeaderItem* pHeaderItem = m_pHeaderCtrl->GetColumn(nColumnIndex);
                    if ((pHeaderItem != nullptr) && pHeaderItem->IsColumnVisible()) {
                        nToltalFixedWidth += pHeaderItem->GetColumnWidth();
                    }
                }
            }

            UiRect rcViewRect = m_pReportView->GetRect();
            UiPadding rcViewPadding = m_pReportView->GetPadding();
            UiPadding rcHeaderPadding = m_pHeaderCtrl->GetPadding();
            rcViewRect.Deflate(rcViewPadding);
            rcViewRect.Deflate(rcHeaderPadding);
            const int32_t nAvailableWidth = rcViewRect.Width() - nToltalFixedWidth; //Allocatable width
            if (nAvailableWidth >= (int32_t)stretchWidthMap.size()) {
                //First calculate the total ratio
                int32_t nStretchTotal = 0;
                for (const auto& iter : stretchWidthMap) {
                    nStretchTotal += iter.second.GetStretchPercentValue();
                }
                const int32_t nStretchBase = std::max(100, nStretchTotal);
                int32_t nAvailableSpace = nAvailableWidth;
                int32_t nAvailableCount = (int32_t)stretchWidthMap.size();
                for (const auto& iter : stretchWidthMap) {
                    const size_t nColumnIndex = iter.first;
                    int32_t nColunmWidth = iter.second.GetStretchPercentValue() * nAvailableWidth / nStretchBase;
                    if ((nStretchTotal >= 100) && (nAvailableCount == 1)) {
                        //When it needs to be fully allocated and is the last stretch column
                        nColunmWidth = nAvailableSpace;
                    }

                    //Adjust the column width
                    SetColumnWidth(nColumnIndex, nColunmWidth, false);

                    nAvailableSpace -= nColunmWidth;
                    --nAvailableCount;
                }
            }
            else {
                bRet = false;
            }
        }
        else {
            bRet = false;
        }
    }
    return bRet;
}

ListCtrlHeaderItem* ListCtrl::GetColumn(size_t columnIndex) const
{
    if (m_pHeaderCtrl == nullptr) {
        return nullptr;
    }
    else {
        return m_pHeaderCtrl->GetColumn(columnIndex);
    }
}

ListCtrlHeaderItem* ListCtrl::GetColumnById(size_t columnId) const
{
    if (m_pHeaderCtrl == nullptr) {
        return nullptr;
    }
    else {
        return m_pHeaderCtrl->GetColumnById(columnId);
    }
}

size_t ListCtrl::GetColumnIndex(size_t columnId) const
{
    if (m_pHeaderCtrl == nullptr) {
        return Box::InvalidIndex;
    }
    else {
        return m_pHeaderCtrl->GetColumnIndex(columnId);
    }
}

size_t ListCtrl::GetColumnId(size_t columnIndex) const
{
    ListCtrlHeaderItem* pHeaderItem = GetColumn(columnIndex);
    if (pHeaderItem != nullptr) {
        return pHeaderItem->GetColumnId();
    }
    return Box::InvalidIndex;
}

bool ListCtrl::IsValidColumnId(size_t columnId) const
{
    size_t nColumnIndex = GetColumnIndex(columnId);
    return nColumnIndex != Box::InvalidIndex;
}

bool ListCtrl::DeleteColumn(size_t columnIndex)
{
    if (m_pHeaderCtrl == nullptr) {
        return false;
    }
    else {
        return m_pHeaderCtrl->DeleteColumn(columnIndex);
    }
}

bool ListCtrl::DeleteColumnById(size_t columnId)
{
    if (m_pHeaderCtrl == nullptr) {
        return false;
    }
    else {
        return m_pHeaderCtrl->DeleteColumnById(columnId);
    }
}

ListCtrlHeader* ListCtrl::GetHeaderCtrl() const
{
    return m_pHeaderCtrl;
}

void ListCtrl::SetEnableHeaderDragOrder(bool bEnable)
{
    m_bEnableHeaderDragOrder = bEnable;
}

bool ListCtrl::IsEnableHeaderDragOrder() const
{
    return m_bEnableHeaderDragOrder;
}

void ListCtrl::SetFrozenColumnCount(size_t nCount)
{
    if (m_nFrozenColumnCount != nCount) {
        m_nFrozenColumnCount = nCount;
        Refresh();
    }
}

size_t ListCtrl::GetFrozenColumnCount() const
{
    return m_nFrozenColumnCount;
}

void ListCtrl::SetHeaderVisible(bool bVisible)
{
    m_bShowHeaderCtrl = bVisible;
    if (m_pHeaderCtrl != nullptr) {
        m_pHeaderCtrl->SetVisible(bVisible);
    }
    Refresh();
}

bool ListCtrl::IsHeaderVisible() const
{
    if (m_pHeaderCtrl != nullptr) {
        return m_pHeaderCtrl->IsVisible();
    }
    else if(!IsInited()){
        return m_bShowHeaderCtrl;
    }
    else {
        return false;
    }
}

void ListCtrl::SetHeaderHeight(int32_t nHeaderHeight, bool bNeedDpiScale)
{
    if (nHeaderHeight < 0) {
        nHeaderHeight = 0;
    }
    if (bNeedDpiScale) {
        Dpi().ScaleInt(nHeaderHeight);
    }
    if (m_pHeaderCtrl != nullptr) {
        m_pHeaderCtrl->SetFixedHeight(UiFixedInt(nHeaderHeight), true, false);
        RelayoutOrRedraw();
    }
    m_nHeaderHeight = nHeaderHeight;
    Refresh();
}

int32_t ListCtrl::GetHeaderHeight() const
{
    int32_t nHeaderHeight = 0;
    if (m_pHeaderCtrl != nullptr) {
        nHeaderHeight = m_pHeaderCtrl->GetFixedHeight().GetInt32();
    }
    else {
        nHeaderHeight = m_nHeaderHeight;
        if (nHeaderHeight < 0) {
            nHeaderHeight = GetDataItemHeight();
        }
    }
    return nHeaderHeight;
}

void ListCtrl::SetDataItemHeight(int32_t nItemHeight, bool bNeedDpiScale)
{
    ASSERT(nItemHeight > 0);
    if (nItemHeight <= 0) {
        return;
    }
    if (bNeedDpiScale) {
        Dpi().ScaleInt(nItemHeight);
    }
    if (m_nItemHeight != nItemHeight) {
        m_nItemHeight = nItemHeight;
        //Update the default row height
        m_pData->SetDefaultItemHeight(nItemHeight);
        Refresh();
    }
}

int32_t ListCtrl::GetDataItemHeight() const
{
    return m_nItemHeight;
}

ListCtrlItem* ListCtrl::GetFirstDisplayItem() const
{
    ListCtrlItem* pItem = nullptr;
    ASSERT(m_pReportView != nullptr);
    if (m_pReportView != nullptr) {
        size_t itemCount = m_pReportView->GetItemCount();
        for (size_t index = 0; index < itemCount; ++index) {
            pItem = dynamic_cast<ListCtrlItem*>(m_pReportView->GetItemAt(index));
            if ((pItem != nullptr) && pItem->IsVisible()) {
                break;
            }
        }
    }
    return pItem;
}

ListCtrlItem* ListCtrl::GetNextDisplayItem(ListCtrlItem* pItem) const
{
    ListCtrlItem* pNextItem = nullptr;
    if (pItem == nullptr) {
        pNextItem = GetFirstDisplayItem();
    }
    else {
        ASSERT(m_pReportView != nullptr);
        if (m_pReportView != nullptr) {
            size_t itemCount = m_pReportView->GetItemCount();
            size_t nStartIndex = m_pReportView->GetItemIndex(pItem);
            if ((itemCount > 0) && (nStartIndex < (itemCount - 1))) {
                for (size_t index = nStartIndex + 1; index < itemCount; ++index) {
                    ListCtrlItem* pCheckItem = dynamic_cast<ListCtrlItem*>(m_pReportView->GetItemAt(index));
                    if ((pCheckItem != nullptr) && pCheckItem->IsVisible()) {
                        pNextItem = pCheckItem;
                        break;
                    }
                }
            }
        }
    }
    return pNextItem;
}

ListCtrlIconViewItem* ListCtrl::GetFirstDisplayIconItem() const
{
    ListCtrlIconViewItem* pItem = nullptr;
    ASSERT(m_pIconView != nullptr);
    if (m_pIconView != nullptr) {
        size_t itemCount = m_pIconView->GetItemCount();
        for (size_t index = 0; index < itemCount; ++index) {
            pItem = dynamic_cast<ListCtrlIconViewItem*>(m_pIconView->GetItemAt(index));
            if ((pItem != nullptr) && pItem->IsVisible()) {
                break;
            }
        }
    }
    return pItem;
}

ListCtrlIconViewItem* ListCtrl::GetNextDisplayIconItem(ListCtrlIconViewItem* pItem) const
{
    ListCtrlIconViewItem* pNextItem = nullptr;
    if (pItem == nullptr) {
        pNextItem = GetFirstDisplayIconItem();
    }
    else {
        ASSERT(m_pIconView != nullptr);
        if (m_pIconView != nullptr) {
            size_t itemCount = m_pIconView->GetItemCount();
            size_t nStartIndex = m_pIconView->GetItemIndex(pItem);
            if ((itemCount > 0) && (nStartIndex < (itemCount - 1))) {
                for (size_t index = nStartIndex + 1; index < itemCount; ++index) {
                    ListCtrlIconViewItem* pCheckItem = dynamic_cast<ListCtrlIconViewItem*>(m_pIconView->GetItemAt(index));
                    if ((pCheckItem != nullptr) && pCheckItem->IsVisible()) {
                        pNextItem = pCheckItem;
                        break;
                    }
                }
            }
        }
    }
    return pNextItem;
}

ListCtrlListViewItem* ListCtrl::GetFirstDisplayListItem() const
{
    ListCtrlListViewItem* pItem = nullptr;
    ASSERT(m_pListView != nullptr);
    if (m_pListView != nullptr) {
        size_t itemCount = m_pListView->GetItemCount();
        for (size_t index = 0; index < itemCount; ++index) {
            pItem = dynamic_cast<ListCtrlListViewItem*>(m_pListView->GetItemAt(index));
            if ((pItem != nullptr) && pItem->IsVisible()) {
                break;
            }
        }
    }
    return pItem;
}

ListCtrlListViewItem* ListCtrl::GetNextDisplayListItem(ListCtrlListViewItem* pItem) const
{
    ListCtrlListViewItem* pNextItem = nullptr;
    if (pItem == nullptr) {
        pNextItem = GetFirstDisplayListItem();
    }
    else {
        ASSERT(m_pListView != nullptr);
        if (m_pListView != nullptr) {
            size_t itemCount = m_pListView->GetItemCount();
            size_t nStartIndex = m_pListView->GetItemIndex(pItem);
            if ((itemCount > 0) && (nStartIndex < (itemCount - 1))) {
                for (size_t index = nStartIndex + 1; index < itemCount; ++index) {
                    ListCtrlListViewItem* pCheckItem = dynamic_cast<ListCtrlListViewItem*>(m_pListView->GetItemAt(index));
                    if ((pCheckItem != nullptr) && pCheckItem->IsVisible()) {
                        pNextItem = pCheckItem;
                        break;
                    }
                }
            }
        }
    }
    return pNextItem;
}

void ListCtrl::OnColumnWidthChanged(size_t nColumnId1, size_t nColumnId2)
{
    if ((m_pReportView == nullptr) || (m_pHeaderCtrl == nullptr)){
        return;
    }

    size_t nColumn1 = Box::InvalidIndex;
    size_t nColumn2 = Box::InvalidIndex;
    int32_t nColumnWidth1 = -1;
    int32_t nColumnWidth2 = -1;
    if (!m_pHeaderCtrl->GetColumnInfo(nColumnId1, nColumn1, nColumnWidth1)) {
        nColumnWidth1 = -1;
    }
    if (!m_pHeaderCtrl->GetColumnInfo(nColumnId2, nColumn2, nColumnWidth2)) {
        nColumnWidth2 = -1;
    }
    std::map<size_t, int32_t> subItemWidths;
    if (nColumnWidth1 >= 0) {
        subItemWidths[nColumn1] = nColumnWidth1;
    }
    if (nColumnWidth2 >= 0) {
        subItemWidths[nColumn2] = nColumnWidth2;
    }
    if (subItemWidths.empty()) {
        return;
    }
    m_pReportView->AdjustSubItemWidth(subItemWidths);
    Arrange();
    //The column width has changed; notify the application layer (which may persist the column widths)
    SendEvent(kEventColumnWidthChanged, (WPARAM)nColumnId1, (LPARAM)nColumnId2);
}

void ListCtrl::OnHeaderColumnAdded(size_t nColumnId)
{
    m_pData->AddColumn(nColumnId);
}

void ListCtrl::OnHeaderColumnRemoved(size_t nColumnId)
{
    m_columnSortFlagMap.erase(nColumnId);
    m_columnEditorTypes.erase(nColumnId);
    m_columnCandidates.erase(nColumnId);
    m_pData->RemoveColumn(nColumnId);
}

void ListCtrl::OnColumnSorted(size_t nColumnId, bool bSortedUp)
{
    //Sort the data, then refresh the UI display
    m_nSortedColumnId = nColumnId;
    m_bSortedUp = bSortedUp;
    uint8_t nSortFlag = GetColumnSortFlagById(m_nSortedColumnId);
    m_pData->SortDataItems(nColumnId, GetColumnIndex(nColumnId), bSortedUp, nSortFlag, nullptr, nullptr);
    Refresh();
    //The column sort state has changed; notify the application layer (which may persist the sort state)
    SendEvent(kEventColumnSorted, (WPARAM)nColumnId, (LPARAM)(bSortedUp ? 1 : 0));
}

bool ListCtrl::GetSortColumn(size_t& nSortColumnId, bool& bSortUp) const
{
    nSortColumnId = Box::InvalidIndex;
    if (Box::IsValidItemIndex(GetColumnIndex(m_nSortedColumnId))) {
        nSortColumnId = m_nSortedColumnId;
        bSortUp = m_bSortedUp;
        return true;
    }
    return false;
}

bool ListCtrl::SortDataItems(size_t columnIndex, bool bSortedUp, uint8_t nSortFlag,
                             ListCtrlDataCompareFunc pfnCompareFunc,
                             void* pUserData)
{
    size_t nColumnId = GetColumnId(columnIndex);
    if (nColumnId == Box::InvalidIndex) {
        return false;
    }
    return SortDataItemsById(nColumnId, bSortedUp, nSortFlag, pfnCompareFunc, pUserData);
}

bool ListCtrl::SortDataItemsById(size_t columnId, bool bSortedUp, uint8_t nSortFlag,
                                 ListCtrlDataCompareFunc pfnCompareFunc, void* pUserData)
{
    size_t columnIndex = GetColumnIndex(columnId);
    if (columnIndex == Box::InvalidIndex) {
        return false;
    }
    m_nSortedColumnId = columnId;
    m_bSortedUp = bSortedUp;
    SetColumnSortFlagById(columnId, nSortFlag);
    if (m_pHeaderCtrl != nullptr) {
        //Update the UI sort display
        m_pHeaderCtrl->SetSortColumnId(columnId, bSortedUp, false);
    }    
    bool bRet = m_pData->SortDataItems(columnId, columnIndex, bSortedUp, nSortFlag, pfnCompareFunc, pUserData);
    if (bRet) {
        //The column sort state has changed; notify the application layer (which may persist the sort state)
        SendEvent(kEventColumnSorted, (WPARAM)columnId, (LPARAM)(bSortedUp ? 1 : 0));
    }
    return bRet;
}

void ListCtrl::SetSortCompareFunction(ListCtrlDataCompareFunc pfnCompareFunc, void* pUserData)
{
    m_pData->SetSortCompareFunction(pfnCompareFunc, pUserData);
}

void ListCtrl::SetColumnSortFlag(size_t columnIndex, uint8_t nSortFlag)
{
    SetColumnSortFlagById(GetColumnId(columnIndex), nSortFlag);
}

uint8_t ListCtrl::GetColumnSortFlag(size_t columnIndex)
{
    return GetColumnSortFlagById(GetColumnId(columnIndex));
}

void ListCtrl::SetColumnSortFlagById(size_t columnId, uint8_t nSortFlag)
{
    if (IsValidColumnId(columnId)) {
        m_columnSortFlagMap[columnId] = nSortFlag;
    }
}

uint8_t ListCtrl::GetColumnSortFlagById(size_t columnId)
{
    auto iter = m_columnSortFlagMap.find(columnId);
    if (iter != m_columnSortFlagMap.end()) {
        return iter->second;
    }
    return ListCtrlSubItemSortFlag::kDefault;
}

void ListCtrl::OnHeaderColumnOrderChanged()
{
    Refresh();
    //The display order of the columns has changed; notify the application layer (which may persist the column order)
    SendEvent(kEventColumnOrderChanged, 0, 0);
}

void ListCtrl::OnHeaderColumnCheckStateChanged(size_t nColumnId, bool bChecked)
{
    //The UI state changed, synchronize it to the underlying storage
    m_pData->SetColumnCheck(nColumnId, bChecked, true);
}

void ListCtrl::OnHeaderCheckStateChanged(bool bChecked)
{
    //The UI state changed, synchronize it to the underlying storage
    m_pData->SetAllDataItemsCheck(bChecked);
}

void ListCtrl::OnHeaderColumnVisibleChanged(size_t nColumnId)
{
    Refresh();
    //The show/hide state of the columns has changed; notify the application layer (which may persist the column display state)
    bool bVisible = IsColumnVisibleById(nColumnId);
    SendEvent(kEventColumnVisibleChanged, (WPARAM)nColumnId, (LPARAM)(bVisible ? 1 : 0));
}

void ListCtrl::OnHeaderColumnSplitDoubleClick(ListCtrlHeaderItem* pHeaderItem)
{
    if (!IsEnableColumnWidthAuto()) {
        //Feature disabled
        return;
    }
    //Automatically adjust the width of this column
    if (pHeaderItem != nullptr) {
        size_t nColumnId = pHeaderItem->GetColumnId();
        size_t nColumIndex = GetColumnIndex(nColumnId);
        if (nColumIndex < GetColumnCount()) {
            SetColumnWidthAuto(nColumIndex);
        }
    }
}

void ListCtrl::UpdateHeaderColumnCheckBox(size_t nColumnId)
{
    if (m_pHeaderCtrl == nullptr) {
        return;
    }
    std::vector<size_t> columnIdList;
    if (nColumnId == Box::InvalidIndex) {
        const size_t columnCount = GetColumnCount();
        for (size_t columnIndex = 0; columnIndex < columnCount; ++columnIndex) {
            ListCtrlHeaderItem* pHeaderItem = m_pHeaderCtrl->GetColumn(columnIndex);
            if ((pHeaderItem != nullptr) && (pHeaderItem->IsShowCheckBox())) {
                columnIdList.push_back(pHeaderItem->GetColumnId());
            }
        }
    }
    else {
        size_t columnIndex = GetColumnIndex(nColumnId);
        if (columnIndex != Box::InvalidIndex) {
            ListCtrlHeaderItem* pHeaderItem = m_pHeaderCtrl->GetColumn(columnIndex);
            if ((pHeaderItem != nullptr) && (pHeaderItem->IsShowCheckBox())){
                columnIdList.push_back(nColumnId);
            }
        }
    }
    for (size_t columnId : columnIdList) {
        bool bChecked = false;
        bool bPartChecked = false;
        m_pData->GetColumnCheckStatus(columnId, bChecked, bPartChecked);
        ListCtrlHeaderItem* pHeaderItem = m_pHeaderCtrl->GetColumnById(columnId);
        if ((pHeaderItem != nullptr) && (pHeaderItem->IsShowCheckBox())) {
            pHeaderItem->SetCheckBoxCheck(bChecked, bPartChecked);
        }
    }
}

void ListCtrl::UpdateHeaderCheckBox()
{
    if (!IsDataItemShowCheckBox()) {
        //CheckBox is not shown, ignore
        return;
    }

    if (m_pHeaderCtrl == nullptr) {
        return;
    }
    bool bChecked = false;
    bool bPartChecked = false;
    m_pData->GetDataItemsCheckStatus(bChecked, bPartChecked);
    if ((m_pHeaderCtrl->IsChecked() != bChecked) || 
        (m_pHeaderCtrl->IsPartChecked() != bPartChecked)) {
        bool bOldValue = m_pHeaderCtrl->SetEnableCheckChangeEvent(false);
        m_pHeaderCtrl->SetChecked(bChecked, false);
        m_pHeaderCtrl->SetPartChecked(bPartChecked);
        m_pHeaderCtrl->SetEnableCheckChangeEvent(bOldValue);
    }
}

size_t ListCtrl::GetDataItemCount() const
{
    return m_pData->GetDataItemCount();
}

bool ListCtrl::SetDataItemCount(size_t itemCount)
{
    const size_t nOldDataItemCount = m_pData->GetDataItemCount();
    bool bRet = m_pData->SetDataItemCount(itemCount);
    if (bRet) {
        const size_t nNewDataItemCount = m_pData->GetDataItemCount();
        bool bChanged = (nOldDataItemCount != nNewDataItemCount);
        if (bChanged) {
            //The number of list data items has changed
            UpdateHeaderColumnCheckBox(Box::InvalidIndex);
            UpdateHeaderCheckBox();
            SendEvent(kEventDataItemCountChanged, (WPARAM)nNewDataItemCount, (LPARAM)nOldDataItemCount);
        }
    }
    return bRet;
}

size_t ListCtrl::AddDataItem(const ListCtrlSubItemData& dataItem)
{
    size_t columnId = GetColumnId(0);
    if (columnId == Box::InvalidIndex) {
        return Box::InvalidIndex;
    }
    size_t nItemIndex = m_pData->AddDataItem(columnId, dataItem);
    if (nItemIndex != Box::InvalidIndex) {
        UpdateHeaderColumnCheckBox(Box::InvalidIndex);
        UpdateHeaderCheckBox();
    }    
    return nItemIndex;
}

bool ListCtrl::InsertDataItem(size_t itemIndex, const ListCtrlSubItemData& dataItem)
{
    size_t columnId = GetColumnId(0);
    if (columnId == Box::InvalidIndex) {
        return Box::InvalidIndex;
    }
    bool bRet = m_pData->InsertDataItem(itemIndex, columnId, dataItem);
    if (bRet) {
        UpdateHeaderColumnCheckBox(Box::InvalidIndex);
        UpdateHeaderCheckBox();
    }
    return bRet;
}

bool ListCtrl::SetSubItemData(size_t itemIndex, size_t columnIndex, const ListCtrlSubItemData& subItemData)
{
    return SetSubItemDataById(itemIndex, GetColumnId(columnIndex), subItemData);
}

bool ListCtrl::SetSubItemDataById(size_t itemIndex, size_t columnId, const ListCtrlSubItemData& subItemData)
{
    bool bCheckChanged = false;
    bool bRet = m_pData->SetSubItemData(itemIndex, columnId, subItemData, bCheckChanged);
    if (bCheckChanged) {
        UpdateHeaderColumnCheckBox(GetColumnId(itemIndex));
    }
    return bRet;
}

bool ListCtrl::GetSubItemData(size_t itemIndex, size_t columnIndex, ListCtrlSubItemData& subItemData) const
{
    return GetSubItemDataById(itemIndex, GetColumnId(columnIndex), subItemData);
}

bool ListCtrl::GetSubItemDataById(size_t itemIndex, size_t columnId, ListCtrlSubItemData& subItemData) const
{
    return m_pData->GetSubItemData(itemIndex, columnId, subItemData);
}

bool ListCtrl::DeleteDataItem(size_t itemIndex)
{
    bool bRet = m_pData->DeleteDataItem(itemIndex);
    if (bRet) {
        UpdateHeaderColumnCheckBox(Box::InvalidIndex);
        UpdateHeaderCheckBox();
    }
    return bRet;
}

bool ListCtrl::DeleteAllDataItems()
{
    bool bRet = m_pData->DeleteAllDataItems();
    if (bRet) {
        UpdateHeaderColumnCheckBox(Box::InvalidIndex);
        UpdateHeaderCheckBox();
    }
    return bRet;
}

size_t ListCtrl::DeleteDataItems(const std::vector<size_t>& itemIndexs)
{
    size_t nDeleted = m_pData->DeleteDataItems(itemIndexs);
    if (nDeleted > 0) {
        UpdateHeaderColumnCheckBox(Box::InvalidIndex);
        UpdateHeaderCheckBox();
    }
    return nDeleted;
}

void ListCtrl::CompactStorage()
{
    m_pData->CompactStorage();
}

bool ListCtrl::SetDataItemData(size_t itemIndex, const ListCtrlItemData& itemData)
{
    bool bChanged = false;
    bool bCheckChanged = false;
    bool bRet = m_pData->SetDataItemData(itemIndex, itemData, bChanged, bCheckChanged);
    if (bRet && bCheckChanged) {
        UpdateHeaderCheckBox();
    }
    return bRet;
}

bool ListCtrl::GetDataItemData(size_t itemIndex, ListCtrlItemData& itemData) const
{
    return m_pData->GetDataItemData(itemIndex, itemData);
}

bool ListCtrl::SetDataItemUserData(size_t itemIndex, size_t userData)
{
    return m_pData->SetDataItemUserData(itemIndex, userData);
}

size_t ListCtrl::GetDataItemUserData(size_t itemIndex) const
{
    return m_pData->GetDataItemUserData(itemIndex);
}

bool ListCtrl::SetDataItemVisible(size_t itemIndex, bool bVisible)
{
    bool bChanged = false;
    bool bRet = m_pData->SetDataItemVisible(itemIndex, bVisible, bChanged);
    if (bChanged) {
        UpdateHeaderColumnCheckBox(Box::InvalidIndex);
        UpdateHeaderCheckBox();
    }
    return bRet;
}

bool ListCtrl::IsDataItemVisible(size_t itemIndex) const
{
    return m_pData->IsDataItemVisible(itemIndex);
}

bool ListCtrl::SetDataItemSelected(size_t itemIndex, bool bSelected)
{
    bool bChanged = false;
    bool bOldChecked = m_pData->IsDataItemChecked(itemIndex);
    bool bRet = m_pData->SetDataItemSelected(itemIndex, bSelected, bChanged);
    if (bChanged) {
        if (m_pData->IsDataItemChecked(itemIndex) != bOldChecked) {
            UpdateHeaderCheckBox();
        }
    }
    return bRet;
}

bool ListCtrl::IsDataItemSelected(size_t itemIndex) const
{
    return m_pData->IsDataItemSelected(itemIndex);
}

bool ListCtrl::SetDataItemImageId(size_t itemIndex, int32_t imageId)
{
    bool bChanged = false;
    bool bRet = m_pData->SetDataItemImageId(itemIndex, imageId, bChanged);
    if (bChanged) {
        Refresh();
    }
    return bRet;
}

int32_t ListCtrl::GetDataItemImageId(size_t itemIndex) const
{
    return m_pData->GetDataItemImageId(itemIndex);
}

bool ListCtrl::SetDataItemAlwaysAtTop(size_t itemIndex, int8_t nAlwaysAtTop)
{
    bool bChanged = false;
    bool bRet = m_pData->SetDataItemAlwaysAtTop(itemIndex, nAlwaysAtTop, bChanged);
    if (bChanged) {
        Refresh();
    }
    return bRet;
}

int8_t ListCtrl::GetDataItemAlwaysAtTop(size_t itemIndex) const
{
    return m_pData->GetDataItemAlwaysAtTop(itemIndex);
}

bool ListCtrl::SetDataItemHeight(size_t itemIndex, int32_t nItemHeight, bool bNeedDpiScale)
{
    bool bChanged = false;
    if (nItemHeight < 0) {
        nItemHeight = 0;
    }
    if (bNeedDpiScale) {
        Dpi().ScaleInt(nItemHeight);
    }
    bool bRet = m_pData->SetDataItemHeight(itemIndex, nItemHeight, bChanged);
    if (bChanged) {
        Refresh();
    }
    return bRet;
}

int32_t ListCtrl::GetDataItemHeight(size_t itemIndex) const
{
    return m_pData->GetDataItemHeight(itemIndex);
}

bool ListCtrl::SetSubItemText(size_t itemIndex, size_t columnIndex, const std::string& text)
{
    return SetSubItemTextById(itemIndex, GetColumnId(columnIndex), text);
}

bool ListCtrl::SetSubItemTextById(size_t itemIndex, size_t columnId, const std::string& text)
{
    return m_pData->SetSubItemText(itemIndex, columnId, text);
}

std::string ListCtrl::GetSubItemText(size_t itemIndex, size_t columnIndex) const
{
    return GetSubItemTextById(itemIndex, GetColumnId(columnIndex));
}

std::string ListCtrl::GetSubItemTextById(size_t itemIndex, size_t columnId) const
{
    return m_pData->GetSubItemText(itemIndex, columnId);
}

bool ListCtrl::SetSubItemSortGroup(size_t itemIndex, size_t columnIndex, int32_t nSortGroup)
{
    return SetSubItemSortGroupById(itemIndex, GetColumnId(columnIndex), nSortGroup);
}

bool ListCtrl::SetSubItemSortGroupById(size_t itemIndex, size_t columnId, int32_t nSortGroup)
{
    return m_pData->SetSubItemSortGroup(itemIndex, columnId, nSortGroup);
}

int32_t ListCtrl::GetSubItemGroup(size_t itemIndex, size_t columnIndex) const
{
    return GetSubItemGroupById(itemIndex, GetColumnId(columnIndex));
}

int32_t ListCtrl::GetSubItemGroupById(size_t itemIndex, size_t columnId) const
{
    return m_pData->GetSubItemSortGroup(itemIndex, columnId);
}

bool ListCtrl::SetSubItemUserDataN(size_t itemIndex, size_t columnIndex, uint64_t userDataN)
{
    return SetSubItemUserDataNById(itemIndex, GetColumnId(columnIndex), userDataN);
}

bool ListCtrl::SetSubItemUserDataNById(size_t itemIndex, size_t columnId, uint64_t userDataN)
{
    return m_pData->SetSubItemUserDataN(itemIndex, columnId, userDataN);
}

uint64_t ListCtrl::GetSubItemUserDataN(size_t itemIndex, size_t columnIndex) const
{
    return GetSubItemUserDataNById(itemIndex, GetColumnId(columnIndex));
}

uint64_t ListCtrl::GetSubItemUserDataNById(size_t itemIndex, size_t columnId) const
{
    return m_pData->GetSubItemUserDataN(itemIndex, columnId);
}

bool ListCtrl::SetSubItemUserDataS(size_t itemIndex, size_t columnIndex, const std::string& userDataS)
{
    return SetSubItemUserDataSById(itemIndex, GetColumnId(columnIndex), userDataS);
}

bool ListCtrl::SetSubItemUserDataSById(size_t itemIndex, size_t columnId, const std::string& userDataS)
{
    return m_pData->SetSubItemUserDataS(itemIndex, columnId, userDataS);
}

std::string ListCtrl::GetSubItemUserDataS(size_t itemIndex, size_t columnIndex) const
{
    return GetSubItemUserDataSById(itemIndex, GetColumnId(columnIndex));
}

std::string ListCtrl::GetSubItemUserDataSById(size_t itemIndex, size_t columnId) const
{
    return m_pData->GetSubItemUserDataS(itemIndex, columnId);
}

bool ListCtrl::SetSubItemTextColor(size_t itemIndex, size_t columnIndex, const UiColor& textColor)
{
    return SetSubItemTextColorById(itemIndex, GetColumnId(columnIndex), textColor);
}

bool ListCtrl::SetSubItemTextColorById(size_t itemIndex, size_t columnId, const UiColor& textColor)
{
    return m_pData->SetSubItemTextColor(itemIndex, columnId, textColor);
}

UiColor ListCtrl::GetSubItemTextColor(size_t itemIndex, size_t columnIndex) const
{
    return GetSubItemTextColorById(itemIndex, GetColumnId(columnIndex));
}

UiColor ListCtrl::GetSubItemTextColorById(size_t itemIndex, size_t columnId) const
{
    UiColor textColor;
    m_pData->GetSubItemTextColor(itemIndex, columnId, textColor);
    return textColor;
}

bool ListCtrl::SetSubItemTextFormat(size_t itemIndex, size_t columnIndex, int32_t nTextFormat)
{
    return SetSubItemTextFormatById(itemIndex, GetColumnId(columnIndex), nTextFormat);
}

bool ListCtrl::SetSubItemTextFormatById(size_t itemIndex, size_t columnId, int32_t nTextFormat)
{
    return m_pData->SetSubItemTextFormat(itemIndex, columnId, nTextFormat);
}

int32_t ListCtrl::GetSubItemTextFormat(size_t itemIndex, size_t columnIndex) const
{
    return GetSubItemTextFormatById(itemIndex, GetColumnId(columnIndex));
}

int32_t ListCtrl::GetSubItemTextFormatById(size_t itemIndex, size_t columnId) const
{
    return m_pData->GetSubItemTextFormat(itemIndex, columnId);
}

bool ListCtrl::SetSubItemSingleLine(size_t itemIndex, size_t columnIndex, bool bSingleLine)
{
    return SetSubItemSingleLineById(itemIndex, GetColumnId(columnIndex), bSingleLine);
}

bool ListCtrl::SetSubItemSingleLineById(size_t itemIndex, size_t columnId, bool bSingleLine)
{
    //Supported by the Report view only (the Icon/List views have no concept of columns; their single/multi-line behaviour is controlled by the view's own interface)
    if (m_listCtrlType != ListCtrlType::Report) {
        return false;
    }
    return m_pData->SetSubItemSingleLine(itemIndex, columnId, bSingleLine);
}

bool ListCtrl::IsSubItemSingleLine(size_t itemIndex, size_t columnIndex) const
{
    return IsSubItemSingleLineById(itemIndex, GetColumnId(columnIndex));
}

bool ListCtrl::IsSubItemSingleLineById(size_t itemIndex, size_t columnId) const
{
    if (m_listCtrlType != ListCtrlType::Report) {
        return true;
    }
    return m_pData->IsSubItemSingleLine(itemIndex, columnId);
}

bool ListCtrl::SetSubItemBkColor(size_t itemIndex, size_t columnIndex, const UiColor& bkColor)
{
    return SetSubItemBkColorById(itemIndex, GetColumnId(columnIndex), bkColor);
}

bool ListCtrl::SetSubItemBkColorById(size_t itemIndex, size_t columnId, const UiColor& bkColor)
{
    return m_pData->SetSubItemBkColor(itemIndex, columnId, bkColor);
}

UiColor ListCtrl::GetSubItemBkColor(size_t itemIndex, size_t columnIndex) const
{
    return GetSubItemBkColorById(itemIndex, GetColumnId(columnIndex));
}

UiColor ListCtrl::GetSubItemBkColorById(size_t itemIndex, size_t columnId) const
{
    UiColor bkColor;
    m_pData->GetSubItemBkColor(itemIndex, columnId, bkColor);
    return bkColor;
}

bool ListCtrl::IsSubItemShowCheckBox(size_t itemIndex, size_t columnIndex) const
{
    return IsSubItemShowCheckBoxById(itemIndex, GetColumnId(columnIndex));
}

bool ListCtrl::IsSubItemShowCheckBoxById(size_t itemIndex, size_t columnId) const
{
    return m_pData->IsSubItemShowCheckBox(itemIndex, columnId);
}

bool ListCtrl::SetSubItemShowCheckBox(size_t itemIndex, size_t columnIndex, bool bShowCheckBox)
{
    return SetSubItemShowCheckBoxById(itemIndex, GetColumnId(columnIndex), bShowCheckBox);
}

bool ListCtrl::SetSubItemShowCheckBoxById(size_t itemIndex, size_t columnId, bool bShowCheckBox)
{
    return m_pData->SetSubItemShowCheckBox(itemIndex, columnId, bShowCheckBox);
}

bool ListCtrl::SetSubItemCheck(size_t itemIndex, size_t columnIndex, bool bChecked)
{
    return SetSubItemCheckById(itemIndex, GetColumnId(columnIndex), bChecked);
}

bool ListCtrl::SetSubItemCheckById(size_t itemIndex, size_t columnId, bool bChecked)
{
    return m_pData->SetSubItemCheck(itemIndex, columnId, bChecked, true);
}

bool ListCtrl::IsSubItemChecked(size_t itemIndex, size_t columnIndex) const
{
    return IsSubItemCheckedById(itemIndex, GetColumnId(columnIndex));
}

bool ListCtrl::IsSubItemCheckedById(size_t itemIndex, size_t columnId) const
{
    bool bChecked = false;
    m_pData->GetSubItemCheck(itemIndex, columnId, bChecked);
    return bChecked;
}

bool ListCtrl::SetSubItemImageId(size_t itemIndex, size_t columnIndex, int32_t imageId)
{
    return SetSubItemImageIdById(itemIndex, GetColumnId(columnIndex), imageId);
}

bool ListCtrl::SetSubItemImageIdById(size_t itemIndex, size_t columnId, int32_t imageId)
{
    return m_pData->SetSubItemImageId(itemIndex, columnId, imageId);
}

int32_t ListCtrl::GetSubItemImageId(size_t itemIndex, size_t columnIndex) const
{
    return GetSubItemImageIdById(itemIndex, GetColumnId(columnIndex));
}

int32_t ListCtrl::GetSubItemImageIdById(size_t itemIndex, size_t columnId) const
{
    return m_pData->GetSubItemImageId(itemIndex, columnId);
}

bool ListCtrl::SetSubItemEditable(size_t itemIndex, size_t columnIndex, bool bEditable)
{
    return SetSubItemEditableById(itemIndex, GetColumnId(columnIndex), bEditable);
}

bool ListCtrl::SetSubItemEditableById(size_t itemIndex, size_t columnId, bool bEditable)
{
    return m_pData->SetSubItemEditable(itemIndex, columnId, bEditable);
}

bool ListCtrl::IsSubItemEditable(size_t itemIndex, size_t columnIndex) const
{
    return IsSubItemEditableById(itemIndex, GetColumnId(columnIndex));
}

bool ListCtrl::IsSubItemEditableById(size_t itemIndex, size_t columnId) const
{
    return m_pData->IsSubItemEditable(itemIndex, columnId);
}

void ListCtrl::SetColumnEditorType(size_t columnIndex, ListCtrlEditorType editorType)
{
    SetColumnEditorTypeById(GetColumnId(columnIndex), editorType);
}

void ListCtrl::SetColumnEditorTypeById(size_t columnId, ListCtrlEditorType editorType)
{
    if (!IsValidColumnId(columnId)) {
        return;
    }
    if (editorType == ListCtrlEditorType::kEditText) {
        //Default values are not stored in the map, to avoid meaningless entries
        m_columnEditorTypes.erase(columnId);
    }
    else {
        m_columnEditorTypes[columnId] = editorType;
    }
}

ListCtrlEditorType ListCtrl::GetColumnEditorType(size_t columnIndex) const
{
    return GetColumnEditorTypeById(GetColumnId(columnIndex));
}

ListCtrlEditorType ListCtrl::GetColumnEditorTypeById(size_t columnId) const
{
    auto iter = m_columnEditorTypes.find(columnId);
    if (iter != m_columnEditorTypes.end()) {
        return iter->second;
    }
    return ListCtrlEditorType::kEditText;
}

void ListCtrl::SetColumnCandidates(size_t columnIndex, const std::vector<std::string>& candidates)
{
    SetColumnCandidatesById(GetColumnId(columnIndex), candidates);
}

void ListCtrl::SetColumnCandidatesById(size_t columnId, const std::vector<std::string>& candidates)
{
    if (!IsValidColumnId(columnId)) {
        return;
    }
    if (candidates.empty()) {
        //Empty list: clear the candidates, do not store an entry, to avoid a meaningless entry
        m_columnCandidates.erase(columnId);
    }
    else {
        m_columnCandidates[columnId] = candidates;
    }
}

std::vector<std::string> ListCtrl::GetColumnCandidates(size_t columnIndex) const
{
    return GetColumnCandidatesById(GetColumnId(columnIndex));
}

std::vector<std::string> ListCtrl::GetColumnCandidatesById(size_t columnId) const
{
    auto iter = m_columnCandidates.find(columnId);
    if (iter != m_columnCandidates.end()) {
        return iter->second;
    }
    return std::vector<std::string>();
}

bool ListCtrl::SetSubItemProgress(size_t itemIndex, size_t columnIndex, int32_t nProgress)
{
    return SetSubItemProgressById(itemIndex, GetColumnId(columnIndex), nProgress);
}

bool ListCtrl::SetSubItemProgressById(size_t itemIndex, size_t columnId, int32_t nProgress)
{
    return m_pData->SetSubItemProgress(itemIndex, columnId, nProgress);
}

int32_t ListCtrl::GetSubItemProgress(size_t itemIndex, size_t columnIndex) const
{
    return GetSubItemProgressById(itemIndex, GetColumnId(columnIndex));
}

int32_t ListCtrl::GetSubItemProgressById(size_t itemIndex, size_t columnId) const
{
    return m_pData->GetSubItemProgress(itemIndex, columnId);
}

bool ListCtrl::SetSubItemRichText(size_t itemIndex, size_t columnIndex, bool bRichText)
{
    return SetSubItemRichTextById(itemIndex, GetColumnId(columnIndex), bRichText);
}

bool ListCtrl::SetSubItemRichTextById(size_t itemIndex, size_t columnId, bool bRichText)
{
    return m_pData->SetSubItemRichText(itemIndex, columnId, bRichText);
}

bool ListCtrl::IsSubItemRichText(size_t itemIndex, size_t columnIndex) const
{
    return IsSubItemRichTextById(itemIndex, GetColumnId(columnIndex));
}

bool ListCtrl::IsSubItemRichTextById(size_t itemIndex, size_t columnId) const
{
    return m_pData->IsSubItemRichText(itemIndex, columnId);
}

bool ListCtrl::IsMultiSelect() const
{
    return m_bMultiSelect;
}

void ListCtrl::SetMultiSelect(bool bMultiSelect)
{
    m_bMultiSelect = bMultiSelect;
    if (m_pReportView != nullptr) {
        m_pReportView->SetMultiSelect(bMultiSelect);
    }
    if (m_pIconView != nullptr) {
        m_pIconView->SetMultiSelect(bMultiSelect);
    }
    if (m_pListView != nullptr) {
        m_pListView->SetMultiSelect(bMultiSelect);
    }
    UpdateHeaderCheckBox();
    if (!bMultiSelect) {
        //Switch to single-selection mode: in cell selection mode, the selected cells are clipped to 1 (the first selected cell is kept),
        //consistent with the behaviour of row selection mode (ListBox::OnSwitchToSingleSelect)
        std::vector<ListCtrlCellId> cellList;
        if (GetSelectedCells(cellList) > 1) {
            std::vector<ListCtrlCellId> keepCellList;
            keepCellList.push_back(cellList.front());
            SetSelectedCells(keepCellList, true);
        }
    }
}

void ListCtrl::SetSelectedDataItems(const std::vector<size_t>& selectedIndexs, bool bClearOthers)
{
    std::vector<size_t> refreshIndexs;
    m_pData->SetSelectedElements(selectedIndexs, bClearOthers, refreshIndexs);
    if (!refreshIndexs.empty()) {
        RefreshDataItems(refreshIndexs);
        UpdateHeaderCheckBox();
    }
}

void ListCtrl::GetSelectedDataItems(std::vector<size_t>& itemIndexs) const
{
    m_pData->GetSelectedElements(itemIndexs);
}

void ListCtrl::SetCheckedDataItems(const std::vector<size_t>& itemIndexs, bool bClearOthers)
{
    std::vector<size_t> refreshIndexs;
    m_pData->SetCheckedDataItems(itemIndexs, bClearOthers, refreshIndexs);    
    if (!refreshIndexs.empty()) {
        RefreshDataItems(refreshIndexs);
        UpdateHeaderCheckBox();
    }
}

void ListCtrl::GetCheckedDataItems(std::vector<size_t>& itemIndexs) const
{
    m_pData->GetCheckedDataItems(itemIndexs);
}

void ListCtrl::SetSelectAll()
{
    if (!IsMultiSelect()) {
        //Single-selection mode: at most one selected unit (row or cell), so "select all" is meaningless and nothing is changed
        //(this also avoids reaching the IsMultiSelect assertion inside ListCtrlData::SelectAll in row mode)
        return;
    }
    if (m_selectionMode == ListCtrlSelectionMode::kCell) {
        //Cell selection mode: select all cells
        std::vector<size_t> columnIdList;
        const size_t nColumnCount = GetColumnCount();
        for (size_t nColumnIndex = 0; nColumnIndex < nColumnCount; ++nColumnIndex) {
            const size_t nColumnId = GetColumnId(nColumnIndex);
            if (IsValidColumnId(nColumnId)) {
                columnIdList.push_back(nColumnId);
            }
        }
        m_pData->SelectAllCells(columnIdList);
        //If the current cell is invalid, position the current cell/anchor at the top-left cell
        if (!m_pData->GetCurrentCell().IsValid() &&
            (GetDataItemCount() > 0) && !columnIdList.empty()) {
            ListCtrlCellId cell;
            cell.nRowIndex = 0;
            cell.nColumnId = columnIdList[0];
            m_pData->SetAnchorCell(cell);
            m_pData->SetCurrentCell(cell);
        }
        NotifyCellSelectionChanged();
        RefreshCellSelectionView();
        return;
    }
    std::vector<size_t> refreshIndexs;
    m_pData->SelectAll(refreshIndexs);
    if (!refreshIndexs.empty()) {
        RefreshDataItems(refreshIndexs);
        if (IsAutoCheckSelect()) {
            UpdateHeaderCheckBox();
        }
    }    
}

void ListCtrl::SetSelectNone()
{
    if (m_selectionMode == ListCtrlSelectionMode::kCell) {
        //Cell selection mode: clear all cell selections
        //Reuse the conditional dispatch of ClearCellSelection: refresh and dispatch the selection changed event only when a selection state really exists
        ClearCellSelection();
        return;
    }
    std::vector<size_t> refreshIndexs;
    m_pData->SelectNone(refreshIndexs);
    if (!refreshIndexs.empty()) {
        RefreshDataItems(refreshIndexs);
        if (IsAutoCheckSelect()) {
            UpdateHeaderCheckBox();
        }
    }
}

ListCtrlSelectionMode ListCtrl::GetSelectionMode() const
{
    return m_selectionMode;
}

void ListCtrl::SetSelectionMode(ListCtrlSelectionMode mode)
{
    if (m_selectionMode == mode) {
        return;
    }
    if (mode == ListCtrlSelectionMode::kCell) {
        //Switching to cell selection mode: clear the current row selection first
        if (m_pData != nullptr) {
            std::vector<size_t> refreshIndexs;
            m_pData->SelectNone(refreshIndexs);
            if (!refreshIndexs.empty()) {
                RefreshDataItems(refreshIndexs);
                if (IsAutoCheckSelect()) {
                    UpdateHeaderCheckBox();
                }
            }
            //The cell selection state is reset as well
            m_pData->ClearCellSelection();
        }
    }
    else {
        //Switching back to row selection mode: clear the cell selection state
        if (m_pData != nullptr) {
            m_pData->ClearCellSelection();
        }
    }
    m_selectionMode = mode;
    m_bCellDragSelecting = false;
    m_bCellDragAdditive = false;
    m_hoverCell = HoverCellId();
    NotifyCellSelectionChanged();
    RefreshCellSelectionView();
}

ListCtrlCellId ListCtrl::GetCurrentCell() const
{
    if (m_pData != nullptr) {
        return m_pData->GetCurrentCell();
    }
    return ListCtrlCellId();
}

bool ListCtrl::SetCurrentCell(size_t nRowIndex, size_t nColumnId)
{
    if (m_selectionMode != ListCtrlSelectionMode::kCell) {
        return false;
    }
    if ((nRowIndex >= GetDataItemCount()) || !IsValidColumnId(nColumnId)) {
        return false;
    }
    ListCtrlCellId cell;
    cell.nRowIndex = nRowIndex;
    cell.nColumnId = nColumnId;

    ListCtrlCellId oldCell = GetCurrentCell();
    //Replacement semantics: clear the other selected cells, select only this cell, and sync the anchor/current cell
    std::vector<size_t> columnIdList;
    columnIdList.push_back(nColumnId);
    m_pData->SetCellSelectionRect(nRowIndex, nRowIndex, columnIdList, false);
    m_pData->SetAnchorCell(cell);
    m_pData->SetCurrentCell(cell);
    if (oldCell != cell) {
        NotifyCurrentCellChanged(cell);
    }
    NotifyCellSelectionChanged();
    RefreshCellSelectionView();
    return true;
}

bool ListCtrl::SetCellSelection(const ListCtrlCellId& anchorCell, const ListCtrlCellId& focusCell, bool bAdditive)
{
    if (m_selectionMode != ListCtrlSelectionMode::kCell) {
        return false;
    }
    if ((anchorCell.nRowIndex >= GetDataItemCount()) || !IsValidColumnId(anchorCell.nColumnId)) {
        return false;
    }
    //When the focus cell is invalid, only the anchor is updated and the selection set is not changed
    if (!focusCell.IsValid()) {
        m_pData->SetAnchorCell(anchorCell);
        return true;
    }
    if ((focusCell.nRowIndex >= GetDataItemCount()) || !IsValidColumnId(focusCell.nColumnId)) {
        return false;
    }
    ListCtrlCellId oldCell = GetCurrentCell();

    //Expand "anchor column -> focus column" into a consecutive list of column IDs according to the current column order (the column order may change, so the column index interval is authoritative)
    size_t nIndexBegin = GetColumnIndex(anchorCell.nColumnId);
    size_t nIndexEnd = GetColumnIndex(focusCell.nColumnId);
    if (!Box::IsValidItemIndex(nIndexBegin) || !Box::IsValidItemIndex(nIndexEnd)) {
        return false;
    }
    if (nIndexBegin > nIndexEnd) {
        const size_t nTemp = nIndexBegin;
        nIndexBegin = nIndexEnd;
        nIndexEnd = nTemp;
    }
    std::vector<size_t> columnIdList;
    for (size_t nColumnIndex = nIndexBegin; nColumnIndex <= nIndexEnd; ++nColumnIndex) {
        const size_t nColumnId = GetColumnId(nColumnIndex);
        if (IsValidColumnId(nColumnId)) {
            columnIdList.push_back(nColumnId);
        }
    }
    if (!IsMultiSelect()) {
        //Single-selection mode: ignore the additive semantics and select only the focus cell (at most one cell)
        columnIdList.clear();
        columnIdList.push_back(focusCell.nColumnId);
        m_pData->SetCellSelectionRect(focusCell.nRowIndex, focusCell.nRowIndex, columnIdList, false);
    }
    else {
        m_pData->SetCellSelectionRect(anchorCell.nRowIndex, focusCell.nRowIndex, columnIdList, bAdditive);
    }
    m_pData->SetAnchorCell(anchorCell);
    m_pData->SetCurrentCell(focusCell);
    if (oldCell != focusCell) {
        NotifyCurrentCellChanged(focusCell);
    }
    NotifyCellSelectionChanged();
    RefreshCellSelectionView();
    return true;
}

void ListCtrl::ClearCellSelection()
{
    if (m_pData == nullptr) {
        return;
    }
    //Refresh the UI and dispatch the selection changed event only when a selection state was really cleared, avoiding redundant refreshes and events when nothing changed
    if (m_pData->ClearCellSelection()) {
        NotifyCellSelectionChanged();
        RefreshCellSelectionView();
    }
}

void ListCtrl::GetCellSelection(ListCtrlCellId& anchorCell, ListCtrlCellId& focusCell) const
{
    if (m_pData != nullptr) {
        m_pData->GetCellSelection(anchorCell, focusCell);
    }
    else {
        anchorCell = ListCtrlCellId();
        focusCell = ListCtrlCellId();
    }
}

void ListCtrl::SetSelectedCells(const std::vector<ListCtrlCellId>& cellList, bool bClearOthers)
{
    if (m_selectionMode != ListCtrlSelectionMode::kCell) {
        return;
    }
    if (bClearOthers || !IsMultiSelect()) {
        //In single-selection mode, the existing selection must be cleared even if the caller asked not to clear the others: single-selection semantics allow at most one cell
        m_pData->ClearCellSelection();
    }
    bool bFirst = true;
    for (const ListCtrlCellId& cell : cellList) {
        if ((cell.nRowIndex >= GetDataItemCount()) || !IsValidColumnId(cell.nColumnId)) {
            continue;
        }
        if (!m_pData->IsSelectableElement(cell.nRowIndex)) {
            //Topmost rows and invisible rows do not take part in the cell selection
            continue;
        }
        m_pData->SetCellSelected(cell.nRowIndex, cell.nColumnId, true);
        if (bFirst) {
            m_pData->SetAnchorCell(cell);
            m_pData->SetCurrentCell(cell);
            bFirst = false;
        }
        if (!IsMultiSelect()) {
            //Single-selection mode: keep at most one cell, so it ends after taking the first valid cell of the list
            break;
        }
    }
    NotifyCellSelectionChanged();
    RefreshCellSelectionView();
}

size_t ListCtrl::GetSelectedCells(std::vector<ListCtrlCellId>& cellList) const
{
    cellList.clear();
    if ((m_selectionMode != ListCtrlSelectionMode::kCell) || (m_pData == nullptr)) {
        return 0;
    }
    const size_t nCount = GetDataItemCount();
    const size_t nColumnCount = GetColumnCount();
    for (size_t nRowIndex = 0; nRowIndex < nCount; ++nRowIndex) {
        if (!m_pData->IsSelectableElement(nRowIndex)) {
            //Topmost rows and invisible rows do not take part in the cell selection
            continue;
        }
        for (size_t nColumnIndex = 0; nColumnIndex < nColumnCount; ++nColumnIndex) {
            const size_t nColumnId = GetColumnId(nColumnIndex);
            if (!IsValidColumnId(nColumnId)) {
                continue;
            }
            if (!m_pData->IsCellSelected(nRowIndex, nColumnId)) {
                continue;
            }
            ListCtrlCellId cell;
            cell.nRowIndex = nRowIndex;
            cell.nColumnId = nColumnId;
            cellList.push_back(cell);
        }
    }
    return cellList.size();
}

bool ListCtrl::IsCellSelected(size_t nRowIndex, size_t nColumnId) const
{
    if ((m_selectionMode != ListCtrlSelectionMode::kCell) || (m_pData == nullptr)) {
        return false;
    }
    return m_pData->IsCellSelected(nRowIndex, nColumnId);
}

bool ListCtrl::IsCellCurrent(size_t nRowIndex, size_t nColumnId) const
{
    if (m_selectionMode != ListCtrlSelectionMode::kCell) {
        return false;
    }
    const ListCtrlCellId cell = GetCurrentCell();
    return cell.IsValid() && (cell.nRowIndex == nRowIndex) && (cell.nColumnId == nColumnId);
}

void ListCtrl::GetSelectedCellRanges(std::vector<ListCtrlCellRange>& rangeList) const
{
    rangeList.clear();
    if ((m_selectionMode != ListCtrlSelectionMode::kCell) || (m_pData == nullptr)) {
        return;
    }
    const size_t nRowCount = GetDataItemCount();
    const size_t nColumnCount = GetColumnCount();
    if ((nRowCount == 0) || (nColumnCount == 0)) {
        return;
    }

    //The area currently being extended vertically (merged by the column index interval)
    struct PendingRange
    {
        size_t nColumnIndexBegin;
        size_t nColumnIndexEnd;
        size_t nRowBegin;
        size_t nRowEnd;
    };
    std::vector<PendingRange> pending;   //Areas that were still being extended by the previous row
    std::vector<PendingRange> nextPending;
    std::vector<std::pair<size_t, size_t>> rowRuns;  //The consecutive selected column segments of this row (column index intervals)
    std::vector<bool> runUsed;

    for (size_t nRowIndex = 0; nRowIndex < nRowCount; ++nRowIndex) {
        //1. Compute the consecutive selected column segments of this row (topmost rows and invisible rows do not take part in the cell selection, and are treated as empty segments so that the area above them is settled here)
        rowRuns.clear();
        if (m_pData->IsSelectableElement(nRowIndex)) {
            bool bInRun = false;
            size_t nRunBegin = 0;
            for (size_t nColumnIndex = 0; nColumnIndex < nColumnCount; ++nColumnIndex) {
                const size_t nColumnId = GetColumnId(nColumnIndex);
                const bool bSelected = IsValidColumnId(nColumnId) &&
                                       m_pData->IsCellSelected(nRowIndex, nColumnId);
                if (bSelected && !bInRun) {
                    bInRun = true;
                    nRunBegin = nColumnIndex;
                }
                else if (!bSelected && bInRun) {
                    bInRun = false;
                    rowRuns.emplace_back(nRunBegin, nColumnIndex - 1);
                }
            }
            if (bInRun) {
                rowRuns.emplace_back(nRunBegin, nColumnCount - 1);
            }
        }

        //2. Merge vertically with the areas of the previous row (an area can only be extended when the column index intervals are exactly the same)
        nextPending.clear();
        runUsed.assign(rowRuns.size(), false);
        for (const PendingRange& p : pending) {
            bool bMatched = false;
            for (size_t r = 0; r < rowRuns.size(); ++r) {
                if (!runUsed[r] && (rowRuns[r].first == p.nColumnIndexBegin) &&
                    (rowRuns[r].second == p.nColumnIndexEnd)) {
                    runUsed[r] = true;
                    PendingRange np = p;
                    np.nRowEnd = nRowIndex;
                    nextPending.push_back(np);
                    bMatched = true;
                    break;
                }
            }
            if (!bMatched) {
                //This area can no longer be extended: settle it and output it as an external area
                ListCtrlCellRange range;
                range.nRowBegin = p.nRowBegin;
                range.nRowEnd = p.nRowEnd;
                range.nColumnIdBegin = GetColumnId(p.nColumnIndexBegin);
                range.nColumnIdEnd = GetColumnId(p.nColumnIndexEnd);
                if (range.IsValid()) {
                    rangeList.push_back(range);
                }
            }
        }
        //3. The segments newly appearing in this row become the starting point of new areas
        for (size_t r = 0; r < rowRuns.size(); ++r) {
            if (runUsed[r]) {
                continue;
            }
            PendingRange np;
            np.nColumnIndexBegin = rowRuns[r].first;
            np.nColumnIndexEnd = rowRuns[r].second;
            np.nRowBegin = nRowIndex;
            np.nRowEnd = nRowIndex;
            nextPending.push_back(np);
        }
        pending.swap(nextPending);
    }

    //Finish: output the areas that are still being extended
    for (const PendingRange& p : pending) {
        ListCtrlCellRange range;
        range.nRowBegin = p.nRowBegin;
        range.nRowEnd = p.nRowEnd;
        range.nColumnIdBegin = GetColumnId(p.nColumnIndexBegin);
        range.nColumnIdEnd = GetColumnId(p.nColumnIndexEnd);
        if (range.IsValid()) {
            rangeList.push_back(range);
        }
    }
}

void ListCtrl::NotifyCurrentCellChanged(const ListCtrlCellId& cell)
{
    SendEvent(kEventCurrentCellChanged, (WPARAM)cell.nRowIndex, (LPARAM)cell.nColumnId);
}

void ListCtrl::NotifyCellSelectionChanged()
{
    //List of selected areas: after merging consecutive columns and vertically adjacent rows, several non-contiguous areas may result
    std::vector<ListCtrlCellRange> rangeList;
    GetSelectedCellRanges(rangeList);
    if (rangeList.empty()) {
        SendEvent(kEventCellSelectionChanged, (WPARAM)0, (LPARAM)0, nullptr);
    }
    else {
        SendEvent(kEventCellSelectionChanged, (WPARAM)rangeList.size(), (LPARAM)0, &rangeList);
    }
}

void ListCtrl::RefreshCellSelectionView()
{
    if (m_pReportView != nullptr) {
        m_pReportView->Invalidate();
    }
}

void ListCtrl::UpdateHoverCell(size_t nRowIndex, size_t nColumnId)
{
    if ((nRowIndex == Box::InvalidIndex) || !IsValidColumnId(nColumnId) || (m_pData == nullptr)) {
        //No cell hit: clear the hover state
        ClearHoverCell();
        return;
    }
    //Convert the "display index" into the "row identity" for storage, decoupled from the display order (after sorting/adding/removing rows the hover highlight still follows the original data row)
    HoverCellId hoverCell;
    hoverCell.nRowId = m_pData->GetRowIdByDisplayIndex(nRowIndex);
    hoverCell.nColumnId = nColumnId;
    if (!hoverCell.IsValid()) {
        ClearHoverCell();
        return;
    }
    if (m_hoverCell != hoverCell) {
        m_hoverCell = hoverCell;
        RefreshCellSelectionView();
    }
}

void ListCtrl::ClearHoverCell()
{
    if (m_hoverCell.IsValid()) {
        m_hoverCell = HoverCellId();
        RefreshCellSelectionView();
    }
}

bool ListCtrl::IsCellHovered(size_t nRowIndex, size_t nColumnId) const
{
    if ((m_selectionMode != ListCtrlSelectionMode::kCell) || !m_hoverCell.IsValid() || (m_pData == nullptr)) {
        return false;
    }
    //Compare by row identity: decoupled from the display order
    return (m_pData->GetRowIdByDisplayIndex(nRowIndex) == m_hoverCell.nRowId) && (m_hoverCell.nColumnId == nColumnId);
}

bool ListCtrl::OnCellMouseEvent(const EventArgs& msg, size_t nRowIndex, size_t nColumnId)
{
    if (m_selectionMode != ListCtrlSelectionMode::kCell) {
        return false;
    }
    const bool bHitCell = (nRowIndex != Box::InvalidIndex) && IsValidColumnId(nColumnId);
    switch (msg.eventType) {
    case kEventMouseButtonDown:
    case kEventMouseRButtonDown:
        //Left and right button down have the same semantics (Shift extends / Ctrl toggles / plain click), and both enter the drag-selection state
        BeginCellDrag(msg, nRowIndex, nColumnId);
        break;
    case kEventMouseHover:
        //Update the cell the mouse is hovering over (used to paint the cell hover highlight), and suppress the row-level hover background at the same time
        UpdateHoverCell(nRowIndex, nColumnId);
        break;
    case kEventMouseMove:
        //Update the cell the mouse is hovering over
        UpdateHoverCell(nRowIndex, nColumnId);
        if (m_bCellDragSelecting && bHitCell) {
            //During dragging, the "selection" is driven by the rubber-band rectangle (ListCtrlReportView::OnFrameSelectionForCells);
            //here only the "current cell focus frame" follows the cell hit by the mouse (without clearing the selected cells)
            ListCtrlCellId cell;
            cell.nRowIndex = nRowIndex;
            cell.nColumnId = nColumnId;
            const ListCtrlCellId oldCell = GetCurrentCell();
            if (oldCell != cell) {
                if (m_pData != nullptr) {
                    m_pData->SetCurrentCell(cell);
                }
                NotifyCurrentCellChanged(cell);
                RefreshCellSelectionView();
            }
        }
        break;
    case kEventMouseButtonUp:
    case kEventMouseRButtonUp:
        m_bCellDragSelecting = false;
        m_bCellDragAdditive = false;
        break;
    default:
        return false;
    }
    return true;
}

void ListCtrl::BeginCellDrag(const EventArgs& msg, size_t nRowIndex, size_t nColumnId)
{
    if (m_pData == nullptr) {
        return;
    }
    m_bCellDragSelecting = false;
    m_bCellDragAdditive = false;
    if ((nRowIndex == Box::InvalidIndex) || !IsValidColumnId(nColumnId)) {
        //No cell hit (blank area): clear the cell selection
        ClearCellSelection();
        return;
    }
    ListCtrlCellId cell;
    cell.nRowIndex = nRowIndex;
    cell.nColumnId = nColumnId;

    const bool bShiftDown = IsKeyDown(msg, ModifierKey::kShift);
    const bool bCtrlDown = IsKeyDown(msg, ModifierKey::kControl);

    if ((msg.eventType == kEventMouseRButtonDown) && m_pData->IsCellSelected(nRowIndex, nColumnId)) {
        //Right click on an "already selected" cell: keep the current selection state unchanged (so that a context menu can be popped up to act on the selected cells),
        //and only sync the anchor/current cell to this cell (without changing the selection set and without dispatching the selection changed event).
        //The semantics are consistent with row selection mode: see the handling of right-click on a selected item in ListBox::ListCtrlSelectItem
        m_pData->SetAnchorCell(cell);
        const ListCtrlCellId oldCell = GetCurrentCell();
        m_pData->SetCurrentCell(cell);
        if (oldCell != cell) {
            NotifyCurrentCellChanged(cell);
        }
        RefreshCellSelectionView();
        //Note: return directly here and do not enter the drag-selection state; otherwise further dragging would be cleared by the rubber-band logic according to the "replace" semantics
        return;
    }
    if (!IsMultiSelect()) {
        //Single-selection mode: ignore the multi-select semantics of Shift/Ctrl and always treat it as a "plain click" (at most one cell is selected);
        //also do not enter the drag-selection state, so that the "current cell focus frame" following the mouse while the selection set stays unchanged does not cause visual misalignment
        SetCurrentCell(nRowIndex, nColumnId);
        m_bCellDragAdditive = false;
        return;
    }
    if (bShiftDown) {
        //Shift (including Ctrl+Shift): extend from the anchor to the clicked cell; Ctrl+Shift appends, otherwise it replaces
        ListCtrlCellId anchorCell;
        ListCtrlCellId focusCell;
        GetCellSelection(anchorCell, focusCell);
        if (!anchorCell.IsValid()) {
            anchorCell = cell;
        }
        SetCellSelection(anchorCell, cell, bCtrlDown);
        m_bCellDragAdditive = bCtrlDown;
    }
    else if (bCtrlDown) {
        //Ctrl+click: toggle the selection state of this cell (clicking again deselects it) without clearing the other selected cells
        const bool bSelected = m_pData->IsCellSelected(nRowIndex, nColumnId);
        m_pData->SetCellSelected(nRowIndex, nColumnId, !bSelected);
        m_pData->SetAnchorCell(cell);
        const ListCtrlCellId oldCell = GetCurrentCell();
        m_pData->SetCurrentCell(cell);
        if (oldCell != cell) {
            NotifyCurrentCellChanged(cell);
        }
        NotifyCellSelectionChanged();
        RefreshCellSelectionView();
        //Ctrl+drag: enter additive mode from then on
        m_bCellDragAdditive = true;
    }
    else {
        //Plain click: select this cell (clearing the others) and sync the anchor/current cell
        SetCurrentCell(nRowIndex, nColumnId);
        m_bCellDragAdditive = false;
    }
    //After the mouse button is pressed, enter the drag-selection state
    m_bCellDragSelecting = true;
}

bool ListCtrl::OnComboKeyDown(const EventArgs& msg)
{
    if ((msg.eventType != kEventKeyDown) || !IsComboEditOpened()) {
        return false;
    }
    //Combo candidate edit state: the up/down arrow keys move the candidate, Return commits, Esc cancels; all of them must be handled first and the keys consumed
    if (msg.vkCode == kVK_UP) {
        m_pComboFlyout->MoveSelection(-1);
        return true;
    }
    if (msg.vkCode == kVK_DOWN) {
        m_pComboFlyout->MoveSelection(1);
        return true;
    }
    if (msg.vkCode == kVK_RETURN) {
        //Commit the currently selected candidate item
        OnComboCandidateSelected(m_pComboFlyout->GetSelectedIndex());
        return true;
    }
    if (msg.vkCode == kVK_ESCAPE) {
        //Cancel the edit: close the flyout without writing the data back
        CloseComboFlyout();
        return true;
    }
    //Other list navigation keys: while the flyout is open they do not take part in the row/cell navigation of the list and are consumed directly, so that a leaked key does not move the selection
    const bool bNavigationKey = (msg.vkCode == kVK_LEFT) || (msg.vkCode == kVK_RIGHT) ||
                                (msg.vkCode == kVK_PRIOR) || (msg.vkCode == kVK_NEXT) ||
                                (msg.vkCode == kVK_HOME) || (msg.vkCode == kVK_END);
    if (bNavigationKey) {
        return true;
    }
    return false;
}

bool ListCtrl::OnCellKeyDown(const EventArgs& msg)
{
    if ((m_selectionMode != ListCtrlSelectionMode::kCell) || (msg.eventType != kEventKeyDown)) {
        return false;
    }
    const bool bCtrlDown = Keyboard::IsKeyDown(kVK_CONTROL);
    const bool bShiftDown = Keyboard::IsKeyDown(kVK_SHIFT);

    //Ctrl + A: select all cells (effective only in multi-select mode; single-selection mode allows at most one cell, so this key is ignored)
    if (bCtrlDown && ((msg.vkCode == 'A') || (msg.vkCode == 'a'))) {
        if (IsMultiSelect()) {
            SetSelectAll();
        }
        return true;
    }
    //Ctrl + Space: toggle the selection state of the current cell (effective only in multi-select mode;
    //single-selection mode allows at most one cell, consistent with row selection mode, which does not support this key either, so the key is ignored)
    if (bCtrlDown && (msg.vkCode == kVK_SPACE)) {
        if (IsMultiSelect()) {
            const ListCtrlCellId cell = GetCurrentCell();
            if (cell.IsValid()) {
                const bool bSelected = m_pData->IsCellSelected(cell.nRowIndex, cell.nColumnId);
                m_pData->SetCellSelected(cell.nRowIndex, cell.nColumnId, !bSelected);
                NotifyCellSelectionChanged();
                RefreshCellSelectionView();
            }
        }
        return true;
    }

    //Ctrl + C: copy the selected cell area to the clipboard (TSV format); Ctrl + Shift + C: copy it in CSV format
    if (bCtrlDown && ((msg.vkCode == 'C') || (msg.vkCode == 'c'))) {
        //Consume the key regardless of whether the switch is on, so that it does not keep bubbling to other handling logic
        CopySelectionToClipboard(bShiftDown);
        return true;
    }

    //F2: enter the edit state of the current cell (entered immediately, without the 600ms timer used when entering the edit state)
    if (msg.vkCode == kVK_F2) {
        EnterCurrentCellEdit();
        //Consume the key whether or not the edit state was entered successfully, so that it does not keep bubbling
        return true;
    }

    //Arrow keys: move the current cell
    const bool bArrowKey = (msg.vkCode == kVK_UP) || (msg.vkCode == kVK_DOWN) ||
                            (msg.vkCode == kVK_LEFT) || (msg.vkCode == kVK_RIGHT) ||
                            (msg.vkCode == kVK_PRIOR) || (msg.vkCode == kVK_NEXT) ||
                            (msg.vkCode == kVK_HOME) || (msg.vkCode == kVK_END);
    if (!bArrowKey) {
        return false;
    }
    const size_t nRowCount = GetDataItemCount();
    const size_t nColumnCount = GetColumnCount();
    if ((nRowCount == 0) || (nColumnCount == 0)) {
        return true;
    }

    ListCtrlCellId cell = GetCurrentCell();
    if (!cell.IsValid()) {
        //No current cell: position it at the first cell
        SetCurrentCell(0, GetColumnId(0));
        return true;
    }

    const size_t nOldColumnIndex = GetColumnIndex(cell.nColumnId);
    size_t nNewRowIndex = cell.nRowIndex;
    size_t nNewColumnIndex = Box::IsValidItemIndex(nOldColumnIndex) ? nOldColumnIndex : 0;

    if (msg.vkCode == kVK_UP) {
        if (nNewRowIndex > 0) {
            --nNewRowIndex;
        }
    }
    else if (msg.vkCode == kVK_DOWN) {
        if (nNewRowIndex + 1 < nRowCount) {
            ++nNewRowIndex;
        }
    }
    else if (msg.vkCode == kVK_LEFT) {
        if (nNewColumnIndex > 0) {
            --nNewColumnIndex;
        }
    }
    else if (msg.vkCode == kVK_RIGHT) {
        if (nNewColumnIndex + 1 < nColumnCount) {
            ++nNewColumnIndex;
        }
    }
    else if (msg.vkCode == kVK_PRIOR) {
        nNewRowIndex = (nNewRowIndex > 5) ? (nNewRowIndex - 5) : 0;
    }
    else if (msg.vkCode == kVK_NEXT) {
        nNewRowIndex = nNewRowIndex + 5;
        if (nNewRowIndex >= nRowCount) {
            nNewRowIndex = nRowCount - 1;
        }
    }
    else if (msg.vkCode == kVK_HOME) {
        //Home: the first column of the current row; Ctrl+Home: the first cell of the table
        if (bCtrlDown) {
            nNewRowIndex = 0;
        }
        nNewColumnIndex = 0;
    }
    else if (msg.vkCode == kVK_END) {
        //End: the last column of the current row; Ctrl+End: the last cell of the table
        if (bCtrlDown) {
            nNewRowIndex = nRowCount - 1;
        }
        nNewColumnIndex = nColumnCount - 1;
    }

    const size_t nNewColumnId = GetColumnId(nNewColumnIndex);
    if (!IsValidColumnId(nNewColumnId)) {
        return true;
    }

    ListCtrlCellId newFocusCell;
    newFocusCell.nRowIndex = nNewRowIndex;
    newFocusCell.nColumnId = nNewColumnId;

    if (bCtrlDown && !bShiftDown) {
        //Ctrl+arrow: move only the focus of the current cell without changing the selection set
        const ListCtrlCellId oldCell = GetCurrentCell();
        m_pData->SetCurrentCell(newFocusCell);
        if (oldCell != newFocusCell) {
            NotifyCurrentCellChanged(newFocusCell);
        }
        RefreshCellSelectionView();
    }
    else if (bShiftDown && IsMultiSelect()) {
        //Shift(+Ctrl): extend the area from the anchor; Ctrl+Shift appends, Shift replaces (multi-select mode only; single-selection mode degrades to a plain move)
        ListCtrlCellId anchorCell;
        ListCtrlCellId focusCell;
        GetCellSelection(anchorCell, focusCell);
        if (!anchorCell.IsValid()) {
            anchorCell = cell;
        }
        SetCellSelection(anchorCell, newFocusCell, bCtrlDown);
    }
    else {
        //Plain arrow keys: move the current cell (single-selection replaces that cell)
        SetCurrentCell(nNewRowIndex, nNewColumnId);
    }
    //Make sure the row of the current cell is visible
    if (m_pReportView != nullptr) {
        m_pReportView->EnsureDataItemVisible(nNewRowIndex, false);
    }
    return true;
}

void ListCtrl::GetDisplayDataItems(std::vector<size_t>& itemIndexList) const
{
    itemIndexList.clear();
    if (m_listCtrlType == ListCtrlType::Report) {
        if (m_pReportView != nullptr) {
            m_pReportView->GetDisplayDataItems(itemIndexList);
        }
    }
    else if (m_listCtrlType == ListCtrlType::Icon) {
        if (m_pIconView != nullptr) {
            m_pIconView->GetDisplayDataItems(itemIndexList);
        }
    }
    else if (m_listCtrlType == ListCtrlType::List) {
        if (m_pListView != nullptr) {
            m_pListView->GetDisplayDataItems(itemIndexList);
        }
    }
}

size_t ListCtrl::GetTopDataItem() const
{
    size_t nTopItemIndex = Box::InvalidIndex;
    if (m_listCtrlType == ListCtrlType::Report) {
        if (m_pReportView != nullptr) {
            nTopItemIndex = m_pReportView->GetTopElementIndex();
        }
    }
    else if (m_listCtrlType == ListCtrlType::Icon) {
        if (m_pIconView != nullptr) {
            nTopItemIndex = m_pIconView->GetTopElementIndex();
        }
    }
    else if (m_listCtrlType == ListCtrlType::List) {
        if (m_pListView != nullptr) {
            nTopItemIndex = m_pListView->GetTopElementIndex();
        }
    }
    return nTopItemIndex;
}

bool ListCtrl::IsDataItemDisplay(size_t itemIndex) const
{
    bool bItemVisible = false;
    if (m_listCtrlType == ListCtrlType::Report) {
        if (m_pReportView != nullptr) {
            bItemVisible = m_pReportView->IsDataItemDisplay(itemIndex);
        }
    }
    else if (m_listCtrlType == ListCtrlType::Icon) {
        if (m_pIconView != nullptr) {
            bItemVisible = m_pIconView->IsDataItemDisplay(itemIndex);
        }
    }
    else if (m_listCtrlType == ListCtrlType::List) {
        if (m_pListView != nullptr) {
            bItemVisible = m_pListView->IsDataItemDisplay(itemIndex);
        }
    }
    return bItemVisible;
}

bool ListCtrl::EnsureDataItemVisible(size_t itemIndex, bool bToTop)
{
    bool bRet = false;
    if (m_listCtrlType == ListCtrlType::Report) {
        if (m_pReportView != nullptr) {
            bRet = m_pReportView->EnsureDataItemVisible(itemIndex, bToTop);
        }
    }
    else if (m_listCtrlType == ListCtrlType::Icon) {
        if (m_pIconView != nullptr) {
            bRet = m_pIconView->EnsureDataItemVisible(itemIndex, bToTop);
        }
    }
    else if (m_listCtrlType == ListCtrlType::List) {
        if (m_pListView != nullptr) {
            bRet = m_pListView->EnsureDataItemVisible(itemIndex, bToTop);
        }
    }
    return bRet;
}

void ListCtrl::Refresh(bool bSync)
{
    if (!IsInited()) {
        return;
    }
    if (m_bEnableRefresh) {
        if (m_listCtrlType == ListCtrlType::Report) {
            if (m_pReportView != nullptr) {
                m_pReportView->Refresh(bSync);
            }
        }
        else if (m_listCtrlType == ListCtrlType::Icon) {
            if (m_pIconView != nullptr) {
                m_pIconView->Refresh(bSync);
            }
        }
        else if (m_listCtrlType == ListCtrlType::List) {
            if (m_pListView != nullptr) {
                m_pListView->Refresh(bSync);
            }
        }
    }
}

void ListCtrl::RefreshDataItems(const std::vector<size_t>& dataItemIndexs)
{
    if (m_bEnableRefresh && !dataItemIndexs.empty()) {
        if (m_listCtrlType == ListCtrlType::Report) {
            if (m_pReportView != nullptr) {
                m_pReportView->RefreshElements(dataItemIndexs);
            }
        }
        else if (m_listCtrlType == ListCtrlType::Icon) {
            if (m_pIconView != nullptr) {
                m_pIconView->RefreshElements(dataItemIndexs);
            }
        }
        else if (m_listCtrlType == ListCtrlType::List) {
            if (m_pListView != nullptr) {
                m_pListView->RefreshElements(dataItemIndexs);
            }
        }        
    }
}

bool ListCtrl::SetEnableRefresh(bool bEnable)
{
    bool bOldEnable = m_bEnableRefresh;
    m_bEnableRefresh = bEnable;
    return bOldEnable;
}

bool ListCtrl::IsEnableRefresh() const
{
    return m_bEnableRefresh;
}

bool ListCtrl::RefreshSubItem(size_t dataItemIndex, size_t columnIndex)
{
    size_t columnId = GetColumnId(columnIndex);
    return RefreshSubItemById(dataItemIndex, columnId);
}

bool ListCtrl::RefreshSubItemById(size_t dataItemIndex, size_t columnId)
{
    if (!m_bEnableRefresh) {
        return false;
    }
    //Single-cell refresh is supported by the Report view only: the Icon/List views have no concept of columns
    if (m_listCtrlType != ListCtrlType::Report) {
        return false;
    }
    if (m_pReportView == nullptr) {
        return false;
    }
    if ((dataItemIndex == Box::InvalidIndex) || (dataItemIndex >= GetDataItemCount())) {
        return false;
    }
    if (!IsValidColumnId(columnId)) {
        return false;
    }
    return m_pReportView->RefreshSubItemData(dataItemIndex, columnId);
}

bool ListCtrl::SetColumnVisible(size_t columnIndex, bool bColumnVisible)
{
    ListCtrlHeaderItem* pHeaderItem = GetColumn(columnIndex);
    if (pHeaderItem == nullptr) {
        return false;
    }
    if (pHeaderItem->IsColumnVisible() == bColumnVisible) {
        return true;
    }
    pHeaderItem->SetColumnVisible(bColumnVisible);
    return true;
}

bool ListCtrl::SetColumnVisibleById(size_t columnId, bool bColumnVisible)
{
    size_t columnIndex = GetColumnIndex(columnId);
    if (columnIndex == Box::InvalidIndex) {
        return false;
    }
    return SetColumnVisible(columnIndex, bColumnVisible);
}

bool ListCtrl::IsColumnVisible(size_t columnIndex) const
{
    ListCtrlHeaderItem* pHeaderItem = GetColumn(columnIndex);
    if (pHeaderItem == nullptr) {
        return false;
    }
    return pHeaderItem->IsColumnVisible();
}

bool ListCtrl::IsColumnVisibleById(size_t columnId) const
{
    size_t columnIndex = GetColumnIndex(columnId);
    if (columnIndex == Box::InvalidIndex) {
        return false;
    }
    return IsColumnVisible(columnIndex);
}

void ListCtrl::SetAutoCheckSelect(bool bAutoCheckSelect)
{
    m_bAutoCheckSelect = bAutoCheckSelect;
    m_pData->SetAutoCheckSelect(bAutoCheckSelect);
}

bool ListCtrl::IsAutoCheckSelect() const
{
    return m_bAutoCheckSelect;
}

bool ListCtrl::SetHeaderShowCheckBox(bool bShow)
{
    m_bHeaderShowCheckBox = bShow;
    bool bRet = false;
    if (IsInited()) {
        ListCtrlHeader* pHeaderCtrl = GetHeaderCtrl();
        ASSERT(pHeaderCtrl != nullptr);
        if (pHeaderCtrl != nullptr) {
            bRet = pHeaderCtrl->SetShowCheckBox(bShow);
        }
    }
    else {
        bRet = true;
    }
    return bRet;
}

bool ListCtrl::IsHeaderShowCheckBox() const
{
    bool bRet = false;
    if (IsInited()) {
        ListCtrlHeader* pHeaderCtrl = GetHeaderCtrl();
        ASSERT(pHeaderCtrl != nullptr);
        if (pHeaderCtrl != nullptr) {
            bRet = pHeaderCtrl->IsShowCheckBox();
        }
    }
    else {
        bRet = m_bHeaderShowCheckBox;
    }
    return bRet;
}

void ListCtrl::SetDataItemShowCheckBox(bool bShow)
{
    if (m_bDataItemShowCheckBox != bShow) {
        m_bDataItemShowCheckBox = bShow;
        if (IsInited()) {
            Refresh();
        }
    }
}

bool ListCtrl::IsDataItemShowCheckBox() const
{
    return m_bDataItemShowCheckBox;
}

bool ListCtrl::SetDataItemCheck(size_t itemIndex, bool bCheck)
{
    bool bChanged = false;
    bool bRet = m_pData->SetDataItemChecked(itemIndex, bCheck, bChanged);
    if (bChanged) {
        UpdateHeaderCheckBox();
    }
    return bRet;
}

bool ListCtrl::IsDataItemCheck(size_t itemIndex) const
{
    return m_pData->IsDataItemChecked(itemIndex);
}

void ListCtrl::OnItemEnterEditMode(size_t itemIndex, size_t nColumnId,
                                   IListBoxItem* pItem, ListCtrlLabel* pSubItem)
{
    ASSERT(itemIndex < GetDataItemCount());
    ASSERT(GetColumnIndex(nColumnId) < GetColumnCount());
    ASSERT((pItem != nullptr) && (pSubItem != nullptr));
    ASSERT(pItem->GetElementIndex() == itemIndex);
    if (itemIndex >= GetDataItemCount()) {
        return;
    }
    if (pItem->GetElementIndex() != itemIndex) {
        return;
    }
    size_t nColumnIndex = GetColumnIndex(nColumnId);
    if (nColumnIndex >= GetColumnCount()) {
        return;
    }

    ListCtrlEditParam editParam;
    editParam.listCtrlType = m_listCtrlType;
    editParam.nItemIndex = itemIndex;
    editParam.nColumnId = nColumnId;
    editParam.nColumnIndex = nColumnIndex;
    editParam.pItem = pItem;
    editParam.pSubItem = pSubItem;

    //An edit request is valid only when both the "edit state" and the "edit parameters" are valid; if either is invalid, entering the edit state is abandoned
    //(this is the synchronous click path, and normally both should be valid, hence the assertion)
    if (!IsValidItemEditState(editParam) || !IsValidItemEditParam(editParam)) {
        ASSERT(0);
        return;
    }

    //The combo candidate editor does not need the rich text edit box class, but the other editors require it
    const ListCtrlEditorType editorType = GetColumnEditorTypeById(nColumnId);
    if (editorType != ListCtrlEditorType::kEditCombo) {
        std::string editClass = GetRichEditClass();
        ASSERT(!editClass.empty());
        if (editClass.empty()) {
            return;
        }
    }

    //Start a timer, executed only once (reason for using a timer: to avoid affecting the double-click operation)
    m_editModeFlag.Cancel();
    std::function<void()> editModeCallback = UiBind(&ListCtrl::OnItemEditMode, this, editParam);
    TimerManager& timer = GlobalManager::Instance().Timer();
    timer.AddTimer(m_editModeFlag.GetWeakFlag(), editModeCallback, 600, 1);
}

bool ListCtrl::IsValidItemEditState(const ListCtrlEditParam& editParam) const
{
    if (!IsEnableItemEdit()) {
        return false;
    }
    if (editParam.listCtrlType != m_listCtrlType) {
        return false;
    }

    if (editParam.listCtrlType == ListCtrlType::Icon) {
        //Icon view
        ListCtrlIconViewItem* pItem = dynamic_cast<ListCtrlIconViewItem*>(editParam.pItem);
        if ((pItem == nullptr) || !pItem->IsVisible() || !pItem->IsSelected() || !pItem->IsFocused()) {
            return false;
        }
        if (m_pIconView == nullptr) {
            return false;
        }
    }
    else if (editParam.listCtrlType == ListCtrlType::List) {
        //List view
        ListCtrlListViewItem* pItem = dynamic_cast<ListCtrlListViewItem*>(editParam.pItem);
        if ((pItem == nullptr) || !pItem->IsVisible() || !pItem->IsSelected() || !pItem->IsFocused()) {
            return false;
        }
        if (m_pListView == nullptr) {
            return false;
        }
    }
    else {
        //Report view
        ListCtrlItem* pItem = dynamic_cast<ListCtrlItem*>(editParam.pItem);
        if (pItem == nullptr) {
            ASSERT(pItem != nullptr);
            return false;
        }
        if (!pItem->IsVisible()) {
            return false;
        }
        if (m_selectionMode != ListCtrlSelectionMode::kCell) {
            //Row selection mode: keep the original requirement of "selected and focused"
            ASSERT(pItem->IsSelected() && pItem->IsFocused());
            if (!pItem->IsSelected() || !pItem->IsFocused()) {
                return false;
            }
        }
        else {
            //Cell selection mode: before entering the edit state, the target cell must still be the "current cell".
            //Clicking to enter the edit state goes through the 600ms timer path; if the user clicks another cell before the timer fires,
            //the "current cell" has already changed while the row item of the target cell is still visible, so the timer would still apply to the old cell
            //and the old cell would wrongly enter the edit state (which is not expected); therefore this check compares against the "current cell".
            const ListCtrlCellId curCell = GetCurrentCell();
            if (!curCell.IsValid() ||
                (curCell.nRowIndex != editParam.nItemIndex) ||
                (curCell.nColumnId != editParam.nColumnId)) {
                return false;
            }
        }
        //In cell selection mode the "row selection" does not take part (the row selection is already cleared when switching to kCell), so it is not required
        ASSERT(m_pReportView != nullptr);
        if (m_pReportView == nullptr) {
            return false;
        }
    }
    return true;
}

bool ListCtrl::IsValidItemEditParam(const ListCtrlEditParam& editParam) const
{
    if (editParam.listCtrlType != m_listCtrlType) {
        return false;
    }
    if (editParam.listCtrlType == ListCtrlType::Icon) {
        //Icon view
        ListCtrlIconViewItem* pItem = dynamic_cast<ListCtrlIconViewItem*>(editParam.pItem);
        if (pItem == nullptr) {
            return false;
        }
        size_t nDataItemIndex = editParam.nItemIndex;
        ListCtrlIconViewItem* pDestItem = nullptr;
        ListCtrlIconViewItem* pNextItem = GetFirstDisplayIconItem();
        while (pNextItem != nullptr) {
            if (pNextItem->GetElementIndex() == nDataItemIndex) {
                pDestItem = pNextItem;
                break;
            }
            pNextItem = GetNextDisplayIconItem(pNextItem);
        }
        if (pDestItem != pItem) {
            //Already changed
            return false;
        }

        size_t nSubItemIndex = pItem->GetItemIndex(editParam.pSubItem);
        ASSERT(nSubItemIndex < pItem->GetItemCount());
        if (nSubItemIndex >= pItem->GetItemCount()) {
            return false;
        }

        size_t nColumnIndex = editParam.nColumnIndex;
        if (GetColumnId(nColumnIndex) != editParam.nColumnId) {
            return false;
        }
    }
    else if (editParam.listCtrlType == ListCtrlType::List) {
        //List view
        ListCtrlListViewItem* pItem = dynamic_cast<ListCtrlListViewItem*>(editParam.pItem);
        if (pItem == nullptr) {
            return false;
        }
        size_t nDataItemIndex = editParam.nItemIndex;        
        ListCtrlListViewItem* pDestItem = nullptr;
        ListCtrlListViewItem* pNextItem = GetFirstDisplayListItem();
        while (pNextItem != nullptr) {
            if (pNextItem->GetElementIndex() == nDataItemIndex) {
                pDestItem = pNextItem;
                break;
            }
            pNextItem = GetNextDisplayListItem(pNextItem);
        }
        if (pDestItem != pItem) {
            //Already changed
            return false;
        }

        size_t nSubItemIndex = pItem->GetItemIndex(editParam.pSubItem);
        ASSERT(nSubItemIndex < pItem->GetItemCount());
        if (nSubItemIndex >= pItem->GetItemCount()) {
            return false;
        }

        size_t nColumnIndex = editParam.nColumnIndex;
        if (GetColumnId(nColumnIndex) != editParam.nColumnId) {
            return false;
        }
    }
    else {
        //Report view
        ListCtrlItem* pItem = dynamic_cast<ListCtrlItem*>(editParam.pItem);
        ListCtrlSubItem* pSubItem = dynamic_cast<ListCtrlSubItem*>(editParam.pSubItem);
        if ((pItem == nullptr) || (pSubItem == nullptr)) {
            return false;
        }
        size_t nDataItemIndex = editParam.nItemIndex;
        size_t nColumnIndex = editParam.nColumnIndex;
        ListCtrlItem* pDestItem = nullptr;
        ListCtrlItem* pNextItem = GetFirstDisplayItem();
        while (pNextItem != nullptr) {
            if (pNextItem->GetElementIndex() == nDataItemIndex) {
                pDestItem = pNextItem;
                break;
            }
            pNextItem = GetNextDisplayItem(pNextItem);
        }
        if (pDestItem != pItem) {
            //Already changed
            return false;
        }
        if (pItem->GetSubItemIndex(pSubItem) != nColumnIndex) {
            return false;
        }
        if (GetColumnId(nColumnIndex) != editParam.nColumnId) {
            return false;
        }
    }
    return true;
}

void ListCtrl::OnItemEditMode(ListCtrlEditParam editParam)
{
    //If either the edit state or the edit parameters are invalid, entering the edit state is abandoned
    //(for example, when the timer fires the data row has already been scrolled/reused, so the edit parameters may have become invalid)
    if (!IsValidItemEditState(editParam) || !IsValidItemEditParam(editParam)) {
        return;
    }
    //Dispatch according to the editor type declared for the column: the combo candidate editor does not use the rich text edit box
    const ListCtrlEditorType editorType = GetColumnEditorTypeById(editParam.nColumnId);
    if (editorType == ListCtrlEditorType::kEditCombo) {
        //Combo candidate editor: pop up a candidate flyout below the cell (supported by both row selection mode and cell selection mode)
        EnterComboEditMode(editParam, editParam.pSubItem);
        return;
    }
    if (editorType == ListCtrlEditorType::kEditDate) {
        //Date editor: float a DateTime control over the cell and pop up a month calendar once it has focus (the rich text edit box is not used)
        EnterDateEditMode(editParam, editParam.pSubItem);
        return;
    }

    std::string editClass = GetRichEditClass();
    if (editClass.empty()) {
        return;
    }
    if (m_pRichEdit == nullptr) {
        m_pRichEdit = new RichEdit(GetWindow());
        AddItem(m_pRichEdit);
        m_pRichEdit->SetClass(editClass);
        //Record the character whitelist declared on the control class, so that a non-number editor can restore it (avoiding it being overwritten by the temporary whitelist of the number editor when the control is reused)
        m_richEditLimitChars = m_pRichEdit->GetLimitChars();
    }
    else if(GetItemCount() > 0) {
        //Move the Edit control to the end
        size_t nItem = GetItemIndex(m_pRichEdit);
        size_t nLastItemIndex = GetItemCount() - 1;
        if (nItem != nLastItemIndex) {
            SetItemIndex(m_pRichEdit, nLastItemIndex);
        }
    }
    if (m_pRichEdit == nullptr) {
        return;
    }

    ListCtrlLabel* pSubItem = editParam.pSubItem;
    if (pSubItem == nullptr) {
        return;
    }
    size_t nDataItemIndex = editParam.nItemIndex;

    std::string sOldItemText = pSubItem->GetText();
    UiRect rcItem = pSubItem->GetTextRect();
    UiPoint offsetPt = pSubItem->GetScrollOffsetInScrollBox();
    rcItem.Offset(-offsetPt.x, -offsetPt.y);
    UiRect rect = GetRect();
    UiMargin rcMargin(rcItem.left - rect.left, rcItem.top - rect.top, 0, 0);
    m_pRichEdit->SetMargin(rcMargin, false);
    m_pRichEdit->SetFloat(true);
    m_pRichEdit->SetVisible(true);
    //Restrict the characters that can be entered according to the editor type: the number editor only allows digits, a sign and a decimal point
    //(the RichEdit editor is reused, so the restriction must be set again every time the edit state is entered, to avoid a leftover restriction from the previous edit)
    if (editorType == ListCtrlEditorType::kEditNumber) {
        m_pRichEdit->SetLimitChars("0123456789+-.");
    }
    else {
        //Non-number editor: restore the character whitelist declared on the control class (possibly empty, meaning no restriction)
        m_pRichEdit->SetLimitChars(m_richEditLimitChars);
    }
    m_pRichEdit->SetText(sOldItemText);
    m_pRichEdit->SetSelAll();
    UpdateRichEditSize(pSubItem);

    //Restore the focus: restore the focus of the edit box to the original list control
    auto RestoreItemFocus = [this, nDataItemIndex]() {
            if ((m_pRichEdit != nullptr) && m_pRichEdit->IsVisible() && m_pRichEdit->IsFocused()) {
                ListCtrlItem* pDestItem = nullptr;
                ListCtrlItem* pNextItem = GetFirstDisplayItem();
                while (pNextItem != nullptr) {
                    if (pNextItem->GetElementIndex() == nDataItemIndex) {
                        pDestItem = pNextItem;
                        break;
                    }
                    pNextItem = GetNextDisplayItem(pNextItem);
                }
                if (pDestItem != nullptr) {
                    pDestItem->SetFocus();
                }
            }
        };

    //Trigger the event: start editing
    ListCtrlEditParam enterEditParam = editParam;
    enterEditParam.sNewText.clear();
    enterEditParam.bCancelled = false;
    SendEvent(kEventEnterEdit, (WPARAM)&enterEditParam);
    if (enterEditParam.bCancelled || !IsValidItemEditState(editParam) || !IsValidItemEditParam(editParam)) {
        //The state is already invalid, or the user cancelled the edit
        ClearEditEvents();
        RestoreItemFocus();
        LeaveEditMode();
        return;
    }
    if (m_selectionMode == ListCtrlSelectionMode::kCell) {
        //Record the cell being edited (cleared by ClearEditEvents when the edit state is left)
        m_editingCell.nRowIndex = editParam.nItemIndex;
        m_editingCell.nColumnId = editParam.nColumnId;
    }

    m_pRichEdit->SetFloat(true);
    m_pRichEdit->SetVisible(true);
    m_pRichEdit->SetFocus();

    //When the text changes, automatically adjust the size of the edit box
    m_pRichEdit->DetachEvent(kEventTextChanged);
    m_pRichEdit->AttachTextChanged([this, pSubItem](const EventArgs&) {
        UpdateRichEditSize(pSubItem);
        return true;
        });

    //When the editing ends, trigger the event
    auto OnLeaveRichEdit = [this, sOldItemText, RestoreItemFocus, editParam]() -> bool {
        std::string sNewItemText;
        if ((m_pRichEdit != nullptr) && m_pRichEdit->IsVisible()) {
            sNewItemText = m_pRichEdit->GetText();
            m_pRichEdit->SetVisible(false);                     
        }
        if ((sNewItemText != sOldItemText) && IsValidItemEditParam(editParam)) {
            //The text content has changed
            return OnItemEdited(editParam, sNewItemText);
        }
        return false;
        };

    //Press Enter: apply the modification
    m_pRichEdit->DetachEvent(kEventReturn);
    m_pRichEdit->AttachReturn([this, RestoreItemFocus, OnLeaveRichEdit](const EventArgs&) {
        ClearEditEvents();
        RestoreItemFocus();
        if (OnLeaveRichEdit() && (m_selectionMode == ListCtrlSelectionMode::kCell)) {
            //Cell selection mode: after a successful commit, the current cell moves down by one row
            MoveCurrentCellToNextRow();
        }
        return false;
        });
    //Press ESC: cancel the modification
    m_pRichEdit->DetachEvent(kEventKeyDown);
    m_pRichEdit->AttachEvent(kEventKeyDown, [this, RestoreItemFocus](const EventArgs& msg) {
        if ((msg.eventType == kEventKeyDown) && (msg.vkCode == kVK_ESCAPE)) {
            ClearEditEvents();
            RestoreItemFocus();
            if ((m_pRichEdit != nullptr) && m_pRichEdit->IsVisible()) {
                m_pRichEdit->SetVisible(false);
            }
            return false;
        }
        else {
            return true;
        }
        }, 0);

    //The control lost focus: apply the modification
    m_pRichEdit->DetachEvent(kEventKillFocus);
    m_pRichEdit->AttachKillFocus([this, RestoreItemFocus, OnLeaveRichEdit](const EventArgs&) {
        ClearEditEvents();
        RestoreItemFocus();
        OnLeaveRichEdit();
        return false;
        });

    //The window lost focus: apply the modification
    m_pRichEdit->DetachEvent(kEventWindowKillFocus);
    m_pRichEdit->AttachWindowKillFocus([this, RestoreItemFocus, OnLeaveRichEdit](const EventArgs&) {
        ClearEditEvents();
        RestoreItemFocus();
        OnLeaveRichEdit();
        return false;
        });
}

void ListCtrl::UpdateRichEditSize(ListCtrlLabel* pSubItem)
{
    if ((pSubItem == nullptr) || (m_pRichEdit == nullptr)) {
        return;
    }
    UiRect rcItem = pSubItem->GetTextRect();
    UiPoint offsetPt = pSubItem->GetScrollOffsetInScrollBox();
    rcItem.Offset(-offsetPt.x, -offsetPt.y);
    UiRect rect = GetRect();
    bool bSingleLine = pSubItem->IsSingleLine();
    if (bSingleLine) {
        if (m_pRichEdit->IsMultiLine()) {
            m_pRichEdit->SetMultiLine(false);
        }
        m_pRichEdit->SetFixedHeight(UiFixedInt::MakeAuto(), false, false);
        m_pRichEdit->SetFixedWidth(UiFixedInt::MakeAuto(), false, false);
        UiEstSize sz = m_pRichEdit->EstimateSize(UiSize(0, 0));
        //Set the width
        m_pRichEdit->SetFixedWidth(UiFixedInt(sz.cx.GetInt32()), true, false);

        //Set the height
        const int32_t nEditHeight = sz.cy.GetInt32() + sz.cy.GetInt32() / 2;
        m_pRichEdit->SetFixedHeight(UiFixedInt(nEditHeight), true, false);
        m_pRichEdit->SetMinHeight(nEditHeight, false);

        //Set the position and size limits of the text box
        uint32_t textStyle = pSubItem->GetTextStyle();
        if (textStyle & TEXT_VCENTER) {
            //CENTER alignment
            rcItem.top = rcItem.CenterY() - nEditHeight / 2;
            rcItem.bottom = rcItem.top + nEditHeight;
        }
        else if (textStyle & TEXT_BOTTOM) {
            //BOTTOM alignment
            rcItem.top = rcItem.bottom - nEditHeight;
        }

        UiMargin rcMargin(rcItem.left - rect.left, rcItem.top - rect.top, 0, 0);
        rcMargin.Validate();
        m_pRichEdit->SetMargin(rcMargin, false);

        m_pRichEdit->SetMinWidth(rcItem.Width(), false);
        int32_t nMaxWidth = std::max(rcItem.Width(), rect.Width() - rcMargin.left);
        m_pRichEdit->SetMaxWidth(nMaxWidth, false);
    }
    else {
        if (!m_pRichEdit->IsMultiLine()) {
            m_pRichEdit->SetMultiLine(true);
        }

        //Fixed width: same as the original control
        m_pRichEdit->SetFixedWidth(UiFixedInt(rcItem.Width()), false, false);
        m_pRichEdit->SetMaxWidth(rcItem.Width(), false);
        m_pRichEdit->SetMinWidth(rcItem.Width(), false);
        
        //Set the height and position
        m_pRichEdit->SetFixedHeight(UiFixedInt::MakeAuto(), false, false);        
        UiEstSize sz = m_pRichEdit->EstimateSize(UiSize(rcItem.Width(), 0));
        m_pRichEdit->SetFixedHeight(UiFixedInt(sz.cy.GetInt32()), true, false);

        UiMargin rcMargin(rcItem.left - rect.left, rcItem.top - rect.top, 0, 0);
        m_pRichEdit->SetMargin(rcMargin, false);

        int32_t nMaxHeight = std::max(rcItem.Height(), rect.Height() - rcMargin.top);
        m_pRichEdit->SetMaxHeight(nMaxHeight, false);
        m_pRichEdit->SetMinHeight(rcItem.Height(), false);
    }
}

void ListCtrl::UpdateDateTimeSize(ListCtrlLabel* pSubItem)
{
    if ((pSubItem == nullptr) || (m_pDateTime == nullptr)) {
        return;
    }
    UiRect rcItem = pSubItem->GetTextRect();
    UiPoint offsetPt = pSubItem->GetScrollOffsetInScrollBox();
    rcItem.Offset(-offsetPt.x, -offsetPt.y);
    UiRect rect = GetRect();
    //Width: the same as the text area of the cell; height: not less than the default height, so that the date text is not clipped
    const int32_t nHeight = std::max(rcItem.Height(), Dpi().GetScaleInt(28));
    m_pDateTime->SetFixedWidth(UiFixedInt(rcItem.Width()), false, false);
    m_pDateTime->SetFixedHeight(UiFixedInt(nHeight), false, false);
    UiMargin rcMargin(rcItem.left - rect.left, rcItem.top - rect.top, 0, 0);
    rcMargin.Validate();
    m_pDateTime->SetMargin(rcMargin, false);
}

void ListCtrl::EnterDateEditMode(const ListCtrlEditParam& editParam, ListCtrlLabel* pSubItem)
{
    if ((pSubItem == nullptr) || (m_listCtrlType != ListCtrlType::Report)) {
        //The date editor is used in the report view only
        return;
    }
    std::string dateTimeClass = GetDateTimeClass();
    ASSERT(!dateTimeClass.empty());
    if (dateTimeClass.empty()) {
        return;
    }
    if (m_pDateTime == nullptr) {
        m_pDateTime = new DateTime(GetWindow());
        AddItem(m_pDateTime);
        m_pDateTime->SetClass(dateTimeClass);
    }
    else if (GetItemCount() > 0) {
        //Move the DateTime control to the end so that it is painted on top of the other sub-items
        size_t nItem = GetItemIndex(m_pDateTime);
        size_t nLastItemIndex = GetItemCount() - 1;
        if (nItem != nLastItemIndex) {
            SetItemIndex(m_pDateTime, nLastItemIndex);
        }
    }
    if (m_pDateTime == nullptr) {
        return;
    }

    const size_t nDataItemIndex = editParam.nItemIndex;
    const std::string sOldItemText = pSubItem->GetText();

    //Date editor: always use the "year-month-day + drop-down calendar" edit format
    //(the DateTime control is reused, so the edit format must be set again every time the edit state is entered)
    m_pDateTime->SetEditFormat(DateTime::EditFormat::kDateCalendar);
    const ListCtrlEditorType editorType = GetColumnEditorTypeById(editParam.nColumnId);
    if (sOldItemText.empty() || !IsValidCellEditText(sOldItemText, editorType)) {
        //Empty text or invalid date text: SetDateTimeString fails to parse it, asserts and keeps the old value,
        //so the date value is cleared directly here (the edit state starts empty and is written back only after the user picks a date)
        m_pDateTime->ClearTime();
    }
    else {
        m_pDateTime->SetDateTimeString(sOldItemText);
        //SetDateTimeString only updates the internal date value and does not refresh the displayed text;
        //the editor control is reused (OnInit does not parse the text again), so the text must be set explicitly, otherwise the edit state shows an empty string
        m_pDateTime->SetText(m_pDateTime->GetDateTimeString());
    }
    //Use the current text of the editor as the "before modification" baseline: the date value is normalized to a fixed format,
    //so if the original cell text were used as the baseline, an unmodified value would be wrongly treated as changed and trigger a needless write-back
    const std::string sEditBaseText = m_pDateTime->GetDateTimeString();
    m_pDateTime->SetFloat(true);
    m_pDateTime->SetVisible(true);
    UpdateDateTimeSize(pSubItem);

    //Restore the focus: give the focus of the edit control back to the original list control
    auto RestoreItemFocus = [this, nDataItemIndex]() {
            if ((m_pDateTime != nullptr) && m_pDateTime->IsVisible() && m_pDateTime->IsFocused()) {
                ListCtrlItem* pDestItem = GetDisplayItemByIndex(nDataItemIndex);
                if (pDestItem != nullptr) {
                    pDestItem->SetFocus();
                }
            }
        };

    //Trigger the event: began editing
    ListCtrlEditParam enterEditParam = editParam;
    enterEditParam.sNewText.clear();
    enterEditParam.bCancelled = false;
    SendEvent(kEventEnterEdit, (WPARAM)&enterEditParam);
    if (enterEditParam.bCancelled || !IsValidItemEditState(editParam) || !IsValidItemEditParam(editParam)) {
        //The state has become invalid, or the user cancelled the edit
        ClearEditEvents();
        RestoreItemFocus();
        LeaveEditMode();
        return;
    }
    if (m_selectionMode == ListCtrlSelectionMode::kCell) {
        //Record the cell being edited (cleared by ClearEditEvents when the edit state is left)
        m_editingCell.nRowIndex = editParam.nItemIndex;
        m_editingCell.nColumnId = editParam.nColumnId;
    }

    m_pDateTime->SetVisible(true);
    //The editor control is reused: clear the "suppress reopening" state left over from the previous edit,
    //otherwise the calendar flyout will not pop up automatically when the control gets the focus again (a second mouse click would be required)
    m_pDateTime->ResetCalendarFlyoutState();
    //When the control is reused, its layout rectangle is not updated immediately after the margin changes (SetMargin only marks it as pending layout),
    //while the calendar flyout is positioned according to the current rectangle of the control when SetFocus is called; therefore the window layout must be refreshed once first,
    //otherwise the flyout stays at the position of the cell edited last time (out of sync with the current cell)
    if (GetWindow() != nullptr) {
        GetWindow()->UpdateWindow();
    }
    //After the focus is obtained, the DateTime control pops up the month calendar automatically
    m_pDateTime->SetFocus();

    //Commit: read the current date text
    //(commit only when the control is still visible, so that a repeated commit is not wrongly treated as clearing the cell because the control is already hidden)
    auto OnLeaveDateTime = [this, sEditBaseText, RestoreItemFocus, editParam]() -> bool {
        std::string sNewItemText;
        if ((m_pDateTime != nullptr) && m_pDateTime->IsVisible()) {
            sNewItemText = m_pDateTime->GetDateTimeString();
            m_pDateTime->SetVisible(false);
        }
        if ((sNewItemText != sEditBaseText) && IsValidItemEditParam(editParam)) {
            //The date text has changed
            return OnItemEdited(editParam, sNewItemText);
        }
        return false;
        };

    //Date changed (a date was selected in the calendar / the date was cleared): apply the modification
    m_pDateTime->DetachEvent(kEventValueChanged);
    m_pDateTime->AttachTimeChanged([this, RestoreItemFocus, OnLeaveDateTime](const EventArgs&) {
        ClearEditEvents();
        RestoreItemFocus();
        if (OnLeaveDateTime() && (m_selectionMode == ListCtrlSelectionMode::kCell)) {
            //Cell selection mode: after a successful commit, the current cell moves down by one row
            MoveCurrentCellToNextRow();
        }
        return false;
        });

    //Esc key pressed: cancel the modification
    m_pDateTime->DetachEvent(kEventKeyDown);
    m_pDateTime->AttachEvent(kEventKeyDown, [this, RestoreItemFocus](const EventArgs& msg) {
        if ((msg.eventType == kEventKeyDown) && (msg.vkCode == kVK_ESCAPE)) {
            ClearEditEvents();
            RestoreItemFocus();
            if ((m_pDateTime != nullptr) && m_pDateTime->IsVisible()) {
                m_pDateTime->SetVisible(false);
            }
            return false;
        }
        else {
            return true;
        }
        }, 0);

    //The control lost the focus: apply the modification
    //(note: the calendar flyout takes the focus, so when it pops up the window-level kill focus event kEventWindowKillFocus is triggered first;
    // hence only the control-level kEventKillFocus is listened to here and not kEventWindowKillFocus,
    // otherwise the modification would be wrongly committed as soon as the calendar pops up, ending the edit immediately)
    m_pDateTime->DetachEvent(kEventKillFocus);
    m_pDateTime->AttachKillFocus([this, RestoreItemFocus, OnLeaveDateTime](const EventArgs&) {
        ClearEditEvents();
        RestoreItemFocus();
        OnLeaveDateTime();
        return false;
        });
}

bool ListCtrl::OnItemEdited(const ListCtrlEditParam& editParam, const std::string& newItemText)
{
    //Editor type validation: invalid input is neither written back to the data nor reported through an event (equivalent to discarding this edit and keeping the original value)
    const ListCtrlEditorType editorType = GetColumnEditorTypeById(editParam.nColumnId);
    if (!IsValidCellEditText(newItemText, editorType)) {
        return false;
    }
    //Trigger the event: end editing; if the user cancelled the edit, do not perform the modification
    ListCtrlEditParam leaveEditParam = editParam;
    leaveEditParam.sNewText = newItemText;
    leaveEditParam.bCancelled = false;
    SendEvent(kEventLeaveEdit, (WPARAM)&leaveEditParam);
    if (!leaveEditParam.bCancelled) {
        //The user did not cancel the edit
        return SetSubItemText(editParam.nItemIndex, editParam.nColumnIndex, newItemText);
    }    
    return false;
}

ListCtrlItem* ListCtrl::GetDisplayItemByIndex(size_t nItemIndex) const
{
    ListCtrlItem* pDestItem = nullptr;
    ListCtrlItem* pNextItem = GetFirstDisplayItem();
    while (pNextItem != nullptr) {
        if (pNextItem->GetElementIndex() == nItemIndex) {
            pDestItem = pNextItem;
            break;
        }
        pNextItem = GetNextDisplayItem(pNextItem);
    }
    return pDestItem;
}

bool ListCtrl::IsValidCellEditText(const std::string& strNewText, ListCtrlEditorType editorType)
{
    std::string strValue = strNewText;
    StringUtil::Trim(strValue);
    if (strValue.empty()) {
        //Empty string: means clearing the cell, always considered valid
        return true;
    }
    if (editorType == ListCtrlEditorType::kEditText) {
        return true;
    }
    if (editorType == ListCtrlEditorType::kEditCombo) {
        //Combo candidate editor: the candidates are configured by the caller and the committed text is the candidate text itself, so no further format validation is needed
        return true;
    }
    if (editorType == ListCtrlEditorType::kEditNumber) {
        //Decimal number: an optional sign, at most one decimal point and at least one digit
        bool bHasDigit = false;
        bool bHasDot = false;
        for (size_t i = 0; i < strValue.size(); ++i) {
            const std::string::value_type ch = strValue[i];
            if ((ch >= '0') && (ch <= '9')) {
                bHasDigit = true;
            }
            else if ((ch == '.') && !bHasDot) {
                bHasDot = true;
            }
            else if (((ch == '+') || (ch == '-')) && (i == 0)) {
                //Sign
            }
            else {
                return false;
            }
        }
        return bHasDigit;
    }
    //Date: reuse the date parsing of the calendar (supports yyyy-mm-dd / yyyy/mm/dd)
    struct tm date = {};
    return Calendar::ParseDateString(strValue, date);
}

void ListCtrl::EnterComboEditMode(const ListCtrlEditParam& editParam, ListCtrlLabel* pSubItem)
{
    if ((pSubItem == nullptr) || (m_listCtrlType != ListCtrlType::Report)) {
        return;
    }
    const std::vector<std::string> candidates = GetColumnCandidatesById(editParam.nColumnId);
    if (candidates.empty()) {
        //No candidate values have been configured for this column: do not enter the edit state (avoid popping up an empty flyout)
        return;
    }

    //If a candidate flyout already exists (for example when the same cell is clicked again to edit it): close the old flyout first,
    //and then set the new edited cell state of this function.
    //Reason: Flyout::ShowAt internally closes the "currently active flyout" and triggers its close callback synchronously;
    //if the old flyout were still not closed at that point, that callback would clear the new edited cell state just set by this function,
    //making the row/column information invalid on commit and triggering an assertion
    CloseComboFlyout();

    //Trigger the event: began editing (the application may cancel this edit)
    ListCtrlEditParam enterEditParam = editParam;
    enterEditParam.sNewText.clear();
    enterEditParam.bCancelled = false;
    SendEvent(kEventEnterEdit, (WPARAM)&enterEditParam);
    if (enterEditParam.bCancelled || !IsValidItemEditState(editParam) || !IsValidItemEditParam(editParam)) {
        //The user cancelled the edit, or the state has become invalid
        return;
    }

    //Record the cell being edited (cleared by ClearEditEvents when the edit state is left)
    m_editingCell.nRowIndex = editParam.nItemIndex;
    m_editingCell.nColumnId = editParam.nColumnId;

    //The flyout width matches the cell width (GetPos returns physical pixels, while the size embedded in the XML is in DIP, so a conversion is needed)
    int32_t nWidth = pSubItem->GetPos().Width();
    Window* pOwnerWindow = GetWindow();
    if ((nWidth > 0) && (pOwnerWindow != nullptr)) {
        if (pOwnerWindow->Dpi().HasPixelDensity()) {
            pOwnerWindow->Dpi().ClientSizeToWindowSize(nWidth);
        }
        else {
            nWidth = pOwnerWindow->Dpi().GetUnscaleWindowSize(nWidth);
        }
    }
    if (nWidth <= 0) {
        //Fallback width, so that an unavailable flyout width does not make the popup fail
        nWidth = 120;
    }

    //Initially selected item: the index of the current cell text in the candidate list (the 1st item is selected when there is no match)
    size_t nSelectedIndex = 0;
    const std::string strCellText = pSubItem->GetText();
    for (size_t nIndex = 0; nIndex < candidates.size(); ++nIndex) {
        if (candidates[nIndex] == strCellText) {
            nSelectedIndex = nIndex;
            break;
        }
    }

    ListCtrlComboFlyout* pFlyout = new ListCtrlComboFlyout(GetWindow());
    pFlyout->AttachItemSelect(UiBind(&ListCtrl::OnComboCandidateSelected, this, std::placeholders::_1));
    pFlyout->AttachClosed(UiBind(this, [this, pFlyout](Flyout::CloseReason /*reason*/) {
        //Flyout closed (by clicking outside / losing the focus): the edit is treated as abandoned and no data is written back
        if (m_pComboFlyout == pFlyout) {
            //Only clean up the state when the flyout being closed is still the one currently recorded:
            //this avoids the close callback of an old flyout mistakenly clearing the state of a flyout created later
            m_pComboFlyout = nullptr;
            m_editingCell = ListCtrlCellId();
        }
        }, std::placeholders::_1));

    //When ShowAt fails the object has already been destroyed by the framework and the callback will not be triggered again
    if (!pFlyout->ShowAt(pSubItem, candidates, nSelectedIndex, nWidth, Flyout::Placement::Bottom)) {
        //Popup failed: end the edit state
        ClearEditEvents();
    }
    else {
        m_pComboFlyout = pFlyout;
    }
}

bool ListCtrl::OnComboCandidateSelected(size_t nIndex)
{
    ListCtrlComboFlyout* pFlyout = m_pComboFlyout;
    if (pFlyout == nullptr) {
        return false;
    }
    if (!m_editingCell.IsValid()) {
        //The edited cell has become invalid (for example the old state was cleared because the flyout was reopened): abandon this commit, to avoid writing the data back with an invalid row/column
        CloseComboFlyout();
        return false;
    }
    const std::vector<std::string>& candidates = pFlyout->GetItems();
    if (nIndex >= candidates.size()) {
        return false;
    }
    //Assemble the edit parameters (the edit state is cleared once the flyout is closed, so the information the callback needs is fetched first)
    ListCtrlEditParam editParam;
    editParam.listCtrlType = m_listCtrlType;
    editParam.nItemIndex = m_editingCell.nRowIndex;
    editParam.nColumnId = m_editingCell.nColumnId;
    editParam.nColumnIndex = GetColumnIndex(editParam.nColumnId);
    editParam.pItem = GetDisplayItemByIndex(editParam.nItemIndex);
    editParam.pSubItem = dynamic_cast<ListCtrlLabel*>(pFlyout->GetAnchor());
    const std::string strNewText = candidates[nIndex];
    const bool bRowMode = (m_selectionMode != ListCtrlSelectionMode::kCell);
    std::vector<size_t> selectedRowsBefore;
    if (bRowMode) {
        //Row selection mode: snapshot the currently selected rows first, to verify after the commit that the selection was not moved unexpectedly
        GetSelectedDataItems(selectedRowsBefore);
    }

    //Close the flyout first, then commit the write-back (to avoid re-entering the edit flow during the callback)
    CloseComboFlyout();

    const bool bEdited = OnItemEdited(editParam, strNewText);
    if (bEdited) {
        if (!bRowMode) {
            //After a successful commit, the current cell moves down by one row (consistent with the semantics of committing with Return in the rich text editor)
            MoveCurrentCellToNextRow();
        }
        else {
            //In row selection mode, committing a candidate must not change the selected rows; if it does, some internal logic moved the selection,
            //so the problem is exposed with an assertion and the selection from before the commit is restored, guaranteeing that row selection mode does not move any selection
            std::vector<size_t> selectedRowsAfter;
            GetSelectedDataItems(selectedRowsAfter);
            if (selectedRowsAfter != selectedRowsBefore) {
                ASSERT(0);
                SetSelectedDataItems(selectedRowsBefore, true);
            }
        }
    }
    return bEdited;
}

void ListCtrl::CloseComboFlyout()
{
    if (m_pComboFlyout != nullptr) {
        //Set it to null first, to avoid reentrancy during the Dismiss callback
        ListCtrlComboFlyout* pFlyout = m_pComboFlyout;
        m_pComboFlyout = nullptr;
        pFlyout->Dismiss();
    }
    //Leave the edit state: clear the cell being edited
    m_editingCell = ListCtrlCellId();
}

bool ListCtrl::IsComboEditOpened() const
{
    return (m_pComboFlyout != nullptr) && m_pComboFlyout->IsOpen();
}

bool ListCtrl::EnterCellEdit(size_t nRowIndex, size_t nColumnId)
{
    if (!IsEnableItemEdit() || (m_selectionMode != ListCtrlSelectionMode::kCell)) {
        return false;
    }
    if ((m_listCtrlType != ListCtrlType::Report) || (m_pReportView == nullptr) || (m_pData == nullptr)) {
        return false;
    }
    if ((nRowIndex >= GetDataItemCount()) || !IsValidColumnId(nColumnId)) {
        return false;
    }
    if (!IsSubItemEditableById(nRowIndex, nColumnId)) {
        //This cell is not editable
        return false;
    }
    if (m_editingCell.IsValid() && (m_editingCell.nRowIndex == nRowIndex) && (m_editingCell.nColumnId == nColumnId)) {
        //The cell is already being edited
        return true;
    }
    //If another cell is being edited, end its edit state first (to avoid having two edit boxes at the same time)
    LeaveEditMode();

    //Position the current cell at that cell first
    if (!SetCurrentCell(nRowIndex, nColumnId)) {
        return false;
    }
    if (!IsDataItemDisplay(nRowIndex)) {
        //The row is not in the visible area: scroll it into the visible area first
        EnsureDataItemVisible(nRowIndex, false);
    }
    ListCtrlItem* pItem = GetDisplayItemByIndex(nRowIndex);
    if ((pItem == nullptr) || !pItem->IsSelectableType()) {
        //Rows that cannot be selected (such as topmost rows) are not allowed to be edited
        return false;
    }
    //Find the corresponding cell control inside the row by column ID
    ListCtrlSubItem* pSubItem = nullptr;
    size_t nItemCount = pItem->GetItemCount();
    for (size_t nItem = 0; nItem < nItemCount; ++nItem) {
        ListCtrlSubItem* pNextSubItem = dynamic_cast<ListCtrlSubItem*>(pItem->GetItemAt(nItem));
        if ((pNextSubItem != nullptr) && (pNextSubItem->GetDataColumnId() == nColumnId)) {
            pSubItem = pNextSubItem;
            break;
        }
    }
    if (pSubItem == nullptr) {
        return false;
    }

    //Assemble the edit parameters and enter the edit state directly (without the 600ms timer used when entering the edit state)
    ListCtrlEditParam editParam;
    editParam.listCtrlType = m_listCtrlType;
    editParam.nItemIndex = nRowIndex;
    editParam.nColumnId = nColumnId;
    editParam.nColumnIndex = GetColumnIndex(nColumnId);
    editParam.pItem = pItem;
    editParam.pSubItem = pSubItem;
    m_editModeFlag.Cancel();
    OnItemEditMode(editParam);
    return true;
}

bool ListCtrl::EnterCurrentCellEdit()
{
    if (m_selectionMode != ListCtrlSelectionMode::kCell) {
        return false;
    }
    const ListCtrlCellId cell = GetCurrentCell();
    if (!cell.IsValid()) {
        return false;
    }
    return EnterCellEdit(cell.nRowIndex, cell.nColumnId);
}

bool ListCtrl::IsCellEditing() const
{
    return m_editingCell.IsValid();
}

bool ListCtrl::GetEditingCell(ListCtrlCellId& cell) const
{
    cell = m_editingCell;
    return IsCellEditing();
}

void ListCtrl::MoveCurrentCellToNextRow()
{
    if (m_selectionMode != ListCtrlSelectionMode::kCell) {
        //Only cell selection mode supports moving the current cell (row selection mode does not move any selection)
        return;
    }
    const ListCtrlCellId cell = GetCurrentCell();
    if (!cell.IsValid() || ((cell.nRowIndex + 1) >= GetDataItemCount())) {
        //The current cell is invalid or is already the last row: do not move
        return;
    }
    const size_t nNextRowIndex = cell.nRowIndex + 1;
    SetCurrentCell(nNextRowIndex, cell.nColumnId);
    EnsureDataItemVisible(nNextRowIndex, false);
}

void ListCtrl::OnViewMouseEvents(const EventArgs& msg)
{
    if ((msg.eventType == kEventMouseWheel) ||
        (msg.eventType == kEventMouseButtonDown) ||
        (msg.eventType == kEventMouseButtonUp) ||
        (msg.eventType == kEventMouseDoubleClick) ||
        (msg.eventType == kEventMouseRButtonDown) ||
        (msg.eventType == kEventMouseRButtonUp) ||
        (msg.eventType == kEventMouseRDoubleClick) ||
        (msg.eventType == kEventContextMenu)) {
        if ((msg.GetSender() != m_pRichEdit) && (msg.GetSender() != m_pDateTime)) {
            LeaveEditMode();
        }
    }    
}

void ListCtrl::OnViewKeyboardEvents(const EventArgs& msg)
{
    if (IsComboEditOpened()) {
        //The combo candidate flyout is open: the key has already been handled with priority by the view layer (OnComboKeyDown),
        //so the edit state is not ended here, otherwise the flyout would be wrongly closed while the key bubbles up
        return;
    }
    if ((msg.GetSender() != m_pRichEdit) && (msg.GetSender() != m_pDateTime)) {
        if (msg.vkCode != kVK_F2) {
            //Use F2 to enter the edit state and do not exit the edit state automatically, so that the F2 function does not stop working
            LeaveEditMode();
        }
    }    
}

void ListCtrl::LeaveEditMode()
{
    //Cancel the timer of the edit state
    m_editModeFlag.Cancel();
    //Combo candidate edit state: close the candidate flyout (without writing the data back)
    CloseComboFlyout();
    if ((m_pRichEdit != nullptr) && m_pRichEdit->IsVisible() && m_pRichEdit->IsFocused()) {
        m_pRichEdit->SendEvent(kEventKillFocus);
    }
    if ((m_pRichEdit != nullptr) && m_pRichEdit->IsVisible()) {
        m_pRichEdit->SetVisible(false);
    }
    if ((m_pDateTime != nullptr) && m_pDateTime->IsVisible() && m_pDateTime->IsFocused()) {
        m_pDateTime->SendEvent(kEventKillFocus);
    }
    if (m_pDateTime != nullptr) {
        //Close the calendar flyout that may still be open first, so that no dangling topmost calendar window is left after the anchor control is hidden
        m_pDateTime->CloseCalendarFlyout();
    }
    if ((m_pDateTime != nullptr) && m_pDateTime->IsVisible()) {
        m_pDateTime->SetVisible(false);
    }
    ClearEditEvents();
}

void ListCtrl::ClearEditEvents()
{
    if (m_pRichEdit != nullptr) {
        m_pRichEdit->DetachEvent(kEventWindowKillFocus);
        m_pRichEdit->DetachEvent(kEventKillFocus);
        m_pRichEdit->DetachEvent(kEventReturn);
        m_pRichEdit->DetachEvent(kEventTextChanged);
        m_pRichEdit->DetachEvent(kEventKeyDown);
    }
    if (m_pDateTime != nullptr) {
        m_pDateTime->DetachEvent(kEventValueChanged);
        m_pDateTime->DetachEvent(kEventKillFocus);
        m_pDateTime->DetachEvent(kEventKeyDown);
    }
    //Leave the edit state: clear the cell being edited
    m_editingCell = ListCtrlCellId();
}

}//namespace ui

