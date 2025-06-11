# RandPlay Project Configuration

This project uses CMake for building. Below are the configuration options and build information.

## CMake Configuration

### Build Types
- **Debug**: No optimization, debug symbols, _DEBUG defined
- **Release**: Optimized for speed, NDEBUG defined (default)

### Compiler Options (MSVC)
- C++17 standard
- Exception handling enabled (/EHsc)
- Warning level 3 (/W3)
- Strict conformance mode (/permissive-)

### Dependencies
- Windows API libraries:
  - user32.lib - User interface functions
  - gdi32.lib - Graphics Device Interface
  - comctl32.lib - Common Controls
  - shell32.lib - Shell functions
  - ole32.lib - OLE functions
  - shlwapi.lib - Shell lightweight utility functions

### Build Structure
```
project_root/
├── build/                  # CMake build directory
│   ├── bin/               # Output executables
│   │   ├── Debug/         # Debug builds
│   │   └── Release/       # Release builds
│   └── CMakeFiles/        # CMake internal files
├── CMakeLists.txt         # CMake configuration
└── source files...
```

### Build Commands
- Generate: `cmake ..`
- Build Debug: `cmake --build . --config Debug`
- Build Release: `cmake --build . --config Release`
- Clean: `cmake --build . --target clean`

### Visual Studio Integration
The project generates Visual Studio solution files (.sln) that can be opened directly in Visual Studio for development and debugging.
