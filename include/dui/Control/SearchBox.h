#ifndef UI_CONTROL_SEARCHBOX_H_
#define UI_CONTROL_SEARCHBOX_H_

#include "dui/Box/HBox.h"
#include "dui/Control/AutoComplete.h"
#include <vector>

namespace ui
{
class RichEdit;

/** Search box control (SearchBox): a composite control made of a search icon on the left, an edit control in the
*   middle and a clear button on the right
*
*   Internal structure:
*   - Search icon: a plain Control that does not respond to the mouse and never takes the focus
*   - Edit control: a RichEdit with the placeholder (prompt) enabled by default; pressing Enter fires kEventReturn
*   - Clear button: reuses the built-in clear_btn_class mechanism of RichEdit and is shown automatically when the
*     text is not empty
*
*   XML attributes (in addition to the common HBox attributes):
*     prompt_text     Placeholder text
*     prompt_text_id  Multilingual ID of the placeholder text
*     prompt_color    Placeholder text color
*     text            Initial search text
*     text_id         Multilingual ID of the initial search text (when both text and text_id are set, the attribute
*                     set last wins)
*     debounce_interval Debounce interval (milliseconds): while the search text keeps changing, kEventTextChanged is
*                     fired only after the typing paused for that interval; 0 (the default) fires on every change and
*                     keeps the previous behavior. It avoids firing a search request on every keystroke.
*     suggestions     Initial suggestion data source; items are separated by semicolons ";" (the search history is
*                     recorded automatically by the control when a search is submitted)
*     max_visible     Maximum number of visible suggestion rows in the drop-down (default 8; a scroll bar appears
*                     beyond it)
*     history_enabled Whether to record and show the search history (default true)
*     max_history     Maximum number of search history entries (default 10; the oldest entries are dropped beyond it)
*     filter_enabled  Whether to filter the suggestions by the typed text (default true); when disabled the full list
*                     is always shown without filtering
*     min_chars       Minimum number of typed characters that triggers filtering (default 1)
*     match_mode      Filtering match mode: prefix (default) / contains
*     ignore_case     Whether filtering ignores the character case (default true)
*
*   Suggestion flyout display rules (explicit and consistent):
*   - When the edit control gets the focus it is evaluated immediately and the flyout is opened when there is
*     something to show, otherwise it is not popped up.
*   - The flyout is refreshed on every text change and closed when there is nothing to show.
*   - Filter switch filter_enabled (default true): when enabled, the suggestions are filtered according to
*     match_mode/ignore_case once min_chars characters are typed; when the input is empty or shorter than min_chars
*     everything is shown (history first, then the suggestions, duplicates removed). When disabled the full list is
*     always shown (no input filtering).
*   - The flyout does not take the focus, losing the focus of the edit control does not close it; clicking outside
*     the flyout and the anchor closes it automatically.
*   - Focus hand-over: when a candidate is clicked with the mouse, the framework first moves the native keyboard
*     focus to the flyout window; before the flyout closes it hands the focus back to this window uniformly (see
*     Flyout::DoClose), so typing can continue after selecting or dismissing with Esc, and the flyout is not opened
*     again because of it.
*   - After a selected candidate has been filled back in, the flyout is not opened again (the focus-triggered popup
*     is suppressed once), which avoids the flicker of popping up right after the selection.
*   - Enter semantics: when the up/down keys were not used to navigate the candidates, Enter commits "the current
*     text of the edit control"; when the up/down keys were used to navigate and highlight a candidate, Enter selects
*     that candidate (it is filled back in and kEventAutoCompleteSelect fires).
*
*   Events:
*   - kEventTextChanged: fired when the search text changes (forwarded from the internal edit control)
*   - kEventReturn: fired when Enter is pressed without a selected candidate, used to run the search (the search
*     history is recorded when the search is submitted)
*   - kEventAutoCompleteSelect: fired after a candidate has been selected (mouse click or keyboard Enter); WPARAM is
*     the index of the candidate in the current candidate list, and GetSearchText() already returns the candidate
*     text at that point
*
*   Skin classes (defined in global.xml): search_box / search_box_icon /
*   search_box_edit / search_box_clear_btn; the drop-down flyout reuses the AutoComplete skin classes
*   auto_complete_flyout / auto_complete_list / auto_complete_item
*/
class DUI_API SearchBox : public HBox
{
    typedef HBox BaseClass;

    //The internal edit control subclass needs to call the private key pre-processing interface
    friend class SearchBoxEdit;
public:
    explicit SearchBox(Window* pWindow);
    SearchBox(const SearchBox& r) = delete;
    SearchBox& operator=(const SearchBox& r) = delete;
    virtual ~SearchBox() override;

    virtual std::string GetType() const override;
    virtual void SetAttribute(const std::string& strName, const std::string& strValue) override;

    /** Gives the focus to the control (the focus is actually set on the internal edit control)
    */
    virtual void SetFocus() override;

