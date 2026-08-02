#ifndef UI_UTILS_BITMAP_HELPER_WINDOWS_H_
#define UI_UTILS_BITMAP_HELPER_WINDOWS_H_

#include "duilib/Render/IRender.h"

#ifdef DUILIB_BUILD_FOR_WIN
#include "duilib/duilib_config_windows.h"

namespace ui 
{
    class DUILIB_API BitmapHelper
    {
    public:
        /** Create a Render object and copy the content of the HDC to the Render
        * @return The returned Render object; the caller is responsible for releasing its resources
        */
        static IRender* CreateRenderObject(int srcRenderWidth, int srcRenderHeight, HDC hSrcDc, int srcDcWidth, int srcDcHeight);

        /** Create a Render object and copy the content of the Bitmap to the Render
        * @return The returned Render object; the caller is responsible for releasing its resources
        */
        static IRender* CreateRenderObject(IBitmap* pBitmap);

        /** Create a Bitmap object and copy the content of the HDC to the bitmap
        * @return The returned IBitmap object; the caller is responsible for releasing its resources
        */
        static IBitmap* CreateBitmapObject(int srcRenderWidth, int srcRenderHeight, HDC hSrcDc, int srcDcWidth, int srcDcHeight);

        /** Get an HBITMAP handle from a GDI Bitmap object
        * @return Returns the HBITMAP handle; the external caller needs to release the resource to avoid a resource leak
        */
        static HBITMAP CreateGDIBitmap(IBitmap* pBitmap);

        /** Create a device-independent bitmap
        *@return Returns the bitmap handle; the caller releases the bitmap resource
        */
        static HBITMAP CreateGDIBitmap(int32_t nWidth, int32_t nHeight, bool flipHeight, LPVOID* pBits);
    };
}

#endif //DUILIB_BUILD_FOR_WIN

#endif //UI_UTILS_BITMAP_HELPER_WINDOWS_H_

