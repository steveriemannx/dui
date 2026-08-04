#include "dui/Core/CursorManager.h"

#ifdef DUI_BUILD_FOR_SDL

#include "dui/Core/GlobalManager.h"
#include "dui/Core/Window.h"
#include "dui/Core/Control.h"
#include "dui/Utils/FilePathUtil.h"
#include "dui/Utils/FileUtil.h"
#include <map>

#include <SDL3/SDL.h>

namespace ui
{
class CursorManager::TImpl
{
public:
    TImpl();
    ~TImpl();

    /** Handles of the loaded cursor resources
    */
    std::map<FilePath, SDL_Cursor*> m_cursorMap;

    /** System cursors
    */
    std::map<CursorType, SDL_Cursor*> m_systemCursorMap;
};

CursorManager::TImpl::TImpl()
{
}

CursorManager::TImpl::~TImpl()
{
    for (auto iter : m_cursorMap) {
        if (iter.second != nullptr) {
            SDL_DestroyCursor(iter.second);
        }
    }
    for (auto iter : m_systemCursorMap) {
        if (iter.second != nullptr) {
            SDL_DestroyCursor(iter.second);
        }
    }
}

CursorManager::CursorManager()
{
    m_impl = new TImpl;
}

CursorManager::~CursorManager()
{
    if (m_impl != nullptr) {
        delete m_impl;
        m_impl = nullptr;
    }
}

bool CursorManager::SetCursor(CursorType cursorType)
{
    auto iter = m_impl->m_systemCursorMap.find(cursorType);
    if (iter != m_impl->m_systemCursorMap.end()) {
        ASSERT(iter->second != nullptr);
        bool nRet = SDL_SetCursor(iter->second);
        ASSERT_UNUSED_VARIABLE(nRet);
        return nRet;
    }
    
    SDL_Cursor* sdlCursor = nullptr;
    switch (cursorType) {
    case CursorType::kCursorArrow:
        sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
        break;
    case CursorType::kCursorIBeam:
        sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_TEXT);
        break;
    case CursorType::kCursorHand:
        sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
        break;
    case CursorType::kCursorWait:
        sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
        break;
    case CursorType::kCursorCross:
        sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
        break;
    case CursorType::kCursorSizeWE:
        sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_EW_RESIZE);
        break;
    case CursorType::kCursorSizeNS:
        sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NS_RESIZE);
        break;
    case CursorType::kCursorSizeNWSE:
        sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NWSE_RESIZE);
        break;
    case CursorType::kCursorSizeNESW:
        sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NESW_RESIZE);
        break;
    case CursorType::kCursorSizeAll:
        sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE);
        break;    
    case CursorType::kCursorNo:
        sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NOT_ALLOWED);
        break;
    case CursorType::kCursorProgress:
        sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_PROGRESS);
        break;
    default:
        break;
    }

    if (sdlCursor == nullptr) {
        //Default cursor
        sdlCursor = SDL_GetDefaultCursor();
    }
    else {
        m_impl->m_systemCursorMap[cursorType] = sdlCursor;
    }

    bool nRet = false;
    ASSERT(sdlCursor != nullptr);
    if (sdlCursor != nullptr) {
        nRet = SDL_SetCursor(sdlCursor);
        ASSERT_UNUSED_VARIABLE(nRet);
    }
    return nRet;
}

