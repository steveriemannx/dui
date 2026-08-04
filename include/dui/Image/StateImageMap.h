#ifndef UI_IMAGE_STATE_IMAGE_MAP_H_
#define UI_IMAGE_STATE_IMAGE_MAP_H_

#include "dui/Image/StateImage.h"

namespace ui
{
/** The mapping of control image types to state images
*/
class DUI_API StateImageMap
{
public:
    StateImageMap();

    /** Set the associated control interface
    */
    void SetControl(Control* pControl);

    /** Set the image attributes
    * @param [in] stateImageType The image type, e.g., the foreground image or background image of the normal state; the foreground image or background image of the selected state, etc.
    * @param [in] stateType The image state, e.g., normal, focused, pushed, disabled states, etc.
    * @param [in] strImagePath The image attribute string
    * @param [in] dpi The DPI scaling management interface
    */
    void SetImageString(StateImageType stateImageType,
                        ControlStateType stateType,
                        const DString& strImagePath,
                        const DpiManager& dpi);

    /** Get the image attributes
    *@param [in] stateImageType The image type, e.g., the foreground image or background image of the normal state; the foreground image or background image of the selected state, etc.
    *@param [in] stateType The image state, e.g., normal, focused, pushed, disabled states, etc.
    */
    DString GetImageString(StateImageType stateImageType, ControlStateType stateType) const;

    /** Whether the image of the Hot state is included
    */
    bool HasHotImage() const;

    /** Whether there are state images
    */
    bool HasStateImages(void) const;

    /** Whether the image of the specified type is included
    */
    bool HasStateImage(StateImageType stateImageType) const;

    /** Draw the image of the specified type and state
    * @param [in] pRender The drawing interface
    * @param [in] stateImageType The image type
    * @param [in] stateType The control state, used to select which image to draw
    * @param [in] sImageModify The additional attributes of the image
    * @param [out] pDestRect Returns the final destination rectangle area where the image is drawn
    * @return Returns true if the drawing succeeds, otherwise false
    */
    bool PaintStateImage(IRender* pRender,
                         StateImageType stateImageType,
                         ControlStateType stateType,
                         const DString& sImageModify = _T(""),
                         UiRect* pDestRect = nullptr);

    /** Get the image interface used to estimate the Control control size (width and height)
    */
    Image* GetEstimateImage(StateImageType stateImageType) const;

    /** Get the image interface of the specified image type and state
    */
    Image* GetStateImage(StateImageType stateImageType, ControlStateType stateType) const;

    /** Get all image interfaces
    */
    void GetAllImages(std::vector<Image*>& allImages) const;

    /** Clear the caches of all image types and release resources
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

    //The state images of each image type (the foreground image or background image of the normal state; the foreground image or background image of the selected state)
    std::map<StateImageType, StateImage> m_stateImageMap;
};

} // namespace ui

#endif // UI_IMAGE_STATE_IMAGE_MAP_H_
