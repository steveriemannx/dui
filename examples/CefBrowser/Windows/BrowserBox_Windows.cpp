#include "BrowserBox_Windows.h"

#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)

#include "Windows/taskbar/TaskbarManager.h"

using namespace ui;

BrowserBox_Windows::BrowserBox_Windows(ui::Window* pWindow, std::string id):
    BrowserBox(pWindow, id),
    m_pTaskBarItem(nullptr)
{
}

void BrowserBox_Windows::InitBrowserBox(const DString& url)
{
    BaseClass::InitBrowserBox(url);
    // Initialize the taskbar thumbnail
    ui::CefControl* pCefControl = GetCefControl();
    if (pCefControl == nullptr) {
        return;
    }
    m_pTaskBarItem = new TaskbarTabItem(pCefControl);
    if (m_pTaskBarItem) {
        m_pTaskBarItem->Init(url, GetBrowserId());
    }
}

void BrowserBox_Windows::UninitBrowserBox()
{
    BaseClass::UninitBrowserBox();
    if (m_pTaskBarItem) {
        m_pTaskBarItem->UnInit();
    }
}

TaskbarTabItem* BrowserBox_Windows::GetTaskbarItem() const
{
    return m_pTaskBarItem;
}

void BrowserBox_Windows::Invalidate()
{
    BaseClass::Invalidate();
    if (m_pTaskBarItem) {
        m_pTaskBarItem->InvalidateTab();
    }
}

void BrowserBox_Windows::SetPos(UiRect rc)
{
    BaseClass::SetPos(rc);
    if (m_pTaskBarItem) {
        m_pTaskBarItem->InvalidateTab();
    }
}

void BrowserBox_Windows::OnTitleChange(CefRefPtr<CefBrowser> browser, const DString& title)
{
    BaseClass::OnTitleChange(browser, title);
    if (m_pTaskBarItem) {
        m_pTaskBarItem->SetTaskbarTitle(title);
    }
}

void BrowserBox_Windows::OnDownloadFavIconFinished(CefRefPtr<CefBrowser> browser,
                                                   const CefString& image_url,
                                                   int http_status_code,
                                                   CefRefPtr<CefImage> image)
{
    BaseClass::OnDownloadFavIconFinished(browser, image_url, http_status_code, image);
    if ((m_pTaskBarItem != nullptr) && (image != nullptr)) {
        HICON hIcon = ConvertCefImageToHICON(*image);
        m_pTaskBarItem->SetTaskbarIcon(hIcon);
    }
}

HICON BrowserBox_Windows::ConvertCefImageToHICON(CefImage& cefImage) const
{
    // 1. Get the CefImage parameters
    int32_t nWidth = cefImage.GetWidth();
    int32_t nHeight = cefImage.GetHeight();
    if ((nWidth < 1) || (nHeight < 1)) {
        return nullptr;
    }
    CefRefPtr<CefBinaryValue> cefImageData = cefImage.GetAsBitmap(1.0f, CEF_COLOR_TYPE_BGRA_8888, CEF_ALPHA_TYPE_PREMULTIPLIED, nWidth, nHeight);
    if (cefImageData == nullptr) {
        return nullptr;
    }
    size_t nDataSize = cefImageData->GetSize();
    if (nDataSize == 0) {
        return nullptr;
    }
    ASSERT((int32_t)nDataSize == nHeight * nWidth * sizeof(uint32_t));
    if ((int32_t)nDataSize != nHeight * nWidth * sizeof(uint32_t)) {
        return nullptr;
    }

    std::vector<uint8_t> imageData;
    imageData.resize(nDataSize);
    nDataSize = cefImageData->GetData(imageData.data(), imageData.size(), 0);
    ASSERT(nDataSize == imageData.size());
    if (nDataSize != imageData.size()) {
        return nullptr;
    }

    // 2. Create the color bitmap
    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = nWidth;
    bmi.bmiHeader.biHeight = -nHeight;  // Top-to-bottom layout
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBitsColor = nullptr;
    HBITMAP hBmpColor = ::CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS, &pBitsColor, nullptr, 0);
    memcpy(pBitsColor, imageData.data(), nWidth * nHeight * 4);  // Copy the ARGB data

    // 3. Create the mask bitmap (this can be optimized to real mask generation logic)
    HBITMAP hBmpMask = ::CreateBitmap(nWidth, nHeight, 1, 1, nullptr);

    // 4. Generate the HICON
    ICONINFO iconInfo = { 0 };
    iconInfo.fIcon = TRUE;
    iconInfo.hbmColor = hBmpColor;
    iconInfo.hbmMask = hBmpMask;
    HICON hIcon = ::CreateIconIndirect(&iconInfo);

    // 5. Release the temporary resources
    ::DeleteObject(hBmpColor);
    ::DeleteObject(hBmpMask);

    return hIcon;
}

#endif // defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
