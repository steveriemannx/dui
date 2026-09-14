#include "dui/dui.h"
#include "MoveControlForm.h"
#include "embedded_resources.inc"
#include "dui/Utils/UiBuilder.h"

int main()
{
    return ui::RunMemory<MoveControlForm>("move_control_code", EmbeddedResources());
}
