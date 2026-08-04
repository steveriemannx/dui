#include "dui/RenderSkia/FontMgr_Skia.h"
#include "dui/Utils/StringConvert.h"
#include "dui/Utils/PerformanceUtil.h"

#include "dui/RenderSkia/SkiaHeaderBegin.h"
#include "include/core/SkFontMgr.h"
#include "include/core/SkFont.h"
#include "include/core/SkData.h"

#include "include/core/SkRefCnt.h"      // Contains the complete definition of sk_sp
#include "include/core/SkTypeface.h"    // Ensures the complete definition of SkFontMgr

#if defined(SK_BUILD_FOR_MAC)
    #include "include/ports/SkFontMgr_mac_ct.h"  // macOS-specific API
#endif

#if defined(SK_BUILD_FOR_WIN)
    #include "include/ports/SkTypeface_win.h"
#elif defined(SK_BUILD_FOR_MAC)
    #include "include/ports/SkTypeface_mac.h"
#else
    #include "include/ports/SkFontMgr_fontconfig.h"
    #include "include/ports/SkFontScanner_FreeType.h"
#endif

#include "dui/RenderSkia/SkiaHeaderEnd.h"

#include <map>

namespace ui
{
/** Font data loaded from a file
*/
struct FontFromFile
{
    //Font name
    DString m_fontFamilyName;

    //Font style -> font data
    std::vector<sk_sp<SkTypeface>> m_fontTypefaceList;
};

/** Font manager loaded from files
*/
class FontFileManager
{
public:
    /** Add the result of loading a font file
    */
    bool AddFontTypeface(const sk_sp<SkTypeface>& spTypeface)
    {
        ASSERT(spTypeface != nullptr);
        if (spTypeface == nullptr) {
            return false;
        }

        //After a successful load, add it to the manager for centralized management
        SkString fontName;
        spTypeface->getFamilyName(&fontName);
        ASSERT(!fontName.isEmpty());
        if (fontName.isEmpty()) {
            return false;
        }

        DString fontFamilyName = StringConvert::UTF8ToT(fontName.c_str());
        ASSERT(!fontFamilyName.empty());
        if (fontFamilyName.empty()) {
            return false;
        }
        size_t nCount = m_fontFamilies.size();
        size_t nNewIndex = nCount;
        for (size_t nIndex = 0; nIndex < nCount; ++nIndex) {
            FontFromFile& fontFromFile = m_fontFamilies[nIndex];
            if (fontFromFile.m_fontFamilyName == fontFamilyName) {
                nNewIndex = nIndex;
                break;
            }
        }

        if (nNewIndex >= m_fontFamilies.size()) {
            //Add
            FontFromFile fontFromFile;
            fontFromFile.m_fontFamilyName = fontFamilyName;
            fontFromFile.m_fontTypefaceList.push_back(spTypeface);
            m_fontFamilies.push_back(fontFromFile);
        }
        else {
            //Update
            bool bFoundSame = false;
            FontFromFile& fontFromFile = m_fontFamilies[nNewIndex];
            for (const sk_sp<SkTypeface>& typeface : fontFromFile.m_fontTypefaceList) {
                if (typeface->fontStyle() == spTypeface->fontStyle()) {
                    //Do not add duplicates
                    bFoundSame = true;
                    break;
                }
            }
            if (!bFoundSame) {
                fontFromFile.m_fontTypefaceList.push_back(spTypeface);
            }
        }
        return true;
    }

    /** Create a font
    */
    sk_sp<SkTypeface> MakeTypeface(const DString& fontName, SkFontStyle style)
    {
        sk_sp<SkTypeface> skTypeface;
        if (m_fontFamilies.empty()) {
            return skTypeface;
        }
        for (const FontFromFile& fontFromFile : m_fontFamilies) {
            if (fontName == fontFromFile.m_fontFamilyName) {
                for (const sk_sp<SkTypeface>& typeface : fontFromFile.m_fontTypefaceList) {
                    if (typeface->fontStyle() == style) {
                        //Found a matching font
                        skTypeface = typeface;
                        break;
                    }
                }
            }
        }
        return skTypeface;
    }

    /** Get the number of fonts
    */
    uint32_t GetFontCont() const
    {
        return (uint32_t)m_fontFamilies.size();
    }

    /** Get the font name
    */
    DString GetFontName(uint32_t nIndex) const
    {
        DString fontName;
        if (nIndex < m_fontFamilies.size()) {
            const FontFromFile& fontFromFile = m_fontFamilies[nIndex];
            fontName = fontFromFile.m_fontFamilyName;
        }
        return fontName;
    }

    /** Whether the font is included
    */
    bool HasFontName(const DString& fontName) const
    {
        for (const FontFromFile& fontFromFile : m_fontFamilies) {
            if (fontName == fontFromFile.m_fontFamilyName) {
                return true;
            }
        }
        return false;
    }

