#ifndef UI_RENDER_BITMAP_ALPHA_H_
#define UI_RENDER_BITMAP_ALPHA_H_

#include "duilib/Core/UiRect.h"

namespace ui
{

/** Save and restore the Alpha values of bitmap data
*/
class DUILIB_API BitmapAlpha
{
public:
    BitmapAlpha(uint8_t* pPiexl, int32_t nWidth, int32_t nHeight, int32_t nChannels);
    ~BitmapAlpha();

public:
    void ClearAlpha(const UiRect& rcDirty, uint8_t alpha) const;
    void RestoreAlpha(const UiRect& rcDirty, const UiPadding& rcShadowPadding, uint8_t alpha) const;
    void RestoreAlpha(const UiRect& rcDirty, const UiPadding& rcShadowPadding) const;

private:
    /** Image data
    */
    uint8_t* m_pPiexl;

    /** Image width
    */
    int32_t m_nWidth;

    /** Image height
    */
    int32_t m_nHeight;

    /** The number of channels; only 4 (ARGB) is currently supported
    */
    int32_t m_nChannels;
};
} // namespace ui

#endif // UI_RENDER_BITMAP_ALPHA_H_
