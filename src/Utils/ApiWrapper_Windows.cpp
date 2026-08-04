#include "dui/Utils/ApiWrapper_Windows.h"

#ifdef DUI_BUILD_FOR_WIN

#include <VersionHelpers.h>
#include <map>
#include "dui/Core/GlobalManager.h"
#include "dui/Render/IRender.h"

namespace ui
{

UINT GetDpiForWnd(HWND hWnd)
{
    if (!::IsWindow(hWnd)) {
        return 0;
    }
    uint32_t uDPI = 0;
    if ((uDPI == 0) && ::IsWindows8OrGreater()) {
        HMONITOR hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        if (hMonitor != nullptr) {
            uint32_t dpix = 96;
            uint32_t dpiy = 96;
            if (GetDpiForMonitorWrapper(hMonitor, MDT_EFFECTIVE_DPI, &dpix, &dpiy)) {
                uDPI = dpix;
            }
        }
    }
    if (uDPI == 0) {
        HDC hDC = ::GetDC(hWnd);
        if (hDC != nullptr) {
            uDPI = (uint32_t)::GetDeviceCaps(hDC, LOGPIXELSX);
            ::ReleaseDC(hWnd, hDC);
        }
    }
    if ((uDPI == 0) && ::IsWindows10OrGreater()) {
        if (!GetDpiForWindowWrapper(hWnd, uDPI)) {
            uDPI = 0;
        }
    }
    return uDPI;
}

bool GetDpiForSystemWrapper(UINT& dpi)
{
    typedef UINT(WINAPI* GetDpiForSystemPtr)();
    static GetDpiForSystemPtr get_dpi_for_system_func = reinterpret_cast<GetDpiForSystemPtr>(GetProcAddress(GetModuleHandleA("user32.dll"), "GetDpiForSystem"));
    dpi = 96;
    if (get_dpi_for_system_func) {
        dpi = get_dpi_for_system_func();
        return true;
    }
    return false;
}

bool GetDpiForMonitorWrapper(HMONITOR hMonitor, MONITOR_DPI_TYPE dpiType, UINT *dpiX, UINT *dpiY)
{
    typedef HRESULT(WINAPI *GetDpiForMonitorPtr)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);

    static GetDpiForMonitorPtr get_dpi_for_monitor_func = reinterpret_cast<GetDpiForMonitorPtr>(GetProcAddress(GetModuleHandleA("user32.dll"), "GetDpiForMonitorInternal"));
    if (get_dpi_for_monitor_func) {
        if (get_dpi_for_monitor_func(hMonitor, dpiType, dpiX, dpiY) != S_OK) {
            return true;
        }
    }
    return false;
}

bool GetDpiForWindowWrapper(HWND hwnd, UINT& dpi)
{
    typedef UINT(WINAPI* GetDpiForWindowPtr)(HWND hwnd);
    static GetDpiForWindowPtr get_dpi_for_window_func = reinterpret_cast<GetDpiForWindowPtr>(GetProcAddress(GetModuleHandleA("user32.dll"), "GetDpiForWindow"));
    dpi = 96;
    if (get_dpi_for_window_func) {
        dpi = get_dpi_for_window_func(hwnd);
        return true;
    }
    return false;
}

int GetSystemMetricsForDpiWrapper(int nIndex, UINT dpi)
{
    int nMetrics = 0;
    typedef int(WINAPI* GetSystemMetricsForDpiPtr)(int nIndex, UINT dpi);
    static GetSystemMetricsForDpiPtr get_system_metrics_for_dpi_func = reinterpret_cast<GetSystemMetricsForDpiPtr>(GetProcAddress(GetModuleHandleA("user32.dll"), "GetSystemMetricsForDpi"));
    if (get_system_metrics_for_dpi_func) {
        nMetrics = get_system_metrics_for_dpi_func(nIndex, dpi);
    }
    else {
        nMetrics = ::GetSystemMetrics(nIndex);
    }
    return nMetrics;
}

bool SetProcessDpiAwarenessContextWrapper(PROCESS_DPI_AWARENESS_CONTEXT value)
{
    typedef    BOOL (WINAPI *SetProcessDpiAwarenessContextPtr)(PROCESS_DPI_AWARENESS_CONTEXT value);
    static SetProcessDpiAwarenessContextPtr set_process_dpi_awareness_context_func = reinterpret_cast<SetProcessDpiAwarenessContextPtr>(GetProcAddress(GetModuleHandleA("user32.dll"), "SetProcessDpiAwarenessContext"));
    bool isOk = false;
    if (set_process_dpi_awareness_context_func) {
        isOk = set_process_dpi_awareness_context_func(value) != FALSE;
    }
    return isOk;
}

