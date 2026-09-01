#include "DragForm.h"

DragForm::DragForm()
{
}

DragForm::~DragForm()
{
}

DString DragForm::GetSkinFolder()
{
    return DUI_T("cef_browser");
}

DString DragForm::GetSkinFile()
{
    return DUI_T("drag_form.xml");
}
