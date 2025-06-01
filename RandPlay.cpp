// Define UNICODE before including Windows headers
#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <string>
#include <vector>
#include <fstream>
#include <random>
#include <filesystem>
#include <algorithm>
#include <sstream>

// Include the resource header
#include "resource.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' \"\
\"version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Get the application data path - now returns current directory
std::wstring GetAppDataPath()
{
    // Always use current directory for settings
    return std::filesystem::current_path().wstring();
}

// Get the executable directory path
std::wstring GetExecutablePath()
{
    wchar_t exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);
    
    // Convert to filesystem path and get the parent directory
    std::filesystem::path path(exePath);
    return path.parent_path().wstring();
}

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shlwapi.lib")

// Window class name
const wchar_t CLASS_NAME[] = L"RandPlayWindowClass";

// Menu IDs
#define IDM_SETTINGS 201
#define IDM_EXIT 202

// Dialog IDs
#define IDD_SETTINGS_DIALOG 301

// Control IDs
#define IDC_DIRECTORY_EDIT 101
#define IDC_BROWSE_BUTTON 102
#define IDC_EXTENSION_COMBO 103
#define IDC_CUSTOM_EXT_EDIT 104
#define IDC_BUILD_INDEX_BUTTON 105
#define IDC_OPEN_RANDOM_BUTTON 106
#define IDC_RECENT_FILES_LIST 107
#define IDC_FOLDER_INFO_STATIC 110

// Common file extensions
const wchar_t* VIDEO_EXTENSIONS[] = { L".mp4", L".avi", L".mkv", L".mov", L".wmv" };
const wchar_t* IMAGE_EXTENSIONS[] = { L".jpg", L".jpeg", L".png", L".gif", L".bmp" };
const wchar_t* EXTENSION_TYPES[] = { L"Video Files", L"Image Files", L"Custom Extension" };

// Global variables
HWND g_hwndMain = NULL;
HWND g_hwndDirectoryEdit = NULL;
HWND g_hwndExtensionCombo = NULL;
HWND g_hwndCustomExtEdit = NULL;
HWND g_hwndRecentFilesList = NULL;
HWND g_hwndFolderInfoStatic = NULL;
std::wstring g_currentFilePath;
std::wstring g_currentDirectory;
std::wstring g_currentExtension;
int g_indexedFileCount = 0;
const wchar_t* INDEX_FILE_NAME = L"index.dat";
const int MAX_RECENT_FILES = 10;
std::vector<std::wstring> g_recentFiles;
const wchar_t* SETTINGS_FILE_NAME = L"settings.dat";

// Settings structure
struct Settings {
    wchar_t directory[MAX_PATH];
    int extensionType; // 0=video, 1=image, 2=custom
    wchar_t customExtension[32];
};

// Function declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK SettingsDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InitializeControls(HWND hwnd);
void BrowseForFolder(HWND hwndParent);
void BuildIndex();
void OpenRandomFile();
void AddToRecentFiles(const std::wstring& filePath);
void UpdateRecentFilesList();
void OpenFileFromList(int index);
void OpenFolderFromList(int index);
void ShowSettingsDialog();
void SaveSettings();
void LoadSettings();
std::wstring GetAppDataPath();
std::wstring GetExecutablePath(); // New function to get the executable path
void UpdateFolderInfo();
std::wstring GetSelectedExtension();
HFONT CreateUIFont();
int ScaleDPI(int value);

// Helper function to load string resources
std::wstring LoadStringResource(UINT stringID)
{
    wchar_t buffer[1024];
    int len = LoadString(GetModuleHandle(NULL), stringID, buffer, sizeof(buffer)/sizeof(wchar_t));
    if (len > 0)
    {
        return std::wstring(buffer, len);
    }
    return L"";
}

