#ifndef UI_CONTROL_RICHEDIT_WINDOWS_H_
#define UI_CONTROL_RICHEDIT_WINDOWS_H_

#include "dui/Box/ScrollBox.h"
#include "dui/Image/Image.h"

#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)

#include "dui/Control/RichEditCtrl_Windows.h"

/** Define a macro switch that determines whether the RichText feature is supported (enabled by default)
*/
#define DUI_RICHEDIT_SUPPORT_RICHTEXT 1

namespace ui 
{

/** The index range of characters
*/
struct TextCharRange
{
    int32_t cpMin = -1; //The starting index of the character
    int32_t cpMax = -1; //The ending index of the character
};

/** Parameters for character search
*/
struct FindTextParam
{
    bool bMatchCase = true;      //Whether to match case when searching
    bool bMatchWholeWord = true; //Whether to match whole words when searching
    bool bFindDown = true;       //Whether to search forward, true means forward, false means backward
    TextCharRange chrg;          //The range of characters to search
    DString findText;            //The text to search for
};

class RichEditHost;
class ControlDropTarget_Windows;
class VBox;
class DUI_API RichEdit : public ScrollBox
{
    typedef ScrollBox BaseClass;
public:
    explicit RichEdit(Window* pWindow);
    RichEdit(const RichEdit& r) = delete;
    RichEdit& operator=(const RichEdit& r) = delete;
    virtual ~RichEdit() override;
public:
    //Overrides of base class virtual functions
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& pstrName, const DString& pstrValue) override;
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;
    virtual void SetPos(UiRect rc) override;
    virtual void SetScrollPos(UiSize64 szPos) override;
    virtual void PaintStateImages(IRender* pRender) override;
    virtual void ClearImageCache() override;
    virtual void HandleEvent(const EventArgs& msg) override;
    virtual UiEstSize EstimateSize(UiSize szAvailable) override;
    virtual UiSize EstimateText(UiSize szAvailable) override;

public:
    /** Set the control's text, which triggers a text change event
     * @param [in] strText The text content to set
     */
    void SetText(const DStringW& strText);
    void SetText(const DStringA& strText);

    /** Set the control's text without triggering a text change event
     * @param [in] strText The text content to set
     */
    void SetTextNoEvent(const DString& strText);

    /** Set the text ID of the control
     * @param[in] strTextId The ID to set, which must exist in the loaded language file
     */
    void SetTextId(const DString& strTextId);

    /** Get the text in the control
     * @return Returns the text content in the control
     */
    DString GetText() const;

    /** Get the length of the content (Unicode encoding, number of characters)
     * @return Returns the content length
     */
    int32_t GetTextLength() const;

    /** Get the currently set font index
     * @return Returns the font index (corresponding to the font order in global.xml)
     */
    DString GetFontId() const;

    /** Set the font index
     * @param[in] index The font index to set (corresponding to the font order in global.xml)
     */
    void SetFontId(const DString& strFontId);

    /** Get the font information (the font size has been DPI scaled)
    * @return RichText mode: returns the font information of the currently selected text, otherwise returns the default font information
    */
    UiFont GetFontInfo() const;

    /** Set the font information (high priority, overrides the font set via SetFontId)
    * @param [in] fontInfo The font information; the font size has been DPI scaled
    */
    bool SetFontInfo(const UiFont& fontInfo);

    /** Get the currently used font ID
    * @return The return value is the same as the GetFontId() function
    */
    DString GetCurrentFontId() const;

    /** Set the normal text color
     * @param[in] dwTextColor The text color to set
     */
    void SetTextColor(const DString& dwTextColor);

    /** Get the normal text color
     */
    DString GetTextColor() const;

    /** Get the color of the selected text (only valid in rich text mode)
    */
    DString GetSelectionTextColor() const;

    /** Set the color of the selected text (only valid in rich text mode)
     * @param[in] textColor The text color to set
     */
    void SetSelectionTextColor(const DString& textColor);

    /** Set the text color in the Disabled state
     * @param[in] dwTextColor The text color to set
     */
    void SetDisabledTextColor(const DString& dwTextColor);

    /** Get the text color in the Disabled state
     */
    DString GetDisabledTextColor() const;

public:
    /** Set whether to show the prompt text
 * @param[in] bPrompt Set to true to show, false to hide
 */
    void SetPromptMode(bool bPrompt);

    /** Get the prompt text
     */
    DString GetPromptText() const;

    /** Get the prompt text
     * @return Returns the prompt text in UTF8 format
     */
    std::string GetUTF8PromptText() const;

    /** Set the prompt text
     * @param[in] strText The prompt text to set
     */
    void SetPromptText(const DString& strText);

    /** Set the prompt text
     * @param[in] strText The prompt text in UTF8 format to set
     */
    void SetUTF8PromptText(const std::string& strText);

    /** Set the prompt text ID
     * @param[in] strText The prompt text ID to set, which must exist in the loaded language file
     */
    void SetPromptTextId(const DString& strTextId);

