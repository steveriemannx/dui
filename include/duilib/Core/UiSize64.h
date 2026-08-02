#ifndef UI_CORE_UISIZE64_H_
#define UI_CORE_UISIZE64_H_

#include "duilib/duilib_defs.h"
#include <cstdint>

namespace ui 
{

/** Wrapper for Size64
*/
class DUILIB_API UiSize64
{
public:
    UiSize64()
    {
        cx = cy = 0;
    }

    UiSize64(const UiSize64& src)
    {
        cx = src.cx;
        cy = src.cy;
    }

    UiSize64(int64_t cxValue, int64_t cyValue)
    {
        cx = cxValue;
        cy = cyValue;
    }

    UiSize64& operator=(const UiSize64& src)
    {
        cx = src.cx;
        cy = src.cy;
        return *this;
    }

    /** Set a new width and height
    */
    void SetWidthHeight(int64_t cxValue, int64_t cyValue)
    {
        cx = cxValue;
        cy = cyValue;
    }

    /** Set a new width
    */
    void SetWidth(int64_t cxValue) { cx = cxValue; }

    /** Set a new height
    */
    void SetHeight(int64_t cyValue) { cy = cyValue; }

    /** Return the width
    */
    constexpr int64_t Width() const { return cx; }

    /** Return the height
    */
    constexpr int64_t Height() const { return cy; }

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
    bool Equals(int64_t cxValue, int64_t cyValue) const
    {
        return cx == cxValue && cy == cyValue;
    }

    /** Determine whether it is the same as another Size
    */
    bool Equals(const UiSize64& dst) const
    {
        return cx == dst.cx && cy == dst.cy;
    }

    /** Determine whether the two values are equal
    */
    friend bool operator == (const UiSize64& a, const UiSize64& b)
    {
        return a.Equals(b);
    }

    /** Determine whether the two values are not equal
    */
    friend bool operator != (const UiSize64& a, const UiSize64& b)
    {
        return !a.Equals(b);
    }

public:
    /** Width
    */
    int64_t cx;

    /** Height
    */
    int64_t cy;
};

} // namespace ui

#endif // UI_CORE_UISIZE_H_