bool AreDpiAwarenessContextsEqualWrapper(PROCESS_DPI_AWARENESS_CONTEXT dpiContextA, PROCESS_DPI_AWARENESS_CONTEXT dpiContextB)
{
    typedef BOOL (WINAPI *AreDpiAwarenessContextsEqualPtr)(PROCESS_DPI_AWARENESS_CONTEXT dpiContextA, PROCESS_DPI_AWARENESS_CONTEXT dpiContextB);
    static AreDpiAwarenessContextsEqualPtr are_process_dpi_awareness_context_equal_func = reinterpret_cast<AreDpiAwarenessContextsEqualPtr>(GetProcAddress(GetModuleHandleA("user32.dll"), "AreDpiAwarenessContextsEqual"));
    bool isOk = false;
    if (are_process_dpi_awareness_context_equal_func) {
        if (are_process_dpi_awareness_context_equal_func(dpiContextA, dpiContextB)) {
            isOk = true;
        }
    }
    return isOk;
}

bool GetProcessDpiAwarenessContextWrapper(PROCESS_DPI_AWARENESS_CONTEXT& value)
{
    typedef PROCESS_DPI_AWARENESS_CONTEXT(WINAPI *GetDpiAwarenessContextForProcessPtr)(HANDLE hProcess);
    static GetDpiAwarenessContextForProcessPtr get_process_dpi_awareness_context_func = reinterpret_cast<GetDpiAwarenessContextForProcessPtr>(GetProcAddress(GetModuleHandleA("user32.dll"), "GetDpiAwarenessContextForProcess"));
    bool isOk = false;
    if (get_process_dpi_awareness_context_func) {
        value = get_process_dpi_awareness_context_func(nullptr);
        isOk = true;
    }
    return isOk;
}

bool SetProcessDPIAwarenessWrapper(PROCESS_DPI_AWARENESS value)
{
    typedef BOOL(WINAPI *SetProcessDpiAwarenessPtr)(PROCESS_DPI_AWARENESS);
    static SetProcessDpiAwarenessPtr set_process_dpi_awareness_func = reinterpret_cast<SetProcessDpiAwarenessPtr>(GetProcAddress(GetModuleHandleA("user32.dll"), "SetProcessDpiAwarenessInternal"));
    if (set_process_dpi_awareness_func) {
        if (set_process_dpi_awareness_func(value)) {
            return true;
        }
    }
    return false;
}

bool GetProcessDPIAwarenessWrapper(PROCESS_DPI_AWARENESS& awareness)
{
    typedef BOOL (WINAPI* GetProcessDpiAwarenessPtr)(HANDLE, PROCESS_DPI_AWARENESS*);
    static GetProcessDpiAwarenessPtr get_process_dpi_awareness_func = reinterpret_cast<GetProcessDpiAwarenessPtr>(GetProcAddress(GetModuleHandleA("user32.dll"), "GetProcessDpiAwarenessInternal"));
    if (get_process_dpi_awareness_func) {
        if (get_process_dpi_awareness_func(nullptr, &awareness)) {
            return true;
        }
    }
    return false;
}

bool SetProcessDPIAwareWrapper()
{
    typedef BOOL(WINAPI *SetProcessDPIAwarePtr)(VOID);
    static SetProcessDPIAwarePtr set_process_dpi_aware_func = reinterpret_cast<SetProcessDPIAwarePtr>(GetProcAddress(GetModuleHandleA("user32.dll"), "SetProcessDPIAware"));
    return set_process_dpi_aware_func && set_process_dpi_aware_func();
}

bool IsProcessDPIAwareWrapper(bool& bAware)
{
    typedef BOOL(WINAPI* IsProcessDPIAwarePtr)();
    static IsProcessDPIAwarePtr is_process_dpi_aware_func = reinterpret_cast<IsProcessDPIAwarePtr>(GetProcAddress(GetModuleHandleA("user32.dll"), "IsProcessDPIAware"));
    if (is_process_dpi_aware_func) {
        bAware = is_process_dpi_aware_func() != FALSE;
        return true;
    }
    return false;
}

bool RegisterTouchWindowWrapper(HWND hwnd, ULONG ulFlags)
{
    typedef BOOL(WINAPI *RegisterTouchWindowPtr)(HWND, ULONG);

    static RegisterTouchWindowPtr register_touch_window_func = reinterpret_cast<RegisterTouchWindowPtr>(GetProcAddress(GetModuleHandleA("user32.dll"), "RegisterTouchWindow"));

    if (register_touch_window_func) {
        return (TRUE == register_touch_window_func(hwnd, ulFlags));
    }

    return false;
}


