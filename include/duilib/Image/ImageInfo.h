#ifndef UI_IMAGE_IMAGE_INFO_H_
#define UI_IMAGE_IMAGE_INFO_H_

#include "duilib/Render/IRender.h"
#include "duilib/Core/UiTypes.h"
#include "duilib/Image/ImageDecoder.h"
#include "duilib/Image/ImageLoadParam.h"

namespace ui
{
    class IRender;
    class Control;
    class DpiManager;

/** Image information
*/
class DUILIB_API ImageInfo
{
public:
    ImageInfo();
    ~ImageInfo();

    ImageInfo(const ImageInfo&) = delete;
    ImageInfo& operator = (const ImageInfo&) = delete;

public:
    /** Get the UI interface DPI scaling ratio when loading the image (example: 100 means the scaling percentage is 100%, no scaling)
    */
    uint32_t GetLoadDpiScale() const;

    /** Get the DPI scaling ratio of the DPI-adaptive image (example: an image with a DPI scaling percentage of 120 (i.e., enlarged to 120%): "image.png" corresponds to "image@120.png")
    */
    uint32_t GetImageFileDpiScale() const;

    /** Get the image width (note: this width is not necessarily the same as the width of the image bitmap; this width is used to calculate the layout width)
    *   Single-frame image/SVG image: this width is consistent with the image width
    *
    */
    int32_t GetWidth() const;

    /** Get the image height (note: this width is not necessarily the same as the width of the image bitmap; this width is used to calculate the layout width)
    */
    int32_t GetHeight() const;

    /** The width and height scaling ratio of the original image when loaded (1.0f means no scaling)
    */
    float GetImageSizeScale() const;

public:
    /** Whether it is an SVG image
    */
    bool IsSvgImage() const;

    /** Get the bitmap of the SVG image, supports vector scaling
    * @param [in] rcDest The drawing destination area
    * @param [in] rcSource The image source area
    */
    std::shared_ptr<IBitmap> GetSvgBitmap(const UiRect& rcDest, UiRect& rcSource);

public:
    /** Get a bitmap image data (single-frame image)
     * @return Returns the interface pointer of the bitmap; if it returns nullptr and bDecodeError is false, the image decoding has not been completed yet (in the case of multi-threaded decoding)
     *                           if it returns nullptr and bDecodeError is true, an error occurred in image decoding
     */
    std::shared_ptr<IBitmap> GetBitmap(bool* bDecodeError);

public:
    /** Query whether the image data of a certain frame is ready (in multi-threaded decoding, the frame data is decoded in a background thread)
    * @param [in] nFrameIndex The index number of the image frame, a subscript value numbered from 0; value range: [0, GetFrameCount())
    */
    bool IsFrameDataReady(uint32_t nFrameIndex);

    /** Get an image frame data
    * @param [in] nFrameIndex The index number of the image frame, a subscript value numbered from 0; value range: [0, GetFrameCount())
    * @param [in] szDestRectSize The size of the destination area, used for scaling vector images
    */
    AnimationFramePtr GetFrame(uint32_t nFrameIndex, const UiSize& szDestRectSize);

    /** Get the playback duration of an image frame, in milliseconds
    * @param [in] nFrameIndex The index number of the image frame, a subscript value numbered from 0; value range: [0, GetFrameCount())
    */
    int32_t GetFrameDelayMs(uint32_t nFrameIndex);

    /** Get the frame count of the image
    */
    uint32_t GetFrameCount() const;

    /** Whether it is a multi-frame image (such as GIF, etc.)
    */
    bool IsMultiFrameImage() const;

    /** Get the number of loop plays (TODO: not used, the playback implementation source needs to be modified)
    *@return Return values: -1 means the animation loops forever
    *              >= 0 means the specific number of times the animation loops
    */
    int32_t GetLoopCount() const;

public:
    /** Get the loading KEY of the image
    */
    DString GetLoadKey() const;

    /** Set the KEY of the actual image, used for the lifecycle management of the image
    * @param [in] imageKey The KEY of the actual image
    */
    void SetImageKey(const DString& imageKey);

    /** Get the KEY of the actual image
    */
    DString GetImageKey() const;

