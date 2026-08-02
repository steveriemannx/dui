#ifndef UI_CORE_UIPADDING_H_
#define UI_CORE_UIPADDING_H_

#include "duilib/duilib_defs.h"
#include <cstdint>

namespace ui 
{
/** Data structure wrapper for the inner padding (not a rectangle; it has no width/height concept)
*   Inner padding: Padding is inside the rectangular area that contains the control.
    The padding attribute is a layout (Layout) attribute; only containers with layout characteristics, such as Box and its subclasses, have the padding attribute, and Control itself has no padding attribute
*/
class DUILIB_API UiPadding
{
public:
    UiPadding()
    {
        left = top = right = bottom = 0;
    }
    UiPadding(int32_t nLeft, int32_t nTop, int32_t nRight, int32_t nBottom)
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
    bool Equals(const UiPadding& r) const
    {
        return left == r.left && top == r.top &&
               right == r.right && bottom == r.bottom;
    }

    /** Determine whether the two values are equal
    */
    friend bool operator == (const UiPadding& a, const UiPadding& b)
    {
        return a.Equals(b);
    }

    /** Determine whether the two values are not equal
    */
    friend bool operator != (const UiPadding& a, const UiPadding& b)
    {
        return !a.Equals(b);
    }

public:
    /** Inner padding on the left side of the rectangle
    */
    int32_t left;

    /** Inner padding above the rectangle
    */
    int32_t top;

    /** Inner padding on the right side of the rectangle
    */
    int32_t right;

    /** Inner padding below the rectangle
    */
    int32_t bottom;
};

/** 16-bit unsigned version, used for storage to save space
*/
class DUILIB_API UiPadding16
{
public:
    UiPadding16()
    {
        left = top = right = bottom = 0;
    }
    UiPadding16(uint16_t nLeft, uint16_t nTop, uint16_t nRight, uint16_t nBottom)
    {
        left = nLeft;
        top = nTop;
        right = nRight;
        bottom = nBottom;
    }
public:
    /** Inner padding on the left side of the rectangle
    */
    uint16_t left;

    /** Inner padding above the rectangle
    */
    uint16_t top;

    /** Inner padding on the right side of the rectangle
    */
    uint16_t right;

    /** Inner padding below the rectangle
    */
    uint16_t bottom;
};

} // namespace ui

#endif // UI_CORE_UIPADDING_H_
