#ifndef UI_CONTROL_RICHEDIT_SDL_H_
#define UI_CONTROL_RICHEDIT_SDL_H_

#include "duilib/Box/ScrollBox.h"
#include "duilib/Image/Image.h"
#include "duilib/Control/RichEditData.h"

#if defined(DUILIB_BUILD_FOR_SDL) || defined(DUILIB_BUILD_FOR_WAYLAND)

namespace ui 
{

class VBox;
class DrawRichTextCache;

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

class DUILIB_API RichEdit : public ScrollBox, protected IRichTextData
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
    virtual void HandleEvent(const EventArgs& msg) override; 
    virtual void SetWindow(Window* pWindow) override;
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;
    virtual void PaintStateImages(IRender* pRender) override;
    virtual void ClearImageCache() override;
    virtual UiSize EstimateText(UiSize szAvailable) override;
    virtual UiSize64 CalcRequiredSize(const UiRect& rc, bool bEstimateOnly) override;
    virtual void OnScrollOffsetChanged(const UiSize& oldScrollOffset, const UiSize& newScrollOffset) override;

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
    DStringA GetTextA() const;
    DStringW GetTextW() const;

    /** Get the length of the content (number of characters in UTF16 encoding, TODO: ambiguous)
     * @return Returns the content length
     */
    int32_t GetTextLength() const;

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

    /** Whether it is empty
    */
    bool IsEmpty() const;

    /** Set the font ID
     * @param[in] index The font ID to set (corresponding to the font ID in global.xml)
     */
    void SetFontId(const DString& strFontId);

    /** Get the currently set font ID (the font ID set via SetFontId)
     * @return Returns the font ID (corresponding to the font ID in global.xml)
     */
    DString GetFontId() const;

    /** Get the font information (the font size has been DPI scaled)
    */
    UiFont GetFontInfo() const;

    /** Set the font information (high priority, overrides the font set via SetFontId)
    * @param [in] fontInfo The font information; the font size has been DPI scaled
    */
    bool SetFontInfo(const UiFont& fontInfo);

    /** Get the currently used font ID
    * @return If the font has been set via the SetFontInfo function, returns the internal font ID; if SetFontInfo has not been called to set the font, returns the font ID set via SetFontId
    */
    DString GetCurrentFontId() const;

    /** Set the normal text color
     * @param[in] dwTextColor The text color to set
     */
    void SetTextColor(const DString& dwTextColor);

    /** Get the normal text color
     */
    DString GetTextColor() const;

    /** Get the color of the selected text (not supported)
    */
    DString GetSelectionTextColor() const;

    /** Set the color of the selected text (not supported)
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

    /** Set the background color of the selected text (focused state)
    */
    void SetSelectionBkColor(const DString& selectionBkColor);

    /** Get the background color of the selected text (focused state)
    */
    DString GetSelectionBkColor() const;

    /** Set the background color of the selected text (non-focused state)
    */
    void SetInactiveSelectionBkColor(const DString& selectionBkColor);

    /** Get the background color of the selected text (non-focused state)
    */
    DString GetInactiveSelectionBkColor() const;

    /** Set the background color of the current row (the row of the caret, focused state)
    */
    void SetCurrentRowBkColor(const DString& currentRowBkColor);

    /** Get the background color of the current row (focused state)
    */
    DString GetCurrentRowBkColor() const;

    /** Set the background color of the current row (the row of the caret, non-focused state)
    */
    void SetInactiveCurrentRowBkColor(const DString& currentRowBkColor);

    /** Get the background color of the current row (non-focused state)
    */
    DString GetInactiveCurrentRowBkColor() const;

    /** Get the row spacing multiplier
    */
    float GetRowSpacingMul() const;

    /** Row spacing multiplier: the ratio of the font size (the default is usually 1.0, i.e. 100% of the font size), used to adjust the row spacing proportionally
    */
    void SetRowSpacingMul(float fRowSpacingMul);

    /** Get the row spacing additive amount
    */
    float GetRowSpacingAdd() const;

    /** Row spacing additive amount: a fixed additional pixel value (the default is usually 0), used to add a fixed offset (pixels) on top of the proportional adjustment
    */
    void SetRowSpacingAdd(float fRowSpacingAdd);

public:
    /** Set whether the prompt text is allowed to be shown
     * @param[in] bPrompt Set to true to show, false to hide
     */
    void SetPromptMode(bool bPrompt);

