#ifndef EXAMPLES_REPLACE_FORM_H_
#define EXAMPLES_REPLACE_FORM_H_

// duilib
#include "duilib/duilib.h"

class MainForm;
class ReplaceForm : public ui::WindowImplBase
{
public:
    explicit ReplaceForm(MainForm* pMainForm);
    virtual ~ReplaceForm() override;

    /** Resource-related interface
     * The GetSkinFolder interface sets the skin resource path of the window you are drawing
     * The GetSkinFile interface sets the xml description file of the window you are drawing
     */
    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;

    /** Called after the window is created, allowing subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

private:
    /** Pure code UI construction (corresponds to the replace.xml layout)
    */
    void BuildUI();

private:
    // Find Next
    void OnFindNext();
    // Replace
    void OnReplace();
    // Replace All
    void OnReplaceAll();

private:
    // Find/Replace interface
    MainForm* m_pMainForm;

    // Search direction options
    ui::Option* m_pDirectionOption;

    // Whether to match case
    ui::CheckBox* m_pCaseSensitive;

    // Whether to match whole words
    ui::CheckBox* m_pMatchWholeWord;

    // Find content
    ui::RichEdit* m_pFindText;

    // Replace content
    ui::RichEdit* m_pReplaceText;
};

#endif //EXAMPLES_REPLACE_FORM_H_