    /** Set the original image data interface
    * @param [in] loadParam The loading parameters
    * @param [in] pImageData The original image data interface
    * @param [in] bImageDpiScaleEnabled Whether the image supports DPI adaptation
    * @param [in] nImageFileDpiScale The DPI scaling ratio of the DPI-adaptive image
    */
    bool SetImageData(const ImageLoadParam& loadParam,
                      const std::shared_ptr<IImage>& pImageData,
                      bool bImageDpiScaleEnabled,
                      uint32_t nImageFileDpiScale);

    /** Get the image data interface of the associated original image
    */
    std::shared_ptr<IImage> GetImageData() const;

    /** Perform validation, correction, and DPI adaptation on the image source area, destination area, and corner sizes
    * @param [in] dpi The DPI scaling interface
    * @param [out] rcDestCorners The corner information of the drawing destination area, an output parameter; set internally according to rcImageCorners, then output
    * @param [in/out] rcSource The image area (the passed-in value has not been DPI-scaled)
    * @param [in/out] rcSourceCorners The corner information of the image area (the passed-in value has not been DPI-scaled)
    */
    void ScaleImageSourceRect(const DpiManager& dpi, UiRect& rcDestCorners, UiRect& rcSource, UiRect& rcSourceCorners);
    void ScaleImageSourceRect(const DpiManager& dpi, UiRect& rcSource);

private:
    /** Release the image resources (release with delay, so as to be shared)
    */
    void ReleaseImage();

    /** Get the interface of the multi-frame image
    */
    std::shared_ptr<IAnimationImage> GetAnimationImage(uint32_t nFrameIndex) const;

    /** Calculate the size of the ImageInfo (this size may be different from the size of the image itself)
    */
    void CalcImageInfoSize(const ImageLoadParam& loadParam,
                           const std::shared_ptr<IImage>& pImageData,
                           bool bImageDpiScaleEnabled,
                           uint32_t nImageFileDpiScale,
                           int32_t& nImageInfoWidth,
                           int32_t& nImageInfoHeight) const;

    /** Get the bitmap of the SVG image, supports vector scaling
    * @param [in] fImageSizeScale The scaling ratio of the image
    */
    std::shared_ptr<IBitmap> GetSvgBitmap(float fImageSizeScale);


private:
    /** The KEY of the actual image, used for the lifecycle management of the image (images with multiple DPIs may actually point to the same file)
    */
    UiString m_imageKey;

    /** The loading parameters of the image
    */
    ImageLoadParam m_loadParam;

    /** Whether there is a user-defined scaling ratio / the user-defined scaling ratio in the X/Y directions
    *   These variables are only used for multi-frame images
    */
    bool m_bHasCustomSizeScale;
    float m_fCustomSizeScaleX;
    float m_fCustomSizeScaleY;

    /** Get the DPI scaling ratio of the DPI-adaptive image (example: an image with a DPI scaling percentage of 120 (i.e., enlarged to 120%): "image.png" corresponds to "image@120.png")
    */
    uint32_t m_nImageFileDpiScale;

    /** Whether image DPI adaptation is supported
    */
    bool m_bEnableImageDpiScale;

private:
    /** The number of loop plays (greater than or equal to 0; if equal to 0, the animation loops; the APNG format supports setting the number of loop plays)
    */
    int32_t m_nLoopCount;

    /** The total frame count of the current image
    */
    uint32_t m_nFrameCount;

    /** The width of the image
    */
    int32_t m_nImageInfoWidth;

    /**The height of the image
    */
    int32_t m_nImageInfoHeight;

    /** The image interface of the original image (the decoded drawing image data is extracted from this original image; after extraction is finished, it is no longer used and can be released to reduce memory usage)
    *   After use, it is placed in ImageManager::m_delayReleaseImageList for delayed release, to implement the sharing of the original image
    */
    std::shared_ptr<IImage> m_pImageData;

private:
    /** The image type
    */
    ImageType m_imageType;

    /** The image data (single frame, cached; but SVG images have no cache)
    */
    std::shared_ptr<IBitmap> m_pBitmap;

    /** The width and height scaling ratio of the original image when loaded (1.0f means no scaling)
    */
    float m_fImageSizeScale;
};

} // namespace ui

#endif // UI_IMAGE_IMAGE_INFO_H_
