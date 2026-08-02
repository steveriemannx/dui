#ifndef EXAMPLES_RENDER_TEST2_H_
#define EXAMPLES_RENDER_TEST2_H_

// duilib
#include "duilib/duilib.h"

namespace ui {

class RenderTest2 : public ui::Control
{
    typedef ui::Control BaseClass;
public:
    explicit RenderTest2(ui::Window* pWindow);
    virtual ~RenderTest2() override;

public:
    /** Draw the control itself and its child controls
     * @param[in] pRender render interface
     * @param[in] rcPaint the specified drawing area
     */
    virtual void AlphaPaint(IRender* pRender, const UiRect& rcPaint) override;

    /** Draw the control itself
    * @param[in] pRender render interface
    * @param[in] rcPaint the specified drawing area
    */
    virtual void Paint(IRender* pRender, const UiRect& rcPaint) override;

    /** Draw the child controls of the control
    * @param[in] pRender render interface
    * @param[in] rcPaint the specified drawing area
    */
    virtual void PaintChild(IRender* pRender, const UiRect& rcPaint) override;

private:
    /** Adapt an integer value to DPI
    */
    int32_t DpiScaledInt(int32_t iValue);
    float DpiScaledFloat(int32_t iValue);

    /** Draw a color map of tiled regular hexagons within a rectangle, filling the rectangle as much as possible
    */
    void DrawColorMap(IRender* pRender, const UiRect& rect);

    /** Draw a regular hexagon (path fill using polygon vertices)
    * @param [in] pRender render interface
    * @param [in] centerPt the center point coordinates of the regular hexagon
    * @param [in] radius the radius of the regular hexagon, the distance from the center to the vertices (also the side length)
    * @param [in] penColor the color of the pen; if 0, the border is not drawn
    * @param [in] penWidth the width of the pen; if 0, the border is not drawn
    * @param [in] brushColor the color of the brush; if 0, use the fill color
    */
    bool DrawRegularHexagon(IRender* pRender, const UiPointF& centerPt, int32_t radius, 
                            const UiColor& penColor, int32_t penWidth, const UiColor& brushColor);

    /** Draw a regular hexagon (drawn by rotating triangles)
    * @param [in] pRender render interface
    * @param [in] centerPt the center point coordinates of the regular hexagon
    * @param [in] radius the radius of the regular hexagon, the distance from the center to the vertices (also the side length)
    * @param [in] penColor the color of the pen; if 0, the border is not drawn
    * @param [in] penWidth the width of the pen; if 0, the border is not drawn
    * @param [in] brushColor the color of the brush; if 0, use the fill color
    */
    bool DrawRegularHexagon3(IRender* pRender, const UiPoint& centerPt, int32_t radius, 
                             const UiColor& penColor, int32_t penWidth, const UiColor& brushColor);

};

} //end of namespace ui
#endif //EXAMPLES_RENDER_TEST2_H_
