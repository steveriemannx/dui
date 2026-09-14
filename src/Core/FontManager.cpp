#include "dui/Core/FontManager.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/DpiManager.h"
#include "dui/Render/IRender.h"
#include "dui/Utils/StringUtil.h"
#include "dui/Utils/FilePathUtil.h"
#include "dui/Utils/LogUtil.h"

namespace ui 
{

FontManager::FontManager():
    m_bDefaultFontInited(false)
{
}

FontManager::~FontManager()
{
    RemoveAllFonts();
    RemoveAllFontFiles();
}

bool FontManager::AddFont(const std::string& fontId, const UiFont& fontInfo, bool bDefault)
{
    if (fontId.empty()) {
        return false;
    }

    ASSERT(fontInfo.m_fontSize > 0);
    if (fontInfo.m_fontSize <= 0) {
        //The assert above is gone in a release build, and without this line the font
        //id is simply missing from then on
        DUI_LOG_WARN(StringUtil::Printf("font \"%s\" was not added: size %d is not a usable size",
                                        fontId.c_str(),
                                        fontInfo.m_fontSize));
        return false;
    }

    auto iter = m_fontIdMap.find(fontId);
    if (iter != m_fontIdMap.end()) {
        //Avoid adding the same font ID repeatedly
        return false;
    }

    //Save the font info, but do not create the font data
    m_fontIdMap[fontId] = fontInfo;
    if (bDefault) {
        //The default font ID
        m_defaultFontId = fontId;
    }
    return true;
}

const std::string& FontManager::GetDefaultFontId() const
{
    return m_defaultFontId;
}

void FontManager::SetDefaultFontFamilyNames(const std::string& defaultFontFamilyNames)
{
    m_defaultFontFamilyNames.clear();
    m_bDefaultFontInited = false;
    if (!defaultFontFamilyNames.empty()) {
        std::list<std::string> fontFamilyNames = StringUtil::Split(defaultFontFamilyNames, ",");
        for (std::string fontFamilyName : fontFamilyNames) {
            StringUtil::Trim(fontFamilyName);
            if (!fontFamilyName.empty()) {
                m_defaultFontFamilyNames.push_back(fontFamilyName);
            }
        }
    }
}

std::string FontManager::GetDpiFontId(const std::string& fontId, uint32_t nZoomPercent) const
{
    std::string dpiFontId;
    if (!fontId.empty()) {
        dpiFontId = fontId + "@" + StringUtil::UInt32ToString(nZoomPercent);
    }
    return dpiFontId;
}

IFont* FontManager::GetIFont(const std::string& fontId, const DpiManager& dpi)
{
    return GetIFont(fontId, dpi.GetDisplayScaleFactor());
}

IFont* FontManager::GetIFont(const std::string& fontId, uint32_t nZoomPercent)
{
    if (nZoomPercent == 0) {
        nZoomPercent = 100;
    }
    //First search the cache
    IFont* pFont = nullptr;
    if (!fontId.empty()) {        
        std::string dpiFontId = GetDpiFontId(fontId, nZoomPercent);
        auto iter = m_fontMap.find(dpiFontId);
        if (iter != m_fontMap.end()) {
            pFont = iter->second;
        }
    }
    if (pFont == nullptr) {
        auto iter = m_fontIdMap.find(fontId);
        if ((iter == m_fontIdMap.end()) && !m_defaultFontId.empty()) {
            //This font ID does not exist, use the default font ID
            std::string dpiFontId = GetDpiFontId(m_defaultFontId, nZoomPercent);
            auto pos = m_fontMap.find(dpiFontId);
            if (pos != m_fontMap.end()) {
                pFont = pos->second;
            }
        }
    }
    if (pFont != nullptr) {
        //Use the font data already created in the cache
        return pFont;
    }

    //Not in the cache, the font needs to be created
    UiFont fontInfo;
    std::string realFontId = fontId;
    auto iter = m_fontIdMap.find(realFontId);
    if (iter == m_fontIdMap.end()) {
        realFontId = m_defaultFontId;
        iter = m_fontIdMap.find(realFontId);
        if (iter != m_fontIdMap.end()) {
            fontInfo = iter->second;
        }
        else {
            realFontId.clear();
        }
    }
    else {
        fontInfo = iter->second;
    }
    if (realFontId.empty()) {
        //No such font ID
        return nullptr;
    }
    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    if (pRenderFactory == nullptr) {
        return nullptr;
    }

    //Initialize the default font family names
    IFontMgr* pFontMgr = pRenderFactory->GetFontMgr();
    if (!m_bDefaultFontInited && !m_defaultFontFamilyNames.empty() && (pFontMgr != nullptr)) {
        std::string missingFontNames;
        auto pos = m_defaultFontFamilyNames.begin();
        while (pos != m_defaultFontFamilyNames.end()) {
            const std::string& fontFamilyName = *pos;
            if (!pFontMgr->HasFontName(fontFamilyName)) {
                //Remove fonts that do not exist
                missingFontNames += StringUtil::Printf("\"%s\" ", fontFamilyName.c_str());
                pos = m_defaultFontFamilyNames.erase(pos);
            }
            else {
                break;
            }
        }
        m_bDefaultFontInited = true;
        StringUtil::TrimRight(missingFontNames);
        if (!m_defaultFontFamilyNames.empty()) {
            pFontMgr->SetDefaultFontName(m_defaultFontFamilyNames.front());
            if (!missingFontNames.empty()) {
                //A font that is configured but not installed is otherwise invisible:
                //the text simply comes out in another face
                DUI_LOG_WARN(StringUtil::Printf("default font families not installed: %s; text uses \"%s\"",
                                                missingFontNames.c_str(),
                                                m_defaultFontFamilyNames.front().c_str()));
            }
        }
        else {
            DUI_LOG_WARN(StringUtil::Printf("none of the default font families is installed (%s); text uses the platform's default typeface",
                                            missingFontNames.c_str()));
        }
    }

    std::string dpiFontId = GetDpiFontId(realFontId, nZoomPercent);
    if (fontInfo.m_fontName.empty() || 
        StringUtil::IsEqualNoCase(fontInfo.m_fontName.c_str(), "system")) {
        if (!m_defaultFontFamilyNames.empty()) {
            fontInfo.m_fontName = m_defaultFontFamilyNames.front();
        }
        else {
            fontInfo.m_fontName = "Microsoft YaHei"; //Microsoft YaHei; this line is unreachable if a default font is set
        }
    }

    //Apply DPI scaling to the font size
    ASSERT(fontInfo.m_fontSize > 0);
    if (nZoomPercent != 100) {
        fontInfo.m_fontSize = DpiManager::MulDiv(fontInfo.m_fontSize, (int32_t)nZoomPercent, 100);
        if (fontInfo.m_fontSize < 1) {
            fontInfo.m_fontSize = 1;
        }
    }

    pFont = pRenderFactory->CreateIFont();
    if (pFont == nullptr) {
        return nullptr;
    }
    bool isInitOk = pFont->InitFont(fontInfo);
    if (!isInitOk) {
        DUI_LOG_WARN(StringUtil::Printf("font \"%s\" (face \"%s\", size %d) could not be created; text using it falls back to the default font",
                                        realFontId.c_str(),
                                        fontInfo.m_fontName.c_str(),
                                        (int32_t)fontInfo.m_fontSize));
        delete pFont;
        pFont = nullptr;
        return nullptr;
    }
    m_fontMap.insert(std::make_pair(dpiFontId, pFont));
    return pFont;
}

bool FontManager::HasFontId(const std::string& fontId) const
{
    auto pos = m_fontIdMap.find(fontId);
    bool bFound = pos != m_fontIdMap.end();
    return bFound;
}

bool FontManager::RemoveFontId(const std::string& fontId)
{
    if (fontId == m_defaultFontId) {
        return false;
    }
    bool bDeleted = false;
    const std::string zoomFontId = fontId + "@";
    auto iter = m_fontMap.begin();
    while (iter != m_fontMap.end()) {
        if (iter->first.find(zoomFontId) == 0) {
            //Matched the font ID
            if (iter->second != nullptr) {
                delete iter->second;//The IFont pointer
            }
            bDeleted = true;
            iter = m_fontMap.erase(iter);
        }
        else {
            ++iter;
        }
    }
    auto pos = m_fontIdMap.find(fontId);
    if (pos != m_fontIdMap.end()) {
        m_fontIdMap.erase(pos);
        bDeleted = true;
    }
    return bDeleted;
}

bool FontManager::RemoveIFont(const std::string& fontId, uint32_t nZoomPercent)
{
    bool bDeleted = false;
    if (!fontId.empty()) {
        std::string realFontId = GetDpiFontId(fontId, nZoomPercent);
        auto iter = m_fontMap.find(realFontId);
        if (iter != m_fontMap.end()) {
            //Matched the font ID
            if (iter->second != nullptr) {
                delete iter->second;//The IFont pointer
            }
            bDeleted = true;
            m_fontMap.erase(iter);
        }
    }
    return bDeleted;
}

void FontManager::RemoveAllFonts()
{
    for (auto fontInfo : m_fontMap) {
        IFont* pFont = fontInfo.second;
        if (pFont != nullptr) {
            delete pFont;
        }
    }
    m_fontMap.clear();
    m_defaultFontId.clear();
    m_fontIdMap.clear();

    IFontMgr* pFontMgr = nullptr;
    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    if (pRenderFactory != nullptr) {
        pFontMgr = pRenderFactory->GetFontMgr();
    }
    if (pFontMgr != nullptr) {
        pFontMgr->ClearFontCache();
    }
}

bool FontManager::AddFontFile(const std::string& strFontFile, const std::string& /*strFontDesc*/)
{
    FilePath fontFilePath = FilePathUtil::JoinFilePath(GlobalManager::Instance().GetFontFilePath(), FilePath(strFontFile));
    IFontMgr* pFontMgr = nullptr;
    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    if (pRenderFactory != nullptr) {
        pFontMgr = pRenderFactory->GetFontMgr();
    }
    if (pFontMgr == nullptr) {
        return false;
    }

    bool bRet = false;
    if (GlobalManager::Instance().MemoryResources().IsOpen()) {
        std::vector<unsigned char> file_data;
        if (GlobalManager::Instance().MemoryResources().GetData(fontFilePath, file_data)) {
            //Load from the memory stream
            bRet = pFontMgr->LoadFontFileData(file_data.data(), file_data.size());
        }
    }
    else {
        //Load from the file
        bRet = pFontMgr->LoadFontFile(fontFilePath.ToString());
    }
    if (!bRet) {
        //The font is not registered at all, so every font id that names it silently
        //resolves to the default face
        DUI_LOG_WARN(StringUtil::Printf("font file \"%s\" could not be loaded", fontFilePath.ToString().c_str()));
    }
    ASSERT(bRet);
    return bRet;
}

void FontManager::RemoveAllFontFiles()
{
    IFontMgr* pFontMgr = nullptr;
    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    if (pRenderFactory != nullptr) {
        pFontMgr = pRenderFactory->GetFontMgr();
    }
    if (pFontMgr != nullptr) {
        pFontMgr->ClearFontFiles();
    }
}

void FontManager::GetFontNameList(std::vector<std::string>& fontNameList) const
{
    fontNameList.clear();
    IFontMgr* pFontMgr = nullptr;
    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    if (pRenderFactory != nullptr) {
        pFontMgr = pRenderFactory->GetFontMgr();
    }
    if (pFontMgr == nullptr) {
        return;
    }
    std::string fontName;
    uint32_t nFontCount = pFontMgr->GetFontCount();
    for (uint32_t nIndex = 0; nIndex < nFontCount; ++nIndex) {
        if (pFontMgr->GetFontName(nIndex, fontName)) {
            fontNameList.push_back(fontName);
        }
    }
    if (!fontNameList.empty()) {
        std::sort(fontNameList.begin(), fontNameList.end());
    }
}

void FontManager::GetFontSizeList(const DpiManager& dpi, std::vector<FontSizeInfo>& fontSizeList) const
{
    fontSizeList.clear();
    fontSizeList.push_back({ "8",  8.0f, 0 });
    fontSizeList.push_back({ "9",  9.0f, 0 });
    fontSizeList.push_back({ "10", 10.0f, 0 });
    fontSizeList.push_back({ "11", 11.0f, 0 });
    fontSizeList.push_back({ "12", 12.0f, 0 });
    fontSizeList.push_back({ "14", 14.0f, 0 });
    fontSizeList.push_back({ "16", 16.0f, 0 });
    fontSizeList.push_back({ "18", 18.0f, 0 });
    fontSizeList.push_back({ "20", 20.0f, 0 });
    fontSizeList.push_back({ "22", 22.0f, 0 });
    fontSizeList.push_back({ "24", 24.0f, 0 });
    fontSizeList.push_back({ "26", 26.0f, 0 });
    fontSizeList.push_back({ "28", 28.0f, 0 });
    fontSizeList.push_back({ "32", 32.0f, 0 });
    fontSizeList.push_back({ "36", 36.0f, 0 });
    fontSizeList.push_back({ "48", 48.0f, 0 });
    fontSizeList.push_back({ "72", 72.0f, 0 });

#ifdef DUI_BUILD_FOR_WIN
    if (::GetACP() == 936) {
        //Only used in the Chinese environment
        fontSizeList.push_back({ "1 inch", 95.6f, 0 });
        fontSizeList.push_back({ "Extra Large", 83.7f, 0 });
        fontSizeList.push_back({ "Extra No.", 71.7f, 0 });
        fontSizeList.push_back({ "No.0", 56.0f, 0 });
        fontSizeList.push_back({ "Small 0", 48.0f, 0 });
        fontSizeList.push_back({ "No.1", 34.7f, 0 });
        fontSizeList.push_back({ "Small 1", 32.0f, 0 });
        fontSizeList.push_back({ "No.2", 29.3f, 0 });
        fontSizeList.push_back({ "Small 2", 24.0f, 0 });
        fontSizeList.push_back({ "No.3", 21.3f, 0 });
        fontSizeList.push_back({ "Small 3", 20.0f, 0 });
        fontSizeList.push_back({ "No.4", 18.7f, 0 });
        fontSizeList.push_back({ "Small 4", 16.0f, 0 });
        fontSizeList.push_back({ "No.5", 14.0f, 0 });
        fontSizeList.push_back({ "Small 5", 12.0f, 0 });
        fontSizeList.push_back({ "No.6", 10.0f, 0 });
        fontSizeList.push_back({ "Small 6", 8.7f, 0 });
        fontSizeList.push_back({ "No.7", 7.3f, 0 });
        fontSizeList.push_back({ "No.8", 6.7f, 0 });
    }
#endif

    //Update the DPI adaptive values
    for (FontSizeInfo& fontSize : fontSizeList) {
        int32_t nSize = static_cast<int32_t>(fontSize.fFontSize * 1000);
        dpi.ScaleInt(nSize);
        fontSize.fDpiFontSize = nSize / 1000.0f;
    }
}

}
