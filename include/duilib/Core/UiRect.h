#ifndef UI_CORE_UIRECT_H_
#define UI_CORE_UIRECT_H_

#include "duilib/Core/UiPoint.h"
#include "duilib/Core/UiSize.h"
#include "duilib/Core/UiPadding.h"
#include "duilib/Core/UiMargin.h"
#include <algorithm>

namespace ui 
{
/** Wrapper for a rectangular area
*/
class DUILIB_API UiRect
{
public:
    UiRect()
    {
        left = top = right = bottom = 0;
    }

    UiRect(int32_t nLeft, int32_t nTop, int32_t nRight, int32_t nBottom)
    {
        left = nLeft;
        top = nTop;
        right = nRight;
        bottom = nBottom;
    }

    /** Get the left value
    */
    constexpr int32_t Left() const { return left; }

    /** Get the right value
    */
    constexpr int32_t Right() const { return right; }

    /** Get the top value
    */
    constexpr int32_t Top() const { return top; }

    /** Get the bottom value
    */
    constexpr int32_t Bottom() const { return bottom; }

    /** Get the width
    */
    int32_t Width() const { return right - left; }

    /** Get the height
    */
    int32_t Height() const { return bottom - top; }

    /** Get the X-axis coordinate of the center point
    */
    int32_t CenterX() const { return (left + right) / 2; }

    /** Get the Y-axis coordinate of the center point
    */
    int32_t CenterY() const { return (top + bottom) / 2; }

    /** Get the center point coordinates of the rectangle (X-axis coordinate and Y-axis coordinate)
    */
    UiPoint Center() const { return { CenterX(), CenterY() }; }

    /** Clear to zero
    */
    void Clear()
    {
        left = top = right = bottom = 0;
    }

    /** Determine whether all values are zero
    */
    bool IsZero() const
    {
        return (left == 0) && (right == 0) && (top == 0) && (bottom == 0);
    }

    /** Determine whether it is an empty rectangle (the width and height are not valid values)
    */
    bool IsEmpty() const
    {
        int32_t w = Width();
        int32_t h = Height();
        return (w <= 0 || h <= 0);
    }

    /** Check whether the width and height values are valid, and correct them
    */
    void Validate()
    {
        if (right < left) {
            right = left;
        }
        if (bottom < top) {
            bottom = top;
        }
    }

    /** Offset operation
    */
    void Offset(int32_t cx, int32_t cy)
    {
        left += cx;
        right += cx;
        top += cy;
        bottom += cy;
    }

    /** Offset operation
    */
    void Offset(const UiPoint& offset)
    {
        left += offset.X();
        right += offset.X();
        top += offset.Y();
        bottom += offset.Y();
    }

    /** Enlarge the rectangular area (width increases by 2*dx, height increases by 2*dy)
    */
    void Inflate(int32_t dx, int32_t dy)
    {
        left -= dx;
        right += dx;
        top -= dy;
        bottom += dy;
    }

    /** Enlarge the rectangular area
    * @param [in] margin The four-side parameters of the outer margin
    */
    void Inflate(const UiMargin& margin)
    {
        left -= margin.left;
        right += margin.right;
        top -= margin.top;
        bottom += margin.bottom;
    }

    /** Enlarge the rectangular area
    * @param [in] padding The four-side parameters of the inner padding
    */
    void Inflate(const UiPadding& padding)
    {
        left -= padding.left;
        right += padding.right;
        top -= padding.top;
        bottom += padding.bottom;
    }

    /** Enlarge the rectangular area
    * @param nLeft The enlargement value of left
    * @param nTop The enlargement value of top
    * @param nRight The enlargement value of right
    * @param nBottom The enlargement value of bottom
    */
    void Inflate(int32_t nLeft, int32_t nTop, int32_t nRight, int32_t nBottom)
    {
        left -= nLeft;
        right += nRight;
        top -= nTop;
        bottom += nBottom;
    }

    /** Shrink the rectangular area (width decreases by 2*dx, height decreases by 2*dy)
    */
    void Deflate(int32_t dx, int32_t dy)
    {
        left += dx;
        right -= dx;
        top += dy;
        bottom -= dy;
    }