bool UnregisterTouchWindowWrapper(HWND hwnd)
{
    typedef BOOL(WINAPI *UnregisterTouchWindowPtr)(HWND);

    static UnregisterTouchWindowPtr unregister_touch_window_func = reinterpret_cast<UnregisterTouchWindowPtr>(GetProcAddress(GetModuleHandleA("user32.dll"), "UnregisterTouchWindow"));

    if (unregister_touch_window_func) {
        return (TRUE == unregister_touch_window_func(hwnd));
    }

    return false;
}

bool GetTouchInputInfoWrapper(HTOUCHINPUT hTouchInput, UINT cInputs, PTOUCHINPUT pInputs, int cbSize)
{
    typedef BOOL(WINAPI *GetTouchInputInfoPtr)(HTOUCHINPUT, UINT, PTOUCHINPUT, int);

    static GetTouchInputInfoPtr get_touch_input_info_func = reinterpret_cast<GetTouchInputInfoPtr>(GetProcAddress(GetModuleHandleA("user32.dll"), "GetTouchInputInfo"));
    
    if (get_touch_input_info_func) {
        return (TRUE == get_touch_input_info_func(hTouchInput, cInputs, pInputs, cbSize));
    }

    return false;
}


bool CloseTouchInputHandleWrapper(HTOUCHINPUT hTouchInput)
{
    typedef BOOL(WINAPI *CloseTouchInputHandlePtr)(HTOUCHINPUT);

    static CloseTouchInputHandlePtr close_touch_input_handle_func = reinterpret_cast<CloseTouchInputHandlePtr>(GetProcAddress(GetModuleHandleA("user32.dll"), "CloseTouchInputHandle"));

    if (close_touch_input_handle_func) {
        return (TRUE == close_touch_input_handle_func(hTouchInput));
    }

    return false;
}

bool GetPointerTypeWrapper(UINT32 pointerId, POINTER_INPUT_TYPE *pointerType)
{
    typedef BOOL(WINAPI* GetPointerTypePtr)(UINT32 pointerId, POINTER_INPUT_TYPE *pointerType);
    static GetPointerTypePtr get_pointer_type = reinterpret_cast<GetPointerTypePtr>(GetProcAddress(GetModuleHandleA("user32.dll"), "GetPointerType"));

    if (get_pointer_type) {
        return (TRUE == get_pointer_type(pointerId, pointerType));
    }

    return false;
}

bool GetPointerInfoWrapper(UINT32 pointerId, POINTER_INFO *pointerInfo)
{
    typedef BOOL(WINAPI* GetPointerInfoPtr)(UINT32 pointerId, POINTER_INFO *pointerInfo);
    static GetPointerInfoPtr get_pointer_info = reinterpret_cast<GetPointerInfoPtr>(GetProcAddress(GetModuleHandleA("user32.dll"), "GetPointerInfo"));

    if (get_pointer_info) {
        return (TRUE == get_pointer_info(pointerId, pointerInfo));
    }

    return false;
}

bool GetPointerTouchInfoWrapper(UINT32 pointerId, POINTER_TOUCH_INFO *touchInfo)
{
    typedef BOOL(WINAPI* GetPointerTouchInfoPtr)(UINT32 pointerId, POINTER_TOUCH_INFO *touchInfo);
    static GetPointerTouchInfoPtr get_pointer_touch_info = reinterpret_cast<GetPointerTouchInfoPtr>(GetProcAddress(GetModuleHandleA("user32.dll"), "GetPointerTouchInfo"));

    if (get_pointer_touch_info) {
        return (TRUE == get_pointer_touch_info(pointerId, touchInfo));
    }

    return false;
}

bool GetPointerPenInfoWrapper(UINT32 pointerId, POINTER_PEN_INFO *penInfo)
{
    typedef BOOL(WINAPI* GetPointerPenInfoPtr)(UINT32 pointerId, POINTER_PEN_INFO *penInfo);
    static GetPointerPenInfoPtr get_pointer_pen_info = reinterpret_cast<GetPointerPenInfoPtr>(GetProcAddress(GetModuleHandleA("user32.dll"), "GetPointerPenInfo"));

    if (get_pointer_pen_info) {
        return (TRUE == get_pointer_pen_info(pointerId, penInfo));
    }

    return false;
}

bool EnableMouseInPointerWrapper(BOOL fEnable)
{
    typedef BOOL(WINAPI* EnableMouseInPointerPtr)(BOOL);
    static EnableMouseInPointerPtr enable_mouse_in_pointer = reinterpret_cast<EnableMouseInPointerPtr>(GetProcAddress(GetModuleHandleA("user32.dll"), "EnableMouseInPointer"));

    if (enable_mouse_in_pointer) {
        return (TRUE == enable_mouse_in_pointer(fEnable));
    }

    return false;
}

