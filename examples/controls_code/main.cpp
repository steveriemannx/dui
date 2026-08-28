#include "dui/dui.h"
#include "ControlForm.h"
#include "dui/Utils/UiBuilder.h"

int main()
{
    return ui::Run<ControlForm>(_T("controls"));
}
