#ifndef UI_CONTROL_COLORPICKER_STANDARD_H_
#define UI_CONTROL_COLORPICKER_STANDARD_H_

#include "dui/Core/Control.h"
#include "dui/Render/IRender.h"

namespace ui
{
/** Color picker: standard colors
*/
class DUI_API ColorPickerStatard : public Control
{
    typedef Control BaseClass;
public:
    explicit ColorPickerStatard(Window* pWindow);

    /** Get the control type
    */
    virtual DString GetType() const override;

    /** Select a color
    */
    void SelectColor(const UiColor& color);

    /**
    * @brief The entry function for drawing the control
    * @param[in] pRender Specifies the drawing area
    * @param[in] rcPaint Specifies the drawing coordinates
    * @return None
    */
    virtual void Paint(IRender* pRender, const UiRect& rcPaint) override;

    /** Draw a color map tiled with hexagons within a rectangle, filling the rectangle as much as possible
    */
    void DrawColorMap(IRender* pRender, const UiRect& rect);

    /** Draw a regular hexagon (filled using polygon vertices as the path)
    * @param [in] pRender The rendering interface
    * @param [in] centerPt The center point coordinates of the hexagon
    * @param [in] radius The radius of the hexagon, the distance from the center to a vertex (also the side length)
    * @param [in] penColor The color of the pen; if it is 0, no border is drawn
    * @param [in] penWidth The width of the pen; if it is 0, no border is drawn
    * @param [in] brushColor The color of the brush; if it is 0, no fill is drawn
    */
    bool DrawRegularHexagon(IRender* pRender, const UiPointF& centerPt, int32_t radius,
                            const UiColor& penColor, float penWidth, const UiColor& brushColor);

    /** Mouse move
    */
    virtual bool MouseMove(const EventArgs& msg) override;

    /** Mouse left button down
    */
    virtual bool ButtonDown(const EventArgs& msg) override;

    /** Get the color information of the current mouse position
    */
    bool GetColorInfo(const UiPoint& ptMouse, UiColor& ptColor) const;

    /** Calculate the distance between two points
    */
    float GetPointsDistance(const UiPointF& pt1, const UiPointF& pt2) const;

    /** Initialize the color table
    */
    void InitColorMap();

    /** Listen to the color selection event
    * @param [in] callback The callback function triggered when the selected color changes
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    * Parameter description:
    *    wParam: The currently newly selected color value; a color can be generated with UiColor((uint32_t)wParam)
    *    lParam: The previously selected old color value; a color can be generated with UiColor((uint32_t)lParam)
    */
    void AttachSelectColor(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventSelectColor, callback, callbackID); }

private:
    /** The color table
    */
    struct ColorInfo
    {
        //The color value
        UiColor color;

        //The center point coordinates of the hexagon of this color
        UiPointF centerPt;
    };
    std::vector<ColorInfo> m_colorMap;

    /** The radius of the hexagon
    */
    int32_t m_radius;

    /** Which color is currently selected
    */
    UiColor m_selectedColor;
};

}//namespace ui

#endif //UI_CONTROL_COLORPICKER_STANDARD_H_
