#ifndef EXAMPLES_BASIC_MAIN_FORM_H_
#define EXAMPLES_BASIC_MAIN_FORM_H_

// dui
#include "dui/dui.h"

class MainForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    MainForm() = default;
    virtual ~MainForm() override = default;

    /** Resource-related interfaces
     * GetSkinFolder sets the skin resource path of the window to be drawn
     * GetSkinFile sets the XML description file of the window to be drawn
     */
    virtual std::string GetSkinFolder() override { return "basic"; }
    virtual std::string GetSkinFile() override { return "basic.xml"; }

    /** Called after the window is created, for subclasses to do some initialization work
     */
    virtual void OnInitWindow() override;

};

#endif //EXAMPLES_BASIC_MAIN_FORM_H_
