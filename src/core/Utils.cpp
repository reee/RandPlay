#include "Utils.h"
#include <filesystem>
#include <sstream>

namespace Utils {
    
    // Get the application data path - now returns current directory
    std::wstring GetAppDataPath() {
        // Always use current directory for settings
        return std::filesystem::current_path().wstring();
    }

    // Get the executable directory path
    std::wstring GetExecutablePath() {
        wchar_t exePath[MAX_PATH];
        GetModuleFileName(NULL, exePath, MAX_PATH);
        
        // Convert to filesystem path and get the parent directory
        std::filesystem::path path(exePath);
        return path.parent_path().wstring();
    }

    // Helper function to load string resources
    std::wstring LoadStringResource(UINT stringID) {
        wchar_t buffer[512];
        int len = LoadString(GetModuleHandle(NULL), stringID, buffer, 512);
        if (len > 0) {
            return std::wstring(buffer, len);
        }
        return L"";
    }

    // Enable visual styles and DPI awareness
    HRESULT EnableVisualStyles() {
        HRESULT hr = S_OK;
        
        // Enable visual styles
        hr = CoInitialize(NULL);
        if (SUCCEEDED(hr)) {
            ACTCTX actCtx = {0};
            actCtx.cbSize = sizeof(actCtx);
            actCtx.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID | ACTCTX_FLAG_HMODULE_VALID;
            actCtx.lpResourceName = MAKEINTRESOURCE(2);
            actCtx.hModule = GetModuleHandle(NULL);
            
            HANDLE hActCtx = CreateActCtx(&actCtx);
            if (hActCtx != INVALID_HANDLE_VALUE) {
                ULONG_PTR cookie;
                ActivateActCtx(hActCtx, &cookie);
            }
        }
        
        return hr;
    }

    // Create a better font for the UI that scales with DPI
    HFONT CreateUIFont() {
        int dpi = GetDPIForWindow(NULL);
        return CreateUIFontForDPI(dpi);
    }

    // Create a font for a specific DPI (used for DPI changes)
    HFONT CreateUIFontForDPI(int dpi) {
        int fontSize = -MulDiv(9, dpi, 72); // 9pt font

        // Use GB2312_CHARSET for better Chinese support
        // Try Microsoft YaHei UI first, fallback to Segoe UI
        return CreateFont(
            fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei UI"
        );
    }

    // Get DPI for a window (or primary monitor if hwnd is NULL)
    int GetDPIForWindow(HWND hwnd) {
        // Try to use GetDpiForWindow (Windows 10 1607+)
        typedef UINT (WINAPI *GetDpiForWindowFunc)(HWND);
        static GetDpiForWindowFunc pGetDpiForWindow = nullptr;

        if (!pGetDpiForWindow) {
            HMODULE hUser32 = GetModuleHandle(L"user32.dll");
            if (hUser32) {
                pGetDpiForWindow = (GetDpiForWindowFunc)GetProcAddress(hUser32, "GetDpiForWindow");
            }
        }

        if (pGetDpiForWindow && hwnd) {
            return pGetDpiForWindow(hwnd);
        }

        // Fallback to GetDeviceCaps
        HDC hdc = GetDC(NULL);
        int dpi = GetDeviceCaps(hdc, LOGPIXELSX);
        ReleaseDC(NULL, hdc);

        return dpi;
    }

    // Helper function to scale UI elements based on DPI
    int ScaleDPI(int value) {
        int dpi = GetDPIForWindow(NULL);
        return MulDiv(value, dpi, 96); // 96 is the default DPI
    }
}
