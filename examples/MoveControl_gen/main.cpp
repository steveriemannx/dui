#include "dui/dui.h"
#include "MoveControlForm.h"
#include "embedded_resources.inc"
#include "dui/Utils/UiBuilder.h"

int main()
{
    return ui::RunMemory<MoveControlForm>(DUI_T("move_control_gen"), GetEmbeddedResourcesData(), GetEmbeddedResourcesSize());
}
