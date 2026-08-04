#ifndef EXAMPLES_FIND_REPLACE_FORM_H_
#define EXAMPLES_FIND_REPLACE_FORM_H_

// dui
#include "dui/dui.h"

class RichEditFindReplace
{
public:
    RichEditFindReplace();

public:
    // Set the associated RichEdit control
    void SetRichEdit(ui::RichEdit* pRichEdit);

public:
    // Find
    bool FindRichText(const DString& findText, bool bFindDown, bool bMatchCase, bool bMatchWholeWord, ui::Window* pWndDialog);

    // Find Next
    bool FindNext();

    // Replace
    bool ReplaceRichText(const DString& findText, const DString& replaceText, bool bFindDown, bool bMatchCase, bool bMatchWholeWord, ui::Window* pWndDialog);
    
    // Replace All
    bool ReplaceAllRichText(const DString& findText, const DString& replaceText, bool bFindDown, bool bMatchCase, bool bMatchWholeWord, ui::Window* pWndDialog);

protected:
    // Not found, callback function
    virtual void OnTextNotFound(const DString& findText);

    // Start replacing all
    virtual void OnReplaceAllCoreBegin();

    // Finished replacing all
    virtual void OnReplaceAllCoreEnd(int replaceCount);

private:
    // Find function implementation
    bool FindTextSimple(const DString& findText, bool bFindDown, bool bMatchCase, bool bMatchWholeWord);

    // Find and select
    bool FindAndSelect(const ui::FindTextParam& findParam);

    // Compare whether the current selection matches the text
    bool SameAsSelected(const DString& replaceText, bool bMatchCase);

    // Not found, callback function
    void TextNotFound(const DString& findText);

    // Adjust the position of the find/replace window
    void AdjustDialogPosition(ui::Window* pWndDialog);

private:
    // RichEdit control interface
    ui::RichEdit* m_pRichEdit;

    // Find text
    DString m_sFindNext;

    // Replace text
    DString m_sReplaceWith;

    // Search direction options
    bool m_bFindDown;

    // Whether to match case
    bool m_bMatchCase;

    // Whether to match whole words
    bool m_bMatchWholeWord;

private:
    // Find state: whether it is the first search
    bool m_bFirstSearch;

    // Initial search position
    int32_t m_nInitialSearchPos;

    // Old cursor
    ui::CursorID m_nOldCursor;
};

#endif //EXAMPLES_FIND_REPLACE_FORM_H_
