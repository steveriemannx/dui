#ifndef UI_BOX_GRIDBOX_H_
#define UI_BOX_GRIDBOX_H_

#include "dui/Core/Box.h"
#include "dui/Box/ScrollBox.h"
#include "dui/Layout/GridLayout.h"

namespace ui
{
/** A container with a grid layout (template class)
*/
template<typename InheritType = Box>
class GridBoxT : public InheritType
{
    typedef GridBoxT<InheritType> BaseClass;
public:
    explicit GridBoxT(Window* pWindow) :
        InheritType(pWindow, new GridLayout())
    {
    }

    /** Get the control type
    */
    virtual DString GetType() const override { return DUI_CTR_GRIDBOX; }

public:
    /** Get the number of rows (0 means auto-calculated)
     */
    int32_t GetRows() const
    {
        ASSERT(this->GetLayout()->GetLayoutType() == LayoutType::GridLayout);
        GridLayout* pGridLayout = dynamic_cast<GridLayout*>(this->GetLayout());
        if (pGridLayout != nullptr) {
            return pGridLayout->GetRows();
        }
        return 0;
    }

    /** Set the number of rows (0 means auto-calculated)
     * @param [in] nRows The number of rows
     */
    void SetRows(int32_t nRows)
    {
        ASSERT(this->GetLayout()->GetLayoutType() == LayoutType::GridLayout);
        GridLayout* pGridLayout = dynamic_cast<GridLayout*>(this->GetLayout());
        if (pGridLayout != nullptr) {
            pGridLayout->SetRows(nRows);
        }
    }

    /** Get the number of columns (0 means auto-calculated)
     */
    int32_t GetColumns() const
    {
        ASSERT(this->GetLayout()->GetLayoutType() == LayoutType::GridLayout);
        GridLayout* pGridLayout = dynamic_cast<GridLayout*>(this->GetLayout());
        if (pGridLayout != nullptr) {
            return pGridLayout->GetColumns();
        }
        return 0;
    }

    /** Set the number of columns (0 means auto-calculated)
     * @param [in] nCols The number of columns
     */
    void SetColumns(int32_t nCols)
    {
        ASSERT(this->GetLayout()->GetLayoutType() == LayoutType::GridLayout);
        GridLayout* pGridLayout = dynamic_cast<GridLayout*>(this->GetLayout());
        if (pGridLayout != nullptr) {
            pGridLayout->SetColumns(nCols);
        }
    }

    /** Set the grid cell width (0 means auto-calculated)
    */
    void SetGridWidth(int32_t nGridWidth, bool bNeedDpiScale)
    {
        ASSERT(this->GetLayout()->GetLayoutType() == LayoutType::GridLayout);
        GridLayout* pGridLayout = dynamic_cast<GridLayout*>(this->GetLayout());
        if (pGridLayout != nullptr) {
            pGridLayout->SetGridWidth(nGridWidth, bNeedDpiScale);
        }
    }

    /** Get the grid cell width (0 means auto-calculated)
    * @return Returns the grid width (already DPI scaled)
    */
    int32_t GetGridWidth() const
    {
        ASSERT(this->GetLayout()->GetLayoutType() == LayoutType::GridLayout);
        GridLayout* pGridLayout = dynamic_cast<GridLayout*>(this->GetLayout());
        if (pGridLayout != nullptr) {
            return pGridLayout->GetGridWidth();
        }
        return 0;
    }

    /** Set the grid cell height (0 means auto-calculated)
    */
    void SetGridHeight(int32_t nGridHeight, bool bNeedDpiScale)
    {
        ASSERT(this->GetLayout()->GetLayoutType() == LayoutType::GridLayout);
        GridLayout* pGridLayout = dynamic_cast<GridLayout*>(this->GetLayout());
        if (pGridLayout != nullptr) {
            pGridLayout->SetGridHeight(nGridHeight, bNeedDpiScale);
        }
    }

    /** Get the grid cell height (0 means auto-calculated)
    * @return Returns the grid height (already DPI scaled)
    */
    int32_t GetGridHeight() const
    {
        ASSERT(this->GetLayout()->GetLayoutType() == LayoutType::GridLayout);
        GridLayout* pGridLayout = dynamic_cast<GridLayout*>(this->GetLayout());
        if (pGridLayout != nullptr) {
            return pGridLayout->GetGridHeight();
        }
        return 0;
    }

    /** Whether to scale down proportionally when the control content exceeds the boundary
     *   true  Use the size of the child control; if it exceeds the grid size, scale it down proportionally so that the control content is fully displayed in the grid
     *   false Ignore the size of the child control itself; the child control size matches the grid size
     */
    void SetScaleDown(bool bScaleDown)
    {
        ASSERT(this->GetLayout()->GetLayoutType() == LayoutType::GridLayout);
        GridLayout* pGridLayout = dynamic_cast<GridLayout*>(this->GetLayout());
        if (pGridLayout != nullptr) {
            pGridLayout->SetScaleDown(bScaleDown);
        }
    }

    /** Check whether it scales down proportionally when exceeding the boundary
     *@return Returns true  Use the size of the child control; if it exceeds the grid size, scale it down proportionally so that the control content is fully displayed in the grid
     *              false Ignore the size of the child control itself; the child control size matches the grid size
     */
    bool IsScaleDown() const
    {
        ASSERT(this->GetLayout()->GetLayoutType() == LayoutType::GridLayout);
        GridLayout* pGridLayout = dynamic_cast<GridLayout*>(this->GetLayout());
        if (pGridLayout != nullptr) {
            return pGridLayout->IsScaleDown();
        }
        return false;
    }
};

/** A container with a grid layout
*/
class DUI_API GridBox : public GridBoxT<Box>
{
    typedef GridBoxT<Box> BaseClass;
public:
    explicit GridBox(Window* pWindow) :
        GridBoxT<Box>(pWindow)
    {
    }

    /** Get the control type
    */
    virtual DString GetType() const override { return DUI_CTR_GRIDBOX; }
};

/** A container with a grid layout (supports a scroll bar)
*/
class DUI_API GridScrollBox : public GridBoxT<ScrollBox>
{
    typedef GridBoxT<ScrollBox> BaseClass;
public:
    explicit GridScrollBox(Window* pWindow) :
        GridBoxT<ScrollBox>(pWindow)
    {
    }

    /** Get the control type
    */
    virtual DString GetType() const override { return DUI_CTR_GRID_SCROLLBOX; }
};

} //namespace ui
#endif // UI_BOX_GRIDBOX_H_
