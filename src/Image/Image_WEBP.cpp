#include "dui/Image/Image_WEBP.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Render/IRender.h"
#include "dui/Utils/FileUtil.h"

#include "third_party/libwebp/src/webp/decode.h"
#include "third_party/libwebp/src/webp/demux.h"
#include <atomic>

namespace ui
{
//Decode the WebP image data, one frame at a time
static AnimationFramePtr DecodeImage_WEBP(WebPAnimDecoder* pWebPAnimDecoder,
                                          float fImageSizeScale,
                                          size_t nFrameIndex,
                                          int32_t& nPrevTimestamp)
{
    ASSERT(pWebPAnimDecoder != nullptr);
    if (pWebPAnimDecoder == nullptr) {
        return nullptr;
    }
    ASSERT(fImageSizeScale > 0);
    if (fImageSizeScale < 0) {
        return nullptr;
    }

    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    ASSERT(pRenderFactory != nullptr);
    if (pRenderFactory == nullptr) {
        return nullptr;
    }

    WebPAnimInfo anim_info;
    if (!WebPAnimDecoderGetInfo(pWebPAnimDecoder, &anim_info)) {
        return nullptr;
    }

    AnimationFramePtr pFrameData;
    //WebPAnimDecoderReset(pWebPAnimDecoder);
    if (WebPAnimDecoderHasMoreFrames(pWebPAnimDecoder)) {
        uint8_t* pImageData = nullptr;
        int timestamp = 0;
        if (WebPAnimDecoderGetNext(pWebPAnimDecoder, &pImageData, &timestamp)) {
            // Control the frame display duration (milliseconds) according to the timestamp
            int32_t duration = timestamp - nPrevTimestamp;
            pFrameData = std::make_shared<IAnimationImage::AnimationFrame>();
            pFrameData->m_nFrameIndex = (int32_t)nFrameIndex;
            pFrameData->SetDelayMs(duration);
            pFrameData->m_nOffsetX = 0;//Neither OffsetX nor OffsetY needs to be processed
            pFrameData->m_nOffsetY = 0;
            pFrameData->m_bDataPending = false;
            pFrameData->m_pBitmap.reset(pRenderFactory->CreateBitmap());
            ASSERT(pFrameData->m_pBitmap != nullptr);
            if (pFrameData->m_pBitmap == nullptr) {
                pFrameData.reset();
            }
            else if (!pFrameData->m_pBitmap->Init(anim_info.canvas_width, anim_info.canvas_height, pImageData, fImageSizeScale)) {
                pFrameData.reset();
            }
            else {
                nPrevTimestamp = timestamp;
            }
        }
        else {
            //Image data decoding failed
            //ASSERT(0);
        }
    }
    return pFrameData;
}

struct Image_WEBP::TImpl
{
public:
    //File data
    std::vector<uint8_t> m_fileData;

    //Image file path
    FilePath m_imageFilePath;

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

    //Data of each image frame
    std::vector<AnimationFramePtr> m_frames;

public:
    //Whether asynchronous thread decoding of the image data is supported
    bool m_bAsyncDecode = false;

    //Whether the image data is being decoded
    std::atomic<bool> m_bAsyncDecoding = false;

    //The timestamp of the previous frame, used to calculate the playback duration
    int32_t m_nPrevTimestamp = 0;

    //The handle after loading
    WebPAnimDecoder* m_pWebPAnimDecoder = nullptr;

    //The data that WebP depends on
    WebPData m_webpData;

    //Data of each image frame (data decoded with delay)
    std::vector<AnimationFramePtr> m_delayFrames;

public:
    //Clean up resources
    void ClearImageData()
    {
        if (m_pWebPAnimDecoder != nullptr) {
            WebPAnimDecoderDelete(m_pWebPAnimDecoder);
            m_pWebPAnimDecoder = nullptr;
        }
        if (!m_fileData.empty()) {
            std::vector<uint8_t> fileData;
            m_fileData.swap(fileData);
        }
    }

