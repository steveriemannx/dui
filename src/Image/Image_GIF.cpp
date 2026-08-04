#include "dui/Image/Image_GIF.h"
#include "dui/Image/FrameSequence_gif.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Render/IRender.h"

#include "third_party/giflib/gif_lib.h"
#include <atomic>

namespace ui
{
//Memory data source structure: stores the GIF data in memory, its total size, and the current read position
typedef struct {
    const unsigned char* data;  // Pointer to the original GIF data in memory
    size_t size;                // Total number of data bytes
    size_t position;            // Current read offset (starting from 0)
} UiGifMemorySource;

/** Custom memory read function
* @param gif: The internal file handle of giflib
* @param buf: The buffer that receives the data
* @param len: The number of bytes requested to read
* @return: The number of bytes actually read (0 means the data has been fully read)
*/
static int UiGifMemoryReadFunc(GifFileType* gif, GifByteType* buf, int len)
{
    if (gif == nullptr || buf == nullptr || len <= 0) {
        return 0;
    }

    // Get the memory data source from UserData
    UiGifMemorySource* source = (UiGifMemorySource*)gif->UserData;
    if (source == nullptr || source->data == nullptr || source->position >= source->size) {
        return 0;  // The data has been exhausted
    }

    // Calculate the number of bytes that can actually be read (to avoid out-of-bounds)
    int bytes_to_read = (len > (int)(source->size - source->position)) ? (int)(source->size - source->position) : len;

    // Copy the data from memory to the buffer
    memcpy(buf, source->data + source->position, bytes_to_read);
    source->position += bytes_to_read;

    return bytes_to_read;
}

/** Initialize the decoder: create a GIF decode handle from the memory data
* @param data: The original GIF data in memory
* @param size: The total number of data bytes
* @param error_code: Output error code (refer to the D_GIF_* constants)
* @return: Returns the GifFileType handle on success, nullptr on failure
*/
static GifFileType* UiGifInitDecoder(const unsigned char* data, size_t size, int* error_code)
{
    // Validate the input parameters
    if (data == nullptr || size == 0 || error_code == nullptr) {
        if (error_code != nullptr) {
            *error_code = D_GIF_ERR_OPEN_FAILED;
        }
        return nullptr;
    }

    // Initialize the memory data source
    UiGifMemorySource* source = (UiGifMemorySource*)malloc(sizeof(UiGifMemorySource));
    if (source == nullptr) {
        *error_code = D_GIF_ERR_NOT_ENOUGH_MEM;
        return nullptr;
    }
    source->data = data;
    source->size = size;
    source->position = 0;

    // Call DGifOpen to create the decode handle
    GifFileType* gif = DGifOpen(source, UiGifMemoryReadFunc, error_code);
    if (gif == nullptr) {
        free(source);
        return nullptr;
    }
    return gif;
}

/** Release the decoder resources (including the memory data source and the GIF handle)
 * @param gif: The GifFileType handle to release
 * @param error_code: Output the error code at closing time (can be nullptr)
 */
static void UiGifFreeDecoder(GifFileType* gif, int* error_code)
{
    if (gif == nullptr) {
        return;
    }

    // Release the memory data source (stored in UserData)
    if (gif->UserData != nullptr) {
        free(gif->UserData);
        gif->UserData = nullptr;
    }

    // Close the GIF handle
    DGifCloseFile(gif, error_code);
}

// RGBA pixel structure
struct UiGifRGBA {
    uint8_t r, g, b, a;
};

/** Parse a GIF file into per-frame RGBA data
 * @param gif The GIF image data decoder
 * @param nFrameIndex Which frame to load
 * @param fImageSizeScale The scaling ratio of the image
 * @param canvas The canvas
 * @param nPrevFrameIndex The index number of the previous frame
 * @return Returns the created frame data
 */
static AnimationFramePtr UiGifToRgbaFrames(FrameSequence_gif& gif,
                                           int32_t nFrameIndex,
                                           float fImageSizeScale,
                                           std::vector<UiGifRGBA>& canvas,
                                           int32_t& nPrevFrameIndex)
{
    ASSERT((nFrameIndex >= 0) && (nFrameIndex < gif.GetFrameCount()));
    if ((nFrameIndex < 0) || (nFrameIndex >= gif.GetFrameCount())) {
        return nullptr;
    }

    ASSERT(nPrevFrameIndex == (nFrameIndex - 1));
    if (nPrevFrameIndex != (nFrameIndex - 1)) {
        return nullptr;
    }

    // Check whether the GIF width and height are valid
    ASSERT((gif.GetWidth() > 0) && (gif.GetHeight() > 0));
    if ((gif.GetWidth() <= 0) || (gif.GetHeight() <= 0)) {
        return nullptr;
    }

    // Get the render factory instance
    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    ASSERT(pRenderFactory != nullptr);
    if (pRenderFactory == nullptr) {
        return nullptr;
    }

    const int nImageWidth = gif.GetWidth();
    const int nImageHeight = gif.GetHeight();
    const int outputPixelStride = nImageWidth;
    const int canvas_pixel_count = nImageWidth * nImageHeight;
    if ((int)canvas.size() != canvas_pixel_count) {
        canvas.resize(canvas_pixel_count); // Initialize the canvas
    }

    // Pre-create the bitmap object of the current frame
    std::shared_ptr<IBitmap> pBitmap(pRenderFactory->CreateBitmap());
    if (pBitmap == nullptr) {
        return nullptr;
    }

    AnimationFramePtr pFrameData = std::make_shared<IAnimationImage::AnimationFrame>();
    pFrameData->m_nFrameIndex = nFrameIndex;
    pFrameData->m_nOffsetX = 0;
    pFrameData->m_nOffsetY = 0;
    pFrameData->m_bDataPending = false;
    pFrameData->m_pBitmap = pBitmap;

    // Get the data of the current frame
    gif.DrawFrame(nFrameIndex, (Color8888*)canvas.data(), outputPixelStride, nPrevFrameIndex);

    // Update the bitmap data
#ifdef DUI_BUILD_FOR_WIN
    std::vector<UiGifRGBA> canvasWin = canvas;
    //Swap R and B; the Windows platform uses the ABGR format
    for (int y = 0; y < nImageHeight; y++) {
        for (int x = 0; x < nImageWidth; x++) {
            UiGifRGBA& pixelColor = canvasWin[y * outputPixelStride + x];
            std::swap(pixelColor.b, pixelColor.r);
        }
    }
    pFrameData->m_pBitmap->Init(nImageWidth, nImageHeight, canvasWin.data(), fImageSizeScale);
#else
    pFrameData->m_pBitmap->Init(nImageWidth, nImageHeight, canvas.data(), fImageSizeScale);
#endif

    nPrevFrameIndex = nFrameIndex;
    return pFrameData;
}

struct Image_GIF::TImpl
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

