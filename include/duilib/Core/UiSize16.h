#ifndef UI_CORE_UISIZE16_H_
#define UI_CORE_UISIZE16_H_

#include "duilib/duilib_defs.h"
#include <cstdint>

namespace ui 
{

/** Wrapper for Size
*/
class DUILIB_API UiSize16
{
public:
    UiSize16()
    {
        cx = cy = 0;
    }

    UiSize16(const UiSize16& src)
    {
        cx = src.cx;
        cy = src.cy;
    }

    UiSize16(int16_t cxValue, int16_t cyValue)
    {
        cx = cxValue;
        cy = cyValue;
    }

    UiSize16& operator=(const UiSize16& src)
    {
        cx = src.cx;
        cy = src.cy;
        return *this;
    }

    /** Set a new width and height
    */
    void SetWidthHeight(int16_t cxValue, int16_t cyValue)
    {
        cx = cxValue;
        cy = cyValue;
    }

    /** Set a new width
    */
    void SetWidth(int16_t cxValue) { cx = cxValue; }

    /** Set a new height
    */
    void SetHeight(int16_t cyValue) { cy = cyValue; }

    /** Return the width
    */
    constexpr int16_t Width() const { return cx; }

    /** Return the height
    */
    constexpr int16_t Height() const { return cy; }

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
    bool Equals(int16_t cxValue, int16_t cyValue) const
    {
        return cx == cxValue && cy == cyValue;
    }

    /** Determine whether it is the same as another Size
    */
    bool Equals(const UiSize16& dst) const
    {
        return cx == dst.cx && cy == dst.cy;
    }

    /** Determine whether the two values are equal
    */
    friend bool operator == (const UiSize16& a, const UiSize16& b)
    {
        return a.Equals(b);
    }

    /** Determine whether the two values are not equal
    */
    friend bool operator != (const UiSize16& a, const UiSize16& b)
    {
        return !a.Equals(b);
    }

public:
    /** Width
    */
    int16_t cx;

    /** Height
    */
    int16_t cy;
};

} // namespace ui

#endif // UI_CORE_UISIZE16_H_
