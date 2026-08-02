#ifndef UI_CORE_ICONMANAGER_H_
#define UI_CORE_ICONMANAGER_H_

#include "duilib/Core/UiTypes.h"
#include <map>
#include <string>
#include <vector>
#include <mutex>
#include <functional>

namespace ui 
{
class Window;
class ImageLoadAttribute;

/** Icon bitmap data
*/
class DUILIB_API IconBitmapData
{
public:
    /** Icon bitmap data, the data length is: m_nBitmapHeight * m_nBitmapWidth * 4
    */
    std::vector<uint8_t> m_bitmapData;

    /** Bitmap data width
    */
    int32_t m_nBitmapWidth = 0;

    /** Bitmap data height
    */
    int32_t m_nBitmapHeight = 0;
};

/** Callback prototype: used to receive the delete icon event (the function must be thread-safe, because it may be called in a child thread)
*/
typedef std::function<void (uint32_t nIconId)> RemoveIconEvent;

/** Icon resource manager (thread-safe, suitable for small image resources such as icons)
 *  Note: supports the HICON handle resource of Windows, but HICON is not used internally, because HICON is a kernel GDI resource with a limit of about 10,000 per process; once exhausted, the process crashes.
 */
class DUILIB_API IconManager
{
public:
    IconManager();
    ~IconManager();
    IconManager(const IconManager&) = delete;
    IconManager& operator = (const IconManager&) = delete;

public:
    /** Get the resource string of an ICON (can be used as an image file path)
    * @return Returns the resource string, for example: "icon:1"
    */
    DString GetIconString(uint32_t id) const;

    /** Determine whether it is an ICON resource string
    *@param [in] str Resource string, the correct form is for example: "icon:1"
    */
    bool IsIconString(const DString& str) const;

    /** Parse the icon ID from the ICON resource string
    *@param [in] str Resource string, the correct form is for example: "icon:1"
    */
    uint32_t GetIconID(const DString& str) const;

    /** Get the size of the icon corresponding to the ICON resource string
    *@param [in] str Resource string, the correct form is for example: "icon:1"
    *@return Returns the size of the icon, or empty on failure
    */
    UiSize GetIconSize(const DString& str) const;

public:

#ifdef DUILIB_BUILD_FOR_WIN
    /** Add a HICON handle
    * @param [in] hIcon The ICON handle to add; after adding, the lifecycle of the handle resource is managed by this class
    * @return Returns the ID corresponding to the handle, or 0 on failure
    */
    uint32_t AddIcon(HICON hIcon);
#endif

    /** Add an icon (suitable for small icons)
    * @param [in] pBitmapData The starting address of the bitmap data
    * @param [in] nBitmapDataSize The size of the bitmap data
    * @param [in] nBitmapWidth The bitmap width
    * @param [in] nBitmapHeight The bitmap height
    * @return Returns the ID corresponding to the icon, or 0 on failure
    */
    uint32_t AddIcon(const uint8_t* pBitmapData, int32_t nBitmapDataSize, int32_t nBitmapWidth, int32_t nBitmapHeight);

    /** Get the icon bitmap data
    * @param [in] id Icon ID (the ID returned by AddIcon)
    * @param [out] bitmapData Returns the bitmap data on success
    */
    bool GetIconBitmapData(uint32_t id, IconBitmapData& bitmapData) const;

    /** Delete an icon resource by ID
    * @param [in] id The icon ID to delete (the ID returned by AddIcon)
    */
    void RemoveIcon(uint32_t id);

public:
    /** Add an icon resource (relative to the local resource path, for example: "public/shadow/test.png")
    * @param [in] imageString Image resource string, the format is the same as setting an image in XML
    * @return Returns the ID corresponding to the icon, or 0 on failure
    */
    uint32_t AddIcon(const DString& imageString);

    /** Determine whether it is a resource string in ImageString format
    *@param [in] id Icon ID (the ID returned by AddIcon)
    */
    bool IsImageString(uint32_t id) const;

    /** Get the image resource string of an ICON
    * @param [in] id Icon ID (the ID returned by AddIcon)
    * @return Returns the resource string, for example: "public/shadow/test.png"
    */
    DString GetImageString(uint32_t id) const;

    /** Set the callback function used to receive the delete icon event
    * @param [in] callback Callback function
    * @return Returns the ID corresponding to the callback, which can be used to remove the callback
    */
    uint32_t AttachRemoveIconEvent(RemoveIconEvent callback);

    /** Delete the callback function used to receive the delete icon event
    * @param [in] callbackID The ID of the callback function, returned by AttachRemoveIconEvent
    */
    void DetachRemoveIconEvent(uint32_t callbackID);

private:
    /** Add an icon
    */
    uint32_t AddIconBitmapData(IconBitmapData& bitmapData);

private:
    /** ICON bitmap resource mapping table
    */
    std::map<uint32_t, IconBitmapData> m_iconMap;

    /** ImageString resource mapping table
    */
    std::map<uint32_t, UiString> m_imageStringMap;

    /** Multi-thread synchronization lock for data
    */
    mutable std::mutex m_iconMutex;

    /** Next ID
    */
    uint32_t m_nNextID;

    /** ICON resource string prefix
    */
    const DString m_prefix;

    /** Callback function used to receive the delete icon event
    */
    std::map<uint32_t, RemoveIconEvent> m_callbackMap;

    /** Next callback function ID
    */
    uint32_t m_nNextCallbackID;
};

} //namespace ui 

#endif //UI_CORE_ICONMANAGER_H_