    /** Set the prompt text ID
     * @param[in] strText The prompt text ID in UTF8 format to set, which must exist in the loaded language file
     */
    void SetUTF8PromptTextId(const std::string& strTextId);

public:
    /** Set the text padding information
     * @param[in] padding The padding information
     * @param[in] bNeedDpiScale Whether DPI scaling is supported
     */
    void SetTextPadding(UiPadding padding, bool bNeedDpiScale);

    /** Get the text padding
     */
    UiPadding GetTextPadding() const;

    /** Whether it is multi-line text
    */
    bool IsMultiLine() const;

    /** Set whether it is multi-line text
    */
    void SetMultiLine(bool bMultiLine);

    /** Get the display mode of text that exceeds the rectangular area
     * @return Returns true when in multi-line mode the content is wrapped, false means truncated display
     */
    bool IsWordWrap() const;

    /** Set the display mode of text that exceeds the rectangular area
     * @param[in] bWordWrap When true and in multi-line mode, the content is wrapped; false means truncated display
     */
    void SetWordWrap(bool bWordWrap);

    /** Determine whether to accept TAB key messages
     * @return Returns true to accept: the TAB key is converted to text as an input character; false means not accepted: the TAB key is used as a control shortcut instead of as input text
     */
    virtual bool IsWantTab() const override;

    /** Determine whether to accept TAB key messages
     * @param[in] bWantTab True to accept: the TAB key is converted to text as an input character; false means not accepted: the TAB key is used as a control shortcut instead of as input text
     */
    void SetWantTab(bool bWantTab);

    /** Determine whether to accept Enter key messages
     * @return Returns true to accept: the Enter key is converted to text as an input character; false means not accepted: the Enter key is used as a control shortcut instead of as input text
     */
    bool IsWantReturn() const;

    /** Set whether to accept Enter key messages
     * @param[in] bWantReturn True to accept: the Enter key is converted to text as an input character; false means not accepted: the Enter key is used as a control shortcut instead of as input text
     */
    void SetWantReturn(bool bWantReturn);

    /** Determine whether to accept the CTRL+RETURN key combination message
     * @return Returns true to accept, false to reject
     */
    bool IsWantCtrlReturn() const;

    /** Set whether to accept the CTRL+RETURN key combination message
     * @param[in] bWantCtrlReturn True accepts the message, false rejects it
     */
    void SetWantCtrlReturn(bool bWantCtrlReturn);

    /** Whether it is in the read-only state
     * @return Returns true for the read-only state, otherwise false
     */
    bool IsReadOnly() const;

    /** Set the control to the read-only state
     * @param[in] bReadOnly Set true to make the control read-only, false for writable
     */
    void SetReadOnly(bool bReadOnly);

    /** Whether it is a password mode control
     * @return Returns true if it is a password control, otherwise false
     */
    bool IsPasswordMode() const;

    /** Set the control as a password control (displays ***)
     * @param[in] bPasswordMode Set true to display *** as the content, false to display normal content
     */
    void SetPasswordMode(bool bPasswordMode);

    /** Set whether to show the password
    */
    void SetShowPassword(bool bShow);

    /** Whether to show the password
    */
    bool IsShowPassword() const;

    /** Set the password character
    */
    void SetPasswordChar(DStringW::value_type ch);

    /** Set whether to briefly show input characters then hide them (only valid when IsShowPassword() is true, i.e. in password mode)
    */
    void SetFlashPasswordChar(bool bFlash);

    /** Get whether input characters are briefly shown then hidden
    */
    bool IsFlashPasswordChar() const;

    /** Whether only numbers are allowed
    */
    bool IsNumberOnly() const;

    /** Set whether only numbers are allowed
    */
    void SetNumberOnly(bool bNumberOnly);

    /** Set the maximum allowed number (only valid when IsNumberOnly() is true)
    */
    void SetMaxNumber(int32_t maxNumber);

    /** Get the maximum allowed number
    */
    int32_t GetMaxNumber() const;

    /** Set the minimum allowed number (only valid when IsNumberOnly() is true)
    */
    void SetMinNumber(int32_t minNumber);

    /** Get the minimum allowed number
    */
    int32_t GetMinNumber() const;

    /** Get the number format (the format of a 64-bit signed integer, e.g. "I64d")
    */
    void SetNumberFormat64(const DString& numberFormat);

    /** Get the number format (the format of a 64-bit signed integer)
    */
    DString GetNumberFormat64() const;

    /** Get the limit of the number of characters
     * @return Returns the limit of the number of characters
     */
    int32_t GetLimitText() const;

    /** Set the limit of the number of characters
     * @param [in] iChars The number of characters to limit
     */
    void SetLimitText(int32_t iChars);

    /** Get which characters are allowed to be entered
    */
    DString GetLimitChars() const;

    /** Set which characters are allowed to be entered, e.g. for color values: limit_chars="#0123456789ABCDEFabcdef"
    * @param [in] limitChars The list of allowed characters
    */
    void SetLimitChars(const DString& limitChars);

    /** Get the image in the focused state
    * @return Returns the image in the focused state
    */
    DString GetFocusedImage();

