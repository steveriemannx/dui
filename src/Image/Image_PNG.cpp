#include "dui/Image/Image_PNG.h"
#include "dui/Image/APngDecoder.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Render/IRender.h"

#include <vector>
#include <cmath>
#include <fstream>
#include <atomic>

namespace ui
{
//The encapsulation of the PNG/APNG decoding functionality
namespace ReadPngHeader
{
    struct FrameDuration {
        uint32_t sequence;      // Frame sequence number
        uint16_t delay_num;     // Delay numerator
        uint16_t delay_den;     // Delay denominator
        uint32_t duration_ms;   // Converted milliseconds
    };

    struct PngImageInfo {
        uint32_t width;         //Image width
        uint32_t height;        //Image height
        bool is_apng;           //true means it is an APNG image, false means it is a normal PNG image
        uint32_t frame_count;   //Total frame count of the image
        int32_t  loop_count;    //Number of playback loops
        std::vector<FrameDuration> frames; //Playback delay time of each frame
    };

    // Custom chunk callback function
    // Note: the return value type is int, used to indicate whether to keep the chunk
    static int read_chunk_callback(png_structp png_ptr, png_unknown_chunkp chunk)
    {
        // In libpng 1.6, the return value controls whether to keep the chunk
        int keep = 0;

        if ((chunk->size == 26) && (memcmp(chunk->name, "fcTL", 4) == 0)) {
            PngImageInfo* imageInfo = (PngImageInfo*)png_get_user_chunk_ptr(png_ptr);
            FrameDuration fd;
            // Parse the fcTL chunk data (APNG frame control chunk)
            fd.sequence = png_get_uint_32(chunk->data);                  // Bytes 0-3: frame sequence number
            fd.delay_num = png_get_uint_16(chunk->data + 20);             // Bytes 20-21: delay numerator
            fd.delay_den = png_get_uint_16(chunk->data + 22);             // Bytes 22-23: delay denominator

            // Calculate the milliseconds (rounded)
            if (fd.delay_den == 0) {
                fd.duration_ms = fd.delay_num * 10;  // Default denominator of 100 (1000/100=10)
            }
            else {
                fd.duration_ms = static_cast<unsigned int>(
                    std::round(static_cast<double>(fd.delay_num) * 1000 / fd.delay_den)
                    );
            }
            if (fd.duration_ms == 0) {
                fd.duration_ms = 100; //Default value
            }
            if (imageInfo != nullptr) {
                imageInfo->frames.push_back(fd);
                keep = 1; // Keep this chunk
            }
        }
        return keep; // Returning 1 means keep the chunk, 0 means not keep
    }

    // Parse from the memory data (overloaded version)
    struct PngBuffer {
        const uint8_t* data;
        size_t size;
        size_t offset;
    };

    //The callback function for reading data from memory
    static void png_read_mem_callback(png_structp png, png_bytep data, png_size_t length) {
        PngBuffer* buf = static_cast<PngBuffer*>(png_get_io_ptr(png));
        if (buf->offset + length > buf->size) {
            png_error(png, "Read error");
            return;
        }
        memcpy(data, buf->data + buf->offset, length);
        buf->offset += length;
    }

    //Load the image information from the memory data
    bool load_apng_image_info(const std::vector<uint8_t>& fileData, PngImageInfo& pngImageInfo)
    {
        pngImageInfo.width = 0;
        pngImageInfo.height = 0;
        pngImageInfo.is_apng = false;
        pngImageInfo.frame_count = 0;
        pngImageInfo.loop_count = 0;
        pngImageInfo.frames.clear();
        if (fileData.size() < 8) {
            return false;  // At least 8 bytes of signature are required
        }

        // Check the PNG signature
        if (png_sig_cmp(reinterpret_cast<const png_byte*>(fileData.data()), 0, 8) != 0) {
            return false;
        }

        png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                                 APngDecoder::PngErrorCallback, APngDecoder::PngWarningCallback);
        if (!png) {
            return false;
        }

        png_infop info = png_create_info_struct(png);
        if (!info) {
            png_destroy_read_struct(&png, nullptr, nullptr);
            return false;
        }
#pragma warning (push)
#pragma warning (disable: 4611)
        if (setjmp(png_jmpbuf(png))) {
            png_destroy_read_struct(&png, &info, nullptr);
            return false;
        }
#pragma warning (pop)

        // Set the memory read callback and user data
        PngBuffer buf = { fileData.data(), fileData.size(), 0 };
        png_set_read_fn(png, &buf, png_read_mem_callback);