/** Load a cursor from in-memory data
*/
static SDL_Cursor* LoadCursorFromData(const Window* pWindow, std::vector<uint8_t>& fileData, const FilePath& imageFilePath)
{
    if (fileData.empty() || (pWindow == nullptr)) {
        return nullptr;
    }
    
    ImageDecoderFactory& imageDecoders = GlobalManager::Instance().ImageDecoders();
    float fImageSizeScale = pWindow->Dpi().GetDisplayScale();
    ImageDecodeParam decodeParam;
    decodeParam.m_imageFilePath = imageFilePath;
    decodeParam.m_fImageSizeScale = fImageSizeScale;
    decodeParam.m_pFileData = std::make_shared<std::vector<uint8_t>>(fileData);
    std::shared_ptr<IBitmap> pBitmap = imageDecoders.DecodeImageData(decodeParam);
    if (pBitmap == nullptr) {
        return nullptr;
    }
    uint32_t nWidth = pBitmap->GetWidth();
    uint32_t nHeight = pBitmap->GetHeight();
    if ((nWidth < 1) || (nHeight < 1)) {
        return nullptr;
    }

    void* pPixelBits = pBitmap->LockPixelBits();
    ASSERT(pPixelBits != nullptr);
    if (pPixelBits == nullptr) {
        return nullptr;
    }

#ifdef DUI_BUILD_FOR_WIN
    //Swap R and G, the Windows platform uses the ABGR format and needs to be converted to the RGBA32 format
    // RGBA pixel structure
    struct CurRGBA {
        uint8_t r, g, b, a;
    };
    ASSERT(sizeof(CurRGBA) == 4);
    CurRGBA* pPixelBitsRGBA = (CurRGBA*)pPixelBits;
    const int32_t nImageHeight = pBitmap->GetHeight();
    const int32_t nImageWidth = pBitmap->GetWidth();
    for (int y = 0; y < nImageHeight; y++) {
        for (int x = 0; x < nImageWidth; x++) {
            CurRGBA& pixelColor = pPixelBitsRGBA[y * nImageWidth + x];
            std::swap(pixelColor.b, pixelColor.r);
        }
    }
#endif
    SDL_Surface* cursorSurface = SDL_CreateSurfaceFrom(nWidth, nHeight, SDL_PIXELFORMAT_RGBA32, pPixelBits, nWidth * sizeof(uint32_t));
    ASSERT(cursorSurface != nullptr);
    if (cursorSurface == nullptr) {
        return nullptr;
    }

    int hot_x = 0;
    int hot_y = 0;
    DString imagePath = imageFilePath.ToString();
    size_t nDot = imagePath.rfind(_T('.'));
    if ((nDot != DString::npos) && (fileData.size() > 16)) {
        DString ext = imagePath.substr(nDot);
        if (StringUtil::IsEqualNoCase(ext, _T(".cur"))) {
            //hot_x is located at 0x0A, 2 bytes
            hot_x = (fileData[0x0B] << 8) | fileData[0x0A];
            //hot_y is located at 0x0C, 2 bytes
            hot_y = (fileData[0x0D] << 8) | fileData[0x0C];
            if ((hot_x >= (int)pBitmap->GetWidth()) || (hot_x < 0)) {
                hot_x = 0;
            }
            if ((hot_y >= (int)pBitmap->GetHeight()) || (hot_y < 0)){
                hot_y = 0;
            }
            pWindow->Dpi().ScaleInt(hot_x);
            pWindow->Dpi().ScaleInt(hot_y);
        }
    }

    SDL_Cursor* sdlCursor = SDL_CreateColorCursor(cursorSurface, hot_x, hot_y);
    SDL_DestroySurface(cursorSurface);
    cursorSurface = nullptr;

    ASSERT(sdlCursor != nullptr);
    return sdlCursor;
}

bool CursorManager::SetImageCursor(const Window* pWindow, const FilePath& curImagePath)
{
    ASSERT(!curImagePath.IsEmpty());
    ASSERT(pWindow != nullptr);
    if ((pWindow == nullptr) || curImagePath.IsEmpty()) {
        return false;
    }

    //Set the window icon
    FilePath cursorFullPath = GlobalManager::Instance().GetExistsResFullPath(pWindow->GetResourcePath(), pWindow->GetXmlPath(), curImagePath);
    ASSERT(!cursorFullPath.IsEmpty());
    if (cursorFullPath.IsEmpty()) {
        return false;
    }

    SDL_Cursor* sdlCursor = nullptr;
    auto iter = m_impl->m_cursorMap.find(cursorFullPath);
    if (iter != m_impl->m_cursorMap.end()) {
        sdlCursor = iter->second;
    }
    else {
        //Load the cursor
        std::vector<uint8_t> fileData;
        if (GlobalManager::Instance().Zip().IsUseZip() && GlobalManager::Instance().Zip().IsZipResExist(cursorFullPath)) {
            //Use the zip package
            bool bRet = GlobalManager::Instance().Zip().GetZipData(cursorFullPath, fileData);
            ASSERT_UNUSED_VARIABLE(bRet);
        }
        else {
            //Use the local file
            bool bRet = FileUtil::ReadFileData(cursorFullPath, fileData);
            ASSERT_UNUSED_VARIABLE(bRet);
        }
        ASSERT(!fileData.empty());
        if (!fileData.empty()) {
            //Load the cursor from memory
            sdlCursor = LoadCursorFromData(pWindow, fileData, curImagePath);
            ASSERT(sdlCursor != nullptr);
            if (sdlCursor != nullptr) {
                m_impl->m_cursorMap[cursorFullPath] = sdlCursor;
            }
        }
    }
    ASSERT(sdlCursor != nullptr);
    if (sdlCursor != nullptr) {
        bool nRet = SDL_SetCursor(sdlCursor);
        ASSERT_UNUSED_VARIABLE(nRet);
        return nRet;
    }
    return false;
}

bool CursorManager::ShowCursor(bool bShow)
{
    bool nRet = false;
    if (bShow) {
        nRet = SDL_ShowCursor();
    }
    else {
        nRet = SDL_HideCursor();
    }
    ASSERT(nRet);
    return nRet;
}

CursorID CursorManager::GetCursorID() const
{
    return (CursorID)SDL_GetCursor();
}

bool CursorManager::SetCursorByID(CursorID cursorId)
{
    bool nRet = SDL_SetCursor((SDL_Cursor*)cursorId);
    ASSERT(nRet);
    return nRet;
}

} // namespace ui

#endif // DUI_BUILD_FOR_WIN
