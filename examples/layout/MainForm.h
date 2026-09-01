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
     * The GetSkinFolder interface sets the skin resource path for the window to be drawn
     * The GetSkinFile interface sets the xml description file for the window to be drawn
     */
    virtual DString GetSkinFolder() override { return "layout"; }
    virtual DString GetSkinFile() override { return "layout.xml"; }

    /** Called after the window is created, for subclasses to do some initialization work
     */
    virtual void OnInitWindow() override;

private:
    void BindEvents();
};

#endif //EXAMPLES_MAIN_FORM_H_