    /** Get whether the prompt text is allowed to be shown
    */
    bool AllowPromptMode() const;

    /** Get the prompt text
     */
    DString GetPromptText() const;

    /** Set the prompt text
     * @param[in] strText The prompt text to set
     */
    void SetPromptText(const DString& strText);

    /** Set the prompt text ID
     * @param[in] strText The prompt text ID to set, which must exist in the loaded language file
     */
    void SetPromptTextId(const DString& strTextId);

    /** Set the color of the prompt text
    */
    void SetPromptTextColor(const DString& promptColor);

    /** Get the color of the prompt text
    */
    DString GetPromptTextColor() const;

public:
    /** Set the text padding information
     * @param[in] padding The padding information
     * @param[in] bNeedDpiScale Whether DPI scaling is supported
     */
    void SetTextPadding(UiPadding padding, bool bNeedDpiScale);

    /** Get the text padding
     */
    UiPadding GetTextPadding() const;

    /** Whether it is multi-line text (multi-line is supported by default, not single-line text mode)
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

    /** Get the password character
    */
    DStringW::value_type GetPasswordChar() const;

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

    /** Get the number format (the format of a 64-bit signed integer, e.g. "%I64d")
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

    /** Set the zoom percentage
    * @param [in] fZoomRatio The zoom ratio, e.g. "100" means 100%, no zoom; "200" means a zoom ratio of 200%
    */
    void SetZoomPercent(uint32_t nZoomPercent);

    /** Get the zoom percentage
    */
    uint32_t GetZoomPercent() const;

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

    /** Set the text horizontal alignment
    */
    void SetTextHAlignType(HorAlignType alignType);

    /** Get the text horizontal alignment
    */
    HorAlignType GetHAlignType() const;

    /** Set the text vertical alignment
    */
    void SetTextVAlignType(VerAlignType alignType);

    /** Get the text vertical alignment
    */
    VerAlignType GetVAlignType() const;

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

public:
    /** Listen for the Enter key press event
     * @param [in] callback The custom callback function for when Enter is pressed
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     */
    void AttachReturn(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventReturn, callback, callbackID); }

    /** Listen for the ESC key press event
     * @param [in] callback The custom callback function for when Enter is pressed
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     */
    void AttachEsc(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventEsc, callback, callbackID); }

    /** Listen for the TAB key press event
     * @param [in] callback TAB pressed custom callback function
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     */
    void AttachTab(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventTab, callback, callbackID); }

    /* Listen for the zoom ratio change event
     * @param [in] callback The custom callback function for after the text is modified
     */
    void AttachZoom(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventZoom, callback, callbackID); }

    /* Listen for the text modified event
     * @param [in] callback The custom callback function for after the text is modified
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     */
    void AttachTextChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventTextChanged, callback, callbackID); }

    /* Listen for the text selection change event
     * @param [in] callback The custom callback function for after the text selection changes
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     */
    void AttachSelChanged(const EventCallback& callback, EventCallbackID callbackID = 0);

    /** Listen for the hyperlink click event
     * @param [in] callback The callback function for after the hyperlink is clicked
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
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
     * @param[in] nStartChar Returns the start character position of the selected text, the index value starting from 0 in the string; if no text is selected, returns -1
     * @param[in] nEndChar Returns the index of the character after the last character of the selected text; if no text is selected, returns 0
     */
    void GetSel(int32_t& nStartChar, int32_t& nEndChar) const;

    /** Select a portion of the content
     * @param[in] nStartChar The start position to select
     * @param[in] nEndChar The end position to select
     * @return Returns the number of selected characters
     */
    int32_t SetSel(int32_t nStartChar, int32_t nEndChar);

    /** Replace the selected content
     * @param [in] newText The target text to replace with
     * @param [in] bCanUndo Whether it can be undone, true for yes, otherwise false
     */
    bool ReplaceSel(const DString& newText, bool bCanUndo);

    /** Get the selected text content
     * @return Returns the selected text content
     */
    DString GetSelText() const;

    /** Whether there is currently selected text
    */
    bool HasSelText() const;

    /** Get the content of the specified range
     * @param[in] nStartChar The start position
     * @param[in] nEndChar The end position
     * @return Returns the content of the specified position
     */
    DString GetTextRange(int32_t nStartChar, int32_t nEndChar) const;

    /** Set whether to hide or show the selected text (whether to hide the selection when the control is inactive)
     * @param [in] bHideSelection Whether to show, true to hide, false to show
     */
    void SetHideSelection(bool bHideSelection);
    void HideSelection(bool bHideSelection);

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
    int32_t CharFromPos(UiPoint pt);

    /** Clear the undo list
     */
    void EmptyUndoBuffer();

    /** Set the number of items the undo list can hold
     * @param [in] nLimit
     */
    void SetUndoLimit(uint32_t nLimit);

    /** Ensure the character is in the visible range
    * @param [in] nCharIndex The position of the character
    */
    void EnsureCharVisible(int32_t nCharIndex);

    /** Find text
    * @param [in] findParam The search parameters
    * @param [out] chrgText The matched text, the index range of the characters
    */
    bool FindRichText(const FindTextParam& findParam, TextCharRange& chrgText) const;

    /** Whether it is rich text mode
     * @return Always returns false; plain text mode, rich text mode is not supported
     */
    bool IsRichText() const;

    /** Get the text content and convert it to a number
    */
    int64_t GetTextNumber() const;

    /** Convert a number to text and set the text content
    */
    void SetTextNumber(int64_t nValue);

    /** Adjust the text number value
    */
    void AdjustTextNumber(int32_t nDelta);

