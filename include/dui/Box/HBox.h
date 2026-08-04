#ifndef UI_BOX_HBOX_H_
#define UI_BOX_HBOX_H_

#include "dui/Core/Box.h"
#include "dui/Layout/HLayout.h"
#include "dui/Layout/HFlowLayout.h"

namespace ui
{

/** Box with horizontal layout (no wrapping)
*/
class DUI_API HBox : public Box
{
public:
    explicit HBox(Window* pWindow) :
        Box(pWindow, new HLayout())
    {
    }

    virtual DString GetType() const override { return DUI_CTR_HBOX; }
};

/** Box with horizontal flow layout (auto wrapping)
*/
class DUI_API HFlowBox : public Box
{
public:
    explicit HFlowBox(Window* pWindow) :
        Box(pWindow, new HFlowLayout())
    {
    }

    virtual DString GetType() const override { return DUI_CTR_HFLOWBOX; }
};

}
#endif // UI_BOX_HBOX_H_
