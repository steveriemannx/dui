#include "dui/Image/ImageInfo.h"
#include "dui/Image/ImageUtil.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Utils/PerformanceUtil.h"
#include <cmath>

namespace ui 
{

ImageInfo::ImageInfo():    
    m_nLoopCount(-1),
    m_nFrameCount(0),
    m_nImageInfoWidth(0),
    m_nImageInfoHeight(0),
    m_imageType(ImageType::kImageBitmap),
    m_bEnableImageDpiScale(true),
    m_bHasCustomSizeScale(false),
    m_fCustomSizeScaleX(0),
    m_fCustomSizeScaleY(0),
    m_nImageFileDpiScale(100),
    m_fImageSizeScale(1.0f)
{
}

ImageInfo::~ImageInfo()
{
    //Delayed release of the original image
    ReleaseImage();
}

void ImageInfo::ReleaseImage()
{
    if (m_pImageData != nullptr) {
        DString imageFullPath = m_loadParam.GetImageLoadPath().m_imageFullPath.ToString();
        GlobalManager::Instance().Image().ReleaseImage(m_pImageData, imageFullPath);
        m_pImageData.reset();
    }
}

bool ImageInfo::IsSvgImage() const
{
    return (m_imageType == ImageType::kImageSvg);
}

std::shared_ptr<IBitmap> ImageInfo::GetSvgBitmap(const UiRect& rcDest, UiRect& rcSource)
{
    std::shared_ptr<IImage> pImageData = m_pImageData;
    ASSERT(pImageData != nullptr);
    if (pImageData == nullptr) {
        return nullptr;
    }
    std::shared_ptr<ISvgImage> pSvgImage;
    if (m_imageType == ImageType::kImageSvg) {
        pSvgImage = pImageData->GetImageSvg();        
    }
    ASSERT(pSvgImage != nullptr);
    if (pSvgImage == nullptr) {
        return nullptr;
    }
    ASSERT((rcSource.Width() > 0) && (rcSource.Height() > 0));
    if ((rcSource.Width() <= 0) && (rcSource.Height() <= 0)) {
        return nullptr;
    }
    ASSERT((pSvgImage->GetWidth() > 0) && (pSvgImage->GetHeight() > 0));
    if ((pSvgImage->GetWidth() <= 0) && (pSvgImage->GetHeight() <= 0)) {
        return nullptr;
    }
    ASSERT((m_nImageInfoWidth > 0) && (m_nImageInfoHeight > 0));
    if ((m_nImageInfoWidth <= 0) && (m_nImageInfoHeight <= 0)) {
        return nullptr;
    }

    float fSizeScaleX = static_cast<float>(rcDest.Width()) / rcSource.Width();
    float fSizeScaleY = static_cast<float>(rcDest.Height()) / rcSource.Height();
    float fImageSizeScale = fSizeScaleX < fSizeScaleY ? fSizeScaleX  : fSizeScaleY ;
    std::shared_ptr<IBitmap> pBitmap = GetSvgBitmap(fImageSizeScale);
    return pBitmap;
}

std::shared_ptr<IBitmap> ImageInfo::GetSvgBitmap(float fImageSizeScale)
{
    GlobalManager::Instance().AssertUIThread();
    //SVG image, no cache
    std::shared_ptr<IImage> pImageData = m_pImageData;
    ASSERT(pImageData != nullptr);
    if (pImageData == nullptr) {
        return nullptr;
    }
    ASSERT((m_nImageInfoWidth > 0) && (m_nImageInfoHeight > 0));
    if ((m_nImageInfoWidth <= 0) && (m_nImageInfoHeight <= 0)) {
        return nullptr;
    }

    std::shared_ptr<IBitmap> pBitmap;
    if (m_imageType == ImageType::kImageSvg) {
        std::shared_ptr<ISvgImage> pSvgImage = pImageData->GetImageSvg();
        ASSERT(pSvgImage != nullptr);
        if (pSvgImage == nullptr) {
            return nullptr;
        }
        int32_t nWidth = (int32_t)ImageUtil::GetScaledImageSize((uint32_t)m_nImageInfoWidth, fImageSizeScale);
        int32_t nHeight = (int32_t)ImageUtil::GetScaledImageSize((uint32_t)m_nImageInfoHeight, fImageSizeScale);
        pBitmap = pSvgImage->GetBitmap(UiSize(nWidth, nHeight));
    }
    return pBitmap;
}

std::shared_ptr<IBitmap> ImageInfo::GetBitmap(bool* bDecodeError)
{
    GlobalManager::Instance().AssertUIThread();
    if (m_imageType == ImageType::kImageBitmap) {
        //Bitmap image: use the cached image first
        if (m_pBitmap != nullptr) {
            return m_pBitmap;
        }
    }
    else if (m_imageType == ImageType::kImageSvg) {
        //SVG image
        std::shared_ptr<IBitmap> pBitmap = GetSvgBitmap(IMAGE_SIZE_SCALE_NONE);
        if (pBitmap == nullptr) {
            if (bDecodeError != nullptr) {
                *bDecodeError = true;
            }
        }
        return pBitmap;
    }
    else {
        if (bDecodeError != nullptr) {
            *bDecodeError = true;
        }
        ASSERT(0);
        return nullptr;
    }

    //Set the internal data of the image (delayed decoding)
    std::shared_ptr<IImage> pImageData = m_pImageData;
    ASSERT(pImageData != nullptr);
    if (pImageData == nullptr) {
        if (bDecodeError != nullptr) {
            *bDecodeError = true;
        }
        return nullptr;
    }

    ImageType imageType = pImageData->GetImageType();
    if (imageType == ImageType::kImageBitmap) {
        std::shared_ptr<IBitmapImage> pBitmapImage = pImageData->GetImageBitmap();
        ASSERT(pBitmapImage != nullptr);
        if (pBitmapImage == nullptr) {
            if (bDecodeError != nullptr) {
                *bDecodeError = true;
            }
            return nullptr;
        }
        std::shared_ptr<IBitmap> pBitmap = pBitmapImage->GetBitmap(bDecodeError);
        if (pBitmap == nullptr) {
            return nullptr;
        }
        if (((int32_t)pBitmap->GetWidth() == m_nImageInfoWidth) && ((int32_t)pBitmap->GetHeight() == m_nImageInfoHeight)) {
            m_pBitmap = pBitmap;
        }
        else {
            //If the size is inconsistent, generate the cached bitmap, then release the original image
            m_pBitmap = ImageUtil::ResizeImageBitmap(pBitmap.get(), m_nImageInfoWidth, m_nImageInfoHeight);
            ASSERT(m_pBitmap != nullptr);
            ReleaseImage();
            if (m_pBitmap == nullptr) {
                if (bDecodeError != nullptr) {
                    *bDecodeError = true;
                }
            }
        }
        return m_pBitmap;
    }
    else {
        //Unknown error
        if (bDecodeError != nullptr) {
            *bDecodeError = true;
        }
        ASSERT(0);
    }
    return nullptr;
}

std::shared_ptr<IAnimationImage> ImageInfo::GetAnimationImage(uint32_t nFrameIndex) const
{
    GlobalManager::Instance().AssertUIThread();
    ASSERT(m_imageType == ImageType::kImageAnimation);
    if (m_imageType != ImageType::kImageAnimation) {
        //Animation image
        return nullptr;
    }
    const uint32_t nFrameCount = GetFrameCount();
    ASSERT(nFrameCount > 0);
    if (nFrameCount == 0) {
        return nullptr;
    }
    ASSERT(nFrameIndex <= nFrameCount);
    if (nFrameIndex >= nFrameCount) {
        return nullptr;
    }
    std::shared_ptr<IImage> pImageData = m_pImageData;
    ASSERT(pImageData != nullptr);
    if (pImageData == nullptr) {
        return nullptr;
    }

    std::shared_ptr<IAnimationImage> pAnimationImage;
    ImageType imageType = pImageData->GetImageType();
    if (imageType == ImageType::kImageAnimation) {
        pAnimationImage = pImageData->GetImageAnimation();
        ASSERT(pAnimationImage != nullptr);
    }
    return pAnimationImage;
}

AnimationFramePtr ImageInfo::GetFrame(uint32_t nFrameIndex, const UiSize& szDestRectSize)
{
    PerformanceStat statPerformance(_T("ImageInfo::GetFrame"));
    GlobalManager::Instance().AssertUIThread();
    std::shared_ptr<IAnimationImage> pAnimationImage = GetAnimationImage(nFrameIndex);
    ASSERT(pAnimationImage != nullptr);
    if (pAnimationImage == nullptr) {
        //Abnormal animation image interface
        return nullptr;
    }
    const uint32_t nFrameCount = GetFrameCount();
    ASSERT(nFrameCount > 0);
    if (nFrameCount == 0) {
        //Abnormal animation image frame count
        return nullptr;
    }

    AnimationFramePtr pAnimationFrame;
    pAnimationFrame = std::make_shared<IAnimationImage::AnimationFrame>();
    if (pAnimationImage->ReadFrameData(nFrameIndex, szDestRectSize, pAnimationFrame.get())) {
        pAnimationFrame->m_nFrameIndex = nFrameIndex;
        if (pAnimationFrame->m_bDataPending) {
            //The data decoding has not been finished, return directly
            ASSERT(pAnimationFrame->m_pBitmap == nullptr);
            return pAnimationFrame;
        }
        ASSERT(pAnimationFrame->GetDelayMs() > 0);
        ASSERT(pAnimationFrame->m_pBitmap != nullptr);
    }
    return pAnimationFrame;
}

bool ImageInfo::IsFrameDataReady(uint32_t nFrameIndex)
{
    GlobalManager::Instance().AssertUIThread();
    std::shared_ptr<IAnimationImage> pAnimationImage = GetAnimationImage(nFrameIndex);
    ASSERT(pAnimationImage != nullptr);
    if (pAnimationImage != nullptr) {
        return pAnimationImage->IsFrameDataReady(nFrameIndex);
    }
    return false;
}

int32_t ImageInfo::GetFrameDelayMs(uint32_t nFrameIndex)
{
    GlobalManager::Instance().AssertUIThread();
    std::shared_ptr<IAnimationImage> pAnimationImage = GetAnimationImage(nFrameIndex);
    ASSERT(pAnimationImage != nullptr);
    if (pAnimationImage != nullptr) {
        return pAnimationImage->GetFrameDelayMs(nFrameIndex);
    }
    //Avoid returning 0, otherwise the external business logic will have problems
    return IMAGE_ANIMATION_DELAY_MS;
}

bool ImageInfo::SetImageData(const ImageLoadParam& loadParam,
                             const std::shared_ptr<IImage>& pImageData,                             
                             bool bImageDpiScaleEnabled,
                             uint32_t nImageFileDpiScale)
{
    GlobalManager::Instance().AssertUIThread();
    ASSERT(pImageData != nullptr);
    if (pImageData == nullptr) {
        return false;
    }
    ASSERT(nImageFileDpiScale > 0);
    if (nImageFileDpiScale == 0) {
        return false;
    }

    //Calculate the size of the ImageInfo
    int32_t nImageInfoWidth = 0;
    int32_t nImageInfoHeight = 0;
    CalcImageInfoSize(loadParam, pImageData, bImageDpiScaleEnabled, nImageFileDpiScale, nImageInfoWidth, nImageInfoHeight);

    ASSERT((nImageInfoWidth > 0) && (nImageInfoHeight > 0));
    if ((nImageInfoWidth <= 0) || (nImageInfoHeight <= 0)) {
        return false;
    }

    const int32_t nImageWidth = pImageData->GetWidth();
    const int32_t nImageHeight = pImageData->GetWidth();
    ASSERT((nImageWidth > 0) && (nImageHeight > 0));
    if ((nImageWidth <= 0) || (nImageHeight <= 0)) {
        return false;
    }
    
    //Set the width and height
    m_nImageInfoWidth = nImageInfoWidth;
    m_nImageInfoHeight = nImageInfoHeight;
    m_fImageSizeScale = pImageData->GetImageSizeScale();
    m_bEnableImageDpiScale = bImageDpiScaleEnabled;
    m_nImageFileDpiScale = nImageFileDpiScale;

    m_bHasCustomSizeScale = false;
    m_fCustomSizeScaleX = 1.0f;
    m_fCustomSizeScaleY = 1.0f;
    if (nImageInfoWidth != nImageWidth) {
        m_fCustomSizeScaleX = static_cast<float>(nImageInfoWidth) / nImageWidth;
        m_bHasCustomSizeScale = true;
    }
    if (nImageInfoHeight != nImageHeight) {
        m_fCustomSizeScaleY = static_cast<float>(nImageInfoHeight) / nImageHeight;
        m_bHasCustomSizeScale = true;
    }

    m_loadParam = loadParam;
    m_pImageData = pImageData;    

    //Set the internal data of the image
    ImageType imageType = pImageData->GetImageType();
    m_imageType = imageType;
    if (imageType == ImageType::kImageBitmap) {
        m_nFrameCount = 1;
        m_nLoopCount = -1;
    }
    else if (imageType == ImageType::kImageSvg) {
        m_nFrameCount = 1;
        m_nLoopCount = -1;
    }
    else if (imageType == ImageType::kImageAnimation) {
        std::shared_ptr<IAnimationImage> pAnimationImage = pImageData->GetImageAnimation();
        ASSERT(pAnimationImage != nullptr);
        if (pAnimationImage == nullptr) {
            return false;
        }
        m_nFrameCount = pAnimationImage->GetFrameCount();
        m_nLoopCount = pAnimationImage->GetLoopCount();
        ASSERT(m_nFrameCount != 0);
        if (m_nFrameCount == 0) {
            return false;
        }
        if (m_nLoopCount == 0) {
            m_nLoopCount = -1;
        }
    }
    else {
        ASSERT(0);
        return false;
    }
    return true;
}

void ImageInfo::CalcImageInfoSize(const ImageLoadParam& loadParam,
                                  const std::shared_ptr<IImage>& pImageData,
                                  bool bImageDpiScaleEnabled,
                                  uint32_t nImageFileDpiScale,
                                  int32_t& nImageInfoWidth,
                                  int32_t& nImageInfoHeight) const
{
    nImageInfoWidth = 0;
    nImageInfoHeight = 0;
    ASSERT(pImageData != nullptr);
    if (pImageData == nullptr) {
        return;
    }
    ASSERT(nImageFileDpiScale > 0);
    if (nImageFileDpiScale == 0) {
        return;
    }
    if (!bImageDpiScaleEnabled) {
        ASSERT(nImageFileDpiScale == 100);
        if (nImageFileDpiScale != 100) {
            return;
        }
    }

    //Calculate the width and height of the ImageInfo (note: may be different from the actual width and height values of the image)
    //The width and height of the ImageInfo: affect the layout
    //The width and height of the image: used for drawing
    nImageInfoWidth = pImageData->GetWidth();
    nImageInfoHeight = pImageData->GetHeight();
    ASSERT((nImageInfoWidth > 0) && (nImageInfoHeight > 0));
    if ((nImageInfoWidth <= 0) || (nImageInfoHeight <= 0)) {
        return;
    }    
    if (bImageDpiScaleEnabled && (nImageFileDpiScale != 100) && ImageUtil::IsValidImageScale(pImageData->GetImageSizeScale())) {
        const float fRealImageSizeScale = pImageData->GetImageSizeScale(); //The actually loaded scaling ratio (this value may not be the same as the scaling ratio passed in when loading)
        //Example: the original image file is "autumn.png"; if the matched DPI-adaptive image file name is "autumn@175.png", then the value of nImageFileDpiScale is 175
        const float fSizeScale = static_cast<float>(loadParam.GetLoadDpiScale()) / 100.0f;
        //The DPI-adaptive image is used (not the original image), so the original image size needs to be used to calculate the ImageInfo size
        int32_t nImageOrgWidth = static_cast<int32_t>(std::ceil(nImageInfoWidth * 1.0f / fRealImageSizeScale));
        nImageOrgWidth = static_cast<int32_t>(std::ceil(nImageOrgWidth * 100.0f / nImageFileDpiScale));    //Original image size
        nImageOrgWidth = (int32_t)ImageUtil::GetScaledImageSize((uint32_t)nImageOrgWidth, fSizeScale); //Recalculate the ImageInfo size using the original image size

        int32_t nImageOrgHeight = static_cast<int32_t>(std::ceil(nImageInfoHeight * 1.0f / fRealImageSizeScale));
        nImageOrgHeight = static_cast<int32_t>(std::ceil(nImageOrgHeight * 100.0f / nImageFileDpiScale));    //Original image size
        nImageOrgHeight = (int32_t)ImageUtil::GetScaledImageSize((uint32_t)nImageOrgHeight, fSizeScale); //Recalculate the ImageInfo size using the original image size

        if ((nImageOrgWidth > 0) && (nImageOrgHeight > 0)) {
            nImageInfoWidth = nImageOrgWidth;
            nImageInfoHeight = nImageOrgHeight;
        }
    }

    //Calculate the set ratio, which affects the loaded scaling percentage (the image attribute set in the form of width='300' or width='300%')
    uint32_t nImageFixedWidth = 0;
    uint32_t nImageFixedHeight = 0;
    const bool bHasFixedSize = loadParam.GetImageFixedSize(nImageFixedWidth, nImageFixedHeight); //Absolute values, DPI adaptation has been done

    float fImageFixedWidthPercent = 1.0f;
    float fImageFixedHeightPercent = 1.0f;
    const bool bHasFixedPercent = loadParam.GetImageFixedPercent(fImageFixedWidthPercent, fImageFixedHeightPercent);//Percentage

    if (bHasFixedSize || bHasFixedPercent) {
        //The image attribute is set: the image attribute set in the form of width='300' or width='300%'
        bool bFixedWidthSet = (nImageFixedWidth > 0) || ImageUtil::NeedResizeImage(fImageFixedWidthPercent);
        bool bFixedHeightSet = (nImageFixedHeight > 0) || ImageUtil::NeedResizeImage(fImageFixedHeightPercent);
        if (bFixedWidthSet && bFixedHeightSet) {
            //Both the width and height are set
            if (nImageFixedWidth > 0) {
                nImageInfoWidth = nImageFixedWidth;
            }
            else if (ImageUtil::NeedResizeImage(fImageFixedWidthPercent)) {
                nImageInfoWidth = (int32_t)ImageUtil::GetScaledImageSize((uint32_t)nImageInfoWidth, fImageFixedWidthPercent);
            }

            if (nImageFixedHeight > 0) {
                nImageInfoHeight = nImageFixedHeight;
            }
            else if (ImageUtil::NeedResizeImage(fImageFixedHeightPercent)) {
                nImageInfoHeight = (int32_t)ImageUtil::GetScaledImageSize((uint32_t)nImageInfoHeight, fImageFixedHeightPercent);
            }
        }
        else if (bFixedWidthSet) {
            //Only the width is set, the height is scaled proportionally
            int32_t nOldImageInfoWidth = nImageInfoWidth;
            if (nImageFixedWidth > 0) {
                nImageInfoWidth = nImageFixedWidth;
            }
            else if (ImageUtil::NeedResizeImage(fImageFixedWidthPercent)) {
                nImageInfoWidth = (int32_t)ImageUtil::GetScaledImageSize((uint32_t)nImageInfoWidth, fImageFixedWidthPercent);
            }
            float fNewScale = static_cast<float>(nImageInfoWidth) / nOldImageInfoWidth;
            nImageInfoHeight = (int32_t)ImageUtil::GetScaledImageSize((uint32_t)nImageInfoHeight, fNewScale);
        }
        else if (bFixedHeightSet) {
            //Only the height is set, the width is scaled proportionally
            int32_t nOldImageInfoHeight = nImageInfoHeight;
            if (nImageFixedHeight > 0) {
                nImageInfoHeight = nImageFixedHeight;
            }
            else if (ImageUtil::NeedResizeImage(fImageFixedHeightPercent)) {
                nImageInfoHeight = (int32_t)ImageUtil::GetScaledImageSize((uint32_t)nImageInfoHeight, fImageFixedHeightPercent);
            }
            float fNewScale = static_cast<float>(nImageInfoHeight) / nOldImageInfoHeight;
            nImageInfoWidth = (int32_t)ImageUtil::GetScaledImageSize((uint32_t)nImageInfoWidth, fNewScale);
        }
    }
}

void ImageInfo::ScaleImageSourceRect(const DpiManager& dpi, UiRect& rcDestCorners, UiRect& rcSource, UiRect& rcSourceCorners)
{
    int32_t nImageInfoWidth = GetWidth();
    int32_t nImageInfoHeight = GetHeight();
    ASSERT((nImageInfoWidth > 0) && (nImageInfoHeight > 0));
    if ((nImageInfoWidth == 0) || (nImageInfoHeight == 0)) {
        return;
    }

    //Process rcSourceCorners: perform fault tolerance on the corner values (the four edges represent margins, not a rectangular area)
    //When loading through XML parsing, DPI adaptation is not done;
    //When drawing, if the image has done DPI adaptation, DPI adaptation should also be done; if the image has not done DPI adaptation, it is not needed.
    if ((rcSourceCorners.left < 0) || (rcSourceCorners.top < 0) ||
        (rcSourceCorners.right < 0) || (rcSourceCorners.bottom < 0)) {
        rcSourceCorners.Clear();
    }
    else if (m_bEnableImageDpiScale) {
        dpi.ScaleRect(rcSourceCorners);
    }

    //Process rcDestCorners: assigned from rcSourceCorners to keep the corners consistent, avoiding distortion of the four corners when drawing the image;
    //When using nine-patch drawing, the four corners exist to avoid distortion of the four corners during drawing
    rcDestCorners = rcSourceCorners;

    // If the source bitmap has been DPI-scaled, the corresponding rcImageSource also needs to be scaled
    if ((rcSource.left < 0) || (rcSource.top < 0) ||
        (rcSource.right < 0) || (rcSource.bottom < 0) ||
        (rcSource.Width() <= 0) || (rcSource.Height() <= 0)) {
        //If it is an invalid value, reset it to the whole image size
        rcSource.left = 0;
        rcSource.top = 0;
        rcSource.right = (int32_t)nImageInfoWidth;
        rcSource.bottom = (int32_t)nImageInfoHeight;
    }
    else if (m_bEnableImageDpiScale) {
        //If the value is set externally, perform DPI adaptation processing
        dpi.ScaleRect(rcSource);
    }

    //Fault tolerance processing of the image source
    if (rcSource.right > (int32_t)nImageInfoWidth) {
        rcSource.right = (int32_t)nImageInfoWidth;
    }
    if (rcSource.bottom > (int32_t)nImageInfoHeight) {
        rcSource.bottom = (int32_t)nImageInfoHeight;
    }
}

void ImageInfo::ScaleImageSourceRect(const DpiManager& dpi, UiRect& rcSource)
{
    UiRect rcDestCorners;
    UiRect rcSourceCorners;
    ScaleImageSourceRect(dpi, rcDestCorners, rcSource, rcSourceCorners);
}

std::shared_ptr<IImage> ImageInfo::GetImageData() const
{
    return m_pImageData;
}

int32_t ImageInfo::GetWidth() const
{
    return m_nImageInfoWidth;
}

int32_t ImageInfo::GetHeight() const
{
    return m_nImageInfoHeight;
}

float ImageInfo::GetImageSizeScale() const
{
    return m_fImageSizeScale;
}

uint32_t ImageInfo::GetFrameCount() const
{
    return m_nFrameCount;
}

bool ImageInfo::IsMultiFrameImage() const
{
    return GetFrameCount() > 1;
}

int32_t ImageInfo::GetLoopCount() const
{
    return m_nLoopCount;
}

DString ImageInfo::GetLoadKey() const
{
    return m_loadParam.GetLoadKey(m_loadParam.GetLoadDpiScale());
}

uint32_t ImageInfo::GetLoadDpiScale() const
{
    return m_loadParam.GetLoadDpiScale();
}

uint32_t ImageInfo::GetImageFileDpiScale() const
{
    return m_nImageFileDpiScale;
}

void ImageInfo::SetImageKey(const DString& imageKey)
{
    m_imageKey = imageKey;
}

DString ImageInfo::GetImageKey() const
{
    return m_imageKey.c_str();
}

}