// Enable visual styles and DPI awareness
HRESULT EnableVisualStyles()
{
    HRESULT hr = S_OK;
    
    // Enable visual styles
    hr = CoInitialize(NULL);
    if (SUCCEEDED(hr))
    {
        ACTCTX actCtx = {0};
        actCtx.cbSize = sizeof(actCtx);
        actCtx.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID | ACTCTX_FLAG_HMODULE_VALID;
        actCtx.lpResourceName = MAKEINTRESOURCE(2);
        actCtx.hModule = GetModuleHandle(NULL);
        
        HANDLE hActCtx = CreateActCtx(&actCtx);
        if (hActCtx != INVALID_HANDLE_VALUE)
        {
            ULONG_PTR cookie;
            ActivateActCtx(hActCtx, &cookie);
        }
    }
    
    return hr;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Set DPI awareness to improve text rendering
    SetProcessDPIAware();
    
    // Enable visual styles for modern look
    EnableVisualStyles();
    
    // Initialize common controls
    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icc.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icc);

    // Register the window class with improved appearance
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.style = CS_HREDRAW | CS_VREDRAW;  // Redraw on resize to prevent blurry text

    RegisterClass(&wc);

    // Scale window size based on DPI
    int windowWidth = ScaleDPI(300);
    int windowHeight = ScaleDPI(300);
    
    // Create the window with a more modern style
    g_hwndMain = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,     // Extended window style for better borders
        CLASS_NAME,                 // Window class
        LoadStringResource(IDS_APP_TITLE).c_str(),  // Window title from resources
        WS_OVERLAPPEDWINDOW,        // Window style
        CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight,  // Size and position scaled for DPI
        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (g_hwndMain == NULL)
    {
        return 0;
    }

    ShowWindow(g_hwndMain, nCmdShow);
    UpdateWindow(g_hwndMain);

    // Message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        {
            // Load settings before initializing controls
            LoadSettings();
            
            // Create menu
            HMENU hMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MAINMENU));
            SetMenu(hwnd, hMenu);
            
            // Set a light gray background color for the window
            SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)GetSysColorBrush(COLOR_BTNFACE));
            
            // Initialize controls
            InitializeControls(hwnd);
            
            // Update folder info
            UpdateFolderInfo();
            
            // Update recent files list
            UpdateRecentFilesList();
            return 0;
        }

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDM_SETTINGS:
            ShowSettingsDialog();
            break;
            
        case IDM_EXIT:
            DestroyWindow(hwnd);
            break;
            
        case IDC_BUILD_INDEX_BUTTON:
            BuildIndex();
            break;
            
        case IDC_OPEN_RANDOM_BUTTON:
            OpenRandomFile();
            break;
        }
        break;
        
    case WM_NOTIFY:
        {
            NMHDR* pnmhdr = (NMHDR*)lParam;
            if (pnmhdr->idFrom == IDC_RECENT_FILES_LIST)
            {
                if (pnmhdr->code == NM_DBLCLK)
                {
                    // Handle double-click on any column
                    NMITEMACTIVATE* pnmia = (NMITEMACTIVATE*)lParam;
                    int index = pnmia->iItem;
                    if (index != -1)
                    {
                        // Check which column was clicked
                        if (pnmia->iSubItem == 1) // Folder column
                        {
                            OpenFolderFromList(index);
                        }
                        else // File name column
                        {
                            OpenFileFromList(index);
                        }
                    }
                    return 0;
                }
                else if (pnmhdr->code == NM_CLICK)
                {
                    // Handle single click for better feedback
                    NMITEMACTIVATE* pnmia = (NMITEMACTIVATE*)lParam;
                    int index = pnmia->iItem;
                    if (index != -1)
                    {
                        // Ensure the item is selected
                        ListView_SetItemState(g_hwndRecentFilesList, index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
                    }
                    return 0;
                }
            }
            return 0; // Important: Return 0 to indicate we processed the message
        }
        break;

    case WM_SIZE:
        {
            // Resize controls when the window is resized
            if (g_hwndRecentFilesList && g_hwndFolderInfoStatic)
            {
                RECT rcClient;
                GetClientRect(hwnd, &rcClient);
                int width = rcClient.right - rcClient.left;
                int height = rcClient.bottom - rcClient.top;
                
                // Resize and reposition controls - using the same dimensions as in InitializeControls
                int margin = ScaleDPI(10); // Reduced margin for more compact layout
                int buttonHeight = ScaleDPI(30); // Reduced button height
                int controlHeight = ScaleDPI(24); // Reduced control height
                int bottomInfoHeight = controlHeight;
                int buttonSpacing = ScaleDPI(5); // Reduced spacing between buttons
                
                // Position the recent files list
                SetWindowPos(g_hwndRecentFilesList, NULL, 
                    margin, margin + buttonHeight + buttonSpacing, 
                    width - margin * 2, height - margin * 2 - buttonHeight - bottomInfoHeight - buttonSpacing,
                    SWP_NOZORDER);
                
                // Position the folder info static at the bottom
                SetWindowPos(g_hwndFolderInfoStatic, NULL,
                    margin, height - margin - bottomInfoHeight, width - margin * 2, bottomInfoHeight,
                    SWP_NOZORDER);
                
                // Force redraw to ensure text is visible
                InvalidateRect(g_hwndRecentFilesList, NULL, TRUE);
                InvalidateRect(g_hwndFolderInfoStatic, NULL, TRUE);
                
                // No background color setting for ListView
            }
            return 0;
        }

    case WM_CTLCOLORSTATIC:
        // Set the folder info text color to green
        if ((HWND)lParam == g_hwndFolderInfoStatic)
        {
            HDC hdcStatic = (HDC)wParam;
            SetTextColor(hdcStatic, RGB(0, 128, 0)); // Green text
            SetBkMode(hdcStatic, TRANSPARENT);
            return (LRESULT)GetSysColorBrush(COLOR_BTNFACE);
        }
        break;
        
    case WM_DESTROY:
        // Save settings before exiting
        SaveSettings();
        
        // Clean up ListView item data to prevent memory leaks
        if (g_hwndRecentFilesList)
        {
            int itemCount = ListView_GetItemCount(g_hwndRecentFilesList);
            for (int i = 0; i < itemCount; i++)
            {
                LVITEM lvi = {};
                lvi.mask = LVIF_PARAM;
                lvi.iItem = i;
                if (ListView_GetItem(g_hwndRecentFilesList, &lvi) && lvi.lParam)
                {
                    delete[] reinterpret_cast<wchar_t*>(lvi.lParam);
                }
            }
        }
        
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Create a better font for the UI that scales with DPI
HFONT CreateUIFont()
{
    // Get the device context for the screen
    HDC hdc = GetDC(NULL);
    
    // Get the logical DPI
    int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
    int dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
    
    // Calculate font size based on DPI (12 points at 96 DPI to match menu font size)
    int fontSize = -MulDiv(12, dpiY, 96);
    
    // Release the device context
    ReleaseDC(NULL, hdc);
    
    return CreateFont(
        fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    );
}
// Helper function to scale UI elements based on DPI
int ScaleDPI(int value)
{
    static int dpiY = 0;
    
    if (dpiY == 0)
    {
        HDC hdc = GetDC(NULL);
        dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
        ReleaseDC(NULL, hdc);
    }
    
    return MulDiv(value, dpiY, 96);
}

void InitializeControls(HWND hwnd)
{
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
    HFONT hFont = CreateUIFont();
    
    // Calculate control dimensions and positions
    int margin = ScaleDPI(10); // Reduced margin for more compact layout
    int buttonHeight = ScaleDPI(30); // Reduced button height
    int controlHeight = ScaleDPI(24); // Reduced control height
    int buttonWidth = ScaleDPI(120); // Reduced button width
    int bottomInfoHeight = controlHeight;
    int buttonSpacing = ScaleDPI(5); // Reduced spacing between buttons
    
    // Create the "Build Index" button
    HWND hwndBuildIndex = CreateWindowEx(
        0, L"BUTTON", LoadStringResource(IDS_BUILD_INDEX).c_str(),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        margin, margin, buttonWidth, buttonHeight,
        hwnd, (HMENU)IDC_BUILD_INDEX_BUTTON, GetModuleHandle(NULL), NULL);
    SendMessage(hwndBuildIndex, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    // Create the "Open Random File" button - positioned closer to the first button
    HWND hwndOpenRandom = CreateWindowEx(
        0, L"BUTTON", LoadStringResource(IDS_OPEN_RANDOM).c_str(),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        margin + buttonWidth + buttonSpacing, margin, buttonWidth, buttonHeight,
        hwnd, (HMENU)IDC_OPEN_RANDOM_BUTTON, GetModuleHandle(NULL), NULL);
    SendMessage(hwndOpenRandom, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    // Create the recent files list as a ListView with two columns - without border to match the gray background
    g_hwndRecentFilesList = CreateWindowEx(
        0, WC_LISTVIEW, NULL,
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL,
        margin, margin + buttonHeight + buttonSpacing, 
        width - margin * 2, height - margin * 2 - buttonHeight - bottomInfoHeight - buttonSpacing,
        hwnd, (HMENU)IDC_RECENT_FILES_LIST, GetModuleHandle(NULL), NULL);
    SendMessage(g_hwndRecentFilesList, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    // Enable full row selection but without hover effects
    ListView_SetExtendedListViewStyle(g_hwndRecentFilesList, LVS_EX_FULLROWSELECT);
    
    // Add columns to the ListView
    LVCOLUMN lvc = {};
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;
    
    // File name column
    lvc.iSubItem = 0;
    lvc.cx = ScaleDPI(140);
    lvc.pszText = const_cast<LPWSTR>(LoadStringResource(IDS_COL_FILENAME).c_str());
    ListView_InsertColumn(g_hwndRecentFilesList, 0, &lvc);
    
    // Folder path column
    lvc.iSubItem = 1;
    lvc.cx = width - margin * 2 - ScaleDPI(140); // Remaining width
    lvc.pszText = const_cast<LPWSTR>(LoadStringResource(IDS_COL_FOLDERPATH).c_str());
    ListView_InsertColumn(g_hwndRecentFilesList, 1, &lvc);
    
    // Create the folder info static control at the bottom
    g_hwndFolderInfoStatic = CreateWindowEx(
        0, L"STATIC", LoadStringResource(IDS_NO_FOLDER).c_str(),
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        margin, height - margin - bottomInfoHeight, width - margin * 2, bottomInfoHeight,
        hwnd, (HMENU)IDC_FOLDER_INFO_STATIC, GetModuleHandle(NULL), NULL);
    SendMessage(g_hwndFolderInfoStatic, WM_SETFONT, (WPARAM)hFont, TRUE);
    // We'll handle the text color in WM_CTLCOLORSTATIC
}

// Settings dialog procedure
INT_PTR CALLBACK SettingsDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static Settings tempSettings;
    
    switch (uMsg)
    {
    case WM_INITDIALOG:
        {
            // Initialize dialog with current settings
            SetWindowText(GetDlgItem(hwnd, IDC_DIRECTORY_EDIT), g_currentDirectory.c_str());
            
            // Add items to the combo box
            HWND hwndCombo = GetDlgItem(hwnd, IDC_EXTENSION_COMBO);
            for (const wchar_t* ext : EXTENSION_TYPES)
            {
                SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)ext);
            }
            
            // Set current selection
            int extType = 0; // Default to video files
            if (!g_currentExtension.empty())
            {
                // Try to determine the extension type
                if (g_currentExtension.find(L".mp4") != std::wstring::npos ||
                    g_currentExtension.find(L".avi") != std::wstring::npos ||
                    g_currentExtension.find(L".mkv") != std::wstring::npos)
                {
                    extType = 0; // Video files
                }
                else if (g_currentExtension.find(L".jpg") != std::wstring::npos ||
                         g_currentExtension.find(L".png") != std::wstring::npos ||
                         g_currentExtension.find(L".gif") != std::wstring::npos)
                {
                    extType = 1; // Image files
                }
                else
                {
                    extType = 2; // Custom extension
                    SetWindowText(GetDlgItem(hwnd, IDC_CUSTOM_EXT_EDIT), g_currentExtension.c_str());
                }
            }
            
            SendMessage(hwndCombo, CB_SETCURSEL, extType, 0);
            
            // Enable/disable custom extension edit based on selection
            EnableWindow(GetDlgItem(hwnd, IDC_CUSTOM_EXT_EDIT), extType == 2);
            
            // Save temporary settings
            wcscpy_s(tempSettings.directory, g_currentDirectory.c_str());
            tempSettings.extensionType = extType;
            if (extType == 2 && !g_currentExtension.empty())
            {
                wcscpy_s(tempSettings.customExtension, g_currentExtension.c_str());
            }
            else
            {
                wcscpy_s(tempSettings.customExtension, L".txt");
            }
            
            return TRUE;
        }
        
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BROWSE_BUTTON:
            {
                BrowseForFolder(hwnd);
                return TRUE;
            }
            
        case IDC_EXTENSION_COMBO:
            {
                if (HIWORD(wParam) == CBN_SELCHANGE)
                {
                    int index = ComboBox_GetCurSel(GetDlgItem(hwnd, IDC_EXTENSION_COMBO));
                    // Enable or disable custom extension edit based on selection
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
                g_currentDirectory = tempSettings.directory;
                
                // Update extension based on type
                if (tempSettings.extensionType == 0) // Video files
                {
                    g_currentExtension = L"";
                    for (const wchar_t* ext : VIDEO_EXTENSIONS)
                    {
                        g_currentExtension += ext;
                        g_currentExtension += L";";
                    }
                    if (!g_currentExtension.empty())
                        g_currentExtension.pop_back(); // Remove last semicolon
                }
                else if (tempSettings.extensionType == 1) // Image files
                {
                    g_currentExtension = L"";
                    for (const wchar_t* ext : IMAGE_EXTENSIONS)
                    {
                        g_currentExtension += ext;
                        g_currentExtension += L";";
                    }
                    if (!g_currentExtension.empty())
                        g_currentExtension.pop_back(); // Remove last semicolon
                }
                else // Custom extension
                {
                    g_currentExtension = tempSettings.customExtension;
                    // Ensure it starts with a dot
                    if (!g_currentExtension.empty() && g_currentExtension[0] != L'.')
                        g_currentExtension = L"." + g_currentExtension;
                }
                
                // Save settings to file
                SaveSettings();
                
                // Update folder info
                UpdateFolderInfo();
                
                EndDialog(hwnd, IDOK);
                return TRUE;
            }
            
        case IDCANCEL:
            {
                EndDialog(hwnd, IDCANCEL);
                return TRUE;
            }
        }
        break;
    }
    
    return FALSE;
}

// Show the settings dialog
void ShowSettingsDialog()
{
    DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SETTINGS_DIALOG), g_hwndMain, SettingsDialogProc);
}

