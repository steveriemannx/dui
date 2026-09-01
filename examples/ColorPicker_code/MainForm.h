#ifndef EXAMPLES_COLORPICKER_CODE_MAIN_FORM_H_
#define EXAMPLES_COLORPICKER_CODE_MAIN_FORM_H_

// dui
#include "dui/dui.h"
#include "dui/Control/ColorPicker.h"

// LIBRARY-OWNED WINDOW EXCEPTION:
// ui::ColorPicker owns its private XML skin and control construction.
// This wrapper only supplies the initial color and public callbacks.
// Do NOT add SetupWindow()/BuildUI() here.
class MainForm : public ui::ColorPicker
{
    typedef ui::ColorPicker BaseClass;
public:
    MainForm();
    virtual ~MainForm() override = default;
};

#endif //EXAMPLES_COLORPICKER_CODE_MAIN_FORM_H_