// Detect whether "Show window contents while dragging" is enabled - dynamically loads the registry API version
bool IsDragWindowContentsEnabled()
{
    typedef LONG(WINAPI* PFUNC_RegOpenKeyExW)(
        HKEY hKey,
        LPCWSTR lpSubKey,
        DWORD ulOptions,
        REGSAM samDesired,
        PHKEY phkResult
        );

    typedef LONG(WINAPI* PFUNC_RegQueryValueExW)(
        HKEY hKey,
        LPCWSTR lpValueName,
        LPDWORD lpReserved,
        LPDWORD lpType,
        LPBYTE lpData,
        LPDWORD lpcbData
        );

    typedef LONG(WINAPI* PFUNC_RegCloseKey)(
        HKEY hKey
        );

    HMODULE hModAdvapi32 = LoadLibraryW(L"Advapi32.dll");
    if (NULL == hModAdvapi32) {
        return false;
    }

    // Get the function addresses of the registry APIs
    PFUNC_RegOpenKeyExW pfnRegOpenKeyExW = (PFUNC_RegOpenKeyExW)GetProcAddress(hModAdvapi32, "RegOpenKeyExW");
    PFUNC_RegQueryValueExW pfnRegQueryValueExW = (PFUNC_RegQueryValueExW)GetProcAddress(hModAdvapi32, "RegQueryValueExW");
    PFUNC_RegCloseKey pfnRegCloseKey = (PFUNC_RegCloseKey)GetProcAddress(hModAdvapi32, "RegCloseKey");

    if (NULL == pfnRegOpenKeyExW || NULL == pfnRegQueryValueExW || NULL == pfnRegCloseKey) {
        FreeLibrary(hModAdvapi32);
        return false;
    }

    HKEY hKey = NULL;
    LONG lResult = ERROR_SUCCESS;
    DWORD dwValueType = 0;
    wchar_t szValueBuffer[8] = { 0 };
    DWORD dwValueBuffer = 0;
    DWORD dwBufferSize = 0;

    // Open the registry key
    lResult = pfnRegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Control Panel\\Desktop",
        0,
        KEY_READ,
        &hKey
    );

    if (lResult != ERROR_SUCCESS) {
        if (hKey) {
            pfnRegCloseKey(hKey);
        }
        FreeLibrary(hModAdvapi32);      // Release the DLL handle
        return false;
    }

    bool bEnabled = false;
    // First try reading it as a string type (the mainstream scenario on Win10/11)
    dwBufferSize = sizeof(szValueBuffer);
    lResult = pfnRegQueryValueExW(
        hKey,
        L"DragFullWindows",
        NULL,
        &dwValueType,
        (LPBYTE)szValueBuffer,
        &dwBufferSize
    );

    if (lResult == ERROR_SUCCESS) {
        if (dwValueType == REG_SZ) {
            bEnabled = (wcscmp(szValueBuffer, L"1") == 0);
        }
        else if (dwValueType == REG_DWORD) {
            dwValueBuffer = *(DWORD*)szValueBuffer;
            bEnabled = (dwValueBuffer == 1);
        }
    }
    else {
        // The string read failed, try reading as a DWORD type (compatible with Win7)
        dwBufferSize = sizeof(dwValueBuffer);
        lResult = pfnRegQueryValueExW(
            hKey,
            L"DragFullWindows",
            NULL,
            NULL,
            (LPBYTE)&dwValueBuffer,
            &dwBufferSize
        );
        if (lResult == ERROR_SUCCESS) {
            bEnabled = (dwValueBuffer == 1);
        }
    }

    // Close the registry key handle
    pfnRegCloseKey(hKey);

    //Release the loaded DLL
    FreeLibrary(hModAdvapi32);
    return bEnabled;
}


// Helper function: convert width and height to a unique key (handles the case where 0 means 256px)
static DWORD GetIconSizeKey(BYTE bWidth, BYTE bHeight)
{
    // Store the width in the high bits and the height in the low bits; 0 becomes 256
    UINT width = (bWidth == 0) ? 256 : bWidth;
    UINT height = (bHeight == 0) ? 256 : bHeight;
    return (width << 16) | height;
}