// Save settings to file
void SaveSettings()
{
    std::wstring filePath = GetAppDataPath() + L"\\" + SETTINGS_FILE_NAME;
    
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open())
    {
        return;
    }
    
    Settings settings;
    wcscpy_s(settings.directory, g_currentDirectory.c_str());
    
    // Determine extension type
    if (g_currentExtension.find(L".mp4") != std::wstring::npos ||
        g_currentExtension.find(L".avi") != std::wstring::npos ||
        g_currentExtension.find(L".mkv") != std::wstring::npos)
    {
        settings.extensionType = 0; // Video files
    }
    else if (g_currentExtension.find(L".jpg") != std::wstring::npos ||
             g_currentExtension.find(L".png") != std::wstring::npos ||
             g_currentExtension.find(L".gif") != std::wstring::npos)
    {
        settings.extensionType = 1; // Image files
    }
    else
    {
        settings.extensionType = 2; // Custom extension
        wcscpy_s(settings.customExtension, g_currentExtension.c_str());
    }
    
    file.write(reinterpret_cast<const char*>(&settings), sizeof(Settings));
    file.close();
}



// Load settings from file
void LoadSettings()
{
    std::wstring filePath = GetAppDataPath() + L"\\" + SETTINGS_FILE_NAME;
    
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open())
    {
        // Default settings
        g_currentDirectory = std::filesystem::current_path().wstring();
        g_currentExtension = L".mp4;.avi;.mkv;.mov;.wmv";
        return;
    }
    
    // Read settings
    Settings settings;
    file.read(reinterpret_cast<char*>(&settings), sizeof(Settings));
    file.close();
    
    // Apply settings
    g_currentDirectory = settings.directory;
    
    if (settings.extensionType == 0) // Video files
    {
        g_currentExtension = L"";
        for (const wchar_t* ext : VIDEO_EXTENSIONS)
        {
            g_currentExtension += ext;
            g_currentExtension += L";";
        }
        if (!g_currentExtension.empty())
            g_currentExtension.pop_back(); // Remove last semicolon
    }
    else if (settings.extensionType == 1) // Image files
    {
        g_currentExtension = L"";
        for (const wchar_t* ext : IMAGE_EXTENSIONS)
        {
            g_currentExtension += ext;
            g_currentExtension += L";";
        }
        if (!g_currentExtension.empty())
            g_currentExtension.pop_back(); // Remove last semicolon
    }
    else // Custom extension
    {
        g_currentExtension = settings.customExtension;
    }
}