    /** Clear the loaded font data
    */
    void Clear()
    {
        m_fontFamilies.clear();
    }

private:
    /** Font name -> font data
    */
    std::vector<FontFromFile> m_fontFamilies;
};

class FontMgr_Skia::TImpl
{
public:
    /** Skia font manager
    */
    sk_sp<SkFontMgr> m_pSkFontMgr;

    /** The list of fonts loaded from files
    */
    FontFileManager m_fontFileMgr;

    /** The default font name
    */
    DString m_defaultFontName;

    /** FontStyleSet cache for font names (some Linux systems create fonts very slowly; a single call takes tens of milliseconds, so caching is necessary)
    */
    std::map<std::string, sk_sp<SkFontStyleSet>> m_fontStyleSetMap;
};

FontMgr_Skia::FontMgr_Skia()
{
    m_impl = new TImpl;

    //Create the Skia font manager object, unique within the process
#if defined(SK_BUILD_FOR_WIN)
    //Windows system
    m_impl->m_pSkFontMgr = SkFontMgr_New_DirectWrite();
#elif defined(SK_BUILD_FOR_MAC) 
    // macOS/iOS systems
    m_impl->m_pSkFontMgr = SkFontMgr_New_CoreText(nullptr);  // nullptr means the system default font set
#else
    //Linux system, FreeBSD system
    m_impl->m_pSkFontMgr = SkFontMgr_New_FontConfig(nullptr, SkFontScanner_Make_FreeType());
#endif

    ASSERT(m_impl->m_pSkFontMgr != nullptr);
}

FontMgr_Skia::~FontMgr_Skia()
{
    if (m_impl != nullptr) {
        delete m_impl;
        m_impl = nullptr;
    }
}

uint32_t FontMgr_Skia::GetFontCount() const
{
    uint32_t nFontCount = 0;
    if (m_impl->m_pSkFontMgr != nullptr) {
        nFontCount = (uint32_t)m_impl->m_pSkFontMgr->countFamilies();
    }
    nFontCount += m_impl->m_fontFileMgr.GetFontCont();
    return nFontCount;
}

bool FontMgr_Skia::GetFontName(uint32_t nIndex, DString& fontName) const
{
    fontName.clear();
    ASSERT(m_impl->m_pSkFontMgr != nullptr);
    if (m_impl->m_pSkFontMgr == nullptr) {
        return false;
    }

    uint32_t nFontCount = GetFontCount();
    ASSERT(nIndex < nFontCount);
    if (nIndex >= nFontCount) {
        return false;
    }
    if (nIndex < (uint32_t)m_impl->m_pSkFontMgr->countFamilies()) {
        SkString fontFamilyName;
        m_impl->m_pSkFontMgr->getFamilyName((int)nIndex, &fontFamilyName);
        fontName = StringConvert::UTF8ToT(fontFamilyName.c_str());
    }
    else {
        uint32_t nFontFileIndex = nIndex - (uint32_t)m_impl->m_pSkFontMgr->countFamilies();
        fontName = m_impl->m_fontFileMgr.GetFontName(nFontFileIndex);
    }

    return !fontName.empty();
}

bool FontMgr_Skia::HasFontName(const DString& fontName) const
{
    if (fontName.empty()) {
        return false;
    }

    ASSERT(m_impl->m_pSkFontMgr != nullptr);
    if (m_impl->m_pSkFontMgr == nullptr) {
        return false;
    }
    bool bFound = false;
    std::string fontFamily = StringConvert::TToUTF8(fontName); //Convert to UTF8 format
    int nCountFamilies = m_impl->m_pSkFontMgr->countFamilies();
    for (int nIndex = 0; nIndex < nCountFamilies; ++nIndex) {
        SkString fontFamilyName;
        m_impl->m_pSkFontMgr->getFamilyName((int)nIndex, &fontFamilyName);
        if (!fontFamilyName.isEmpty() && (fontFamily == fontFamilyName.c_str())) {
            bFound = true;
            break;
        }
    }
    if (!bFound) {
        bFound = m_impl->m_fontFileMgr.HasFontName(fontName);
    }
    return bFound;
}

void FontMgr_Skia::SetDefaultFontName(const DString& fontName)
{
    if (HasFontName(fontName)) {
        //The font must exist
        m_impl->m_defaultFontName = fontName;
    }
    else {
        ASSERT(0);
    }
}

bool FontMgr_Skia::LoadFontFile(const DString& fontFilePath)
{
    ASSERT(!fontFilePath.empty());
    if (fontFilePath.empty()) {
        return false;
    }

    ASSERT(m_impl->m_pSkFontMgr != nullptr);
    if (m_impl->m_pSkFontMgr == nullptr) {
        return false;
    }

    std::string fontFile = StringConvert::TToUTF8(fontFilePath); //Convert to a UTF8-format path
    ASSERT(!fontFile.empty());
    if (fontFile.empty()) {
        return false;
    }
    sk_sp<SkTypeface> spTypeface = m_impl->m_pSkFontMgr->makeFromFile(fontFile.c_str());
    return m_impl->m_fontFileMgr.AddFontTypeface(spTypeface);
}

bool FontMgr_Skia::LoadFontFileData(const void* data, size_t length)
{
    ASSERT(data != nullptr);
    ASSERT(length > 0);
    if ((data == nullptr) || (length == 0)) {
        return false;
    }
    sk_sp<SkData> skData = SkData::MakeWithCopy(data, length);
    ASSERT(skData != nullptr);
    if (skData == nullptr) {
        return false;
    }
    sk_sp<SkTypeface> spTypeface = m_impl->m_pSkFontMgr->makeFromData(skData);
    return m_impl->m_fontFileMgr.AddFontTypeface(spTypeface);
}

void FontMgr_Skia::ClearFontFiles()
{
    m_impl->m_fontFileMgr.Clear();
}

void FontMgr_Skia::ClearFontCache()
{
    m_impl->m_fontStyleSetMap.clear();
}

SkFont* FontMgr_Skia::CreateSkFont(const UiFont& fontInfo)
{
    PerformanceStat statPerformance(_T("FontMgr_Skia::CreateSkFont"));
    ASSERT(!fontInfo.m_fontName.empty());
    if (fontInfo.m_fontName.empty()) {
        return nullptr;
    }

    ASSERT(fontInfo.m_fontSize != 0);
    if (fontInfo.m_fontSize == 0) {
        return nullptr;
    }

    SkFontStyle fontStyle = SkFontStyle::Normal();
    if (fontInfo.m_bBold && fontInfo.m_bItalic) {
        fontStyle = SkFontStyle::BoldItalic();
    }
    else if (fontInfo.m_bBold) {
        fontStyle = SkFontStyle::Bold();
    }
    else if (fontInfo.m_bItalic) {
        fontStyle = SkFontStyle::Italic();
    }
    sk_sp<SkFontMgr> pSkFontMgr = m_impl->m_pSkFontMgr;
    ASSERT(pSkFontMgr != nullptr);
    if (pSkFontMgr == nullptr) {
        return nullptr;
    }

    //The list of fonts to be created (including the default font)
    std::vector<DString> fontNameList;
    if (!fontInfo.m_fontName.empty() && HasFontName(fontInfo.m_fontName.c_str())) {
        fontNameList.push_back(fontInfo.m_fontName.c_str());
    }
    if (!m_impl->m_defaultFontName.empty() && (m_impl->m_defaultFontName != fontInfo.m_fontName) && HasFontName(m_impl->m_defaultFontName)) {
        fontNameList.push_back(m_impl->m_defaultFontName);
    }

    sk_sp<SkTypeface> spTypeface;
    for (const DString& inFontName : fontNameList) {
        //First check whether the externally loaded fonts meet the requirements; if no match, create via the system fonts
        spTypeface = m_impl->m_fontFileMgr.MakeTypeface(inFontName.c_str(), fontStyle);
        if (spTypeface != nullptr) {
            break;
        }

        //Create the font using the FontMgr interface
        std::string fontName = StringConvert::TToUTF8(inFontName);
        ASSERT(!fontName.empty());
        if (fontName.empty()) {
            continue;
        }
        sk_sp<SkFontStyleSet> skFontStyleSet;
        auto iter = m_impl->m_fontStyleSetMap.find(fontName.c_str());
        if (iter != m_impl->m_fontStyleSetMap.end()) {
            skFontStyleSet = iter->second;
        }
        if (skFontStyleSet == nullptr) {
            skFontStyleSet = pSkFontMgr->matchFamily(fontName.c_str());
            if (skFontStyleSet != nullptr) {
                m_impl->m_fontStyleSetMap[fontName] = skFontStyleSet;
            }
        }
        if (skFontStyleSet != nullptr) {
            spTypeface = skFontStyleSet->matchStyle(fontStyle);
        }
        if (spTypeface != nullptr) {
            break;
        }
    }

    if (spTypeface == nullptr) {
        //Use the system default font (but correctness is not guaranteed; for example, on Windows, the font created by this interface cannot display Chinese)
        spTypeface = pSkFontMgr->legacyMakeTypeface(nullptr, fontStyle);
    }
    ASSERT(spTypeface != nullptr);
    if (spTypeface == nullptr) {
        return nullptr;
    }
    SkFont* skFont = new SkFont();
    skFont->setTypeface(spTypeface);
    skFont->setSize(SkIntToScalar(std::abs(fontInfo.m_fontSize)));
    skFont->setEdging(SkFont::Edging::kSubpixelAntiAlias);
    skFont->setSubpixel(true);
    return skFont;
}

void FontMgr_Skia::DeleteSkFont(SkFont* pSkFont)
{
    if (pSkFont != nullptr) {
        delete pSkFont;
    }
}

void* FontMgr_Skia::GetSkiaFontMgrPtr() const
{
    return &(m_impl->m_pSkFontMgr);
}

} // namespace ui