    /** Set the image in the focused state
     * @param[in] strImage The image location to set
     */
    void SetFocusedImage(const DString& strImage);

    /** Set whether to allow adjusting the zoom ratio via Ctrl + mouse wheel
    */
    void SetEnableWheelZoom(bool bEnable);

    /** Get whether adjusting the zoom ratio via Ctrl + mouse wheel is allowed
    */
    bool IsEnableWheelZoom(void) const;

    /** Whether the default context menu is allowed
    */
    void SetEnableDefaultContextMenu(bool bEnable);

    /** Whether the default context menu is allowed
    */
    bool IsEnableDefaultContextMenu() const;

    /** Set whether the Spin control is supported
    * @param [in] bEnable true means the Spin control is supported, false means not supported
    * @param [in] spinClass Spin control's Class attribute; the string needs to contain 3 values, see the rich_edit_spin setting in global.xml for details
    *             Example values: rich_edit_spin_box,rich_edit_spin_btn_up,rich_edit_spin_btn_down
    * @param [in] nMin Sets the minimum value of the number
    * @param [in] nMax Sets the maximum value of the number; if both nMin and nMax are 0, the minimum and maximum values of the number are not set
    */
    bool SetEnableSpin(bool bEnable, const DString& spinClass, int32_t nMin = 0, int32_t nMax = 0);

public:
    /** Create the caret
     * @param [in] xWidth The caret width
     * @param [in] yHeight The caret height
     */
    void CreateCaret(int32_t xWidth, int32_t yHeight);

    /** Get the width and height of the caret
    */
    void GetCaretSize(int32_t& xWidth, int32_t& yHeight) const;

    /** Set whether to show the caret
     * @param [in] fShow Set true to show, false to hide
     */
    void ShowCaret(bool fShow);

    /** Set the caret color
     * @param[in] dwColor The color value to set, which must exist in global.xml
     */
    void SetCaretColor(const DString& dwColor);

    /** Get the caret color
     * @return Returns the caret color
     */
    DString GetCaretColor() const;

    /** Get the caret rectangle position
     * @return Returns the caret rectangle position
     */
    UiRect GetCaretRect() const;

    /** Set the caret position
     * @param [in] xPos X coordinate
     * @param [in] yPos Y coordinate
     */
    void SetCaretPos(int32_t xPos, int32_t yPos);

    /** Get the caret position
     * @param [out] xPos X coordinate
     * @param [out] yPos Y coordinate
    */
    void GetCaretPos(int32_t& xPos, int32_t& yPos) const;

    /** Set not to show the caret in read-only mode
    */
    void SetNoCaretReadonly();

    /** Set whether to use the Control's caret
    */
    void SetUseControlCursor(bool bUseControlCursor);

    /** Get the row spacing multiplier
    */
    float GetRowSpacingMul() const;

    /** Set the row spacing multiplier (only valid when IsRichText() is true)
    */
    void SetRowSpacingMul(float fRowSpacingMul);

public:
    /** Listen for the Enter key press event
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * @param [in] callback The custom callback function for when Enter is pressed
     */
    void AttachReturn(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventReturn, callback, callbackID); }

    /** Listen for the ESC key press event
     * @param [in] callback The custom callback function for when Enter is pressed
     * @param [in] callback The custom callback function for when Enter is pressed
     */
    void AttachEsc(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventEsc, callback, callbackID); }

    /** Listen for the TAB key press event
     * @param [in] callback TAB pressed custom callback function
     * @param [in] callback The custom callback function for when Enter is pressed
     */
    void AttachTab(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventTab, callback, callbackID); }

    /* Listen for the zoom ratio change event
     * @param [in] callback The custom callback function for after the text is modified
     * @param [in] callback The custom callback function for when Enter is pressed
     */
    void AttachZoom(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventZoom, callback, callbackID); }

    /* Listen for the text modified event
     * @param [in] callback The custom callback function for after the text is modified
     * @param [in] callback The custom callback function for when Enter is pressed
     */
    void AttachTextChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventTextChanged, callback, callbackID); }

    /* Listen for the text selection change event
     * @param [in] callback The custom callback function for after the text selection changes
     * @param [in] callback The custom callback function for when Enter is pressed
     */
    void AttachSelChanged(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the hyperlink click event
     * @param [in] callback The callback function for after the hyperlink is clicked
     * @param [in] callback The custom callback function for when Enter is pressed
     */
    void AttachLinkClick(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventLinkClick, callback, callbackID); }

