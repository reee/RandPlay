#pragma once

#include "../include/pch.h"

class UIHelpers {
public:
    // UI initialization
    static void InitializeControls(HWND hwnd);
    
    // Dialog management
    static INT_PTR CALLBACK SettingsDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static void ShowSettingsDialog();
    static void BrowseForFolder(HWND hwndParent);
    
    // Recent files management
    static void AddToRecentFiles(const std::wstring& filePath);
    static void UpdateRecentFilesList();
    static void OpenFileFromList(int index);
    static void OpenFolderFromList(int index);
    
    // Folder info management
    static void UpdateFolderInfo();
    
    // Application operations
    static void BuildIndex();
    static void OpenRandomFile();
};

// External global variables (defined in main.cpp)
extern HWND g_hwndMain;
extern HWND g_hwndRecentFilesList;
extern HWND g_hwndFolderInfoStatic;
extern std::wstring g_currentFilePath;
extern std::vector<std::wstring> g_recentFiles;

// External global managers (defined in main.cpp)
extern class SettingsManager* g_settingsManager;
extern class FileIndexer* g_fileIndexer;
