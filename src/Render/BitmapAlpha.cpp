 #include "duilib/Render/BitmapAlpha.h"

namespace ui
{

BitmapAlpha::BitmapAlpha(uint8_t* pPiexl, int32_t nWidth, int32_t nHeight, int32_t nChannels):
    m_pPiexl(pPiexl),
    m_nWidth(nWidth),
    m_nHeight(nHeight),
    m_nChannels(nChannels)
{
    ASSERT(nChannels == 4);
    ASSERT(nWidth > 0);
    ASSERT(nHeight > 0);
    ASSERT(pPiexl != nullptr);
}

BitmapAlpha::~BitmapAlpha()
{
}

//This function must be executed, otherwise semi-transparent UI will malfunction; in the current callers, the alpha value passed in is always 0
void BitmapAlpha::ClearAlpha(const UiRect& rcDirty, uint8_t alpha) const
{
    ASSERT((m_pPiexl != nullptr) && (m_nChannels == 4) && (m_nWidth > 0) && (m_nHeight > 0));
    if ((m_pPiexl == nullptr) || (m_nChannels != 4) || (m_nWidth <= 0) ||(m_nHeight <= 0)){
        return;
    }
    if ((rcDirty.left == 0) && (rcDirty.top == 0) && (rcDirty.Width() == m_nWidth) && (rcDirty.Height() == m_nHeight)) {
        //Clear all
        ::memset(m_pPiexl, alpha, m_nHeight * m_nWidth * m_nChannels);
        return;
    }

    uint32_t* pBmpBits = (uint32_t*)m_pPiexl;
    int32_t nTop = std::max(rcDirty.top, 0);
    int32_t nBottom = std::min(rcDirty.bottom, m_nHeight);
    int32_t nLeft = std::max(rcDirty.left, 0);
    int32_t nRight = std::min(rcDirty.right, m_nWidth);
    if (nRight > nLeft) {
        const size_t nBytes = (size_t)(nRight - nLeft) * 4;
        for (int32_t i = nTop; i < nBottom; ++i) {
            ::memset(pBmpBits + i * m_nWidth + nLeft, alpha, nBytes);
        }
    }
}

//This function currently affects: the RichEdit control. If not called, other controls are unaffected.
void BitmapAlpha::RestoreAlpha(const UiRect& rcDirty, const UiPadding& rcShadowPadding, uint8_t alpha) const
{
    // This function applies to cases where APIs such as GDI render bitmaps, causing the alpha channel to be lost; it can restore the alpha channel
    // However, when APIs such as GDI+ and AlphaBlend set a semi-transparent alpha channel on the bitmap during rendering, the alpha channel may not be corrected properly
    ASSERT((m_pPiexl != nullptr) && (m_nChannels == 4) && (m_nWidth > 0) && (m_nHeight > 0));
    if ((m_pPiexl == nullptr) || (m_nChannels != 4) || (m_nWidth <= 0) || (m_nHeight <= 0)) {
        return;
    }

    uint32_t * pBmpBits = (uint32_t*)m_pPiexl;
    int32_t nTop = std::max(rcDirty.top, 0);
    int32_t nBottom = std::min(rcDirty.bottom, m_nHeight);
    int32_t nLeft = std::max(rcDirty.left, 0);
    int32_t nRight = std::min(rcDirty.right, m_nWidth);

    nLeft = std::max(nLeft, rcShadowPadding.left);
    nRight = std::min(nRight, m_nWidth - rcShadowPadding.right);
    nTop = std::max(nTop, rcShadowPadding.top);
    nBottom = std::min(nBottom, m_nHeight - rcShadowPadding.bottom);

    for (int32_t i = nTop; i < nBottom; i++) {
        for (int32_t j = nLeft; j < nRight; j++) {
            uint8_t* a = (uint8_t*)(pBmpBits + i * m_nWidth + j) + 3;
            // When ClearAlpha sets the alpha channel to a certain value
            // If this value has not changed, nothing was drawn on top; set alpha to 0
            if (alpha != 0 && *a == alpha) {
                *a = 0;
            }
            // If this value becomes 0, it means GDI functions such as DrawText drew on top and set alpha to 0; in this case, set alpha to 255
            else if (*a == 0) {
                *a = 255;
            }
        }
    }
}

//This function currently affects: the RichEdit control. If not called, other controls are unaffected.
void BitmapAlpha::RestoreAlpha(const UiRect& rcDirty, const UiPadding& rcShadowPadding) const
{
    // In all cases, set the alpha channel of this area to 255
    ASSERT((m_pPiexl != nullptr) && (m_nChannels == 4) && (m_nWidth > 0) && (m_nHeight > 0));
    if ((m_pPiexl == nullptr) || (m_nChannels != 4) || (m_nWidth <= 0) || (m_nHeight <= 0)) {
        return;
    }

    uint32_t* pBmpBits = (uint32_t*)m_pPiexl;
    int32_t nTop = std::max(rcDirty.top, 0);
    int32_t nBottom = std::min(rcDirty.bottom, m_nHeight);
    int32_t nLeft = std::max(rcDirty.left, 0);
    int32_t nRight = std::min(rcDirty.right, m_nWidth);

    nLeft = std::max(nLeft, rcShadowPadding.left);
    nRight = std::min(nRight, m_nWidth - rcShadowPadding.right);
    nTop = std::max(nTop, rcShadowPadding.top);
    nBottom = std::min(nBottom, m_nHeight - rcShadowPadding.bottom);

    for (int i = nTop; i < nBottom; ++i) {
        for (int j = nLeft; j < nRight; ++j) {            
            uint8_t* a = (uint8_t*)(pBmpBits + i * m_nWidth + j) + 3;
            if (*a != 255) {
                *a = 255;
            }
        }
    }
}

}
