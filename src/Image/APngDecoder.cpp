#include "duilib/Image/APngDecoder.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cstring>
#include <stdexcept>

// Cross-platform debug output
#ifdef _DEBUG
    #if defined(_WIN32) || defined(_WIN64)
        #define DEBUG_OUTPUT(msg) OutputDebugStringA(msg)
    #else
        #define DEBUG_OUTPUT(msg) fprintf(stderr, "%s", msg)
    #endif
#else
    #define DEBUG_OUTPUT(msg)
#endif

#define SASSERT(x) assert(x)

namespace ui
{

// Implementation of the memory reader
size_t APngDecoder::MemReader::read(png_bytep data, png_size_t length)
{
    if (nLen < length) {
        length = nLen;
    }
    memcpy(data, pbuf, length);
    pbuf += length;
    nLen -= length;
    return length;
}

// Implementation of the file reader
APngDecoder::FileReader::FileReader(const std::string& filePath)
{
    // Open the file in binary mode
    fs.open(filePath, std::ios::binary);
}

size_t APngDecoder::FileReader::read(png_bytep data, png_size_t length)
{
    if (!fs) {
        return 0;
    }
    fs.read(reinterpret_cast<char*>(data), length);
    return static_cast<size_t>(fs.gcount());
}

// PNG warning callback
void APngDecoder::PngWarningCallback(png_structp /*png_ptr*/, png_const_charp message)
{
   // (void)png_ptr;
    std::string warningMsg;
    if (message != nullptr) {
        warningMsg = StringUtil::Printf("PNG decoding warning: %s", message);
    }
#if defined(_WIN32) || defined(_WIN64)
    DEBUG_OUTPUT(warningMsg.c_str());
#endif
}

// PNG error callback
void APngDecoder::PngErrorCallback(png_structp /*png_ptr*/, png_const_charp message)
{
    std::string errMsg;
    if (message != nullptr) {
        errMsg = StringUtil::Printf("PNG decoding error: %s", message);
    }
    DEBUG_OUTPUT(errMsg.c_str());
}

// libpng read callback
void APngDecoder::PngReadData(png_structp png_ptr, png_bytep data, png_size_t length)
{
    if (!png_ptr) {
        return;
    }

    IPngReader* reader = static_cast<IPngReader*>(png_get_io_ptr(png_ptr));
    size_t read = reader->read(data, length);
    if (read < length) {
        png_error(png_ptr, "Read error");
    }
}

// Constructor
APngDecoder::APngDecoder() = default;

// Destructor
APngDecoder::~APngDecoder()
{
    Destroy();
}

// Load APNG from memory
bool APngDecoder::LoadFromMemory(const uint8_t* pBuf, size_t nLen, bool bLoadAllFrames)
{
    if ((pBuf == nullptr) || (nLen == 0)) {
        return false;
    }
    //Release the original resources first
    Destroy();
    try {
        m_reader = std::make_unique<MemReader>((const char*)pBuf, nLen);
        return LoadPng(m_reader.get(), bLoadAllFrames);
    }
    catch (...) {
        Destroy();
        return false;
    }
}

bool APngDecoder::LoadFromFile(const std::string& filePath, bool bLoadAllFrames)
{
    if (filePath.empty()) {
        return false;
    }

    // Release the original resources first
    Destroy();
    try {
        m_reader = std::make_unique<FileReader>(filePath);
        return LoadPng(m_reader.get(), bLoadAllFrames);
    }
    catch (...) {
        Destroy();
        return false;
    }
}

// Internal implementation of loading PNG
bool APngDecoder::LoadPng(IPngReader* reader, bool bLoadAllFrames)
{
    m_loadAllFrames = bLoadAllFrames;

    // Verify the file signature
    png_byte sig[8];
    if (reader->read(sig, 8) != 8 || !png_check_sig(sig, 8)) {
        m_state = State::FAILED;
        return false;
    }

    // Initialize the libpng structures
    m_pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                      PngErrorCallback, PngWarningCallback);
    if (!m_pngPtr) {
        m_state = State::FAILED;
        return false;
    }

