#ifndef UI_CORE_UIPOINT_H_
#define UI_CORE_UIPOINT_H_

#include "duilib/duilib_defs.h"
#include <cstdint>

namespace ui 
{

/** Wrapper for a point
*/
class DUILIB_API UiPoint
{
public:
    UiPoint()
    {
        x = y = 0;
    }

    UiPoint(int32_t xValue, int32_t yValue)
    {
        x = xValue;
        y = yValue;
    }

    /** Set new X and Y values
    */
    void SetXY(int32_t xValue, int32_t yValue)
    {
        x = xValue;
        y = yValue;
    }

    /** Set a new X value
    */
    void SetX(int32_t xValue) { x = xValue; }

    /** Set a new Y value
    */
    void SetY(int32_t yValue) { y = yValue;    }

    /** Offset operation
    */
    void Offset(int32_t offsetX, int32_t offsetY)
    {
        x += offsetX;
        y += offsetY;
    }

    /** Offset operation
    */
    void Offset(const UiPoint& offsetPoint)
    {
        x += offsetPoint.x;
        y += offsetPoint.y;
    }

    /** Return the x value
    */
    constexpr int32_t X() const { return x; }

    /** Return the y value
    */
    constexpr int32_t Y() const { return y; }

    /** Return true if both x and y are 0
    */
    bool IsZero() const 
    { 
        return x == 0 && y == 0; 
    }

    /** Determine whether it is the same as another point
    */
    bool Equals(int32_t xValue, int32_t yValue) const 
    {
        return x == xValue && y == yValue;
    }

    /** Determine whether it is the same as another point
    */
    bool Equals(const UiPoint& dst) const
    {
        return x == dst.x && y == dst.y;
    }

    /** Determine whether the two values are equal
    */
    friend bool operator == (const UiPoint& a, const UiPoint& b)
    {
        return a.Equals(b);
    }

    /** Determine whether the two values are not equal
    */
    friend bool operator != (const UiPoint& a, const UiPoint& b)
    {
        return !a.Equals(b);
    }

public:
    /** X-axis coordinate
    */
    int32_t x;

    /** Y-axis coordinate
    */
    int32_t y;
};

} // namespace ui

#endif // UI_CORE_UIPOINT_H_
