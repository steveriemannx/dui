#ifndef UI_CORE_ZIP_STREAM_IO_H_
#define UI_CORE_ZIP_STREAM_IO_H_

#include "duilib/duilib_defs.h"
#include "third_party/zlib/contrib/minizip/ioapi.h"

namespace ui 
{
/** Image list
 */
class DUILIB_API ZipStreamIO
{
public:
    /** Construct from in-memory data
    * @param [in] pData Starting address of the in-memory data
    * @param [in] nDataLen Length of the in-memory data
    */
    ZipStreamIO(uint8_t* pData, uint32_t nDataLen);
    ~ZipStreamIO();
    ZipStreamIO(const ZipStreamIO&) = delete;
    ZipStreamIO& operator = (const ZipStreamIO&) = delete;

public:

    /** Fill the unzip IO callback function interface
    */
    void FillFopenFileFunc(zlib_filefunc_def* pzlib_filefunc_def);

private:

    // Data read operations
    static voidpf ZCALLBACK fopen_file_func(voidpf opaque, const char* filename, int mode);
    static uLong ZCALLBACK fread_file_func(voidpf opaque, voidpf stream, void* buf, uLong size);
    static uLong ZCALLBACK fwrite_file_func(voidpf opaque, voidpf stream, const void* buf, uLong size);
    static long ZCALLBACK ftell_file_func(voidpf opaque, voidpf stream);
    static long ZCALLBACK fseek_file_func(voidpf opaque, voidpf stream, uLong offset, int origin);
    static int ZCALLBACK fclose_file_func(voidpf opaque, voidpf stream);
    static int ZCALLBACK ferror_file_func(voidpf opaque, voidpf stream);

private:
    /** Starting address of the in-memory data
    */
    uint8_t* m_pData;

    /** Length of the in-memory data
    */
    int32_t m_nDataLen;

    /** Current data position
    */
    int32_t m_nCurPos;
};

} //namespace ui 

#endif //UI_CORE_ZIP_STREAM_IO_H_
