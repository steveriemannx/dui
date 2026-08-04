#include "dui/Image/ImageLoadParam.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Utils/StringUtil.h"
#include "dui/Utils/FilePath.h"
#include "dui/Utils/FilePathUtil.h"

namespace ui 
{
ImageLoadParam::ImageLoadParam():
    m_bImageDpiScaleEnabled(true),
    m_nLoadDpiScale(100),
    m_bAsyncDecode(false),
    m_bIconAsAnimation(false),
    m_nIconFrameDelayMs(1000),
    m_nIconSize(0),
    m_fPagMaxFrameRate(30.0f),
    m_bAssertEnabled(true)
{
}

ImageLoadParam::ImageLoadParam(DString srcWidth,
                               DString srcHeight,
                               bool bImageDpiScaleEnabled,
                               uint32_t nLoadDpiScale,
                               bool bAsyncDecode,
                               bool bIconAsAnimation,
                               int32_t nIconFrameDelayMs,
                               uint32_t nIconSize,
                               float fPagMaxFrameRate,
                               bool bAssertEnabled):
    m_bImageDpiScaleEnabled(bImageDpiScaleEnabled),
    m_nLoadDpiScale(nLoadDpiScale),
    m_bAsyncDecode(bAsyncDecode),
    m_bIconAsAnimation(bIconAsAnimation),
    m_nIconFrameDelayMs(nIconFrameDelayMs),
    m_nIconSize(nIconSize),
    m_fPagMaxFrameRate(fPagMaxFrameRate),
    m_bAssertEnabled(bAssertEnabled)
{
    StringUtil::Trim(srcWidth);
    StringUtil::Trim(srcHeight);
    m_srcWidth = srcWidth;
    m_srcHeight = srcHeight;
}

void ImageLoadParam::SetImageLoadPath(const ImageLoadPath& imageLoadPath)
{
    m_srcImageLoadPath = imageLoadPath;
    m_srcImageLoadPath.m_imageFullPath.NormalizeFilePath();//Path normalization
}

const ImageLoadPath& ImageLoadParam::GetImageLoadPath() const
{
    ASSERT(!m_srcImageLoadPath.m_imageFullPath.IsEmpty());
    return m_srcImageLoadPath;
}

bool ImageLoadParam::IsSvgImageFile() const
{
    if (m_srcImageLoadPath.m_imageFullPath.IsEmpty()) {
        return false;
    }
    DString fileExt = FilePathUtil::GetFileExtension(m_srcImageLoadPath.m_imageFullPath.ToString());
    StringUtil::MakeUpperString(fileExt);
    if (fileExt == _T("SVG")) {
        return true;
    }
    return false;
}

DString ImageLoadParam::GetLoadKey(uint32_t nLoadDpiScale) const
{
    //The format is (the content in brackets is optional): <image path>[@nLoadDpiScale]#IsImageDpiScaleEnabled()[$srcWidth:srcHeight]
    ASSERT(!m_srcImageLoadPath.m_imageFullPath.IsEmpty());
    DString fullPath = m_srcImageLoadPath.m_imageFullPath.ToString();
    if ((nLoadDpiScale != 0) && (nLoadDpiScale != 100)) {
        //Append the scaling percentage
        fullPath += _T("@");
        fullPath += StringUtil::UInt32ToString(nLoadDpiScale);
    }
    fullPath += IsImageDpiScaleEnabled() ? _T("#1") : _T("#0");
    if (!m_srcWidth.empty() || !m_srcHeight.empty()) {
        fullPath += _T("$");
        fullPath += m_srcWidth.c_str();
        fullPath += _T(":");
        fullPath += m_srcHeight.c_str();
    }
    return fullPath;
}

void ImageLoadParam::SetImageDpiScaleEnabled(bool bImageDpiScaleEnabled)
{
    m_bImageDpiScaleEnabled = bImageDpiScaleEnabled;
}

bool ImageLoadParam::IsImageDpiScaleEnabled() const
{
    return m_bImageDpiScaleEnabled;
}

void ImageLoadParam::SetLoadDpiScale(uint32_t nLoadDpiScale)
{
    m_nLoadDpiScale = nLoadDpiScale;    
}

uint32_t ImageLoadParam::GetLoadDpiScale() const
{
    if (m_nLoadDpiScale == 0) {
        return 100;
    }
    return m_nLoadDpiScale;
}

bool ImageLoadParam::IsIconAsAnimation() const
{
    return m_bIconAsAnimation;
}

uint32_t ImageLoadParam::GetIconSize() const
{
    return m_nIconSize;
}

int32_t ImageLoadParam::GetIconFrameDelayMs() const
{
    return m_nIconFrameDelayMs;
}

float ImageLoadParam::GetPagMaxFrameRate() const
{
    return m_fPagMaxFrameRate;
}

bool ImageLoadParam::IsAsyncDecodeEnabled() const
{
    return m_bAsyncDecode;
}

bool ImageLoadParam::IsAssertEnabled() const
{
    return m_bAssertEnabled;
}

bool ImageLoadParam::HasImageFixedSize(void) const
{
    uint32_t nImageFixedWidth = 0;
    uint32_t nImageFixedHeight = 0;
    return GetImageFixedSize(nImageFixedWidth, nImageFixedHeight);
}

bool ImageLoadParam::GetImageFixedSize(uint32_t& nImageWidth, uint32_t& nImageHeight) const
{
    if (!GetScaledFixedSize(m_srcWidth.c_str(), nImageWidth)) {
        nImageWidth = 0;
    }
    if (!GetScaledFixedSize(m_srcHeight.c_str(), nImageHeight)) {
        nImageHeight = 0;
    }
    return (nImageHeight > 0) || (nImageWidth > 0);
}

bool ImageLoadParam::GetScaledFixedSize(const DString& srcSize, uint32_t& nScaledSize) const
{
    nScaledSize = 0;
    if (!srcSize.empty()) {
        if (srcSize.back() == _T('%')) {
            //Scale according to the percentage
            nScaledSize = 0;
        }
        else {
            //Set a fixed value
            nScaledSize = StringUtil::StringToInt32(srcSize.c_str());
            if (nScaledSize > 0) {
                uint32_t nLoadDpiScale = GetLoadDpiScale();
                if ((nLoadDpiScale > 0) && (nLoadDpiScale != 100)) {
                    if (IsImageDpiScaleEnabled()) {
                        //The image supports DPI adaptation
                        nScaledSize = static_cast<uint32_t>(nScaledSize * nLoadDpiScale * 1.0 / 100.0 + 0.5);
                    }
                }
            }
        }
    }
    return nScaledSize > 0;
}

bool ImageLoadParam::HasImageFixedPercent() const
{
    float fImageFixedWidthPercent = 1.0f;
    float fImageFixedHeightPercent = 1.0f;
    return GetImageFixedPercent(fImageFixedWidthPercent, fImageFixedHeightPercent);
}

bool ImageLoadParam::GetImageFixedPercent(float& fImageWidthPercent, float& fImageHeightPercent) const
{
    bool bRetWidth = GetScaledFixedPercent(m_srcWidth.c_str(), fImageWidthPercent);
    if (!bRetWidth) {
        fImageWidthPercent = 1.0f;
    }
    bool bRetHeight = GetScaledFixedPercent(m_srcHeight.c_str(), fImageHeightPercent);
    if (!bRetHeight) {
        fImageHeightPercent = 1.0f;
    }
    return bRetWidth || bRetHeight;
}

bool ImageLoadParam::GetScaledFixedPercent(const DString& srcSize, float& fScaledPercent) const
{
    bool bRet = false;
    fScaledPercent = 1.0f;
    if (!srcSize.empty()) {
        if (srcSize.back() == _T('%')) {
            //Scale according to the percentage (the actual value needs to be divided by 100)
            double fRatio = StringUtil::StringToDouble(srcSize);            
            if (fRatio > 1) {//The minimum value is 1%
                bRet = true;                
                fRatio /= 100;
                fScaledPercent = static_cast<float>(fRatio);
            }
        }
    }
    return bRet && !ImageUtil::IsSameImageScale(fScaledPercent, 1.0f);
}

void ImageLoadParam::SetMaxDestRectSize(const UiSize& rcMaxDestRectSize)
{
    m_rcMaxDestRectSize = rcMaxDestRectSize;
}

UiSize ImageLoadParam::GetMaxDestRectSize() const
{
    return m_rcMaxDestRectSize;
}

}
