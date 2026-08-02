#ifndef UI_CORE_UIPOINTF_H_
#define UI_CORE_UIPOINTF_H_

#include "duilib/Core/UiPoint.h"

namespace ui 
{

/** Wrapper for a point
*/
class DUILIB_API UiPointF
{
public:
    /** Construct from UiPoint
    */
    static UiPointF MakeFromPoint(const UiPoint& pt)
    {
        return UiPointF((float)pt.x, (float)pt.y);
    }

public:
    UiPointF()
    {
        x = y = 0;
    }

    UiPointF(float xValue, float yValue)
    {
        x = xValue;
        y = yValue;
    }

    UiPointF(int32_t xValue, int32_t yValue)
    {
        x = (float)xValue;
        y = (float)yValue;
    }

    /** Set new X and Y values
    */
    void SetXY(float xValue, float yValue)
    {
        x = xValue;
        y = yValue;
    }

    /** Set a new X value
    */
    void SetX(float xValue) { x = xValue; }

    /** Set a new Y value
    */
    void SetY(float yValue) { y = yValue;    }

    /** Offset operation
    */
    void Offset(float offsetX, float offsetY)
    {
        x += offsetX;
        y += offsetY;
    }

    /** Offset operation
    */
    void Offset(const UiPointF& offsetPoint)
    {
        x += offsetPoint.x;
        y += offsetPoint.y;
    }

    /** Return the x value
    */
    constexpr float X() const { return x; }

    /** Return the y value
    */
    constexpr float Y() const { return y; }

    /** Return true if both x and y are 0
    */
    bool IsZero() const 
    { 
        return x == 0 && y == 0; 
    }

    /** Determine whether it is the same as another point
    */
    bool Equals(float xValue, float yValue) const
    {
        return x == xValue && y == yValue;
    }

    /** Determine whether it is the same as another point
    */
    bool Equals(const UiPointF& dst) const
    {
        return x == dst.x && y == dst.y;
    }

    /** Determine whether the two values are equal
    */
    friend bool operator == (const UiPointF& a, const UiPointF& b)
    {
        return a.Equals(b);
    }

    /** Determine whether the two values are not equal
    */
    friend bool operator != (const UiPointF& a, const UiPointF& b)
    {
        return !a.Equals(b);
    }

public:
    /** X-axis coordinate
    */
    float x;

    /** Y-axis coordinate
    */
    float y;
};

} // namespace ui

#endif // UI_CORE_UIPOINTF_H_
