#include "LanguageManager.h"
#include "Utils.h"
#include "Settings.h"
#include "../ui/UIHelpers.h"

// Static member initialization
HMODULE LanguageManager::s_hResourceDLL = NULL;
int LanguageManager::s_currentLanguage = AppConstants::Language::AUTO_DETECT;
bool LanguageManager::s_initialized = false;

void LanguageManager::Initialize() {
    if (s_initialized) {
        return; // Already initialized
    }

    // Get language preference from settings
    int preferredLang = g_settingsManager->GetLanguage();

    // Determine which language to load
    int targetLanguage;
    if (preferredLang == AppConstants::Language::AUTO_DETECT) {
        targetLanguage = DetectSystemLanguage();
    } else {
        targetLanguage = preferredLang;
    }

    // Load the appropriate language
    LoadLanguage(targetLanguage);
    s_initialized = true;
}

int LanguageManager::DetectSystemLanguage() {
    // Get system default locale ID
    LCID systemLocale = GetUserDefaultLCID();
    LCID uiLanguage = GetUserDefaultUILanguage();

    // Primary language ID from LCID
    WORD primaryLang = PRIMARYLANGID(systemLocale);
    WORD uiPrimaryLang = PRIMARYLANGID(uiLanguage);

    // Check for Chinese (Simplified)
    // Chinese = 0x04 (from LANG_CHINESE in winnt.h)
    // Sublanguage for Simplified Chinese = 0x02 (SUBLANG_CHINESE_SIMPLIFIED)

    // Check if system is set to Chinese (Simplified)
    if (primaryLang == 0x04) {
        // Additional check for Simplified Chinese
        if (SUBLANGID(systemLocale) == SUBLANG_CHINESE_SIMPLIFIED ||
            systemLocale == 0x0804 || // Chinese (Simplified) PRC
            systemLocale == 0x1004) { // Chinese (Singapore)
            return AppConstants::Language::CHINESE;
        }
    }

    // Check UI language preference as well
    if (uiPrimaryLang == 0x04) {
        if (SUBLANGID(uiLanguage) == SUBLANG_CHINESE_SIMPLIFIED ||
            uiLanguage == 0x0804 ||
            uiLanguage == 0x1004) {
            return AppConstants::Language::CHINESE;
        }
    }

    // Default to English for all other languages
    return AppConstants::Language::ENGLISH;
}

bool LanguageManager::LoadLanguage(int language) {
    // Unload current DLL if any
    if (s_hResourceDLL) {
        FreeLibrary(s_hResourceDLL);
        s_hResourceDLL = NULL;
    }

    // Determine DLL name based on language
    const wchar_t* dllName;
    if (language == AppConstants::Language::CHINESE) {
        dllName = L"RandPlay_zh_cn.dll";
    } else {
        dllName = L"RandPlay_en.dll";
    }

    // Try to load the language DLL from the executable directory
    std::wstring exePath = Utils::GetExecutablePath();
    std::wstring dllPath = exePath + L"\\" + dllName;

    // Load as image resource (doesn't execute code, just loads resources)
    s_hResourceDLL = LoadLibraryExW(dllPath.c_str(), NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE);

    if (s_hResourceDLL) {
        s_currentLanguage = language;
        // Set thread locale for consistency
        SetThreadLocale(language == AppConstants::Language::CHINESE ? 0x0804 : 0x0409);
        return true;
    }

    // Fallback: if we tried to load Chinese DLL but failed, try English
    if (language == AppConstants::Language::CHINESE) {
        s_hResourceDLL = LoadLibraryExW(L"RandPlay_en.dll", NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE);
        if (s_hResourceDLL) {
            s_currentLanguage = AppConstants::Language::ENGLISH;
            SetThreadLocale(0x0409);
            return true;
        }
    }

    // Last resort: use NULL (will load from EXE resources)
    s_currentLanguage = AppConstants::Language::ENGLISH;
    SetThreadLocale(0x0409);
    return false;
}

int LanguageManager::GetCurrentLanguage() {
    return s_currentLanguage;
}

std::wstring LanguageManager::LoadStringResource(UINT stringID) {
    wchar_t buffer[512];

    // First try to load from resource DLL
    if (s_hResourceDLL) {
        int len = LoadStringW(s_hResourceDLL, stringID, buffer, 512);
        if (len > 0) {
            return std::wstring(buffer, len);
        }
    }

    // Fallback to main executable resources
    HMODULE hExe = GetModuleHandle(NULL);
    int len = LoadStringW(hExe, stringID, buffer, 512);
    if (len > 0) {
        return std::wstring(buffer, len);
    }

    // Return empty string if not found
    return L"";
}
