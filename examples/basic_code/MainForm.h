#ifndef EXAMPLES_BASIC_CODE_MAIN_FORM_H_
#define EXAMPLES_BASIC_CODE_MAIN_FORM_H_

#include "duilib/duilib.h"

/** Mode 3 (pure code): the window layout is built entirely with C++ code, with no layout XML
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
    bool OnButtonClick(const ui::EventArgs& msg);
};

#endif
