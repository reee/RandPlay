#include "UIHelpers.h"
#include "../constants/ResourceIds.h"
#include "../constants/AppConstants.h"
#include "../core/Utils.h"
#include "../core/Settings.h"
#include "../core/FileIndexer.h"
#include <random>
#include <filesystem>

// Settings dialog temporary storage
struct TempSettings {
    wchar_t directory[MAX_PATH];
    int extensionType;
    wchar_t customExtension[32];
};

void UIHelpers::InitializeControls(HWND hwnd) {
    // Initialize common controls for ListView
    INITCOMMONCONTROLSEX icex = {};
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    // Get client area dimensions
    RECT rcClient;
    GetClientRect(hwnd, &rcClient);
    int width = rcClient.right - rcClient.left;
    int height = rcClient.bottom - rcClient.top;

    // Create a better font for UI elements
    HFONT hFont = Utils::CreateUIFont();

    // Calculate control dimensions and positions
    int margin = Utils::ScaleDPI(10);
    int buttonHeight = Utils::ScaleDPI(30);
    int controlHeight = Utils::ScaleDPI(24);
    int buttonWidth = Utils::ScaleDPI(120);
    int bottomInfoHeight = controlHeight;
    int buttonSpacing = Utils::ScaleDPI(5);

    // Create the "Build Index" button
    g_hwndBuildIndexButton = CreateWindowEx(
        0, L"BUTTON", Utils::LoadStringResource(IDS_BUILD_INDEX).c_str(),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        margin, margin, buttonWidth, buttonHeight,
        hwnd, reinterpret_cast<HMENU>(static_cast<uintptr_t>(IDC_BUILD_INDEX_BUTTON)), GetModuleHandle(NULL), NULL);
    SendMessage(g_hwndBuildIndexButton, WM_SETFONT, (WPARAM)hFont, TRUE);

    // Create the "Open Random File" button
    g_hwndOpenRandomButton = CreateWindowEx(
        0, L"BUTTON", Utils::LoadStringResource(IDS_OPEN_RANDOM).c_str(),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        margin + buttonWidth + buttonSpacing, margin, buttonWidth, buttonHeight,
        hwnd, reinterpret_cast<HMENU>(static_cast<uintptr_t>(IDC_OPEN_RANDOM_BUTTON)), GetModuleHandle(NULL), NULL);
    SendMessage(g_hwndOpenRandomButton, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    // Create the recent files list as a ListView
    g_hwndRecentFilesList = CreateWindowEx(
        0, WC_LISTVIEW, NULL,
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL,
        margin, margin + buttonHeight + buttonSpacing, 
        width - margin * 2, height - margin * 2 - buttonHeight - bottomInfoHeight - buttonSpacing,
        hwnd, reinterpret_cast<HMENU>(static_cast<uintptr_t>(IDC_RECENT_FILES_LIST)), GetModuleHandle(NULL), NULL);
    SendMessage(g_hwndRecentFilesList, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    // Enable full row selection
    ListView_SetExtendedListViewStyle(g_hwndRecentFilesList, LVS_EX_FULLROWSELECT);
    
    // Add columns to the ListView
    LVCOLUMN lvc = {};
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;
    
    // File name column
    lvc.iSubItem = 0;
    lvc.cx = Utils::ScaleDPI(140);
    lvc.pszText = const_cast<LPWSTR>(Utils::LoadStringResource(IDS_COL_FILENAME).c_str());
    ListView_InsertColumn(g_hwndRecentFilesList, 0, &lvc);
    
    // Folder path column
    lvc.iSubItem = 1;
    lvc.cx = width - margin * 2 - Utils::ScaleDPI(140);
    lvc.pszText = const_cast<LPWSTR>(Utils::LoadStringResource(IDS_COL_FOLDERPATH).c_str());
    ListView_InsertColumn(g_hwndRecentFilesList, 1, &lvc);
      // Create the folder info static control at the bottom
    g_hwndFolderInfoStatic = CreateWindowEx(
        0, L"STATIC", Utils::LoadStringResource(IDS_NO_FOLDER).c_str(),
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        margin, height - margin - bottomInfoHeight, width - margin * 2, bottomInfoHeight,
        hwnd, reinterpret_cast<HMENU>(static_cast<uintptr_t>(IDC_FOLDER_INFO_STATIC)), GetModuleHandle(NULL), NULL);
    SendMessage(g_hwndFolderInfoStatic, WM_SETFONT, (WPARAM)hFont, TRUE);
}

INT_PTR CALLBACK UIHelpers::SettingsDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static TempSettings tempSettings;
    
    switch (uMsg) {    case WM_INITDIALOG:
        {
            // Set dialog title and button texts using string resources
            SetWindowText(hwnd, Utils::LoadStringResource(IDS_SETTINGS_TITLE).c_str());
            SetWindowText(GetDlgItem(hwnd, IDC_BROWSE_BUTTON), Utils::LoadStringResource(IDS_BROWSE).c_str());
            SetWindowText(GetDlgItem(hwnd, IDOK), Utils::LoadStringResource(IDS_OK).c_str());
            SetWindowText(GetDlgItem(hwnd, IDCANCEL), Utils::LoadStringResource(IDS_CANCEL).c_str());
            
            // Initialize dialog with current settings
            SetWindowText(GetDlgItem(hwnd, IDC_DIRECTORY_EDIT), g_settingsManager->GetCurrentDirectory().c_str());
              // Add items to the combo box using string resources
            HWND hwndCombo = GetDlgItem(hwnd, IDC_EXTENSION_COMBO);
            SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)Utils::LoadStringResource(IDS_VIDEO_FILES).c_str());
            SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)Utils::LoadStringResource(IDS_IMAGE_FILES).c_str());
            SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)Utils::LoadStringResource(IDS_CUSTOM_EXTENSION).c_str());
            
            // Set current selection based on current extension
            int extType = 0; // Default to video files
            const std::wstring& currentExt = g_settingsManager->GetCurrentExtension();
            if (!currentExt.empty()) {
                if (currentExt.find(L".mp4") != std::wstring::npos ||
                    currentExt.find(L".avi") != std::wstring::npos ||
                    currentExt.find(L".mkv") != std::wstring::npos) {
                    extType = 0; // Video files
                }
                else if (currentExt.find(L".jpg") != std::wstring::npos ||
                         currentExt.find(L".png") != std::wstring::npos ||
                         currentExt.find(L".gif") != std::wstring::npos) {
                    extType = 1; // Image files
                }
                else {
                    extType = 2; // Custom extension
                    SetWindowText(GetDlgItem(hwnd, IDC_CUSTOM_EXT_EDIT), currentExt.c_str());
                }
            }
            
            SendMessage(hwndCombo, CB_SETCURSEL, extType, 0);
            EnableWindow(GetDlgItem(hwnd, IDC_CUSTOM_EXT_EDIT), extType == 2);
            
            // Save temporary settings
            wcscpy_s(tempSettings.directory, g_settingsManager->GetCurrentDirectory().c_str());
            tempSettings.extensionType = extType;
            if (extType == 2 && !currentExt.empty()) {
                wcscpy_s(tempSettings.customExtension, currentExt.c_str());
            } else {
                wcscpy_s(tempSettings.customExtension, L".txt");
            }
            
            return TRUE;
        }
        
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_BROWSE_BUTTON:
            {
                BrowseForFolder(hwnd);
                return TRUE;
            }
            
        case IDC_EXTENSION_COMBO:
            {
                if (HIWORD(wParam) == CBN_SELCHANGE) {
                    int index = ComboBox_GetCurSel(GetDlgItem(hwnd, IDC_EXTENSION_COMBO));
                    EnableWindow(GetDlgItem(hwnd, IDC_CUSTOM_EXT_EDIT), index == 2);
                }
                return TRUE;
            }
            
        case IDOK:
            {
                // Save settings
                GetWindowText(GetDlgItem(hwnd, IDC_DIRECTORY_EDIT), tempSettings.directory, MAX_PATH);
                tempSettings.extensionType = ComboBox_GetCurSel(GetDlgItem(hwnd, IDC_EXTENSION_COMBO));
                GetWindowText(GetDlgItem(hwnd, IDC_CUSTOM_EXT_EDIT), tempSettings.customExtension, 32);
                  // Update global settings
                g_settingsManager->SetCurrentDirectory(tempSettings.directory);
                g_settingsManager->SetExtensionType(tempSettings.extensionType, tempSettings.customExtension);
                
                // Reload index count since directory or extension may have changed
                g_fileIndexer->LoadIndexCount();
                
                // Update folder info display
                UpdateFolderInfo();
                
                EndDialog(hwnd, IDOK);
                return TRUE;
            }
            
        case IDCANCEL:
            EndDialog(hwnd, IDCANCEL);
            return TRUE;
        }
        break;
    }
    
    return FALSE;
}

