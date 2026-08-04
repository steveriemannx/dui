#ifndef UI_CORE_UISIZE_H_
#define UI_CORE_UISIZE_H_

#include "dui/Core/UiSize64.h"
#include "dui/Core/UiSize16.h"

namespace ui 
{

/** Wrapper for Size
*/
class DUI_API UiSize
{
public:
    UiSize()
    {
        cx = cy = 0;
    }

    UiSize(const UiSize& src)
    {
        cx = src.cx;
        cy = src.cy;
    }

    UiSize(int32_t cxValue, int32_t cyValue)
    {
        cx = cxValue;
        cy = cyValue;
    }

    UiSize& operator=(const UiSize& src)
    {
        cx = src.cx;
        cy = src.cy;
        return *this;
    }

    /** Set a new width and height
    */
    void SetWidthHeight(int32_t cxValue, int32_t cyValue)
    {
        cx = cxValue;
        cy = cyValue;
    }

    /** Set a new width
    */
    void SetWidth(int32_t cxValue) { cx = cxValue; }

    /** Set a new height
    */
    void SetHeight(int32_t cyValue) { cy = cyValue; }

    /** Return the width
    */
    constexpr int32_t Width() const { return cx; }

    /** Return the height
    */
    constexpr int32_t Height() const { return cy; }

    /** Check whether the width and height values are valid, and correct them
    */
    void Validate()
    {
        if (cx < 0) {
            cx = 0;
        }
        if (cy < 0) {
            cy = 0;
        }
    }

    /** Whether the height and width values are valid
    */
    bool IsValid() const { return cx >= 0 && cy >= 0; }

    /** Return true if both cx and cy are 0
    */
    bool IsEmpty() const { return cx == 0 && cy == 0; }

    /** Set both cx and cy to 0
    */
    void Clear() 
    { 
        cx = 0; 
        cy = 0; 
    }

    /** Determine whether it is the same as another Size
    */
    bool Equals(int32_t cxValue, int32_t cyValue) const
    {
        return cx == cxValue && cy == cyValue;
    }

    /** Determine whether it is the same as another Size
    */
    bool Equals(const UiSize& dst) const
    {
        return cx == dst.cx && cy == dst.cy;
    }

    /** Determine whether the two values are equal
    */
    friend bool operator == (const UiSize& a, const UiSize& b)
    {
        return a.Equals(b);
    }

    /** Determine whether the two values are not equal
    */
    friend bool operator != (const UiSize& a, const UiSize& b)
    {
        return !a.Equals(b);
    }

public:
    /** Width
    */
    int32_t cx;

    /** Height
    */
    int32_t cy;
};

} // namespace ui

#endif // UI_CORE_UISIZE_H_
