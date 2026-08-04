#include "dui/CEFControl/internal/CefMemoryBlock.h"

#ifdef DUI_BUILD_FOR_CEF

#include "dui/Render/IRender.h"

namespace ui {

CefMemoryBlock::CefMemoryBlock():
    m_pBits(nullptr),
    m_nWidth(0),
    m_nHeight(0)
{
}

CefMemoryBlock::~CefMemoryBlock()
{
    Clear();
}

bool CefMemoryBlock::Init(const void* buffer, const std::vector<UiRect>& dirtyRectList, int32_t width, int32_t height)
{
    std::lock_guard<std::mutex> threadGuard(m_memMutex);
    ASSERT((width > 0) && (height > 0) && (buffer != nullptr));
    if ((width <= 0) || (height <= 0) || (buffer == nullptr)) {
        return false;
    }

    bool bDirtyRectValid = !dirtyRectList.empty();
    for (const UiRect& rect : dirtyRectList) {
        if ((rect.left < 0) || (rect.right > width) ||
            (rect.top < 0) || (rect.bottom > height) ||
            (rect.right <= rect.left) || (rect.bottom <= rect.top)) {
            bDirtyRectValid = false;
            break;
        }
    }

    if ((m_nWidth != width) || (m_nHeight != height) || (m_pBits == nullptr)) {
        // Needs to be re-initialized
        if (m_pBits != nullptr) {
            delete[] m_pBits;
            m_pBits = nullptr;
        }
        m_pBits = new uint8_t[width * height * sizeof(uint32_t)];
        m_nWidth = width;
        m_nHeight = height;
        bDirtyRectValid = false;
    }
    // Copy the data
    if (m_pBits != nullptr) {
        if (!bDirtyRectValid || dirtyRectList.empty() || dirtyRectList[0] == UiRect(0, 0, width, height)) {
            // Full rendering
            memcpy(m_pBits, buffer, width * height * sizeof(uint32_t));
        }
        else {
            // Incremental rendering: only draw the dirty regions
            uint32_t* pBmpBits = (uint32_t*)m_pBits;
            int32_t offset = 0;
            for (const UiRect& rect : dirtyRectList) {
                for (int32_t i = rect.top; i < rect.bottom; ++i) {
                    // Copy the data row by row
                    offset = i * width + rect.left;
                    memcpy(pBmpBits + offset, (uint32_t*)buffer + offset, rect.Width() * sizeof (uint32_t));
                }
            }
        }
        return true;
    }
    return false;
}

void CefMemoryBlock::Clear()
{
    std::lock_guard<std::mutex> threadGuard(m_memMutex);
    if (m_pBits != nullptr) {
        delete[] m_pBits;
        m_pBits = nullptr;
    }
    m_nWidth = 0;
    m_nHeight = 0;
}

void CefMemoryBlock::PaintData(IRender* pRender,const UiRect& rc)
{
    std::lock_guard<std::mutex> threadGuard(m_memMutex);
    // Using the interface that writes data directly gives the best performance
    if ((GetWidth() <= 0) || (GetHeight() <= 0) || rc.IsEmpty()) {
        return;
    }
    UiRect rcMemory;
    rcMemory.left = rc.left;
    rcMemory.top = rc.top;
    rcMemory.right = rcMemory.left + GetWidth();
    rcMemory.bottom = rcMemory.top + GetHeight();
    if (!rcMemory.IsEmpty() && IsValid()) {
        bool bRet = false;
        if ((rc.Width() == GetWidth()) && (rc.Height() == GetHeight())) {
            // The target region is the same size as the image data; write directly
            bRet = pRender->WritePixels(GetBits(), GetWidth() * GetHeight() * sizeof(uint32_t), rcMemory);
        }
        else {
            const UiRect& rcPaint = rc;//the actual dirty region (avoid drawing out of bounds, because WritePixels ignores Clip, which would cause out-of-bounds drawing and cover other controls)
            bRet = pRender->WritePixels(GetBits(), GetWidth() * GetHeight() * sizeof(uint32_t), rcMemory, rcPaint);
        }        
        ASSERT_UNUSED_VARIABLE(bRet);
    }
}

bool CefMemoryBlock::MakeImageSnapshot(IRender* pRender)
{
    std::lock_guard<std::mutex> threadGuard(m_memMutex);
    int32_t nWidth = GetWidth();
    int32_t nHeight = GetHeight();
    uint8_t* pBits = GetBits();
    if ((nWidth < 1) || (nHeight < 1) || (pBits == nullptr) || (pRender == nullptr)) {
        return false;
    }
    UiRect dcPaint;
    dcPaint.left = 0;
    dcPaint.top = 0;
    dcPaint.right = dcPaint.left + nWidth;
    dcPaint.bottom = dcPaint.top + nHeight;

    if (pRender->Resize(nWidth, nHeight)) {
        return pRender->WritePixels(pBits, nWidth * nHeight * sizeof(uint32_t), dcPaint);
    }
    return false;    
}

bool CefMemoryBlock::IsValid() const
{
    return (m_pBits != nullptr);
}

uint8_t* CefMemoryBlock::GetBits() const
{
    return m_pBits;
}

int32_t CefMemoryBlock::GetWidth() const
{
    return m_nWidth;
}

int32_t CefMemoryBlock::GetHeight() const
{
    return m_nHeight;
}

} //namespace ui

#endif //DUI_BUILD_FOR_CEF
