#pragma once

#include <windows.h>
#include <string>
#include "../constants/AppConstants.h"

class LanguageManager {
public:
    // Initialize with auto-detection or saved preference
    static void Initialize();

    // Load language resource DLL
    static bool LoadLanguage(int language);

    // Get current language
    static int GetCurrentLanguage();

    // Detect system language
    static int DetectSystemLanguage();

    // Wrapper for LoadString - tries resource DLL first, then EXE
    static std::wstring LoadStringResource(UINT stringID);

    // Get the current resource DLL handle (for loading menus/dialogs)
    static HMODULE GetResourceDLL() { return s_hResourceDLL; }

private:
    static HMODULE s_hResourceDLL;
    static int s_currentLanguage;
    static bool s_initialized;
};