    /** Shrink the rectangular area
    * @param padding The four-side parameters of the inner padding
    */
    void Deflate(const UiPadding& padding)
    {
        left += padding.left;
        right -= padding.right;
        top += padding.top;
        bottom -= padding.bottom;
    }

    /** Shrink the rectangular area
    * @param margin The four-side parameters of the outer margin
    */
    void Deflate(const UiMargin& margin)
    {
        left += margin.left;
        right -= margin.right;
        top += margin.top;
        bottom -= margin.bottom;
    }

    /** Shrink the rectangular area
    * @param nLeft The shrink value of left
    * @param nTop The shrink value of top
    * @param nRight The shrink value of right
    * @param nBottom The shrink value of bottom
    */
    void Deflate(int32_t nLeft, int32_t nTop, int32_t nRight, int32_t nBottom)
    {
        left += nLeft;
        right -= nRight;
        top += nTop;
        bottom -= nBottom;
    }

    /** Take the union of two rectangles
    * @param [in] r Another rectangle
    * @return Returns true if the union is not an empty rectangle; returns false if the union is an empty rectangle
    */
    bool Union(const UiRect& r)
    {
        if (r.left >= r.right || r.top >= r.bottom) {
            // r is an empty rectangle
            return !IsEmpty();
        }
        if (left >= right || top >= bottom) {
            // This one is an empty rectangle
            *this = r;
        }
        else {
            // Neither is an empty rectangle
            left   = (std::min)(left, r.left);
            top    = (std::min)(top, r.top);
            right  = (std::max)(right, r.right);
            bottom = (std::max)(bottom, r.bottom);
        }
        return !IsEmpty();
    }

    /** Take the intersection of two rectangles
    * @param [in] r Another rectangle
    * @return Returns true if the intersection is not an empty rectangle; returns false if the intersection is an empty rectangle
    */
    bool Intersect(const UiRect& r)
    {
        left = (std::max)(left, r.Left());
        top = (std::max)(top, r.Top());
        right = (std::min)(right, r.Right());
        bottom = (std::min)(bottom, r.Bottom());
        return !IsEmpty();
    }

    /** Take the intersection of two rectangles (a, b), and generate a new rectangle c from the intersection
    * @return Returns true if the intersection c is not an empty rectangle; returns false if the intersection c is an empty rectangle
    */
    static bool Intersect(UiRect& c,const UiRect& a, const UiRect& b)
    {
        c.left = (std::max)(a.Left(), b.Left());
        c.top = (std::max)(a.Top(), b.Top());
        c.right = (std::min)(a.Right(), b.Right());
        c.bottom = (std::min)(a.Bottom(), b.Bottom());
        return !c.IsEmpty();
    }

    /** Determine whether a point is within the rectangle
    */
    bool ContainsPt(const UiPoint& pt) const
    {
        return pt.X() >= left && pt.X() < right && pt.Y() >= top && pt.Y() < bottom;
    }

    /** Determine whether a point is within the rectangle
    */
    bool ContainsPt(int32_t x, int32_t y) const
    {
        return x >= left && x < right && y >= top && y < bottom;
    }

    /** Determine whether another rectangle is contained
    */
    bool ContainsRect(const UiRect& r) const
    {
        return  !r.IsEmpty() && !this->IsEmpty() &&
                left <= r.left && top <= r.top   &&
                right >= r.right && bottom >= r.bottom;
    }

    /** Determine whether it is the same as another rectangle
    */
    bool Equals(const UiRect& r) const
    {
        return left == r.left && top == r.top &&
               right == r.right && bottom == r.bottom;
    }

    /** Determine whether the two values are equal
    */
    friend bool operator == (const UiRect& a, const UiRect& b)
    {
        return a.Equals(b);
    }

    /** Determine whether the two values are not equal
    */
    friend bool operator != (const UiRect& a, const UiRect& b)
    {
        return !a.Equals(b);
    }

public:
    /** The X-axis coordinate of the top-left corner
    */
    int32_t left;

    /** The Y-axis coordinate of the top-left corner
    */
    int32_t top;

    /** The X-axis coordinate of the bottom-right corner
    */
    int32_t right;

    /** The Y-axis coordinate of the bottom-right corner
    */
    int32_t bottom;
};

} // namespace ui

#endif // UI_CORE_UIPOINT_H_
