#ifndef UI_BOX_TILEBOX_H_
#define UI_BOX_TILEBOX_H_

#include "dui/Core/Box.h"
#include "dui/Layout/VTileLayout.h"
#include "dui/Layout/HTileLayout.h"

namespace ui
{

/** Box with tile layout (vertical layout)
*/
class DUI_API VTileBox : public Box
{
public:
    explicit VTileBox(Window* pWindow) :
        Box(pWindow, new VTileLayout())
    {
    }

    virtual DString GetType() const override { return DUI_CTR_VTILE_BOX; }
};

/** Box with tile layout (horizontal layout)
*/
class DUI_API HTileBox : public Box
{
public:
    explicit HTileBox(Window* pWindow) :
        Box(pWindow, new HTileLayout())
    {
    }

    virtual DString GetType() const override { return DUI_CTR_HTILE_BOX; }
};

}
#endif // UI_BOX_TILEBOX_H_
