#include "dui/Control/AutoComplete.h"
#include "dui/Control/RichEdit.h"
#include "dui/Box/ListBox.h"
#include "dui/Box/ListBoxItem.h"
#include "dui/Utils/StringUtil.h"
#include "dui/Core/Keycode.h"
#include "dui/Core/WindowMessage.h"
#include "dui/Core/Callback.h"
#include "dui/Core/Window.h"
#include <algorithm>
#include <functional>

namespace ui
{
/** Single-line edit control used internally by AutoComplete: intercepts the up/down arrow keys, Enter and Esc while the suggestion flyout is open
*/
class AutoCompleteEdit : public RichEdit
{
    typedef RichEdit BaseClass;
public:
    explicit AutoCompleteEdit(Window* pWindow) :
        RichEdit(pWindow),
        m_pOwner(nullptr)
    {
    }

    /** Sets the owner control
    */
    void SetOwner(AutoComplete* pOwner)
    {
        m_pOwner = pOwner;
    }

    virtual bool OnKeyDown(const EventArgs& msg) override
    {
        //While the flyout is open, navigation keys are handled by the owner and are not passed to the edit control (so the caret does not move and Enter does not insert a new line)
        if ((m_pOwner != nullptr) && m_pOwner->HandleEditKeyDown(msg)) {
            return true;
        }
        return BaseClass::OnKeyDown(msg);
    }

private:
    /** Owner control
    */
    AutoComplete* m_pOwner;
};

AutoCompleteFlyout::AutoCompleteFlyout(Window* pParentWindow) :
    Flyout(pParentWindow),
    m_nSelectedIndex(Box::InvalidIndex),
    m_nMaxVisible(8),
    m_nWidthDip(200),
    m_nGapDip(4),
    m_pList(nullptr),
    m_pContent(nullptr)
{
    //The flyout does not take the focus: the parent window stays active and the edit control keeps receiving input
    SetNoFocus(true);
    //Close automatically when the user clicks outside the flyout
    SetAutoDismiss(true);
    //Always shown below the anchor, no vertical flipping
    SetAllowFlip(false);
    SetGap(m_nGapDip);
}

bool AutoCompleteFlyout::ShowAt(Control* pAnchor, const std::vector<UiString>& items, size_t nSelectedIndex,
                                int32_t nWidthDip, Placement placement)
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

    //Embedded skin XML: the Window size is the client size without the shadow, the framework adds the shadow automatically
    //The initial size follows the current number of suggestion rows; later text changes adjust the height in place through UpdateItems, without recreating the window
    std::string sXml = StringUtil::Printf(
        "<Window size=\"%d,%d\" caption=\"0,0,0,0\" use_system_caption=\"false\" "
        "shadow_type=\"default\" shadow_attached=\"true\" layered_window=\"true\" size_box=\"0,0,0,0\">"
        "<VBox class=\"auto_complete_flyout\" name=\"auto_complete_content\" width=\"%d\" height=\"%d\">"
        "<VListBox class=\"auto_complete_list\" name=\"auto_complete_list\" "
        "width=\"stretch\" height=\"stretch\"/>"
        "</VBox></Window>",
        m_nWidthDip, nHeight, m_nWidthDip, nHeight);

    return Flyout::ShowAt(pAnchor, sXml, placement);
}

void AutoCompleteFlyout::OnInitWindow()
{
    BaseClass::OnInitWindow();
    InitControls();
}

void AutoCompleteFlyout::InitControls()
{
    ASSERT(m_pList == nullptr);
    if (m_pList != nullptr) {
        return;
    }
    m_pList = dynamic_cast<VListBox*>(FindControl("auto_complete_list"));
    ASSERT(m_pList != nullptr);
    if (m_pList == nullptr) {
        return;
    }
    //Content container: its height follows the number of suggestion rows
    m_pContent = dynamic_cast<Box*>(FindControl("auto_complete_content"));

    //Set the initial height from the number of suggestion rows (including the container padding), so no unnecessary scroll bar appears
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

    //Mouse hover over a list item: synchronize the highlight (to stay consistent with keyboard navigation)
    m_pList->AttachItemMouseEnter([this](const EventArgs& msg) {
        const size_t nIndex = static_cast<size_t>(msg.wParam);
        if (nIndex < m_items.size()) {
            m_pList->SelectItem(nIndex, false, false);
        }
        return true;
        });

    RebuildList();
}

