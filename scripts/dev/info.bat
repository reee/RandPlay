@echo off
echo RandPlay Project Information
echo ============================
echo.

:: Project info
echo Project: RandPlay - Random File Player
echo Version: 1.0.0
echo Build System: CMake
echo.

:: Check builds
echo Build Status:
echo -------------

if exist "build\bin\Release\RandPlay.exe" (
    echo [✓] Release build available
    for %%i in ("build\bin\Release\RandPlay.exe") do echo     Size: %%~zi bytes
    for %%i in ("build\bin\Release\RandPlay.exe") do echo     Date: %%~ti
) else (
    echo [✗] Release build not found
)

if exist "build\bin\Debug\RandPlay.exe" (
    echo [✓] Debug build available
    for %%i in ("build\bin\Debug\RandPlay.exe") do echo     Size: %%~zi bytes
    for %%i in ("build\bin\Debug\RandPlay.exe") do echo     Date: %%~ti
    if exist "build\bin\Debug\RandPlay.pdb" (
        echo     Debug symbols: Available
    )
) else (
    echo [✗] Debug build not found
)

echo.

:: CMake info
echo CMake Information:
echo -----------------
if exist "build\CMakeCache.txt" (
    echo [✓] CMake project configured
    findstr "CMAKE_BUILD_TYPE" build\CMakeCache.txt 2>nul
    findstr "CMAKE_CXX_COMPILER_ID" build\CMakeCache.txt 2>nul
) else (
    echo [✗] CMake project not configured
)

echo.

:: Available scripts
echo Available Scripts:
echo -----------------
echo build_cmake_auto.bat [debug^|release] - Auto-detect CMake and build
echo cmake_build.bat [debug^|release]      - Build with specific CMake path
echo quick.bat [debug^|release]            - Quick build and run
echo run.bat                              - Run the application
echo clean.bat                           - Clean build files
echo open_vs.bat                         - Open in Visual Studio
echo info.bat                            - Show this information

echo.
pause
