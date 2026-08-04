#ifndef EXAMPLES_FIND_FORM_H_
#define EXAMPLES_FIND_FORM_H_

// dui
#include "dui/dui.h"

class MainForm;
class FindForm : public ui::WindowImplBase
{
public:
    explicit FindForm(MainForm* pMainForm);
    virtual ~FindForm() override;

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
    /** Pure code UI construction (corresponds to the find.xml layout)
    */
    void BuildUI();

private:
    // Find Next
    void OnFindNext();

private:
    // Find interface
    MainForm* m_pMainForm;

    // Search direction options
    ui::Option* m_pDirectionOption;

    // Whether to match case
    ui::CheckBox* m_pCaseSensitive;

    // Whether to match whole words
    ui::CheckBox* m_pMatchWholeWord;

    // Find content
    ui::RichEdit* m_pFindText;
};

#endif //EXAMPLES_FIND_FORM_H_
