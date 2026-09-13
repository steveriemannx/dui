#include "dui/dui.h"
#include "ControlForm.h"
#include "embedded_resources.inc"
#include "dui/Utils/UiBuilder.h"

int main()
{
    return ui::RunMemory<ControlForm>("controls (Codegen)", GetEmbeddedResourcesData(), GetEmbeddedResourcesSize());
}