    m_infoPtr = png_create_info_struct(m_pngPtr);
    if (!m_infoPtr) {
        png_destroy_read_struct(&m_pngPtr, nullptr, nullptr);
        m_pngPtr = nullptr;
        m_state = State::FAILED;
        return false;
    }

#pragma warning (push)
#pragma warning (disable: 4611)
    // Set up the error handling jump
    if (setjmp(png_jmpbuf(m_pngPtr))) {
        Destroy();
        return false;
    }
#pragma warning (pop)

    // Configure the read function
    png_set_read_fn(m_pngPtr, reader, PngReadData);
    png_set_sig_bytes(m_pngPtr, 8);

    // Image format conversion configuration
    png_set_expand(m_pngPtr);
    png_set_add_alpha(m_pngPtr, 0xff, PNG_FILLER_AFTER);
    png_set_interlace_handling(m_pngPtr);
    png_set_gray_to_rgb(m_pngPtr);
    png_set_strip_16(m_pngPtr);

    // Read the image information
    png_read_info(m_pngPtr, m_infoPtr);
    png_read_update_info(m_pngPtr, m_infoPtr);

    // Initialize the image parameters
    m_width = png_get_image_width(m_pngPtr, m_infoPtr);
    m_height = png_get_image_height(m_pngPtr, m_infoPtr);
    m_bytesPerRow = m_width * 4;
    m_frameSize = m_bytesPerRow * m_height;

    // Initialize the frame buffer
    m_frameBuffer = std::make_unique<uint8_t[]>(m_frameSize);
    m_currentFrameBuffer = std::make_unique<uint8_t[]>(m_frameSize);
    if (!m_frameBuffer || !m_currentFrameBuffer) {
        throw std::bad_alloc();
    }
    memset(m_currentFrameBuffer.get(), 0, m_frameSize);

    // Initialize the row pointers
    m_rowPointers = std::make_unique<png_bytep[]>(m_height);
    if (!m_rowPointers) {
        throw std::bad_alloc();
    }
    for (int32_t i = 0; i < m_height; ++i) {
        m_rowPointers[i] = m_frameBuffer.get() + m_bytesPerRow * i;
    }

    // Check whether it is an APNG
    if (!png_get_valid(m_pngPtr, m_infoPtr, PNG_INFO_acTL)) {
        // Non-animated PNG
        m_frameCount = 1;
        m_loopCount = 0;
        m_frameDelays = std::make_unique<uint16_t[]>(1);
        m_frameDelays[0] = 100; // Default delay 100ms
        m_frameData = std::make_unique<uint8_t[]>(m_frameSize);
    }
    else {
        // Animated PNG
        m_frameCount = png_get_num_frames(m_pngPtr, m_infoPtr);
        if (m_frameCount < 1) m_frameCount = 1;

        if (!m_loadAllFrames) m_frameCount = 1;

        m_loopCount = png_get_num_plays(m_pngPtr, m_infoPtr);
        m_frameDelays = std::make_unique<uint16_t[]>(m_frameCount);
        m_frameData = std::make_unique<uint8_t[]>(m_frameSize * m_frameCount);

        if (!m_frameDelays || !m_frameData) {
            throw std::bad_alloc();
        }
    }

    m_currentFrame = 0;
    m_state = State::READ_HEADER;
    return true;
}