    //Data of each image frame
    std::vector<AnimationFramePtr> m_frames;

    //Playback delay time of each frame, milliseconds
    std::vector<int32_t> m_framesDelayMs;

public:
    //Data of each image frame (data decoded with delay)
    std::vector<AnimationFramePtr> m_delayFrames;

    //Whether asynchronous thread decoding of the image data is supported
    bool m_bAsyncDecode = false;

    //Whether the image data is being decoded
    std::atomic<bool> m_bAsyncDecoding = false;

    //The handle after loading
    GifFileType* m_gifDecoder = nullptr;

    //The encapsulation of the GIF decoding implementation
    FrameSequence_gif m_gifFrameSequence;

    //The canvas for drawing the GIF image
    std::vector<UiGifRGBA> m_gifCanvas;

    //The index number of the previous frame
    int32_t m_nLastFrameIndex = -1;

public:
    //Initialize from the already opened file handle
    bool InitImageData(GifFileType* dec,
                       std::vector<uint8_t>& fileData,
                       bool bLoadAllFrames,
                       bool bAsyncDecode,
                       float fImageSizeScale,
                       const UiSize& rcMaxDestRectSize)
    {
        ASSERT(dec != nullptr);
        if (dec == nullptr) {
            return false;
        }
        if (!m_gifFrameSequence.Init(dec)) {
            m_gifFrameSequence.Clear();
            UiGifFreeDecoder(dec, nullptr);
            //When loading fails, the original file data needs to be restored
            m_fileData.swap(fileData);
            return false;
        }

        m_fImageSizeScale = fImageSizeScale;
        m_bLoadAllFrames = bLoadAllFrames;
        m_bAsyncDecode = bAsyncDecode;

        m_nWidth = (uint32_t)m_gifFrameSequence.GetWidth();
        m_nHeight = (uint32_t)m_gifFrameSequence.GetHeight();
        m_nFrameCount = (int32_t)m_gifFrameSequence.GetFrameCount();

        if (m_bAssertEnabled) {
            ASSERT(m_nWidth > 0);
            ASSERT(m_nHeight > 0);
            ASSERT(m_nFrameCount > 0);
        }
        if ((m_nFrameCount <= 0) || ((int32_t)m_nWidth <= 0) || ((int32_t)m_nHeight <= 0)) {
            //The image format is correct, but there is an error in the image data (do not restore the image data)
            m_bDecodeError = true;
            UiGifFreeDecoder(dec, nullptr);
            m_gifFrameSequence.Clear();
            return false;
        }


        float fScale = fImageSizeScale;
        if (ImageUtil::GetBestImageScale(rcMaxDestRectSize, m_nWidth, m_nHeight, fImageSizeScale, fScale)) {
            m_nWidth = ImageUtil::GetScaledImageSize(m_nWidth, fScale);
            m_nHeight = ImageUtil::GetScaledImageSize(m_nHeight, fScale);
            m_fImageSizeScale = fScale;
        }
        else {
            m_nWidth = ImageUtil::GetScaledImageSize(m_nWidth, fImageSizeScale);
            m_nHeight = ImageUtil::GetScaledImageSize(m_nHeight, fImageSizeScale);
        }
        ASSERT(m_nWidth > 0);
        ASSERT(m_nHeight > 0);
        ASSERT(m_nFrameCount > 0);

        if ((m_nFrameCount <= 0) || ((int32_t)m_nWidth <= 0) || ((int32_t)m_nHeight <= 0)) {
            //Loading failed
            m_bDecodeError = true;
            UiGifFreeDecoder(dec, nullptr);
            m_gifFrameSequence.Clear();
            return false;
        }
        //Number of playback loops
        m_nLoops = m_gifFrameSequence.GetDefaultLoopCount();
        if (m_nLoops < 1) {
            m_nLoops = -1;
        }

        //Decode the playback time of each frame
        m_framesDelayMs.clear();
        for (int frame_idx = 0; frame_idx < m_nFrameCount; ++frame_idx) {
            GraphicsControlBlock gcb;
            memset(&gcb, 0, sizeof(GraphicsControlBlock));
            DGifSavedExtensionToGCB(dec, frame_idx, &gcb);
            m_framesDelayMs.push_back(gcb.DelayTime * 10);
        }

        m_gifDecoder = dec;
        m_gifCanvas.clear();
        m_nLastFrameIndex = -1;
        return true;
    }

