#ifndef EXAMPLES_MULTILANG_GEN_MAIN_FORM_H_
#define EXAMPLES_MULTILANG_GEN_MAIN_FORM_H_

#include "dui/dui.h"

class MainForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    MainForm() = default;
    virtual ~MainForm() override = default;

    virtual std::string GetSkinFolder() override { return "MultiLang"; }
    virtual std::string GetSkinFile() override { return ""; }
    virtual void OnInitWindow() override;

private:
    void BuildUI();
    void ShowPopupMenu(const ui::UiPoint& point);
    void BindEvents();
};

#endif