public:
    /** Get the modified flag
     * @return Returns true if the modified flag is set, otherwise false
     */
    bool GetModify() const;

    /** Set the modified flag
     * @param[in] bModified Set true to indicate the text has been modified, false for not modified, defaults to true
     */
    void SetModify(bool bModified = true);

    /** Select all
     * @return Returns the number of selected items
     */
    int32_t SetSelAll();

    /** Select nothing
     */
    void SetSelNone();

    /** Set whether to deselect the selection after losing focus
     * @param[in] bOnSel Set true to deselect the selection, false to keep it
     */
    void SetNoSelOnKillFocus(bool bOnSel);

    /** Set whether to select all content after gaining focus
     * @param[in] bSelAll Set true to select all content when gaining focus, false to not select
     */
    void SetSelAllOnFocus(bool bSelAll);

    /** Get the start and end positions of the selected text
     * @param[in] nStartChar Returns the start position
     * @param[in] nEndChar Returns the end position
     */
    void GetSel(int32_t& nStartChar, int32_t& nEndChar) const;

    /** Select a portion of the content
     * @param[in] nStartChar The start position to select
     * @param[in] nEndChar The end position to select
     * @return Returns the number of selected characters
     */
    int32_t SetSel(int32_t nStartChar, int32_t nEndChar);

    /** Replace the selected content
     * @param[in] lpszNewText The text to replace with
     * @param[in] bCanUndo Whether it can be undone, true for yes, otherwise false
     */
    void ReplaceSel(const DString& lpszNewText, bool bCanUndo);

    /** Get the selected text content
     * @return Returns the selected text content
     */
    DString GetSelText() const;

    /** Get the content of the specified range
     * @param[in] nStartChar The start position
     * @param[in] nEndChar The end position
     * @return Returns the content of the specified position
     */
    DString GetTextRange(int32_t nStartChar, int32_t nEndChar) const;

    /** Set whether to hide or show the selected text
     * @param [in] bHide Whether to show, true to hide, false to show
     * @param [in] bChangeStyle Whether to change the style, true to change, false to not change
     */
    void HideSelection(bool bHide = true, bool bChangeStyle = false);

    /** Whether Redo is possible
    */
    bool CanRedo() const;

    /** Redo operation
     * @return Returns true on success, false on failure
     */
    bool Redo();

    /** Whether Undo is possible
    */
    bool CanUndo() const;

    /** Undo operation
     * @return Returns true on success, false on failure
     */
    bool Undo();

    /** Delete the currently selected content
     */
    void Clear();

    /** Copy the selected content
     */
    void Copy();

    /** Cut the selected content
     */
    void Cut();

    /** Paste
     */
    void Paste();

    /** Check whether pasting is possible
    */
    bool CanPaste() const;

    /** Get the total number of lines
     * @return Returns the total number of lines
     */
    int32_t GetLineCount() const;

    /** Get one line of data
     * @param[in] nIndex The line number to get
     * @param[in] nMaxLength The maximum data length of the current line to get
     * @return Returns the obtained line of data
     */
    DString GetLine(int32_t nIndex, int32_t nMaxLength) const;

    /** Get the index of the first character of the specified line
     * @param[in] nLine The line number to get, defaults to -1, meaning the line of the current insertion point
     * @return Returns the index of the first character of the specified line
     */
    int32_t LineIndex(int32_t nLine = -1) const;

    /** Get the data length of the specified line
     * @param[in] nLine The line number to get, defaults to -1, meaning the line of the current insertion point
     * @return Returns the data length of the specified line
     */
    int32_t LineLength(int32_t nLine = -1) const;

    /** Scroll the text
     * @param[in] nLines Specifies the vertical scroll direction
     * @return Returns true on success, false on failure
     */
    bool LineScroll(int32_t nLines);

    /** Get the line number of the specified character
     * @param[in] nIndex The index position of the character
     * @return Returns the line number of the current character
     */
    int32_t LineFromChar(int32_t nIndex) const;

    /** Retrieve the client-area coordinates of the specified character in the edit control.
     * @param[in] nChar The character index position
     * @return The return value contains the client-area coordinates of the character.
     */
    UiPoint PosFromChar(int32_t nChar) const;

    /** Get information about the character closest to the specified point in the client area of the edit control
     * @param[in] pt The coordinate information
     * @return The return value specifies the zero-based character index of the character closest to the specified point. If the specified point is beyond the last character in the control, the return value indicates the last character in the edit control.
     */
    int32_t CharFromPos(UiPoint pt) const;

    /** Clear the undo list
     */
    void EmptyUndoBuffer();

    /** Set the number of items the undo list can hold
     * @param [in] nLimit
     * @return Returns the number of items the undo list can hold after setting
     */
    uint32_t SetUndoLimit(uint32_t nLimit);

public:
    /** Scroll the scrollbar up
     * @param[in] deltaValue Unused
     */
    virtual void LineUp(int32_t deltaValue = DUI_NOSET_VALUE) override;

    /** Scroll the scrollbar down
     * @param[in] deltaValue Unused
     */
    virtual void LineDown(int32_t deltaValue = DUI_NOSET_VALUE) override;

    /** Page up
     */
    virtual void PageUp() override;

    /** Page down
     */
    virtual void PageDown() override;

    /** Return to the top
     */
    virtual void HomeUp() override;

    /** Scroll to the bottom
     * @param[in] arrange Unused
     */
    virtual void EndDown(bool arrange = true) override;

    /** Scroll the scrollbar left
     * @param[in] deltaValue Unused
     */
    virtual void LineLeft(int32_t deltaValue = DUI_NOSET_VALUE) override;

    /** Scroll the scrollbar right
     * @param[in] deltaValue Unused
     */
    virtual void LineRight(int32_t deltaValue = DUI_NOSET_VALUE) override;

    /** Page left horizontally
     */
    virtual void PageLeft() override;

    /** Page right horizontally
     */
    virtual void PageRight() override;

    /** Return to the far left
     */
    virtual void HomeLeft() override;

    /** Return to the far right
     */
    virtual void EndRight() override;

