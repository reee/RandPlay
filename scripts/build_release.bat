@echo off
setlocal EnableDelayedExpansion

REM ============================================================
REM RandPlay Release Build Script
REM ============================================================
REM Clean rebuild + package a distributable folder under dist\.
REM
REM Output: dist\RandPlay_{version}\ containing:
REM   RandPlay.exe, RandPlay_en.dll, RandPlay_zh_cn.dll,
REM   RandPlay.ico, README.md, LICENSE,
REM   VERSION.txt, HOW_TO_USE.txt
REM
REM Usage:
REM   scripts\build_release.bat
REM ============================================================

call "%~dp0functions.bat"
call :print_header "Release Build Script"
call :print_info "Preparing release build and distribution package"
echo.

REM --- locate CMake ---
if not defined CMAKE_CMD (
    call :print_error "cmake not found. Install CMake or Visual Studio 2019+ (Build Tools) and re-run."
    call :print_info  "Download: https://cmake.org/download/"
    pause & exit /b 1
)
call :print_info "Using CMake: %CMAKE_CMD%"
echo.

REM --- clean previous outputs ---
call :print_step 1 "Cleaning build directories"
call :safe_rmdir "%BUILD_DIR%"
call :safe_rmdir "%PROJECT_ROOT%\dist"
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%" 2>nul
if not exist "%PROJECT_ROOT%\dist" mkdir "%PROJECT_ROOT%\dist" 2>nul

REM --- generate build files ---
call :print_step 2 "Generating build files (x64)"
"%CMAKE_CMD%" -B "%BUILD_DIR%" -A x64 -S "%PROJECT_ROOT%"
if errorlevel 1 (
    call :print_error "Failed to generate build files"
    pause & exit /b 1
)

REM --- build Release ---
call :print_step 3 "Building Release"
"%CMAKE_CMD%" --build "%BUILD_DIR%" --config Release
if errorlevel 1 (
    call :print_error "Build failed"
    pause & exit /b 1
)

REM --- stage distribution ---
call :print_step 4 "Packaging distribution"
set "RELEASE_DIR=%PROJECT_ROOT%\dist\RandPlay_%PROJECT_VERSION%"
if not exist "%RELEASE_DIR%" mkdir "%RELEASE_DIR%" 2>nul
set "SRC_BIN=%BIN_DIR%\Release"

REM Core runtime files (EXE + language resource DLLs + icon)
set "PACKAGE_ERROR=0"
for %%f in ("RandPlay.exe" "RandPlay_en.dll" "RandPlay_zh_cn.dll" "RandPlay.ico") do (
    if exist "%SRC_BIN%\%%~f" (
        copy /y "%SRC_BIN%\%%~f" "%RELEASE_DIR%\" >nul
        call :print_success "Copied %%~f"
    ) else (
        call :print_error "Missing required build output: %%~f"
        set "PACKAGE_ERROR=1"
    )
)
REM Documentation
for %%f in ("README.md" "LICENSE") do (
    if exist "%PROJECT_ROOT%\%%~f" (
        copy /y "%PROJECT_ROOT%\%%~f" "%RELEASE_DIR%\" >nul
        call :print_success "Copied %%~f"
    ) else (
        call :print_warning "Missing optional file: %%~f"
    )
)
if "!PACKAGE_ERROR!"=="1" (
    call :print_error "Packaging incomplete due to missing build outputs"
    pause & exit /b 1
)

REM --- generate VERSION.txt ---
call :print_info "Generating VERSION.txt"
call :get_timestamp
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
    echo   RandPlay.exe        - Application executable
    echo   RandPlay_en.dll     - English language resources
    echo   RandPlay_zh_cn.dll  - Simplified Chinese language resources
    echo   RandPlay.ico        - Application icon
    echo   README.md           - Project documentation
    echo   LICENSE             - License information
    echo.
    echo System Requirements:
    echo   - Windows 10 or later
    echo.
    echo Repository: https://github.com/reee/RandPlay
) > "%RELEASE_DIR%\VERSION.txt"

