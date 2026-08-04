#include "dui/Core/ZipManager.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/ZipStreamIO.h"
#include "dui/Utils/StringUtil.h"
#include "dui/Utils/StringConvert.h"
#include "dui/Utils/FilePathUtil.h"

#include "third_party/zlib/zlib.h"
#include "third_party/zlib/contrib/minizip/unzip.h"

namespace ui 
{
/** The maximum length of a path inside the zip archive
*/
#define MAX_PATH_LEN (size_t)(1024)

ZipManager::ZipManager():
    m_hzip(nullptr),
    m_bUseMemory(false),
    m_pMemoryData(nullptr),
    m_memoryDataSize(0)
{
}

ZipManager::~ZipManager()
{
    CloseResZip();
}


bool ZipManager::IsUseZip() const
{
    // Returns true when either a zip archive or an embedded memory
    // resource archive is in use (both are "archive-style" resource sources)
    return (m_hzip != nullptr) || m_bUseMemory;
}

bool ZipManager::OpenMemoryArchive(const uint8_t* pData, size_t nSize)
{
    CloseResZip();
    if ((pData == nullptr) || (nSize < 16)) {
        return false;
    }
    // Binary format (little-endian):
    //   [magic "DUIR" u32][version u32][count u32]
    //   count x [u32 pathLen][path UTF-8][u64 dataOffset][u64 dataLen]
    //   [data blocks]
    // Must match the embed_resources tool (cmake/embed_resources.cpp).
    size_t nPos = 0;
    auto readU32 = [pData, nSize, &nPos](uint32_t& value) -> bool {
        if (nPos + 4 > nSize) {
            return false;
        }
        memcpy(&value, pData + nPos, 4);
        nPos += 4;
        return true;
    };
    auto readU64 = [pData, nSize, &nPos](uint64_t& value) -> bool {
        if (nPos + 8 > nSize) {
            return false;
        }
        memcpy(&value, pData + nPos, 8);
        nPos += 8;
        return true;
    };

    uint32_t magic = 0;
    uint32_t version = 0;
    uint32_t count = 0;
    if (!readU32(magic) || !readU32(version) || !readU32(count)) {
        return false;
    }
    if ((magic != 0x52495544) || (version != 1)) { // "DUIR"
        return false;
    }

    std::map<DStringW, MemoryResData> index;
    for (uint32_t i = 0; i < count; ++i) {
        uint32_t pathLen = 0;
        uint64_t dataOffset = 0;
        uint64_t dataLen = 0;
        if (!readU32(pathLen) || (nPos + pathLen > nSize)) {
            return false;
        }
        std::string pathA(reinterpret_cast<const char*>(pData + nPos), pathLen);
        nPos += pathLen;
        if (!readU64(dataOffset) || !readU64(dataLen) || (dataOffset + dataLen > nSize)) {
            return false;
        }
        DStringW key = StringConvert::UTF8ToWString(pathA);
        key = StringUtil::MakeLowerString(key);
        NormalizeZipFilePath(key);
        index[key] = { static_cast<size_t>(dataOffset), static_cast<size_t>(dataLen) };
    }

    m_pMemoryData = pData;
    m_memoryDataSize = nSize;
    m_memoryIndex.swap(index);
    m_bUseMemory = true;
    return true;
}

#ifdef DUI_BUILD_FOR_WIN

bool ZipManager::OpenResZip(HMODULE hModule, LPCTSTR resourceName, LPCTSTR resourceType, const DString& password)
{
    HRSRC rsc = ::FindResource(hModule, resourceName, resourceType);
    ASSERT(rsc != nullptr);
    if (rsc == nullptr) {
        return false;
    }
    uint8_t* pData = (uint8_t*)::LoadResource(hModule, rsc);
    uint32_t nDataSize = ::SizeofResource(hModule, rsc);
    ASSERT((pData != nullptr) && (nDataSize > 0));
    if ((pData == nullptr) || (nDataSize == 0)) {
        return false;
    }
    CloseResZip();
    m_password = password;
    m_pZipStreamIO = std::make_unique<ZipStreamIO>(pData, nDataSize);
    zlib_filefunc_def pzlib_filefunc_def;
    m_pZipStreamIO->FillFopenFileFunc(&pzlib_filefunc_def);
    m_hzip = ::unzOpen2(nullptr, &pzlib_filefunc_def);
    return m_hzip != nullptr;
}
#endif

bool ZipManager::OpenZipFile(const FilePath& path, const DString& password)
{
    CloseResZip();
    DStringA nativePath = path.NativePathA();
    if (nativePath.empty()) {
        return false;
    }
    m_password = password;
    m_hzip = ::unzOpen(nativePath.c_str());
    return m_hzip != nullptr;
}

bool ZipManager::GetZipData(const FilePath& path, std::vector<unsigned char>& fileData) const
{
    fileData.clear();
    GlobalManager::Instance().AssertUIThread();
    if (m_bUseMemory) {
        // Embedded memory resource archive (Qt qrc style): direct memory access
        const DStringW key = NormalizeMemoryPath(path);
        auto it = m_memoryIndex.find(key);
        if (it == m_memoryIndex.end()) {
            return false;
        }
        const MemoryResData& data = it->second;
        fileData.assign(m_pMemoryData + data.nOffset, m_pMemoryData + data.nOffset + data.nSize);
        return true;
    }
    ASSERT(m_hzip != nullptr);
    if (m_hzip == nullptr) {
        return false;
    }
    const FilePath normalizePath = FilePathUtil::NormalizeFilePath(path);
    std::string filePathA;
    if (!LocateFile(normalizePath, filePathA)) {
        return false;
    }

    size_t fileNameLen = std::max(filePathA.size() + 1, MAX_PATH_LEN);
    std::vector<char> szFileName;
    szFileName.resize(fileNameLen, 0);
    unz_file_info file_info = {0, };
    int nRet = ::unzGetCurrentFileInfo(m_hzip, &file_info, &szFileName[0], (uLong)szFileName.size() - 1, nullptr, 0, nullptr, 0);
    if (nRet != UNZ_OK) {
        return false;
    }
    
    if (file_info.uncompressed_size == 0) {
        return false;
    }

    // Whether the file is encrypted
    bool bFileEncrypted = file_info.flag & 1;

    nRet = ::unzStringFileNameCompare(szFileName.data(), filePathA.c_str(), 0);
    ASSERT(nRet == 0);
    if (nRet != 0) {
        return false;
    }
    if (!m_password.empty() && bFileEncrypted) {
        // The password is in local encoding (ANSI)
        std::string password;
#ifdef DUI_BUILD_FOR_WIN
    #ifdef DUI_UNICODE
        password = StringConvert::UnicodeToMBCS(m_password);
    #else
        password = m_password;
    #endif
#else
        password = StringConvert::TToUTF8(m_password);
#endif
        nRet = ::unzOpenCurrentFilePassword(m_hzip, password.c_str());
    }
    else {
        nRet = ::unzOpenCurrentFile(m_hzip);
    }
    if (nRet != UNZ_OK) {
        return false;
    }

    fileData.resize(file_info.uncompressed_size);
    nRet = ::unzReadCurrentFile(m_hzip, &fileData[0], (uLong)fileData.size());
    ::unzCloseCurrentFile(m_hzip);
    ASSERT(nRet == (int)fileData.size());
    if (nRet != (int)fileData.size()) {
        fileData.clear();
        return false;
    }
    return true;
}

bool ZipManager::IsZipResExist(const FilePath& path) const
{
    GlobalManager::Instance().AssertUIThread();
    if (path.IsEmpty()) {
        return false;
    }
    if (m_bUseMemory) {
        // Embedded memory resource archive: direct index lookup
        const DStringW key = NormalizeMemoryPath(path);
        return m_memoryIndex.find(key) != m_memoryIndex.end();
    }
    if (m_hzip == nullptr) {
        return false;
    }
    if (m_zipPathCache.empty()) {
        // When querying for the first time, build a cache to avoid having to traverse all files in the archive every time (the ::unzLocateFile function works by traversing all files, which has poor performance)
        int nRet = ::unzGoToFirstFile(m_hzip);
        while (nRet == UNZ_OK) {
            size_t fileNameLen = MAX_PATH_LEN;
            std::vector<char> szFileName;
            szFileName.resize(fileNameLen, 0);
            unz_file_info file_info = { 0, };
            nRet = ::unzGetCurrentFileInfo(m_hzip, &file_info, &szFileName[0], (uLong)szFileName.size() - 1, nullptr, 0, nullptr, 0);
            if (nRet == UNZ_OK) {
                // Whether the encoding of the file name is UTF8 format
                bool bUtf8 = file_info.flag & (1 << 11);
                DString fileName = GetZipFilePath(szFileName.data(), bUtf8);

#ifdef DUI_BUILD_FOR_WIN
                DStringW innerFilePath = StringConvert::MBCSToUnicode(szFileName.data(), bUtf8 ? CP_UTF8 : CP_ACP);
#else
                DStringW innerFilePath = StringConvert::UTF8ToWString(szFileName.data());
#endif
                // File names inside the archive are case-insensitive; convert to lowercase before comparing
                innerFilePath = StringUtil::MakeLowerString(innerFilePath);
                // Add to the cache
                m_zipPathCache.insert(innerFilePath);

                // The next file
                nRet = ::unzGoToNextFile(m_hzip);
            }
        }
    }

    const FilePath normalizePath = FilePathUtil::NormalizeFilePath(path);
    DStringW innerFilePath = normalizePath.ToStringW();
    innerFilePath = StringUtil::MakeLowerString(innerFilePath);
    NormalizeZipFilePath(innerFilePath);
    auto it = m_zipPathCache.find(innerFilePath);
    if (it != m_zipPathCache.end()) {
        return true;
    }
    return false;
}

bool ZipManager::LocateFile(const FilePath& normalizePath, std::string& filePathA) const
{
    // The encoding of file paths inside the archive differs per file; when locating a file, the encoding of the file name cannot be determined, so try them one by one
    filePathA = normalizePath.NativePathA();// Try MBCS encoding first
    ASSERT(!filePathA.empty());
    if (filePathA.empty()) {
        return false;
    }
    NormalizeZipFilePath(filePathA);
    const int iCaseSensitivity = 2; //Case-insensitive
    int nRet = ::unzLocateFile(m_hzip, filePathA.c_str(), iCaseSensitivity);
    if (nRet == UNZ_OK) {
        return true;
    }

    std::string oldFilePathA = filePathA;
    filePathA = normalizePath.ToStringA();// Then try UTF8 encoding
    ASSERT(!filePathA.empty());
    if (filePathA.empty()) {
        return false;
    }
    NormalizeZipFilePath(filePathA);
    if (oldFilePathA == filePathA) {
        // The path is unchanged; do not query again
        return false;
    }
    nRet = ::unzLocateFile(m_hzip, filePathA.c_str(), iCaseSensitivity);
    if (nRet == UNZ_OK) {
        return true;
    }
    filePathA.clear();
    return false;
}

void ZipManager::CloseResZip()
{
    if (m_hzip != nullptr) {
        ::unzClose(m_hzip);
        m_hzip = nullptr;
    }
    m_zipPathCache.clear();
    m_pZipStreamIO.reset();
    m_bUseMemory = false;
    m_pMemoryData = nullptr;
    m_memoryDataSize = 0;
    m_memoryIndex.clear();
}

DStringW ZipManager::NormalizeMemoryPath(const FilePath& path) const
{
    const FilePath normalizePath = FilePathUtil::NormalizeFilePath(path);
    DStringW innerFilePath = normalizePath.ToStringW();
    innerFilePath = StringUtil::MakeLowerString(innerFilePath);
    NormalizeZipFilePath(innerFilePath);
    return innerFilePath;
}

bool ZipManager::GetZipFileList(const FilePath& dirPath, std::vector<DString>& fileList) const
{
    fileList.clear();
    GlobalManager::Instance().AssertUIThread();
    DString filePath = dirPath.NativePath();
    if (!filePath.empty() &&
        (filePath[filePath.size() - 1] != _T('\\')) &&
        (filePath[filePath.size() - 1] != _T('/'))) {
        filePath += _T("/");
    }
    DString innerPath = FilePathUtil::NormalizeFilePath(filePath);
    if (innerPath.empty()) {
        return false;
    }
    // Uniformly replace path separators with '/'
    NormalizeZipFilePath(innerPath);

    if (m_bUseMemory) {
        // Embedded memory resource archive: list direct children of the directory
        DStringW prefix = StringUtil::MakeLowerString(FilePath(innerPath).ToStringW());
        NormalizeZipFilePath(prefix);
        for (const auto& kv : m_memoryIndex) {
            const DStringW& key = kv.first;
            if ((key.size() > prefix.size()) && (key.compare(0, prefix.size(), prefix) == 0)) {
                DStringW remainder = key.substr(prefix.size());
                if (remainder.find(L'/') == DStringW::npos) {
                    fileList.push_back(StringConvert::WStringToT(remainder));
                }
            }
        }
        return true;
    }
    if (m_hzip == nullptr) {
        return false;
    }
    int nRet = ::unzGoToFirstFile(m_hzip);
    if (nRet != UNZ_OK) {
        return false;
    }
    DString fileName;
    while (nRet == UNZ_OK) {
        size_t fileNameLen = MAX_PATH_LEN;
        std::vector<char> szFileName;
        szFileName.resize(fileNameLen, 0);
        unz_file_info file_info = { 0, };
        nRet = ::unzGetCurrentFileInfo(m_hzip, &file_info, &szFileName[0], (uLong)szFileName.size() - 1, nullptr, 0, nullptr, 0);
        if (nRet != UNZ_OK) {
            return false;
        }

        // Whether the encoding of the file name is UTF8 format
        bool bUtf8 = file_info.flag & (1 << 11);
        fileName = GetZipFilePath(szFileName.data(), bUtf8);

        // zip has an 'attribute' 32bit value. Its lower half is windows stuff
        // its upper half is standard unix stat.st_mode. We'll start trying
        // to read it in unix mode

        // Whether the file name is a directory
        bool bDir = (file_info.external_fa & 0x40000000) != 0;
        // but in normal hostmodes these are overridden by the lower half...
        int host = file_info.version >> 8;
        if (host == 0 || host == 7 || host == 11 || host == 14) {
            //0 - FAT filesystem (MS-DOS, OS/2, NT/Win32)
            //7 - Macintosh
            //11 - NTFS filesystem (NT)
            //14 - VFAT
            bDir = (file_info.external_fa & 0x00000010) != 0;
        }
        if (!bDir) {
            size_t nPos = fileName.find(innerPath);
            if ((nPos == 0) && (fileName.size() > innerPath.size())) {
                fileName = fileName.substr(innerPath.size());
                if (fileName.find(_T('/')) == DString::npos) {
                    fileList.push_back(fileName);
                }
            }
        }

        // Jump to the next file
        nRet = ::unzGoToNextFile(m_hzip);
    }
    return true;
}

void ZipManager::NormalizeZipFilePath(std::string& innerFilePath) const
{
    const size_t nCount = innerFilePath.size();
    for (size_t nIndex = 0; nIndex < nCount; ++nIndex) {
        if (innerFilePath[nIndex] == '\\') {
            innerFilePath[nIndex] = '/';
        }
    }
}

void ZipManager::NormalizeZipFilePath(std::wstring& innerFilePath) const
{
    const size_t nCount = innerFilePath.size();
    for (size_t nIndex = 0; nIndex < nCount; ++nIndex) {
        if (innerFilePath[nIndex] == L'\\') {
            innerFilePath[nIndex] = L'/';
        }
    }
}

DString ZipManager::GetZipFilePath(const char* szInZipFilePath, bool bUtf8) const
{
    DString filePath;
    if (szInZipFilePath == nullptr) {
        return filePath;
    }
#ifdef DUI_BUILD_FOR_WIN
    #ifdef DUI_UNICODE
        filePath = StringConvert::MBCSToUnicode(szInZipFilePath, bUtf8 ? CP_UTF8 : CP_ACP);
    #else
        if (bUtf8) {
            filePath = szInZipFilePath;            
        }
        else {
            filePath = StringConvert::MBCSToT(szInZipFilePath);
        }
    #endif
#else
    UNUSED_VARIABLE(bUtf8);
    filePath = StringConvert::UTF8ToT(szInZipFilePath);
#endif
    return filePath;
}

}

