#ifndef UI_IMAGE_IMAGE_H_
#define UI_IMAGE_IMAGE_H_

#include "duilib/Image/ImageInfo.h"
#include "duilib/Image/ImageAttribute.h"
#include "duilib/Image/ImageLoadParam.h"
#include "duilib/Image/StateImageMap.h"

namespace ui
{
class Control;
class ImagePlayer;
class DpiManager;

/** Image-related wrapper; supported file formats: SVG/PNG/GIF/JPG/BMP/APNG/WEBP/ICO/Lottie-JSON/PAG
*/
class DUILIB_API Image : public virtual SupportWeakCallback
{
public:
    Image();
    ~Image();
    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

public:
    /** @name Image attributes
    * @{
    */
    /** Initialize the image attributes
    */
    void InitImageAttribute();

    /** Set and initialize the image attributes
    * @param [in] strImageString The image attribute string
    * @param [in] dpi The DPI scaling interface
    */
    void SetImageString(const DString& strImageString, const DpiManager& dpi);

    /** Update the image attributes
    * @param [in] strImageString The image attribute string
    * @param [in] dpi The DPI scaling interface
    */
    void UpdateImageAttribute(const DString& strImageString, const DpiManager& dpi);

    /** Get the image attributes (including the file name, image setting attributes, etc.)
    */
    DString GetImageString() const;

    /** Determine whether the image attributes are equal
    */
    bool EqualToImageString(const DString& imageString) const;

    /** Get the image file name (including the relative path, excluding the image attributes)
    */
    DString GetImagePath() const;

    /** Get the outer margin of the image attributes
    * @param [in] dpi The DPI scaling manager
    * @return Returns the margin data adapted according to the passed-in DPI scaling manager
    */
    UiMargin GetImageMargin(const DpiManager& dpi) const;

    /** Set the outer margin of the image attributes (no DPI adaptation is performed internally)
    * @param [in] newMargin The margin to set
    * @param [in] bNeedDpiScale Whether newMargin needs DPI scaling
    * @param [in] dpi The DPI manager associated with the newMargin data
    */
    void SetImageMargin(const UiMargin& newMargin, bool bNeedDpiScale, const DpiManager& dpi);

    /** Determine whether image painting is disabled
    */
    bool IsImagePaintEnabled() const;

    /** Set whether background image painting is disabled
    */
    void SetImagePaintEnabled(bool bEnable);

    /** Set the image attribute: the number of plays (only for multi-frame images)
    */
    void SetImagePlayCount(int32_t nPlayCount);

    /** Set the image attribute: opacity (only for multi-frame images)
    */
    void SetImageFade(uint8_t nFade);

    /** Get the image attributes (read-only)
    */
    const ImageAttribute& GetImageAttribute() const;

    /** Get the image loading attributes
    */
    ImageLoadParam GetImageLoadParam() const;

    /** @} */

public:
    /** @name Image data (the image data is loaded externally)
    * @{
    */
    /** Get the image information interface
    */
    const std::shared_ptr<ImageInfo>& GetImageInfo() const;

    /** Set the image information interface
    */
    void SetImageInfo(const std::shared_ptr<ImageInfo>& imageInfo);

    /** Clear the image information cache data and release resources
    */
    void ClearImageCache();

public:
    /** Whether it is a multi-frame image (such as GIF, etc.)
    */
    bool IsMultiFrameImage() const;

    /** Set the current image frame (only for multi-frame images)
    */
    void SetCurrentFrameIndex(uint32_t nCurrentFrame);

    /** Get the current image frame index (only for multi-frame images)
    */
    uint32_t GetCurrentFrameIndex() const;

    /** Get the frame count of the image
    */
    uint32_t GetFrameCount() const;

    /** Get the number of loop plays (TODO: not used, the playback implementation source needs to be modified)
    *@return Return values: -1 means the animation loops forever
    *              >= 0 means the specific number of times the animation loops
    */
    int32_t GetLoopCount() const;

    /** Get the image data of the current image frame (multi-frame image)
    * @param [in] rcDest The drawing destination area, used for scaling vector images
    * @param [in,out] rcSource The image source area; if scaled, the corresponding size of this area will be modified synchronously
    * @param [in,out] rcSourceCorners The nine-patch rounded corner attributes of the image
    */
    AnimationFramePtr GetCurrentFrame(const UiRect& rcDest,
                                      UiRect& rcSource,
                                      UiRect& rcSourceCorners) const;

public:
    /** Get the image data (single-frame image; for formats such as SVG, vector-scaled images are supported)
    * @param [in] bImageStretch Whether the image will be stretched when drawing
    * @param [in] rcDest The drawing destination area, used for scaling vector images
    * @param [in,out] rcSource The image source area; if scaled, the corresponding size of this area will be modified synchronously
    * @param [in,out] rcSourceCorners The nine-patch rounded corner attributes of the image
    * @param [out] bDecodeError Returns true if an image decoding error is encountered
    */
    std::shared_ptr<IBitmap> GetCurrentBitmap(bool bImageStretch,
                                              const UiRect& rcDest,
                                              UiRect& rcSource,
                                              UiRect& rcSourceCorners,
                                              bool* bDecodeError) const;

