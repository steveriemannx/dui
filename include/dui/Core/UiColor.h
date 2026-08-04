#ifndef UI_CORE_UICOLOR_H_
#define UI_CORE_UICOLOR_H_

#include "dui/Core/UiColors.h"

namespace ui 
{
#ifndef DUI_BUILD_FOR_WIN
    typedef unsigned long COLORREF;
    #define GetRValue(rgb)      ((uint8_t)(rgb))
    #define GetGValue(rgb)      ((uint8_t)(((uint16_t)(rgb)) >> 8))
    #define GetBValue(rgb)      ((uint8_t)((rgb)>>16))
    #define RGB(r,g,b)          ((COLORREF)(((uint8_t)(r)|((uint16_t)((uint8_t)(g))<<8))|(((uint32_t)(uint8_t)(b))<<16)))
#endif

/** Wrapper for color values (ARGB format)
*/
class DUI_API UiColor
{
public:

    // Color value type
    typedef uint32_t ARGB;

    UiColor():
        Argb(0)
    {
    }

    UiColor(uint8_t r, uint8_t g, uint8_t b)
    {
        Argb = MakeARGB(255, r, g, b);
    }

    UiColor(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
    {
        Argb = MakeARGB(a, r, g, b);
    }

    explicit UiColor(ARGB argb)
    {
        Argb = argb;
    }

    explicit UiColor(int32_t argb)
    {
        Argb = (ARGB)argb;
    }

    /** Get the Alpha value of the color
    */
    uint8_t GetAlpha() const { return (uint8_t)(Argb >> AlphaShift); }

    /** Get the Alpha value of the color
    */
    uint8_t GetA() const { return (uint8_t)(Argb >> AlphaShift); }

    /** Get the Red value of the color
    */
    uint8_t GetRed() const { return (uint8_t)(Argb >> RedShift); }

    /** Get the Red value of the color
    */
    uint8_t GetR() const { return (uint8_t)(Argb >> RedShift); }

    /** Get the Green value of the color
    */
    uint8_t GetGreen() const { return (uint8_t)(Argb >> GreenShift); }

    /** Get the Green value of the color
    */
    uint8_t GetG() const { return (uint8_t)(Argb >> GreenShift); }

    /** Get the Blue value of the color
    */
    uint8_t GetBlue() const { return (uint8_t)(Argb >> BlueShift); }

    /** Get the Blue value of the color
    */
    uint8_t GetB() const { return (uint8_t)(Argb >> BlueShift); }

    /** Get the ARGB value of the color
    */
    ARGB GetARGB() const { return Argb; }

    /** Whether the color value is empty
    */
    bool IsEmpty() const { return Argb == 0; }

    /** Set the ARGB value of the color
    */
    void SetARGB(ARGB argb) { Argb = argb; }

    /** Set the color from a COLORREF color value (COLORREF only contains RGB values, without the A value)
    */
    void SetFromCOLORREF(uint32_t rgb)
    {
        Argb = MakeARGB(255, GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));
    }

    /** Convert an ARGB color to a COLORREF color value (COLORREF only contains RGB values, without the A value)
    */
    uint32_t ToCOLORREF() const
    {
        return RGB(GetRed(), GetGreen(), GetBlue());
    }

    /** Determine whether it is the same as another point
    */
    bool Equals(const UiColor& dst) const
    {
        return Argb == dst.Argb;
    }

    /** Determine whether the two values are equal
    */
    friend bool operator == (const UiColor& a, const UiColor& b)
    {
        return a.Equals(b);
    }

    /** Determine whether the two values are not equal
    */
    friend bool operator != (const UiColor& a, const UiColor& b)
    {
        return !a.Equals(b);
    }

    // Shift count and bit mask for A, R, G, B components
    enum
    {
        AlphaShift = 24,
        RedShift   = 16,
        GreenShift = 8,
        BlueShift  = 0
    };

    enum
    {
        AlphaMask = 0xff000000,
        RedMask   = 0x00ff0000,
        GreenMask = 0x0000ff00,
        BlueMask  = 0x000000ff
    };

    // Assemble A, R, G, B values into a 32-bit integer
    static ARGB MakeARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
    {
        return (((ARGB)(b) << BlueShift)  |
                ((ARGB)(g) << GreenShift) |
                ((ARGB)(r) << RedShift)   |
                ((ARGB)(a) << AlphaShift));
    }

private:
    // Color value
    ARGB Argb;
};

} // namespace ui

#endif // UI_CORE_UICOLOR_H_