void UIHelpers::ShowSettingsDialog() {
    DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SETTINGS_DIALOG), g_hwndMain, SettingsDialogProc);
}

void UIHelpers::BrowseForFolder(HWND hwndParent) {
    BROWSEINFO bi = {};
    bi.hwndOwner = hwndParent;
    bi.lpszTitle = Utils::LoadStringResource(IDS_SELECT_DIRECTORY).c_str();
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
    if (pidl != NULL) {
        wchar_t path[MAX_PATH];
        if (SHGetPathFromIDList(pidl, path)) {
            if (hwndParent != g_hwndMain) {
                SetWindowText(GetDlgItem(hwndParent, IDC_DIRECTORY_EDIT), path);
            } else {
                g_settingsManager->SetCurrentDirectory(path);
                UpdateFolderInfo();
            }
        }
        CoTaskMemFree(pidl);
    }
}

void UIHelpers::AddToRecentFiles(const std::wstring& filePath) {
    if (!std::filesystem::exists(filePath)) {
        return;
    }
    
    auto it = std::find(g_recentFiles.begin(), g_recentFiles.end(), filePath);
    if (it != g_recentFiles.end()) {
        g_recentFiles.erase(it);
    }
    
    g_recentFiles.insert(g_recentFiles.begin(), filePath);
    
    if (g_recentFiles.size() > AppConstants::MAX_RECENT_FILES) {
        g_recentFiles.resize(AppConstants::MAX_RECENT_FILES);
    }
    
    UpdateRecentFilesList();
}

