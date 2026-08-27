#include "dui/dui.h"
#include "MoveControlForm.h"
#include "dui/Utils/UiBuilder.h"

int main()
{
    return ui::Run<MoveControlForm>(_T("move_control"));
}
