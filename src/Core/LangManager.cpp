#include "dui/Core/LangManager.h"
#include "dui/Utils/StringUtil.h"
#include "dui/Utils/StringConvert.h"
#include "dui/Utils/FileUtil.h"

namespace ui 
{
LangManager::LangManager()
{
};

LangManager::~LangManager()
{
    m_stringTable.clear();
};

bool LangManager::LoadStringTable(const FilePath& strFilePath)
{
    m_stringTable.clear();
    std::vector<uint8_t> fileData;
    FileUtil::ReadFileData(strFilePath, fileData);
    if (fileData.empty()) {
        return false;
    }
    return LoadStringTable(fileData);
}

bool LangManager::LoadStringTable(const std::vector<uint8_t>& fileData)
{
    std::vector<std::string> string_list;
    if (fileData.empty()) {
        return false;
    }
    size_t bomSize = 0;
    if ((fileData.size() >= 3)   &&
        (fileData.at(0) == 0xEF) &&
        (fileData.at(1) == 0xBB) &&
        (fileData.at(2) == 0xBF) ) {
        //Skip the UTF8 BOM header
        bomSize = 3;
    }
    std::string fragment((const char*)fileData.data() + bomSize, fileData.size() - bomSize);
    StringUtil::ReplaceAll("\r\n", "\n", fragment);
    StringUtil::ReplaceAll("\r", "\n", fragment);
    fragment.append("\n");
    std::string src;
    std::string string_resourse;
    for (const auto& it : fragment)    {
        if (it == '\0' || it == '\n') {
            if (!src.empty()) {
                string_resourse.clear();
                string_resourse = StringConvert::UTF8ToT(src);
                string_resourse = StringUtil::Trim(string_resourse);
                if (!string_resourse.empty()) {
                    string_list.push_back(string_resourse);
                }
                src.clear();
            }
            continue;
        }
        src.push_back(it);
    }
    AnalyzeStringTable(string_list);
    return true;
}

void LangManager::ClearStringTable()
{
    m_stringTable.clear();
}

bool LangManager::AnalyzeStringTable(const std::vector<std::string>& list)
{
    int    nCount = (int)list.size();
    if (nCount <= 0) {
        return false;
    }
    std::string id;
    std::string strResource;
    for (int i = 0; i < nCount; ++i) {
        const std::string& strSrc = list[i];
        if (strSrc.empty() || strSrc.at(0) == ';') {
            //The comment starts with ";"
            continue;
        }
        size_t pos = strSrc.find('=');
        if (pos == std::string::npos) {
            //No separator, ignore it
            continue;
        }

        id = strSrc.substr(0, pos);
        if (!id.empty()) {
            id = StringUtil::Trim(id);
        }
        if ((pos + 1) < strSrc.size()) {
            strResource = strSrc.substr(pos + 1);
            strResource = StringUtil::Trim(strResource);
            //Replace \n and \r with the real line feed and carriage return characters
            StringUtil::ReplaceAll("\\r", "\r", strResource);
            StringUtil::ReplaceAll("\\n", "\n", strResource);
        }
        else {
            strResource.clear();
        }
        if (!id.empty()) {
            m_stringTable[id] = strResource;
        }
    }
    return true;
}

std::string LangManager::GetStringViaID(const std::string& id)
{
    std::string text;
    if (id.empty()) {
        return text;
    }
    auto it = m_stringTable.find(id);
    if (it == m_stringTable.end()) {
        ASSERT(!"MultiLang::GetStringViaID failed!");
        return text;
    }
    else {
        text = it->second;
    }
    return text;
}

}//namespace ui 
