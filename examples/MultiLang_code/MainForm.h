#ifndef EXAMPLES_MULTILANG_CODE_MAIN_FORM_H_
#define EXAMPLES_MULTILANG_CODE_MAIN_FORM_H_

#include "duilib/duilib.h"

/** Mode 3 (pure code): multilingual example; the layout is built entirely with C++ code
*/
class MainForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    MainForm();
    virtual ~MainForm() override;

    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;
    virtual void GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs) override;
    virtual void OnInitWindow() override;

private:
    // Build the UI (corresponding to the MultiLang.xml layout)
    void BuildUI();

    // Show the language selection menu (pure code menu, no XML template)
    void ShowPopupMenu(const ui::UiPoint& point);
};

#endif
