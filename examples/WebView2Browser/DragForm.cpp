#include "DragForm.h"

DragForm::DragForm()
{
}

DragForm::~DragForm()
{
}

DString DragForm::GetSkinFolder()
{
    return DUI_T("webview2_browser");
}

DString DragForm::GetSkinFile()
{
    return DUI_T("drag_form.xml");
}
