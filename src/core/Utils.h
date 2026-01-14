#pragma once

#include <windows.h>
#include <string>

namespace Utils {
    // Path utilities
    std::wstring GetAppDataPath();
    std::wstring GetExecutablePath();

    // UI utilities
    std::wstring LoadStringResource(UINT stringID);
    HFONT CreateUIFont();
    HFONT CreateUIFontForDPI(int dpi);
    int ScaleDPI(int value);
    int GetDPIForWindow(HWND hwnd);
    HRESULT EnableVisualStyles();
}
