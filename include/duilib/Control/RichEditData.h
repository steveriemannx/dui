#ifndef UI_CONTROL_RICHEDIT_DATA_H_
#define UI_CONTROL_RICHEDIT_DATA_H_

#include "duilib/Core/UiTypes.h"
#include "duilib/Core/SharePtr.h"
#include "duilib/Render/IRender.h"
#include <unordered_map>
#include <map>
#include <list>

namespace ui
{
/** Interface for generating formatted text, used for drawing text
*/
class DUILIB_API IRichTextData
{
public:
    /** Generate a drawable format from the text
    * @param [in] textView The text view split and organized by lines, one entry per line (lines split by '\n')
    * @param [out] richTextDataList Returns the formatted text; the number of returned containers should be the same as the passed-in textView; if a line is not updated, it can be filled with empty data: RichTextData()
    * @param [in] nStartLine The starting line number for recalculation (used for incremental calculation)
    * @param [in] modifiedLines The line numbers that have been modified (used for incremental calculation)
    */
    virtual bool GetRichTextForDraw(const std::vector<std::wstring_view>& textView,
                                    std::vector<RichTextData>& richTextDataList,
                                    size_t nStartLine = (size_t)-1,
                                    const std::vector<size_t>& modifiedLines = std::vector<size_t>()) const = 0;

    /** Get the rectangle range of the drawn text (call this interface anytime to get the rectangle range of the drawn text)
    * @return Returns the rectangle range of the currently drawn text; this range excludes the padding and the space occupied by scrollbars
    */
    virtual UiRect GetRichTextDrawRect() const = 0;

    /** Get the drawing alpha
    */
    virtual uint8_t GetDrawAlpha() const = 0;

    /** Event that the text region has been recalculated
    */
    virtual void OnTextRectsChanged() = 0;

    /** Get the row height value
    */
    virtual int32_t GetTextRowHeight() const = 0;

    /** Get the caret width
    */
    virtual int32_t GetTextCaretWidth() const = 0;

    /** Whether it is currently in password mode
    */
    virtual bool IsTextPasswordMode() const = 0;

    /** Process the displayed characters in password mode
    */
    virtual void ReplacePasswordChar(DStringW& text) const = 0;

    /** Get the text limit length
    */
    virtual int32_t GetTextLimitLength() const = 0;
};

class DUILIB_API RichEditData
{
public:
    explicit RichEditData(IRichTextData* pRichTextData);
    RichEditData(const RichEditData&) = delete;
    RichEditData& operator=(const RichEditData&) = delete;
    virtual ~RichEditData();

public:
    /** Set the associated rendering interface
    */
    void SetRender(IRender* pRender);

    /** Set the associated rendering interface factory
    */
    void SetRenderFactory(IRenderFactory* pRenderFactory);

    /** Set single-line text mode; when drawing, do not break lines and ignore newline characters ('\n')
    */
    void SetSingleLineMode(bool bSingleLineMode);

    /** Whether it is single-line text mode
    */
    bool IsSingleLineMode() const;

    /** Set the text horizontal alignment (applied to the whole text)
    */
    void SetTextHAlignType(HorAlignType hAlignType);

    /** Get the text horizontal alignment
    */
    HorAlignType GetHAlignType() const;

    /** Set the text vertical alignment (applied to the whole text)
    */
    void SetTextVAlignType(VerAlignType vAlignType);

    /** Get the text vertical alignment
    */
    VerAlignType GetVAlignType() const;

    /** The scrollbar position of the text
    */
    void SetScrollOffset(const UiSize& szScrollOffset);

    /** Calculate the rectangle range needed to display the text (used to estimate the control size)
    * @param [int] rcAvailable The available rectangle range
    */
    UiRect EstimateTextDisplayBounds(const UiRect& rcAvailable);

public:
    /** Set the text and clear the Undo/Redo history
     * @param [in] text The text content
     * @return Returns true if the text has changed, returns false if the text has not changed
     */
    bool SetText(const DStringW& text);

    /** Replace the text in the specified range (adding, modifying, and deleting text are all done through this function)
     *  (1) If nStartChar == nEndChar, it means inserting text at this position
     *  (2) If nEndChar > nStartChar, it means replacing text
     *  (3) If nStartChar < nEndChar, the parameters are wrong
     * @param [in] text The text content
     * @param [in] nStartChar The starting index value
     * @param [in] nEndChar The ending index value
     * @param [in] bCanUndo Whether it can be undone; true means yes, otherwise false
     * @param [in] bClearRedo Whether to clear the Redo list; only takes effect when bCanUndo is false
     * @return Returns true if the text has changed, returns false if the text has not changed
     */
    bool ReplaceText(int32_t nStartChar, int32_t nEndChar, const DStringW& text, bool bCanUndo = true, bool bClearRedo = true);

