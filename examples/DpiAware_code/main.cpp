#include "dui/dui.h"
#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

int main()
{
    return ui::Run<MainForm>(_T("DpiAware (Pure Code)"));
}