public:
    /** Convert screen coordinates to client-area coordinates
    */
    virtual bool ScreenToClient(UiPoint& pt) override;

    /** Convert client-area coordinates to screen coordinates
    */
    virtual bool ClientToScreen(UiPoint& pt) override;

    /** Convert the font size to the font height of the Rich Edit control
    */
    int32_t ConvertToFontHeight(int32_t fontSize) const;

    /** Find text
    * @param [in] findParam The search parameters
    * @param [out] chrgText The matched text, the index range of the characters
    */
    bool FindRichText(const FindTextParam& findParam, TextCharRange& chrgText) const;

    /** Get the text content and convert it to a number
    */
    int64_t GetTextNumber() const;

    /** Convert a number to text and set the text content
    */
    void SetTextNumber(int64_t nValue);

    /** Adjust the text number value
    */
    void AdjustTextNumber(int32_t nDelta);

    /** Set whether to hide the selection
     */
    void SetHideSelection(bool fHideSelection);

    /** Whether to hide the selected text (whether to hide the selection when the control is inactive)
    */
    bool IsHideSelection() const;

    /** Set the size of the bottom border in the focused state
    * @param [in] nBottomBorderSize The size of the bottom border (not DPI scaled)
    * @param [in] bNeedDpiScale Whether DPI scaling is supported
    */
    void SetFocusBottomBorderSize(int32_t nBottomBorderSize);

    /** Get the size of the bottom border in the focused state (not DPI scaled)
    */
    int32_t GetFocusBottomBorderSize() const;

    /** Set the color of the bottom border in the focused state
    */
    void SetFocusBottomBorderColor(const DString& bottomBorderColor);

    /** Get the color of the bottom border in the focused state
    */
    DString GetFocusBottomBorderColor() const;

    /** Set whether to enable drag and drop
    */
    virtual void SetEnableDragDrop(bool bEnable) override;

    /** Check whether drag and drop is enabled
    */
    virtual bool IsEnableDragDrop() const override;

    /** Get the drag and drop interface
    * @return Returns the drag and drop target interface; if nullptr is returned, drag and drop is not supported
    */
    virtual ControlDropTarget_Windows* GetControlDropTarget() override;

    /** Get the drag and drop interface (SDL)
    * @return Returns the drag and drop target interface; if nullptr is returned, drag and drop is not supported
    */
    virtual ControlDropTarget_SDL* GetControlDropTarget_SDL() override;

