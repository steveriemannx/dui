#ifndef UI_CORE_UIRECTF_H_
#define UI_CORE_UIRECTF_H_

#include "duilib/Core/UiRect.h"

namespace ui 
{
/** Wrapper for a rectangular area (floating point)
*/
class DUILIB_API UiRectF
{
public:
    /** Construct from a UiRect
    */
    static UiRectF MakeFromRect(const UiRect& rect)
    {
        return UiRectF((float)rect.left, (float)rect.top, (float)rect.right, (float)rect.bottom);
    }

public:
    UiRectF()
    {
        left = top = right = bottom = 0;
    }

    UiRectF(float nLeft, float nTop, float nRight, float nBottom)
    {
        left = nLeft;
        top = nTop;
        right = nRight;
        bottom = nBottom;
    }

    UiRectF(int32_t nLeft, int32_t nTop, int32_t nRight, int32_t nBottom)
    {
        left = (float)nLeft;
        top = (float)nTop;
        right = (float)nRight;
        bottom = (float)nBottom;
    }

    /** Get the left value
    */
    constexpr float Left() const { return left; }

    /** Get the right value
    */
    constexpr float Right() const { return right; }

    /** Get the top value
    */
    constexpr float Top() const { return top; }

    /** Get the bottom value
    */
    constexpr float Bottom() const { return bottom; }

    /** Get the width
    */
    float Width() const { return right - left; }

    /** Get the height
    */
    float Height() const { return bottom - top; }

    /** Get the X-axis coordinate of the center point
    */
    float CenterX() const { return (left + right) / 2; }

    /** Get the Y-axis coordinate of the center point
    */
    float CenterY() const { return (top + bottom) / 2; }

    /** Get the center point coordinates of the rectangle (X-axis and Y-axis coordinates)
    */
    UiPointF Center() const { return { CenterX(), CenterY() }; }

    /** Reset to zero
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

    /** Determine whether it is an empty rectangle (width and height are not valid values)
    */
    bool IsEmpty() const
    {
        float w = Width();
        float h = Height();
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
    void Offset(float cx, float cy)
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

    /** Expand the rectangular area (width increases by 2*dx, height increases by 2*dy)
    */
    void Inflate(float dx, float dy)
    {
        left -= dx;
        right += dx;
        top -= dy;
        bottom += dy;
    }

    /** Expand the rectangular area
    * @param [in] margin The four-side parameters of the margin
    */
    void Inflate(const UiMargin& margin)
    {
        left -= margin.left;
        right += margin.right;
        top -= margin.top;
        bottom += margin.bottom;
    }

    /** Expand the rectangular area
    * @param [in] padding The four-side parameters of the padding
    */
    void Inflate(const UiPadding& padding)
    {
        left -= padding.left;
        right += padding.right;
        top -= padding.top;
        bottom += padding.bottom;
    }

    /** Expand the rectangular area
    * @param nLeft The expansion value for left
    * @param nTop The expansion value for top
    * @param nRight The expansion value for right
    * @param nBottom The expansion value for bottom
    */
    void Inflate(float nLeft, float nTop, float nRight, float nBottom)
    {
        left -= nLeft;
        right += nRight;
        top -= nTop;
        bottom += nBottom;
    }

    /** Shrink the rectangular area (width decreases by 2*dx, height decreases by 2*dy)
    */
    void Deflate(float dx, float dy)
    {
        left += dx;
        right -= dx;
        top += dy;
        bottom -= dy;
    }

    /** Shrink the rectangular area
    * @param padding The four-side parameters of the padding
    */
    void Deflate(const UiPadding& padding)
    {
        left += padding.left;
        right -= padding.right;
        top += padding.top;
        bottom -= padding.bottom;
    }

    /** Shrink the rectangular area
    * @param margin The four-side parameters of the margin
    */
    void Deflate(const UiMargin& margin)
    {
        left += margin.left;
        right -= margin.right;
        top += margin.top;
        bottom -= margin.bottom;
    }

    /** Shrink the rectangular area
    * @param nLeft The shrink value for left
    * @param nTop The shrink value for top
    * @param nRight The shrink value for right
    * @param nBottom The shrink value for bottom
    */
    void Deflate(float nLeft, float nTop, float nRight, float nBottom)
    {
        left += nLeft;
        right -= nRight;
        top += nTop;
        bottom -= nBottom;
    }

    /** Get the union of two rectangles
    * @param [in] r The other rectangle
    * @return Returns true if the union is not an empty rectangle; returns false if the union is an empty rectangle
    */
    bool Union(const UiRectF& r)
    {
        if (r.left >= r.right || r.top >= r.bottom) {
            //r is an empty rectangle
            return !IsEmpty();
        }
        if (left >= right || top >= bottom) {
            //This rectangle is empty
            *this = r;
        }
        else {
            //Neither is an empty rectangle
            left   = (std::min)(left, r.left);
            top    = (std::min)(top, r.top);
            right  = (std::max)(right, r.right);
            bottom = (std::max)(bottom, r.bottom);
        }
        return !IsEmpty();
    }

    /** Get the intersection of two rectangles
    * @param [in] r The other rectangle
    * @return Returns true if the intersection is not an empty rectangle; returns false if the intersection is an empty rectangle
    */
    bool Intersect(const UiRectF& r)
    {
        left = (std::max)(left, r.Left());
        top = (std::max)(top, r.Top());
        right = (std::min)(right, r.Right());
        bottom = (std::min)(bottom, r.Bottom());
        return !IsEmpty();
    }

    /** Get the intersection of two rectangles (a, b), generating a new rectangle c from the intersection
    * @return Returns true if the intersection c is not an empty rectangle; returns false if the intersection c is an empty rectangle
    */
    static bool Intersect(UiRectF& c,const UiRectF& a, const UiRectF& b)
    {
        c.left = (std::max)(a.Left(), b.Left());
        c.top = (std::max)(a.Top(), b.Top());
        c.right = (std::min)(a.Right(), b.Right());
        c.bottom = (std::min)(a.Bottom(), b.Bottom());
        return !c.IsEmpty();
    }

    /** Determine whether a point is inside the rectangle
    */
    bool ContainsPt(const UiPoint& pt) const
    {
        return pt.X() >= left && pt.X() < right && pt.Y() >= top && pt.Y() < bottom;
    }

    /** Determine whether a point is inside the rectangle
    */
    bool ContainsPt(float x, float y) const
    {
        return x >= left && x < right && y >= top && y < bottom;
    }

    /** Determine whether it contains another rectangle
    */
    bool ContainsRect(const UiRectF& r) const
    {
        return  !r.IsEmpty() && !this->IsEmpty() &&
                left <= r.left && top <= r.top   &&
                right >= r.right && bottom >= r.bottom;
    }

    /** Determine whether it is the same as another rectangle
    */
    bool Equals(const UiRectF& r) const
    {
        return left == r.left && top == r.top &&
               right == r.right && bottom == r.bottom;
    }

    /** Determine whether two values are equal
    */
    friend bool operator == (const UiRectF& a, const UiRectF& b)
    {
        return a.Equals(b);
    }

    /** Determine whether two values are not equal
    */
    friend bool operator != (const UiRectF& a, const UiRectF& b)
    {
        return !a.Equals(b);
    }

public:
    /** The X-axis coordinate of the top-left corner
    */
    float left;

    /** The Y-axis coordinate of the top-left corner
    */
    float top;

    /** The X-axis coordinate of the bottom-right corner
    */
    float right;

    /** The Y-axis coordinate of the bottom-right corner
    */
    float bottom;
};

} // namespace ui

#endif // UI_CORE_UIRECTF_H_
