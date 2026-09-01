#ifndef EXAMPLES_MAIN_FORM_H_
#define EXAMPLES_MAIN_FORM_H_

// dui
#include "dui/dui.h"

class MainForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    MainForm() = default;
    virtual ~MainForm() override = default;

    /** Resource-related interfaces
     * The pure-code version does not load a skin XML file.
     */
    virtual DString GetSkinFolder() override { return DUI_T("layout"); }
    virtual DString GetSkinFile() override { return DUI_T(""); }

    /** Called after the window is created, for subclasses to do some initialization work
     */
    virtual void OnInitWindow() override;

private:
    void SetupWindow();
    void BuildUI();
    void BindEvents();
};

#endif //EXAMPLES_MAIN_FORM_H_
