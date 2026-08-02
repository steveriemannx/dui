#include "duilib/Image/ImageDecoderFactory.h"
#include "duilib/Utils/PerformanceUtil.h"

namespace ui
{
ImageDecoderFactory::ImageDecoderFactory()
{
}

ImageDecoderFactory::~ImageDecoderFactory()
{
}

bool ImageDecoderFactory::AddImageDecoder(const std::shared_ptr<IImageDecoder>& pImageDecoder)
{
    ASSERT(pImageDecoder != nullptr);
    if (pImageDecoder == nullptr) {
        return false;
    }
    auto iter = std::find(m_imageDecoders.begin(), m_imageDecoders.end(), pImageDecoder);
    if (iter == m_imageDecoders.end()) {
        m_imageDecoders.push_back(pImageDecoder);
    }
    return false;
}

bool ImageDecoderFactory::RemoveImageDecoder(const std::shared_ptr<IImageDecoder>& pImageDecoder)
{
    ASSERT(pImageDecoder != nullptr);
    if (pImageDecoder == nullptr) {
        return false;
    }
    auto iter = std::find(m_imageDecoders.begin(), m_imageDecoders.end(), pImageDecoder);
    if (iter != m_imageDecoders.end()) {
        m_imageDecoders.erase(iter);
        return true;
    }
    return false;
}

void ImageDecoderFactory::Clear()
{
    m_imageDecoders.clear();
}

std::unique_ptr<IImage> ImageDecoderFactory::LoadImageData(const ImageDecodeParam& decodeParam)
{
    PerformanceStat statPerformance(_T("ImageDecoderFactory::LoadImageData"));
    const bool bHasFileData = (decodeParam.m_pFileData != nullptr) && !decodeParam.m_pFileData->empty(); //Image file data
    const DString imageFilePath = decodeParam.m_imageFilePath.NativePath(); //Image file path
    ASSERT(!imageFilePath.empty() || bHasFileData);
    if (imageFilePath.empty() && !bHasFileData) {
        return nullptr;
    }

    //File header data, used for signature verification of the image format
    const std::vector<uint8_t>& signatureData = bHasFileData ? *decodeParam.m_pFileData : decodeParam.m_fileHeaderData;
    std::vector<std::shared_ptr<IImageDecoder>> untriedDecoders;//Decoders not yet tried

    std::unique_ptr<IImage> pImageData;
    for (std::shared_ptr<IImageDecoder> pImageDecoder : m_imageDecoders) {
        ASSERT(pImageDecoder != nullptr);
        if (pImageDecoder == nullptr) {
            continue;
        }
        bool bCanDecode = false;
        if (!imageFilePath.empty()) {
            //The file name is not empty; match the decoder by the file extension first
            if (pImageDecoder->CanDecode(imageFilePath)) {
                if (signatureData.empty()) {
                    bCanDecode = true;
                }
                else if (pImageDecoder->CanDecode(signatureData.data(), signatureData.size())) {
                    bCanDecode = true;
                }
            }
        }
        else if (!signatureData.empty()) {
            //The file name is empty; match the decoder by the file data signature
            if (pImageDecoder->CanDecode(signatureData.data(), signatureData.size())) {
                bCanDecode = true;
            }
        }
        if (bCanDecode) {
            pImageData = pImageDecoder->LoadImageData(decodeParam);
            if (pImageData != nullptr) {
                break;
            }
            else if (bHasFileData && decodeParam.m_pFileData->empty()) {
                //An error occurred while loading the image file with the file data; no more attempts (usually the image file data is problematic)
                return nullptr;
            }
        }
        else {
            untriedDecoders.push_back(pImageDecoder);
        }
    }
    if ((pImageData == nullptr) && !signatureData.empty() && !untriedDecoders.empty()) {
        //If the decoder cannot be matched by the extension, try loading by the image data stream (so that the image can be loaded correctly even when the extension is inconsistent with the file format)
        for (std::shared_ptr<IImageDecoder> pImageDecoder : untriedDecoders) {
            if (pImageDecoder->CanDecode(signatureData.data(), signatureData.size())) {
                pImageData = pImageDecoder->LoadImageData(decodeParam);
                if (pImageData != nullptr) {
                    break;
                }
                else if (bHasFileData && decodeParam.m_pFileData->empty()) {
                    //An error occurred while loading the image file with the file data; no more attempts (usually the image file data is problematic)
                    break;
                }
            }
        }
    }
    return pImageData;
}

std::shared_ptr<IBitmap> ImageDecoderFactory::DecodeImageData(const ImageDecodeParam& decodeParam)
{
    std::shared_ptr<IBitmap> pBitmap;
    ImageDecodeParam newDecodeParam = decodeParam;
    newDecodeParam.m_bLoadAllFrames = false;//Only load a single-frame image, multi-frame is not supported
    newDecodeParam.m_bAsyncDecode = false;  //Asynchronous thread decoding is not supported
    std::shared_ptr<IImage> pImage = LoadImageData(newDecodeParam);
    ASSERT(pImage != nullptr);
    if (pImage != nullptr) {
        ASSERT((pImage->GetImageType() == ImageType::kImageBitmap) || (pImage->GetImageType() == ImageType::kImageSvg));
        if (pImage->GetImageType() == ImageType::kImageBitmap) {
            std::shared_ptr<IBitmapImage> pBitmapImage = pImage->GetImageBitmap();
            ASSERT(pBitmapImage != nullptr);
            if (pBitmapImage != nullptr) {
                pBitmap = pBitmapImage->GetBitmap(nullptr);
            }
        }
        else if (pImage->GetImageType() == ImageType::kImageSvg) {
            std::shared_ptr<ISvgImage> pSvgImage = pImage->GetImageSvg();
            ASSERT(pSvgImage != nullptr);
            if (pSvgImage != nullptr) {
                UiSize szImageSize(pImage->GetWidth(), pImage->GetHeight());
                float fRealScaleX = 1.0f;
                float fRealScaleY = 1.0f;
                if ((newDecodeParam.m_rcMaxDestRectSize.cx > 0) && (pImage->GetWidth() > 0)) {
                    fRealScaleX = (float)newDecodeParam.m_rcMaxDestRectSize.cx / (float)pImage->GetWidth();
                }
                if (newDecodeParam.m_rcMaxDestRectSize.cy > 0) {
                    fRealScaleY = (float)newDecodeParam.m_rcMaxDestRectSize.cy / (float)pImage->GetHeight();
                }
                float fRealScale = std::max(fRealScaleX, fRealScaleY);
                if (!ui::IsFloatEqual(fRealScale, 1.0f)) {
                    szImageSize.cx = (int32_t)std::round(szImageSize.cx * fRealScale);
                    szImageSize.cy = (int32_t)std::round(szImageSize.cy * fRealScale);
                }
                pBitmap = pSvgImage->GetBitmap(szImageSize);
            }
        }
    }
    return pBitmap;
}

} // namespace ui
