#ifndef EXAMPLES_BASIC_GEN_MAIN_FORM_H_
#define EXAMPLES_BASIC_GEN_MAIN_FORM_H_

// dui
#include "dui/dui.h"

class MainForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    MainForm() = default;
    virtual ~MainForm() override = default;

    /** Resource-related interfaces.
     *  This is the code-generation (gen) mode: the UI is built from generated
     *  C++ code (InitBasic, see BuildUI), so no XML skin is loaded.
     */
    virtual DString GetSkinFolder() override { return "basic"; }
    virtual DString GetSkinFile() override { return ""; }

    /** Called after the window is created, for subclasses to do some initialization work
     */
    virtual void OnInitWindow() override;

private:
    void BuildUI();
    void BindEvents();
};

#endif //EXAMPLES_BASIC_GEN_MAIN_FORM_H_