bool AutoCompleteFlyout::UpdateItems(const std::vector<UiString>& items, size_t nSelectedIndex)
{
    if (!IsWindow() || (m_pList == nullptr) || items.empty()) {
        return false;
    }
    m_items = items;
    m_nSelectedIndex = nSelectedIndex;

    //Adjust the content container height and the window size to the new number of suggestion rows
    ApplyContentHeight();

    //Refresh the list content: the same list control is reused and only the items are replaced, the window is not destroyed and recreated
    RebuildList();

    //Recompute the size and position of the flyout window from the content height
    PositionAroundAnchor();
    return true;
}

void AutoCompleteFlyout::ApplyContentHeight()
{
    if (m_pContent == nullptr) {
        return;
    }
    //Height of a single suggestion row (physical pixels, matching what RebuildList sets)
    const int32_t nItemHeight = Dpi().GetScaleInt(GetItemHeight());
    const int32_t nRows = std::min<int32_t>(static_cast<int32_t>(m_items.size()), m_nMaxVisible);

    //The content container (auto_complete_flyout) has a padding of its own (padding in the theme) and the layout
    //subtracts that padding from the usable area of the list, so the container height has to include the padding;
    //otherwise the usable height is smaller than the total height of the rows and a scroll bar appears by mistake
    const UiPadding rcPadding = m_pContent->GetPadding();
    const int32_t nContentHeight = nRows * nItemHeight + rcPadding.top + rcPadding.bottom;
    m_pContent->SetFixedHeight(UiFixedInt(nContentHeight), true, false);

    //The width and height of the root container (the shadow container) are computed once from the fixed size of
    //the XML root container when the shadow is attached and are then fixed, so they do not follow a change of
    //the content height. Update the root container height explicitly here (content height + shadow area),
    //otherwise the window size estimated by PositionAroundAnchor does not follow the number of suggestion rows
    Box* pRoot = GetRoot();
    if ((pRoot != nullptr) && (pRoot != m_pContent)) {
        const UiPadding rcShadow = pRoot->GetPadding();
        pRoot->SetFixedHeight(UiFixedInt(nContentHeight + rcShadow.top + rcShadow.bottom), true, false);
    }
}

void AutoCompleteFlyout::RebuildList()
{
    ASSERT(m_pList != nullptr);
    if (m_pList == nullptr) {
        return;
    }
    m_pList->RemoveAllItems();
    for (const UiString& text : m_items) {
        ListBoxItem* pItem = new ListBoxItem(this);
        pItem->SetClass("auto_complete_item");
        pItem->SetText(text.c_str());
        pItem->SetFixedHeight(UiFixedInt(GetItemHeight()), true, true);
        m_pList->AddItem(pItem);
    }
    if (m_nSelectedIndex < m_items.size()) {
        m_pList->SelectItem(m_nSelectedIndex, false, false);
        m_pList->EnsureVisible(m_nSelectedIndex, ListBoxVerVisible::kVisible, ListBoxHorVisible::kVisible);
    }
}

size_t AutoCompleteFlyout::GetSelectedIndex() const
{
    if (m_pList != nullptr) {
        return m_pList->GetCurSel();
    }
    return (m_nSelectedIndex < m_items.size()) ? m_nSelectedIndex : Box::InvalidIndex;
}

void AutoCompleteFlyout::SetSelectedIndex(size_t nIndex)
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

void AutoCompleteFlyout::MoveSelection(int32_t nDelta)
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

void AutoCompleteFlyout::SetMaxVisible(int32_t nMaxVisible)
{
    if (nMaxVisible < 1) {
        nMaxVisible = 1;
    }
    if (nMaxVisible > 100) {
        nMaxVisible = 100;
    }
    m_nMaxVisible = nMaxVisible;
}

int32_t AutoCompleteFlyout::GetMaxVisible() const
{
    return m_nMaxVisible;
}

void AutoCompleteFlyout::AttachItemSelect(const ItemSelectEvent& callback)
{
    m_itemSelectCallbacks.push_back(callback);
}

