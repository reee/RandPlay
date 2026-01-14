#include "../include/pch.h"
#include "constants/ResourceIds.h"
#include "constants/AppConstants.h"
#include "core/Utils.h"
#include "core/Settings.h"
#include "core/FileIndexer.h"
#include "core/LanguageManager.h"
#include "ui/UIHelpers.h"

// Global variables
HWND g_hwndMain = NULL;
HWND g_hwndRecentFilesList = NULL;
HWND g_hwndFolderInfoStatic = NULL;
HWND g_hwndBuildIndexButton = NULL;
HWND g_hwndOpenRandomButton = NULL;
std::wstring g_currentFilePath;
std::vector<std::wstring> g_recentFiles;

// Global instances
SettingsManager* g_settingsManager = nullptr;
FileIndexer* g_fileIndexer = nullptr;

// Forward declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Set DPI awareness to improve text rendering
    // Try Per-Monitor V2 (Windows 10 1703+)
    if (!SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)) {
        // Fallback to Per-Monitor (Windows 8.1+)
        HMODULE hShcore = LoadLibrary(L"shcore.dll");
        if (hShcore) {
            typedef HRESULT (WINAPI *SetProcessDpiAwarenessFunc)(PROCESS_DPI_AWARENESS);
            SetProcessDpiAwarenessFunc pSetProcessDpiAwareness =
                (SetProcessDpiAwarenessFunc)GetProcAddress(hShcore, "SetProcessDpiAwareness");
            if (pSetProcessDpiAwareness) {
                pSetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
            }
            FreeLibrary(hShcore);
        }
        // Final fallback to Windows Vista/7
        if (!SetProcessDPIAware()) {
            // Failed to set DPI awareness, continue anyway
        }
    }

    // Enable visual styles for modern look
    Utils::EnableVisualStyles();
    
    // Initialize common controls
    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icc.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icc);

    // Initialize global managers
    g_settingsManager = new SettingsManager();

    // Initialize language system (must be after settings, before UI)
    LanguageManager::Initialize();

    g_fileIndexer = new FileIndexer();

    // Register the window class with improved appearance
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = AppConstants::CLASS_NAME;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.style = CS_HREDRAW | CS_VREDRAW;

    RegisterClass(&wc);

    // Scale window size based on DPI
    int windowWidth = Utils::ScaleDPI(300);
    int windowHeight = Utils::ScaleDPI(300);
    
    // Create the window
    g_hwndMain = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        AppConstants::CLASS_NAME,
        Utils::LoadStringResource(IDS_APP_TITLE).c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight,
        NULL, NULL, hInstance, NULL
    );

    if (g_hwndMain == NULL) {
        delete g_fileIndexer;
        delete g_settingsManager;
        return 0;
    }

    ShowWindow(g_hwndMain, nCmdShow);
    UpdateWindow(g_hwndMain);

    // Message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup
    delete g_fileIndexer;
    delete g_settingsManager;
    
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        {
            // Create menu - try resource DLL first, then EXE
            HMENU hMenu = NULL;
            HMODULE hResourceDLL = LanguageManager::GetResourceDLL();

            if (hResourceDLL) {
                hMenu = LoadMenu(hResourceDLL, MAKEINTRESOURCE(IDR_MAINMENU));
            }

            if (!hMenu) {
                hMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MAINMENU));
            }

            if (hMenu) {
                SetMenu(hwnd, hMenu);
            }

            // Set a light gray background color for the window
            SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)GetSysColorBrush(COLOR_BTNFACE));            // Initialize controls
            UIHelpers::InitializeControls(hwnd);

            // Load index count to initialize the file counter
            g_fileIndexer->LoadIndexCount();

            // Update folder info
            UIHelpers::UpdateFolderInfo();

            // Update recent files list
            UIHelpers::UpdateRecentFilesList();
            return 0;
        }

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {        case IDM_SETTINGS:
            UIHelpers::ShowSettingsDialog();
            break;
            
        case IDM_EXIT:
            DestroyWindow(hwnd);
            break;
            
        case IDC_BUILD_INDEX_BUTTON:
            UIHelpers::BuildIndex();
            break;
            
        case IDC_OPEN_RANDOM_BUTTON:
            UIHelpers::OpenRandomFile();
            break;
        }
        break;

    case WM_NOTIFY:
        {
            NMHDR* pnmhdr = (NMHDR*)lParam;
            if (pnmhdr->idFrom == IDC_RECENT_FILES_LIST && pnmhdr->code == NM_RCLICK)
            {
                // Handle right-click on ListView
                NMITEMACTIVATE* pnmia = (NMITEMACTIVATE*)lParam;
                int index = pnmia->iItem;
                if (index != -1)
                {
                    // Get the mouse position from the notification
                    POINT pt = {pnmia->ptAction.x, pnmia->ptAction.y};
                    ClientToScreen(g_hwndRecentFilesList, &pt);
                    UIHelpers::ShowListContextMenu(pt.x, pt.y, index);
                }
                return 0;
            }
        }
        break;

    case WM_DPICHANGED:
        {
            // DPI has changed - this happens when window moves to a different DPI monitor
            int dpiX = LOWORD(wParam);
            int dpiY = HIWORD(wParam);

            // Recreate fonts with new DPI
            HFONT hNewFont = Utils::CreateUIFontForDPI(dpiY);

            // Update font for all controls
            if (g_hwndBuildIndexButton) {
                SendMessage(g_hwndBuildIndexButton, WM_SETFONT, (WPARAM)hNewFont, TRUE);
            }
            if (g_hwndOpenRandomButton) {
                SendMessage(g_hwndOpenRandomButton, WM_SETFONT, (WPARAM)hNewFont, TRUE);
            }
            if (g_hwndRecentFilesList) {
                SendMessage(g_hwndRecentFilesList, WM_SETFONT, (WPARAM)hNewFont, TRUE);
            }
            if (g_hwndFolderInfoStatic) {
                SendMessage(g_hwndFolderInfoStatic, WM_SETFONT, (WPARAM)hNewFont, TRUE);
            }

            // Resize and reposition window using the suggested rect
            RECT* pSuggestedRect = (RECT*)lParam;
            SetWindowPos(hwnd, NULL,
                pSuggestedRect->left, pSuggestedRect->top,
                pSuggestedRect->right - pSuggestedRect->left,
                pSuggestedRect->bottom - pSuggestedRect->top,
                SWP_NOZORDER | SWP_NOACTIVATE);

            return 0;
        }

    case WM_SIZE:
        {
            // Handle window resizing
            RECT rcClient;
            GetClientRect(hwnd, &rcClient);
            int width = rcClient.right - rcClient.left;
            int height = rcClient.bottom - rcClient.top;
            
            if (g_hwndRecentFilesList && g_hwndFolderInfoStatic)
            {
                int margin = Utils::ScaleDPI(10);
                int buttonHeight = Utils::ScaleDPI(30);
                int controlHeight = Utils::ScaleDPI(24);
                int bottomInfoHeight = controlHeight;
                int buttonSpacing = Utils::ScaleDPI(5);
                
                // Position the recent files list
                SetWindowPos(g_hwndRecentFilesList, NULL, 
                    margin, margin + buttonHeight + buttonSpacing, 
                    width - margin * 2, height - margin * 2 - buttonHeight - bottomInfoHeight - buttonSpacing,
                    SWP_NOZORDER);
                
                // Position the folder info static at the bottom
                SetWindowPos(g_hwndFolderInfoStatic, NULL,
                    margin, height - margin - bottomInfoHeight, width - margin * 2, bottomInfoHeight,
                    SWP_NOZORDER);
                
                // Force redraw
                InvalidateRect(g_hwndRecentFilesList, NULL, TRUE);
                InvalidateRect(g_hwndFolderInfoStatic, NULL, TRUE);
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