public:
    /** Move up one line
     * @param[in] deltaValue The scroll distance, defaults to DUI_NOSET_VALUE
     */
    virtual void LineUp(int32_t deltaValue = DUI_NOSET_VALUE) override;

    /** Move down one line
     * @param[in] deltaValue The scroll distance, defaults to DUI_NOSET_VALUE
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
     * @param[in] arrange Whether to reset the scrollbar position, defaults to true
     */
    virtual void EndDown(bool arrange = true) override;

    /** Scroll horizontally left
     * @param[in] deltaValue The scroll distance, defaults to DUI_NOSET_VALUE
     */
    virtual void LineLeft(int32_t deltaValue = DUI_NOSET_VALUE) override;

    /** Scroll horizontally right
     * @param[in] deltaValue The scroll distance, defaults to DUI_NOSET_VALUE
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

protected:

    //Some virtual functions of the base class
    virtual bool CanPlaceCaptionBar() const override;
    virtual void OnInit() override;
    virtual uint32_t GetControlFlags() const override;

    //Message handler functions
    virtual bool OnSetCursor(const EventArgs& msg) override;
    virtual bool OnSetFocus(const EventArgs& msg) override;
    virtual bool OnKillFocus(const EventArgs& msg) override;
    virtual bool OnImeStartComposition(const EventArgs& msg) override;
    virtual bool OnImeEndComposition(const EventArgs& msg) override;
    
    virtual bool OnKeyDown(const EventArgs& msg) override;
    virtual bool OnKeyUp(const EventArgs& msg) override;
    virtual bool OnChar(const EventArgs& msg) override;

    //Mouse messages (return true: the message has been handled; return false: the message has not been handled and needs to be forwarded to the parent control)
    virtual bool ButtonDown(const EventArgs& msg) override;
    virtual bool ButtonUp(const EventArgs& msg) override;
    virtual bool ButtonDoubleClick(const EventArgs& msg) override;
    virtual bool RButtonDown(const EventArgs& msg) override;
    virtual bool RButtonUp(const EventArgs& msg) override;
    virtual bool MouseMove(const EventArgs& msg) override;
    virtual bool MouseWheel(const EventArgs& msg) override;
    virtual bool OnWindowKillFocus(const EventArgs& msg) override;//The window that owns the control lost focus

    //Drawing related functions
    virtual void Paint(IRender* pRender, const UiRect& rcPaint) override;
    virtual void PaintChild(IRender* pRender, const UiRect& rcPaint) override;
    virtual void PaintBorder(IRender* pRender) override;

    /** Convert the text into a drawable format
    * @param [in] textView The text view organized and split by lines, one item per line (lines split by '\n')
    * @param [out] richTextDataList Returns the formatted text; the number of items in the returned container should be the same as the passed-in textView; if this line is not updated, empty data can be filled: RichTextData()
    * @param [in] nStartLine The starting line number for recalculation (used for incremental calculation)
    * @param [in] modifiedLines The line numbers that were modified (used for incremental calculation)
    */
    virtual bool GetRichTextForDraw(const std::vector<std::wstring_view>& textView,
                                    std::vector<RichTextData>& richTextDataList,
                                    size_t nStartLine = (size_t)-1,
                                    const std::vector<size_t>& modifiedLines = std::vector<size_t>()) const override;

    /** Get the text drawing rectangle range (call this interface anytime to get the rectangle range of the drawn text when needed)
    * @return Returns the rectangle range of the current text drawing; this range excludes the padding and the space occupied by the scrollbar
    */
    virtual UiRect GetRichTextDrawRect() const override;

    /** Get the drawing alpha
    */
    virtual uint8_t GetDrawAlpha() const override;

    /** Event that the text area has been recalculated
    */
    virtual void OnTextRectsChanged() override;

    /** Get the row height value
    */
    virtual int32_t GetTextRowHeight() const override;

    /** Get the caret width
    */
    virtual int32_t GetTextCaretWidth() const override;

    /** Whether it is currently in password mode
    */
    virtual bool IsTextPasswordMode() const override;

    /** Handle the display characters in password mode
    */
    virtual void ReplacePasswordChar(DStringW& text) const override;

    /** Get the text limit length
    */
    virtual int32_t GetTextLimitLength() const override;

    /** Set the enabled state event
    * @param [in] bChanged true means the state has changed, false means the state has not changed
    */
    virtual void OnSetEnabled(bool bChanged) override;

