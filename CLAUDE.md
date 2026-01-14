# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

RandPlay is a Windows desktop application (C++ with Win32 API) that randomly opens files from a user-selected directory. The application builds an index of files matching specified extensions (video, image, or custom) and provides a simple interface to randomly open files using the system's default application.

## Build Commands


## Architecture

### Module Structure

The codebase follows a modular architecture with clear separation of concerns:

```
src/
├── main.cpp                 # Win32 main entry point, window procedure, message loop
├── core/                    # Business logic (no UI code)
│   ├── FileIndexer.cpp/h    # Directory scanning, index file I/O (index.dat)
│   ├── Settings.cpp/h       # Settings persistence (settings.dat)
│   └── Utils.cpp/h          # Path utilities, DPI scaling, resource loading
├── ui/                      # UI-specific code
│   └── UIHelpers.cpp/h      # Control initialization, dialogs, event handlers
└── constants/
    ├── AppConstants.h       # Application-wide constants
    └── ResourceIds.h        # Resource IDs (menus, dialogs, controls, strings)
```

### Global State Management

The application uses global singletons allocated in `main.cpp`:
- `g_settingsManager` - Manages user settings (directory, file type preferences)
- `g_fileIndexer` - Handles file indexing and storage
- `g_hwndMain`, `g_hwndRecentFilesList`, `g_hwndFolderInfoStatic` - UI window handles
- `g_currentFilePath`, `g_recentFiles` - Runtime state

These are accessed via `extern` declarations in [UIHelpers.h](src/ui/UIHelpers.h#L30-L38).

### Language-Specific Builds

The project builds two separate executables with different resource files:
- **RandPlay_EN**: English UI ([RandPlay_en.rc](resources/RandPlay_en.rc))
- **RandPlay_ZH_CN**: Chinese UI ([RandPlay_zh_cn.rc](resources/RandPlay_zh_cn.rc))

Both share the same C++ source code. String resources are loaded via `Utils::LoadStringResource()`.

### Data Flow

1. **Settings Dialog**: User selects directory and file type → `SettingsManager` saves to `settings.dat`
2. **Build Index**: `FileIndexer::BuildIndex()` scans directory recursively → writes to `index.dat`
3. **Open Random**: `FileIndexer` reads random entry from index → `ShellExecute()` opens with default app
4. **Recent Files**: `UIHelpers::AddToRecentFiles()` tracks opened files → displays in ListView

### Key Implementation Details

- **DPI Scaling**: All UI dimensions use `Utils::ScaleDPI()` for high-DPI display support
- **Resource Encoding**: CMake sets `/l 0x804 /c 65001` for Chinese character support in resources
- **Memory Management**: ListView item data must be freed in `WM_DESTROY` (see [main.cpp:207-221](src/main.cpp#L207-L221))
- **File Storage**: Both index and settings files are stored alongside the executable

## Development Notes

### Adding New Features

1. **UI Changes**: Modify UIHelpers for logic, update both .rc files for strings/controls
2. **New Extensions**: Add to AppConstants arrays, update both resource string tables
3. **New Language**: Create new .rc file, add CMake target following RandPlay_EN pattern

### Resource ID Management

All resource IDs are centrally defined in [ResourceIds.h](src/constants/ResourceIds.h):
- Icon/Menu: 100-299
- Dialogs: 300-399
- Controls: 100-199 (must not conflict with dialog IDs)
- Strings: 1000+

### Common Pitfalls

- Always delete ListView item data in `WM_DESTROY` to prevent memory leaks
- Use wide-string versions of Win32 APIs (e.g., `CreateWindowExW`, not `CreateWindowExA`)
- When adding controls, update both `ResourceIds.h` and both language .rc files
- Index file format must maintain backward compatibility if modified

### Testing Changes

修改后，提示用户自行运行编译命令进行编译

## Environment Requirements

- **OS**: Windows 10 or higher
- **Build**: CMake 3.16+, Visual Studio 2019+ (or Build Tools)
- **C++ Standard**: C++17
- **Windows SDK**: Windows 10 SDK or later
