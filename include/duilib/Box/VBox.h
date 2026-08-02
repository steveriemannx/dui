#ifndef UI_CORE_VBOX_H_
#define UI_CORE_VBOX_H_

#include "duilib/Core/Box.h"
#include "duilib/Layout/VLayout.h"
#include "duilib/Layout/VFlowLayout.h"

namespace ui
{

/** Box with vertical layout (no wrapping)
*/
class DUILIB_API VBox : public Box
{
public:
    explicit VBox(Window* pWindow) :
        Box(pWindow, new VLayout())
    {
    }

    virtual DString GetType() const override { return DUI_CTR_VBOX; }
};

/** Box with vertical flow layout (auto wrapping)
*/
class DUILIB_API VFlowBox : public Box
{
public:
    explicit VFlowBox(Window* pWindow) :
        Box(pWindow, new VFlowLayout())
    {
    }

    virtual DString GetType() const override { return DUI_CTR_VFLOWBOX; }
};

}
#endif // UI_CORE_VBOX_H_
