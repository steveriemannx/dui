#ifndef UI_IMAGE_STATE_IMAGE_H_
#define UI_IMAGE_STATE_IMAGE_H_

#include "dui/Core/UiTypes.h"
#include <map>

namespace ui
{
/** The mapping of control states to images
*/
class Control;
class Image;
class IRender;
class DpiManager;

class DUI_API StateImage
{
public:
    StateImage();
    ~StateImage();

    /** Set the associated control interface
    */
    void SetControl(Control* pControl);

    /** Set the image attributes
    * @param [in] stateType The image type
    * @param [in] strImageString The image attribute string
    * @param [in] dpi The DPI scaling management interface
    */
    void SetImageString(ControlStateType stateType,
                        const DString& strImageString,
                        const DpiManager& dpi);

    /** Get the image attributes
    *@param [in] stateType The image type
    */
    DString GetImageString(ControlStateType stateType) const;

    /** Get the image file name
    *@param [in] stateType The image type
    */
    DString GetImagePath(ControlStateType stateType) const;

    /** Compare whether the image source areas of two states are the same
    *@param [in] stateType1 Image type 1
    *@param [in] stateType2 Image type 2
    */
    bool AreImageSourceRectsEqual(ControlStateType stateType1, ControlStateType stateType2) const;

    /** Get the opacity of the image
    *@param [in] stateType The image type
    */
    int32_t GetImageFade(ControlStateType stateType) const;

    /** Get the image interface (readable and writable)
    */
    Image* GetStateImage(ControlStateType stateType) const;

public:
    /** Whether the image of the Hot state is included
    */
    bool HasHotImage() const;

    /** Whether state images are included
    */
    bool HasImage() const;

    /** Draw the image of the specified state
    * @param [in] pRender The drawing interface
    * @param [in] stateType The control state, used to select which image to draw
    * @param [in] sImageModify The additional attributes of the image
    * @param [out] pDestRect Returns the final destination rectangle area where the image is drawn
    * @return Returns true if the drawing succeeds, otherwise false
    */
    bool PaintStateImage(IRender* pRender, ControlStateType stateType,
                         const DString& sImageModify = _T(""),
                         UiRect* pDestRect = nullptr);

    /** Get the image interface used to estimate the Control control size (width and height)
    */
    Image* GetEstimateImage() const;

    /** Get all image interfaces
    */
    void GetAllImages(std::vector<Image*>& allImages) const;

    /** Clear the image cache and release resources
    */
    void ClearImageCache();

    /** Stop the animations of all images
    */
    void StopImageAnimation();

    /** Pause the animation playback
     */
    void PauseImageAnimation();

    /** Get the image interface with the specified name
    */
    Image* FindImageByName(const DString& imageName) const;

private:
    //The associated control interface
    Control* m_pControl;

    //The image interface of each state
    std::map<ControlStateType, Image*> m_stateImageMap;
};

} // namespace ui

#endif // UI_IMAGE_STATE_IMAGE_H_
