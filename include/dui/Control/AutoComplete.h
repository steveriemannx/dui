#ifndef UI_CONTROL_AUTO_COMPLETE_H_
#define UI_CONTROL_AUTO_COMPLETE_H_

#include "dui/Core/Box.h"
#include "dui/Control/Flyout.h"
#include <functional>
#include <vector>

namespace ui
{
class RichEdit;
class VListBox;

/** Text matching mode for the auto-complete suggestions
*/
enum class AutoCompleteMatchMode : uint8_t
{
    kPrefix     = 0,    //Prefix match (default): the suggestion starts with the typed text
    kContains   = 1     //Contains match: the typed text occurs anywhere in the suggestion
};

/** Auto-complete suggestion flyout (AutoCompleteFlyout): hosts the candidate list in a Flyout
*
*   Features:
*   1. Does not take the focus by default (WS_EX_NOACTIVATE), so the edit control keeps the
*      keyboard focus while the flyout is shown and the user can keep typing.
*   2. When the number of suggestions changes the flyout height follows it (the width is fixed to the
*      edit control width); when the text changes the same flyout window is reused and its content is
*      refreshed in place instead of being destroyed and recreated.
*   3. Supports selecting with the mouse (click/hover); the up/down arrow keys and Enter are
*      forwarded to the host control for handling.
*   4. Modeless window: the framework deletes the object automatically once the window has been
*      closed, so the caller does not need to release it manually
*/
class DUI_API AutoCompleteFlyout : public Flyout
{
    typedef Flyout BaseClass;
public:
    /** Callback for a selected suggestion (mouse click/keyboard Enter); the parameter is the suggestion index (zero based)
    */
    typedef std::function<void(size_t nIndex)> ItemSelectEvent;

    /** Constructor
    * @param [in] pParentWindow Parent window (the anchor control must belong to this window)
    */
    explicit AutoCompleteFlyout(Window* pParentWindow);
    AutoCompleteFlyout(const AutoCompleteFlyout& r) = delete;
    AutoCompleteFlyout& operator=(const AutoCompleteFlyout& r) = delete;
    virtual ~AutoCompleteFlyout() override = default;

    /** Shows the suggestion flyout below the anchor control
    * @param [in] pAnchor Anchor control (the host edit control/container)
    * @param [in] items Suggestion text list
    * @param [in] nSelectedIndex Index of the initially selected item
    * @param [in] nWidthDip Width of the flyout (DIP, usually the width of the edit control)
    * @param [in] placement Preferred popup direction
    * @return Returns true when the flyout was shown; on failure the object has already been destroyed by the framework
    */
    bool ShowAt(Control* pAnchor, const std::vector<UiString>& items, size_t nSelectedIndex,
                int32_t nWidthDip, Placement placement = Placement::Bottom);

    /** Updates the suggestion content in place (the already created flyout window is reused, not recreated)
    *   Only the list content is refreshed; the flyout height is adjusted to the new number of suggestion rows
    *   and the position is recomputed
    * @param [in] items New suggestion text list (must not be empty)
    * @param [in] nSelectedIndex Index of the newly selected item
    * @return Returns true when the update succeeded (returns false when the flyout window has not been created yet)
    */
    bool UpdateItems(const std::vector<UiString>& items, size_t nSelectedIndex);

    /** Gets the index of the currently selected item; returns Box::InvalidIndex when nothing is selected
    */
    size_t GetSelectedIndex() const;

    /** Sets the currently selected item (does not fire the select event)
    */
    void SetSelectedIndex(size_t nIndex);

    /** Moves the current selection (up/down arrow keys)
    * @param [in] nDelta +1 moves to the next item, -1 to the previous one; the selection stops at the first/last item
    */
    void MoveSelection(int32_t nDelta);

    /** Sets the maximum number of visible suggestion rows (a scroll bar appears when there are more)
    */
    void SetMaxVisible(int32_t nMaxVisible);
    int32_t GetMaxVisible() const;

