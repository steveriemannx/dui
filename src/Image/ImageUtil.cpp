#include "dui/Image/ImageUtil.h"
#include "dui/Render/IRender.h"
#include "dui/Core/GlobalManager.h"
#include <cmath>

#pragma warning (push)
#pragma warning (disable: 4505)
    #define STBIR_DEFAULT_FILTER_UPSAMPLE STBIR_FILTER_TRIANGLE
    #include "third_party/stb_image/stb_image_resize2.h"
#pragma warning (pop)

namespace ui
{

bool ImageUtil::NeedResizeImage(float fImageSizeScale)
{
    if (fImageSizeScale < 0.001f) {
        return false;
    }
    else if (std::fabs(fImageSizeScale - 1.0f) < 0.001f) {
        return false;
    }
    return true;
}

bool ImageUtil::IsValidImageScale(float fImageSizeScale)
{
    if (fImageSizeScale < 0.001f) {
        return false;
    }
    return true;
}

uint32_t ImageUtil::GetScaledImageSize(uint32_t nImageSize, float fImageSizeScale)
{
    if (NeedResizeImage(fImageSizeScale)) {
        nImageSize = static_cast<uint32_t>(std::ceil(nImageSize * fImageSizeScale));
    }
    return nImageSize;
}

int32_t ImageUtil::GetScaledImageOffset(int32_t nImageOffset, float fImageSizeScale)
{
    if (NeedResizeImage(fImageSizeScale)) {
        //Round after calculation
        nImageOffset = static_cast<int32_t>(std::ceil(nImageOffset * fImageSizeScale));
    }
    return nImageOffset;
}

bool ImageUtil::IsSameImageScale(float fImageSizeScale1, float fImageSizeScale2)
{
    if (std::fabs(fImageSizeScale1 - fImageSizeScale2) < 0.001f) {
        return true;
    }
    return false;
}

uint32_t ImageUtil::GetScaledImageSize(uint32_t nImageSize, uint32_t nNewDpiScale, uint32_t nOrgDpiScale)
{
    if ((nNewDpiScale != nOrgDpiScale) && (nOrgDpiScale != 0)) {
        nImageSize = static_cast<uint32_t>(std::ceil(nImageSize * static_cast<float>(nNewDpiScale) / static_cast<float>(nOrgDpiScale)));
    }
    return nImageSize;
}

bool ImageUtil::FlipPixelBits(uint8_t* pPixelBits, size_t nPixelBitsLen, uint32_t nWidth, uint32_t nHeight)
{
    ASSERT((pPixelBits != nullptr) && (nPixelBitsLen > 0));
    if ((pPixelBits == nullptr) || (nPixelBitsLen == 0)) {
        return false;
    }
    ASSERT((nWidth > 0) && (nHeight > 0));
    if ((nWidth == 0) || (nHeight == 0)) {
        return false;
    }
    const size_t expectedLen = static_cast<size_t>(nHeight) * nWidth * 4;
    ASSERT(nPixelBitsLen == expectedLen);
    if (nPixelBitsLen != expectedLen) {
        return false;
    }

    const uint32_t rowBytes = nWidth * 4;
    std::vector<uint8_t> tempRow(rowBytes); //Temporary row buffer

    // Swap by rows
    for (uint32_t i = 0; i < nHeight / 2; ++i) {
        uint8_t* topRow = pPixelBits + i * rowBytes;
        uint8_t* bottomRow = pPixelBits + (nHeight - 1 - i) * rowBytes;

        // Copy and swap the whole row
        memcpy(tempRow.data(), topRow, rowBytes);
        memcpy(topRow, bottomRow, rowBytes);
        memcpy(bottomRow, tempRow.data(), rowBytes);
    }
    return true;
}

bool ImageUtil::ResizeImageData(const uint8_t* pPixelBits, size_t nPixelBitsLen, uint32_t nWidth, uint32_t nHeight,
                                float fImageSizeScale,
                                std::vector<uint8_t>& outPixelBits, uint32_t& nNewWidth, uint32_t& nNewHeight)
{
    ASSERT((pPixelBits != nullptr) && (nPixelBitsLen > 0));
    if ((pPixelBits == nullptr) || (nPixelBitsLen == 0)) {
        return false;
    }
    ASSERT((nWidth > 0) && (nHeight > 0));
    if ((nWidth == 0) || (nHeight == 0)) {
        return false;
    }
    const size_t expectedLen = static_cast<size_t>(nHeight) * nWidth * 4;
    ASSERT(nPixelBitsLen == expectedLen);
    if (nPixelBitsLen != expectedLen) {
        return false;
    }

    nNewWidth = GetScaledImageSize(nWidth, fImageSizeScale);
    nNewHeight = GetScaledImageSize(nHeight, fImageSizeScale);
    size_t nOutPixelBitsLen = static_cast<size_t>(nNewHeight) * nNewWidth * 4;
    outPixelBits.resize(nOutPixelBitsLen);
    uint8_t* pOutPixelBits = outPixelBits.data();
    return ResizeImageData(pPixelBits, nPixelBitsLen, nWidth, nHeight,
                           pOutPixelBits, nOutPixelBitsLen, nNewWidth, nNewHeight);
}

bool ImageUtil::ResizeImageData(const uint8_t* pPixelBits, size_t nPixelBitsLen, uint32_t nWidth, uint32_t nHeight,
                                uint8_t* pOutPixelBits, size_t nOutPixelBitsLen, uint32_t nNewWidth, uint32_t nNewHeight)
{
    ASSERT((pPixelBits != nullptr) && (nPixelBitsLen > 0));
    if ((pPixelBits == nullptr) || (nPixelBitsLen == 0)) {
        return false;
    }
    ASSERT((nWidth > 0) && (nHeight > 0));
    if ((nWidth == 0) || (nHeight == 0)) {
        return false;
    }
    const size_t expectedLen = static_cast<size_t>(nHeight) * nWidth * 4;
    ASSERT(nPixelBitsLen == expectedLen);
    if (nPixelBitsLen != expectedLen) {
        return false;
    }

    ASSERT((pOutPixelBits != nullptr) && (nOutPixelBitsLen > 0));
    if ((pOutPixelBits == nullptr) || (nOutPixelBitsLen == 0)) {
        return false;
    }
    ASSERT((nNewWidth > 0) && (nNewHeight > 0));
    if ((nNewWidth == 0) || (nNewHeight == 0)) {
        return false;
    }
    const size_t outExpectedLen = static_cast<size_t>(nNewHeight) * nNewWidth * 4;
    ASSERT(nOutPixelBitsLen == outExpectedLen);
    if (nOutPixelBitsLen != outExpectedLen) {
        return false;
    }

    if ((nWidth == nNewWidth) && (nHeight == nNewHeight)) {
        memcpy(pOutPixelBits, pPixelBits, outExpectedLen);
        return true;
    }
    //Perform the resize operation on the image data
    if (1) {
        //Implemented with stb_image
        const unsigned char* input_pixels = (const unsigned char*)pPixelBits;
        int input_w = (int)nWidth;
        int input_h = (int)nHeight;
        int input_stride_in_bytes = 0;
        unsigned char* output_pixels = (unsigned char*)pOutPixelBits;
        int output_w = (int)nNewWidth;
        int output_h = (int)nNewHeight;
        int output_stride_in_bytes = 0;
        stbir_pixel_layout num_channels = STBIR_RGBA;
        unsigned char* result = stbir_resize_uint8_linear(input_pixels, input_w, input_h, input_stride_in_bytes,
                                                          output_pixels, output_w, output_h, output_stride_in_bytes,
                                                          num_channels);
        return result != nullptr;
    }
    return false;
}

std::unique_ptr<IBitmap> ImageUtil::ResizeImageBitmap(IBitmap* pBitmap, int32_t nNewWidth, int32_t nNewHeight)
{
    if ((pBitmap == nullptr) || (pBitmap->GetWidth() < 1) || (pBitmap->GetHeight() < 1)) {
        return nullptr;
    }
    if ((nNewWidth < 1) || (nNewHeight < 1)) {
        return nullptr;
    }

    std::unique_ptr<IBitmap> pNewBitmap;
    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    ASSERT(pRenderFactory != nullptr);
    if (pRenderFactory != nullptr) {
        pNewBitmap.reset(pRenderFactory->CreateBitmap());
    }
    ASSERT(pNewBitmap != nullptr);
    if (pNewBitmap == nullptr) {
        return nullptr;
    }
    if (!pNewBitmap->Init((uint32_t)nNewWidth, (uint32_t)nNewHeight, nullptr)) {
        ASSERT(0);
        return nullptr;
    }
    void* pOutPixelBits = pNewBitmap->LockPixelBits();
    size_t nOutPixelBitsLen = (size_t)(nNewHeight * nNewWidth * 4);

    void* pPixelBits = pBitmap->LockPixelBits();
    uint32_t nWidth = pBitmap->GetWidth();
    uint32_t nHeight = pBitmap->GetHeight();
    size_t nPixelBitsLen = nHeight * nWidth * 4;

    if (ResizeImageData((const uint8_t*)pPixelBits, nPixelBitsLen, nWidth, nHeight,
                        (uint8_t*)pOutPixelBits, nOutPixelBitsLen, (uint32_t)nNewWidth, (uint32_t)nNewHeight)) {
        pNewBitmap->UnLockPixelBits();
    }
    else {
        pNewBitmap.reset();
    }
    ASSERT(pNewBitmap != nullptr);
    return pNewBitmap;
}

bool ImageUtil::GetBestImageScale(const UiSize& rcMaxDestRectSize,
                                  int32_t nImageWidth,
                                  int32_t nImageHeight,
                                  float fMaxScale,
                                  float& fScale)
{
    if ((nImageWidth <= 0) || (nImageHeight <= 0)) {
        return false;
    }
    if ((rcMaxDestRectSize.cx > 0) && (rcMaxDestRectSize.cy > 0)) {
        //Both the width and height are set
        if ((nImageWidth <= rcMaxDestRectSize.cx) || (nImageHeight <= rcMaxDestRectSize.cy)) {
            //When the image is smaller than the destination area, calculate an appropriate scaling ratio (enlarge moderately)
            float fScaleX = static_cast<float>(rcMaxDestRectSize.cx) / nImageWidth;
            float fScaleY = static_cast<float>(rcMaxDestRectSize.cy) / nImageHeight;
            fScale = std::max(fScaleX, fScaleY);
        }
        else {
            //When the image is larger than the destination area, calculate an appropriate scaling ratio (shrink moderately)
            float fScaleX = static_cast<float>(rcMaxDestRectSize.cx) / nImageWidth;
            float fScaleY = static_cast<float>(rcMaxDestRectSize.cy) / nImageHeight;
            fScale = std::min(fScaleX, fScaleY);
        }
    }
    else if (rcMaxDestRectSize.cx > 0) {
        //Only the width is set
        fScale = static_cast<float>(rcMaxDestRectSize.cx) / nImageWidth;
    }
    else if (rcMaxDestRectSize.cy > 0) {
        //Only the height is set
        fScale = static_cast<float>(rcMaxDestRectSize.cy) / nImageHeight;
    }
    else {
        return false;
    }

    if (fMaxScale > 0.99f) {
        //Limit the maximum value
        if (fScale > fMaxScale) {
            fScale = fMaxScale;
        }
    }

    //Limit the minimum value to avoid too-small scaling
    const float fMinScale = 1.0f / 16;
    if (fScale < fMinScale) {
        fScale = fMinScale;
    }
    const int32_t minImageSize = GlobalManager::Instance().Dpi().GetScaleInt(12);
    if ((int32_t)(nImageWidth * fScale) < minImageSize) {
        fScale = static_cast<float>(minImageSize) / nImageWidth;
    }
    if ((int32_t)(nImageHeight * fScale) < minImageSize) {
        fScale = static_cast<float>(minImageSize) / nImageHeight;
    }
    return true;
}

} //namespace ui
