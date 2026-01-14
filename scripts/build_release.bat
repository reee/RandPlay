@echo off
setlocal EnableDelayedExpansion

REM ============================================================
REM RandPlay Release Build Script
REM ============================================================

REM Set project paths
set "SCRIPT_DIR=%~dp0"
set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"
pushd "%SCRIPT_DIR%\.."
set "PROJECT_ROOT=%CD%"
popd

set "BUILD_DIR=%PROJECT_ROOT%\build"
set "BIN_DIR=%BUILD_DIR%\bin"
set "PROJECT_VERSION=1.0.0"

REM Color codes (Windows 10+)
set "COLOR_RESET=[0m"
set "COLOR_CYAN=[96m"
set "COLOR_GREEN=[92m"
set "COLOR_RED=[91m"
set "COLOR_WHITE=[97m"
set "COLOR_BLUE=[94m"

REM Print header
echo.
echo %COLOR_CYAN%=============================================================%COLOR_RESET%
echo %COLOR_CYAN%RandPlay - Release Build Script%COLOR_RESET%
echo %COLOR_CYAN%=============================================================%COLOR_RESET%
echo.

echo %COLOR_BLUE%[INFO] Preparing release build and distribution%COLOR_RESET%
echo.

REM Clean directories
echo %COLOR_WHITE%[1] Cleaning build directories%COLOR_RESET%
if exist "%BUILD_DIR%" (
    echo %COLOR_BLUE%[INFO] Removing directory: %BUILD_DIR%%COLOR_RESET%
    rmdir /s /q "%BUILD_DIR%" 2>nul
)
if exist "%PROJECT_ROOT%\dist" (
    echo %COLOR_BLUE%[INFO] Removing directory: %PROJECT_ROOT%\dist%COLOR_RESET%
    rmdir /s /q "%PROJECT_ROOT%\dist" 2>nul
)
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if not exist "%PROJECT_ROOT%\dist" mkdir "%PROJECT_ROOT%\dist"

REM Check if cmake is available
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo %COLOR_RED%[ERROR] cmake not found. Please ensure it is installed.%COLOR_RESET%
    pause
    exit /b 1
)

REM Generate build files
echo %COLOR_WHITE%[2] Generating build files%COLOR_RESET%
cmake -B "%BUILD_DIR%" -A x64
if %errorlevel% neq 0 (
    echo %COLOR_RED%[ERROR] Failed to generate build files%COLOR_RESET%
    pause
    exit /b 1
)

REM Build Release version
echo %COLOR_WHITE%[3] Building Release version%COLOR_RESET%
cmake --build "%BUILD_DIR%" --config Release
if %errorlevel% neq 0 (
    echo %COLOR_RED%[ERROR] Build failed%COLOR_RESET%
    pause
    exit /b 1
)

REM Prepare distribution
echo %COLOR_WHITE%[4] Preparing distribution package%COLOR_RESET%
set "RELEASE_DIR=%PROJECT_ROOT%\dist\RandPlay_%PROJECT_VERSION%"
if not exist "%RELEASE_DIR%" mkdir "%RELEASE_DIR%"

REM Copy executable files
if exist "%BIN_DIR%\Release\RandPlay_EN.exe" (
    copy "%BIN_DIR%\Release\RandPlay_EN.exe" "%RELEASE_DIR%\" >nul
    echo %COLOR_GREEN%[OK] Copied English version%COLOR_RESET%
)

if exist "%BIN_DIR%\Release\RandPlay_ZH_CN.exe" (
    copy "%BIN_DIR%\Release\RandPlay_ZH_CN.exe" "%RELEASE_DIR%\" >nul
    echo %COLOR_GREEN%[OK] Copied Chinese version%COLOR_RESET%
)

REM Copy resource files
if exist "%BIN_DIR%\Release\RandPlay.ico" (
    copy "%BIN_DIR%\Release\RandPlay.ico" "%RELEASE_DIR%\" >nul
)

if exist "%PROJECT_ROOT%\README.md" (
    copy "%PROJECT_ROOT%\README.md" "%RELEASE_DIR%\" >nul
)

