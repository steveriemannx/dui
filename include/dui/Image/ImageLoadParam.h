#ifndef UI_IMAGE_IMAGE_LOAD_PARAM_H_
#define UI_IMAGE_IMAGE_LOAD_PARAM_H_

#include "dui/Core/UiTypes.h"
#include "dui/Utils/FilePath.h"

namespace ui
{
class Control;

/** Path information for image loading
*/
enum class DUI_API ImageLoadPathType
{
    kUnknownPath,   //Unknown type of path
    kLocalPath,     //Local absolute path (not a resource path)
    kLocalResPath,  //Local absolute path (inside the resource path)
    kZipResPath,    //Relative path inside the archive
    kVirtualPath    //Virtual path, e.g.: "icon:1"
};

/** Path information for image loading
*/
struct DUI_API ImageLoadPath
{
    //Image path (local absolute path or relative path inside the archive)
    FilePath m_imageFullPath;

    //The image path type
    ImageLoadPathType m_pathType = ImageLoadPathType::kUnknownPath;
};

/** Image loading parameters, used to load an image
*/
class DUI_API ImageLoadParam
{
public:
    /** Default constructor
    */
    ImageLoadParam();

    /** Constructor
    * @param [in] srcWidth The specified image width, a pixel value or a percentage value, e.g., "300", or "30%", can also be empty
    * @param [in] srcHeight The specified image height, a pixel value or a percentage value, e.g., "300", or "30%", can also be empty
    * @param [in] bImageDpiScaleEnabled Whether the image supports screen DPI adaptation
    * @param [in] nLoadDpiScale The DPI scaling percentage of the drawing target (example: 100 means the scaling percentage is 100%, no scaling)
    * @param [in] bAsyncDecode Whether asynchronous thread decoding of the image data is supported
    * @param [in] bIconAsAnimation If it is an ICO file, specify whether to load as a multi-frame image (displayed as an animation image)
    * @param [in] nIconFrameDelayMs If it is an ICO file, when displayed as a multi-frame image, the playback time interval of each frame, milliseconds (only valid when m_bIconAsAnimation is true)
    * @param [in] nIconSize If it is an ICO file, used to specify the size of the ICO image to load
    * @param [in] fPagMaxFrameRate The maximum frame rate for PAG format playback by default (only for the PAG format)
    * @param [in] bAssertEnabled Whether assertions are allowed when image loading fails
    */
    ImageLoadParam(DString srcWidth,
                   DString srcHeight,
                   bool bImageDpiScaleEnabled,
                   uint32_t nLoadDpiScale /*= 100*/,
                   bool bAsyncDecode /*= false*/,
                   bool bIconAsAnimation /*= false*/,
                   int32_t nIconFrameDelayMs /*= 1000*/,
                   uint32_t nIconSize /*= 0*/,
                   float fPagMaxFrameRate /*= 30.0f*/,
                   bool bAssertEnabled /*= true*/);

    /** Copy construction and copy assignment
    */
    ImageLoadParam(const ImageLoadParam& r) = default;
    ImageLoadParam& operator= (const ImageLoadParam& r) = default;

public:
    /** Set the image path (local absolute path or relative path inside the archive)
    */
    void SetImageLoadPath(const ImageLoadPath& imageLoadPath);

    /** Get the image path (UTF8 or UTF16 encoded, local absolute path or relative path inside the archive)
    */
    const ImageLoadPath& GetImageLoadPath() const;

    /** Determine whether it is an SVG image file (SVG supports vector scaling and needs some special handling)
    */
    bool IsSvgImageFile() const;

public:
    /** Set whether DPI adaptation is supported when loading the image, i.e., scale the image size according to the DPI
    */
    void SetImageDpiScaleEnabled(bool bImageDpiScaleEnabled);

    /** Set whether DPI adaptation is supported when loading the image, i.e., scale the image size according to the DPI
    */
    bool IsImageDpiScaleEnabled() const;

    /** Set the DPI scaling percentage of the drawing target (example: 100 means the scaling percentage is 100%, no scaling)
    */
    void SetLoadDpiScale(uint32_t nLoadDpiScale);

    /** Get the DPI scaling percentage of the drawing target (example: 100 means the scaling percentage is 100%, no scaling)
    */
    uint32_t GetLoadDpiScale() const;

public:
    /** If it is an ICO file, specify whether to load as a multi-frame image (displayed as an animation image)
    */
    bool IsIconAsAnimation() const;

    /** If it is an ICO file, used to specify the size of the ICO image to load
    */
    uint32_t GetIconSize() const;

