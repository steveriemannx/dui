#include "dui/Core/MemoryResourceManager.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Utils/StringUtil.h"
#include "dui/Utils/StringConvert.h"
#include "dui/Utils/FilePathUtil.h"

namespace ui 
{

MemoryResourceManager::MemoryResourceManager():
    m_bUseMemory(false),
    m_pMemoryData(nullptr),
    m_memoryDataSize(0)
{
}

MemoryResourceManager::~MemoryResourceManager()
{
    Close();
}


bool MemoryResourceManager::IsOpen() const
{
    return m_bUseMemory;
}

bool MemoryResourceManager::Open(const uint8_t* pData, size_t nSize)
{
    Close();
    if ((pData == nullptr) || (nSize < 16)) {
        return false;
    }
    // Binary format (little-endian):
    //   [magic "DUIR" u32][version u32][count u32]
    //   count x [u32 pathLen][path UTF-8][u64 dataOffset][u64 dataLen]
    //   [data blocks]
    // Must match the embed_resources tool (tools/embed_resources.cpp).
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

    std::map<std::wstring, MemoryResData> index;
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
        std::wstring key = StringConvert::UTF8ToWString(pathA);
        key = StringUtil::MakeLowerString(key);
    NormalizePath(key);
        index[key] = { static_cast<size_t>(dataOffset), static_cast<size_t>(dataLen) };
    }

    m_pMemoryData = pData;
    m_memoryDataSize = nSize;
    m_memoryIndex.swap(index);
    m_bUseMemory = true;
    return true;
}

bool MemoryResourceManager::GetData(const FilePath& path, std::vector<unsigned char>& fileData) const
{
    fileData.clear();
    GlobalManager::Instance().AssertUIThread();
    if (m_bUseMemory) {
        const std::wstring key = NormalizeMemoryPath(path);
        auto it = m_memoryIndex.find(key);
        if (it == m_memoryIndex.end()) {
            return false;
        }
        const MemoryResData& data = it->second;
        fileData.assign(m_pMemoryData + data.nOffset, m_pMemoryData + data.nOffset + data.nSize);
        return true;
    }
    return false;
}

bool MemoryResourceManager::IsDataExist(const FilePath& path) const
{
    GlobalManager::Instance().AssertUIThread();
    if (path.IsEmpty()) {
        return false;
    }
    if (m_bUseMemory) {
        const std::wstring key = NormalizeMemoryPath(path);
        return m_memoryIndex.find(key) != m_memoryIndex.end();
    }
    return false;
}

void MemoryResourceManager::Close()
{
    m_bUseMemory = false;
    m_pMemoryData = nullptr;
    m_memoryDataSize = 0;
    m_memoryIndex.clear();
}

std::wstring MemoryResourceManager::NormalizeMemoryPath(const FilePath& path) const
{
    const FilePath normalizePath = FilePathUtil::NormalizeFilePath(path);
    std::wstring innerFilePath = normalizePath.ToStringW();
    innerFilePath = StringUtil::MakeLowerString(innerFilePath);
    NormalizePath(innerFilePath);
    return innerFilePath;
}

bool MemoryResourceManager::GetFileList(const FilePath& dirPath, std::vector<std::string>& fileList) const
{
    fileList.clear();
    GlobalManager::Instance().AssertUIThread();
    std::string filePath = dirPath.NativePath();
    if (!filePath.empty() &&
        (filePath[filePath.size() - 1] != '\\') &&
        (filePath[filePath.size() - 1] != '/')) {
        filePath += "/";
    }
    std::string innerPath = FilePathUtil::NormalizeFilePath(filePath);
    if (innerPath.empty()) {
        return false;
    }
    NormalizePath(innerPath);

    if (m_bUseMemory) {
        std::wstring prefix = StringUtil::MakeLowerString(FilePath(innerPath).ToStringW());
        NormalizePath(prefix);
        for (const auto& kv : m_memoryIndex) {
            const std::wstring& key = kv.first;
            if ((key.size() > prefix.size()) && (key.compare(0, prefix.size(), prefix) == 0)) {
                std::wstring remainder = key.substr(prefix.size());
                if (remainder.find(L'/') == std::wstring::npos) {
                    fileList.push_back(StringConvert::WStringToT(remainder));
                }
            }
        }
        return true;
    }
    return false;
}

void MemoryResourceManager::NormalizePath(std::string& innerFilePath) const
{
    const size_t nCount = innerFilePath.size();
    for (size_t nIndex = 0; nIndex < nCount; ++nIndex) {
        if (innerFilePath[nIndex] == '\\') {
            innerFilePath[nIndex] = '/';
        }
    }
}

void MemoryResourceManager::NormalizePath(std::wstring& innerFilePath) const
{
    const size_t nCount = innerFilePath.size();
    for (size_t nIndex = 0; nIndex < nCount; ++nIndex) {
        if (innerFilePath[nIndex] == L'\\') {
            innerFilePath[nIndex] = L'/';
        }
    }
}

}