if exist "%PROJECT_ROOT%\LICENSE" (
    copy "%PROJECT_ROOT%\LICENSE" "%RELEASE_DIR%\" >nul
)

REM Generate version info file
echo %COLOR_BLUE%[INFO] Generating version info file%COLOR_RESET%

REM Get current timestamp
for /f "tokens=2 delims==" %%i in ('wmic OS Get localdatetime /value') do set "dt=%%i"
set "timestamp=%dt:~0,4%-%dt:~4,2%-%dt:~6,2% %dt:~8,2%:%dt:~10,2%:%dt:~12,2%"

(
echo RandPlay %PROJECT_VERSION% - Release Build
echo ==========================================
echo.
echo Build Information:
echo   Date: %timestamp%
echo   Configuration: Release
echo   Platform: Windows x64
echo.
echo Included Files:
echo   RandPlay_EN.exe - English Version
echo   RandPlay_ZH_CN.exe - Chinese Version
echo   RandPlay.ico - Application Icon
echo   README.md - Project Documentation
echo   LICENSE - License Information
echo.
echo System Requirements:
echo   - Windows 10 or later
echo   - .NET Framework 4.7.2 or later
echo.
echo For more information, visit: https://github.com/yourname/RandPlay
) > "%RELEASE_DIR%\VERSION.txt"

REM Generate usage instructions file
echo %COLOR_BLUE%[INFO] Generating usage instructions file%COLOR_RESET%
(
echo RandPlay - Random File Player
echo =============================
echo.
echo OVERVIEW:
echo   RandPlay is a random file player that helps you discover
echo   and play files from your collection in a random order.
echo.
echo LANGUAGE VERSIONS:
echo   This package contains two language versions:
echo.
echo   ENGLISH VERSION:
echo     File: RandPlay_EN.exe
echo     Description: Full English interface
echo.
echo   CHINESE VERSION:
echo     File: RandPlay_ZH_CN.exe
echo     Description: Simplified Chinese interface
echo.
echo GETTING STARTED:
echo   1. Choose your preferred language version
echo   2. Run the corresponding .exe file
echo   3. Go to File ^> Settings to configure directories
echo   4. Build the file index
echo   5. Enjoy random file playback
echo.
echo FEATURES:
echo   - Random file selection from configured directories
echo   - Multi-language support ^(English and Chinese^)
echo   - Customizable file filters
echo   - Fast file indexing
echo   - Portable application ^(no installation required^)
echo.
echo SYSTEM REQUIREMENTS:
echo   - Windows 10 or later
echo   - Administrative rights may be required for some directories
echo.
echo SUPPORT:
echo   For issues, feature requests, or documentation,
echo   please refer to the README.md file or visit our
echo   project repository.
echo.
echo Enjoy using RandPlay!
) > "%RELEASE_DIR%\HOW_TO_USE.txt"

echo %COLOR_WHITE%[5] Complete%COLOR_RESET%

REM Display summary
echo.
echo %COLOR_GREEN%[OK] Release package created successfully%COLOR_RESET%
echo %COLOR_BLUE%[INFO] Release directory: %RELEASE_DIR%%COLOR_RESET%

if exist "%RELEASE_DIR%\RandPlay_EN.exe" (
    for %%f in ("%RELEASE_DIR%\RandPlay_EN.exe") do (
        echo %COLOR_BLUE%[INFO] RandPlay_EN.exe - %%~zf bytes%COLOR_RESET%
    )
)

if exist "%RELEASE_DIR%\RandPlay_ZH_CN.exe" (
    for %%f in ("%RELEASE_DIR%\RandPlay_ZH_CN.exe") do (
        echo %COLOR_BLUE%[INFO] RandPlay_ZH_CN.exe - %%~zf bytes%COLOR_RESET%
    )
)

echo.
echo %COLOR_BLUE%[INFO] Open release directory? (Y/N)%COLOR_RESET%
set /p open_dir=">"
if /i "%open_dir%"=="y" (
    start "" "%RELEASE_DIR%"
)

echo.
pause
