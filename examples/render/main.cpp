#include "dui/dui.h"
#include "RenderForm.h"
#include "dui/Utils/UiBuilder.h"

int main()
{
    return ui::Run<RenderForm>(DUI_T("Render Test"));
}
