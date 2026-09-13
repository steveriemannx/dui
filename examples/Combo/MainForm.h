#ifndef EXAMPLES_COMBO_MAIN_FORM_H_
#define EXAMPLES_COMBO_MAIN_FORM_H_

#include "dui/dui.h"

class MainForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;

public:
    MainForm() = default;
    virtual ~MainForm() override = default;

    virtual std::string GetSkinFolder() override { return "combo"; }
    virtual std::string GetSkinFile() override { return "combo.xml"; }
    virtual void OnInitWindow() override;
};

#endif // EXAMPLES_COMBO_MAIN_FORM_H_
