#include "FileIndexer.h"
#include "Utils.h"
#include "../constants/AppConstants.h"
#include <filesystem>
#include <algorithm>
#include <cstdio>

FileIndexer::FileIndexer() : indexedFileCount(0) {
}

FileIndexer::~FileIndexer() {
}

bool FileIndexer::BuildIndex(const std::wstring& directory, const std::wstring& extensionFilter) {
    if (directory.empty() || extensionFilter.empty()) {
        return false;
    }
    
    // Parse the extensions
    std::vector<std::wstring> extensions = ParseExtensions(extensionFilter);
    if (extensions.empty()) {
        return false;
    }
    
    std::vector<std::wstring> filePaths;
    
    try {
        // Recursively search for files with the specified extensions
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::wstring filePath = entry.path().wstring();
                std::wstring fileExt = entry.path().extension().wstring();
                
                // Convert to lowercase for case-insensitive comparison
                std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), ::towlower);
                
                // Check if this file has one of the specified extensions
                for (const auto& ext : extensions) {
                    std::wstring lowerExt = ext;
                    std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), ::towlower);
                    
                    if (fileExt == lowerExt) {
                        filePaths.push_back(filePath);
                        break;
                    }
                }
            }
        }
    }
    catch (const std::exception&) {
        return false;
    }
    
    // Write to index file
    if (WriteIndexToFile(filePaths)) {
        indexedFileCount = static_cast<int>(filePaths.size());
        return true;
    }
    
    return false;
}

bool FileIndexer::LoadIndex(std::vector<std::wstring>& filePaths) {
    std::wstring indexFilePath = GetIndexFilePath();
    
    if (!std::filesystem::exists(indexFilePath)) {
        return false;
    }
    
    // Open the binary index file for reading
    FILE* indexFile = NULL;
    _wfopen_s(&indexFile, indexFilePath.c_str(), L"rb");
    
    if (indexFile == NULL) {
        return false;
    }
    
    // Read the number of files
    uint32_t numFiles = 0;
    if (fread(&numFiles, sizeof(uint32_t), 1, indexFile) != 1) {
        fclose(indexFile);
        return false;
    }
    
    filePaths.clear();
    filePaths.reserve(numFiles);
    
    // Read each file path
    for (uint32_t i = 0; i < numFiles; i++) {
        // Read the length of the string
        uint32_t length = 0;
        if (fread(&length, sizeof(uint32_t), 1, indexFile) != 1) {
            fclose(indexFile);
            return false;
        }
        
        // Read the string data
        std::wstring filePath;
        filePath.resize(length);
        if (fread(&filePath[0], sizeof(wchar_t), length, indexFile) != length) {
            fclose(indexFile);
            return false;
        }
        
        filePaths.push_back(filePath);
    }
    
    fclose(indexFile);
    indexedFileCount = static_cast<int>(numFiles);
    return true;
}

int FileIndexer::GetIndexedFileCount() const {
    return indexedFileCount;
}

bool FileIndexer::IndexExists() const {
    return std::filesystem::exists(GetIndexFilePath());
}

std::vector<std::wstring> FileIndexer::ParseExtensions(const std::wstring& extensionFilter) {
    std::vector<std::wstring> extensions;
    size_t pos = 0;
    std::wstring ext;
    std::wstring delimiter = L";";
    
    std::wstring tempFilter = extensionFilter;
    while ((pos = tempFilter.find(delimiter)) != std::wstring::npos) {
        ext = tempFilter.substr(0, pos);
        tempFilter.erase(0, pos + delimiter.length());
        if (!ext.empty())
            extensions.push_back(ext);
    }
    
    // Add the last extension if there is one
    if (!tempFilter.empty()) {
        extensions.push_back(tempFilter);
    }
    
    // If no extensions were found, add the entire filter as one extension
    if (extensions.empty() && !extensionFilter.empty()) {
        extensions.push_back(extensionFilter);
    }
    
    return extensions;
}

bool FileIndexer::WriteIndexToFile(const std::vector<std::wstring>& filePaths) {
    std::wstring indexFilePath = GetIndexFilePath();
    
    FILE* indexFile = NULL;
    _wfopen_s(&indexFile, indexFilePath.c_str(), L"wb");
    
    if (indexFile == NULL) {
        return false;
    }
    
    // Write the number of files first
    uint32_t numFiles = static_cast<uint32_t>(filePaths.size());
    fwrite(&numFiles, sizeof(uint32_t), 1, indexFile);
    
    // Write each file path
    for (const auto& path : filePaths) {
        // Write the length of the string first (in characters)
        uint32_t length = static_cast<uint32_t>(path.length());
        fwrite(&length, sizeof(uint32_t), 1, indexFile);
        
        // Write the string data
        fwrite(path.c_str(), sizeof(wchar_t), length, indexFile);
    }
    
    fclose(indexFile);
    return true;
}

std::wstring FileIndexer::GetIndexFilePath() const {
    std::wstring exePath = Utils::GetExecutablePath();
    return exePath + L"\\" + AppConstants::INDEX_FILE_NAME;
}
