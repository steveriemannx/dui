#ifndef EXAMPLES_HELLO3_MAIN_FORM_H_
#define EXAMPLES_HELLO3_MAIN_FORM_H_

// dui
#include "dui/dui.h"

class MainForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    MainForm() = default;
    virtual ~MainForm() override = default;

    /** Resources are loaded from the executable directory. */
    virtual DString GetSkinFolder() override { return DUI_T(""); }
    virtual DString GetSkinFile() override { return DUI_T("hello.xml"); }

    /** Called after the window is created, for subclasses to do some initialization work
     */
    virtual void OnInitWindow() override;

private:
    void BindEvents();
};

#endif //EXAMPLES_HELLO3_MAIN_FORM_H_