// Update folder info display
void UpdateFolderInfo()
{
    if (!g_hwndFolderInfoStatic)
        return;
    
    // Count the number of indexed files
    g_indexedFileCount = 0;
    
    // The index file is stored in the executable directory
    std::wstring exePath = GetExecutablePath();
    std::wstring indexFilePath = exePath + L"\\" + INDEX_FILE_NAME;
    
    // Check if the index file exists
    if (std::filesystem::exists(indexFilePath))
    {
        // Open the binary index file for reading
        FILE* indexFile = NULL;
        errno_t err = _wfopen_s(&indexFile, indexFilePath.c_str(), L"rb");
        
        if (err == 0 && indexFile != NULL)
        {
            // Read the number of entries (first 4 bytes)
            uint32_t numFiles = 0;
            size_t readResult = fread(&numFiles, sizeof(uint32_t), 1, indexFile);
            
            if (readResult == 1)
            {
                g_indexedFileCount = static_cast<int>(numFiles);
            }
            
            fclose(indexFile);
        }
    }
    
    std::wstring info = LoadStringResource(IDS_CURRENT_FOLDER) + g_currentDirectory;
    info += LoadStringResource(IDS_INDEXED_FILES) + std::to_wstring(g_indexedFileCount);
    
    SetWindowText(g_hwndFolderInfoStatic, info.c_str());
}

