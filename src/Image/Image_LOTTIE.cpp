#include "duilib/Image/Image_LOTTIE.h"
#include "duilib/Core/GlobalManager.h"
#include "duilib/Render/IRender.h"
#include "duilib/RenderSkia/FontMgr_Skia.h"

#include "duilib/RenderSkia/SkiaHeaderBegin.h"
#include "modules/skottie/include/Skottie.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkFontMgr.h"
#include "duilib/RenderSkia/SkiaHeaderEnd.h"

namespace ui
{
//Decode the LOTTIE image data (decode one frame of the image, not including the image playback time field)
static bool DecodeImage_LOTTIE(sk_sp<skottie::Animation>& pSkAnimation,
                               uint32_t nImageWidth,
                               uint32_t nImageHeight,
                               int32_t nFrame,
                               AnimationFramePtr& frame)
{
    ASSERT(pSkAnimation != nullptr);
    if (pSkAnimation == nullptr) {
        return false;
    }
    ASSERT((nImageWidth > 0) && (nImageHeight > 0));
    if ((nImageWidth <= 0) || (nImageHeight <= 0)) {
        return false;
    }

    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    ASSERT(pRenderFactory != nullptr);
    if (pRenderFactory == nullptr) {
        return false;
    }

    //Generate the bitmap, vector-scaled
    frame.reset();
    pSkAnimation->seekFrame(static_cast<double>(nFrame));
    std::shared_ptr<IBitmap> pBitmap(pRenderFactory->CreateBitmap());
    ASSERT(pBitmap != nullptr);
    if (pBitmap == nullptr) {
        frame.reset();
        return false;
    }

    SkBitmap skBitmap;
#ifdef DUILIB_BUILD_FOR_WIN
    SkImageInfo info = SkImageInfo::Make((int32_t)nImageWidth, (int32_t)nImageHeight, SkColorType::kN32_SkColorType, SkAlphaType::kPremul_SkAlphaType);
#else
    SkImageInfo info = SkImageInfo::Make((int32_t)nImageWidth, (int32_t)nImageHeight, SkColorType::kRGBA_8888_SkColorType, SkAlphaType::kPremul_SkAlphaType);
#endif
    skBitmap.allocPixels(info);
    SkCanvas canvas(skBitmap);
    SkRect dst = SkRect::MakeLTRB(0.0f, 0.0f, (float)nImageWidth, (float)nImageHeight);
    pSkAnimation->render(&canvas, &dst);
    if (!pBitmap->Init(nImageWidth, nImageHeight, skBitmap.getPixels())) {
        pBitmap.reset();
    }
    auto pFrameData = std::make_shared<IAnimationImage::AnimationFrame>();
    pFrameData->m_nFrameIndex = nFrame;
    pFrameData->m_nOffsetX = 0; // Neither OffsetX nor OffsetY needs to be processed
    pFrameData->m_nOffsetY = 0;
    pFrameData->m_bDataPending = false;
    pFrameData->m_pBitmap = pBitmap;
    if (pBitmap == nullptr) {
        pFrameData->m_bDataError = true;
    }
    frame = pFrameData;
    return true;
}

struct Image_LOTTIE::TImpl
{
    //File data
    std::vector<uint8_t> m_fileData;

    //The handle after loading
    sk_sp<skottie::Animation> m_pSkAnimation;

    //Image width
    uint32_t m_nWidth = 0;

    //Image height
    uint32_t m_nHeight = 0;

    //Total frame count of the image
    int32_t m_nFrameCount = 0;

    //Number of playback loops
    int32_t m_nLoops = -1;

    //The playback time interval between frames, milliseconds
    int32_t m_nFrameDelayMs = IMAGE_ANIMATION_DELAY_MS_MIN;

