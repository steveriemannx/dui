#ifndef UI_UTILS_APIWRAPPER_WINDOWS_H_
#define UI_UTILS_APIWRAPPER_WINDOWS_H_

#include "duilib/Utils/Macros_Windows.h"

#ifdef DUILIB_BUILD_FOR_WIN

namespace ui
{
    // Get the DPI value of the specified window; returns 0 if it fails
    UINT GetDpiForWnd(HWND hWnd);

    // DPI adaptation
    //Windows 10 version 1607 (after the process starts, if the system DPI is changed, this API still returns the old value)
    bool GetDpiForSystemWrapper(UINT& dpi);
    //Windows 10 version 1607 (after the process starts, if the system DPI is changed, this API still returns the old value)
    bool GetDpiForWindowWrapper(HWND hwnd, UINT& dpi);
    //Windows 10 version 1607
    int GetSystemMetricsForDpiWrapper(int nIndex, UINT dpi);

    //Windows 8.1 and later
    bool GetDpiForMonitorWrapper(HMONITOR hMonitor, MONITOR_DPI_TYPE dpiType, UINT *dpiX, UINT *dpiY);

    //Windows 10 version 1703 and above
    bool SetProcessDpiAwarenessContextWrapper(PROCESS_DPI_AWARENESS_CONTEXT value);
    bool GetProcessDpiAwarenessContextWrapper(PROCESS_DPI_AWARENESS_CONTEXT& value);
    bool AreDpiAwarenessContextsEqualWrapper(PROCESS_DPI_AWARENESS_CONTEXT dpiContextA, PROCESS_DPI_AWARENESS_CONTEXT dpiContextB);
    
    //Windows 8.1 and above
    bool SetProcessDPIAwarenessWrapper(PROCESS_DPI_AWARENESS value);
    bool GetProcessDPIAwarenessWrapper(PROCESS_DPI_AWARENESS& awareness);

    //Windows Vista and above
    bool SetProcessDPIAwareWrapper();
    bool IsProcessDPIAwareWrapper(bool& bAware);

    // WM_TOUCH
    bool RegisterTouchWindowWrapper(HWND hwnd, ULONG ulFlags);
    bool UnregisterTouchWindowWrapper(HWND hwnd);

    bool GetTouchInputInfoWrapper(HTOUCHINPUT hTouchInput, UINT cInputs, PTOUCHINPUT pInputs, int cbSize);
    bool CloseTouchInputHandleWrapper(HTOUCHINPUT hTouchInput); 

    // WM_POINTER
    bool GetPointerTypeWrapper(UINT32 pointerId, POINTER_INPUT_TYPE *pointerType);
    bool GetPointerInfoWrapper(UINT32 pointerId, POINTER_INFO *pointerInfo);
    bool GetPointerTouchInfoWrapper(UINT32 pointerId, POINTER_TOUCH_INFO *touchInfo);
    bool GetPointerPenInfoWrapper(UINT32 pointerId, POINTER_PEN_INFO *penInfo);
    bool EnableMouseInPointerWrapper(BOOL fEnable);

    // Detect whether "Show window contents while dragging" is enabled
    bool IsDragWindowContentsEnabled();

    /** Create two icons (a large icon and a small icon) from icon file data; the icon handles are released by the caller
    * @param [in] iconFileData The icon data, which can be in any supported image format, though ICO format is recommended for best results
    * @param [in] imageFilePath The path of the image, used to determine the image type
    * @param [in] uDpiScaleFactor The current UI scale percentage factor (for example, 100 means a scale of 100%, i.e. no scaling)
    * @param [out] hSmallIcon The handle of the small icon; the icon handle is released by the caller
    * @param [out] hBigIcon The handle of the large icon; the icon handle is released by the caller
    */
    bool CreateIconsFromData(const std::vector<uint8_t>& iconFileData,
                             const DString& imageFilePath,
                             uint32_t uDpiScaleFactor,
                             HICON* hSmallIcon, HICON* hBigIcon);
}

#endif //DUILIB_BUILD_FOR_WIN

#endif //UI_UTILS_APIWRAPPER_WINDOWS_H_