    //Clean up resources
    void ClearImageData()
    {
        if (m_gifDecoder != nullptr) {
            UiGifFreeDecoder(m_gifDecoder, nullptr);
            m_gifDecoder = nullptr;
        }
        m_gifFrameSequence.Clear();
        std::vector<uint8_t> fileData;
        m_fileData.swap(fileData);

        std::vector<UiGifRGBA> gifCanvas;
        m_gifCanvas.swap(gifCanvas);
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

Image_GIF::Image_GIF()
{
    m_impl = std::make_unique<TImpl>();
}

Image_GIF::~Image_GIF()
{
    m_impl->ClearImageData();
}

bool Image_GIF::LoadImageFile(std::vector<uint8_t>& fileData,
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
    m_impl->m_bAssertEnabled = bAssertEnabled;
    m_impl->m_imageFilePath = imageFilePath;
    m_impl->m_fileData.clear();
    m_impl->m_fileData.swap(fileData);

    if (!m_impl->m_fileData.empty()) {
        int nErrorCode = 0;
        GifFileType* dec = UiGifInitDecoder(m_impl->m_fileData.data(), m_impl->m_fileData.size(), &nErrorCode);
        if (dec == nullptr) {
            //When loading fails, the original file data needs to be restored
            m_impl->m_fileData.swap(fileData);
            return false;
        }
        return m_impl->InitImageData(dec, fileData, bLoadAllFrames, bAsyncDecode, fImageSizeScale, rcMaxDestRectSize);
    }
    else {
        DStringA gifFileName = imageFilePath.NativePathA();
        ASSERT(!gifFileName.empty());
        int nErrorCode = 0;
        GifFileType* dec = DGifOpenFileName(gifFileName.c_str(), &nErrorCode);
        if (dec == nullptr) {
            return false;
        }
        std::vector<uint8_t> emptyFileData;
        return m_impl->InitImageData(dec, emptyFileData, bLoadAllFrames, bAsyncDecode, fImageSizeScale, rcMaxDestRectSize);
    }
}

bool Image_GIF::IsDelayDecodeEnabled() const
{
    if (m_impl->m_bAsyncDecode && (m_impl->m_gifDecoder != nullptr)) {
        return true;
    }
    return false;
}

bool Image_GIF::IsDelayDecodeFinished() const
{
    if (m_impl->m_bAsyncDecoding) {
        return false;
    }
    if (m_impl->m_bDecodeError || (m_impl->m_gifDecoder == nullptr)) {
        return true;
    }
    return (int32_t)(m_impl->m_frames.size() + m_impl->m_delayFrames.size()) == m_impl->m_nFrameCount;
}

uint32_t Image_GIF::GetDecodedFrameIndex() const
{
    if (m_impl->m_frames.empty()) {
        return 0;
    }
    else {
        return (uint32_t)m_impl->m_frames.size() - 1;
    }
}

bool Image_GIF::DelayDecode(uint32_t nMinFrameIndex, std::function<bool(void)> IsAborted, bool* bDecodeError)
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
        const int32_t nFrameIndex = (int32_t)(m_impl->m_delayFrames.size() + m_impl->m_frames.size());
        AnimationFramePtr pNewAnimationFrame;
        pNewAnimationFrame = UiGifToRgbaFrames(m_impl->m_gifFrameSequence,
                                               nFrameIndex,
                                               fImageSizeScale,
                                               m_impl->m_gifCanvas,
                                               m_impl->m_nLastFrameIndex);
        if (pNewAnimationFrame != nullptr) {
            pNewAnimationFrame->SetDelayMs(GetFrameDelayMs(nFrameIndex));
            m_impl->m_delayFrames.push_back(pNewAnimationFrame);
        }
        else {
            bRet = false;
            m_impl->m_bDecodeError = true;
            if (bDecodeError != nullptr) {
                *bDecodeError = true;
            }
            break;
        }
    }