private:
    void OnLButtonDown(const UiPoint& ptMouse, Control* pSender, bool bShiftDown);
    void OnLButtonUp(const UiPoint& ptMouse, Control* pSender);
    void OnLButtonDoubleClick(const UiPoint& ptMouse, Control* pSender);
    void OnRButtonDown(const UiPoint& ptMouse, Control* pSender);
    void OnRButtonUp(const UiPoint& ptMouse, Control* pSender);
    void OnMouseMove(const UiPoint& ptMouse, Control* pSender);
    void OnMouseWheel(int32_t wheelDelta, bool bCtrlDown);
    void OnWindowKillFocus();

private:
    /** Show the menu on the RichEdit
    * @param [in] point The coordinates in the client area
    */
    void ShowPopupMenu(const ui::UiPoint& point);

    /** Determine whether a character is in the list of restricted characters
    */
    bool IsInLimitChars(DStringW::value_type charValue) const;

    /** Determine whether the paste operation is allowed (determine whether there are character restrictions or number restrictions)
    * @return If true is returned, the paste operation is not allowed
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

    /** Get the font interface
    */
    IFont* GetIFontInternal(const DString& fontId) const;

    /** Get the internal font ID of this control
    */
    DString GetInternalFontId() const;

    /** Draw the caret
     * @param[in] pRender The drawing engine
     * @param[in] rcPaint The drawing area
     */
    void PaintCaret(IRender* pRender, const UiRect& rcPaint);

    /** Draw the background color of the current editing row
    */
    void PaintCurrentRowBkColor(IRender* pRender, const UiRect& rcPaint);

    /** Draw the selection background color
    */
    void PaintSelectionColor(IRender* pRender, const UiRect& rcPaint);

    /** Toggle whether the caret is shown
    */
    void ChangeCaretVisiable();

    /** Draw the prompt text
     * @param[in] pRender The drawing engine
     */
    void PaintPromptText(IRender* pRender);

    /** Stop the password character flashing
    */
    void StopFlashPasswordChar();

    /** Get the next zoom percentage value
    * @param [in] nOldZoomPercent The current zoom percentage
    * @param [in] bZoomIn true means zoom in, false means zoom out
    */
    uint32_t GetNextZoomPercent(uint32_t nOldZoomPercent, bool bZoomIn) const;

    /** Zoom percentage changed
    */
    void OnZoomPercentChanged(uint32_t nOldZoomPercent, uint32_t nNewZoomPercent);

    /** Font changed
    */
    void OnFontChanged(const DString& fontId);

private:

#if defined (DUILIB_BUILD_FOR_WIN) && !defined (DUILIB_BUILD_FOR_SDL)
    /** Get the associated window handle
    */
    HWND GetWindowHWND() const;
#endif

