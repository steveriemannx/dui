#include "dui/Control/SearchBox.h"
#include "dui/Control/RichEdit.h"
#include "dui/Control/AutoComplete.h"
#include "dui/Core/Control.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/Callback.h"
#include "dui/Core/Keycode.h"
#include "dui/Utils/StringUtil.h"
#include <algorithm>
#include <list>

namespace ui
{

/** Single-line edit control used internally by SearchBox: while the suggestion flyout is open it intercepts the
*   up/down arrow keys, Enter and Esc
*/
class SearchBoxEdit : public RichEdit
{
    typedef RichEdit BaseClass;
public:
    explicit SearchBoxEdit(Window* pWindow) :
        RichEdit(pWindow),
        m_pOwner(nullptr)
    {
    }

    /** Sets the host control
    */
    void SetOwner(SearchBox* pOwner)
    {
        m_pOwner = pOwner;
    }

    virtual bool OnKeyDown(const EventArgs& msg) override
    {
        //While the flyout is open, the navigation keys are handed to the host control and are not passed on to the
        //edit control (so the caret does not move and Enter does not insert a new line)
        if ((m_pOwner != nullptr) && m_pOwner->HandleEditKeyDown(msg)) {
            return true;
        }
        return BaseClass::OnKeyDown(msg);
    }

private:
    /** Host control
    */
    SearchBox* m_pOwner;
};

SearchBox::SearchBox(Window* pWindow) :
    BaseClass(pWindow),
    m_pEdit(nullptr),
    m_nDebounceIntervalMs(0),
    m_nDebounceTaskId(0)
{
}

std::string SearchBox::GetType() const
{
    return DUI_CTR_SEARCH_BOX;
}

void SearchBox::SetAttribute(const std::string& strName, const std::string& strValue2)
{
    //dui does not support variable expansion in attribute values, so the value is used as is
    const std::string& strValue = strValue2;
    //These attributes are forwarded to the internal edit control; before OnInit the child controls do not exist
    //yet, so the values are cached
    if ((strName == "prompt_text") || (strName == "prompttext")) {
        m_promptText = strValue;
        if (m_pEdit != nullptr) {
            m_pEdit->SetAttribute(strName, strValue);
        }
    }
    else if ((strName == "prompt_text_id") || (strName == "prompt_textid") || (strName == "prompttextid")) {
        m_promptTextId = strValue;
        if (m_pEdit != nullptr) {
            m_pEdit->SetAttribute(strName, strValue);
        }
    }
    else if ((strName == "prompt_color") || (strName == "promptcolor")) {
        m_promptColor = strValue;
        if (m_pEdit != nullptr) {
            m_pEdit->SetAttribute(strName, strValue);
        }
    }
    else if (strName == "text") {
        m_initText = strValue;
        if (m_pEdit != nullptr) {
            //Keep it consistent with the other attributes: forward it through SetAttribute, which reuses the
            //complete text handling of RichEdit (including the IsReplaceNewline newline replacement and so on)
            m_pEdit->SetAttribute(strName, strValue);
        }
    }
    else if ((strName == "text_id") || (strName == "textid")) {
        m_initTextId = strValue;
        if (m_pEdit != nullptr) {
            m_pEdit->SetAttribute(strName, strValue);
        }
    }
    else if (strName == "debounce_interval") {
        SetDebounceInterval(static_cast<size_t>(StringUtil::StringToInt32(strValue)));
    }
    else if (strName == "suggestions") {
        ParseInitItems(strValue);
    }
    else if (strName == "max_visible") {
        SetMaxVisible(StringUtil::StringToInt32(strValue));
    }
    else if (strName == "history_enabled") {
        m_bHistoryEnabled = (strValue == "true");
    }
    else if (strName == "max_history") {
        SetMaxHistory(StringUtil::StringToInt32(strValue));
    }
    else if (strName == "filter_enabled") {
        m_bFilterEnabled = (strValue == "true");
    }
    else if (strName == "min_chars") {
        SetMinChars(StringUtil::StringToInt32(strValue));
    }
    else if (strName == "match_mode") {
        if (strValue == "contains") {
            m_matchMode = AutoCompleteMatchMode::kContains;
        }
        else {
            m_matchMode = AutoCompleteMatchMode::kPrefix;
        }
    }
    else if (strName == "ignore_case") {
        m_bIgnoreCase = (strValue == "true");
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

void SearchBox::OnInit()
{
    if (IsInited()) {
        return;
    }
    BaseClass::OnInit();

    //Search icon on the left (does not respond to the mouse and never takes the focus, clicks pass through to the
    //container)
    Control* pIcon = new Control(GetWindow());
    pIcon->SetClass("search_box_icon");
    pIcon->SetNoFocus();
    pIcon->SetMouseEnabled(false);
    AddItem(pIcon);

    //Edit control in the middle
    SearchBoxEdit* pEdit = new SearchBoxEdit(GetWindow());
    pEdit->SetOwner(this);
    m_pEdit = pEdit;
    m_pEdit->SetClass("search_box_edit");
    m_pEdit->SetAttribute("want_tab", "false");
    //The placeholder is enabled by default
    m_pEdit->SetAttribute("prompt_mode", "true");
    if (!m_promptText.empty()) {
        m_pEdit->SetAttribute("prompt_text", std::string(m_promptText.c_str()));
    }
    if (!m_promptTextId.empty()) {
        m_pEdit->SetAttribute("prompt_text_id", std::string(m_promptTextId.c_str()));
    }
    if (!m_promptColor.empty()) {
        m_pEdit->SetAttribute("prompt_color", std::string(m_promptColor.c_str()));
    }
    //Clear button on the right (shown automatically when the text is not empty, clears the text when clicked; the
    //built-in mechanism of RichEdit is reused)
    m_pEdit->SetAttribute("clear_btn_class", "search_box_clear_btn");
    if (!m_initText.empty()) {
        m_pEdit->SetText(m_initText.c_str());
    }
    if (!m_initTextId.empty()) {
        m_pEdit->SetAttribute("text_id", std::string(m_initTextId.c_str()));
    }
    AddItem(m_pEdit);

    //Forward the internal edit control events: text change, Enter search, focus closing the suggestions
    m_pEdit->AttachTextChanged([this](const EventArgs& /*args*/) {
        OnEditInput();
        return true;
        });
    m_pEdit->AttachReturn([this](const EventArgs& /*args*/) {
        //Submit the search (Enter not intercepted by the flyout): close the flyout, record the history and fire
        //kEventReturn
        CommitSearch();
        return true;
        });
    m_pEdit->AttachSetFocus([this](const EventArgs& /*args*/) {
        OnEditFocus();
        return true;
        });
    //Note: kEventKillFocus is deliberately not hooked to close the flyout, because:
    //1. The flyout works in no-focus mode, so its outside-click detection (AutoDismiss) already covers the "click
    //   another area to close" scenario;
    //2. The flyout window may briefly take over the foreground focus while it is created (Flyout restores the
    //   parent window focus internally); closing the flyout in KillFocus would re-enter DoClose inside the ShowAt
    //   call stack and cause a duplicated flyout and focus flicker.
}

void SearchBox::SetFocus()
{
    if (m_pEdit != nullptr) {
        m_pEdit->SetFocus();
    }
    else {
        BaseClass::SetFocus();
    }
}

RichEdit* SearchBox::GetEditControl() const
{
    return m_pEdit;
}

void SearchBox::SetSearchText(const std::string& text)
{
    if (m_pEdit != nullptr) {
        m_pEdit->SetText(text);
    }
    else {
        m_initText = text;
    }
}

std::string SearchBox::GetSearchText() const
{
    if (m_pEdit != nullptr) {
        return m_pEdit->GetText();
    }
    return std::string(m_initText.c_str());
}

void SearchBox::SetDebounceInterval(size_t nIntervalMs)
{
    m_nDebounceIntervalMs = nIntervalMs;
}

void SearchBox::OnEditInput()
{
    //Refresh the suggestion flyout immediately (not debounced); kEventTextChanged is still fired with the debounce
    //delay
    UpdatePopup();
    if (m_nDebounceIntervalMs > 0) {
        //Cancel the previous debounce task that has not fired yet, so the event fires exactly once after the typing
        //paused for the whole interval
        if (m_nDebounceTaskId != 0) {
            GlobalManager::Instance().Thread().CancelTask(m_nDebounceTaskId);
            m_nDebounceTaskId = 0;
        }
        m_nDebounceTaskId = GlobalManager::Instance().Thread().PostDelayedTask(
            ui::kThreadUI,
            UiBind(this, [this]() {
                m_nDebounceTaskId = 0;
                SendEvent(kEventTextChanged);
            }),
            static_cast<int32_t>(m_nDebounceIntervalMs));
    }
    else {
        SendEvent(kEventTextChanged);
    }
}

SearchBox::~SearchBox()
{
    if (m_nDebounceTaskId != 0) {
        GlobalManager::Instance().Thread().CancelTask(m_nDebounceTaskId);
        m_nDebounceTaskId = 0;
    }
    if (m_nSuppressClearTaskId != 0) {
        GlobalManager::Instance().Thread().CancelTask(m_nSuppressClearTaskId);
        m_nSuppressClearTaskId = 0;
    }
    ClosePopup();
}

void SearchBox::OnEditFocus()
{
    //SetFocus is called again after a selected candidate has been filled back in; the flyout must not be opened
    //again in that case (so it does not pop up right after the selection)
    if (m_bSuppressNextFocusPopup) {
        m_bSuppressNextFocusPopup = false;
        return;
    }
    UpdatePopup();
}

void SearchBox::ScheduleClearSuppressFlag()
{
    if (m_nSuppressClearTaskId != 0) {
        GlobalManager::Instance().Thread().CancelTask(m_nSuppressClearTaskId);
        m_nSuppressClearTaskId = 0;
    }
    //Clear the suppression flag with a delay: the kEventSetFocus that the framework re-sends synchronously when the
    //flyout closes is blocked by the flag, and the normal "open on focus" behavior is restored after the next
    //message cycle, so a later legitimate focus of the user is not swallowed by mistake
    m_nSuppressClearTaskId = GlobalManager::Instance().Thread().PostDelayedTask(
        ui::kThreadUI,
        UiBind(this, [this]() {
            m_nSuppressClearTaskId = 0;
            m_bSuppressNextFocusPopup = false;
        }),
        100);
}

void SearchBox::CommitSearch()
{
    //Suppress the kEventSetFocus → UpdatePopup reopen that may follow when the focus is restored (by a Toast) or
    //SetFocus is called, so the drop-down stays closed after submitting (or selecting); the flag is cleared in the
    //next message cycle, so a later legitimate focus is not swallowed
    m_bSuppressNextFocusPopup = true;
    ClosePopup();
    AddSearchHistory(GetSearchText());
    if ((m_pEdit != nullptr) && !m_pEdit->IsFocused()) {
        //Make sure the edit control holds the focus after submitting with Enter, so typing can continue
        m_pEdit->SetFocus();
    }
    SendEvent(kEventReturn);
    //Clear the suppression flag with a delay: this covers the asynchronous kEventSetFocus that may be triggered when
    //the focus is restored (by a Toast), and the normal "open on focus" behavior follows afterwards
    ScheduleClearSuppressFlag();
}

// ----------------------------------------------------------------------------
// SearchBox suggestion (suggestions + history) drop-down
// ----------------------------------------------------------------------------

void SearchBox::SetCandidates(const std::vector<std::string>& items)
{
    m_items.clear();
    for (const std::string& text : items) {
        std::string item = StringUtil::Trim(text.c_str());
        if (!item.empty()) {
            m_items.push_back(item);
        }
    }
}

std::vector<std::string> SearchBox::GetCandidates() const
{
    std::vector<std::string> result;
    result.reserve(m_items.size());
    for (const UiString& item : m_items) {
        result.push_back(item.c_str());
    }
    return result;
}

void SearchBox::AddCandidate(const std::string& text)
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

void SearchBox::ClearCandidates()
{
    m_items.clear();
    ClosePopup();
}

void SearchBox::SetMaxVisible(int32_t nMaxVisible)
{
    if (nMaxVisible < 1) {
        nMaxVisible = 1;
    }
    if (nMaxVisible > 100) {
        nMaxVisible = 100;
    }
    m_nMaxVisible = nMaxVisible;
    if (m_pFlyout != nullptr) {
        m_pFlyout->SetMaxVisible(m_nMaxVisible);
    }
}

int32_t SearchBox::GetMaxVisible() const
{
    return m_nMaxVisible;
}

void SearchBox::SetFilterEnabled(bool bEnabled)
{
    m_bFilterEnabled = bEnabled;
    //Refresh the flyout immediately with the new rules after the filter switch changed
    if (m_pEdit != nullptr) {
        UpdatePopup();
    }
}

bool SearchBox::IsFilterEnabled() const
{
    return m_bFilterEnabled;
}

void SearchBox::SetMinChars(int32_t nMinChars)
{
    if (nMinChars < 0) {
        nMinChars = 0;
    }
    if (nMinChars > 100) {
        nMinChars = 100;
    }
    m_nMinChars = nMinChars;
}

int32_t SearchBox::GetMinChars() const
{
    return m_nMinChars;
}

void SearchBox::SetMatchMode(AutoCompleteMatchMode matchMode)
{
    m_matchMode = matchMode;
}

AutoCompleteMatchMode SearchBox::GetMatchMode() const
{
    return m_matchMode;
}

void SearchBox::SetIgnoreCase(bool bIgnoreCase)
{
    m_bIgnoreCase = bIgnoreCase;
}

bool SearchBox::IsIgnoreCase() const
{
    return m_bIgnoreCase;
}

void SearchBox::SetHistoryEnabled(bool bEnabled)
{
    m_bHistoryEnabled = bEnabled;
    if (!m_bHistoryEnabled) {
        ClosePopup();
    }
}

bool SearchBox::IsHistoryEnabled() const
{
    return m_bHistoryEnabled;
}

void SearchBox::SetMaxHistory(int32_t nMax)
{
    if (nMax < 1) {
        nMax = 1;
    }
    if (nMax > 1000) {
        nMax = 1000;
    }
    m_nMaxHistory = nMax;
    while (static_cast<int32_t>(m_history.size()) > m_nMaxHistory) {
        m_history.pop_back();
    }
}

int32_t SearchBox::GetMaxHistory() const
{
    return m_nMaxHistory;
}

std::vector<std::string> SearchBox::GetHistory() const
{
    std::vector<std::string> result;
    result.reserve(m_history.size());
    for (const UiString& item : m_history) {
        result.push_back(item.c_str());
    }
    return result;
}

void SearchBox::ClearHistory()
{
    m_history.clear();
    ClosePopup();
}

void SearchBox::AddSearchHistory(const std::string& text)
{
    UiString t(StringUtil::Trim(text.c_str()).c_str());
    if (t.empty() || !m_bHistoryEnabled) {
        return;
    }
    //Remove an identical existing entry and put the new one on top (MRU), so no duplicates are kept
    //Note: the comparison must use the content (*it == t) and never the c_str() pointers (two UiString objects
    //have different buffers, so comparing the pointers would never match)
    for (auto it = m_history.begin(); it != m_history.end(); ++it) {
        if (*it == t) {
            m_history.erase(it);
            break;
        }
    }
    m_history.insert(m_history.begin(), t);
    while (static_cast<int32_t>(m_history.size()) > m_nMaxHistory) {
        m_history.pop_back();
    }
}

bool SearchBox::HandleEditKeyDown(const EventArgs& msg)
{
    if ((m_pFlyout == nullptr) || !m_pFlyout->IsOpen()) {
        //The flyout is not open: hand the key back to the default handling of the edit control (Enter → kEventReturn)
        return false;
    }
    if (msg.vkCode == kVK_DOWN) {
        m_bUserNavigated = true;
        m_pFlyout->MoveSelection(1);
        return true;
    }
    if (msg.vkCode == kVK_UP) {
        m_bUserNavigated = true;
        m_pFlyout->MoveSelection(-1);
        return true;
    }
    if (msg.vkCode == kVK_ESCAPE) {
        ClosePopup();
        return true;
    }
    if (msg.vkCode == kVK_RETURN) {
        //The up/down keys were used to navigate and a candidate is highlighted: Enter selects that candidate (it is
        //filled back in and kEventAutoCompleteSelect fires)
        if (m_bUserNavigated) {
            const size_t nIndex = m_pFlyout->GetSelectedIndex();
            if (nIndex != Box::InvalidIndex) {
                OnCandidateSelected(nIndex);
                return true;
            }
        }
        //No navigation, or the flyout is not open: submit "the current text of the edit control" (record the
        //history + kEventReturn) without filling in a candidate
        CommitSearch();
        return true;
    }
    return false;
}

void SearchBox::UpdatePopup()
{
    //Re-entrancy guard: while AutoCompleteFlyout::ShowAt creates the flyout window it may briefly take over the
    //foreground focus of the parent window; the framework restores the parent window focus right afterwards, which
    //synchronously triggers the edit control kEventSetFocus → OnEditFocus → UpdatePopup nested call; without the
    //guard the flyout would be created twice (two drop-downs)
    if (m_bUpdatingPopup) {
        return;
    }
    m_bUpdatingPopup = true;
    UpdatePopupInternal();
    m_bUpdatingPopup = false;
}

void SearchBox::UpdatePopupInternal()
{
    if (m_pEdit == nullptr) {
        return;
    }
    //The "navigated" state is cleared every time the candidate list is rebuilt: the navigation is only valid for the
    //current candidate set, and once the text changed the candidate set is different, so Enter has to go back to
    //"submit the typed text"
    m_bUserNavigated = false;
    BuildCandidates();

    if (m_candidates.empty()) {
        ClosePopup();
        return;
    }

    //When the flyout is already open: refresh the candidate content and height in place and reuse the same window
    if ((m_pFlyout != nullptr) && m_pFlyout->IsOpen()) {
        if (m_pFlyout->UpdateItems(m_candidates, 0)) {
            return;
        }
        //The update failed (the window is no longer valid): fall back to the rebuild path
        m_pFlyout = nullptr;
    }
    OpenPopup();
}

void SearchBox::OpenPopup()
{
    if (m_pEdit == nullptr) {
        return;
    }
    //The flyout width matches the client area of the control (GetPos is in physical pixels while XML sizes are in
    //DIP, so a conversion is needed)
    int32_t nWidth = GetPos().Width();
    if (nWidth <= 0) {
        return;
    }
    Window* pOwnerWindow = GetWindow();
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
    pFlyout->AttachClosed(UiBind(this, [this, pFlyout](Flyout::CloseReason reason) {
        m_pFlyout = nullptr;
        //When the flyout content was clicked with the mouse, the framework moves the native keyboard focus to the
        //flyout window; when the flyout closes, Flyout::DoClose hands the focus back to this window and synchronously
        //re-sends the edit control kEventSetFocus. Apart from "clicked outside" (the focus legitimately went
        //somewhere else), closing must not reopen the flyout because of that (the intent of Esc, for example, is to
        //dismiss it), so the one-shot suppression flag blocks that re-sent event. The flag is set only when the
        //flyout really holds the focus (that is, when the hand-over is certain to happen), to avoid pointless
        //suppression.
        if ((reason != Flyout::CloseReason::kClickOutside) && pFlyout->IsWindowFocused()) {
            m_bSuppressNextFocusPopup = true;
            ScheduleClearSuppressFlag();
        }
        }, std::placeholders::_1));

    //When ShowAt fails the object has already been destroyed by the framework and the callbacks do not fire any more
    if (pFlyout->ShowAt(this, m_candidates, 0, nWidth, Flyout::Placement::Bottom)) {
        m_pFlyout = pFlyout;
    }
}

void SearchBox::ClosePopup()
{
    if (m_pFlyout != nullptr) {
        //Clear the pointer first, so the Dismiss callback cannot re-enter here
        AutoCompleteFlyout* pFlyout = m_pFlyout;
        m_pFlyout = nullptr;
        pFlyout->Dismiss();
    }
}

void SearchBox::OnCandidateSelected(size_t nIndex)
{
    if (nIndex >= m_candidates.size()) {
        return;
    }
    const std::string text = m_candidates[nIndex].c_str();

    //Fill the text back into the edit control without firing textChanged, so the flyout does not open again
    if (m_pEdit != nullptr) {
        m_pEdit->SetTextNoEvent(text);
        const int32_t nTextLength = m_pEdit->GetTextLength();
        m_pEdit->SetSel(nTextLength, nTextLength);
    }
    //Suppress the reopen triggered when the focus is restored (by a Toast); the drop-down has to stay closed after a
    //selection
    m_bSuppressNextFocusPopup = true;
    ClosePopup();
    AddSearchHistory(text);
    if ((m_pEdit != nullptr) && !m_pEdit->IsFocused()) {
        //Focus the control again after filling the text back in, so typing can continue
        m_pEdit->SetFocus();
    }
    SendEvent(kEventAutoCompleteSelect, static_cast<WPARAM>(nIndex));
    //Clear the suppression flag with a delay: this covers the asynchronous kEventSetFocus that may be triggered when
    //the focus is restored (by a Toast)
    ScheduleClearSuppressFlag();
}

void SearchBox::BuildCandidates()
{
    m_candidates.clear();
    //Collect the merged data source: history (on top) + suggestions, de-duplicated
    std::vector<UiString> merged;
    merged.reserve(m_history.size() + m_items.size());
    auto pushUnique = [&merged](const UiString& s) {
        for (const UiString& e : merged) {
            if (e == s) {   //Content comparison; the c_str() pointers must not be compared
                return;
            }
        }
        merged.push_back(s);
    };
    if (m_bHistoryEnabled) {
        for (const UiString& h : m_history) {
            pushUnique(h);
        }
    }
    for (const UiString& s : m_items) {
        pushUnique(s);
    }

    if (!m_bFilterEnabled) {
        //No filtering: show everything (history + suggestions)
        m_candidates.swap(merged);
        return;
    }

    //Filter mode: filter only once the input reached min_chars; otherwise (empty input/not enough characters) show
    //everything (focused state)
    const std::string input = StringUtil::Trim(m_pEdit->GetText().c_str());
    if (static_cast<int32_t>(input.size()) < m_nMinChars) {
        m_candidates.swap(merged);
        return;
    }
    std::vector<UiString> filtered;
    filtered.reserve(merged.size());
    for (const UiString& s : merged) {
        if (MatchItem(s.c_str(), input)) {
            filtered.push_back(s);
        }
    }
    m_candidates.swap(filtered);
}

bool SearchBox::MatchItem(const std::string& item, const std::string& input) const
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

void SearchBox::ParseInitItems(const std::string& items)
{
    std::list<std::string> itemList = StringUtil::Split(items, ";");
    for (const std::string& item : itemList) {
        AddCandidate(item);
    }
}

} // namespace ui
