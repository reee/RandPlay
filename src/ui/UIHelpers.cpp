#include "UIHelpers.h"
#include "../constants/ResourceIds.h"
#include "../constants/AppConstants.h"
#include "../core/Utils.h"
#include "../core/Settings.h"
#include "../core/FileIndexer.h"
#include "../core/LanguageManager.h"
#include "../core/HotkeyManager.h"
#include <random>
#include <filesystem>

// Settings dialog temporary storage
struct TempSettings {
    wchar_t directory[MAX_PATH];
    int extensionType;
    wchar_t customExtension[32];
    int language;
    UINT hotkeyModifiers;
    UINT hotkeyKey;
    bool hotkeyEnabled;
};

// Hotkey capture dialog data
struct HotkeyCaptureData {
    UINT capturedModifiers;
    UINT capturedKey;
    bool captured;
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
            // Create the same font as main window for consistency
            HFONT hFont = Utils::CreateUIFont();

            // Set font for all controls in the dialog
            SendMessage(GetDlgItem(hwnd, IDC_DIRECTORY_EDIT), WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(GetDlgItem(hwnd, IDC_BROWSE_BUTTON), WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(GetDlgItem(hwnd, IDC_EXTENSION_COMBO), WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(GetDlgItem(hwnd, IDC_CUSTOM_EXT_EDIT), WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(GetDlgItem(hwnd, IDC_LANGUAGE_COMBO), WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(GetDlgItem(hwnd, IDC_HOTKEY_ENABLED), WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(GetDlgItem(hwnd, IDC_HOTKEY_DISPLAY), WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(GetDlgItem(hwnd, IDC_SET_HOTKEY_BTN), WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(GetDlgItem(hwnd, IDOK), WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(GetDlgItem(hwnd, IDCANCEL), WM_SETFONT, (WPARAM)hFont, TRUE);

            // Set font for all static text controls
            EnumChildWindows(hwnd, [](HWND hwndChild, LPARAM lParam) -> BOOL {
                wchar_t className[256];
                GetClassName(hwndChild, className, 256);
                if (wcscmp(className, L"STATIC") == 0) {
                    SendMessage(hwndChild, WM_SETFONT, (WPARAM)lParam, TRUE);
                }
                return TRUE;
            }, (LPARAM)hFont);

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

            // Initialize language combo box
            HWND hwndLangCombo = GetDlgItem(hwnd, IDC_LANGUAGE_COMBO);
            SendMessage(hwndLangCombo, CB_ADDSTRING, 0, (LPARAM)Utils::LoadStringResource(IDS_LANGUAGE_AUTO).c_str());
            SendMessage(hwndLangCombo, CB_ADDSTRING, 0, (LPARAM)Utils::LoadStringResource(IDS_LANGUAGE_EN).c_str());
            SendMessage(hwndLangCombo, CB_ADDSTRING, 0, (LPARAM)Utils::LoadStringResource(IDS_LANGUAGE_ZH).c_str());

            // Set current language selection (convert from settings: -1->0, 0->1, 1->2)
            int currentLang = g_settingsManager->GetLanguage();
            int langIndex = currentLang + 1; // Map -1,0,1 to 0,1,2
            SendMessage(hwndLangCombo, CB_SETCURSEL, langIndex, 0);

            // Initialize hotkey controls
            Button_SetCheck(GetDlgItem(hwnd, IDC_HOTKEY_ENABLED), HotkeyManager::GetEnabled() ? BST_CHECKED : BST_UNCHECKED);
            SetWindowText(GetDlgItem(hwnd, IDC_HOTKEY_DISPLAY), HotkeyManager::GetHotkeyString().c_str());
            SetWindowText(GetDlgItem(hwnd, IDC_SET_HOTKEY_BTN), Utils::LoadStringResource(IDS_SET_HOTKEY).c_str());

            // Save temporary settings
            wcscpy_s(tempSettings.directory, g_settingsManager->GetCurrentDirectory().c_str());
            tempSettings.extensionType = extType;
            tempSettings.language = currentLang;
            if (extType == 2 && !currentExt.empty()) {
                wcscpy_s(tempSettings.customExtension, currentExt.c_str());
            } else {
                wcscpy_s(tempSettings.customExtension, L".txt");
            }

            // Save current hotkey settings to temp storage
            HotkeyManager::GetHotkey(tempSettings.hotkeyModifiers, tempSettings.hotkeyKey, tempSettings.hotkeyEnabled);

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

        case IDC_HOTKEY_ENABLED:
            {
                // Update tempSettings when checkbox state changes
                tempSettings.hotkeyEnabled = (Button_GetCheck(GetDlgItem(hwnd, IDC_HOTKEY_ENABLED)) == BST_CHECKED);

                // Update display
                if (tempSettings.hotkeyEnabled) {
                    HotkeyManager::SetHotkey(tempSettings.hotkeyModifiers, tempSettings.hotkeyKey, true);
                    SetWindowText(GetDlgItem(hwnd, IDC_HOTKEY_DISPLAY), HotkeyManager::GetHotkeyString().c_str());
                } else {
                    SetWindowText(GetDlgItem(hwnd, IDC_HOTKEY_DISPLAY), Utils::LoadStringResource(IDS_HOTKEY_DISABLED).c_str());
                }
                return TRUE;
            }

        case IDC_SET_HOTKEY_BTN:
            {
                // Show hotkey capture dialog
                HotkeyCaptureData captureData = {};
                captureData.capturedModifiers = tempSettings.hotkeyModifiers;
                captureData.capturedKey = tempSettings.hotkeyKey;
                captureData.captured = false;

                INT_PTR result = DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_HOTKEY_CAPTURE_DIALOG),
                                                hwnd, HotkeyCaptureDialogProc, reinterpret_cast<LPARAM>(&captureData));

                if (result == IDOK && captureData.captured) {
                    // Update temp settings with captured hotkey
                    tempSettings.hotkeyModifiers = captureData.capturedModifiers;
                    tempSettings.hotkeyKey = captureData.capturedKey;

                    // Update HotkeyManager temporarily for display
                    HotkeyManager::SetHotkey(tempSettings.hotkeyModifiers, tempSettings.hotkeyKey, tempSettings.hotkeyEnabled);

                    // Update display
                    SetWindowText(GetDlgItem(hwnd, IDC_HOTKEY_DISPLAY), HotkeyManager::GetHotkeyString().c_str());
                }
                return TRUE;
            }
            
        case IDOK:
            {
                // Save settings
                GetWindowText(GetDlgItem(hwnd, IDC_DIRECTORY_EDIT), tempSettings.directory, MAX_PATH);
                tempSettings.extensionType = ComboBox_GetCurSel(GetDlgItem(hwnd, IDC_EXTENSION_COMBO));
                GetWindowText(GetDlgItem(hwnd, IDC_CUSTOM_EXT_EDIT), tempSettings.customExtension, 32);

                // Get language selection (convert back: 0,1,2 to -1,0,1)
                int langIndex = ComboBox_GetCurSel(GetDlgItem(hwnd, IDC_LANGUAGE_COMBO));
                tempSettings.language = langIndex - 1;

                // Update global settings
                g_settingsManager->SetCurrentDirectory(tempSettings.directory);
                g_settingsManager->SetExtensionType(tempSettings.extensionType, tempSettings.customExtension);
                g_settingsManager->SetLanguageAndSave(tempSettings.language);

                // Save hotkey settings
                g_settingsManager->SetHotkeyConfig(tempSettings.hotkeyModifiers, tempSettings.hotkeyKey, tempSettings.hotkeyEnabled);
                HotkeyManager::SetHotkey(tempSettings.hotkeyModifiers, tempSettings.hotkeyKey, tempSettings.hotkeyEnabled);

                // Reload index count since directory or extension may have changed
                g_fileIndexer->LoadIndexCount();

                // Update folder info display
                UpdateFolderInfo();

                // Apply language change if it changed
                int oldLang = g_settingsManager->GetLanguage();
                if (oldLang != tempSettings.language) {
                    // Language preference changed, reload language
                    int targetLang = tempSettings.language;
                    if (targetLang == AppConstants::Language::AUTO_DETECT) {
                        targetLang = LanguageManager::DetectSystemLanguage();
                    }

                    if (LanguageManager::LoadLanguage(targetLang)) {
                        // Reload all UI text with new language
                        ReloadAllUIText();
                    }
                }

                // Reregister hotkey with new configuration
                HotkeyManager::ReregisterHotkey(g_hwndMain);

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
    // Try to load dialog from resource DLL first, then from EXE
    HWND hwndDialog = NULL;
    HMODULE hResourceDLL = LanguageManager::GetResourceDLL();

    if (hResourceDLL) {
        // Try loading from resource DLL
        INT_PTR result = DialogBox(hResourceDLL, MAKEINTRESOURCE(IDD_SETTINGS_DIALOG), g_hwndMain, SettingsDialogProc);
        if (result != -1 || GetLastError() != ERROR_RESOURCE_NAME_NOT_FOUND) {
            return; // Successfully loaded or got a real error (not "not found")
        }
    }

    // Fallback to EXE resources
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

void UIHelpers::ShowListContextMenu(int xPos, int yPos, int itemIndex) {
    if (itemIndex < 0 || !g_hwndRecentFilesList)
        return;

    HMENU hMenu = CreatePopupMenu();
    if (!hMenu)
        return;

    AppendMenuW(hMenu, MF_STRING, 1, Utils::LoadStringResource(IDS_MENU_OPEN_FILE).c_str());
    AppendMenuW(hMenu, MF_STRING, 2, Utils::LoadStringResource(IDS_MENU_OPEN_FOLDER).c_str());

    int selectedCmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_LEFTALIGN, xPos, yPos, 0, g_hwndMain, NULL);
    DestroyMenu(hMenu);

    if (selectedCmd == 1) {
        OpenFileFromList(itemIndex);
    } else if (selectedCmd == 2) {
        OpenFolderFromList(itemIndex);
    }
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
    std::wstring fileName = path.filename().wstring();

    if (!std::filesystem::exists(folderPath)) {
        MessageBox(g_hwndMain, Utils::LoadStringResource(IDS_FOLDER_NOT_EXISTS).c_str(),
                   Utils::LoadStringResource(IDS_ERROR).c_str(), MB_OK | MB_ICONERROR);
        return;
    }

    // Use explorer /select to open the folder and select the file
    std::wstring selectParams = L"/select,\"" + filePath + L"\"";
    ShellExecuteW(NULL, L"open", L"explorer.exe", selectParams.c_str(), NULL, SW_SHOW);
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

void UIHelpers::UpdateListViewColumns() {
    if (!g_hwndRecentFilesList)
        return;

    // Get current column widths
    RECT rcClient;
    GetClientRect(g_hwndRecentFilesList, &rcClient);
    int width = rcClient.right - rcClient.left;
    int margin = Utils::ScaleDPI(10);

    // Delete existing columns
    while (ListView_DeleteColumn(g_hwndRecentFilesList, 0));

    // Re-add columns with new language strings
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
}

void UIHelpers::ReloadAllUIText() {
    if (!g_hwndMain)
        return;

    // Reload menu - try resource DLL first, then EXE
    HMENU hMenu = NULL;
    HMODULE hResourceDLL = LanguageManager::GetResourceDLL();

    if (hResourceDLL) {
        hMenu = LoadMenu(hResourceDLL, MAKEINTRESOURCE(IDR_MAINMENU));
    }

    if (!hMenu) {
        hMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MAINMENU));
    }

    if (hMenu) {
        SetMenu(g_hwndMain, hMenu);
    }

    // Update window title
    SetWindowText(g_hwndMain, Utils::LoadStringResource(IDS_APP_TITLE).c_str());

    // Update folder info text
    UpdateFolderInfo();

    // Update ListView column headers
    UpdateListViewColumns();

    // Force redraw of the main window
    InvalidateRect(g_hwndMain, NULL, TRUE);
    UpdateWindow(g_hwndMain);
}

INT_PTR CALLBACK UIHelpers::HotkeyCaptureDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HotkeyCaptureData* pData = nullptr;

    switch (uMsg) {
        case WM_INITDIALOG:
        {
            pData = (HotkeyCaptureData*)lParam;
            pData->capturedModifiers = 0;
            pData->capturedKey = 0;
            pData->captured = false;

            // Set prompt text
            SetWindowText(hwnd, Utils::LoadStringResource(IDS_PRESS_KEY).c_str());

            // Center the dialog on parent window
            HWND hwndParent = GetParent(hwnd);
            if (hwndParent) {
                RECT rcParent, rcDialog;
                GetWindowRect(hwndParent, &rcParent);
                GetWindowRect(hwnd, &rcDialog);

                int x = rcParent.left + (rcParent.right - rcParent.left - (rcDialog.right - rcDialog.left)) / 2;
                int y = rcParent.top + (rcParent.bottom - rcParent.top - (rcDialog.bottom - rcDialog.top)) / 2;
                SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            }

            // Set timer to check keyboard state every 50ms
            SetTimer(hwnd, 1, 50, NULL);
            return TRUE;
        }

        case WM_TIMER:
        {
            if (wParam == 1 && pData && !pData->captured) {
                pData->capturedModifiers = 0;
                pData->capturedKey = 0;

                // Check modifier keys
                if (GetAsyncKeyState(VK_CONTROL) & 0x8000) pData->capturedModifiers |= MOD_CONTROL;
                if (GetAsyncKeyState(VK_SHIFT) & 0x8000) pData->capturedModifiers |= MOD_SHIFT;
                if (GetAsyncKeyState(VK_MENU) & 0x8000) pData->capturedModifiers |= MOD_ALT;
                if (GetAsyncKeyState(VK_LWIN) & 0x8000 || GetAsyncKeyState(VK_RWIN) & 0x8000) pData->capturedModifiers |= MOD_WIN;

                // Check function keys F1-F24
                for (UINT vk = VK_F1; vk <= VK_F24; vk++) {
                    if (GetAsyncKeyState(vk) & 0x8000) {
                        pData->capturedKey = vk;
                        pData->captured = true;
                        KillTimer(hwnd, 1);
                        EndDialog(hwnd, IDOK);
                        return TRUE;
                    }
                }

                // Check letter keys A-Z
                for (UINT vk = 'A'; vk <= 'Z'; vk++) {
                    if (GetAsyncKeyState(vk) & 0x8000) {
                        pData->capturedKey = vk;
                        pData->captured = true;
                        KillTimer(hwnd, 1);
                        EndDialog(hwnd, IDOK);
                        return TRUE;
                    }
                }

                // Check digit keys 0-9
                for (UINT vk = '0'; vk <= '9'; vk++) {
                    if (GetAsyncKeyState(vk) & 0x8000) {
                        pData->capturedKey = vk;
                        pData->captured = true;
                        KillTimer(hwnd, 1);
                        EndDialog(hwnd, IDOK);
                        return TRUE;
                    }
                }

                // Check other common keys
                if (GetAsyncKeyState(VK_SPACE) & 0x8000) { pData->capturedKey = VK_SPACE; pData->captured = true; }
                else if (GetAsyncKeyState(VK_RETURN) & 0x8000) { pData->capturedKey = VK_RETURN; pData->captured = true; }
                else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) { pData->capturedKey = VK_ESCAPE; pData->captured = true; }
                else if (GetAsyncKeyState(VK_TAB) & 0x8000) { pData->capturedKey = VK_TAB; pData->captured = true; }
                else if (GetAsyncKeyState(VK_BACK) & 0x8000) { pData->capturedKey = VK_BACK; pData->captured = true; }
                else if (GetAsyncKeyState(VK_DELETE) & 0x8000) { pData->capturedKey = VK_DELETE; pData->captured = true; }
                else if (GetAsyncKeyState(VK_INSERT) & 0x8000) { pData->capturedKey = VK_INSERT; pData->captured = true; }

                if (pData->captured) {
                    KillTimer(hwnd, 1);
                    EndDialog(hwnd, IDOK);
                }
            }
            return TRUE;
        }

        case WM_DESTROY:
            KillTimer(hwnd, 1);
            return FALSE;
    }
    return FALSE;
}