// Decode the next frame
bool APngDecoder::DecodeNextFrame() {
    if (m_state == State::FINISHED || m_state == State::FAILED) {
        return false;
    }

    try {
#pragma warning (push)
#pragma warning (disable: 4611)
        if (setjmp(png_jmpbuf(m_pngPtr))) {
            m_state = State::FAILED;
            return false;
        }
#pragma warning (pop)

        // Handle a single-frame PNG
        if (m_frameCount == 1) {
            png_read_image(m_pngPtr, m_rowPointers.get());
            memcpy(m_frameData.get(), m_frameBuffer.get(), m_frameSize);
            m_currentFrame++;
            m_state = State::FINISHED;
            return true;
        }

        // Handle a multi-frame APNG
        if (m_currentFrame < m_frameCount) {
            // Read the frame header information
            png_read_frame_head(m_pngPtr, m_infoPtr);

            // Calculate the delay time
            if (png_get_valid(m_pngPtr, m_infoPtr, PNG_INFO_fcTL)) {
                png_uint_16 delayNum = m_infoPtr->next_frame_delay_num;
                png_uint_16 delayDen = m_infoPtr->next_frame_delay_den;
                if (delayDen == 0) delayDen = 100;
                m_frameDelays[m_currentFrame] = static_cast<uint16_t>(
                    (delayNum * 1000.0) / delayDen);
            }
            else {
                m_frameDelays[m_currentFrame] = 100;
            }

            // Read the current frame data
            png_read_image(m_pngPtr, m_rowPointers.get());

            // Calculate the drawing position
            auto xOffset = m_infoPtr->next_frame_x_offset;
            auto yOffset = m_infoPtr->next_frame_y_offset;
            auto frameWidth = m_infoPtr->next_frame_width;
            auto frameHeight = m_infoPtr->next_frame_height;

            uint8_t* dstLine = m_currentFrameBuffer.get() +
                yOffset * m_bytesPerRow + xOffset * 4;
            uint8_t* srcLine = m_frameBuffer.get();

            // Frame blending operation
            switch (m_infoPtr->next_frame_blend_op) {
            case PNG_BLEND_OP_OVER: {
                for (unsigned int y = 0; y < frameHeight; ++y) {
                    uint8_t* dst = dstLine;
                    uint8_t* src = srcLine;
                    for (unsigned int x = 0; x < frameWidth; ++x) {
                        // Alpha blending calculation
                        uint8_t alpha = src[3];
                        dst[0] = static_cast<uint8_t>(
                            (dst[0] * (255 - alpha) + src[0] * alpha) / 255);
                        dst[1] = static_cast<uint8_t>(
                            (dst[1] * (255 - alpha) + src[1] * alpha) / 255);
                        dst[2] = static_cast<uint8_t>(
                            (dst[2] * (255 - alpha) + src[2] * alpha) / 255);
                        dst[3] = static_cast<uint8_t>(
                            (dst[3] * (255 - alpha) + src[3] * alpha) / 255);
                        dst += 4;
                        src += 4;
                    }
                    dstLine += m_bytesPerRow;
                    srcLine += m_bytesPerRow;
                }
                break;
            }
            case PNG_BLEND_OP_SOURCE: {
                // Direct overwrite
                for (unsigned int y = 0; y < frameHeight; ++y) {
                    memcpy(dstLine, srcLine, frameWidth * 4);
                    dstLine += m_bytesPerRow;
                    srcLine += m_bytesPerRow;
                }
                break;
            }
            default:
                SASSERT(false);
                break;
            }

            // Save the current frame
            uint8_t* targetFrame = m_frameData.get() + m_currentFrame * m_frameSize;
            memcpy(targetFrame, m_currentFrameBuffer.get(), m_frameSize);

            // Handle the frame disposal operation
            dstLine = m_currentFrameBuffer.get() + yOffset * m_bytesPerRow + xOffset * 4;

            switch (m_infoPtr->next_frame_dispose_op) {
            case PNG_DISPOSE_OP_BACKGROUND:
                // Clear the background
                for (unsigned int y = 0; y < frameHeight; ++y) {
                    memset(dstLine, 0, frameWidth * 4);
                    dstLine += m_bytesPerRow;
                }
                break;
            case PNG_DISPOSE_OP_PREVIOUS:
                // Restore the previous frame
                if (m_currentFrame > 0) {
                    memcpy(m_currentFrameBuffer.get(),
                        targetFrame - m_frameSize,
                        m_frameSize);
                }
                break;
            case PNG_DISPOSE_OP_NONE:
                // No processing
                break;
            default:
                SASSERT(false);
                break;
            }

            m_currentFrame++;
            if (m_currentFrame >= m_frameCount) {
                m_state = State::FINISHED;
            }
            return true;
        }
    }
    catch (...) {
        m_state = State::FAILED;
        return false;
    }

    return false;
}

// Get the decoding progress
void APngDecoder::GetProgress(int32_t* pCurFrame, int32_t* pTotalFrames) const
{
    if (pCurFrame != nullptr) {
        *pCurFrame = m_currentFrame;
    }
    if (pTotalFrames != nullptr) {
        *pTotalFrames = m_frameCount;
    }
}