#ifdef DUI_RICHEDIT_SUPPORT_RICHTEXT
public:
    /** Whether it is rich text mode
     * @return Returns true for rich text mode: supports rich text formats and RTF format
               Returns false for plain text mode: the text in a plain text control can only have one format
     */
    bool IsRichText() const;

    /** Set the control to rich text mode
     * @param[in] bRichText Set true for rich text mode, false for plain text mode
     */
    void SetRichText(bool bRichText);

    /** Set whether to save the boundary of the selected content
    */
    void SetSaveSelection(bool fSaveSelection);

    /** Get the selection type of the control
     * @return Returns the selection type of the control, see: https://docs.microsoft.com/en-us/windows/desktop/controls/em-selectiontype
        SEL_TEXT: Text.
        SEL_OBJECT: At least one COM object.
        SEL_MULTICHAR: More than one character of text.
        SEL_MULTIOBJECT: More than one COM object.
     */
    WORD GetSelectionType() const;

    /** Find text
    */
    int32_t FindRichText(DWORD dwFlags, FINDTEXTW& ft) const;

    /** Find text
    */
    int32_t FindRichText(DWORD dwFlags, FINDTEXTEXW& ft) const;

    /** Set the zoom percentage
    * @param [in] fZoomRatio The zoom ratio, e.g. "100" means 100%, no zoom; "200" means a zoom ratio of 200%
    */
    void SetZoomPercent(uint32_t nZoomPercent);

    /** Get the zoom percentage
    */
    uint32_t GetZoomPercent() const;

    /** Get whether automatic URL detection is enabled (read from RichEditHost)
     * @return Returns true if automatic detection is enabled, otherwise false
     */
    bool GetAutoURLDetect() const;

    /** Set whether to enable automatic URL detection
     * @param[in] bAutoDetect Set true to detect URLs, false to not detect, defaults to true
     * @return Returns true on success, false on failure
     */
    bool SetAutoURLDetect(bool bAutoDetect = true);

    /** Get the event mask of the control
     * @return Returns the event mask
     */
    DWORD GetEventMask() const;

    /** Set the event mask of the control
     * @param[in] dwEventMask The event mask value to set
     * @return Returns the event mask value before setting
     */
    DWORD SetEventMask(DWORD dwEventMask);

    /** Select a portion of the content
     * @param[in] cr The start and end positions of the text to select
     * @return Returns the number of selected characters
     */
    int32_t SetSel(CHARRANGE& cr);

    /** Whether a Beep sound is allowed
    */
    bool GetAllowBeep() const;

    /** Set whether a Beep sound is allowed
    */
    void SetAllowBeep(bool bAllowBeep);

    /** Scroll the caret into view
     */
    void ScrollCaret();

    /** Insert text
     * @param[in] nInsertAfterChar The position to insert at
     * @param[in] text The text to insert
     * @param[in] bCanUndo Whether it can be undone, true for yes, otherwise false, defaults to false
     * @return Returns the text position after insertion
     */
    int32_t InsertText(int32_t nInsertAfterChar, const DString& text, bool bCanUndo = false);

    /** Append text
     * @param [in] text The text to append
     * @param [in] bCanUndo Whether it can be undone, true for yes, otherwise false, defaults to false
     * @param [in] bScrollBottom Whether to scroll the view to the bottom
     * @return Returns the text position after appending
     */
    int32_t AppendText(const DString& text, bool bCanUndo = false, bool bScrollBottom = true);

    /** Get the character format
     * @param[out] cf Returns the obtained character format
     * @return Returns the value of dwMask in parameter cf
     */
    DWORD GetDefaultCharFormat(CHARFORMAT2W& cf) const;

    /** Set the default character format
     * @param[in] cf The character format to set
     * @return Returns true on success, false on failure
     */
    bool SetDefaultCharFormat(CHARFORMAT2W& cf);

    /** Get the character format of the selected text
     * @param[out] cf Returns the obtained character format
     * @return Returns the value of dwMask in parameter cf
     */
    DWORD GetSelectionCharFormat(CHARFORMAT2W& cf) const;

    /** Set the character format of the selected text
     * @param[in] cf The character format to set
     * @return Returns true on success, false on failure
     */
    bool SetSelectionCharFormat(CHARFORMAT2W& cf);

    /** Set the word format at the current insertion point
     * @param[in] cf The word format to set
     * @return Returns true on success, false on failure
     */
    bool SetWordCharFormat(CHARFORMAT2W& cf);

    /** Get the current paragraph format
     * @param[out] pf Returns the current paragraph format
     * @return Returns the dwMask member of the pf parameter
     */
    DWORD GetParaFormat(PARAFORMAT2& pf) const;

    /** Set the current paragraph format
     * @param[in] pf The paragraph format style to set
     * @return Returns true on success, otherwise false
     */
    bool SetParaFormat(PARAFORMAT2& pf);

    /** Get the start and end positions of the selected text
     * @param[out] cr Returns the start and end positions
     */
    void GetSel(CHARRANGE& cr) const;

    /** Check whether the specified clipboard format can be pasted
     * @param[in] nFormat The format to check
     * @return Returns true if possible, otherwise false
     */
    BOOL CanPaste(UINT nFormat);

    /** Paste a specific clipboard format into the control
     * @param[in] uClipFormat Specifies the clipboard format
     * @param[in] dwAspect Specifies the presentation form
     * @param[in] hMF If dwAspect is DVASPECT_ICON, this function should contain the icon handle
     */
    void PasteSpecial(UINT uClipFormat, DWORD dwAspect = 0, HMETAFILE hMF = 0);

    /** Add a callback to control the input content
     * @param[in] nFormat Specifies the replacement options for the data format, see: https://docs.microsoft.com/en-us/windows/desktop/controls/em-streamin
     * @param[in] es A structure containing the custom callback
     * @return Returns the size of the data read into the stream
     */
    long StreamIn(UINT nFormat, EDITSTREAM& es);

    /** Specify a callback to control the output content
     * @param[in] nFormat Specifies the replacement options for the data format, see: https://docs.microsoft.com/en-us/windows/desktop/controls/em-streamin
     * @param[in] es A structure containing the custom callback
     * @return Returns the size of the data written to the stream
     */
    long StreamOut(UINT nFormat, EDITSTREAM& es);

    /** Add colored text
     * @param[in] str The text content
     * @param[in] color The color value, which must exist in global.xml
     */
    void AddColorText(const DString& str, const DString& color);

    /** Add a hyperlink with a specified font and text color
     * @param[in] str The text content
     * @param[in] color The text color
     * @param[in] linkInfo The link address
     * @param[in] font The font index
     */
    void AddLinkColorTextEx(const DString& str, const DString& color, const DString& linkInfo = _T(""), const DString& strFontId = _T(""));

#endif

