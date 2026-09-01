#include "dui/dui.h"
#include "ListBoxForm.h"
#include "embedded_resources.inc"
#include "dui/Utils/UiBuilder.h"

int main()
{
    return ui::RunMemory<ListBoxForm>(DUI_T("ListBox (Pure Code)"), GetEmbeddedResourcesData(), GetEmbeddedResourcesSize());
}
