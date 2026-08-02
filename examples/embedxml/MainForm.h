#ifndef EXAMPLES_EMBEDXML_MAIN_FORM_H_
#define EXAMPLES_EMBEDXML_MAIN_FORM_H_

#include "duilib/duilib.h"

class MainForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    MainForm();
    virtual ~MainForm() override;

    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;
    virtual void OnInitWindow() override;
};

#endif