/** Find the location of the icon resource of the specified size in the icon data
* @param [in] pIconData The starting address of the ICO icon data (corresponding to the data of a *.ico file)
* @param [in] nDataSize The length of the icon data
* @param [in] targetWidth The width of the target icon
* @param [in] targetHeight The height of the target icon
* @param [out] outResSize The length of the returned icon data
* @return The starting address of the icon resource data
*/
static const BYTE* ExtractIconResource(const BYTE* pIconData, DWORD nDataSize,
                                       int32_t targetWidth, int32_t targetHeight, DWORD& outResSize)
{
#pragma pack(push, 1)
    typedef struct
    {
        WORD idReserved;   // Reserved field, must be 0
        WORD idType;       // Resource type: 1=icon, 2=cursor
        WORD idCount;      // Number of icons/cursors
    } ICONDIR;

    typedef struct
    {
        BYTE bWidth;       // Icon width (0 means 256px)
        BYTE bHeight;      // Icon height (0 means 256px)
        BYTE bColorCount;  // Number of colors (0 means >=8bpp)
        BYTE bReserved;    // Reserved field, must be 0
        WORD wPlanes;      // Number of planes (fixed at 1 for icons)
        WORD wBitCount;    // Bits per pixel
        DWORD dwBytesInRes;// The byte size of this icon resource
        DWORD dwImageOffset;// The offset of this icon resource in the file
    } ICONDIRENTRY;
#pragma pack(pop)

    outResSize = 0;
    if ((pIconData == nullptr) || (nDataSize == 0)) {
        return nullptr;
    }

    // Parse the ICO file header
    const ICONDIR* pIconDir = (const ICONDIR*)pIconData;
    if ((pIconDir->idReserved != 0 || pIconDir->idType != 1 || pIconDir->idCount == 0)) {
        return nullptr;
    }

    // Iterate over all icon entries and do a preliminary filter: for icons of the same size, keep only the one with the largest bit depth (wBitCount)
    const ICONDIRENTRY* pFirstEntry = (const ICONDIRENTRY*)(pIconData + sizeof(ICONDIR));
    // Group with a map: key = combined key of width + height, value = all icon entries of that size
    std::map<DWORD, std::vector<ICONDIRENTRY>> iconGroups;
    for (UINT i = 0; i < pIconDir->idCount; i++) {
        const ICONDIRENTRY* pEntry = &pFirstEntry[i];
        DWORD sizeKey = GetIconSizeKey(pEntry->bWidth, pEntry->bHeight);
        iconGroups[sizeKey].push_back(*pEntry);
    }
    // Iterate over each group, keep the icon with the largest bit depth, discard the other icon data, and sort the icons by size from smallest to largest
    std::vector<ICONDIRENTRY> allIconList;
    for (auto& group : iconGroups) {
        auto& entries = group.second;
        // Find the entry with the largest wBitCount in this group
        auto maxEntryIt = std::max_element(entries.begin(), entries.end(),
            [](const ICONDIRENTRY& a, const ICONDIRENTRY& b) {
                return a.wBitCount < b.wBitCount;
            });

        // Add the entry with the largest bit depth to the result container
        if (maxEntryIt != entries.end()) {
            allIconList.push_back(*maxEntryIt);
        }
    }
    if (allIconList.empty()) {
        return nullptr;
    }

    // Select the icon that best matches
    ICONDIRENTRY bestEntry = allIconList.back(); //by default, select the icon with the largest size
    for (size_t nIndex = 0; nIndex < allIconList.size(); ++nIndex) {
        const ICONDIRENTRY& entry = allIconList[nIndex];
        int entryWidth = (entry.bWidth == 0) ? 256 : entry.bWidth;
        int entryHeight = (entry.bHeight == 0) ? 256 : entry.bHeight;
        bool isSizeQualified = (entryWidth >= targetWidth) && (entryHeight >= targetHeight);
        if (!isSizeQualified) {
            continue;
        }
        //The size meets the requirement
        if ((entryWidth == targetWidth) && (entryHeight == targetHeight)) {
            //The size exactly meets the requirement: select it directly
            bestEntry = allIconList[nIndex];
        }
        else if (nIndex == 0) {
            //The first icon meets the requirement: select it directly
            bestEntry = allIconList[nIndex];
        }
        else {
            //A non-first icon meets the requirement: compare which one is more suitable (less distortion when scaled)
            const ICONDIRENTRY& preEntry = allIconList[nIndex - 1];
            int preEntryWidth = (preEntry.bWidth == 0) ? 256 : preEntry.bWidth;
            int preEntryHeight = (preEntry.bHeight == 0) ? 256 : preEntry.bHeight;
            float wRatio = (float)(targetWidth - preEntryWidth) / (float)preEntryWidth;
            float hRatio = (float)(targetHeight - preEntryHeight) / (float)preEntryHeight;
            float preRatio = std::max(wRatio, hRatio);

            wRatio = (float)(entryWidth - targetWidth) / (float)entryWidth;
            hRatio = (float)(entryHeight - targetHeight) / (float)entryHeight;
            float curRatio = std::max(wRatio, hRatio);
            if (curRatio < preRatio) {
                bestEntry = allIconList[nIndex];            //select the one with the larger size
            }
            else {
                const float minRatio = 0.20f; //set the minimum upscale ratio
                if (preRatio < minRatio) {
                    bestEntry = allIconList[nIndex - 1];    //select the one with the smaller size
                }
                else {
                    bestEntry = allIconList[nIndex];        //select the one with the larger size
                }
            }
        }
        break;
    }

    // Validate the resource offset and size
    if ((bestEntry.dwImageOffset + bestEntry.dwBytesInRes) > nDataSize) {
        return nullptr;
    }
    // Output the result
    outResSize = bestEntry.dwBytesInRes;
    if (outResSize == 0) {
        return nullptr;
    }
    return pIconData + bestEntry.dwImageOffset;
}