        // Register the user chunk callback
        png_set_read_user_chunk_fn(png, &pngImageInfo, read_chunk_callback);
        const png_byte chunk_name[] = "fcTL";
        png_set_keep_unknown_chunks(png, PNG_HANDLE_CHUNK_ALWAYS, chunk_name, 1);

        png_read_info(png, info);
        png_read_update_info(png, info);

        pngImageInfo.width = info->width;
        pngImageInfo.height = info->height;

        if (!png_get_valid(png, info, PNG_INFO_acTL)) {
            //Single-frame image
            pngImageInfo.frame_count = 1;
            pngImageInfo.loop_count = 0;
            pngImageInfo.is_apng = false;
        }
        else {
            pngImageInfo.is_apng = true;
        }
        png_read_end(png, info);
        if (pngImageInfo.is_apng) {
            pngImageInfo.frame_count = (uint32_t)pngImageInfo.frames.size();
            pngImageInfo.loop_count = png_get_num_plays(png, info);
        }
        png_destroy_read_struct(&png, &info, nullptr);

        ASSERT((pngImageInfo.width > 0) && (pngImageInfo.height > 0));
        if ((pngImageInfo.width <= 0) || (pngImageInfo.height <= 0)) {
            return false;
        }
        ASSERT(pngImageInfo.frame_count > 0);
        if (pngImageInfo.frame_count <= 0) {
            return false;
        }
        if (pngImageInfo.frame_count > 1) {
            //Multi-frame image
            ASSERT(pngImageInfo.frame_count == pngImageInfo.frames.size());
            if (pngImageInfo.frame_count != pngImageInfo.frames.size()) {
                return false;
            }
            if (pngImageInfo.loop_count <= 0) {
                pngImageInfo.loop_count = -1;
            }
        }
        return true;
    }

    // The callback function for reading data from a file
    static void png_read_file_callback(png_structp png, png_bytep data, png_size_t length) {
        std::ifstream* fp = static_cast<std::ifstream*>(png_get_io_ptr(png));
        fp->read(reinterpret_cast<char*>(data), length);
        if (!fp->good()) {
            png_error(png, "Read error");
        }
    }

    //Load the image information from a file
    bool load_apng_image_info(const std::string& filePath, PngImageInfo& pngImageInfo)
    {
        // Initialize the output structure
        pngImageInfo.width = 0;
        pngImageInfo.height = 0;
        pngImageInfo.is_apng = false;
        pngImageInfo.frame_count = 0;
        pngImageInfo.loop_count = 0;
        pngImageInfo.frames.clear();

        // Open the file
        std::ifstream fp(filePath, std::ios::binary);
        if (!fp) {
            return false;
        }

        // Check the PNG signature (8 bytes)
        png_byte sig[8] = {0};
        fp.read(reinterpret_cast<char*>(sig), 8);
        if (!fp || png_sig_cmp(sig, 0, 8) != 0) {
            return false;
        }

        // Create png_struct and png_info
        png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                                 APngDecoder::PngErrorCallback, APngDecoder::PngWarningCallback);
        if (!png) {
            return false;
        }

        png_infop info = png_create_info_struct(png);
        if (!info) {
            png_destroy_read_struct(&png, nullptr, nullptr);
            return false;
        }

        // Set up the error handling (libpng uses setjmp/longjmp)
#pragma warning(push)
#pragma warning(disable: 4611)
        if (setjmp(png_jmpbuf(png))) {
            png_destroy_read_struct(&png, &info, nullptr);
            return false;
        }
#pragma warning(pop)

        // Set the file read callback
        png_set_read_fn(png, &fp, png_read_file_callback);

        // Tell libpng that we have already read the 8-byte signature
        png_set_sig_bytes(png, 8);

        // Register the user chunk callback
        png_set_read_user_chunk_fn(png, &pngImageInfo, read_chunk_callback);
        const png_byte chunk_name[] = "fcTL";
        png_set_keep_unknown_chunks(png, PNG_HANDLE_CHUNK_ALWAYS, chunk_name, 1);

        png_read_info(png, info);
        png_read_update_info(png, info);

        pngImageInfo.width = info->width;
        pngImageInfo.height = info->height;

        if (!png_get_valid(png, info, PNG_INFO_acTL)) {
            //Single-frame image
            pngImageInfo.frame_count = 1;
            pngImageInfo.loop_count = 0;
            pngImageInfo.is_apng = false;
        }
        else {
            pngImageInfo.is_apng = true;
        }
        png_read_end(png, info);
        if (pngImageInfo.is_apng) {
            pngImageInfo.frame_count = (uint32_t)pngImageInfo.frames.size();
            pngImageInfo.loop_count = png_get_num_plays(png, info);
        }
        png_destroy_read_struct(&png, &info, nullptr);

