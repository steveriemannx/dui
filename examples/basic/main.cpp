#include "dui/dui.h"
#include "dui/Utils/UiBuilder.h"

int main()
{
    return ui::RunXml(
        ui::XmlWindowOptions()
            .Title(_T("basic"))
            .SkinFolder(_T("basic"))
            .SkinFile(_T("basic.xml"))
    );
}
