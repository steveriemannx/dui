#include "dui/Image/Image_PAG.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Render/IRender.h"
#include "dui/Image/ImageDecoder.h"
#include "dui/Utils/FileUtil.h"

#ifdef DUI_IMAGE_SUPPORT_LIB_PAG

#if defined (_MSC_VER)
    #pragma warning (push)
    #pragma warning (disable: 4068)
#endif

#include "pag/pag.h"

#if defined (_MSC_VER)
    #pragma warning (pop)
#endif

namespace ui
{

struct Image_PAG::TImpl
{
    //Image file path
    FilePath m_imageFilePath;

    //File data
    std::vector<uint8_t> m_fileData;

    //Image width
    uint32_t m_nWidth = 0;

    //Image height
    uint32_t m_nHeight = 0;

    //Total frame count of the image
    int32_t m_nFrameCount = 0;

    //Number of playback loops
    int32_t m_nLoops = -1;

    //Whether to load all frames
    bool m_bLoadAllFrames = true;

    //Whether assertions are allowed when image data errors occur
    bool m_bAssertEnabled = true;

    //Whether an image data decoding error exists
    bool m_bDecodeError = false;

    //Scaling ratio
    float m_fImageSizeScale = IMAGE_SIZE_SCALE_NONE;

    //The time interval of each frame
    int32_t m_frameDelayMs = IMAGE_ANIMATION_DELAY_MS;

public:
    // PAG related objects
    std::shared_ptr<pag::PAGComposition> m_pagComposition = nullptr;
    std::shared_ptr<pag::PAGDecoder> m_pagDecoder = nullptr;

public:
    //Initialize the data from the already opened file
    bool InitImageData(bool bLoadAllFrames,
                       float fPagMaxFrameRate,
                       float fImageSizeScale,
                       const UiSize& rcMaxDestRectSize)
    {
        if (m_pagComposition == nullptr) {
            return false;
        }

        m_fImageSizeScale = fImageSizeScale;
        m_bLoadAllFrames = bLoadAllFrames;
        m_pagDecoder = pag::PAGDecoder::MakeFrom(m_pagComposition, fPagMaxFrameRate, 1.0f);
        if (m_pagDecoder == nullptr) {
            return false;
        }

        //Calculate the appropriate scaling ratio
        int32_t nWidth = m_pagDecoder->width();
        int32_t nHeight = m_pagDecoder->height();
        if (m_bAssertEnabled) {
            ASSERT((nWidth > 0) && (nHeight > 0));
        }
        if ((nWidth <= 0) || (nHeight <= 0)) {
            return false;
        }
        float fRealScale = fImageSizeScale;
        if (ImageUtil::GetBestImageScale(rcMaxDestRectSize, nWidth, nHeight, fImageSizeScale, fRealScale)) {
            m_fImageSizeScale = fRealScale;
            m_pagDecoder = pag::PAGDecoder::MakeFrom(m_pagComposition, fPagMaxFrameRate, fRealScale);
            if (m_pagDecoder == nullptr) {
                return false;
            }
        }
        else {
            m_fImageSizeScale = fImageSizeScale;
        }

        pag::PAGDecoder& pagDecoder = *m_pagDecoder;
        if ((pagDecoder.width() <= 0) || (pagDecoder.height() <= 0) || (pagDecoder.numFrames() <= 0)) {
            return false;
        }

        m_nWidth = (uint32_t)pagDecoder.width();
        m_nHeight = (uint32_t)pagDecoder.height();
        m_nFrameCount = (int32_t)pagDecoder.numFrames();

        if (m_bAssertEnabled) {
            ASSERT(m_nWidth > 0);
            ASSERT(m_nHeight > 0);
            ASSERT(m_nFrameCount > 0);
        }

        if ((m_nFrameCount <= 0) || ((int32_t)m_nWidth <= 0) || ((int32_t)m_nHeight <= 0)) {
            //When loading fails, the original file data needs to be restored
            return false;
        }
        //Loop playback is fixed to play forever, because the GIF format has no such setting
        m_nLoops = -1;
        if (!bLoadAllFrames) {
            m_nFrameCount = 1;
        }

        //The playback time of each frame
        m_frameDelayMs = (int32_t)(1000 / pagDecoder.frameRate());
        return true;
    }
};

Image_PAG::Image_PAG()
{
    m_impl = std::make_unique<TImpl>();
}

Image_PAG::~Image_PAG()
{

}

bool Image_PAG::LoadImageFile(std::vector<uint8_t>& fileData,
                              const FilePath& imageFilePath,
                              bool bLoadAllFrames,
                              float fPagMaxFrameRate,
                              float fImageSizeScale,
                              const UiSize& rcMaxDestRectSize,
                              bool bAssertEnabled)
{
    ASSERT(!fileData.empty() || !imageFilePath.IsEmpty());
    if (fileData.empty() && imageFilePath.IsEmpty()) {
        return false;
    }
    bool bNeedRestoreData = !fileData.empty();
    if (fileData.empty()) {
        //Need to read into memory first and then load (the function that loads from the file path is not used, because that function has a bug and returns failure)
        FileUtil::ReadFileData(imageFilePath, fileData);
        if (fileData.empty()) {
            return false;
        }
    }
    m_impl->m_bAssertEnabled = bAssertEnabled;
    m_impl->m_imageFilePath = imageFilePath;
    m_impl->m_fileData.clear();
    m_impl->m_fileData.swap(fileData);
    //Note: libpag does not actually implement the password feature internally, the interface just supports this parameter
    m_impl->m_pagComposition = pag::PAGFile::Load(m_impl->m_fileData.data(), m_impl->m_fileData.size(), "");
    if (m_impl->m_pagComposition == nullptr) {
        //When loading fails, restore the data
        if (bNeedRestoreData) {
            m_impl->m_fileData.swap(fileData);
        }
        return false;
    }
    bool bRet = m_impl->InitImageData(bLoadAllFrames, fPagMaxFrameRate, fImageSizeScale, rcMaxDestRectSize);
    if (!bRet) {
        //When loading fails, restore the data
        if (bNeedRestoreData) {
            m_impl->m_fileData.swap(fileData);
        }
    }
    return bRet;
}

bool Image_PAG::IsDelayDecodeEnabled() const
{
    return false;
}

bool Image_PAG::IsDelayDecodeFinished() const
{
    return true;
}

uint32_t Image_PAG::GetDecodedFrameIndex() const
{
    return 0;
}

bool Image_PAG::DelayDecode(uint32_t /*nMinFrameIndex*/, std::function<bool(void)> /*IsAborted*/, bool* /*bDecodeError*/)
{
    return false;
}

bool Image_PAG::MergeDelayDecodeData()
{
    return false;
}

uint32_t Image_PAG::GetWidth() const
{
    return m_impl->m_nWidth;
}

uint32_t Image_PAG::GetHeight() const
{
    return m_impl->m_nHeight;
}

float Image_PAG::GetImageSizeScale() const
{
    return m_impl->m_fImageSizeScale;
}

int32_t Image_PAG::GetFrameCount() const
{
    return m_impl->m_nFrameCount;
}

int32_t Image_PAG::GetLoopCount() const
{
    return m_impl->m_nLoops;
}

bool Image_PAG::IsFrameDataReady(uint32_t /*nFrameIndex*/)
{
    return true;
}

int32_t Image_PAG::GetFrameDelayMs(uint32_t /*nFrameIndex*/)
{
    return m_impl->m_frameDelayMs;
}

bool Image_PAG::ReadFrameData(int32_t nFrameIndex, const UiSize& /*szDestRectSize*/, AnimationFrame* pAnimationFrame)
{
    GlobalManager::Instance().AssertUIThread();
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
        m_impl->m_bDecodeError = true;
    }
    if (m_impl->m_bDecodeError) {
        pAnimationFrame->m_bDataError = true;
        return false;
    }

