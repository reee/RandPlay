#include "HotkeyManager.h"
#include "Settings.h"
#include "../ui/UIHelpers.h"
#include "Utils.h"
#include "../constants/ResourceIds.h"

// Static member initialization
UINT HotkeyManager::s_modifiers = MOD_CONTROL | MOD_SHIFT;
UINT HotkeyManager::s_key = 'R';
bool HotkeyManager::s_enabled = true;
int HotkeyManager::s_hotkeyId = AppConstants::Hotkey::ID_OPEN_RANDOM;
bool HotkeyManager::s_registered = false;

void HotkeyManager::Initialize() {
    // Load hotkey configuration from settings
    UINT modifiers, key;
    bool enabled;
    g_settingsManager->GetHotkeyConfig(modifiers, key, enabled);

    s_modifiers = modifiers;
    s_key = key;
    s_enabled = enabled;
}

bool HotkeyManager::RegisterGlobalHotkey(HWND hwnd) {
    if (!s_enabled || s_key == AppConstants::Hotkey::DISABLED) {
        return false;
    }

    // Unregister existing hotkey if any
    if (s_registered) {
        UnregisterGlobalHotkey(hwnd);
    }

    // Register the new hotkey
    BOOL result = RegisterHotKey(hwnd, s_hotkeyId, s_modifiers, s_key);
    if (result) {
        s_registered = true;
        return true;
    } else {
        // Failed to register (likely conflict with another application)
        DWORD error = GetLastError();
        if (error == ERROR_HOTKEY_ALREADY_REGISTERED) {
            std::wstring errorMsg = Utils::LoadStringResource(IDS_HOTKEY_CONFLICT);
            std::wstring errorTitle = Utils::LoadStringResource(IDS_ERROR);
            MessageBox(hwnd, errorMsg.c_str(), errorTitle.c_str(), MB_OK | MB_ICONWARNING);
        }
        return false;
    }
}

void HotkeyManager::UnregisterGlobalHotkey(HWND hwnd) {
    if (s_registered) {
        UnregisterHotKey(hwnd, s_hotkeyId);
        s_registered = false;
    }
}

bool HotkeyManager::ReregisterHotkey(HWND hwnd) {
    // Unregister first
    UnregisterGlobalHotkey(hwnd);

    // Register with current configuration
    return RegisterGlobalHotkey(hwnd);
}

void HotkeyManager::OnHotkey(HWND hwnd, int hotkeyId) {
    if (hotkeyId == s_hotkeyId) {
        // Trigger open random file
        UIHelpers::OpenRandomFile();
    }
}

std::wstring HotkeyManager::GetHotkeyString() {
    if (!s_enabled || s_key == AppConstants::Hotkey::DISABLED) {
        return Utils::LoadStringResource(IDS_HOTKEY_DISABLED);
    }

    std::wstring result;

    // Add modifier keys in a consistent order
    if (s_modifiers & MOD_CONTROL) result += L"Ctrl+";
    if (s_modifiers & MOD_SHIFT) result += L"Shift+";
    if (s_modifiers & MOD_ALT) result += L"Alt+";
    if (s_modifiers & MOD_WIN) result += L"Win+";

    // Convert virtual key code to key name
    if (s_key >= 'A' && s_key <= 'Z') {
        result += (wchar_t)s_key;
    } else if (s_key >= '0' && s_key <= '9') {
        result += (wchar_t)s_key;
    } else if (s_key >= VK_F1 && s_key <= VK_F24) {
        result += L"F" + std::to_wstring(s_key - VK_F1 + 1);
    } else if (s_key == VK_SPACE) {
        result += L"Space";
    } else if (s_key == VK_RETURN) {
        result += L"Enter";
    } else if (s_key == VK_ESCAPE) {
        result += L"Esc";
    } else if (s_key == VK_TAB) {
        result += L"Tab";
    } else if (s_key == VK_BACK) {
        result += L"Back";
    } else if (s_key == VK_DELETE) {
        result += L"Del";
    } else if (s_key == VK_INSERT) {
        result += L"Ins";
    } else if (s_key == VK_HOME) {
        result += L"Home";
    } else if (s_key == VK_END) {
        result += L"End";
    } else if (s_key == VK_PRIOR) {
        result += L"PgUp";
    } else if (s_key == VK_NEXT) {
        result += L"PgDn";
    } else {
        // Fallback: show virtual key code
        result += L"VK_" + std::to_wstring(s_key);
    }

    return result;
}

void HotkeyManager::SetHotkey(UINT modifiers, UINT key, bool enabled) {
    s_modifiers = modifiers;
    s_key = key;
    s_enabled = enabled;
}

void HotkeyManager::GetHotkey(UINT& modifiers, UINT& key, bool& enabled) {
    modifiers = s_modifiers;
    key = s_key;
    enabled = s_enabled;
}
