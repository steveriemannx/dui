#ifndef UI_CEF_CONTROL_MEMORY_BLOCK_H_
#define UI_CEF_CONTROL_MEMORY_BLOCK_H_

#include "dui/dui_config.h"

#ifdef DUI_BUILD_FOR_CEF

#include <mutex>
#include <vector>

namespace ui {

class IRender;
class UiRect;

/** Wrapper for a bitmap data memory block
 */
class CefMemoryBlock
{
public:
    CefMemoryBlock();
    ~CefMemoryBlock();

    /** Initialize the bitmap data
    */
    bool Init(const void* buffer, const std::vector<UiRect>& dirtyRectList, int32_t width, int32_t height);

    /** Draw the bitmap data to the Render
    * @param [in] pRender the render interface
    * @param [in] rc the target region to draw
    */
    void PaintData(IRender* pRender, const UiRect& rc);

    /** Check whether the memory block is usable
    */
    bool IsValid() const;

    /** Draw the web page data to the Render, keeping the size consistent with the web page
    */
    bool MakeImageSnapshot(IRender* pRender);

    /** Get the width of the in-memory bitmap
    */
    int32_t GetWidth() const;

    /** Get the height of the in-memory bitmap
    */
    int32_t GetHeight() const;

private:
    /** Delete the bitmap data and initialize the variables
    */
    void Clear();

    /** Get the in-memory bitmap data pointer, which can be used to fill in the bitmap data
    */
    uint8_t* GetBits() const;

private:
    /** The memory data
    */
    uint8_t* m_pBits;

    /** The width of the in-memory bitmap
    */
    int32_t m_nWidth;

    /** The height of the in-memory bitmap
    */
    int32_t m_nHeight;

    /** The multi-thread synchronization lock for the data
    */
    std::mutex m_memMutex;
};

} //namespace ui

#endif //DUI_BUILD_FOR_CEF

#endif //UI_CEF_CONTROL_MEMORY_BLOCK_H_