/** Supports the ICO format
*/
static bool CreateIconsFromIcoData(const std::vector<uint8_t>& iconFileData, uint32_t uDpiScaleFactor,
                                   HICON* hSmallIcon, HICON* hBigIcon)
{
    if (iconFileData.empty()) {
        return false;
    }
    if ((hSmallIcon == nullptr) && (hBigIcon == nullptr)) {
        return false;
    }
    if (hSmallIcon != nullptr) {
        *hSmallIcon = nullptr;
    }
    if (hBigIcon != nullptr) {
        *hBigIcon = nullptr;
    }
    //Little Endian Only
    int16_t test = 1;
    bool bLittleEndianHost = (*((char*)&test) == 1);
    ASSERT_UNUSED_VARIABLE(bLittleEndianHost);

    bool bValidIcoFile = false;
    std::vector<uint8_t> fileData = iconFileData;
    fileData.resize(fileData.size() + 1024); //pad with blank space
    typedef struct tagIconDir {
        uint16_t idReserved;
        uint16_t idType;
        uint16_t idCount;
    } ICONHEADER;
    typedef struct tagIconDirectoryEntry {
        uint8_t  bWidth;
        uint8_t  bHeight;
        uint8_t  bColorCount;
        uint8_t  bReserved;
        uint16_t  wPlanes;
        uint16_t  wBitCount;
        uint32_t dwBytesInRes;
        uint32_t dwImageOffset;
    } ICONDIRENTRY;

    ICONHEADER* icon_header = (ICONHEADER*)fileData.data();
    if ((icon_header->idReserved == 0) && (icon_header->idType == 1)) {
        bValidIcoFile = true;
        for (int32_t c = 0; c < icon_header->idCount; ++c) {
            size_t nDataOffset = sizeof(ICONHEADER) + sizeof(ICONDIRENTRY) * c;
            if (nDataOffset >= fileData.size()) {
                bValidIcoFile = false;
                break;
            }
            ICONDIRENTRY* pIconDir = (ICONDIRENTRY*)((uint8_t*)fileData.data() + nDataOffset);
            if (pIconDir->dwImageOffset >= iconFileData.size()) {
                bValidIcoFile = false;
                break;
            }
            else if ((pIconDir->dwImageOffset + pIconDir->dwBytesInRes) > iconFileData.size()) {
                bValidIcoFile = false;
                break;
            }
        }
    }
    //ASSERT(bValidIcoFile);
    if (!bValidIcoFile) {
        return false;
    }

    if (uDpiScaleFactor == 0) {
        uDpiScaleFactor = 100;
    }
    uint32_t uDpi = DpiManager::MulDiv(uDpiScaleFactor, 96u, 100u);
    struct TWinIconInfo
    {
        BOOL bLargeIcon;
        int32_t cxIcon;
        int32_t cyIcon;
    };
    std::vector<TWinIconInfo> iconInfos;

    //Large icon
    if (hBigIcon != nullptr) {
        int32_t cxBestIcon = GetSystemMetricsForDpiWrapper(SM_CXICON, uDpi);
        int32_t cyBestIcon = GetSystemMetricsForDpiWrapper(SM_CYICON, uDpi);
        iconInfos.push_back({ TRUE, cxBestIcon, cyBestIcon });
    }

    //Small icon
    if (hSmallIcon != nullptr) {
        int32_t cxBestIcon = GetSystemMetricsForDpiWrapper(SM_CXSMICON, uDpi);
        int32_t cyBestIcon = GetSystemMetricsForDpiWrapper(SM_CYSMICON, uDpi);
        iconInfos.push_back({ FALSE, cxBestIcon, cyBestIcon });
    }

    for (const TWinIconInfo& iconInfo : iconInfos) {
        DWORD nIconDataSize = 0;
        const BYTE* pIconData = ExtractIconResource((const BYTE*)fileData.data(), (DWORD)fileData.size(), iconInfo.cxIcon, iconInfo.cyIcon, nIconDataSize);
        if (pIconData == nullptr) {
            int32_t offset = ::LookupIconIdFromDirectoryEx((PBYTE)fileData.data(), TRUE, iconInfo.cxIcon, iconInfo.cyIcon, LR_DEFAULTCOLOR | LR_SHARED);
            if (offset > 0) {
                pIconData = (PBYTE)fileData.data() + offset;
                nIconDataSize = (DWORD)fileData.size() - (DWORD)offset;
            }
        }
        if (pIconData != nullptr) {
            HICON hIcon = ::CreateIconFromResourceEx((PBYTE)pIconData, nIconDataSize, TRUE, 0x00030000, iconInfo.cxIcon, iconInfo.cyIcon, LR_DEFAULTCOLOR | LR_SHARED);
            ASSERT(hIcon != nullptr);
            if (hIcon != nullptr) {
                if (iconInfo.bLargeIcon) {
                    //Large icon
                    ASSERT(hBigIcon != nullptr);
                    if (hBigIcon != nullptr) {
                        *hBigIcon = hIcon;
                    }
                }
                else {
                    //Small icon
                    ASSERT(hSmallIcon != nullptr);
                    if (hSmallIcon != nullptr) {
                        *hSmallIcon = hIcon;
                    }
                }
            }
        }
    }
    bool bRet = true;
    if (hSmallIcon != nullptr) {
        if (*hSmallIcon == nullptr) {
            bRet = false;
        }
    }
    if (hBigIcon != nullptr) {
        if (*hBigIcon == nullptr) {
            bRet = false;
        }
    }
    if (!bRet) {
        if (hSmallIcon != nullptr) {
            if (*hSmallIcon != nullptr) {
                ::DestroyIcon(*hSmallIcon);
            }
            *hSmallIcon = nullptr;
        }
        if (hBigIcon != nullptr) {
            if (*hBigIcon != nullptr) {
                ::DestroyIcon(*hBigIcon);
            }
            *hBigIcon = nullptr;
        }
    }
    return bRet;
}

