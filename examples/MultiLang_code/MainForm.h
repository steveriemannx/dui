#ifndef EXAMPLES_MULTILANG_CODE_MAIN_FORM_H_
#define EXAMPLES_MULTILANG_CODE_MAIN_FORM_H_

#include "dui/dui.h"

/** Mode 3 (pure code): multilingual example; the layout is built entirely with C++ code
 */
class MainForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    MainForm() = default;
    virtual ~MainForm() override = default;

    virtual DString GetSkinFolder() override { return "MultiLang"; }
    virtual DString GetSkinFile() override { return ""; }
    virtual void OnInitWindow() override;

private:
    void SetupWindow();
    void BuildUI();
    void ShowPopupMenu(const ui::UiPoint& point);
    void BindEvents();
};

#endif