    m_impl->m_bAsyncDecoding = false;
    return bRet;
}

bool Image_GIF::MergeDelayDecodeData()
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
        bool bDecodeFinished = (int32_t)m_impl->m_frames.size() == m_impl->m_nFrameCount;
        if (bDecodeFinished || m_impl->m_bDecodeError) {
            m_impl->ClearImageData();
        }
    }
    return bRet;
}

uint32_t Image_GIF::GetWidth() const
{
    return m_impl->m_nWidth;
}

uint32_t Image_GIF::GetHeight() const
{
    return m_impl->m_nHeight;
}

float Image_GIF::GetImageSizeScale() const
{
    return m_impl->m_fImageSizeScale;
}

int32_t Image_GIF::GetFrameCount() const
{
    return m_impl->m_nFrameCount;
}

int32_t Image_GIF::GetLoopCount() const
{
    return m_impl->m_nLoops;
}

bool Image_GIF::IsFrameDataReady(uint32_t nFrameIndex)
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

int32_t Image_GIF::GetFrameDelayMs(uint32_t nFrameIndex)
{
    if (nFrameIndex < m_impl->m_frames.size()) {
        auto pAnimationFrame = m_impl->m_frames[nFrameIndex];
        if (pAnimationFrame != nullptr) {
            return pAnimationFrame->GetDelayMs();
        }
    }
    if (nFrameIndex < m_impl->m_framesDelayMs.size()) {
        IAnimationImage::AnimationFrame frame;
        frame.SetDelayMs(m_impl->m_framesDelayMs[nFrameIndex]);
        return frame.GetDelayMs();
    }
    return IMAGE_ANIMATION_DELAY_MS;
}

bool Image_GIF::ReadFrameData(int32_t nFrameIndex, const UiSize& /*szDestRectSize*/, AnimationFrame* pAnimationFrame)
{
    GlobalManager::Instance().AssertUIThread();
    ASSERT(pAnimationFrame != nullptr);
    if (pAnimationFrame == nullptr) {
        return false;
    }
    pAnimationFrame->m_bDataPending = true;
    pAnimationFrame->m_bDataError = false;
    ASSERT((nFrameIndex >= 0) && (nFrameIndex < m_impl->m_nFrameCount));
    if ((nFrameIndex < 0) || (nFrameIndex >= m_impl->m_nFrameCount)) {
        return false;
    }
    ASSERT(m_impl->m_nFrameCount > 0);
    if (m_impl->m_nFrameCount <= 0) {
        m_impl->m_bDecodeError = true;
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

            //Decode one frame at a time
            AnimationFramePtr pNewAnimationFrame;
            pNewAnimationFrame = UiGifToRgbaFrames(m_impl->m_gifFrameSequence,
                                                   nInitFrameIndex,
                                                   fImageSizeScale,
                                                   m_impl->m_gifCanvas,
                                                   m_impl->m_nLastFrameIndex);

            if (pNewAnimationFrame != nullptr) {
                pNewAnimationFrame->SetDelayMs(GetFrameDelayMs(nFrameIndex));
                m_impl->m_frames.push_back(pNewAnimationFrame);
            }
            else {
                //Image decoding error
                m_impl->m_bDecodeError = true;
                pAnimationFrame->m_bDataError = true;
                break;
            }
        }

        if (m_impl->IsDecodeFinished()) {
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
