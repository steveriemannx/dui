#include "duilib/Image/Image_JPEG.h"
#include "duilib/Core/GlobalManager.h"
#include "duilib/Utils/FileUtil.h"

#ifdef DUILIB_IMAGE_SUPPORT_JPEG_TURBO

#include "duilib/Image/turbojpeg.h"

namespace ui
{
struct Image_JPEG::TImpl
{
    //Image file path
    FilePath m_imageFilePath;

    //File data
    std::vector<uint8_t> m_fileData;

    //The handle for decompressing JPEG
    tjhandle m_tjInstance = nullptr;

    //Whether asynchronous thread decoding of the image data is supported
    bool m_bAsyncDecode = false;

    //Whether a decoding error was encountered
    bool m_bDecodeError = false;

    //Whether assertions are allowed when image data errors occur
    bool m_bAssertEnabled = true;

    //Image width
    uint32_t m_nWidth = 0;

    //Image height
    uint32_t m_nHeight = 0;

    //Scaling ratio
    float m_fImageSizeScale = IMAGE_SIZE_SCALE_NONE;

    /** Bitmap data
    */
    std::shared_ptr<IBitmap> m_pBitmap;

    /** Bitmap data (delayed decoding)
    */
    std::shared_ptr<IBitmap> m_pDelayBitmap;
};

Image_JPEG::Image_JPEG()
{
    m_impl = std::make_unique<TImpl>();
}

Image_JPEG::~Image_JPEG()
{
    if (m_impl->m_tjInstance != nullptr) {
        tjDestroy(m_impl->m_tjInstance);
        m_impl->m_tjInstance = nullptr;
    }
}

// Find the value closest to fImageSizeScale (but ensure that the returned value is not less than fImageSizeScale)
static bool FindClosestScale1(const std::vector<tjscalingfactor>& scalingFactorList,
                              float fImageSizeScale,
                              tjscalingfactor& selectedScalingfactor)
{
    if (scalingFactorList.empty()) {
        return false;
    }
    //Store all possible scale values
    struct TScalingfactor {
        size_t index;
        tjscalingfactor factor;
        float value;
        bool operator == (const TScalingfactor& r) const
        {
            return ImageUtil::IsSameImageScale(this->value, r.value);
        }
        bool operator < (const TScalingfactor& r) const
        {
            return this->value < r.value;
        }
    };
    std::vector<TScalingfactor> scales;
    for (size_t index = 0; index < scalingFactorList.size(); ++index) {
        const tjscalingfactor& factor = scalingFactorList[index];
        if ((factor.num < 1) || (factor.denom < 1)) {
            continue;
        }
        TScalingfactor scalingFactor;
        scalingFactor.index = index;
        scalingFactor.factor = factor;
        scalingFactor.value = factor.num * 1.0f / factor.denom;
        scales.push_back(scalingFactor);
    }
    ASSERT(!scales.empty());

    // First check whether there is an exactly matching value
    if (!scales.empty()) {
        TScalingfactor dstFactor;
        dstFactor.value = fImageSizeScale;
        auto iter = std::find(scales.begin(), scales.end(), dstFactor);
        if (iter != scales.end()) {
            selectedScalingfactor = iter->factor;
            return true;
        }
    }

    // Find all elements greater than or equal to the target value
    std::vector<TScalingfactor> candidates;
    for (const TScalingfactor& scale : scales) {
        if (scale.value >= fImageSizeScale) {
            candidates.push_back(scale);
        }
    }

    // If there are qualified candidate values, return the smallest one (closest to the target value)
    if (!candidates.empty()) {
        auto iter = std::min_element(candidates.begin(), candidates.end());
        if (iter != candidates.end()) {
            selectedScalingfactor = iter->factor;
            return true;
        }
    }

    // If all values are smaller than the target value, return the largest element
    if (!scales.empty()) {
        auto iter = std::max_element(scales.begin(), scales.end());
        if (iter != scales.end()) {
            selectedScalingfactor = iter->factor;
            return true;
        }
    }
    return false;
}

// Find the value closest to fImageSizeScale
static bool FindClosestScale2(const std::vector<tjscalingfactor>& scalingFactorList,
                              float fImageSizeScale,
                              tjscalingfactor& selectedScalingfactor)
{
    if (scalingFactorList.empty()) {
        return false;
    }
    //Store all possible scale values
    struct TScalingfactor {
        size_t index;
        tjscalingfactor factor;
        float value;
    };
    std::vector<TScalingfactor> scales;
    for (size_t index = 0; index < scalingFactorList.size(); ++index) {
        const tjscalingfactor& factor = scalingFactorList[index];
        if ((factor.num < 1) || (factor.denom < 1)) {
            continue;
        }

        TScalingfactor scalingFactor;
        scalingFactor.index = index;
        scalingFactor.factor = factor;
        scalingFactor.value = factor.num * 1.0f / factor.denom;
        scales.push_back(scalingFactor);
    }
    ASSERT(!scales.empty());
    if (scales.empty()) {
        return false;
    }

    // Initialize the closest value and the minimum difference
    size_t closestIndex = 0;
    float closest = scales[0].value;
    float minDiff = std::abs(fImageSizeScale - closest);

    // Traverse the vector to find the closest value
    for (size_t i = 1; i < scales.size(); ++i) {
        float currentDiff = std::abs(fImageSizeScale - scales[i].value);
        if (currentDiff < minDiff) {
            minDiff = currentDiff;
            closest = scales[i].value;
            closestIndex = i;
        }
    }
    if (closestIndex < scales.size()) {
        selectedScalingfactor = scales[closestIndex].factor;
        return true;
    }
    return false;
}

bool Image_JPEG::LoadImageFile(std::vector<uint8_t>& fileData,
                               const FilePath& imageFilePath,
                               float fImageSizeScale,
                               bool bAsyncDecode,
                               const UiSize& rcMaxDestRectSize,
                               bool bAssertEnabled)
{
    ASSERT(!fileData.empty() || !imageFilePath.IsEmpty());
    if (fileData.empty() && imageFilePath.IsEmpty()) {
        return false;
    }

    //Automatically release resources
    struct TAutoReleaseJpeg
    {
        std::vector<uint8_t>* pFileData = nullptr;
        tjhandle tjInstance = nullptr;
        ~TAutoReleaseJpeg()
        {
            if (pFileData != nullptr) {
                pFileData->clear();
            }
            if (tjInstance != nullptr) {
                tjDestroy(tjInstance);
                tjInstance = nullptr;
            }
        }
    };
    m_impl->m_bAssertEnabled = bAssertEnabled;
    TAutoReleaseJpeg jpegData;

    if (fileData.empty()) {
        //There is no function that directly supports passing a file path, so it needs to be read into memory first and then processed
        FileUtil::ReadFileData(imageFilePath, fileData);
        if (bAssertEnabled) {
            ASSERT(!fileData.empty());
        }
        if (fileData.empty()) {
            m_impl->m_bDecodeError = true;
            return false;
        }
        jpegData.pFileData = &fileData; //The data needs to be cleared on failure
    }

    // Initialize the turbojpeg decoder
    tjhandle tjInstance = tjInitDecompress();
    ASSERT(tjInstance != nullptr);
    if (tjInstance == nullptr) {
        return false;
    }
    jpegData.tjInstance = tjInstance;

    int width = 0;
    int height = 0;
    int jpegSubsamp = 0;
    int jpegColorspace = 0;
    // Decode the JPEG header information to get the basic image information
    int ret = tjDecompressHeader3(tjInstance,
                                  fileData.data(),
                                  (unsigned long)fileData.size(),
                                  &width,
                                  &height,
                                  &jpegSubsamp,
                                  &jpegColorspace);
    if (ret != 0) {
        return false;
    }
    //Header parsing succeeded
    if (bAssertEnabled) {
        ASSERT((width > 0) && (height > 0));
    }
    if ((width <= 0) || (height <= 0)) {
        m_impl->m_bDecodeError = true;
        return false;
    }
    if (!ImageUtil::IsValidImageScale(fImageSizeScale)) {
        fImageSizeScale = 1.0f;
    }
    //When loading with scaling, only fixed locked ratios are supported; if the ratio does not meet the requirements, decoding will fail. This ratio list can be queried through the API
    int numscalingfactors = 0;
    tjscalingfactor* pScalingFactors = tjGetScalingFactors(&numscalingfactors);
    std::vector<tjscalingfactor> scalingFactorList;
    if ((pScalingFactors != nullptr) && (numscalingfactors > 0)) {
        for (int i = 0; i < numscalingfactors; ++i) {
            const tjscalingfactor& factor = pScalingFactors[i];
            if ((factor.num > 0) && factor.denom > 0) {
                scalingFactorList.push_back(factor);
            }
        }
    }
    tjscalingfactor selectedScalingfactor = TJUNSCALED;//By default, it is the original image size, without scaling
    ASSERT(selectedScalingfactor.num == 1);
    ASSERT(selectedScalingfactor.denom == 1);
    if (!scalingFactorList.empty()) {
        //Query the selected scale
        bool bFoundImageScale = false;
        float fScale = fImageSizeScale;
        if (ImageUtil::GetBestImageScale(rcMaxDestRectSize, width, height, fImageSizeScale, fScale)) {
            if (FindClosestScale1(scalingFactorList, fScale, selectedScalingfactor)) {
                bFoundImageScale = true;
            }
        }
        if (!bFoundImageScale) {
            FindClosestScale2(scalingFactorList, fImageSizeScale, selectedScalingfactor);
        }
    }

    //Final verification, ensure the value is valid
    if ((selectedScalingfactor.num < 1) || (selectedScalingfactor.denom < 1)) {
        selectedScalingfactor.num = 1;
        selectedScalingfactor.denom = 1;
    }
    m_impl->m_nWidth = TJSCALED(width, selectedScalingfactor);
    m_impl->m_nHeight = TJSCALED(height, selectedScalingfactor);
    ASSERT((m_impl->m_nWidth > 0) && (m_impl->m_nHeight > 0));
    if ((m_impl->m_nHeight <= 0) || (m_impl->m_nHeight <= 0)) {
        m_impl->m_bDecodeError = true;
        return false;
    }

    //Loading succeeded
    jpegData.tjInstance = nullptr;
    jpegData.pFileData = nullptr;
    m_impl->m_fileData.clear();

    m_impl->m_fImageSizeScale = selectedScalingfactor.num * 1.0f / selectedScalingfactor.denom;
    m_impl->m_tjInstance = tjInstance;
    m_impl->m_fileData.swap(fileData);
    m_impl->m_bAsyncDecode = bAsyncDecode;
    m_impl->m_imageFilePath = imageFilePath;
    m_impl->m_bDecodeError = false;
    return true;
}

uint32_t Image_JPEG::GetWidth() const
{
    return m_impl->m_nWidth;
}

uint32_t Image_JPEG::GetHeight() const
{
    return m_impl->m_nHeight;
}

float Image_JPEG::GetImageSizeScale() const
{
    return m_impl->m_fImageSizeScale;
}

std::shared_ptr<IBitmap> Image_JPEG::GetBitmap(bool* bDecodeError)
{
    GlobalManager::Instance().AssertUIThread();
    std::shared_ptr<IBitmap> pBitmap;
    if (m_impl->m_bAsyncDecode || (m_impl->m_pBitmap != nullptr)) {
        //Asynchronous decoding, or the decoding has been completed
        pBitmap = m_impl->m_pBitmap;
    }
    else {
        //Delayed decoding
        m_impl->m_pBitmap = DecodeBitmap();
        pBitmap = m_impl->m_pBitmap;
        if (pBitmap == nullptr) {
            m_impl->m_bDecodeError = true;
            if (bDecodeError != nullptr) {
                *bDecodeError = true;
            }
        }
    }
    return pBitmap;
}

std::shared_ptr<IBitmap> Image_JPEG::DecodeBitmap() const
{
    std::shared_ptr<IBitmap> pJpegBitmap;
    const TImpl& impl = *m_impl;
    if ((impl.m_tjInstance != nullptr) &&
        !impl.m_fileData.empty() &&
        (impl.m_nWidth > 0) &&
        (impl.m_nHeight > 0)) {
        IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
        ASSERT(pRenderFactory != nullptr);
        if (pRenderFactory == nullptr) {
            return nullptr;
        }
        IBitmap* pBitmap = pRenderFactory->CreateBitmap();
        ASSERT(pBitmap != nullptr);
        if (pBitmap == nullptr) {
            return nullptr;
        }
        pJpegBitmap.reset(pBitmap);
        if (!pBitmap->Init(impl.m_nWidth, impl.m_nHeight, nullptr)) {
            return nullptr;
        }
        void* pBitmapBits = pBitmap->LockPixelBits();
        if (pBitmapBits == nullptr) {
            return nullptr;
        }

#ifdef DUILIB_BUILD_FOR_WIN
        int pixelFormat = TJPF_BGRA;
#else
        int pixelFormat = TJPF_RGBA;
#endif

        // Perform the decoding: decode the JPG memory data into the RGBA format
        int ret = tjDecompress2(impl.m_tjInstance,
                                impl.m_fileData.data(),
                                (unsigned long)impl.m_fileData.size(),
                                (unsigned char*)pBitmapBits,
                                (int)impl.m_nWidth,
                                0, // Row stride, 0 means using the default value (width * bytesPerPixel)
                                (int)impl.m_nHeight,
                                pixelFormat, // The output format is RGBA/BGRA
                                TJFLAG_FASTDCT); // Use the fast DCT algorithm for acceleration
        if (impl.m_bAssertEnabled) {
            ASSERT(ret == 0);
        }
        if (ret == 0) {
            pBitmap->UnLockPixelBits();
        }
        else {
            pJpegBitmap.reset();
        }
    }
    return pJpegBitmap;
}

bool Image_JPEG::IsDelayDecodeEnabled() const
{
    if (m_impl->m_bAsyncDecode &&
        (m_impl->m_tjInstance != nullptr) &&
        !m_impl->m_fileData.empty() &&
        (m_impl->m_nWidth > 0) &&
        (m_impl->m_nHeight > 0) &&
        (m_impl->m_pDelayBitmap == nullptr) &&
        !m_impl->m_bDecodeError) {
        return true;
    }
    return false;
}

bool Image_JPEG::IsDelayDecodeFinished() const
{
    return (m_impl->m_pBitmap != nullptr) || (m_impl->m_pDelayBitmap != nullptr) || m_impl->m_bDecodeError;
}

uint32_t Image_JPEG::GetDecodedFrameIndex() const
{
    return 0;
}

bool Image_JPEG::DelayDecode(uint32_t /*nMinFrameIndex*/, std::function<bool(void)> /*IsAborted*/, bool* bDecodeError)
{
    bool bRet = false;
    if (IsDelayDecodeEnabled()) {
        ASSERT(m_impl->m_pDelayBitmap == nullptr);
        if (m_impl->m_pDelayBitmap == nullptr) {
            m_impl->m_pDelayBitmap = DecodeBitmap();
            bRet = m_impl->m_pDelayBitmap != nullptr;
            if (!bRet) {
                m_impl->m_bDecodeError = true;
                if (bDecodeError != nullptr) {
                    *bDecodeError = true;
                }
            }
        }
    }
    return bRet;
}

bool Image_JPEG::MergeDelayDecodeData()
{
    GlobalManager::Instance().AssertUIThread();
    bool bRet = false;
    bool bDecodeFinished = m_impl->m_bDecodeError || (m_impl->m_pBitmap != nullptr);
    if ((m_impl->m_pDelayBitmap != nullptr) && (m_impl->m_pBitmap == nullptr)) {
        m_impl->m_pBitmap = m_impl->m_pDelayBitmap;
        m_impl->m_pDelayBitmap.reset();

        bDecodeFinished = true;
        bRet = true;
    }
    if (bDecodeFinished) {
        //Decoding is finished, release the original image resources
        if (m_impl->m_tjInstance != nullptr) {
            tjDestroy(m_impl->m_tjInstance);
            m_impl->m_tjInstance = nullptr;
        }
        std::vector<uint8_t> fileData;
        m_impl->m_fileData.swap(fileData);
    }
    return bRet;
}

} //namespace ui

#endif //DUILIB_IMAGE_SUPPORT_JPEG_TURBO
