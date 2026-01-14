#pragma once

#include <windows.h>
#include <string>

// Settings structure
struct Settings {
    wchar_t directory[MAX_PATH];
    int extensionType; // 0=video, 1=image, 2=custom
    wchar_t customExtension[32];
    int language; // -1=auto detect, 0=English, 1=Chinese
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
    
    void UpdateExtensionFromType(int extensionType, const std::wstring& customExtension = L"");
};