private:
    /** Adjust the caret position (by point coordinates)
    * @param [in] pt The position to set (client-area coordinates)
    */
    void SetCaretPos(const UiPoint& pt);

    /** Adjust the caret position (by character position)
    */
    void SetCaretPos(int32_t nCharPosIndex);

    /** Set the caret position
     * @param [in] xPos X coordinate
     * @param [in] yPos Y coordinate
     */
    void SetCaretPosInternal(int32_t xPos, int32_t yPos);

    /** Get the attributes of the currently drawn text
    */
    uint16_t GetTextStyle() const;

    /** Convert the text into a drawable format
    */
    bool GetRichTextForDraw(std::vector<RichTextData>& richTextDataList) const;

    /** Get the text drawing area
    * @param [in] rc The rectangle area of the current control
    * @return Returns the text drawing area (minus the padding, minus the width and height occupied by the scrollbar)
    */
    UiRect GetTextDrawRect(const UiRect& rc) const;

    /** Redraw (but do not recalculate the layout)
    */
    void Redraw();

    /** Clear the drawing cache and redraw (call this when layout changes caused by row spacing, fonts, etc.)
    */
    void ClearCacheAndRedraw();

    /** Check whether the view needs to be scrolled
    */
    void OnCheckScrollView();

    /** Mouse frame selection operation performed (the coordinates include the scrollPos value)
    * @param [in] ptMouseDown64 The start point where the mouse was pressed
    * @param [in] ptMouseMove64 The end point where the mouse moved
    */
    void OnFrameSelection(UiSize64 ptMouseDown64, UiSize64 ptMouseMove64);

    /** Insert a character at the current caret position (text input mode)
    */
    void OnInputChar(const EventArgs& msg);

    /** Check the flags of the Shift and Ctrl keys
    */
    void CheckKeyDownStartIndex(const EventArgs& msg);

    /** Handle the keyboard shortcuts of the arrow keys for scrolling the view (Ctrl + arrow keys)
    */
    bool OnCtrlArrowKeyDownScrollView(const EventArgs& msg);

    /** Get the distance of scrolling one line vertically, aligned by line
    */
    int32_t GetLineScrollDeltaValue(bool bLineDown) const;

    /** Get the distance of scrolling one page vertically, aligned by page
    */
    int32_t GetPageScrollDeltaValue(bool bPageDown) const;

    /** Keyboard shortcut handling for arrow keys
    */
    bool OnArrowKeyDown(const EventArgs& msg);

    /** Select a portion of the content (internal function)
     * @param[in] nStartChar The start position to select
     * @param[in] nEndChar The end position to select
     * @return Returns the start index of the selected text
     */
    int32_t InternalSetSel(int32_t nStartChar, int32_t nEndChar);

    /** Remove unsupported password characters
    */
    bool RemoveInvalidPasswordChar(DStringA& text);
    bool RemoveInvalidPasswordChar(DStringW& text);

    /** Update the scrollbar range
    */
    void UpdateScrollRange();

    /** Select all when gaining focus
    */
    void CheckSelAllOnFocus();

private:
    bool m_bWantTab;            //Whether to accept the TAB key; if true, the TAB key is treated as text input, otherwise the TAB key is filtered out
    bool m_bWantReturn;         //Whether to accept the Enter key; if true, the Enter key is treated as text input, otherwise the Enter key is filtered out
    bool m_bWantCtrlReturn;     //Whether to accept Ctrl + Enter; if true, the Enter key is treated as text input, otherwise the Enter key is filtered out
     
    bool m_bSelAllEver;         //Only select all on the first mouse up after gaining focus

    bool m_bNoSelOnKillFocus;   //When losing focus, cancel the text selection (for m_bEnabled && IsReadOnly())
    bool m_bSelAllOnFocus;      //When gaining focus, select all text (for m_bEnabled && !IsReadOnly())

#if defined (DUILIB_BUILD_FOR_WIN) && !defined (DUILIB_BUILD_FOR_SDL)
    bool m_bIsComposition;      //Whether the IME composition window is visible
#endif

    bool m_bReadOnly;           //Whether it is read-only mode
    bool m_bPasswordMode;       //Whether it is password mode
    bool m_bShowPassword;       //Whether to show the password
    DStringW::value_type m_chPasswordChar;   //The password character
    bool m_bFlashPasswordChar;  //Whether to briefly show the password character and then hide it
    bool m_bInputPasswordChar;  //Whether there is currently password input

    bool m_bNumberOnly;         //Whether only numbers are allowed
    bool m_bWordWrap;           //Whether to automatically wrap when the display exceeds the boundary

    int32_t m_nLimitText;       //The maximum number of text characters (a limit exists only when it is positive)
    bool m_bModified;           //Whether the text content has been modified

