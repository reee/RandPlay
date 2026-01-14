#pragma once

#include <windows.h>
#include <string>

// Settings structure
struct Settings {
    wchar_t directory[MAX_PATH];
    int extensionType; // 0=video, 1=image, 2=custom
    wchar_t customExtension[32];
    int language; // -1=auto detect, 0=English, 1=Chinese

    // Hotkey configuration
    UINT openRandomHotkeyModifiers;  // Modifier flags (MOD_CONTROL, MOD_ALT, MOD_SHIFT, MOD_WIN)
    UINT openRandomHotkeyKey;        // Virtual key code (e.g., 'O', VK_F5, etc.)
    bool hotkeyEnabled;              // Whether hotkey is enabled
};

class SettingsManager {
public:
    SettingsManager();
    ~SettingsManager();
    
    // Load and save settings
    void LoadSettings();
    void SaveSettings();
    
    // Getters
    const std::wstring& GetCurrentDirectory() const { return currentDirectory; }
    const std::wstring& GetCurrentExtension() const { return currentExtension; }
    int GetLanguage() const { return currentLanguage; }

    // Hotkey configuration
    void GetHotkeyConfig(UINT& modifiers, UINT& key, bool& enabled) const {
        modifiers = currentHotkeyModifiers;
        key = currentHotkeyKey;
        enabled = currentHotkeyEnabled;
    }

    void SetHotkeyConfig(UINT modifiers, UINT key, bool enabled) {
        currentHotkeyModifiers = modifiers;
        currentHotkeyKey = key;
        currentHotkeyEnabled = enabled;
    }

    // Setters
    void SetCurrentDirectory(const std::wstring& directory);
    void SetCurrentExtension(const std::wstring& extension);
    void SetExtensionType(int type, const std::wstring& customExt = L"");
    void SetLanguage(int lang) { currentLanguage = lang; }
    void SetLanguageAndSave(int lang); // Set and persist to disk

    // Helper methods
    std::wstring GetSelectedExtension() const;

private:
    std::wstring currentDirectory;
    std::wstring currentExtension;
    int currentLanguage;
    UINT currentHotkeyModifiers;
    UINT currentHotkeyKey;
    bool currentHotkeyEnabled;
    
    void UpdateExtensionFromType(int extensionType, const std::wstring& customExtension = L"");
};
