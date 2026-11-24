#ifndef UI_CONTROL_TAGINPUT_H_
#define UI_CONTROL_TAGINPUT_H_

#include "dui/Core/Box.h"
#include <vector>

namespace ui
{
class RichEdit;
class TagInputLayout;

/** Tag input control (TagInput): a composite control that turns the typed text into tags when Enter is pressed and
*   supports removing the tags
*
*   Internal structure (horizontal flow layout, the tags wrap onto new rows automatically when there are too many):
*   - Tag: an HBox (class: tag_input_tag) that contains the text Label (tag_input_tag_text)
*          and a delete Button (tag_input_tag_close) on its right side
*   - Edit control: a RichEdit (class: tag_input_edit) that always follows the last tag and whose
*             width fills the remaining space of the current row; when the text is empty, Backspace removes the last
*             tag
*
*   Interaction:
*   - Type text and press Enter: the text becomes a tag and the edit control is cleared
*   - When the delimiter attribute is configured, typing that delimiter (a comma, for example) also creates a tag
*   - Click the X button on the right side of a tag: that tag is removed
*   - Press Backspace while the edit control is empty: the last tag is removed
*
*   XML attributes (in addition to the common Box attributes):
*     prompt_text     Placeholder text of the edit control
*     prompt_text_id  Multilingual ID of the edit control placeholder text
*     prompt_color    Placeholder text color of the edit control
*     allow_duplicate Whether duplicate tags are allowed (default false, not allowed)
*     max_count       Maximum number of tags (default 0, meaning no limit)
*     delimiter       Delimiter that creates a tag directly while typing (default empty, meaning only Enter creates
*                     tags)
*     tags            Initial tag texts, the tags are separated by semicolons ";"
*
*   Events:
*   - kEventTagAdded: fired after a tag was added, WPARAM is the index of the new tag (starting from 0)
*   - kEventTagRemoved: fired before a tag is removed, WPARAM is the index of the removed tag,
*                       the callback can still read the text with GetTagText(index)
*   - kEventTagClick: fired when the body of a tag is clicked, WPARAM is the tag index
*   - kEventTextChanged: fired when the text of the internal edit control changes
*
*   Skin classes (defined in global.xml): tag_input / tag_input_tag /
*   tag_input_tag_text / tag_input_tag_close / tag_input_edit
*/
class DUI_API TagInput : public Box
{
    typedef Box BaseClass;
public:
    explicit TagInput(Window* pWindow);
    TagInput(const TagInput& r) = delete;
    TagInput& operator=(const TagInput& r) = delete;
    virtual ~TagInput() override = default;

    virtual std::string GetType() const override;
    virtual void SetAttribute(const std::string& strName, const std::string& strValue) override;

    /** Gives the focus to the control (the focus is actually set on the internal edit control)
    */
    virtual void SetFocus() override;

    /** Adds a tag
    * @param [in] text Tag text (leading and trailing blanks are trimmed; an empty text returns false)
    * @return true when the tag was added; false when the text is empty, the tag is a duplicate or the maximum
    *         number of tags was reached
    */
    bool AddTag(const std::string& text);

    /** Removes the tag with the given index
    * @param [in] index Tag index (starting from 0)
    * @return true when the tag was removed; false when the index is invalid
    */
    bool RemoveTagAt(size_t index);

    /** Removes all tags (every tag fires the kEventTagRemoved event)
    */
    void RemoveAllTags();

    /** Gets the number of tags
    */
    size_t GetTagCount() const;

    /** Gets the text of the tag with the given index
    */
    std::string GetTagText(size_t index) const;

    /** Sets whether duplicate tags are allowed
    */
    void SetAllowDuplicate(bool bAllowDuplicate);

    /** Whether duplicate tags are allowed
    */
    bool IsAllowDuplicate() const;

    /** Sets the maximum number of tags (0 means no limit)
    */
    void SetMaxCount(int32_t nMaxCount);

    /** Gets the maximum number of tags (0 means no limit)
    */
    int32_t GetMaxCount() const;

    /** Gets the internal edit control
    */
    RichEdit* GetEditControl() const;

public:
    /** Listens for the tag added event (WPARAM is the index of the new tag)
    */
    void AttachTagAdded(const EventCallback& callback, EventCallbackID callbackID = 0)
    {
        AttachEvent(kEventTagAdded, callback, callbackID);
    }

    /** Listens for the tag removed event (WPARAM is the index of the removed tag; the tag still exists when the
    *  event fires)
    */
    void AttachTagRemoved(const EventCallback& callback, EventCallbackID callbackID = 0)
    {
        AttachEvent(kEventTagRemoved, callback, callbackID);
    }

    /** Listens for the tag body click event (WPARAM is the tag index)
    */
    void AttachTagClick(const EventCallback& callback, EventCallbackID callbackID = 0)
    {
        AttachEvent(kEventTagClick, callback, callbackID);
    }

protected:
    /** Initialization interface: creates the tag, edit control and other child controls
    */
    virtual void OnInit() override;

private:
    /** Creates one tag control
    * @param [in] text Tag text
    */
    Control* CreateTagItem(const std::string& text);

    /** Commits the current text of the edit control as a tag
    */
    void CommitInput();

    /** Handles Backspace while the internal edit control is empty: removes the last tag
    * @return true when the key was handled (it is not passed on to the edit control)
    */
    bool HandleEditBackspace();

    /** Edit control text change handler: turns the delimiter into a tag and forwards the text changed event
    */
    void HandleEditTextChanged();

    /** Decides whether a tag with the given text already exists
    */
    bool IsTagExists(const std::string& text) const;

    /** Parses the tags attribute (semicolon separated initial tag texts)
    */
    void ParseInitTags(const std::string& tags);

private:
    /** Internal edit control
    */
    RichEdit* m_pEdit;

    /** Tag text list (the index matches the order of the tag child controls)
    */
    std::vector<UiString> m_tags;

    /** Whether duplicate tags are allowed
    */
    bool m_bAllowDuplicate;

    /** Maximum number of tags (0 means no limit)
    */
    int32_t m_nMaxCount;

    /** Delimiter that creates a tag (empty means it is disabled)
    */
    UiString m_delimiter;

    /** Initial tag texts (the tags attribute of the XML is cached before OnInit, semicolon separated)
    */
    UiString m_initTags;

    /** Placeholder text (cached before OnInit)
    */
    UiString m_promptText;

    /** Multilingual ID of the placeholder text (cached before OnInit)
    */
    UiString m_promptTextId;

    /** Placeholder text color (cached before OnInit)
    */
    UiString m_promptColor;

    friend class TagInputEdit;
};

} // namespace ui

#endif // UI_CONTROL_TAGINPUT_H_
