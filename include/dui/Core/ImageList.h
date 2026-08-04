#ifndef UI_CORE_IMAGE_LIST_H_
#define UI_CORE_IMAGE_LIST_H_

#include "dui/Image/Image.h"
#include "dui/Core/UiTypes.h"
#include <map>
#include <string>
#include <vector>

namespace ui 
{
/** Smart pointer of the image resource
*/
typedef std::shared_ptr<Image> ImagePtr;

/** Image list
 */
class DUI_API ImageList
{
public:
    ImageList();
    ~ImageList();
    ImageList(const ImageList&) = delete;
    ImageList& operator = (const ImageList&) = delete;

public:
    /** Set the image size (optional; if not set, it is obtained from each image)
    * @param [in] imageSize Image size; no DPI scaling is performed internally, it needs to be done externally
    * @param [in] dpi DPI scaling manager
    * @param [in] bNeedDpiScale Whether DPI scaling is supported
    */
    void SetImageSize(UiSize imageSize, const DpiManager& dpi, bool bNeedDpiScale);

    /** Get the image size
    * @return Returns the set image size, or (0,0) if not set
    */
    UiSize GetImageSize() const;

    /** Add an image resource
    * @param [in] imageString Image resource string, same format as the Image::SetImageString function
    * @param [in] dpi DPI scaling management interface
    * @return Returns the ID of the image resource, -1 on failure, otherwise an ID greater than or equal to 0
    */
    int32_t AddImageString(const DString& imageString, const DpiManager& dpi);

    /** Add an image resource, and attach the image size information
    * @param [in] imageString Image resource string, same format as the Image::SetImageString function
    * @param [in] dpi DPI scaling management interface
    * @return Returns the ID of the image resource, -1 on failure, otherwise an ID greater than or equal to 0
    */
    int32_t AddImageStringWithSize(const DString& imageString, const DpiManager& dpi);

    /** Get the image resource string
    * @param [in] imageId The ID of the image resource, the value returned by the AddImage function
    * @return Returns an empty string if there is no associated resource
    */
    DString GetImageString(int32_t imageId) const;

    /** Get the image resource interface (shared resource)
    * @param [in] imageId The ID of the image resource, the value returned by the AddImage function
    * @return Returns nullptr if there is no associated resource
    */
    ImagePtr GetImageData(int32_t imageId) const;

    /** Get the ID of the image resource
    * @param [in] imageString Image resource string, same format as the Image::SetImageString function
    * @return Returns the ID of the associated image resource, or -1 if there is no associated resource
    */
    int32_t GetImageStringId(const DString& imageString) const;

    /** Remove an image resource
    * @param [in] imageId The ID of the image resource, the value returned by the AddImage function
    */
    bool RemoveImageString(int32_t imageId);

    /** Get the number of image resources
    */
    size_t GetImageCount() const;

    /** Get the IDs of all image resources
    */
    void GetImageIdList(std::vector<int32_t>& imageIdList) const;

    /** Clear all image resources
    */
    void Clear();

    /** DPI changed, update the image size
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] dpi DPI scaling manager
    */
    void ChangeDpiScale(const DpiManager& dpi, uint32_t nOldDpiScale);

private:
    /** Next ID
    */
    int32_t m_nNextID;

    /** Image size (optional; if not set, it is obtained from each image)
    */
    UiSize m_imageSize;

    /** Original image size (optional; if not set, it is obtained from each image)
    */
    UiSize m_imageSizeNoDpi;

    /** Image resource mapping table
    */
    std::map<int32_t, ImagePtr> m_imageMap;
};

typedef std::shared_ptr<ImageList> ImageListPtr;

} //namespace ui 

#endif //UI_CORE_IMAGE_LIST_H_
