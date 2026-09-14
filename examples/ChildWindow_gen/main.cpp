#include "dui/dui.h"
#include "MainForm.h"
#include "embedded_resources.inc"
#include "dui/Utils/UiBuilder.h"

int main()
{
    return ui::RunMemory<MainForm>("ChildWindow (Codegen)", EmbeddedResources(), [](MainForm* window) {
        window->PaintNextChildWindow();
    });
}