    /** Listens for the suggestion select event (mouse click/keyboard Enter)
    */
    void AttachItemSelect(const ItemSelectEvent& callback);

public:
    //WindowImplBase interface
    virtual void OnInitWindow() override;

private:
    /** Initializes the internal controls and binds their events
    */
    void InitControls();

    /** Builds the list content from m_items
    */
    void RebuildList();

    /** Adjusts the height of the content container (and of the root container) to the current number of rows
    *   Container height = rows * row height + the padding of the container itself;
    *   ignoring the padding would leave too little usable height for the list and show a scroll bar by mistake
    */
    void ApplyContentHeight();

    /** Computes the height of a single list item (DIP)
    */
    int32_t GetItemHeight() const;

private:
    /** Suggestion texts
    */
    std::vector<UiString> m_items;

    /** Index of the currently selected item
    */
    size_t m_nSelectedIndex;

    /** Maximum number of visible rows
    */
    int32_t m_nMaxVisible;

    /** Flyout width (DIP)
    */
    int32_t m_nWidthDip;

    /** Gap between the flyout and the anchor (DIP)
    */
    int32_t m_nGapDip;

    /** Internal suggestion list control
    */
    VListBox* m_pList;

    /** Flyout content container (its height follows the number of suggestion rows)
    */
    Box* m_pContent;

    /** Suggestion select callbacks
    */
    std::vector<ItemSelectEvent> m_itemSelectCallbacks;
};

/** Auto-complete input control (AutoComplete): shows suggestions while the user types
*
*   Internal structure:
*   - Edit control: single-line RichEdit (class: auto_complete_edit), filling the whole control
*   - Suggestion flyout: when the typed text meets the condition, an AutoCompleteFlyout with the
*     candidate list pops up below the edit control; the flyout does not take the focus and follows the width of the edit control
*
*   Difference from FilterCombo: AutoComplete is built around an edit control, and its suggestion flyout
*   is filtered in real time while typing without occupying layout space; FilterCombo is built around a drop-down list whose fixed option list is expanded by clicking a button.
*
*   Interaction:
*   - As soon as the typed text reaches min_chars, the candidates are filtered from the data source and the suggestion flyout pops up
*   - The up/down arrow keys move the selection, Enter confirms and Esc closes the flyout; a mouse click on a suggestion confirms it directly
*   - After confirmation the suggestion text is written back to the edit control (without firing the text-changed event) and kEventAutoCompleteSelect is fired
*
*   XML attributes (in addition to the common Box attributes):
*     items         Initial candidate data source, multiple items separated by a semicolon ";"
*     prompt_text   Placeholder text of the edit control
*     prompt_text_id  Multi-language ID of the placeholder text of the edit control
*     prompt_color  Color of the placeholder text of the edit control
*     min_chars     Number of characters to type before suggestions start (default 1)
*     max_visible   Maximum number of visible suggestion rows in the flyout (default 8)
*     match_mode    Matching mode: prefix (default, prefix match) / contains (contains match)
*     ignore_case   Whether matching ignores case (default true)
*
*   Events:
*   - kEventAutoCompleteSelect: fired after a suggestion has been selected; WPARAM is the index of the suggestion in the current candidate list
*   - kEventTextChanged: fired when the text of the internal edit control changes
*
*   Skin classes (defined in global.xml): auto_complete / auto_complete_edit
*   Flyout skin classes: auto_complete_flyout / auto_complete_list / auto_complete_item
*/
class DUI_API AutoComplete : public Box
{
    typedef Box BaseClass;
public:
    explicit AutoComplete(Window* pWindow);
    AutoComplete(const AutoComplete& r) = delete;
    AutoComplete& operator=(const AutoComplete& r) = delete;
    virtual ~AutoComplete() override = default;

    virtual std::string GetType() const override;
    virtual void SetAttribute(const std::string& strName, const std::string& strValue) override;

    /** Gives the focus to the control (the focus is actually set on the internal edit control)
    */
    virtual void SetFocus() override;

    /** Sets the candidate data source (suggestions are always filtered from this original data)
    */
    void SetCandidates(const std::vector<std::string>& items);

