#ifndef UI_CORE_STATE_COLOR_MAP_H_
#define UI_CORE_STATE_COLOR_MAP_H_

#include "duilib/Render/IRender.h"
#include "duilib/Core/UiTypes.h"

namespace ui 
{
/** Mapping of control states to color values
*/
class Control;
class IRender;
class DUILIB_API StateColorMap
{
public:
    explicit StateColorMap(Control* pControl);

    /** Get the color value; returns empty if the color is not included
    */
    DString GetStateColor(ControlStateType stateType) const;

    /** Set the color value
    */
    void SetStateColor(ControlStateType stateType, const DString& color);

public:
    /** Whether it contains the color of the Hot state
    */
    bool HasHotColor() const;

    /** Whether it contains the specified color value
    */
    bool HasStateColor(ControlStateType stateType) const;

    /** Whether it contains any color value
    */
    bool HasStateColors() const;

public:
    /** Draw the color of the specified state
    */
    void PaintStateColor(IRender* pRender, const UiRect& rcPaint, ControlStateType stateType) const;

private:
    /** The associated control interface
    */
    Control* m_pControl;

    /** Mapping table of states to color values
    */
    std::vector<UiString> m_stateColors;
};

} // namespace ui

#endif // UI_CORE_STATE_COLOR_MAP_H_
