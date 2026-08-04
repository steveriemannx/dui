#ifndef UI_IMAGE_APNG_DECODER_H_
#define UI_IMAGE_APNG_DECODER_H_

#include "dui/Core/UiTypes.h"
#include "third_party/zlib/zlib.h"
#include "third_party/libpng/pngpriv.h"
#include "third_party/libpng/pngstruct.h"
#include "third_party/libpng/pnginfo.h"
#include "third_party/libpng/png.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <fstream>

namespace ui
{
/** APNG decoder class, used to load and process APNG (Animated Portable Network Graphics) format images
    This class wraps the APNG decoding functionality of the libpng library and provides an object-oriented interface, simplifying the loading, frame management, and resource release flow of APNG images
 */
class APngDecoder
{
public:
    /** Constructor
     */
    APngDecoder();

    /** Destructor, automatically releases all resources
     */
    ~APngDecoder();

    /** Copy constructor is forbidden
     */
    APngDecoder(const APngDecoder&) = delete;

    /** Assignment operator is forbidden
     */
    APngDecoder& operator=(const APngDecoder&) = delete;

    /** Load an APNG image from memory data
     * @param pBuf Pointer to the image data
     * @param nLen The length of the image data (in bytes)
     * @param bLoadAllFrames Whether to load all frames; false loads only the first frame
     * @return Returns true on successful loading, false on failure
     */
    bool LoadFromMemory(const uint8_t* pBuf, size_t nLen, bool bLoadAllFrames);

    /** Load an APNG image from a file
     * @param filePath The file path
     * @param bLoadAllFrames Whether to load all frames; false loads only the first frame
     * @return Returns true on successful loading, false on failure
     */
    bool LoadFromFile(const std::string& filePath, bool bLoadAllFrames);

    /** Decode the next frame (used for step-by-step decoding)
     * @return Returns true on successful decoding; returns false when all frames are decoded or on failure
     */
    bool DecodeNextFrame();

    /** Get the decoding progress
     * @param[out] pCurFrame The number of frames currently decoded (note: not the frame index)
     * @param[out] pTotalFrames The total frame count
     */
    void GetProgress(int32_t* pCurFrame, int32_t* pTotalFrames) const;

    /** Get the image width
     * @return The image width (pixels), or 0 if not loaded successfully
     */
    int32_t GetWidth() const;

    /** Get the image height
     * @return The image height (pixels), or 0 if not loaded successfully
     */
    int32_t GetHeight() const;

    /** Get the total frame count
     * @return The total frame count, or 0 if not loaded successfully
     */
    int32_t GetFrameCount() const;

    /** Get the number of loop plays
     * @return The loop count, 0 means infinite loop
     */
    int32_t GetLoopCount() const;

    /** Get the delay time of the specified frame
     * @param frameIndex The frame index (starting from 0)
     * @return The delay time (milliseconds), or 0 if the index is invalid
     */
    int32_t GetFrameDelay(int32_t frameIndex) const;

    /** Get the pixel data of the specified frame (non-premultiplied Alpha)
     * The pixel format is RGBA8888 (4 bytes per pixel, in the order of red, green, blue, alpha channels)
     * @param frameIndex The frame index (starting from 0)
     * @return Pointer to the pixel data, or nullptr if the index is invalid
     */
    const uint8_t* GetFrameData(int32_t frameIndex) const;

    /** Get the pixel data of the specified frame (premultiplied Alpha)
     *
     * The pixel format is RGBA8888, where the RGB channel values have been premultiplied with the Alpha channel value,
     * i.e., R = R * A / 255, G = G * A / 255, B = B * A / 255
     *
     * @param frameIndex The frame index (starting from 0)
     * @param[out] outData The buffer used to store the premultiplied data; ensure the buffer size is sufficient
     *                     (the size should be: width * height * 4 bytes)
     * @return Returns true on success, false on failure
     */
    bool GetFrameDataPremultiplied(int32_t frameIndex, uint8_t* outData) const;

    /** Release all resources and restore to the not-loaded state
     */
    void Destroy();

public:
    // PNG warning callback function
    static void PngWarningCallback(png_structp png_ptr, png_const_charp message);

    // PNG error callback function
    static void PngErrorCallback(png_structp png_ptr, png_const_charp message);

private:
    // Internal PNG reader interface
    struct IPngReader
    {
        virtual ~IPngReader() = default;
        virtual size_t read(png_bytep data, png_size_t length) = 0;
    };

    // Memory reader implementation
    struct MemReader : public IPngReader
    {
        const char* pbuf;
        size_t nLen;

        MemReader(const char* buf, size_t len) : pbuf(buf), nLen(len) {}
        // Read data into the libpng buffer
        virtual size_t read(png_bytep data, png_size_t length) override;
    };

    // File reader implementation
    struct FileReader : public IPngReader
    {
        std::ifstream fs;

        FileReader(const std::string& filePath);
        // Read data into the libpng buffer
        virtual size_t read(png_bytep data, png_size_t length) override;
    };

    // libpng read callback function
    static void PngReadData(png_structp png_ptr, png_bytep data, png_size_t length);

    // Internal implementation of actually loading PNG/APNG
    bool LoadPng(IPngReader* reader, bool bLoadAllFrames);

    // Decode state enum
    enum class State
    {
        INIT,           // Initial state
        READ_HEADER,    // Header information has been read
        DECODING_FRAMES,// Decoding frames
        FINISHED,       // Decoding finished
        FAILED          // Error state
    };

    // Image width
    int32_t m_width = 0;

    // Image height
    int32_t m_height = 0;

    // Total frame count
    int32_t m_frameCount = 0;

    // Loop count
    int32_t m_loopCount = 0;

    // Current decoded frame index
    int32_t m_currentFrame = 0;

    // Delay time of each frame (milliseconds)
    std::unique_ptr<uint16_t[]> m_frameDelays;

    // Pixel data of all frames (RGBA8888)
    std::unique_ptr<uint8_t[]> m_frameData;

    // Size of single-frame data (bytes) = width * height * 4
    size_t m_frameSize = 0;

    // Number of bytes per row
    size_t m_bytesPerRow = 0;

    // Current frame buffer (used for frame compositing)
    std::unique_ptr<uint8_t[]> m_currentFrameBuffer;

    // Temporary frame data buffer
    std::unique_ptr<uint8_t[]> m_frameBuffer;

    // Row pointer array (for libpng)
    std::unique_ptr<png_bytep[]> m_rowPointers;

    // libpng structure
    png_structp m_pngPtr = nullptr;

    // libpng info structure
    png_infop m_infoPtr = nullptr;

    // Data reader
    std::unique_ptr<IPngReader> m_reader;

    // Decode state
    State m_state = State::INIT;

    // Whether to load all frames
    bool m_loadAllFrames = true;
};

}//namespace ui

#endif // UI_IMAGE_APNG_DECODER_H_