    ASSERT(m_impl->m_pagDecoder != nullptr);
    if (m_impl->m_pagDecoder == nullptr) {
        m_impl->m_bDecodeError = true;
        pAnimationFrame->m_bDataError = true;
        return false;
    }

    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    ASSERT(pRenderFactory != nullptr);
    if (pRenderFactory == nullptr) {
        m_impl->m_bDecodeError = true;
        pAnimationFrame->m_bDataError = true;
        return false;
    }
    std::shared_ptr<IBitmap> pBitmap(pRenderFactory->CreateBitmap());
    ASSERT(pBitmap != nullptr);
    if (pBitmap == nullptr) {
        m_impl->m_bDecodeError = true;
        pAnimationFrame->m_bDataError = true;
        return false;
    }

    pag::PAGDecoder& pagDecoder = *m_impl->m_pagDecoder;
    const uint32_t nImageWidth = (uint32_t)pagDecoder.width();
    const uint32_t nImageHeight = (uint32_t)pagDecoder.height();
    if (!pBitmap->Init(nImageWidth, nImageHeight, nullptr)) {
        m_impl->m_bDecodeError = true;
        pAnimationFrame->m_bDataError = true;
        return false;
    }
    int32_t index = nFrameIndex;
    void* pixels = pBitmap->LockPixelBits();
    ASSERT(pixels != nullptr);
    if (pixels == nullptr) {
        m_impl->m_bDecodeError = true;
        pAnimationFrame->m_bDataError = true;
        return false;
    }
    size_t rowBytes = pBitmap->GetWidth() * 4;
#ifdef DUI_BUILD_FOR_WIN
    pag::ColorType colorType = pag::ColorType::BGRA_8888;
#else
    pag::ColorType colorType = pag::ColorType::RGBA_8888;
#endif
    pag::AlphaType alphaType = pag::AlphaType::Premultiplied;
    bool bRet = pagDecoder.readFrame(index, pixels, rowBytes, colorType, alphaType);
    if (bRet) {
        pBitmap->UnLockPixelBits();
        pAnimationFrame->m_pBitmap = pBitmap;
        pAnimationFrame->m_nOffsetX = 0;
        pAnimationFrame->m_nOffsetY = 0;
        pAnimationFrame->m_bDataPending = false;
        pAnimationFrame->m_nFrameIndex = nFrameIndex;
        pAnimationFrame->SetDelayMs(m_impl->m_frameDelayMs);
        pAnimationFrame->m_bDataError = false;
    }
    else {
        m_impl->m_bDecodeError = true;
        pAnimationFrame->m_bDataError = true;
        pAnimationFrame->m_pBitmap.reset();
    }
    return bRet;
}

} //namespace ui

#endif //DUI_IMAGE_SUPPORT_LIB_PAG
