#pragma once

#include <windows.h>
#include <string>
#include <vector>

class FileIndexer {
public:
    FileIndexer();
    ~FileIndexer();
    
    // Build index from directory and extensions
    bool BuildIndex(const std::wstring& directory, const std::wstring& extensionFilter);
      // Load index from file
    bool LoadIndex(std::vector<std::wstring>& filePaths);
    
    // Load index count only (more efficient than LoadIndex when only count is needed)
    bool LoadIndexCount();
    
    // Get indexed file count
    int GetIndexedFileCount() const;
    
    // Check if index exists
    bool IndexExists() const;
    
private:
    int indexedFileCount;
    
    // Helper methods
    std::vector<std::wstring> ParseExtensions(const std::wstring& extensionFilter);
    bool WriteIndexToFile(const std::vector<std::wstring>& filePaths);
    std::wstring GetIndexFilePath() const;
};
