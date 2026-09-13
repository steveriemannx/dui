#include "dui/dui.h"
#include "RenderForm.h"
#include "embedded_resources.inc"
#include "dui/Utils/UiBuilder.h"

int main()
{
    return ui::RunMemory<RenderForm>("Render Code Test", GetEmbeddedResourcesData(), GetEmbeddedResourcesSize());
}