        ASSERT((pngImageInfo.width > 0) && (pngImageInfo.height > 0));
        if ((pngImageInfo.width <= 0) || (pngImageInfo.height <= 0)) {
            return false;
        }
        ASSERT(pngImageInfo.frame_count > 0);
        if (pngImageInfo.frame_count <= 0) {
            return false;
        }
        if (pngImageInfo.frame_count > 1) {
            //Multi-frame image
            ASSERT(pngImageInfo.frame_count == pngImageInfo.frames.size());
            if (pngImageInfo.frame_count != pngImageInfo.frames.size()) {
                return false;
            }
            if (pngImageInfo.loop_count <= 0) {
                pngImageInfo.loop_count = -1;
            }
        }
        return true;
    }
}

struct Image_PNG::TImpl
{
public:
    //File data
    std::vector<uint8_t> m_fileData;

    //File path
    FilePath m_imageFilePath;

    //Image width (the value scaled according to m_fImageSizeScale)
    uint32_t m_nWidth = 0;

    //Image height (the value scaled according to m_fImageSizeScale)
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
    //Image data decoder
    std::unique_ptr<APngDecoder> m_pImageDecoder;

    //Whether asynchronous thread decoding of the image data is supported
    bool m_bAsyncDecode = false;

    //Whether the image data is being decoded
    std::atomic<bool> m_bAsyncDecoding = false;

    //Data of each image frame (data decoded with delay)
    std::vector<AnimationFramePtr> m_delayFrames;

public:
    //Initialize the image information from the successfully loaded file
    bool InitImageData(std::vector<uint8_t>& fileData,
                       const ReadPngHeader::PngImageInfo& pngImageInfo,
                       bool bLoadAllFrames,
                       bool bAsyncDecode,
                       float fImageSizeScale,
                       const UiSize& rcMaxDestRectSize)
    {
        m_fImageSizeScale = fImageSizeScale;
        m_bLoadAllFrames = bLoadAllFrames;
        m_bAsyncDecode = bAsyncDecode;

        //Only load the key information, do not decode the image data (record the total frame count, width and height)
        m_nWidth = pngImageInfo.width;
        m_nHeight = pngImageInfo.height;
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
        m_nFrameCount = pngImageInfo.frame_count;
        m_nLoops = pngImageInfo.loop_count;
        if (m_nLoops <= 0) {
            m_nLoops = -1;
        }
        if (m_bAssertEnabled) {
            ASSERT(m_nWidth > 0);
            ASSERT(m_nHeight > 0);
            ASSERT(m_nFrameCount > 0);
        }

        bool bLoaded = true;
        if ((m_nFrameCount <= 0) || ((int32_t)m_nWidth <= 0) || ((int32_t)m_nHeight <= 0)) {
            bLoaded = false;
        }
        else if (bLoadAllFrames) {
            //Support loading multiple frames
            m_framesDelayMs.clear();
            if (pngImageInfo.frame_count > 1) {
                for (const ReadPngHeader::FrameDuration& frameDuration : pngImageInfo.frames) {
                    AnimationFrame animFrame;
                    animFrame.SetDelayMs(frameDuration.duration_ms);
                    m_framesDelayMs.push_back(animFrame.GetDelayMs());
                }
            }
        }
        else {
            //Load as a single frame
            m_nFrameCount = 1;
        }
        if (!bLoaded) {
            m_fileData.swap(fileData);
        }
        return bLoaded;
    }