    /** @} */

public:
    /** @name Display of the image
    * @{
    */
    /** Set the associated control interface
    */
    void SetControl(Control* pControl);

    /** Set the display area of the image (call before drawing)
    */
    void SetDrawDestRect(const UiRect& rcImageRect);

    /** Get the display area of the image
    */
    const UiRect& GetDrawDestRect() const;

    /** Redraw the image (according to its display area)
    */
    void RedrawImage();

    /** Play the animation
    */
    void CheckStartImageAnimation();

    /** Pause the animation playback
     */
    void PauseImageAnimation();

    /** Play the animation
     * @param [in] nStartFrame Which frame to start playing from; the first frame, the current frame, and the last frame can be set. Please refer to the AnimationImagePos enum
     * @param [in] nPlayCount Specify the number of plays
                   -1: Play forever
                    0: No valid play count; use the default value of the image (or the preset value)
                   >0: A specific number of plays; stop playing after reaching the play count
     */
    bool StartImageAnimation(AnimationImagePos nStartFrame = AnimationImagePos::kFrameFirst,
                             int32_t nPlayCount = 0);

    /** Stop the animation playback
     * @param [in] nStopFrame Which frame to stop at after playback ends; the first frame, the current frame, and the last frame can be set. Please refer to the AnimationImagePos enum
     * @param [in] bTriggerEvent Whether to notify subscribers of the stop event; refer to the AttachImageAnimationStop method
     */
    void StopImageAnimation(AnimationImagePos nStopFrame = AnimationImagePos::kFrameCurrent,
                            bool bTriggerEvent = false);
    /** @} */

public:
    /** Get the name of the image (can be used as the unique ID of the image)
    */
    DString GetImageName() const;

    /** Set whether an error occurred while loading the image (after an error, the image will no longer be loaded)
    */
    void SetImageError(bool bImageError);

    /** Get whether an error occurred while loading the image (after an error, the image will no longer be loaded)
    */
    bool HasImageError() const;

    /** Set whether the image decode completion event has been notified
    */
    void SetDecodeEventFired(bool bFired);

    /** Get whether the image decode completion event has been notified
    */
    bool IsDecodeEventFired() const;

private:
    /** Initialize the animation player
    */
    ImagePlayer* InitImagePlayer();

    /** Get the image data of the current image frame (single-frame image)
    * @param [in,out] rcSource The image source area; if scaled, the corresponding size of this area will be modified synchronously
    * @param [in,out] rcSourceCorners The nine-patch rounded corner attributes of the image
    * @param [out] bDecodeError Returns true if an image decoding error is encountered
    */
    std::shared_ptr<IBitmap> GetBitmapData(UiRect& rcSource, UiRect& rcSourceCorners, bool* bDecodeError) const;

    /** Adjust the drawing source area according to the bitmap size
    * @param [in] pBitmap The bitmap interface
    * @param [in,out] rcSource The image source area; if scaled, the corresponding size of this area will be modified synchronously
    * @param [in,out] rcSourceCorners The nine-patch rounded corner attributes of the image
    */
    void AdjustImageSourceRect(const std::shared_ptr<IBitmap>& pBitmap, UiRect& rcSource, UiRect& rcSourceCorners) const;

private:
    /** The associated control interface
    */
    Control* m_pControl;

    /** The playback implementation interface of the multi-frame image
    */
    std::unique_ptr<ImagePlayer> m_pImagePlayer;

    /** The image information
    */
    std::shared_ptr<ImageInfo> m_imageInfo;

    /** The destination area where the image is drawn
    */
    UiRect m_rcDrawDestRect;

    /** The image attributes
    */
    ImageAttribute m_imageAttribute;

    /** The image frame currently being played (only for multi-frame images)
    */
    uint32_t m_nCurrentFrame;

    /** Whether an error occurred while loading the image (after an error, the image will no longer be loaded)
    */
    bool m_bImageError;

    /** Whether the image decode completion event has been notified (to avoid this event not being triggered when the original image is reused)
    */
    bool m_bDecodeEventFired;
};

} // namespace ui

#endif // UI_IMAGE_IMAGE_H_