    /** Gets the candidate data source
    */
    std::vector<std::string> GetCandidates() const;

    /** Appends a candidate (ignored when an item with the same text already exists)
    */
    void AddCandidate(const std::string& text);

    /** Clears the candidate data source
    */
    void ClearCandidates();

    /** Sets/gets the text of the edit control (a programmatic setting does not show the suggestion flyout)
    */
    void SetText(const std::string& text);
    std::string GetText() const;

    /** Sets the minimum number of typed characters (the suggestions pop up only after it is reached)
    */
    void SetMinChars(int32_t nMinChars);
    int32_t GetMinChars() const;

    /** Sets the maximum number of visible suggestion rows in the flyout
    */
    void SetMaxVisible(int32_t nMaxVisible);
    int32_t GetMaxVisible() const;

    /** Sets the text matching mode
    */
    void SetMatchMode(AutoCompleteMatchMode matchMode);
    AutoCompleteMatchMode GetMatchMode() const;

    /** Sets whether matching ignores case
    */
    void SetIgnoreCase(bool bIgnoreCase);
    bool IsIgnoreCase() const;

    /** Gets the internal edit control
    */
    RichEdit* GetEditControl() const;

public:
    /** Listens for the suggestion select event (WPARAM is the index of the selected item in the current candidate list)
    */
    void AttachSelect(const EventCallback& callback, EventCallbackID callbackID = 0)
    {
        AttachEvent(kEventAutoCompleteSelect, callback, callbackID);
    }

protected:
    /** Initialization interface: creates the internal edit control
    */
    virtual void OnInit() override;

private:
    friend class AutoCompleteEdit;

    /** Handles a text change in the edit control: forwards the event and refreshes the suggestion flyout
    */
    void HandleEditTextChanged();

    /** Key pre-processing of the edit control (intercepts up/down/Enter/Esc while the flyout is open)
    * @return true means the key has been handled and is not passed on to the edit control
    */
    bool HandleEditKeyDown(const EventArgs& msg);

    /** Re-filters the candidates from the current edit text and refreshes the flyout (open/update/close)
    */
    void UpdatePopup();

    /** Opens the suggestion flyout
    */
    void OpenPopup();

    /** Closes the suggestion flyout (if it is open)
    */
    void ClosePopup();

    /** A suggestion has been confirmed (keyboard Enter/mouse click)
    * @param [in] nIndex Index of the suggestion in m_candidates
    */
    void OnCandidateSelected(size_t nIndex);

    /** Builds the candidate list m_candidates from the typed text (always based on the original data source m_items)
    */
    void BuildCandidates(const std::string& input);

    /** Checks whether a single data item matches the typed text
    */
    bool MatchItem(const std::string& item, const std::string& input) const;

    /** Parses the items attribute (semicolon separated)
    */
    void ParseInitItems(const std::string& items);

private:
    /** Internal edit control
    */
    RichEdit* m_pEdit;

    /** Original candidate data source
    */
    std::vector<UiString> m_items;

    /** Current filtered candidate list (matching what the flyout displays)
    */
    std::vector<UiString> m_candidates;

    /** Suggestion flyout (destroyed by the framework after closing, and set to null)
    */
    AutoCompleteFlyout* m_pFlyout;

    /** Minimum number of typed characters
    */
    int32_t m_nMinChars;

    /** Maximum number of visible suggestion rows in the flyout
    */
    int32_t m_nMaxVisible;

    /** Text matching mode
    */
    AutoCompleteMatchMode m_matchMode;

    /** Whether matching ignores case
    */
    bool m_bIgnoreCase;

    /** Text is being written back programmatically (suppresses the text-changed callback so the flyout does not reopen)
    */
    bool m_bInUpdate;

    /** XML attributes cached before OnInit
    */
    UiString m_promptText;
    UiString m_promptTextId;
    UiString m_promptColor;
};

} // namespace ui

#endif // UI_CONTROL_AUTO_COMPLETE_H_
