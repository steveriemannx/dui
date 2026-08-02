#ifndef UI_RENDER_SKIA_PEN_H_
#define UI_RENDER_SKIA_PEN_H_

#include "duilib/Render/IRender.h"

namespace ui 
{

class Pen_Skia : public IPen
{
public:
    explicit Pen_Skia(UiColor color, float fWidth = 1.0f);
    Pen_Skia(const Pen_Skia& r);
    Pen_Skia& operator=(const Pen_Skia& r) = delete;

    /** Set the pen width
    */
    virtual void SetWidth(float fWidth) override;

    /** Get the pen width
    */
    virtual float GetWidth() const override;

    /** Set the pen color
    */
    virtual void SetColor(UiColor color) override;

    /** Get the pen color
    */
    virtual UiColor GetColor() const override;

    /** Set the line cap style at the start of the line segment
    */
    virtual void SetStartCap(LineCap cap) override;

    /** Get the line cap style at the start of the line segment
    */
    virtual LineCap GetStartCap() const override;

    /** Set the line cap style at the end of the line segment
    */
    virtual void SetEndCap(LineCap cap) override;

    /** Get the line cap style at the end of the line segment
    */
    virtual LineCap GetEndCap() const override;

    /** Set the dash cap style
    */
    virtual void SetDashCap(LineCap cap) override;

    /** Get the dash cap style
    */
    virtual LineCap GetDashCap() const override;

    /** Set the join style used at the end of a line segment
    */
    virtual void SetLineJoin(LineJoin join) override;

    /** Get the join style used at the end of a line segment
    */
    virtual LineJoin GetLineJoin() const override;

    /** Set the drawn line style
    */
    virtual void SetDashStyle(DashStyle style) override;

    /** Get the drawn line style
    */
    virtual DashStyle GetDashStyle() const override;

    /** Clone the Pen object
    */
    virtual IPen* Clone() const override;

private:
    /** The pen color
    */
    UiColor m_color;

    /** The pen width
    */
    float m_fWidth;

    /** The line cap shape at the start of the line segment
    */
    LineCap m_startCap;

    /** The dash cap shape
    */
    LineCap m_dashCap;

    /** The line cap shape at the end of the line segment
    */
    LineCap m_endCap;

    /** The join style used at the end of a line segment where it meets another segment
    */
    LineJoin m_lineJoin;

    /** The dash style
    */
    DashStyle m_dashStyle;
};

} // namespace ui

#endif // UI_RENDER_SKIA_PEN_H_