int32_t AutoCompleteFlyout::GetItemHeight() const
{
    //Kept in sync with fixed_height of auto_complete_item in global.xml (DIP)
    return 28;
}

// ----------------------------------------------------------------------------
// AutoComplete
// ----------------------------------------------------------------------------

AutoComplete::AutoComplete(Window* pWindow) :
    BaseClass(pWindow),
    m_pEdit(nullptr),
    m_pFlyout(nullptr),
    m_nMinChars(1),
    m_nMaxVisible(8),
    m_matchMode(AutoCompleteMatchMode::kPrefix),
    m_bIgnoreCase(true),
    m_bInUpdate(false)
{
}

std::string AutoComplete::GetType() const
{
    return DUI_CTR_AUTO_COMPLETE;
}

void AutoComplete::SetAttribute(const std::string& strName, const std::string& strValue)
{
    //dui does not support variable expansion in attribute values, so the value is used as is
    const std::string& strValue2 = strValue;
    if (strName == "items") {
        ParseInitItems(strValue2);
    }
    else if (strName == "prompt_text") {
        m_promptText = strValue2;
        if (m_pEdit != nullptr) {
            m_pEdit->SetAttribute("prompt_text", strValue2);
        }
    }
    else if (strName == "prompt_text_id") {
        m_promptTextId = strValue2;
        if (m_pEdit != nullptr) {
            m_pEdit->SetAttribute("prompt_text_id", strValue2);
        }
    }
    else if (strName == "prompt_color") {
        m_promptColor = strValue2;
        if (m_pEdit != nullptr) {
            m_pEdit->SetAttribute("prompt_color", strValue2);
        }
    }
    else if (strName == "min_chars") {
        SetMinChars(StringUtil::StringToInt32(strValue2.c_str()));
    }
    else if (strName == "max_visible") {
        SetMaxVisible(StringUtil::StringToInt32(strValue2.c_str()));
    }
    else if (strName == "match_mode") {
        if (strValue2 == "contains") {
            m_matchMode = AutoCompleteMatchMode::kContains;
        }
        else {
            m_matchMode = AutoCompleteMatchMode::kPrefix;
        }
    }
    else if (strName == "ignore_case") {
        m_bIgnoreCase = (strValue2 == "true");
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

void AutoComplete::OnInit()
{
    if (IsInited()) {
        return;
    }
    BaseClass::OnInit();

    ASSERT(m_pEdit == nullptr);
    if (m_pEdit == nullptr) {
        AutoCompleteEdit* pEdit = new AutoCompleteEdit(GetWindow());
        pEdit->SetOwner(this);
        pEdit->SetClass("auto_complete_edit");
        pEdit->SetAttribute("single_line", "true");
        pEdit->SetAttribute("want_tab", "false");
        pEdit->SetAttribute("want_return", "false");
        pEdit->SetAttribute("prompt_mode", "true");
        if (!m_promptText.empty()) {
            pEdit->SetAttribute("prompt_text", m_promptText.c_str());
        }
        if (!m_promptTextId.empty()) {
            pEdit->SetAttribute("prompt_text_id", m_promptTextId.c_str());
        }
        if (!m_promptColor.empty()) {
            pEdit->SetAttribute("prompt_color", m_promptColor.c_str());
        }
        Box::AddItem(pEdit);
        m_pEdit = pEdit;

        //Text changed: forward the event and refresh the suggestion flyout
        m_pEdit->AttachTextChanged(UiBind(this, [this](const EventArgs& /*msg*/) {
            HandleEditTextChanged();
            return true;
            }, std::placeholders::_1));

        //The edit control lost the focus: close the suggestion flyout (clicking the no-focus flyout does not cause a focus loss)
        m_pEdit->AttachKillFocus(UiBind(this, [this](const EventArgs& /*msg*/) {
            ClosePopup();
            return true;
            }, std::placeholders::_1));
    }
}

void AutoComplete::SetFocus()
{
    if (m_pEdit != nullptr) {
        m_pEdit->SetFocus();
    }
}

void AutoComplete::SetCandidates(const std::vector<std::string>& items)
{
    m_items.clear();
    for (const std::string& text : items) {
        std::string item = StringUtil::Trim(text.c_str());
        if (!item.empty()) {
            m_items.push_back(item);
        }
    }
}

std::vector<std::string> AutoComplete::GetCandidates() const
{
    std::vector<std::string> result;
    result.reserve(m_items.size());
    for (const UiString& item : m_items) {
        result.push_back(item.c_str());
    }
    return result;
}

void AutoComplete::AddCandidate(const std::string& text)
{
    std::string item = StringUtil::Trim(text.c_str());
    if (item.empty()) {
        return;
    }
    for (const UiString& existing : m_items) {
        if (item == existing.c_str()) {
            return;
        }
    }
    m_items.push_back(item);
}

void AutoComplete::ClearCandidates()
{
    m_items.clear();
    ClosePopup();
}

void AutoComplete::SetText(const std::string& text)
{
    //Set the text programmatically without showing the suggestion flyout
    if (m_pEdit != nullptr) {
        m_pEdit->SetTextNoEvent(text);
    }
}

std::string AutoComplete::GetText() const
{
    if (m_pEdit != nullptr) {
        return m_pEdit->GetText();
    }
    return std::string();
}

void AutoComplete::SetMinChars(int32_t nMinChars)
{
    if (nMinChars < 0) {
        nMinChars = 0;
    }
    if (nMinChars > 100) {
        nMinChars = 100;
    }
    m_nMinChars = nMinChars;
}

int32_t AutoComplete::GetMinChars() const
{
    return m_nMinChars;
}

void AutoComplete::SetMaxVisible(int32_t nMaxVisible)
{
    if (nMaxVisible < 1) {
        nMaxVisible = 1;
    }
    if (nMaxVisible > 100) {
        nMaxVisible = 100;
    }
    m_nMaxVisible = nMaxVisible;
    if (m_pFlyout != nullptr) {
        m_pFlyout->SetMaxVisible(nMaxVisible);
    }
}

int32_t AutoComplete::GetMaxVisible() const
{
    return m_nMaxVisible;
}

void AutoComplete::SetMatchMode(AutoCompleteMatchMode matchMode)
{
    m_matchMode = matchMode;
}

AutoCompleteMatchMode AutoComplete::GetMatchMode() const
{
    return m_matchMode;
}

void AutoComplete::SetIgnoreCase(bool bIgnoreCase)
{
    m_bIgnoreCase = bIgnoreCase;
}

bool AutoComplete::IsIgnoreCase() const
{
    return m_bIgnoreCase;
}

RichEdit* AutoComplete::GetEditControl() const
{
    return m_pEdit;
}

bool AutoComplete::HandleEditKeyDown(const EventArgs& msg)
{
    if (m_pFlyout == nullptr) {
        return false;
    }
    if (msg.vkCode == kVK_DOWN) {
        m_pFlyout->MoveSelection(1);
        return true;
    }
    if (msg.vkCode == kVK_UP) {
        m_pFlyout->MoveSelection(-1);
        return true;
    }
    if (msg.vkCode == kVK_RETURN) {
        const size_t nIndex = m_pFlyout->GetSelectedIndex();
        if (nIndex != Box::InvalidIndex) {
            OnCandidateSelected(nIndex);
        }
        else {
            ClosePopup();
        }
        return true;
    }
    if (msg.vkCode == kVK_ESCAPE) {
        ClosePopup();
        return true;
    }
    return false;
}

void AutoComplete::HandleEditTextChanged()
{
    //Forward the text-changed event
    SendEvent(kEventTextChanged);

    //Do not pop the flyout up again while the text is being written back programmatically
    if (m_bInUpdate) {
        return;
    }
    UpdatePopup();
}

void AutoComplete::UpdatePopup()
{
    if (m_pEdit == nullptr) {
        return;
    }
    const std::string input = StringUtil::Trim(m_pEdit->GetText().c_str());
    BuildCandidates(input);

    if ((static_cast<int32_t>(input.size()) < m_nMinChars) || m_candidates.empty()) {
        ClosePopup();
        return;
    }

    //The flyout is already open: refresh the suggestion content and height in place and reuse the same window (avoiding the cost of closing and reopening it)
    if ((m_pFlyout != nullptr) && m_pFlyout->IsOpen()) {
        if (m_pFlyout->UpdateItems(m_candidates, 0)) {
            return;
        }
        //The update failed (the window is no longer valid): fall back to recreating the flyout
        m_pFlyout = nullptr;
    }
    OpenPopup();
}

void AutoComplete::OpenPopup()
{
    if (m_pEdit == nullptr) {
        return;
    }
    //The flyout width matches the client area of the control (GetPos returns physical pixels while XML sizes are DIP, so a conversion is needed)
    int32_t nWidth = GetPos().Width();
    if (nWidth <= 0) {
        return;
    }
    Window* pOwnerWindow = GetWindow();
    //GetPos returns physical pixels after layout, while the size embedded into the XML is DIP, so it is converted back with the current DPI
    if (pOwnerWindow != nullptr) {
        if (pOwnerWindow->Dpi().HasPixelDensity()) {
            pOwnerWindow->Dpi().ClientSizeToWindowSize(nWidth);
        }
        else {
            nWidth = pOwnerWindow->Dpi().GetUnscaleWindowSize(nWidth);
        }
    }

    AutoCompleteFlyout* pFlyout = new AutoCompleteFlyout(GetWindow());
    pFlyout->SetMaxVisible(m_nMaxVisible);
    pFlyout->AttachItemSelect(UiBind(this, [this](size_t nIndex) {
        OnCandidateSelected(nIndex);
        }, std::placeholders::_1));
    pFlyout->AttachClosed(UiBind(this, [this](Flyout::CloseReason /*reason*/) {
        m_pFlyout = nullptr;
        }, std::placeholders::_1));

    //When ShowAt fails the object has already been destroyed by the framework and the callbacks no longer fire
    if (pFlyout->ShowAt(this, m_candidates, 0, nWidth, Flyout::Placement::Bottom)) {
        m_pFlyout = pFlyout;
    }
}

void AutoComplete::ClosePopup()
{
    if (m_pFlyout != nullptr) {
        //Clear the pointer first to avoid re-entrancy from the Dismiss callback
        AutoCompleteFlyout* pFlyout = m_pFlyout;
        m_pFlyout = nullptr;
        pFlyout->Dismiss();
    }
}

void AutoComplete::OnCandidateSelected(size_t nIndex)
{
    if (nIndex >= m_candidates.size()) {
        return;
    }
    const std::string text = m_candidates[nIndex].c_str();

    //Write the text back to the edit control without firing textChanged, so the flyout does not pop up again
    m_bInUpdate = true;
    if (m_pEdit != nullptr) {
        m_pEdit->SetTextNoEvent(text);
        const int32_t nTextLength = m_pEdit->GetTextLength();
        m_pEdit->SetSel(nTextLength, nTextLength);
    }
    m_bInUpdate = false;

    ClosePopup();
    if (m_pEdit != nullptr) {
        m_pEdit->SetFocus();
    }
    SendEvent(kEventAutoCompleteSelect, static_cast<WPARAM>(nIndex));
}

void AutoComplete::BuildCandidates(const std::string& input)
{
    m_candidates.clear();
    if (input.empty()) {
        //When the input is empty and suggestions from 0 characters are allowed, show all data
        for (const UiString& item : m_items) {
            m_candidates.push_back(item);
        }
        return;
    }
    for (const UiString& item : m_items) {
        if (MatchItem(item.c_str(), input)) {
            m_candidates.push_back(item);
        }
    }
}

bool AutoComplete::MatchItem(const std::string& item, const std::string& input) const
{
    if (input.empty()) {
        return true;
    }
    if (!m_bIgnoreCase) {
        const size_t pos = item.find(input);
        return (m_matchMode == AutoCompleteMatchMode::kContains) ? (pos != std::string::npos) : (pos == 0);
    }
    const std::string lowerItem = StringUtil::MakeLowerString(item);
    const std::string lowerInput = StringUtil::MakeLowerString(input);
    const size_t pos = lowerItem.find(lowerInput);
    return (m_matchMode == AutoCompleteMatchMode::kContains) ? (pos != std::string::npos) : (pos == 0);
}

void AutoComplete::ParseInitItems(const std::string& items)
{
    std::list<std::string> itemList = StringUtil::Split(items, ";");
    for (const std::string& item : itemList) {
        AddCandidate(item);
    }
}

} // namespace ui