    /** Get the text
    */
    DStringW GetText() const;

    /** Get the text view; the text view is organized by lines, one entry per line (lines split by '\n')
    */
    void GetTextView(std::vector<std::wstring_view>& textView) const;

    /** Get the length of the content (the number of characters in UTF16 encoding)
     * @return Returns the text content length
     */
    size_t GetTextLength() const;

    /** Whether it is empty
    */
    bool IsEmpty() const;

    /** Get the text in the specified range [nStartChar, nEndChar)
     * @param[in] nStartChar The starting index value
     * @param[in] nEndChar The ending index value
     */
    DStringW GetTextRange(int32_t nStartChar, int32_t nEndChar) const;

    /** Determine whether the specified range contains text content
     * @param[in] nStartChar The starting index value
     * @param[in] nEndChar The ending index value
     */
    bool HasTextRange(int32_t nStartChar, int32_t nEndChar) const;

    /** Set the undo limit count
    */
    void SetUndoLimit(uint32_t nUndoLimit);

    /** Get the undo limit count
    */
    uint32_t GetUndoLimit() const;

    /** Whether undo is possible
    */
    bool CanUndo() const;

    /** Undo operation
     * @param [out] nEndCharIndex Returns the ending character index
     * @return Returns true on success, false on failure
     */
    bool Undo(int32_t& nEndCharIndex);

    /** Whether redo is possible
    */
    bool CanRedo() const;

    /** Redo operation
     * @param [out] nEndCharIndex Returns the ending character index
     * @return Returns true on success, false on failure
     */
    bool Redo(int32_t& nEndCharIndex);

    /** Clear the undo list
    */
    void EmptyUndoBuffer();

    /** Clear
     */
    void Clear();

    /** Find text
    * @param [in] bMatchCase Whether to distinguish case when searching
    * @param [in] bMatchWholeWord Whether to match by word when searching
    * @param [in] bFindDown Whether to search forward; true means searching forward, false means searching backward
    * @param [in] nFindStartChar The starting value of the character search range
    * @param [in] nFindEndChar The ending value of the character search range
    * @param [in] findText The text content to search for
    * @param [out] chrgText The matched text, the index range of the characters
    */
    bool FindRichText(bool bMatchCase, bool bMatchWholeWord, bool bFindDown,
                      int32_t nFindStartChar, int32_t nFindEndChar,
                      const DStringW& findText,
                      int32_t& nFoundStartChar, int32_t& nFoundEndChar) const;

public:
    /** Get the total number of rows
     * @return Returns the total number of rows
     */
    int32_t GetRowCount();

    /** Get one row of data
     * @param[in] nRowIndex The row number
     * @return Returns the fetched row of data
     */
    DStringW GetRowText(int32_t nRowIndex);

    /** Get the first character index of the specified row
     * @param[in] nRowIndex The row number
     * @return Returns the first character index of the specified row; returns -1 on failure
     */
    int32_t RowIndex(int32_t nRowIndex);

    /** Get the data length of the specified row
     * @param[in] nRowIndex The row number
     * @return Returns the data length of the specified row
     */
    int32_t RowLength(int32_t nRowIndex);

    /** Get the row number of the specified character
     * @param[in] nCharIndex The index of the character
     * @return Returns the row number of the current character
     */
    int32_t RowFromChar(int32_t nCharIndex);

public:
    /** Get the caret position of the specified character
    * @param[in] nCharIndex The character index position
    */
    UiPoint CaretPosFromChar(int32_t nCharIndex);

    /** Retrieve the client-area coordinates of the specified character in the edit control.
     * @param[in] nCharIndex The character index position
     * @return The return value contains the client-area coordinates of the character.
     */
    UiPoint PosFromChar(int32_t nCharIndex);

    /** Get information about the character nearest to the specified point in the client area of the edit control
     * @param[in] pt The coordinate information (the caller is responsible for handling the scrollbar offset)
     * @return The return value specifies the zero-based character index of the character nearest to the specified point. If the specified point is beyond the last character in the control, the return value indicates the last character in the edit control.
     */
    int32_t CharFromPos(UiPoint pt);

    /** Get the region of the row containing the specified character (including the region without text)
    * @param[in] nCharIndex The index of the character
    */
    UiRect GetCharRowRect(int32_t nCharIndex);

    /** Get the region of the rows containing the selected text
     * @param [in] nStartChar The starting index value
     * @param [in] nEndChar The ending index value
     * @param [out] rowTextRectFs The rectangle range of each row (logical rows)
     */
    void GetCharRangeRects(int32_t nStartChar, int32_t nEndChar, std::map<int32_t, UiRectF>& rowTextRectFs);

public:
    /** Get the index of the next valid character (newline characters are skipped)
    * @param [in] nCharIndex The index of the character
    * @return Returns the index of the next valid character
    */
    int32_t GetNextValidCharIndex(int32_t nCharIndex);

