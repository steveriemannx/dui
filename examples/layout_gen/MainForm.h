#ifndef EXAMPLES_LAYOUT_GEN_MAIN_FORM_H_
#define EXAMPLES_LAYOUT_GEN_MAIN_FORM_H_

// dui
#include "dui/dui.h"

class MainForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    MainForm() = default;
    virtual ~MainForm() override = default;

    /** Resource-related interfaces. The generated UI does not load a skin XML file. */
    virtual std::string GetSkinFolder() override { return "layout"; }
    virtual std::string GetSkinFile() override { return ""; }

    /** Called after the window is created, for subclasses to do some initialization work
     */
    virtual void OnInitWindow() override;

private:
    void BuildUI();
    void BindEvents();
};

#endif //EXAMPLES_LAYOUT_GEN_MAIN_FORM_H_
