# AGENTS.md

Guidance for coding agents working on RandPlay.

## Project Overview

RandPlay is a Windows desktop application (C++17 / Win32 API) that randomly opens files from a user-selected directory using the system's default application. It indexes files matching video, image, or custom extensions and exposes a simple GUI: build index, open a random file, view recent files.

Target: Windows 10+, CMake 3.16+, Visual Studio 2019+ (or Build Tools), Windows 10 SDK.

## Build Commands

The canonical build is CMake. Configure and build from the repo root:

```
cmake -B build -A x64
cmake --build build --config Release
```

Use `--config Debug` for a debug build. Outputs land in `build/bin/{Debug,Release}/`.

The build produces **three targets that must ship together** in the same directory:
- `RandPlay.exe` — the GUI application
- `RandPlay_en.dll` — English resource DLL
- `RandPlay_zh_cn.dll` — Simplified Chinese resource DLL

The EXE loads language resources from the DLLs at runtime via `LoadLibraryExW`; if a DLL is missing it falls back to English DLL, then to resources embedded in the EXE.

There is **no automated test suite**. After code changes, do not attempt to compile yourself — ask the user to run the build.

> Note: `scripts/build.bat`, `scripts/build_release.bat`, and `README.md` still reference an obsolete two-executable build (`RandPlay_EN.exe` / `RandPlay_ZH_CN.exe`) that no longer exists. Do not trust those files for build commands; use CMake directly as shown above.

## Architecture

### Module layout

```
src/
├── main.cpp                  # WinMain, WindowProc, message loop, global state definitions
├── constants/
│   ├── AppConstants.h        # Class name, file names, extension arrays, Language/Hotkey enums
│   └── ResourceIds.h         # All resource IDs (icons, menus, dialogs, controls, strings)
├── core/                     # Business logic, no UI
│   ├── Utils.{h,cpp}         # Path helpers, DPI scaling, font creation, LoadStringResource
│   ├── Settings.{h,cpp}      # SettingsManager: persists settings.dat (binary struct dump)
│   ├── FileIndexer.{h,cpp}   # Recursive scan, index.dat read/write
│   ├── LanguageManager.{h,cpp}  # Resource DLL loading, system-language detection
│   ├── HotkeyManager.{h,cpp}   # Global hotkey register/unregister, display strings
│   └── ResourceDLL.cpp       # DllMain stub for the resource-only DLLs
└── ui/
    └── UIHelpers.{h,cpp}     # Control init, settings dialog, recent-files list, context menu, hotkey capture

resources/
├── RandPlay.rc               # EXE resources (version info, icon, embedded English fallback)
├── RandPlay_en.rc            # English resource DLL (menu + dialogs + #include strings/lang_en.rc)
├── RandPlay_zh_cn.rc         # Chinese resource DLL (0x804 codepage, Chinese strings)
├── icons/RandPlay.ico
└── strings/{lang_en,lang_zh_cn}.rc  # String tables

include/pch.h                 # Precompiled header; defines UNICODE, links comctl32/shlwapi/shcore
```

### Global state

Globals are defined in `main.cpp` and declared `extern` in `ui/UIHelpers.h`:
- UI handles: `g_hwndMain`, `g_hwndRecentFilesList`, `g_hwndFolderInfoStatic`, `g_hwndBuildIndexButton`, `g_hwndOpenRandomButton`
- Runtime state: `g_currentFilePath` (std::wstring), `g_recentFiles` (vector, capped at `MAX_RECENT_FILES`=100)
- Manager singletons (`new` in WinMain, `delete` after the message loop): `g_settingsManager` (SettingsManager*), `g_fileIndexer` (FileIndexer*)
- `LanguageManager` and `HotkeyManager` are fully static classes (no instances).

### Data flow

1. Settings dialog → `SettingsManager` writes `settings.dat`.
2. Build Index → `FileIndexer::BuildIndex()` recursively scans the directory → writes `index.dat`.
3. Open Random / global hotkey → `FileIndexer` returns a random entry → `ShellExecuteW` opens it.
4. Opened files tracked via `UIHelpers::AddToRecentFiles()` → shown in the ListView (with right-click context menu: Open File / Open Folder).

### Data file formats

- **`settings.dat`** — raw `sizeof(Settings)` binary dump, located in the **current working directory** (`Utils::GetAppDataPath()` returns `std::filesystem::current_path()`). `Settings.cpp` reads with a `gcount()` size check for backward compatibility: older/smaller files default language to auto-detect and the hotkey to Ctrl+Shift+R. If you add fields to `struct Settings`, preserve this size-based migration logic.
- **`index.dat`** — binary, located in the **executable's directory** (`Utils::GetExecutablePath()`). Format: `uint32_t numFiles`, then per file `uint32_t length` (in `wchar_t` units, not bytes) followed by `length * sizeof(wchar_t)` of UTF-16 data (no null terminator). Keep this format backward-compatible if modified.

> Inconsistency to be aware of: the two files live in different directories (CWD vs. exe dir). They diverge if the app is launched from a different working directory.

### Language architecture

A single EXE plus resource-only DLLs (not separate per-language executables). `LanguageManager::LoadLanguage()` loads the matching DLL from the exe directory; strings/menus/dialogs are then loaded from it. Language can be switched live from the settings dialog (`UIHelpers::ReloadAllUIText`).

## Conventions

### Win32 / C++

- Always use wide-string W-suffix APIs (`CreateWindowExW`, `ShellExecuteW`, …) and `L""` / `std::wstring`. `UNICODE`/`_UNICODE` are defined in `include/pch.h`.
- Source files are UTF-8; MSVC is invoked with `/utf-8`. Resource `.rc` files are UTF-16 LE with `/l 0x804 /c 65001` for Chinese.
- All UI dimensions go through `Utils::ScaleDPI()` (MulDiv by 96 dpi). Fonts via `Utils::CreateUIFont`/`CreateUIFontForDPI` ("Microsoft YaHei UI", 9pt, `GB2312_CHARSET`). Handle `WM_DPICHANGED` when laying out.
- Free ListView item `lParam` data in `WM_DESTROY` to avoid leaks.

### Resources

Resource IDs are centrally defined in `src/constants/ResourceIds.h`. Ranges (approximate, not strictly enforced):
- Icon/Menu: 100s (`IDI_*`, `IDR_*`), Menu commands: 200s (`IDM_*`)
- Dialogs: 300s (`IDD_*`)
- Controls: 100s (`IDC_*`) — these intentionally overlap icon/menu IDs; they are scoped per-dialog/resource-type and do not collide at runtime.
- Strings: 1000+ (`IDS_*`)

When adding or changing any user-facing string, control, or dialog, update **all three** places: `ResourceIds.h`, `resources/RandPlay_en.rc` + `resources/strings/lang_en.rc`, and `resources/RandPlay_zh_cn.rc` + `resources/strings/lang_zh_cn.rc`. Both languages must stay in sync.

### Adding features

- **UI logic** → `UIHelpers`; **business logic** → a `core/` module.
- **New file extension type** → add to the arrays in `AppConstants.h` and the string tables in both languages.
- **New language** → create `resources/RandPlay_<lang>.rc` + `resources/strings/lang_<lang>.rc`, add a `Language` enum value in `AppConstants.h`, and a new shared-library target in `CMakeLists.txt` following the `RandPlay_en` / `RandPlay_zh_cn` pattern.

## Reference

- `CMAKE_CONFIG.md` — compiler/linker options and build structure (slightly stale on linked libraries; CMakeLists.txt is authoritative).
- `agent_docs/win32-cn.md` — design notes on Chinese-language and HiDPI support in Win32.