    /** Get the index of the previous valid character (newline characters are skipped)
    * @param [in] nCharIndex The index of the character
    * @return Returns the index of the previous valid character
    */
    int32_t GetPrevValidCharIndex(int32_t nCharIndex);

    /** Get the index of the next valid word (split by spaces or punctuation marks)
    * @param [in] nCharIndex The index of the character
    * @return Returns the index of the next valid character
    */
    int32_t GetNextValidWordIndex(int32_t nCharIndex);

    /** Get the index of the previous valid word (split by spaces or punctuation marks)
    * @param [in] nCharIndex The index of the character
    * @return Returns the index of the previous valid character
    */
    int32_t GetPrevValidWordIndex(int32_t nCharIndex);

    /** Get the starting and ending indexes of the word near the current position
    * @param [in] nCharIndex The index of the character
    * @param [out] nWordStartIndex The starting index of the word
    * @param [out] nWordEndIndex The ending index of the word
    */
    bool GetCurrentWordIndex(int32_t nCharIndex, int32_t& nWordStartIndex, int32_t& nWordEndIndex);

    /** Get the character index of the beginning of the current row
    * @param [in] nCharIndex The index of the character
    * @return Returns the character index of the beginning of the current row
    */
    int32_t GetRowStartCharIndex(int32_t nCharIndex);

    /** Get the character index of the end of the current row
    * @param [in] nCharIndex The index of the character
    * @return Returns the character index of the end of the current row
    */
    int32_t GetRowEndCharIndex(int32_t nCharIndex);

    /** Get the character width corresponding to the index
    * @param [in] nCharIndex The index of the character
    */
    int32_t GetCharWidthValue(int32_t nCharIndex);

public:
    /** Set the text drawing cache
    */
    void SetDrawRichTextCache(const std::shared_ptr<DrawRichTextCache>& spDrawRichTextCache);

    /** Get the text drawing cache
    */
    const std::shared_ptr<DrawRichTextCache>& GetDrawRichTextCache() const;

    /** Clear the drawing cache
    */
    void ClearDrawRichTextCache();

    /** Mark for recalculation
    */
    void SetCacheDirty(bool bDirty);

    /** Get the rectangle range occupied by the text
    */
    const UiRect& GetTextRect() const;

    /** Get the vertical alignment offset
    */
    int32_t GetTextRectOfssetY() const;

    /** Get the horizontal alignment offset (by logical row)
    */
    const std::vector<int32_t>& GetTextRowXOffset() const;

    /** Check and recalculate the text region as needed
    */
    void CheckCalcTextRects();

    /** Truncate the text according to the character limit
    */
    void TruncateLimitText(DStringW& text, int32_t nLimitLen) const;

private:
    /** Convert internal coordinates to external coordinates
    */
    const UiPoint& ConvertToExternal(UiPoint& pt) const;
    const UiRect& ConvertToExternal(UiRect& rect) const;
    const UiRectF& ConvertToExternal(UiRectF& rect) const;

    /** Convert external coordinates to internal coordinates
    */
    const UiPoint& ConvertToInternal(UiPoint& pt) const;
    const UiRect& ConvertToInternal(UiRect& rect) const;

private:
    /** Set the text drawing region
    */
    void SetTextDrawRect(const UiRect& rcTextDrawRect, bool bCheckDirty);

    /** Split the text into multiple lines according to newline characters ('\n')
    */
    void SplitLines(const std::wstring_view& textView, std::vector<std::wstring_view>& lineTextViewList);

    /** Clear the undo list
    */
    void ClearUndoList();

    /** Record the operation to the undo list
    */
    void AddToUndoList(int32_t nStartChar, const DStringW& newText, const DStringW& oldText);

    /** Calculate the rectangle region occupied by the text from the cache
    */
    void CalcCacheTextRects(UiRect& rcTextRect);

    /** Update the Y coordinate of each row of text according to the alignment
    */
    void UpdateRowTextOffsetY(RichTextLineInfoList& lineTextInfo, int32_t nOffsetY) const;

    /** Update the X coordinate of each row of text according to the alignment
    */
    void UpdateRowTextOffsetX(RichTextLineInfoList& lineTextInfo, HorAlignType hAlignType, std::vector<int32_t>& rowXOffset, bool& bTextRectXOffsetUpdated) const;

    /** Calculate the region information of the text (recalculate everything)
    */
    void CalcTextRects();

    /** Calculate the region information of the text (only recalculate the modified part of the text)
    * @param [in] nStartLine The starting line number for recalculation
    * @param [in] modifiedLines The line numbers that have been modified
    * @param [in] deletedLines The deleted lines
    * @param [in] nDeletedRows How many rows were deleted
    */
    void CalcTextRects(size_t nStartLine,
                       const std::vector<size_t>& modifiedLines,
                       const std::vector<size_t>& deletedLines,
                       size_t nDeletedRows);