// Get the width
int32_t APngDecoder::GetWidth() const
{
    return m_width;
}

// Get the height
int32_t APngDecoder::GetHeight() const
{
    return m_height;
}

// Get the frame count
int32_t APngDecoder::GetFrameCount() const
{
    return m_frameCount;
}

// Get the loop count
int32_t APngDecoder::GetLoopCount() const
{
    return m_loopCount;
}

// Get the delay of the specified frame
int32_t APngDecoder::GetFrameDelay(int32_t frameIndex) const
{
    if (frameIndex < 0 || frameIndex >= m_frameCount || !m_frameDelays) {
        return 0;
    }
    return (int32_t)m_frameDelays[frameIndex];
}

// Get the data of the specified frame (non-premultiplied)
const uint8_t* APngDecoder::GetFrameData(int32_t frameIndex) const
{
    if (frameIndex < 0 || frameIndex >= m_frameCount || !m_frameData) {
        return nullptr;
    }
    return (const uint8_t*)(m_frameData.get() + frameIndex * m_frameSize);
}

// Get the data of the specified frame (premultiplied Alpha)
bool APngDecoder::GetFrameDataPremultiplied(int32_t frameIndex, uint8_t* outData) const
{
    // Check the parameter validity
    if (frameIndex < 0 || frameIndex >= m_frameCount || !m_frameData || !outData) {
        return false;
    }

    // Get the original frame data
    const uint8_t* srcData = (const uint8_t*)(m_frameData.get() + frameIndex * m_frameSize);
    if (!srcData) {
        return false;
    }

    // Calculate the total pixel count
    size_t pixelCount = static_cast<size_t>(m_width) * m_height;

    // Perform the premultiply operation on each pixel
    size_t pixelIndex = 0;
    for (size_t i = 0; i < pixelCount; ++i) {
        pixelIndex = i * 4;
        // Read the original RGBA values
        uint8_t r = srcData[pixelIndex];
        uint8_t g = srcData[pixelIndex + 1];
        uint8_t b = srcData[pixelIndex + 2];
        uint8_t a = srcData[pixelIndex + 3];

        // Perform the premultiply calculation: R' = (R * A) / 255
        // Use integer arithmetic to avoid floating-point precision issues
#if defined (_WIN32) || defined (_WIN64)
        //Data format: BGRA on Windows, RGBA on other platforms
        outData[pixelIndex] = static_cast<uint8_t>(((uint32_t)b * (uint32_t)a) / 255);
        outData[pixelIndex + 1] = static_cast<uint8_t>(((uint32_t)g * (uint32_t)a) / 255);
        outData[pixelIndex + 2] = static_cast<uint8_t>(((uint32_t)r * (uint32_t)a) / 255);
        outData[pixelIndex + 3] = a;  // The Alpha value remains unchanged
#else
        outData[pixelIndex] = static_cast<uint8_t>(((uint32_t)r * (uint32_t)a) / 255);
        outData[pixelIndex + 1] = static_cast<uint8_t>(((uint32_t)g * (uint32_t)a) / 255);
        outData[pixelIndex + 2] = static_cast<uint8_t>(((uint32_t)b * (uint32_t)a) / 255);
        outData[pixelIndex + 3] = a;  // The Alpha value remains unchanged
#endif
    }
    return true;
}

// Release all resources
void APngDecoder::Destroy()
{
    // Release the libpng resources
    if (m_pngPtr && m_infoPtr) {
        png_destroy_read_struct(&m_pngPtr, &m_infoPtr, nullptr);
        m_pngPtr = nullptr;
        m_infoPtr = nullptr;
    }

    // Reset the member variables
    m_width = 0;
    m_height = 0;
    m_frameCount = 0;
    m_loopCount = 0;
    m_currentFrame = 0;
    m_frameSize = 0;
    m_bytesPerRow = 0;
    m_frameDelays.reset();
    m_frameData.reset();
    m_currentFrameBuffer.reset();
    m_frameBuffer.reset();
    m_rowPointers.reset();
    m_reader.reset();
    m_state = State::INIT;
    m_loadAllFrames = true;
}

}//namespace ui