void UIHelpers::UpdateRecentFilesList() {
    if (!g_hwndRecentFilesList)
        return;
    
    // Clear the list and free any allocated memory
    int itemCount = ListView_GetItemCount(g_hwndRecentFilesList);
    for (int i = 0; i < itemCount; i++) {
        LVITEM lvi = {};
        lvi.mask = LVIF_PARAM;
        lvi.iItem = i;
        if (ListView_GetItem(g_hwndRecentFilesList, &lvi) && lvi.lParam) {
            delete[] reinterpret_cast<wchar_t*>(lvi.lParam);
        }
    }
    ListView_DeleteAllItems(g_hwndRecentFilesList);
    
    // Add each file to the list
    for (size_t i = 0; i < g_recentFiles.size(); i++) {
        const std::wstring& filePath = g_recentFiles[i];
        
        if (!std::filesystem::exists(filePath))
            continue;
        
        std::filesystem::path path(filePath);
        std::wstring fileName = path.filename().wstring();
        std::wstring folderPath = path.parent_path().wstring();
        
        // Create a copy of the file path string for the item data
        wchar_t* pFilePath = new wchar_t[filePath.length() + 1];
        wcscpy_s(pFilePath, filePath.length() + 1, filePath.c_str());
        
        // Add the item to the list
        LVITEM lvi = {};
        lvi.mask = LVIF_TEXT | LVIF_PARAM;
        lvi.iItem = static_cast<int>(i);
        lvi.iSubItem = 0;
        lvi.pszText = const_cast<LPWSTR>(fileName.c_str());
        lvi.lParam = reinterpret_cast<LPARAM>(pFilePath);
        
        int index = ListView_InsertItem(g_hwndRecentFilesList, &lvi);
        ListView_SetItemText(g_hwndRecentFilesList, index, 1, const_cast<LPWSTR>(folderPath.c_str()));
    }
    
    ShowWindow(g_hwndRecentFilesList, SW_SHOW);
    InvalidateRect(g_hwndRecentFilesList, NULL, TRUE);
}

void UIHelpers::OpenFileFromList(int index) {
    if (index < 0 || !g_hwndRecentFilesList)
        return;
    
    LVITEM lvi = {};
    lvi.mask = LVIF_PARAM;
    lvi.iItem = index;
    lvi.iSubItem = 0;
    
    if (!ListView_GetItem(g_hwndRecentFilesList, &lvi))
        return;
    
    wchar_t* pFilePath = reinterpret_cast<wchar_t*>(lvi.lParam);
    if (!pFilePath)
        return;
    
    std::wstring filePath(pFilePath);
    
    if (!std::filesystem::exists(filePath)) {
        MessageBox(g_hwndMain, Utils::LoadStringResource(IDS_FILE_NOT_EXISTS).c_str(), 
                   Utils::LoadStringResource(IDS_ERROR).c_str(), MB_OK | MB_ICONERROR);
        return;
    }
    
    ShellExecute(NULL, L"open", filePath.c_str(), NULL, NULL, SW_SHOW);
}