    //Clean up resources
    void ClearImageData()
    {
        m_pImageDecoder.reset();
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

Image_PNG::Image_PNG()
{
    m_impl = std::make_unique<TImpl>();
}

Image_PNG::~Image_PNG()
{
    m_impl->ClearImageData();
}

bool Image_PNG::LoadImageFile(std::vector<uint8_t>& fileData,
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
    m_impl->m_bDecodeError = false;
    m_impl->m_bAssertEnabled = bAssertEnabled;
    m_impl->m_imageFilePath = imageFilePath;
    m_impl->m_fileData.clear();
    m_impl->m_fileData.swap(fileData);

    if (!m_impl->m_fileData.empty()) {
        //Only load the key information, do not decode the image data
        ReadPngHeader::PngImageInfo pngImageInfo;
        bool bLoaded = ReadPngHeader::load_apng_image_info(m_impl->m_fileData, pngImageInfo);
        if (!bLoaded) {
            //When loading fails, the original file data needs to be restored
            m_impl->m_fileData.swap(fileData);
            return false;
        }
        return m_impl->InitImageData(fileData, pngImageInfo, bLoadAllFrames, bAsyncDecode, fImageSizeScale, rcMaxDestRectSize);
    }
    else {
        DStringA pngFileName = imageFilePath.NativePathA();
        ASSERT(!pngFileName.empty());
        ReadPngHeader::PngImageInfo pngImageInfo;
        bool bLoaded = ReadPngHeader::load_apng_image_info(pngFileName, pngImageInfo);
        if (!bLoaded) {
            return false;
        }
        m_impl->m_fileData.clear();
        m_impl->m_imageFilePath = imageFilePath;
        std::vector<uint8_t> emptyFileData;
        return m_impl->InitImageData(emptyFileData, pngImageInfo, bLoadAllFrames, bAsyncDecode, fImageSizeScale, rcMaxDestRectSize);
    }
}

AnimationFramePtr Image_PNG::DecodeImageFrame()
{
    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    ASSERT(pRenderFactory != nullptr);
    if (pRenderFactory == nullptr) {
        m_impl->m_bDecodeError = true;
        return nullptr;
    }

    float fImageSizeScale = m_impl->m_fImageSizeScale;
    if (m_impl->m_pImageDecoder == nullptr) {
        m_impl->m_pImageDecoder = std::make_unique<APngDecoder>();
        bool bLoaded = false;
        if (!m_impl->m_fileData.empty()) {
            bLoaded = m_impl->m_pImageDecoder->LoadFromMemory(m_impl->m_fileData.data(), m_impl->m_fileData.size(), m_impl->m_bLoadAllFrames);
        }
        else if (!m_impl->m_imageFilePath.IsEmpty()) {
            bLoaded = m_impl->m_pImageDecoder->LoadFromFile(m_impl->m_imageFilePath.NativePathA(), m_impl->m_bLoadAllFrames);
        }
        if (!bLoaded) {
            m_impl->m_pImageDecoder.reset();
        }
    }
    if (m_impl->m_pImageDecoder == nullptr) {
        //Clear the data and mark the error
        m_impl->m_bDecodeError = true;
        m_impl->ClearImageData();
        return nullptr;
    }
    AnimationFramePtr pFrameData;
    APngDecoder& pngDecoder = *(m_impl->m_pImageDecoder);
    if (m_impl->m_bAssertEnabled) {
        ASSERT(m_impl->m_nWidth == ImageUtil::GetScaledImageSize((uint32_t)pngDecoder.GetWidth(), fImageSizeScale));
        ASSERT(m_impl->m_nHeight == ImageUtil::GetScaledImageSize((uint32_t)pngDecoder.GetHeight(), fImageSizeScale));
    }
    if ((m_impl->m_nWidth != ImageUtil::GetScaledImageSize((uint32_t)pngDecoder.GetWidth(), fImageSizeScale)) ||
        (m_impl->m_nHeight != ImageUtil::GetScaledImageSize((uint32_t)pngDecoder.GetHeight(), fImageSizeScale))) {
        //Clear the data and mark the error
        m_impl->m_bDecodeError = true;
        m_impl->ClearImageData();
        return nullptr;
    }

    if (pngDecoder.DecodeNextFrame()) {
        int32_t nCurFrame = 0;
        int32_t nTotalFrames = 0;
        pngDecoder.GetProgress(&nCurFrame, &nTotalFrames);
        int32_t nFrameIndex = nCurFrame - 1;
        if (m_impl->m_bAssertEnabled) {
            ASSERT(nTotalFrames == m_impl->m_nFrameCount);
        }
        if (nTotalFrames != m_impl->m_nFrameCount) {
            //Data error, clear the data and mark the error
            m_impl->m_bDecodeError = true;
            m_impl->ClearImageData();
            return nullptr;
        }
        if (m_impl->m_bAssertEnabled) {
            ASSERT(nFrameIndex < m_impl->m_nFrameCount);
        }
        if (nFrameIndex >= m_impl->m_nFrameCount) {
            //Data error, clear the data and mark the error
            m_impl->m_bDecodeError = true;
            m_impl->ClearImageData();
            return nullptr;
        }

        pFrameData = std::make_shared<IAnimationImage::AnimationFrame>();
        pFrameData->m_nFrameIndex = nFrameIndex;
        pFrameData->SetDelayMs(pngDecoder.GetFrameDelay(nFrameIndex));
        pFrameData->m_nOffsetX = 0;
        pFrameData->m_nOffsetY = 0;
        pFrameData->m_bDataPending = false;
        pFrameData->m_pBitmap.reset(pRenderFactory->CreateBitmap());
        ASSERT(pFrameData->m_pBitmap != nullptr);
        if (pFrameData->m_pBitmap != nullptr) {
            std::vector<uint8_t> bitmapData;
            bitmapData.resize(pngDecoder.GetHeight() * pngDecoder.GetWidth() * 4);
            if (pngDecoder.GetFrameDataPremultiplied(nFrameIndex, bitmapData.data())) {
                bool bRet = pFrameData->m_pBitmap->Init(pngDecoder.GetWidth(), pngDecoder.GetHeight(), bitmapData.data(), fImageSizeScale);
                if (!bRet) {
                    ASSERT(!"Init bitmap data failed!");
                    pFrameData.reset();
                }
            }
            else {
                ASSERT(!"GetFrameDataPremultiplied failed!");
                pFrameData.reset();
            }
        }
    }
    return pFrameData;
}

bool Image_PNG::IsDelayDecodeEnabled() const
{
    if ((m_impl->m_bAsyncDecode && !m_impl->m_bDecodeError) &&
        (!m_impl->m_fileData.empty() || !m_impl->m_imageFilePath.IsEmpty())) {
        //Multi-threaded decoding is supported only for multi-frame images
        return true;
    }
    return false;
}

bool Image_PNG::IsDelayDecodeFinished() const
{
    if (m_impl->m_bAsyncDecoding) {
        return false;
    }
    if (m_impl->m_bDecodeError) {
        return true;
    }
    return (int32_t)(m_impl->m_frames.size() + m_impl->m_delayFrames.size()) == m_impl->m_nFrameCount;
}

uint32_t Image_PNG::GetDecodedFrameIndex() const
{
    if (m_impl->m_frames.empty()) {
        return 0;
    }
    else {
        return (uint32_t)m_impl->m_frames.size() - 1;
    }
}

bool Image_PNG::DelayDecode(uint32_t nMinFrameIndex, std::function<bool(void)> IsAborted, bool* bDecodeError)
{
    if (!IsDelayDecodeEnabled()) {
        ASSERT(0);
        return false;
    }
    if (m_impl->m_bAsyncDecoding) {
        ASSERT(0);
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

    bool bDecodeResult = false;
    while (((IsAborted == nullptr) || !IsAborted()) &&
           (nMinFrameIndex >= (m_impl->m_frames.size() + m_impl->m_delayFrames.size())) &&
           ((m_impl->m_frames.size() + m_impl->m_delayFrames.size()) < nFrameCount)) {
        //Decode one frame at a time
        AnimationFramePtr pNewAnimationFrame = DecodeImageFrame();
        if (pNewAnimationFrame != nullptr) {
            bDecodeResult = true;
            m_impl->m_delayFrames.push_back(pNewAnimationFrame);
        }
        else {
            //Decoding error
            bDecodeResult = false;
            m_impl->m_bDecodeError = true;
            if (bDecodeError != nullptr) {
                *bDecodeError = true;
            }
            break;
        }
    }

    m_impl->m_bAsyncDecoding = false;
    if (!bDecodeResult) {
        ASSERT(m_impl->m_bDecodeError);
    }
    return bDecodeResult;
}

bool Image_PNG::MergeDelayDecodeData()
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
        //If the decoding is finished or an error occurred, release the image resources
        if (m_impl->IsDecodeFinished()) {
            m_impl->ClearImageData();
        }
    }
    return bRet;
}

uint32_t Image_PNG::GetWidth() const
{
    return m_impl->m_nWidth;
}

uint32_t Image_PNG::GetHeight() const
{
    return m_impl->m_nHeight;
}

float Image_PNG::GetImageSizeScale() const
{
    return m_impl->m_fImageSizeScale;
}

int32_t Image_PNG::GetFrameCount() const
{
    return m_impl->m_nFrameCount;
}

int32_t Image_PNG::GetLoopCount() const
{
    return m_impl->m_nLoops;
}

bool Image_PNG::IsFrameDataReady(uint32_t nFrameIndex)
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

int32_t Image_PNG::GetFrameDelayMs(uint32_t nFrameIndex)
{
    GlobalManager::Instance().AssertUIThread();
    if (nFrameIndex < m_impl->m_framesDelayMs.size()) {
        return m_impl->m_framesDelayMs[nFrameIndex];
    }
    return IMAGE_ANIMATION_DELAY_MS;
}

bool Image_PNG::ReadFrameData(int32_t nFrameIndex, const UiSize& /*szDestRectSize*/, AnimationFrame* pAnimationFrame)
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

            AnimationFramePtr pNewAnimationFrame = DecodeImageFrame();
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
            //Decoding is finished, release the resources
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