    //Whether decoding is finished
    bool IsDecodeFinished() const
    {
        if (((int32_t)m_frames.size() == m_nFrameCount) || m_bDecodeError) {
            return true;
        }
        return false;
    }
};

Image_WEBP::Image_WEBP()
{
    m_impl = std::make_unique<TImpl>();
}

Image_WEBP::~Image_WEBP()
{
    m_impl->ClearImageData();
}

bool Image_WEBP::LoadImageFile(std::vector<uint8_t>& fileData,
                               const FilePath& imageFilePath,
                               bool bLoadAllFrames,
                               bool bAsyncDecode,
                               float fImageSizeScale,
                               const UiSize& rcMaxDestRectSize,
                               bool bAssertEnabled)
{
    ASSERT(!fileData.empty() || !imageFilePath.IsEmpty());
    if (fileData.empty() && imageFilePath.IsEmpty()) {
        return false;
    }
    const bool bNeedRestoreData = !fileData.empty();
    if (fileData.empty()) {
        //There is no interface that supports the file path, so it needs to be read into memory
        FileUtil::ReadFileData(imageFilePath, fileData);
        if (bAssertEnabled) {
            ASSERT(!fileData.empty());
        }
        if (fileData.empty()) {
            return false;
        }
    }

    m_impl->m_bDecodeError = false;
    m_impl->m_bAssertEnabled = bAssertEnabled;
    m_impl->m_imageFilePath = imageFilePath;
    m_impl->m_fImageSizeScale = fImageSizeScale;
    m_impl->m_bLoadAllFrames = bLoadAllFrames;
    m_impl->m_bAsyncDecode = bAsyncDecode;

    m_impl->m_fileData.clear();
    m_impl->m_fileData.swap(fileData);

    m_impl->m_webpData.bytes = m_impl->m_fileData.data();
    m_impl->m_webpData.size = m_impl->m_fileData.size();

    WebPAnimDecoderOptions dec_options;
    WebPAnimDecoderOptionsInit(&dec_options);
#ifdef DUI_BUILD_FOR_WIN
    //Data format: BGRA on Windows (premultiplied), RGBA on other platforms (premultiplied)
    dec_options.color_mode = MODE_bgrA;
#else
    dec_options.color_mode = MODE_rgbA;
#endif

    //Enable multi-threaded decoding
    dec_options.use_threads = 1;

    WebPAnimDecoder* dec = WebPAnimDecoderNew(&m_impl->m_webpData, &dec_options);
    if (dec == nullptr) {
        //When loading fails, the original file data needs to be restored
        if (bNeedRestoreData) {
            m_impl->m_fileData.swap(fileData);
        }
        m_impl->m_webpData = { 0, 0 };
        return false;
    }
    WebPAnimInfo anim_info;
    int nRet = WebPAnimDecoderGetInfo(dec, &anim_info);
    if (nRet == 0) {
        //When loading fails, the original file data needs to be restored
        WebPAnimDecoderDelete(dec);
        if (bNeedRestoreData) {
            m_impl->m_fileData.swap(fileData);
        }
        m_impl->m_webpData = { 0, 0 };
        return false;
    }

    m_impl->m_nWidth = anim_info.canvas_width;
    m_impl->m_nHeight = anim_info.canvas_height;
    m_impl->m_nFrameCount = (int32_t)anim_info.frame_count;

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
    if (m_impl->m_bAssertEnabled) {
        ASSERT(m_impl->m_nWidth > 0);
        ASSERT(m_impl->m_nHeight > 0);
        ASSERT(m_impl->m_nFrameCount > 0);
    }

    if ((m_impl->m_nFrameCount <= 0) || ((int32_t)m_impl->m_nWidth <= 0) || ((int32_t)m_impl->m_nHeight <= 0)) {
        //When loading fails, the original file data needs to be restored
        WebPAnimDecoderDelete(dec);
        if (bNeedRestoreData) {
            m_impl->m_fileData.swap(fileData);
        }
        m_impl->m_webpData = { 0, 0 };
        return false;
    }

    m_impl->m_nLoops = (int32_t)anim_info.loop_count;
    if (m_impl->m_nLoops <= 0) {
        m_impl->m_nLoops = -1;
    }
    m_impl->m_pWebPAnimDecoder = dec;
    m_impl->m_bDecodeError = false;
    return true;
}

bool Image_WEBP::IsDelayDecodeEnabled() const
{
    if (m_impl->m_bAsyncDecode &&
        !m_impl->m_fileData.empty() &&
        (m_impl->m_pWebPAnimDecoder != nullptr) &&
        !m_impl->m_bDecodeError) {
        return true;
    }
    return false;
}

bool Image_WEBP::IsDelayDecodeFinished() const
{
    if (m_impl->m_bAsyncDecoding) {
        return false;
    }
    if (m_impl->m_bDecodeError) {
        return true;
    }
    return (int32_t)(m_impl->m_frames.size() + m_impl->m_delayFrames.size()) == m_impl->m_nFrameCount;
}

uint32_t Image_WEBP::GetDecodedFrameIndex() const
{
    if (m_impl->m_frames.empty()) {
        return 0;
    }
    else {
        return (uint32_t)m_impl->m_frames.size() - 1;
    }
}

bool Image_WEBP::DelayDecode(uint32_t nMinFrameIndex, std::function<bool(void)> IsAborted, bool* bDecodeError)
{
    if (!IsDelayDecodeEnabled()) {
        return false;
    }
    if (m_impl->m_bAsyncDecoding) {
        return false;
    }
    ASSERT(m_impl->m_nFrameCount > 0);
    if (m_impl->m_nFrameCount <= 0) {
        m_impl->m_bDecodeError = true;
    }
    if (m_impl->m_bDecodeError) {
        if (bDecodeError != nullptr) {
            *bDecodeError = true;
        }
        return false;
    }
    m_impl->m_bAsyncDecoding = true;
    const size_t nFrameCount = (size_t)m_impl->m_nFrameCount;

    bool bRet = true;
    float fImageSizeScale = m_impl->m_fImageSizeScale;
    while (((IsAborted == nullptr) || !IsAborted()) &&
           (nMinFrameIndex >= (m_impl->m_frames.size() + m_impl->m_delayFrames.size())) &&
           ((m_impl->m_frames.size() + m_impl->m_delayFrames.size()) < nFrameCount)) {
        //Decode one frame at a time
        const size_t nFrameIndex = m_impl->m_delayFrames.size() + m_impl->m_frames.size();
        AnimationFramePtr pNewAnimationFrame;
        pNewAnimationFrame = DecodeImage_WEBP(m_impl->m_pWebPAnimDecoder,
                                              fImageSizeScale,
                                              nFrameIndex,
                                              m_impl->m_nPrevTimestamp);
        if (pNewAnimationFrame != nullptr) {
            m_impl->m_delayFrames.push_back(pNewAnimationFrame);
        }
        else {
            bRet = false;
            m_impl->m_bDecodeError = true;
            if (bDecodeError != nullptr) {
                *bDecodeError = false;
            }
            break;
        }
    }

    m_impl->m_bAsyncDecoding = false;
    return bRet;
}

bool Image_WEBP::MergeDelayDecodeData()
{
    GlobalManager::Instance().AssertUIThread();
    bool bRet = false;
    if (!m_impl->m_bAsyncDecoding && !m_impl->m_delayFrames.empty()) {
        //Merge the data
        for (auto p : m_impl->m_delayFrames) {
            m_impl->m_frames.push_back(p);
        }
        m_impl->m_delayFrames.clear();
        bRet = true;
    }
    if (!m_impl->m_bAsyncDecoding) {
        //If the decoding is finished, release the image resources
        if (m_impl->IsDecodeFinished()) {
            m_impl->ClearImageData();
        }
    }
    return bRet;
}

uint32_t Image_WEBP::GetWidth() const
{
    return m_impl->m_nWidth;
}

uint32_t Image_WEBP::GetHeight() const
{
    return m_impl->m_nHeight;
}

float Image_WEBP::GetImageSizeScale() const
{
    return m_impl->m_fImageSizeScale;
}

int32_t Image_WEBP::GetFrameCount() const
{
    return m_impl->m_nFrameCount;
}

int32_t Image_WEBP::GetLoopCount() const
{
    return m_impl->m_nLoops;
}

bool Image_WEBP::IsFrameDataReady(uint32_t nFrameIndex)
{
    GlobalManager::Instance().AssertUIThread();
    if (m_impl->m_bAsyncDecode) {
        if (nFrameIndex < m_impl->m_frames.size()) {
            return true;
        }
        return false;
    }
    else {
        return true;
    }
}

int32_t Image_WEBP::GetFrameDelayMs(uint32_t nFrameIndex)
{
    GlobalManager::Instance().AssertUIThread();
    int32_t nDelayMs = 1000;
    if (nFrameIndex < m_impl->m_frames.size()) {
        AnimationFramePtr pFrame = m_impl->m_frames[nFrameIndex];
        if (pFrame != nullptr) {
            nDelayMs = pFrame->GetDelayMs();
        }
    }
    else if (!m_impl->m_frames.empty()) {
        AnimationFramePtr pFrame = m_impl->m_frames.back();
        if (pFrame != nullptr) {
            nDelayMs = pFrame->GetDelayMs();
        }
    }
    return nDelayMs;
}

bool Image_WEBP::ReadFrameData(int32_t nFrameIndex, const UiSize& /*szDestRectSize*/, AnimationFrame* pAnimationFrame)
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
        pAnimationFrame->m_bDataError = true;
        return false;
    }
    if (m_impl->m_bDecodeError) {
        pAnimationFrame->m_bDataError = true;
        return false;
    }

    if (!m_impl->m_bAsyncDecode) {
        //In the case of synchronous decoding, decode the required frames
        while ((nFrameIndex >= (int32_t)m_impl->m_frames.size()) &&
               ((int32_t)m_impl->m_frames.size() < m_impl->m_nFrameCount)) {
            ASSERT(m_impl->m_delayFrames.empty());
            uint32_t nInitFrameIndex = (uint32_t)m_impl->m_frames.size();
            float fImageSizeScale = m_impl->m_fImageSizeScale;
            AnimationFramePtr pNewAnimationFrame;
            pNewAnimationFrame = DecodeImage_WEBP(m_impl->m_pWebPAnimDecoder,
                                                  fImageSizeScale,
                                                  nInitFrameIndex,
                                                  m_impl->m_nPrevTimestamp);
            if (pNewAnimationFrame != nullptr) {
                m_impl->m_frames.push_back(pNewAnimationFrame);
            }
            else {
                m_impl->m_bDecodeError = true;
                pAnimationFrame->m_bDataError = true;
                break;
            }
        }

        if (m_impl->IsDecodeFinished()) {
            //If the decoding is finished, release the image resources
            m_impl->ClearImageData();
        }
        else if (!m_impl->m_bDecodeError) {
            ASSERT((nFrameIndex < (int32_t)m_impl->m_frames.size()));
            if ((nFrameIndex >= (int32_t)m_impl->m_frames.size())) {
                pAnimationFrame->m_bDataError = true;
                return false;
            }
        }
    }
    else {
        //Merge the data
        MergeDelayDecodeData();
    }
    bool bRet = false;
    if (nFrameIndex < (int32_t)m_impl->m_frames.size()) {
        AnimationFramePtr pFrameData = m_impl->m_frames[nFrameIndex];
        if (pFrameData != nullptr) {
            ASSERT(pFrameData->m_nFrameIndex == nFrameIndex);
            *pAnimationFrame = *pFrameData;
            pAnimationFrame->m_bDataPending = false;
            pAnimationFrame->m_bDataError = false;
            ASSERT(pAnimationFrame->m_pBitmap != nullptr);
            bRet = true;
        }
        else {
            m_impl->m_bDecodeError = true;
            pAnimationFrame->m_bDataError = true;
        }
    }
    else if (m_impl->m_bAsyncDecode) {
        if ((int32_t)m_impl->m_frames.size() < m_impl->m_nFrameCount) {
            //Multi-frame decoding has not been finished yet
            pAnimationFrame->m_bDataPending = true;
            pAnimationFrame->m_bDataError = false;
            pAnimationFrame->m_pBitmap.reset();
            bRet = true;
        }
        else {
            m_impl->m_bDecodeError = true;
            pAnimationFrame->m_bDataError = true;
        }
    }
    else {
        m_impl->m_bDecodeError = true;
        pAnimationFrame->m_bDataError = true;
    }
    return bRet;
}

} //namespace ui
