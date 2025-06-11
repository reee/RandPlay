#include "Settings.h"
#include "Utils.h"
#include "../constants/AppConstants.h"
#include <filesystem>
#include <fstream>
#include <algorithm>

SettingsManager::SettingsManager() {
    LoadSettings();
}

SettingsManager::~SettingsManager() {
    SaveSettings();
}

void SettingsManager::LoadSettings() {
    std::wstring filePath = Utils::GetAppDataPath() + L"\\" + AppConstants::SETTINGS_FILE_NAME;
    
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        // Default settings
        currentDirectory = std::filesystem::current_path().wstring();
        currentExtension = L".mp4;.avi;.mkv;.mov;.wmv";
        return;
    }
    
    // Read settings
    Settings settings;
    file.read(reinterpret_cast<char*>(&settings), sizeof(Settings));
    file.close();
    
    // Apply settings
    currentDirectory = settings.directory;
    UpdateExtensionFromType(settings.extensionType, settings.customExtension);
}

void SettingsManager::SaveSettings() {
    std::wstring filePath = Utils::GetAppDataPath() + L"\\" + AppConstants::SETTINGS_FILE_NAME;
    
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return;
    }
    
    Settings settings;
    wcscpy_s(settings.directory, currentDirectory.c_str());
    
    // Determine extension type
    if (currentExtension.find(L".mp4") != std::wstring::npos ||
        currentExtension.find(L".avi") != std::wstring::npos ||
        currentExtension.find(L".mkv") != std::wstring::npos) {
        settings.extensionType = 0; // Video files
    }
    else if (currentExtension.find(L".jpg") != std::wstring::npos ||
             currentExtension.find(L".png") != std::wstring::npos ||
             currentExtension.find(L".gif") != std::wstring::npos) {
        settings.extensionType = 1; // Image files
    }
    else {
        settings.extensionType = 2; // Custom extension
        wcscpy_s(settings.customExtension, currentExtension.c_str());
    }
    
    file.write(reinterpret_cast<const char*>(&settings), sizeof(Settings));
    file.close();
}

void SettingsManager::SetCurrentDirectory(const std::wstring& directory) {
    currentDirectory = directory;
}

void SettingsManager::SetCurrentExtension(const std::wstring& extension) {
    currentExtension = extension;
}

void SettingsManager::SetExtensionType(int type, const std::wstring& customExt) {
    UpdateExtensionFromType(type, customExt);
}

std::wstring SettingsManager::GetSelectedExtension() const {
    return currentExtension;
}

void SettingsManager::UpdateExtensionFromType(int extensionType, const std::wstring& customExtension) {
    if (extensionType == 0) { // Video files
        currentExtension = L"";
        for (const wchar_t* ext : AppConstants::VIDEO_EXTENSIONS) {
            currentExtension += ext;
            currentExtension += L";";
        }
        if (!currentExtension.empty())
            currentExtension.pop_back(); // Remove last semicolon
    }
    else if (extensionType == 1) { // Image files
        currentExtension = L"";
        for (const wchar_t* ext : AppConstants::IMAGE_EXTENSIONS) {
            currentExtension += ext;
            currentExtension += L";";
        }
        if (!currentExtension.empty())
            currentExtension.pop_back(); // Remove last semicolon
    }
    else { // Custom extension
        currentExtension = customExtension;
    }
}
