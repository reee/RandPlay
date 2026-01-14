#pragma once

// Application constants
namespace AppConstants {
    // Window class name
    const wchar_t* const CLASS_NAME = L"RandPlayWindowClass";
    
    // File names
    const wchar_t* const INDEX_FILE_NAME = L"index.dat";
    const wchar_t* const SETTINGS_FILE_NAME = L"settings.dat";
    
    // UI constants
    const int MAX_RECENT_FILES = 100;
    
    // Common file extensions
    const wchar_t* const VIDEO_EXTENSIONS[] = { L".mp4", L".avi", L".mkv", L".mov", L".wmv" };
    const wchar_t* const IMAGE_EXTENSIONS[] = { L".jpg", L".jpeg", L".png", L".gif", L".bmp" };
    const wchar_t* const EXTENSION_TYPES[] = { L"Video Files", L"Image Files", L"Custom Extension" };
    
    // Array sizes
    const size_t VIDEO_EXTENSIONS_COUNT = sizeof(VIDEO_EXTENSIONS) / sizeof(VIDEO_EXTENSIONS[0]);
    const size_t IMAGE_EXTENSIONS_COUNT = sizeof(IMAGE_EXTENSIONS) / sizeof(IMAGE_EXTENSIONS[0]);
    const size_t EXTENSION_TYPES_COUNT = sizeof(EXTENSION_TYPES) / sizeof(EXTENSION_TYPES[0]);

    // Language settings
    namespace Language {
        constexpr int AUTO_DETECT = -1;
        constexpr int ENGLISH = 0;
        constexpr int CHINESE = 1;
    }

    // Hotkey settings
    namespace Hotkey {
        constexpr int ID_OPEN_RANDOM = 1;  // Hotkey identifier

        // Modifier flags
        constexpr UINT NONE = 0;
        constexpr UINT CTRL = MOD_CONTROL;
        constexpr int ALT = MOD_ALT;
        constexpr UINT SHIFT = MOD_SHIFT;
        constexpr UINT WIN = MOD_WIN;

        // Common virtual key codes
        constexpr UINT DISABLED = 0;      // Disabled hotkey
    }
}
