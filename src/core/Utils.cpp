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
        HDC hdc = GetDC(NULL);
        int dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
        ReleaseDC(NULL, hdc);
        
        int fontSize = -MulDiv(9, dpiY, 72); // 9pt font
        
        return CreateFont(
            fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
        );
    }

    // Helper function to scale UI elements based on DPI
    int ScaleDPI(int value) {
        HDC hdc = GetDC(NULL);
        int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
        ReleaseDC(NULL, hdc);
        
        return MulDiv(value, dpiX, 96); // 96 is the default DPI
    }
}