void UIHelpers::OpenFolderFromList(int index) {
    if (index < 0 || !g_hwndRecentFilesList)
        return;
    
    LVITEM lvi = {};
    lvi.mask = LVIF_PARAM;
    lvi.iItem = index;
    lvi.iSubItem = 0;
    
    if (!ListView_GetItem(g_hwndRecentFilesList, &lvi))
        return;
    
    wchar_t* pFilePath = reinterpret_cast<wchar_t*>(lvi.lParam);
    if (!pFilePath)
        return;
    
    std::wstring filePath(pFilePath);
    std::filesystem::path path(filePath);
    std::wstring folderPath = path.parent_path().wstring();
    
    if (!std::filesystem::exists(folderPath)) {
        MessageBox(g_hwndMain, Utils::LoadStringResource(IDS_FOLDER_NOT_EXISTS).c_str(),
                   Utils::LoadStringResource(IDS_ERROR).c_str(), MB_OK | MB_ICONERROR);
        return;
    }
    
    ShellExecuteW(NULL, L"explore", folderPath.c_str(), NULL, NULL, SW_SHOW);
}

void UIHelpers::UpdateFolderInfo() {
    if (!g_hwndFolderInfoStatic)
        return;
    
    int indexedFileCount = g_fileIndexer->GetIndexedFileCount();
    
    std::wstring info = Utils::LoadStringResource(IDS_CURRENT_FOLDER) + g_settingsManager->GetCurrentDirectory();
    info += Utils::LoadStringResource(IDS_INDEXED_FILES) + std::to_wstring(indexedFileCount);
    
    SetWindowText(g_hwndFolderInfoStatic, info.c_str());
}

void UIHelpers::BuildIndex() {
    const std::wstring& directory = g_settingsManager->GetCurrentDirectory();
    if (directory.empty()) {
        MessageBox(g_hwndMain, Utils::LoadStringResource(IDS_NO_DIRECTORY).c_str(),
                   Utils::LoadStringResource(IDS_ERROR).c_str(), MB_OK | MB_ICONERROR);
        return;
    }
    
    if (g_fileIndexer->IndexExists()) {
        int result = MessageBox(g_hwndMain, 
                                Utils::LoadStringResource(IDS_INDEX_EXISTS).c_str(), 
                                Utils::LoadStringResource(IDS_CONFIRM).c_str(), 
                                MB_YESNO | MB_ICONQUESTION);
        
        if (result != IDYES) {
            return;
        }
    }

    const std::wstring& extensionFilter = g_settingsManager->GetSelectedExtension();
    if (extensionFilter.empty()) {
        MessageBox(g_hwndMain, Utils::LoadStringResource(IDS_NO_EXTENSION).c_str(),
                   Utils::LoadStringResource(IDS_ERROR).c_str(), MB_OK | MB_ICONERROR);
        return;
    }
    
    if (g_fileIndexer->BuildIndex(directory, extensionFilter)) {
        UpdateFolderInfo();
        
        wchar_t successMsg[256];
        std::wstring successTemplate = Utils::LoadStringResource(IDS_INDEX_SUCCESS);
        swprintf_s(successMsg, 256, successTemplate.c_str(), g_fileIndexer->GetIndexedFileCount());
        MessageBox(g_hwndMain, successMsg, Utils::LoadStringResource(IDS_SUCCESS).c_str(), MB_OK | MB_ICONINFORMATION);
    } else {
        MessageBox(g_hwndMain, Utils::LoadStringResource(IDS_ERROR_SCANNING).c_str(),
                   Utils::LoadStringResource(IDS_ERROR).c_str(), MB_OK | MB_ICONERROR);
    }
}

void UIHelpers::OpenRandomFile() {
    if (!g_fileIndexer->IndexExists()) {
        MessageBox(g_hwndMain, Utils::LoadStringResource(IDS_INDEX_NOT_FOUND).c_str(),
                   Utils::LoadStringResource(IDS_ERROR).c_str(), MB_OK | MB_ICONERROR);
        return;
    }
    
    std::vector<std::wstring> filePaths;
    if (!g_fileIndexer->LoadIndex(filePaths)) {
        MessageBox(g_hwndMain, Utils::LoadStringResource(IDS_FAILED_OPEN_INDEX).c_str(),
                   Utils::LoadStringResource(IDS_ERROR).c_str(), MB_OK | MB_ICONERROR);
        return;
    }
    
    if (filePaths.empty()) {
        MessageBox(g_hwndMain, Utils::LoadStringResource(IDS_NO_FILES_FOUND).c_str(),
                   Utils::LoadStringResource(IDS_ERROR).c_str(), MB_OK | MB_ICONERROR);
        return;
    }
    
    // Generate random index
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, static_cast<int>(filePaths.size()) - 1);
    int randomIndex = distrib(gen);
    g_currentFilePath = filePaths[randomIndex];

    // Add the file to the recent files list
    AddToRecentFiles(g_currentFilePath);

    // Open the file with the default application
    ShellExecute(NULL, L"open", g_currentFilePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