REM --- generate HOW_TO_USE.txt ---
call :print_info "Generating HOW_TO_USE.txt"
(
    echo RandPlay - Random File Player
    echo =============================
    echo.
    echo OVERVIEW:
    echo   RandPlay randomly opens files from a directory you choose,
    echo   using the system's default application for each file.
    echo.
    echo GETTING STARTED:
    echo   1. Run RandPlay.exe ^(all three files must stay together^).
    echo   2. Go to File ^> Settings to choose a directory and file types.
    echo   3. Switch language in the same dialog ^(Auto / English / Chinese^).
    echo   4. Build the file index, then click "Open Random" - or use the
    echo      global hotkey ^(default Ctrl+Shift+R^) to open a random file.
    echo   5. Right-click a recent file to open it or its containing folder.
    echo.
    echo LANGUAGE SUPPORT:
    echo   Language resources are loaded at runtime from the .dll files
    echo   shipped alongside RandPlay.exe. Keep them in the same folder.
    echo.
    echo FEATURES:
    echo   - Random file selection from a configured directory
    echo   - Video / image / custom extension filtering
    echo   - Live language switching ^(English and Simplified Chinese^)
    echo   - Configurable global hotkey
    echo   - Recent files list with context menu
    echo   - Per-monitor DPI awareness
    echo   - Portable ^(no installation required^)
    echo.
    echo SYSTEM REQUIREMENTS:
    echo   - Windows 10 or later
    echo.
    echo Enjoy using RandPlay!
) > "%RELEASE_DIR%\HOW_TO_USE.txt"

call :print_step 5 "Complete"

REM --- summary ---
echo.
call :print_success "Release package created successfully"
call :print_info  "Release directory: %RELEASE_DIR%"
echo.
for %%f in ("%RELEASE_DIR%\RandPlay.exe" "%RELEASE_DIR%\RandPlay_en.dll" "%RELEASE_DIR%\RandPlay_zh_cn.dll") do (
    if exist "%%~f" (
        call :format_size "%%~zf"
        call :print_info "%%~nxf - !_HUMAN_SIZE!"
    )
)

echo.
call :print_info "Open release directory? (Y/N)"
set "open_dir="
set /p open_dir=">"
if /i "!open_dir!"=="y" start "" "%RELEASE_DIR%"

echo.
call :print_success "Done"
pause
endlocal
exit /b 0

REM ================================================================
REM Helpers (same-file labels)
REM ================================================================
:print_header
    echo.
    echo %COLOR_CYAN%=============================================================%COLOR_RESET%
    echo %COLOR_CYAN%%PROJECT_NAME% - %~1%COLOR_RESET%
    echo %COLOR_CYAN%=============================================================%COLOR_RESET%
    echo.
    goto :eof

:print_success
    echo %COLOR_GREEN%[OK] %~1%COLOR_RESET%
    goto :eof

:print_error
    echo %COLOR_RED%[ERROR] %~1%COLOR_RESET%
    goto :eof

:print_warning
    echo %COLOR_YELLOW%[WARNING] %~1%COLOR_RESET%
    goto :eof

:print_info
    echo %COLOR_BLUE%[INFO] %~1%COLOR_RESET%
    goto :eof

:print_step
    echo %COLOR_WHITE%[%~1] %~2%COLOR_RESET%
    goto :eof

:safe_rmdir
    if exist "%~1" (
        call :print_info "Removing directory: %~1"
        rmdir /s /q "%~1" 2>nul
        if exist "%~1" (
            call :print_warning "Directory removal failed, retrying: %~1"
            timeout /t 2 /nobreak >nul
            rmdir /s /q "%~1" 2>nul
        )
    )
    goto :eof

:get_timestamp
    for /f "delims=" %%t in ('powershell -NoProfile -Command "Get-Date -Format 'yyyy-MM-dd HH:mm:ss'"') do set "timestamp=%%t"
    goto :eof

REM format_size <bytes>  ->  sets _HUMAN_SIZE
:format_size
    set "_bytes=%~1"
    set "_HUMAN_SIZE=%_bytes% B"
    if %_bytes% geq 1048576 goto :fmt_mb
    if %_bytes% geq 1024 goto :fmt_kb
    goto :eof
:fmt_mb
    set /a "_mb=%_bytes% / 1048576"
    set "_HUMAN_SIZE=!_mb! MB"
    goto :eof
:fmt_kb
    set /a "_kb=%_bytes% / 1024"
    set "_HUMAN_SIZE=!_kb! KB"
    goto :eof
