#ifndef EXAMPLES_HELLO2_MAIN_FORM_H_
#define EXAMPLES_HELLO2_MAIN_FORM_H_

// dui
#include "dui/dui.h"

class MainForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    MainForm() = default;
    virtual ~MainForm() override = default;

    /** Resource-related interfaces.
     *  hello2 is a self-contained example: its resources live in a local
     *  "hello2_resources" folder next to the executable (see main.cpp), not in
     *  the global dui resources/ tree.
     */
    virtual DString GetSkinFolder() override { return ""; }
    virtual DString GetSkinFile() override { return "hello.xml"; }

    /** Called after the window is created, for subclasses to do some initialization work
     */
    virtual void OnInitWindow() override;

private:
    void BindEvents();
};

#endif //EXAMPLES_HELLO2_MAIN_FORM_H_