protected:

    //Some virtual functions of the base class
    virtual bool CanPlaceCaptionBar() const override;
    virtual void OnInit() override;
    virtual uint32_t GetControlFlags() const override;

    //Message handler functions
    virtual bool OnSetCursor(const EventArgs& msg) override;
    virtual bool OnSetFocus(const EventArgs& msg) override;
    virtual bool OnKillFocus(const EventArgs& msg) override;
    virtual bool OnChar(const EventArgs& msg) override;
    virtual bool OnKeyDown(const EventArgs& msg) override;
    virtual bool OnImeStartComposition(const EventArgs& msg) override;
    virtual bool OnImeEndComposition(const EventArgs& msg) override;
    virtual void Paint(IRender* pRender, const UiRect& rcPaint) override;
    virtual void PaintChild(IRender* pRender, const UiRect& rcPaint) override;
    virtual void PaintBorder(IRender* pRender) override;

    /** Arrange the position information of all internal child controls
     * @param[in] items The list of controls
     */
    virtual void ArrangeChildren(const std::vector<Control*>& items) const;

    /** Set the enabled state event
    * @param [in] bChanged true means the state has changed, false means the state has not changed
    */
    virtual void OnSetEnabled(bool bChanged) override;

private:
    /** Show the menu on the RichEdit
    * @param [in] point The coordinates in the client area
    */
    void ShowPopupMenu(const ui::UiPoint& point);

    /** Determine whether a character is in the list of restricted characters
    */
    bool IsInLimitChars(DStringW::value_type charValue) const;

    /** Determine whether pasting is allowed (determine whether there are character restrictions or number restrictions)
    * @return If true is returned, pasting is not allowed
    */
    bool IsPasteLimited() const;

private:
    /** Trigger the text change event
    */
    void OnTextChanged();

    /** Set the Class name of the Spin function
    */
    bool SetSpinClass(const DString& spinClass);

    /** Start the timer for adjusting the text number value
    */
    void StartAutoAdjustTextNumberTimer(int32_t nDelta);

    /** Start automatically adjusting the text number value
    */
    void StartAutoAdjustTextNumber(int32_t nDelta);

    /** Stop automatically adjusting the text number value
    */
    void StopAutoAdjustTextNumber();

private:
    /** Set the Class name of the clear button function
    */
    void SetClearBtnClass(const DString& btnClass);

    /** Set the Class name of the show-password button function
    */
    void SetShowPasswordBtnClass(const DString& btnClass);

    /** Set the font ID
    */
    void SetFontIdInternal(const DString& fontId);

    /** Set the font color
    */
    void SetTextColorInternal(const UiColor& textColor);

    /** Get the format corresponding to the font
    */
    void GetCharFormat(const DString& fontId, CHARFORMAT2W& cf) const;

    //Text horizontal and vertical alignment
    void SetTextHAlignType(HorAlignType alignType);
    void SetTextVAlignType(VerAlignType alignType);

    /** Draw using a private DC
    */
    void PaintRichEdit(IRender* pRender, const UiRect& rcPaint);

    /** Draw the caret
     * @param[in] pRender The drawing engine
     * @param[in] rcPaint The drawing area
     */
    void PaintCaret(IRender* pRender, const UiRect& rcPaint);

    /** Toggle whether the caret is shown
    */
    void ChangeCaretVisiable();

    /** Draw the prompt text
     * @param[in] pRender The drawing engine
     */
    void PaintPromptText(IRender* pRender);

    /** Forward mouse messages to the control implementation
    */
    void OnMouseMessage(uint32_t uMsg, const EventArgs& msg);

    /** Get the clipboard string
    */
    static void GetClipboardText(DStringW& out);

private:
    //A set of functions used by RichEditHost
    friend class RichEditHost;

    /** Notify message handling
    */
    void OnTxNotify(DWORD iNotify, void* pv);

    /** Get the associated window handle
    */
    HWND GetWindowHWND() const;

    /** Get the drawing device context
    */
    HDC GetDrawDC() const;

    /** Get the rectangle range of the text area
    */
    UiSize GetNaturalSize(LONG width, LONG height);

    /** Set a timer (used by the internal callback)
    */
    void SetTimer(UINT idTimer, UINT uTimeout);

    /** Cancel a timer (used by the internal callback)
    */
    void KillTimer(UINT idTimer);

    /** Get the current zoom ratio, displayed as zoom numerator/denominator: 1/64 < (wParam / lParam) < 64
     * @param[out] nNum The numerator of the zoom ratio
     * @param[out] nDen The denominator of the zoom ratio
     * @return Returns TRUE if the message was handled
     */
    bool GetZoom(int& nNum, int& nDen) const;

    /** Set the zoom ratio
     * @param[in] nNum The numerator of the zoom ratio, value range: [0, 64]
     * @param[in] nDen The denominator of the zoom ratio, value range: [0, 64]
     * @return Returns true on success, false on failure
     */
    bool SetZoom(int nNum, int nDen);

    /** Restore the zoom to the initial state
     * @return Returns true on success, otherwise false
     */
    bool SetZoomOff();

    /** Select all when gaining focus
    */
    void CheckSelAllOnFocus();

    /** Set the row spacing multiplier
    */
    void DoSetRowSpacingMul(float fRowSpacingMul);