private:
    bool m_bNoCaretReadonly;    //Do not show the caret in read-only mode
    bool m_bIsCaretVisiable;    //Whether the caret is visible
    int32_t m_iCaretPosX;       //Caret X coordinate
    int32_t m_iCaretPosY;       //Caret Y coordinate
    int32_t m_iCaretWidth;      //Caret width
    int32_t m_iCaretHeight;     //Caret height
    UiString m_sCaretColor;     //Caret color

    int32_t m_nRowHeight;       //Row height (logical line), related to the font

    WeakCallbackFlag m_drawCaretFlag;   //Lifetime of the caret drawing timer

private:
    UiString m_sFontId;                 //Font ID
    UiString m_sTextColor;              //Normal text color
    UiString m_sDisabledTextColor;      //Disabled state text color
    UiPadding16 m_rcTextPadding;        //Text padding

    UiString m_sFocusBottomBorderColor; //Color of the bottom border in the focused state

    UiString m_sCurrentRowBkColor;         //Background color of the current row (the row of the caret, focused state)
    UiString m_sInactiveCurrentRowBkColor; //Background color of the current row (the row of the caret, non-focused state)
    UiString m_sSelectionBkColor;          //Background color of the selected text (focused state)
    UiString m_sInactiveSelectionBkColor;  //Background color of the selected text (non-focused state)

    UiString m_sPromptColor;            //Prompt text color
    UiString m_sPromptText;             //Prompt text content (only shown when the edit box is empty)
    UiString m_sPromptTextId;           //Prompt text ID
    bool m_bAllowPrompt;                //Whether prompt text is supported

    uint8_t m_nFocusBottomBorderSize;   //Size of the bottom border in the focused state
    float m_fRowSpacingMul;             //Row spacing multiplier
    float m_fRowSpacingAdd;             //Row spacing additive amount

private:
    /** Whether to use the caret set by the Control
    */
    bool m_bUseControlCursor;

    /** The zoom percentage, 100 means 100%
    */
    uint16_t m_nZoomPercent;

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

    /** The list of allowed characters
    */
    std::unique_ptr<DStringW::value_type[]> m_pLimitChars;

    /** The number format
    */
    UiString m_numberFormat;

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

    /** Whether to hide the selection when the control is inactive (when shown: the background color of the selected text differs from normal text)
    */
    bool m_bHideSelection;

    /** Whether the current control is in the active state
    */
    bool m_bActive;

    /** Whether it is currently in text input mode
    */
    bool m_bTextInputMode;

private:
    /** Text content management interface
    */
    RichEditData* m_pTextData;

private:
    /** The start character of the selection
    */
    int32_t m_nSelStartIndex;

    /** The end character of the selection
    */
    int32_t m_nSelEndCharIndex;

    /** The selection start character when the Shift key is held down
    */
    int32_t m_nShiftStartIndex;

    /** The selection start character when the Ctrl key is held down
    */
    int32_t m_nCtrlStartIndex;

    /** The current selection operation direction (forward, backward)
    */
    bool m_bSelForward;

    /** The X coordinate value when the position is switched with the arrow keys
    */
    int32_t m_nSelXPos;

private:
    /** Whether the left or right mouse button was pressed in the view
    */
    bool m_bMouseDownInView;

    /** Whether the left mouse button is pressed
    */
    bool m_bMouseDown;

    /** Whether the right mouse button is pressed
    */
    bool m_bRMouseDown;

    /** Whether it is in a mouse drag operation
    */
    bool m_bInMouseMove;

    /** The mouse position when the mouse was pressed
    */
    UiSize64 m_ptMouseDown;

    /** The mouse position while the mouse is moving
    */
    UiSize64 m_ptMouseMove;

    /** The control interface at the time the mouse was pressed
    */
    Control* m_pMouseSender;

    /** Cancellation mechanism for the timer that scrolls the view
    */
    WeakCallbackFlag m_scrollViewFlag;

    /** Cancellation mechanism for the timer of the password character flashing function
    */
    WeakCallbackFlag m_falshPasswordFlag;
};

} // namespace ui

#endif // DUILIB_BUILD_FOR_SDL

#endif // UI_CONTROL_RICHEDIT_SDL_H_

