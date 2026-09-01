#include "dui/dui.h"
#include "ControlForm.h"
#include "embedded_resources.inc"
#include "dui/Utils/UiBuilder.h"

int main()
{
    return ui::RunMemory<ControlForm>(DUI_T("controls (Pure Code)"), GetEmbeddedResourcesData(), GetEmbeddedResourcesSize());
}