    /** If it is an ICO file, when displayed as a multi-frame image, the playback time interval of each frame, milliseconds
    *   Only valid when IsIconAsAnimation() is true
    */
    int32_t GetIconFrameDelayMs() const;

    /** The maximum frame rate for PAG format playback by default (only for the PAG format)
    */
    float GetPagMaxFrameRate() const;

    /** Whether asynchronous thread decoding of the image data is supported
    */
    bool IsAsyncDecodeEnabled() const;

    /** Whether assertions are allowed when image loading fails
    */
    bool IsAssertEnabled() const;

public:
    /** Get the cache KEY for loading the image
    *   The complete format is: <full image path>@<UI scaling percentage>#<whether DPI adaptation is supported>$<width>:<height>
    * @param [in] nLoadDpiScale The DPI scaling percentage requested for the image
    */
    DString GetLoadKey(uint32_t nLoadDpiScale) const;

    /** Whether the image loading includes a fixed-size setting option
    */
    bool HasImageFixedSize(void) const;

    /** Get the fixed size set for image loading
    * @param [out] nImageWidth The width set for the image; if 0 is returned, there is no data, e.g.: width='300'
    * @param [out] nImageHeight The height set for the image; if 0 is returned, there is no data, e.g.: height='300'
    */
    bool GetImageFixedSize(uint32_t& nImageWidth, uint32_t& nImageHeight) const;

    /** Whether the image loading includes a fixed percentage size setting option
    */
    bool HasImageFixedPercent() const;

    /** Get the fixed percentage size set for image loading
    * @param [out] fImageWidthPercent The width set for the image; if 1.0f is returned, there is no data, e.g.: width='300%'
    * @param [out] fImageHeightPercent The height set for the image; if 1.0f is returned, there is no data, e.g.: height='300%'
    */
    bool GetImageFixedPercent(float& fImageWidthPercent, float& fImageHeightPercent) const;

    /** Set the drawing destination area size of the image (DPI scaling has been done), used to optimize loading performance
    * @param [in] rcMaxDestRectSize The width and height of the area
    */
    void SetMaxDestRectSize(const UiSize& rcMaxDestRectSize);

    /** Get the drawing destination area size of the image (DPI scaling has been done), used to optimize loading performance
    */
    UiSize GetMaxDestRectSize() const;

private:
    /** Get the fixed size set for image loading
    */
    bool GetScaledFixedSize(const DString& srcSize, uint32_t& nScaledSize) const;

    /** Get the percentage size set for image loading
    */
    bool GetScaledFixedPercent(const DString& srcSize, float& fScaledPercent) const;

private:
    //(Attribute name: "file") Local absolute path or relative path inside the archive, excluding the attributes
    ImageLoadPath m_srcImageLoadPath;

    //Set the image width (attribute name: "width"), can enlarge or shrink the image: pixels or percentage %, e.g., 300, or 30%
    UiString m_srcWidth;

    //Set the image height (attribute name: "height"), can enlarge or shrink the image: pixels or percentage %, e.g., 200, or 30%
    UiString m_srcHeight;

    //The destination area size, used to optimize loading performance (for images loaded at the drawing stage, the image width and height are not needed to determine the destination area, so loading can be optimized; for large images, a small image can be loaded to improve the drawing speed and reduce memory usage while ensuring the drawing quality)
    UiSize m_rcMaxDestRectSize;

    //The DPI scaling percentage of the drawing target (example: 100 means the scaling percentage is 100%, no scaling)
    uint32_t m_nLoadDpiScale;

    //The maximum frame rate for PAG format playback by default (only for the PAG format)
    float m_fPagMaxFrameRate;

    //If it is an ICO file, used to specify the size of the ICO image to load
    uint32_t m_nIconSize;

    //If it is an ICO file, when displayed as a multi-frame image, the playback time interval of each frame, milliseconds
    //Only valid when m_bIconAsAnimation is true
    int32_t m_nIconFrameDelayMs;

    //If it is an ICO file, specify whether to load as a multi-frame image (displayed as an animation image)
    bool m_bIconAsAnimation;

    //Whether asynchronous thread decoding of the image data is supported
    bool m_bAsyncDecode;

    //Whether DPI adaptation is supported when loading the image, i.e., scale the image size according to the DPI ("dpi_scale")
    bool m_bImageDpiScaleEnabled;

    //Whether to assert when image loading fails (in debug builds)
    bool m_bAssertEnabled;
};

} // namespace ui

#endif // UI_IMAGE_IMAGE_LOAD_PARAM_H_