// Browse for folder
void BrowseForFolder(HWND hwndParent)
{
    BROWSEINFO bi = {};
    bi.hwndOwner = hwndParent;
    bi.lpszTitle = LoadStringResource(IDS_SELECT_DIRECTORY).c_str();
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
    if (pidl != NULL)
    {
        wchar_t path[MAX_PATH];
        if (SHGetPathFromIDList(pidl, path))
        {
            // If this is called from the settings dialog
            if (hwndParent != g_hwndMain)
            {
                SetWindowText(GetDlgItem(hwndParent, IDC_DIRECTORY_EDIT), path);
            }
            else
            {
                g_currentDirectory = path;
                UpdateFolderInfo();
            }
        }

        CoTaskMemFree(pidl);
    }
}

// Get the selected extension from settings
std::wstring GetSelectedExtension()
{
    return g_currentExtension;
}

// Add a file to the recent files list
void AddToRecentFiles(const std::wstring& filePath)
{
    // Check if file exists
    if (!std::filesystem::exists(filePath))
    {
        return;
    }
    
    // Check if the file is already in the list
    auto it = std::find(g_recentFiles.begin(), g_recentFiles.end(), filePath);
    
    // If found, remove it so we can add it to the top
    if (it != g_recentFiles.end())
    {
        g_recentFiles.erase(it);
    }
    
    // Add the file to the top of the list
    g_recentFiles.insert(g_recentFiles.begin(), filePath);
    
    // Limit the list size
    if (g_recentFiles.size() > MAX_RECENT_FILES)
    {
        g_recentFiles.resize(MAX_RECENT_FILES);
    }
    
    // Update the list display
    UpdateRecentFilesList();
}



