#pragma once

#include <windows.h>
#include <string>

// Settings structure
struct Settings {
    wchar_t directory[MAX_PATH];
    int extensionType; // 0=video, 1=image, 2=custom
    wchar_t customExtension[32];
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
    
    // Setters
    void SetCurrentDirectory(const std::wstring& directory);
    void SetCurrentExtension(const std::wstring& extension);
    void SetExtensionType(int type, const std::wstring& customExt = L"");
    
    // Helper methods
    std::wstring GetSelectedExtension() const;
    
private:
    std::wstring currentDirectory;
    std::wstring currentExtension;
    
    void UpdateExtensionFromType(int extensionType, const std::wstring& customExtension = L"");
};