    /** Gets the internal edit control
    */
    RichEdit* GetEditControl() const;

    /** Sets the search text
    */
    void SetSearchText(const std::string& text);

    /** Gets the search text
    */
    std::string GetSearchText() const;

    /** Sets the debounce interval (milliseconds)
     *   While the search text keeps changing, kEventTextChanged is fired only after the typing paused for that
     *   interval; 0 (the default) fires on every change (the previous behavior). Search scenarios can use it to
     *   avoid firing a search request on every keystroke.
     * @note The debounce also applies to text changes caused by a programmatic SetSearchText.
     */
    void SetDebounceInterval(size_t nIntervalMs);
    size_t GetDebounceInterval() const { return m_nDebounceIntervalMs; }

    /** @name Suggestion data source
     *   The suggestion data source is provided by the application; the search history is recorded automatically by
     *   the control when a search is submitted (see the history interfaces)
     */
    ///@{
    /** Sets the suggestion data source (the flyout shows the whole list, merged with the history and de-duplicated)
    */
    void SetCandidates(const std::vector<std::string>& items);

    /** Gets the suggestion data source
    */
    std::vector<std::string> GetCandidates() const;

    /** Appends one suggestion item (ignored when the same text already exists)
    */
    void AddCandidate(const std::string& text);

    /** Clears the suggestion data source
    */
    void ClearCandidates();
    ///@}

    /** @name Suggestion display parameters
     */
    ///@{
    /** Sets the maximum number of visible suggestion rows in the drop-down (a scroll bar appears beyond it)
     */
    void SetMaxVisible(int32_t nMaxVisible);
    int32_t GetMaxVisible() const;

    /** Sets whether the suggestions are filtered by the typed text (default true)
     *   When enabled, filtering starts once min_chars characters are typed; when disabled the full list is always
     *   shown (no input filtering)
     */
    void SetFilterEnabled(bool bEnabled);
    bool IsFilterEnabled() const;

    /** Sets the minimum number of typed characters that triggers filtering (default 1)
     */
    void SetMinChars(int32_t nMinChars);
    int32_t GetMinChars() const;

    /** Sets the filtering match mode (prefix / contains), prefix by default
     */
    void SetMatchMode(AutoCompleteMatchMode matchMode);
    AutoCompleteMatchMode GetMatchMode() const;

    /** Sets whether filtering ignores the character case (default true)
     */
    void SetIgnoreCase(bool bIgnoreCase);
    bool IsIgnoreCase() const;
    ///@}

    /** @name Search history
     *   The control records the search term automatically when a search is "submitted" (Enter without a selected
     *   candidate, or a selected candidate) and keeps at most max_history entries (MRU: the most recently used
     *   first). The history also takes part in the matching as suggestion candidates.
     */
    ///@{
    /** Sets whether the search history is recorded and shown
     */
    void SetHistoryEnabled(bool bEnabled);
    bool IsHistoryEnabled() const;

    /** Sets the maximum number of search history entries (the oldest entries are dropped beyond it)
     */
    void SetMaxHistory(int32_t nMax);
    int32_t GetMaxHistory() const;

    /** Gets the search history (the most recently used first)
     */
    std::vector<std::string> GetHistory() const;

    /** Clears the search history
     */
    void ClearHistory();

    /** Appends a search history entry explicitly (usually the application does not need to call it, the control
     *  records the entry automatically when a search is submitted)
     */
    void AddSearchHistory(const std::string& text);
    ///@}

public:
    /** Listens for the candidate select event (WPARAM is the index of the selected item in the current candidate list)
    */
    void AttachSelect(const EventCallback& callback, EventCallbackID callbackID = 0)
    {
        AttachEvent(kEventAutoCompleteSelect, callback, callbackID);
    }

    /** Listens for the Enter key down event (fired when no candidate is selected, used to start a search)
    * @param [in] callbackID The ID of the callback function (used to remove the callback function)
    * @param [in] callback The custom callback function invoked when Enter is pressed
    */
    void AttachReturn(const EventCallback& callback, EventCallbackID callbackID = 0)
    {
        AttachEvent(kEventReturn, callback, callbackID);
    }

protected:
    /** Initialization interface: creates the search icon, the edit control and the other child controls
    */
    virtual void OnInit() override;

private:
    /** Internal edit control
    */
    RichEdit* m_pEdit;

    /** Initial search text (the text attribute of the XML is cached before OnInit)
    */
    UiString m_initText;

    /** Multilingual ID of the initial search text (the text_id attribute of the XML is cached before OnInit)
    */
    UiString m_initTextId;

    /** Placeholder text (cached before OnInit)
    */
    UiString m_promptText;

    /** Multilingual ID of the placeholder text (cached before OnInit)
    */
    UiString m_promptTextId;

    /** Placeholder text color (cached before OnInit)
    */
    UiString m_promptColor;

