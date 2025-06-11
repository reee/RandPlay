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
    int ScaleDPI(int value);
    HRESULT EnableVisualStyles();
}
