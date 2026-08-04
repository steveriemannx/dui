#ifndef UI_UTILS_FILEUTIL_H_
#define UI_UTILS_FILEUTIL_H_

#include "dui/Utils/FilePath.h"

namespace ui
{

class DUI_API FileUtil
{
public:
    /** Read the file contents (the entire contents)
    * @param [in] filePath Local file path (absolute path)
    * @param [out] fileData File data, read as binary data
    */
    static bool ReadFileData(const FilePath& filePath, std::vector<uint8_t>& fileData);

    /** Read part of the data at the beginning of the file (partial content)
    * @param [in] filePath Local file path (absolute path)
    * @param [in] nReadSize The length of data to read
    * @param [out] fileHeaderData File data, read as binary data
    */
    static bool ReadFileHeaderData(const FilePath& filePath, uint32_t nReadSize, std::vector<uint8_t>& fileHeaderData);

    /** Write file contents
    * @param [in] filePath Local file path (absolute path)
    * @param [in] fileData File data
    */
    static bool WriteFileData(const FilePath& filePath, const std::vector<uint8_t>& fileData);

    /** Write file contents
    * @param [in] filePath Local file path (absolute path)
    * @param [in] fileData File data
    */
    static bool WriteFileData(const FilePath& filePath, const DStringW& fileData);

    /** Write file contents
    * @param [in] filePath Local file path (absolute path)
    * @param [in] fileData File data
    */
    static bool WriteFileData(const FilePath& filePath, const DStringA& fileData);
};

}

#endif // UI_UTILS_FILEUTIL_H_