/** Supports all image formats
*/
static bool CreateIconsFromImageData(const std::vector<uint8_t>& iconFileData,
                                     const FilePath& imageFilePath,
                                     uint32_t uDpiScaleFactor,                                     
                                     HICON* hSmallIcon, HICON* hBigIcon)
{
    if (iconFileData.empty()) {
        return false;
    }
    if ((hSmallIcon == nullptr) && (hBigIcon == nullptr)) {
        return false;
    }
    if (hSmallIcon != nullptr) {
        *hSmallIcon = nullptr;
    }
    if (hBigIcon != nullptr) {
        *hBigIcon = nullptr;
    }

    if (uDpiScaleFactor == 0) {
        uDpiScaleFactor = 100;
    }
    uint32_t uDpi = DpiManager::MulDiv(uDpiScaleFactor, 96u, 100u);
    struct TWinIconInfo
    {
        BOOL bLargeIcon;
        int32_t cxIcon;
        int32_t cyIcon;
    };
    std::vector<TWinIconInfo> iconInfos;

    //Large icon
    if (hBigIcon != nullptr) {
        int32_t cxBestIcon = GetSystemMetricsForDpiWrapper(SM_CXICON, uDpi);
        int32_t cyBestIcon = GetSystemMetricsForDpiWrapper(SM_CYICON, uDpi);
        iconInfos.push_back({ TRUE, cxBestIcon, cyBestIcon });
    }

    //Small icon
    if (hSmallIcon != nullptr) {
        int32_t cxBestIcon = GetSystemMetricsForDpiWrapper(SM_CXSMICON, uDpi);
        int32_t cyBestIcon = GetSystemMetricsForDpiWrapper(SM_CYSMICON, uDpi);
        iconInfos.push_back({ FALSE, cxBestIcon, cyBestIcon });
    }

    for (const TWinIconInfo& winIconInfo : iconInfos) {
        //Load from the image data
        ImageDecoderFactory& imageDecoders = GlobalManager::Instance().ImageDecoders();
        float fImageSizeScale = uDpiScaleFactor / 100.0f;
        ImageDecodeParam decodeParam;
        decodeParam.m_imageFilePath = imageFilePath;
        decodeParam.m_fImageSizeScale = fImageSizeScale;
        decodeParam.m_pFileData = std::make_shared<std::vector<uint8_t>>(iconFileData);
        decodeParam.m_rcMaxDestRectSize = UiSize(winIconInfo.cxIcon, winIconInfo.cyIcon);
        std::shared_ptr<IBitmap> pBitmap = imageDecoders.DecodeImageData(decodeParam);
        if (pBitmap == nullptr) {
            continue;
        }
        int32_t nWidth = (int32_t)pBitmap->GetWidth();
        int32_t nHeight = (int32_t)pBitmap->GetHeight();
        if ((nWidth < 1) || (nHeight < 1)) {
            continue;
        }

        void* pPixelBits = pBitmap->LockPixelBits();
        ASSERT(pPixelBits != nullptr);
        if (pPixelBits == nullptr) {
            continue;
        }

        //Create the icon
        BITMAPINFO bmpInfo;
        memset(&bmpInfo, 0, sizeof(BITMAPINFO));
        bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmpInfo.bmiHeader.biWidth = nWidth;
        bmpInfo.bmiHeader.biHeight = -nHeight; /* Top-down bitmap */
        bmpInfo.bmiHeader.biPlanes = 1;
        bmpInfo.bmiHeader.biBitCount = 32;
        bmpInfo.bmiHeader.biCompression = BI_RGB;

        HDC hdc = ::GetDC(NULL);
        void* pBits = NULL;
        HBITMAP hBitmap = ::CreateDIBSection(hdc, &bmpInfo, DIB_RGB_COLORS, &pBits, NULL, 0);
        if (hBitmap == nullptr) {
            ::ReleaseDC(NULL, hdc);
            continue;
        }
        memcpy(pBits, pPixelBits, nWidth * nHeight * 4);
        HBITMAP hMask = ::CreateBitmap(nWidth, nHeight, 1, 1, NULL);
        if (hMask == nullptr) {
            ::DeleteObject(hBitmap);
            ::ReleaseDC(NULL, hdc);
            continue;
        }

        HDC hdcMem = ::CreateCompatibleDC(hdc);
        HGDIOBJ oldBitmap = ::SelectObject(hdcMem, hMask);

        for (int y = 0; y < nHeight; y++) {
            for (int x = 0; x < nWidth; x++) {
                BYTE* pixel = (BYTE*)pBits + (y * nWidth + x) * 4;
                BYTE alpha = pixel[3];
                COLORREF maskColor = (alpha == 0) ? RGB(0, 0, 0) : RGB(255, 255, 255);
                ::SetPixel(hdcMem, x, y, maskColor);
            }
        }

        ICONINFO iconInfo;
        iconInfo.fIcon = TRUE;
        iconInfo.xHotspot = 0;
        iconInfo.yHotspot = 0;
        iconInfo.hbmMask = hMask;
        iconInfo.hbmColor = hBitmap;

        HICON hIcon = ::CreateIconIndirect(&iconInfo);
        ASSERT(hIcon != nullptr);
        if (hIcon != nullptr) {
            if (winIconInfo.bLargeIcon) {
                //Large icon
                ASSERT(hBigIcon != nullptr);
                if (hBigIcon != nullptr) {
                    *hBigIcon = hIcon;
                }
            }
            else {
                //Small icon
                ASSERT(hSmallIcon != nullptr);
                if (hSmallIcon != nullptr) {
                    *hSmallIcon = hIcon;
                }
            }
        }

        ::SelectObject(hdcMem, oldBitmap);
        ::DeleteDC(hdcMem);
        ::DeleteObject(hBitmap);
        ::DeleteObject(hMask);
        ::ReleaseDC(NULL, hdc);
    }

    bool bRet = true;
    if (hSmallIcon != nullptr) {
        if (*hSmallIcon == nullptr) {
            bRet = false;
        }
    }
    if (hBigIcon != nullptr) {
        if (*hBigIcon == nullptr) {
            bRet = false;
        }
    }
    if (!bRet) {
        if (hSmallIcon != nullptr) {
            if (*hSmallIcon != nullptr) {
                ::DestroyIcon(*hSmallIcon);
            }
            *hSmallIcon = nullptr;
        }
        if (hBigIcon != nullptr) {
            if (*hBigIcon != nullptr) {
                ::DestroyIcon(*hBigIcon);
            }
            *hBigIcon = nullptr;
        }
    }
    return bRet;
}

bool CreateIconsFromData(const std::vector<uint8_t>& iconFileData,
                         const DString& imageFilePath,
                         uint32_t uDpiScaleFactor,
                         HICON* hSmallIcon, HICON* hBigIcon)
{
    if (CreateIconsFromIcoData(iconFileData, uDpiScaleFactor, hSmallIcon, hBigIcon)) {
        return true;
    }
    return CreateIconsFromImageData(iconFileData, FilePath(imageFilePath), uDpiScaleFactor, hSmallIcon, hBigIcon);
}

} //namespace ui

#endif //DUI_BUILD_FOR_WIN