    //Scaling ratio
    float m_fImageSizeScale = IMAGE_SIZE_SCALE_NONE;
};

Image_LOTTIE::Image_LOTTIE()
{
    m_impl = std::make_unique<TImpl>();
}

Image_LOTTIE::~Image_LOTTIE()
{
    m_impl->m_pSkAnimation.reset();
}

bool Image_LOTTIE::LoadImageFile(std::vector<uint8_t>& fileData,
                                 const FilePath& imageFilePath,
                                 float fImageSizeScale,
                                 const UiSize& rcMaxDestRectSize,
                                 bool bAssertEnabled)
{
    ASSERT(!fileData.empty() || !imageFilePath.IsEmpty());
    if (fileData.empty() && imageFilePath.IsEmpty()) {
        return false;
    }

    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    ASSERT(pRenderFactory != nullptr);
    if (pRenderFactory == nullptr) {
        return false;
    }
    IFontMgr* pFontMgr = pRenderFactory->GetFontMgr();
    ASSERT(pFontMgr != nullptr);
    if (pFontMgr == nullptr) {
        return false;
    }
    FontMgr_Skia* pFontMgrSKia = dynamic_cast<FontMgr_Skia*>(pFontMgr);
    ASSERT(pFontMgrSKia != nullptr);
    if (pFontMgrSKia == nullptr) {
        return false;
    }
    sk_sp<SkFontMgr>* pSkFontMgr = (sk_sp<SkFontMgr>*)pFontMgrSKia->GetSkiaFontMgrPtr();
    ASSERT(pSkFontMgr != nullptr);
    if (pSkFontMgr == nullptr) {
        return false;
    }

    m_impl->m_fileData.clear();
    m_impl->m_fileData.swap(fileData);
    m_impl->m_fImageSizeScale = fImageSizeScale;

    // Load the Lottie animation (also set the font manager)
    if (!m_impl->m_fileData.empty()) {
        m_impl->m_pSkAnimation = skottie::Animation::Builder().setFontManager(*pSkFontMgr).make((const char*)m_impl->m_fileData.data(), m_impl->m_fileData.size());
    }
    else {
        std::string jsonFilePath = imageFilePath.NativePathA();
        m_impl->m_pSkAnimation = skottie::Animation::Builder().setFontManager(*pSkFontMgr).makeFromFile((const char*)jsonFilePath.c_str());
    }
    if (!m_impl->m_pSkAnimation) {
        //When loading fails, the original file data needs to be restored
        m_impl->m_fileData.swap(fileData);
        return false;
    }

    SkISize imageSize = m_impl->m_pSkAnimation->size().toCeil();
    m_impl->m_nWidth = (uint32_t)imageSize.fWidth;
    m_impl->m_nHeight = (uint32_t)imageSize.fHeight;
    float fScale = fImageSizeScale;
    if (ImageUtil::GetBestImageScale(rcMaxDestRectSize, m_impl->m_nWidth, m_impl->m_nHeight, fImageSizeScale, fScale)) {
        m_impl->m_nWidth = ImageUtil::GetScaledImageSize(m_impl->m_nWidth, fScale);
        m_impl->m_nHeight = ImageUtil::GetScaledImageSize(m_impl->m_nHeight, fScale);
        m_impl->m_fImageSizeScale = fScale;
    }
    else {
        m_impl->m_nWidth = ImageUtil::GetScaledImageSize(m_impl->m_nWidth, fImageSizeScale);
        m_impl->m_nHeight = ImageUtil::GetScaledImageSize(m_impl->m_nHeight, fImageSizeScale);
    }
    m_impl->m_nFrameCount = static_cast<int32_t>(m_impl->m_pSkAnimation->duration() * m_impl->m_pSkAnimation->fps() + 0.5);

    if (bAssertEnabled) {
        ASSERT(m_impl->m_nWidth > 0);
        ASSERT(m_impl->m_nHeight > 0);
        ASSERT(m_impl->m_nFrameCount > 0);
    }
    if ((m_impl->m_nFrameCount <= 0) || ((int32_t)m_impl->m_nWidth <= 0) || ((int32_t)m_impl->m_nHeight <= 0)) {
        //When loading fails, the original file data needs to be restored
        m_impl->m_fileData.swap(fileData);
        return false;
    }
    m_impl->m_nFrameDelayMs = int32_t(m_impl->m_pSkAnimation->duration() * 1000) / m_impl->m_nFrameCount;
    if (m_impl->m_nFrameDelayMs < IMAGE_ANIMATION_DELAY_MS_MIN) {
        m_impl->m_nFrameDelayMs = IMAGE_ANIMATION_DELAY_MS_MIN;
    }
    //Loop playback is fixed to play forever, because the GIF format has no such setting
    m_impl->m_nLoops = -1;
    return true;
}

bool Image_LOTTIE::IsDelayDecodeEnabled() const
{
    //Multi-threaded decoding of the image data is not needed
    return false;
}

bool Image_LOTTIE::IsDelayDecodeFinished() const
{
    return true;
}

uint32_t Image_LOTTIE::GetDecodedFrameIndex() const
{
    if (m_impl->m_nFrameCount > 0) {
        return m_impl->m_nFrameCount - 1;
    }
    return 0;
}

bool Image_LOTTIE::DelayDecode(uint32_t /*nMinFrameIndex*/, std::function<bool(void)> /*IsAborted*/, bool* /*bDecodeError*/)
{
    //Multi-threaded decoding of the image data is not needed
    return false;
}

bool Image_LOTTIE::MergeDelayDecodeData()
{
    return false;
}

uint32_t Image_LOTTIE::GetWidth() const
{
    return m_impl->m_nWidth;
}

uint32_t Image_LOTTIE::GetHeight() const
{
    return m_impl->m_nHeight;
}

float Image_LOTTIE::GetImageSizeScale() const
{
    return m_impl->m_fImageSizeScale;
}

int32_t Image_LOTTIE::GetFrameCount() const
{
    return m_impl->m_nFrameCount;
}

int32_t Image_LOTTIE::GetLoopCount() const
{
    return m_impl->m_nLoops;
}

bool Image_LOTTIE::IsFrameDataReady(uint32_t /*nFrameIndex*/)
{
    //Real-time drawing, the data is always available
    return m_impl->m_pSkAnimation != nullptr;
}

int32_t Image_LOTTIE::GetFrameDelayMs(uint32_t /*nFrameIndex*/)
{
    //The playback interval between frames is a fixed value
    return m_impl->m_nFrameDelayMs;
}

bool Image_LOTTIE::ReadFrameData(int32_t nFrameIndex, const UiSize& szDestRectSize, AnimationFrame* pAnimationFrame)
{
    ASSERT(pAnimationFrame != nullptr);
    if (pAnimationFrame == nullptr) {
        return false;
    }
    ASSERT((nFrameIndex >= 0) && (nFrameIndex < m_impl->m_nFrameCount));
    if ((nFrameIndex < 0) || (nFrameIndex >= m_impl->m_nFrameCount)) {
        return false;
    }
    pAnimationFrame->m_bDataPending = false;
    pAnimationFrame->m_bDataError = false;

    ASSERT(m_impl->m_nFrameCount > 0);
    if (m_impl->m_nFrameCount <= 0) {
        pAnimationFrame->m_bDataError = true;
        return false;
    }

    uint32_t nImageWidth = m_impl->m_nWidth;
    uint32_t nImageHeight = m_impl->m_nHeight;
    ASSERT((nImageWidth > 0) && (nImageHeight > 0));
    if ((nImageWidth == 0) || (nImageHeight == 0)) {
        pAnimationFrame->m_bDataError = true;
        return false;
    }
    if ((szDestRectSize.cx > 0) || (szDestRectSize.cy > 0)) {
        bool bScaled = false;
        float fScale = 1.0f;
        if ((szDestRectSize.cx > 0) && (szDestRectSize.cy > 0)) {
            if ((szDestRectSize.cx < (int32_t)nImageWidth) && (szDestRectSize.cy < (int32_t)nImageHeight)) {
                float fScaleX = static_cast<float>(szDestRectSize.cx) / nImageWidth;
                float fScaleY = static_cast<float>(szDestRectSize.cy) / nImageHeight;
                fScale = std::max(fScaleX, fScaleY);
                bScaled = true;
            }
        }
        else if (szDestRectSize.cx > 0) {
            if (szDestRectSize.cx < (int32_t)nImageWidth) {
                fScale = static_cast<float>(szDestRectSize.cx) / nImageWidth;
                bScaled = true;
            }
        }
        else if (szDestRectSize.cy > 0) {
            if (szDestRectSize.cy < (int32_t)nImageHeight) {
                fScale = static_cast<float>(szDestRectSize.cy) / nImageHeight;
                bScaled = true;
            }
        }
        if (bScaled) {
            //Scale down proportionally (to improve the drawing speed)
            uint32_t nNewImageWidth = ImageUtil::GetScaledImageSize(nImageWidth, fScale);
            uint32_t nNewImageHeight = ImageUtil::GetScaledImageSize(nImageHeight, fScale);
            if ((nNewImageWidth > 0) && (nNewImageHeight > 0)) {
                nImageWidth = nNewImageWidth;
                nImageHeight = nNewImageHeight;
            }
        }
    }

    pAnimationFrame->m_bDataPending = false;
    pAnimationFrame->m_bDataError = true;
    AnimationFramePtr frame;
    if (m_impl->m_pSkAnimation != nullptr) {
        if(DecodeImage_LOTTIE(m_impl->m_pSkAnimation,
                              nImageWidth,
                              nImageHeight,
                              nFrameIndex,
                              frame)) {
            if (frame != nullptr) {
                *pAnimationFrame = *frame;
                pAnimationFrame->SetDelayMs(GetFrameDelayMs(nFrameIndex));
                pAnimationFrame->m_bDataError = false; //Confirmed success, mark no error
            }
        }
    }
    return frame != nullptr;
}

} //namespace ui
