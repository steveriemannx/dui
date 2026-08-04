#ifndef UI_CORE_UIMARGIN_H_
#define UI_CORE_UIMARGIN_H_

#include "dui/dui_defs.h"
#include <cstdint>

namespace ui 
{
/** Data structure wrapper for the outer margin (not a rectangle; it has no width/height concept)
*   Outer margin: Margin is inside the rectangular area that excludes the control.
*/
class DUI_API UiMargin
{
public:
    UiMargin()
    {
        left = top = right = bottom = 0;
    }
    UiMargin(int32_t nLeft, int32_t nTop, int32_t nRight, int32_t nBottom)
    {
        left = nLeft;
        top = nTop;
        right = nRight;
        bottom = nBottom;
    }
    /** Clear to zero
    */
    void Clear()
    {
        left = top = right = bottom = 0;
    }
    /** Determine whether it is empty
    */
    bool IsEmpty() const
    {
        return (left == 0) && (top == 0) && (right == 0) && (bottom == 0);
    }

    /** Check whether each value is valid (no value may be negative), and correct them
    */
    void Validate()
    {
        if (left < 0) {
            left = 0;
        }
        if (top < 0) {
            top = 0;
        }
        if (right < 0) {
            right = 0;
        }
        if (bottom < 0) {
            bottom = 0;
        }
    }

    /** Determine whether it is the same as another one
    */
    bool Equals(const UiMargin& r) const
    {
        return left == r.left && top == r.top &&
               right == r.right && bottom == r.bottom;
    }

    /** Determine whether the two values are equal
    */
    friend bool operator == (const UiMargin& a, const UiMargin& b)
    {
        return a.Equals(b);
    }

    /** Determine whether the two values are not equal
    */
    friend bool operator != (const UiMargin& a, const UiMargin& b)
    {
        return !a.Equals(b);
    }

public:
    /** Outer margin on the left side of the rectangle
    */
    int32_t left;

    /** Outer margin above the rectangle
    */
    int32_t top;

    /** Outer margin on the right side of the rectangle
    */
    int32_t right;

    /** Outer margin below the rectangle
    */
    int32_t bottom;
};

/** 16-bit unsigned version, used for storage to save space
*/
class DUI_API UiMargin16
{
public:
    UiMargin16()
    {
        left = top = right = bottom = 0;
    }
    UiMargin16(uint16_t nLeft, uint16_t nTop, uint16_t nRight, uint16_t nBottom)
    {
        left = nLeft;
        top = nTop;
        right = nRight;
        bottom = nBottom;
    }
public:
    /** Outer margin on the left side of the rectangle
    */
    uint16_t left;

    /** Outer margin above the rectangle
    */
    uint16_t top;

    /** Outer margin on the right side of the rectangle
    */
    uint16_t right;

    /** Outer margin below the rectangle
    */
    uint16_t bottom;
};

} // namespace ui

#endif // UI_CORE_UIMARGIN_H_
