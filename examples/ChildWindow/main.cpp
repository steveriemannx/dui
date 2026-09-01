#include "dui/dui.h"
#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

int main()
{
    return ui::Run<MainForm>(DUI_T("ChildWindow"), [](MainForm* window) {
        window->PaintNextChildWindow();
    });
}
