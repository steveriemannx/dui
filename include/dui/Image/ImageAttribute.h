#ifndef UI_IMAGE_IMAGE_ATTRIBUTE_H_
#define UI_IMAGE_IMAGE_ATTRIBUTE_H_

#include "dui/Core/UiTypes.h"

namespace ui
{
class DpiManager;

/** Image attributes
*/
class DUI_API ImageAttribute
{
public:
    ImageAttribute();
    ~ImageAttribute();
    ImageAttribute(const ImageAttribute&);
    ImageAttribute& operator=(const ImageAttribute&);

    /** Initialize the data members
    */
    void Init();

    /** Initialize according to the image parameters (first call Init to initialize the member variables, then update some attributes according to the passed-in parameters)
    * @param [in] strImageString The image parameter string
    * @param [in] dpi The DPI scaling interface
    */
    void InitByImageString(const DString& strImageString, const DpiManager& dpi);

    /** Modify the attribute values according to the image parameters (only the newly set image attributes are updated; the attributes not included are not updated)
    * @param [in] strImageString The image parameter string
    * @param [in] dpi The DPI scaling interface
    */
    void ModifyAttribute(const DString& strImageString, const DpiManager& dpi);

public:
    /** Determine whether the rcDest area contains valid values
    * @param [in] rcDest The area to determine
    */
    static bool HasValidImageRect(const UiRect& rcDest);

    /** Calculate the adaptive drawing area that keeps the aspect ratio
     * @param nImageWidth The original image width
     * @param nImageHeight The original image height
     * @param targetRect The target rectangle area (left, top, right, bottom)
     * @param hAlign The horizontal alignment (left, center, right)
     * @param vAlign The vertical alignment (top, center, bottom)
     * @return The adaptive drawing area (left, top, right, bottom)
     */
    static UiRect CalculateAdaptiveRect(int32_t nImageWidth, int32_t nImageHeight,
                                        const UiRect& targetRect,
                                        const DString& hAlign,
                                        const DString& vAlign);

public:
    /** Get rcSource (without DPI scaling)
    */
    UiRect GetImageSourceRect() const;

    /** Get rcCorner (without DPI scaling)
    */
    UiRect GetImageCorner() const;

    /** Whether the rcCorner attribute exists
    */
    bool HasImageCorner() const;

    /** Get rcDest (whether to perform DPI scaling according to the configuration)
    * @param [in] imageWidth The width of the image
    * @param [in] imageHeight The height of the image
    * @param [in] dpi The DPI scaling interface
    */
    UiRect GetImageDestRect(int32_t imageWidth, int32_t imageHeight, const DpiManager& dpi) const;

    /** Whether the rcDest attribute exists
    */
    bool HasDestRect() const;

    /** Get the outer margin of the image attributes
    * @param [in] dpi The DPI scaling manager
    * @return Returns the margin data adapted according to the passed-in DPI scaling manager
    */
    UiMargin GetImageMargin(const DpiManager& dpi) const;

    /** Set the outer margin of the image attributes
    * @param [in] newMargin The margin to set
    * @param [in] bNeedDpiScale Whether newMargin needs DPI scaling
    * @param [in] dpi The DPI manager associated with the newPadding data
    */
    void SetImageMargin(const UiMargin& neMargin, bool bNeedDpiScale, const DpiManager& dpi);

    /** Determine whether the image is drawn tiled
    */
    bool IsTiledDraw() const;

    /** Get the tiled drawing attributes of the image
    */
    TiledDrawParam GetTiledDrawParam(const DpiManager& dpi) const;

    /** Whether the assertion error when image loading fails is enabled
    */
    bool IsAssertEnabled() const;

    /** Get the name of the image (can be used as the unique ID of the image)
    */
    DString GetImageName() const;

public:
    //Image file attribute string
    UiString m_sImageString;

    //Image file name, including the relative path, excluding the attributes
    UiString m_sImagePath;

    //Image name (a unique string within the control, used to identify the image resource)
    UiString m_sImageName;

    //Set the image width, can enlarge or shrink the image: pixels or percentage %, e.g., 300, or 30%
    UiString m_srcWidth;

    //Set the image height, can enlarge or shrink the image: pixels or percentage %, e.g., 200, or 30%
    UiString m_srcHeight;

    //The horizontal alignment in the drawing destination area (if the rcDest value is specified, this option is invalid)
    UiString m_hAlign;

    //The vertical alignment in the drawing destination area (if the rcDest value is specified, this option is invalid)
    UiString m_vAlign;

    //Tiled drawing related parameters
    std::unique_ptr<TiledDrawParam> m_pTiledDrawParam;

    //Opacity (0 - 255)
    uint8_t m_bFade;

    //Whether DPI adaptation is supported when loading the image, i.e., the image size is scaled according to the DPI ("dpi_scale")
    bool m_bImageDpiScaleEnabled;

    //Whether the rcDest attribute supports DPI adaptation, i.e., scaling according to the DPI ("dest_scale")
    bool m_bDestDpiScaleEnabled;

    //When drawing in nine-patch mode, the middle part is not drawn (e.g., for window shadows, only the border needs to be drawn, not the middle part, to avoid unnecessary drawing actions)
    bool m_bWindowShadowMode;

    //Whether to automatically adapt to the destination area (scale the image proportionally)
    bool m_bAdaptiveDestRect;

    //If it is an animation image, whether to auto-play
    bool m_bAutoPlay;

    //Whether the image supports asynchronous loading (i.e., load the image data in a worker thread to avoid the main interface stuttering)
    bool m_bAsyncLoad;

    //If it is an animation image, the number of plays can be specified
    //    - 1: Play forever
    //    0  : No valid play count; use the default value of the image (or the preset value)
    //    > 0: A specific number of plays; stop playing after reaching the play count
    int32_t m_nPlayCount;

    //If it is a PAG file, used to specify the frame rate of the animation, the default is 30.0f
    float m_fPagMaxFrameRate;

    //If it is an ICO file, used to specify the size of the ICO image to load
    uint32_t m_nIconSize;

    //If it is an ICO file, when displayed as a multi-frame image, the playback time interval of each frame, milliseconds
    //Only valid when m_bIconAsAnimation is true
    int32_t m_nIconFrameDelayMs;

    //If it is an ICO file, specify whether to load as a multi-frame image (displayed as an animation image)
    bool m_bIconAsAnimation;

    //Drawable flag: true means drawing is allowed, false means drawing is disabled
    bool m_bPaintEnabled;

private:
    //The code assertion setting when image loading fails (enabled in debug builds, used to diagnose errors during image loading, especially failures caused by incorrect image data)
    bool m_bAssertEnabled;

    //The DPI scaling percentage corresponding to rcMargin
    uint16_t m_rcMarginScale;

    //The position and size of the drawing destination area (relative to the control area, without DPI scaling)
    UiRect* m_rcDest;

    //The outer margin in the drawing destination area (if the rcDest value is specified, this option is invalid)
    UiMargin16* m_rcMargin;

    //The position and size of the image source area (without DPI scaling)
    UiRect* m_rcSource;

    //The rounded corner attributes (without DPI scaling)
    UiRect* m_rcCorner;
};

} // namespace ui

#endif // UI_IMAGE_IMAGE_ATTRIBUTE_H_