private:
    //RichEdit control helper utility class
    RichEditCtrl m_richCtrl;

    //RichEdit Host class
    RichEditHost* m_pRichHost;

    float m_fRowSpacingMul;     //Row spacing multiplier
    bool m_bVScrollBarFixing;   //Scrollbar fixing flag
    bool m_bWantTab;            //Whether to accept the TAB key; if true, the TAB key is treated as text input, otherwise the TAB key is filtered out
    bool m_bWantReturn;         //Whether to accept the Enter key; if true, the Enter key is treated as text input, otherwise the Enter key is filtered out
    bool m_bWantCtrlReturn;     //Whether to accept Ctrl + Enter; if true, the Enter key is treated as text input, otherwise the Enter key is filtered out
     
    bool m_bSelAllEver;         //Only select all on the first mouse up after gaining focus

    bool m_bNoSelOnKillFocus;   //When losing focus, cancel the text selection (for m_bEnabled && IsReadOnly())
    bool m_bSelAllOnFocus;      //When gaining focus, select all text (for m_bEnabled && !IsReadOnly())
    bool m_bHideSelection;      //Whether to hide the selection state
    bool m_bContextMenuShown;   //Whether the context menu is being shown

    bool m_bIsComposition;      //Whether the IME composition window is visible

private:
    bool m_bNoCaretReadonly;    //Do not show the caret in read-only mode
    bool m_bIsCaretVisiable;    //Whether the caret is visible
    int32_t m_iCaretPosX;       //Caret X coordinate
    int32_t m_iCaretPosY;       //Caret Y coordinate
    int32_t m_iCaretWidth;      //Caret width
    int32_t m_iCaretHeight;     //Caret height
    UiString m_sCaretColor;     //Caret color

    WeakCallbackFlag m_drawCaretFlag;   //Lifetime of the caret drawing timer
    std::map<UINT, WeakCallbackFlag> m_timerFlagMap; //Internal timers

private:
    UiString m_sFontId;                 //Font ID
    UiString m_sTextColor;              //Normal text color
    UiString m_sDisabledTextColor;      //Disabled state text color
    UiPadding16 m_rcTextPadding;        //Text padding

    UiString m_sFocusBottomBorderColor; //Color of the bottom border in the focused state
    
    UiString m_sPromptColor;            //Prompt text color
    UiString m_sPromptText;             //Prompt text content (only shown when the edit box is empty)
    UiString m_sPromptTextId;           //Prompt text ID
    bool m_bAllowPrompt;                //Whether prompt text is supported

    uint8_t m_nFocusBottomBorderSize;    //Size of the bottom border in the focused state

private:
    /** Whether to use the caret set by the Control
    */
    bool m_bUseControlCursor;

    /** Whether to allow adjusting the zoom ratio via Ctrl + mouse wheel
    */
    bool m_bEnableWheelZoom;

    /** Whether to allow using the default context menu
    */
    bool m_bEnableDefaultContextMenu;

    /** Whether to disable triggering the text change event
    */
    bool m_bDisableTextChangeEvent;

    /** The maximum allowed number (only valid when IsNumberOnly() is true)
    */
    int32_t m_maxNumber;

    /** The minimum allowed number (only valid when IsNumberOnly() is true)
    */
    int32_t m_minNumber;

    /** The number format
    */
    UiString m_numberFormat;

    /** The list of allowed characters
    */
    std::unique_ptr<DStringW::value_type[]> m_pLimitChars;

    /** Spin function container
    */
    VBox* m_pSpinBox;

    /** Lifetime management of the timer for automatically adjusting the text number value
    */
    WeakCallbackFlag m_flagAdjustTextNumber;

    /** The image shown when gaining focus
    */
    Image* m_pFocusedImage;

    /** The clear button (only valid in non-read-only mode)
    */
    Control* m_pClearButton;

    /** The show/hide password button (only valid in password mode)
    */
    Control* m_pShowPasswordButton;

private:
    /** Data structure required for drawing
    */
    struct TxDrawData
    {
    public:
        /** DC required for drawing
        */
        HDC m_hDrawDC;

        /** DC original bitmap data
        */
        HGDIOBJ m_hOldBitmap;

        /** The bitmap resource
        */
        HBITMAP m_hBitmap;

        /** The size of the bitmap
        */
        UiSize m_szBitmap;

        /** The data pointer of the bitmap
        */
        LPVOID m_pBitmapBits ;

    public:
        TxDrawData();
        ~TxDrawData();

        /** Clean up resources
        */
        void Clear();

        /** Check and rebuild the bitmap
        */
        bool CheckCreateBitmap(HDC hWindowDC, int32_t nWidth, int32_t nHeight);
    };

    /** Data required for drawing
    */
    TxDrawData m_txDrawData;

    /** Alpha channel cache for drawing
    */
    class FastBytes;
    std::unique_ptr<FastBytes> m_pAlphaValues;

private:
#ifndef DUI_UNICODE
    /** MBCS: input characters
    */
    std::vector<BYTE> m_pendingChars;

    /** The time of the last input
    */
    DWORD m_dwLastCharTime = 0;
#endif

    /** The implementation interface of the drag and drop feature; if not null, the feature is enabled
    */
    ControlDropTarget_Windows* m_pControlDropTarget;
};

} // namespace ui

#endif // DUI_BUILD_FOR_WIN

#endif // UI_CONTROL_RICHEDIT_WINDOWS_H_