    /** Locate the rows and row text offsets that a character range belongs to
    * @param [in] nStartChar The starting index value
    * @param [in] nEndChar The ending index value, nEndChar >= nStartChar
    * @param [out] nStartLine The starting line number, physical row
    * @param [out] nEndLine  The ending line number, physical row, nEndLine >= nStartLine
    * @param [out] nStartCharLineOffset The offset of the starting character in the starting row, valid range [0, row text length)
    * @param [out] nEndCharLineOffset The offset of the ending character in the ending row, valid range [0, row text length)
    */
    bool FindLineTextPos(int32_t nStartChar, int32_t nEndChar,
                         size_t& nStartLine, size_t& nEndLine,
                         size_t& nStartCharLineOffset, size_t& nEndCharLineOffset) const;

    /** Determine whether a character is a separator (space, punctuation mark, etc.)
    */
    bool IsSeperatorChar(DStringW::value_type ch) const;

    /** Get the physical row number and the logical row number within the row of the specified character
    * @param [in] nCharIndex The character index position
    * @param [out] nLineNumber The physical row number
    * @param [out] nLineRowIndex The logical row number in the physical row (numbered from 0 in each row)
    * @param [out] nStartCharRowOffset The character offset in the logical row
    */
    bool GetCharLineRowIndex(int32_t nCharIndex,
                             size_t& nLineNumber,
                             size_t& nLineRowIndex,
                             size_t& nStartCharRowOffset) const;

    /** Get the data of the row containing the specified character
    * @param [in] nCharIndex The character index position
    * @param [out] nStartCharRowOffset The character offset in the logical row
    */
    RichTextRowInfoPtr GetCharRowInfo(int32_t nCharIndex, size_t& nStartCharRowOffset) const;

    /** Get the row where a point is located
    */
    RichTextRowInfoPtr GetRowInfoFromPoint(const UiPoint& pt) const;

    /** Get the data of the first row
    */
    RichTextRowInfoPtr GetFirstRowInfo() const;

    /** Get the data of the last row
    */
    RichTextRowInfoPtr GetLastRowInfo() const;

    /** Get the starting character index of a row of data; returns (size_t)-1 if not found
    */
    size_t GetRowInfoStartIndex(const RichTextRowInfoPtr& spRowInfo) const;

    /** Update the row height data (update after incremental drawing)
    * @param [in] nDrawStartLineIndex The row from which to start processing
    */
    void UpdateRowInfo(size_t nDrawStartLineIndex);

    /** Get the caret position for empty text
    */
    UiPoint PosForEmptyText() const;

    /** Union function suitable for business logic
    */
    void UnionRect(UiRect& rect, const UiRect& r) const;
    void UnionRectF(UiRectF& rect, const UiRectF& r) const;

private:
    /** Interface for generating a drawable format from the text
    */
    IRichTextData* m_pRichText;

    /** Whether it is single-line text mode
    */
    bool m_bSingleLineMode;

    /** The text horizontal alignment
    */
    HorAlignType m_hAlignType;

    /** The text vertical alignment
    */
    VerAlignType m_vAlignType;

    /** The associated rendering interface
    */
    IRender* m_pRender;

    /** The associated rendering interface factory
    */
    IRenderFactory* m_pRenderFactory;

    /** The text drawing region
    */
    UiRect m_rcTextDrawRect;

    /** The region occupied by the text
    */
    UiRect m_rcTextRect;

    /** Whether the vertical offset of the text has been updated (Y coordinate)
    */
    bool m_bTextRectYOffsetUpdated;

    /** Whether the horizontal offset of the text has been updated (X coordinate)
    */
    bool m_bTextRectXOffsetUpdated;

    /** The horizontal offset list of each row (logical rows)
    */
    std::vector<int32_t> m_rowXOffset;

    /** The scrollbar position of the text
    */
    UiSize m_szScrollOffset;

private:
    /** The text data, split by physical rows
    */
    RichTextLineInfoList m_lineTextInfo;

    /** The text drawing cache
    */
    std::shared_ptr<DrawRichTextCache> m_spDrawRichTextCache;

    /** The cache data validity flag
    */
    bool m_bCacheDirty;

private:
    /** Undo data
    */
    struct TUndoData
    {
        int32_t m_nStartChar = -1;
        DStringW m_newText;
        DStringW m_oldText;
    };

    /** Undo data list
    */
    std::list<TUndoData> m_undoList;

    /** Redo data list
    */
    std::list<TUndoData> m_redoList;

    /** The maximum number of redo operations
    */
    uint32_t m_nUndoLimit;
};

} //namespace ui

#endif // UI_CONTROL_RICHEDIT_DATA_H_