// Update the recent files list display
void UpdateRecentFilesList()
{
    if (!g_hwndRecentFilesList)
        return;
    
    // Clear the list and free any allocated memory
    int itemCount = ListView_GetItemCount(g_hwndRecentFilesList);
    for (int i = 0; i < itemCount; i++)
    {
        LVITEM lvi = {};
        lvi.mask = LVIF_PARAM;
        lvi.iItem = i;
        if (ListView_GetItem(g_hwndRecentFilesList, &lvi) && lvi.lParam)
        {
            delete[] reinterpret_cast<wchar_t*>(lvi.lParam);
        }
    }
    ListView_DeleteAllItems(g_hwndRecentFilesList);
    
    // Add each file to the list
    for (size_t i = 0; i < g_recentFiles.size(); i++)
    {
        const std::wstring& filePath = g_recentFiles[i];
        
        // Check if the file exists
        if (!std::filesystem::exists(filePath))
            continue;
        
        // Get the file name and folder path
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
        
        // Set the folder path in the second column
        ListView_SetItemText(g_hwndRecentFilesList, index, 1, const_cast<LPWSTR>(folderPath.c_str()));
    }
    
    // Make sure the list is visible and has focus
    ShowWindow(g_hwndRecentFilesList, SW_SHOW);
    InvalidateRect(g_hwndRecentFilesList, NULL, TRUE);
}

// Open a file from the recent files list
void OpenFileFromList(int index)
{
    if (index < 0 || !g_hwndRecentFilesList)
        return;
    
    // Get the file path from the ListView item data
    LVITEM lvi = {};
    lvi.mask = LVIF_PARAM;
    lvi.iItem = index;
    lvi.iSubItem = 0;
    
    if (!ListView_GetItem(g_hwndRecentFilesList, &lvi))
        return;
    
    // Get the file path from the item data
    wchar_t* pFilePath = reinterpret_cast<wchar_t*>(lvi.lParam);
    if (!pFilePath)
        return;
    
    std::wstring filePath(pFilePath);
    
    // Check if the file exists
    if (!std::filesystem::exists(filePath))
    {
        MessageBox(g_hwndMain, LoadStringResource(IDS_FILE_NOT_EXISTS).c_str(), LoadStringResource(IDS_ERROR).c_str(), MB_OK | MB_ICONERROR);
        return;
    }
    
    // Open the file with the default application
    ShellExecute(NULL, L"open", filePath.c_str(), NULL, NULL, SW_SHOW);
}