    /** Debounce interval (milliseconds), 0 means no debounce (fire immediately)
    */
    size_t m_nDebounceIntervalMs = 0;

    /** ID of the pending debounce task (the return value of PostDelayedTask), 0 means no pending task
    */
    size_t m_nDebounceTaskId = 0;

    // ---- Suggestion (suggestions + history) drop-down ----
    /** Suggestion flyout (the framework destroys it after closing and the pointer is set to null)
    */
    AutoCompleteFlyout* m_pFlyout = nullptr;

    /** Suggestion data source
    */
    std::vector<UiString> m_items;

    /** Search history (MRU: the most recently used first)
    */
    std::vector<UiString> m_history;

    /** Currently displayed candidate list (identical to what the flyout shows)
    */
    std::vector<UiString> m_candidates;

    /** Maximum number of visible suggestion rows in the drop-down (default 8)
     */
    int32_t m_nMaxVisible = 8;

    /** Whether the suggestions are filtered by the typed text (default true); when disabled the full list is shown
     */
    bool m_bFilterEnabled = true;

    /** Minimum number of typed characters that triggers filtering (default 1)
     */
    int32_t m_nMinChars = 1;

    /** Filtering match mode (prefix by default)
     */
    AutoCompleteMatchMode m_matchMode = AutoCompleteMatchMode::kPrefix;

    /** Whether filtering ignores the character case (default true)
     */
    bool m_bIgnoreCase = true;

    /** Whether the user already navigated the candidates with the up/down keys (decides whether Enter "commits the
     *  input" or "selects a candidate")
     */
    bool m_bUserNavigated = false;

    /** Whether the search history is recorded and shown (default true)
     */
    bool m_bHistoryEnabled = true;

    /** Maximum number of search history entries (default 10)
     */
    int32_t m_nMaxHistory = 10;

    /** Whether the "open the flyout" triggered by SetFocus after a selected candidate was filled back in should be
     *  suppressed (one-shot flag)
     */
    bool m_bSuppressNextFocusPopup = false;

    /** ID of the task that clears the "do not open on focus" suppression flag with a delay (0 means no pending task)
     */
    size_t m_nSuppressClearTaskId = 0;

    /** Flyout refresh re-entrancy guard: AutoCompleteFlyout::ShowAt restores the focus of the parent window
     *  internally, which synchronously triggers edit control kEventSetFocus → OnEditFocus → UpdatePopup nested
     *  calls; that has to be blocked, otherwise the flyout would be created twice
     */
    bool m_bUpdatingPopup = false;

    /** Text change handler: refreshes the suggestion flyout immediately and decides, according to the debounce
     *  interval, whether kEventTextChanged is fired immediately or with a delay
     */
    void OnEditInput();

    /** Edit control got the focus: opens the suggestion flyout when there is something to show
     *   After a selected candidate has been filled back in, SetFocus is called again; m_bSuppressNextFocusPopup
     *   suppresses opening the flyout once in that case
     */
    void OnEditFocus();

    /** Clears the "do not open on focus" suppression flag with a delay
     *   When the flyout closes, the framework hands the keyboard focus back to this window and synchronously
     *   re-sends the edit control kEventSetFocus (see the focus hand-over logic of Flyout::DoClose); the flag
     *   blocks the flyout from opening again because of that re-sent event. Clearing it with a delay avoids
     *   swallowing a legitimate focus that arrives right afterwards (for example when the user clicks the edit
     *   control again).
     */
    void ScheduleClearSuppressFlag();

    /** Edit control key pre-processing (intercepts up/down/Enter/Esc while the flyout is open)
     * @return true means the key was handled and is not passed on to the edit control
     */
    bool HandleEditKeyDown(const EventArgs& msg);

    /** Refreshes the suggestion flyout (open/update/close) with a re-entrancy guard
     */
    void UpdatePopup();

    /** Actual implementation of the suggestion flyout refresh (called by UpdatePopup only)
     */
    void UpdatePopupInternal();

    /** Opens the suggestion flyout
     */
    void OpenPopup();

    /** Closes the suggestion flyout (when it is open)
     */
    void ClosePopup();

    /** A candidate was confirmed (keyboard Enter/mouse click)
     * @param [in] nIndex Index of the candidate in m_candidates
     */
    void OnCandidateSelected(size_t nIndex);

    /** Submits the search (Enter without a selected item): records the history and fires kEventReturn
     */
    void CommitSearch();

    /** Builds the candidate list m_candidates (suggestions + history merged and de-duplicated; filtered by the
     *  input in filter mode)
     */
    void BuildCandidates();

    /** Decides whether a single candidate matches the typed text (controlled by m_matchMode / m_bIgnoreCase)
     */
    bool MatchItem(const std::string& item, const std::string& input) const;

    /** Parses the suggestions attribute (semicolon separated)
     */
    void ParseInitItems(const std::string& items);
};

} // namespace ui

#endif // UI_CONTROL_SEARCHBOX_H_
