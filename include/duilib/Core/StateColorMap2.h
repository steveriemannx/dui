#ifndef UI_CORE_STATE_COLOR_MAP2_H_
#define UI_CORE_STATE_COLOR_MAP2_H_

#include "duilib/Render/IRender.h"
#include "duilib/Core/UiTypes.h"

namespace ui 
{
/** Mapping of control states to color values; an implementation supporting more attributes
*/
class Control;
class IRender;
class DUILIB_API StateColorMap2
{
public:
    explicit StateColorMap2(Control* pControl);

    /** Get the color value; returns empty if the color is not included
    * @param [in] stateType Which state's color value to get; refer to the ControlStateType enum
    * @return The color string set for the specified state
    */
    DString GetStateColor(ControlStateType stateType) const;

    /** Get the color rectangle outer margin of the control in a certain state
     * @param [in] stateType Which state's color value to get; refer to the ControlStateType enum
     * @return The color rectangle outer margin set for the specified state
     */
    UiMargin GetStateColorMargin(ControlStateType stateType) const;

    /** Get the corner radius size of the control color rectangle in a certain state
     * @param [in] stateType Which state's color value to get; refer to the ControlStateType enum
     * @return The color rectangle corner radius size set for the specified state
     */
    UiSize GetStateColorRound(ControlStateType stateType) const;

    /** Set the control color, color outer margin, and color rectangle corner radius size for a certain state
     * @param [in] stateType Which state's color value to set; refer to the ControlStateType enum
     * @param [in] color The color value to set; this value must exist in global.xml
     * @param [in] colorMargin The color rectangle outer margin to set; if not set, the color rectangle coincides with the control rectangle
     * @param [in] colorRound The color rectangle corner radius size to set; if not set, the color rectangle follows the shape of the control rectangle
     */
    void SetStateColor(ControlStateType stateType, const DString& color);
    void SetStateColorMargin(ControlStateType stateType, const UiMargin& colorMargin);
    void SetStateColorRound(ControlStateType stateType, const UiSize& colorRound);

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
    /** Draw the color of the specified state (using the pre-fetched color value)
    */
    void DoPaintStateColor(IRender* pRender, UiRect rcPaint, ControlStateType stateType, UiColor colorValue, uint8_t nAlpha = 255) const;

private:
    /** Basic attributes of each color
    */
    struct TColorProperty
    {
        // Color string
        UiString m_colorStr;

        // The outer margin of this color
        UiMargin16 m_colorMargin;

        // The corner radius size of this color
        UiSize16 m_colorRound;
    };

private:
    /** The associated control interface
    */
    Control* m_pControl;

    /** Mapping table of states to color values
    */
    std::vector<TColorProperty> m_stateColors;
};

} // namespace ui

#endif // UI_CORE_STATE_COLOR_MAP2_H_