// Open a folder from the recent files list
void OpenFolderFromList(int index)
{
    if (index < 0 || !g_hwndRecentFilesList)
        return;
    
    // Get the file path from the ListView item data
    LVITEM lvi = {};
    lvi.mask = LVIF_PARAM;
    lvi.iItem = index;
    lvi.iSubItem = 0;
    
    if (!ListView_GetItem(g_hwndRecentFilesList, &lvi))
        return;
    
    // Get the file path from the item data
    wchar_t* pFilePath = reinterpret_cast<wchar_t*>(lvi.lParam);
    if (!pFilePath)
        return;
    
    std::wstring filePath(pFilePath);
    
    // Get the folder path
    std::filesystem::path path(filePath);
    std::wstring folderPath = path.parent_path().wstring();
    
    // Check if the folder exists
    if (!std::filesystem::exists(folderPath))
    {
        MessageBox(g_hwndMain, LoadStringResource(IDS_FOLDER_NOT_EXISTS).c_str(), LoadStringResource(IDS_ERROR).c_str(), MB_OK | MB_ICONERROR);
        return;
    }
    
    // Open the folder in Explorer
    // Use ShellExecuteW to ensure proper Unicode support
    ShellExecuteW(NULL, L"explore", folderPath.c_str(), NULL, NULL, SW_SHOW);
}

void BuildIndex()
{
    // Check if a directory is selected
    if (g_currentDirectory.empty())
    {
        MessageBox(g_hwndMain, LoadStringResource(IDS_NO_DIRECTORY).c_str(), LoadStringResource(IDS_ERROR).c_str(), MB_OK | MB_ICONERROR);
        return;
    }
    
    // Check if index file already exists
    std::wstring exePath = GetExecutablePath();
    std::wstring indexFilePath = exePath + L"\\" + INDEX_FILE_NAME;
    
    // Check if the file exists
    if (std::filesystem::exists(indexFilePath))
    {
        // Ask for confirmation before rebuilding
        int result = MessageBox(g_hwndMain, 
                                LoadStringResource(IDS_INDEX_EXISTS).c_str(), 
                                LoadStringResource(IDS_CONFIRM).c_str(), 
                                MB_YESNO | MB_ICONQUESTION);
        
        if (result != IDYES)
        {
            // User chose not to rebuild
            return;
        }
    }

    // Get the selected extension
    std::wstring extensionFilter = GetSelectedExtension();
    if (extensionFilter.empty())
    {
        MessageBox(g_hwndMain, LoadStringResource(IDS_NO_EXTENSION).c_str(), LoadStringResource(IDS_ERROR).c_str(), MB_OK | MB_ICONERROR);
        return;
    }

    // Parse the extensions (they are separated by semicolons)
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
    if (extensions.empty() && !extensionFilter.empty())
    {
        extensions.push_back(extensionFilter);
    }

    // Open the index file for binary writing
    std::vector<std::wstring> filePaths;
    int fileCount = 0;
    
    try
    {
        // Recursively search for files with the specified extensions
        for (const auto& entry : std::filesystem::recursive_directory_iterator(g_currentDirectory))
        {
            if (entry.is_regular_file())
            {
                std::wstring filePath = entry.path().wstring();
                std::wstring fileExt = entry.path().extension().wstring();
                
                // Convert to lowercase for case-insensitive comparison
                std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), ::towlower);
                
                // Check if this file has one of the specified extensions
                for (const auto& ext : extensions)
                {
                    std::wstring lowerExt = ext;
                    std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), ::towlower);
                    
                    if (fileExt == lowerExt)
                    {
                        filePaths.push_back(filePath);
                        fileCount++;
                        break;
                    }
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        MessageBox(g_hwndMain, LoadStringResource(IDS_ERROR_SCANNING).c_str(), LoadStringResource(IDS_ERROR).c_str(), MB_OK | MB_ICONERROR);
        return;
    }

    // Now write the file paths to the binary index file
    FILE* indexFile = NULL;
    // We already have exePath and indexFilePath defined above
    _wfopen_s(&indexFile, indexFilePath.c_str(), L"wb");
    
    if (indexFile == NULL)
    {
        MessageBox(g_hwndMain, LoadStringResource(IDS_FAILED_CREATE_INDEX).c_str(), LoadStringResource(IDS_ERROR).c_str(), MB_OK | MB_ICONERROR);
        return;
    }
    
    // Write the number of files first
    uint32_t numFiles = static_cast<uint32_t>(filePaths.size());
    fwrite(&numFiles, sizeof(uint32_t), 1, indexFile);
    
    // Write each file path
    for (const auto& path : filePaths)
    {
        // Write the length of the string first (in characters)
        uint32_t length = static_cast<uint32_t>(path.length());
        fwrite(&length, sizeof(uint32_t), 1, indexFile);
        
        // Write the string data
        fwrite(path.c_str(), sizeof(wchar_t), length, indexFile);
    }
    
    fclose(indexFile);

    // Update the indexed file count
    g_indexedFileCount = fileCount;
    
    // Update the folder info display
    // Update the folder info to show the new file count
    UpdateFolderInfo();
    
    // Show a success message
    wchar_t successMsg[256];
    std::wstring successTemplate = LoadStringResource(IDS_INDEX_SUCCESS);
    swprintf_s(successMsg, 256, successTemplate.c_str(), fileCount);
    MessageBox(g_hwndMain, successMsg, LoadStringResource(IDS_SUCCESS).c_str(), MB_OK | MB_ICONINFORMATION);
}

