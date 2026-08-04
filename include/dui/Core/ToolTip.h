#ifndef UI_CORE_TOOLTIP_H_
#define UI_CORE_TOOLTIP_H_

#include "dui/Core/UiPoint.h"
#include "dui/Core/UiRect.h"
#include <string>

namespace ui 
{
class WindowBase;

/** Implementation of ToolTip
*/
class DUI_API ToolTip
{
public:
    ToolTip();
    ToolTip(const ToolTip& r) = delete;
    ToolTip& operator = (const ToolTip& r) = delete;
    ~ToolTip();
    
public:
    /** Set the mouse tracking state
    * @param [in] pParentWnd Parent window
    * @param [in] bTracking Whether to track the mouse state
    */
    void SetMouseTracking(WindowBase* pParentWnd, bool bTracking);

    /**@brief Display ToolTip information
    * @param [in] pParentWnd Parent window
    * @param [in] rect ToolTip display area
    * @param [in] maxWidth Maximum ToolTip display width
    * @param [in] trackPos Tracked position
    * @param [in] text ToolTip display content
    */
    void ShowToolTip(WindowBase* pParentWnd,
                     const UiRect& rect, 
                     uint32_t maxWidth,
                     const UiPoint& trackPos,
                     const DString& text);

    /** Hide ToolTip information
    */
    void HideToolTip();

    /** Clear the mouse tracking state
    */
    void ClearMouseTracking();

    /** Destroy the ToolTip window
    */
    void DestroyToolTip();

private:
    // Internal implementation
    class TImpl;
    TImpl* m_impl;
};

} // namespace ui

#endif // UI_CORE_TOOLTIP_H_
