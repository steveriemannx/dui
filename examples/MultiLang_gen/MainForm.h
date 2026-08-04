#ifndef EXAMPLES_MULTILANG_GEN_MAIN_FORM_H_
#define EXAMPLES_MULTILANG_GEN_MAIN_FORM_H_

#include "dui/dui.h"

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
    // Show the language selection menu (pure code menu, no XML template)
    void ShowPopupMenu(const ui::UiPoint& point);
};

#endif
