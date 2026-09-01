#include "MainForm.h"

MainForm::MainForm()
{
    AttachWindowCreateMsg([this](const ui::EventArgs& /*args*/) {
        SetSelectedColor(ui::UiColor(ui::UiColors::White));
        return true;
    });
}
