#include "dui/dui.h"
#include "ControlForm.h"
#include "dui/Utils/UiBuilder.h"

int main()
{
    return ui::Run<ControlForm>(DUI_T("controls_round"));
}
