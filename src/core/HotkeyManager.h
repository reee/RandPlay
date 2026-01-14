#pragma once

#include <windows.h>
#include <string>
#include "../constants/AppConstants.h"

class HotkeyManager {
public:
    // Register global hotkey
    static bool RegisterGlobalHotkey(HWND hwnd);

    // Unregister global hotkey
    static void UnregisterGlobalHotkey(HWND hwnd);

    // Reregister hotkey (when configuration changes)
    static bool ReregisterHotkey(HWND hwnd);

    // Handle WM_HOTKEY message
    static void OnHotkey(HWND hwnd, int hotkeyId);

    // Get current hotkey as string (for display)
    static std::wstring GetHotkeyString();

    // Set hotkey configuration
    static void SetHotkey(UINT modifiers, UINT key, bool enabled);

    // Get hotkey configuration
    static void GetHotkey(UINT& modifiers, UINT& key, bool& enabled);

    // Check if hotkey is enabled
    static bool GetEnabled() { return s_enabled; }

    // Initialize from settings
    static void Initialize();

private:
    static UINT s_modifiers;
    static UINT s_key;
    static bool s_enabled;
    static int s_hotkeyId;
    static bool s_registered;
};