void OpenRandomFile()
{
    // Construct the full path to the index file using the executable directory
    std::wstring exePath = GetExecutablePath();
    std::wstring indexFilePath = exePath + L"\\" + INDEX_FILE_NAME;
    
    // Check if index file exists
    if (!std::filesystem::exists(indexFilePath))
    {
        MessageBox(g_hwndMain, LoadStringResource(IDS_INDEX_NOT_FOUND).c_str(), LoadStringResource(IDS_ERROR).c_str(), MB_OK | MB_ICONERROR);
        return;
    }

    // Open the binary index file for reading
    FILE* indexFile = NULL;
    _wfopen_s(&indexFile, indexFilePath.c_str(), L"rb");
    
    if (indexFile == NULL)
    {
        MessageBox(g_hwndMain, LoadStringResource(IDS_FAILED_OPEN_INDEX).c_str(), LoadStringResource(IDS_ERROR).c_str(), MB_OK | MB_ICONERROR);
        return;
    }
    
    // Read the number of files
    uint32_t numFiles = 0;
    if (fread(&numFiles, sizeof(uint32_t), 1, indexFile) != 1)
    {
        MessageBox(g_hwndMain, LoadStringResource(IDS_FAILED_READ_INDEX).c_str(), LoadStringResource(IDS_ERROR).c_str(), MB_OK | MB_ICONERROR);
        fclose(indexFile);
        return;
    }
    
    if (numFiles == 0)
    {
        MessageBox(g_hwndMain, LoadStringResource(IDS_NO_FILES_FOUND).c_str(), LoadStringResource(IDS_ERROR).c_str(), MB_OK | MB_ICONERROR);
        fclose(indexFile);
        return;
    }
    
    // Read all file paths
    std::vector<std::wstring> filePaths;
    filePaths.reserve(numFiles);
    
    for (uint32_t i = 0; i < numFiles; i++)
    {
        // Read the length of the string
        uint32_t length = 0;
        if (fread(&length, sizeof(uint32_t), 1, indexFile) != 1)
        {
            MessageBox(g_hwndMain, LoadStringResource(IDS_ERROR_READING_INDEX).c_str(), LoadStringResource(IDS_ERROR).c_str(), MB_OK | MB_ICONERROR);
            fclose(indexFile);
            return;
        }
        
        // Read the string data
        std::vector<wchar_t> buffer(length + 1, 0); // +1 for null terminator
        if (fread(buffer.data(), sizeof(wchar_t), length, indexFile) != length)
        {
            MessageBox(g_hwndMain, LoadStringResource(IDS_ERROR_READING_INDEX).c_str(), LoadStringResource(IDS_ERROR).c_str(), MB_OK | MB_ICONERROR);
            fclose(indexFile);
            return;
        }
        
        // Add the string to our vector
        filePaths.push_back(std::wstring(buffer.data(), length));
    }
    
    fclose(indexFile);

    // Select a random file
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

void OpenContainingFolder()
{
    if (g_currentFilePath.empty())
    {
        MessageBox(g_hwndMain, LoadStringResource(IDS_NO_FILE_OPEN).c_str(), LoadStringResource(IDS_ERROR).c_str(), MB_OK | MB_ICONERROR);
        return;
    }
    
    std::filesystem::path path(g_currentFilePath);
    std::wstring folderPath = path.parent_path().wstring();
    
    ShellExecute(NULL, L"explore", folderPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
