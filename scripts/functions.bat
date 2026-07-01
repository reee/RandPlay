@echo off
REM ================================================================
REM RandPlay Build Script Library - Shared Configuration
REM ================================================================
REM "Source" this from sibling scripts with:  call "%~dp0functions.bat"
REM It sets project paths, ANSI color variables, and locates CMake
REM (sets CMAKE_CMD, or leaves it empty if not found).
REM
REM NOTE: batch labels are NOT callable across files, so this file
REM intentionally contains NO reusable labels. Each script defines
REM its own :print_* helpers (see build.bat / dev.bat for the shared
REM snippet). Delayed expansion is enabled here because several
REM variables reference each other inside the guard block.

setlocal EnableDelayedExpansion

if not defined RANDPLAY_FUNCTIONS_LOADED (
    set "RANDPLAY_FUNCTIONS_LOADED=1"

    REM ============================================================
    REM Project Configuration
    REM ============================================================
    set "PROJECT_NAME=RandPlay"
    set "PROJECT_VERSION=1.0.0"
    set "DEFAULT_CONFIG=Release"

    REM Project root (this file lives in scripts/, go up one level)
    pushd "%~dp0.."
    set "PROJECT_ROOT=!CD!"
    popd

    set "BUILD_DIR=!PROJECT_ROOT!\build"
    set "BIN_DIR=!BUILD_DIR!\bin"
    set "SRC_DIR=!PROJECT_ROOT!\src"
    set "SCRIPTS_DIR=!PROJECT_ROOT!\scripts"

    REM ============================================================
    REM ANSI Color Codes (build the ESC prefix at runtime)
    REM ============================================================
    for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do set "ESC=%%b"
    set "COLOR_RESET=!ESC![0m"
    set "COLOR_RED=!ESC![91m"
    set "COLOR_GREEN=!ESC![92m"
    set "COLOR_YELLOW=!ESC![93m"
    set "COLOR_BLUE=!ESC![94m"
    set "COLOR_CYAN=!ESC![96m"
    set "COLOR_WHITE=!ESC![97m"
)

REM Promote variables to the caller's environment (escape the local
REM scope introduced above). RANDPLAY_FUNCTIONS_LOADED gates re-entry.
endlocal & (
    set "PROJECT_NAME=%PROJECT_NAME%"
    set "PROJECT_VERSION=%PROJECT_VERSION%"
    set "DEFAULT_CONFIG=%DEFAULT_CONFIG%"
    set "PROJECT_ROOT=%PROJECT_ROOT%"
    set "BUILD_DIR=%BUILD_DIR%"
    set "BIN_DIR=%BIN_DIR%"
    set "SRC_DIR=%SRC_DIR%"
    set "SCRIPTS_DIR=%SCRIPTS_DIR%"
    set "ESC=%ESC%"
    set "COLOR_RESET=%COLOR_RESET%"
    set "COLOR_RED=%COLOR_RED%"
    set "COLOR_GREEN=%COLOR_GREEN%"
    set "COLOR_YELLOW=%COLOR_YELLOW%"
    set "COLOR_BLUE=%COLOR_BLUE%"
    set "COLOR_CYAN=%COLOR_CYAN%"
    set "COLOR_WHITE=%COLOR_WHITE%"
    set "RANDPLAY_FUNCTIONS_LOADED=%RANDPLAY_FUNCTIONS_LOADED%"
)

REM ================================================================
REM Locate CMake -> CMAKE_CMD
REM   1) On PATH
REM   2) Bundled with the latest Visual Studio (via vswhere, which
REM      ships with every VS 2017+ install at a fixed location)
REM Leaves CMAKE_CMD empty if neither is available.
REM ================================================================
set "CMAKE_CMD="
for /f "delims=" %%c in ('where cmake 2^>nul') do (
    if not defined CMAKE_CMD set "CMAKE_CMD=%%c"
)
if defined CMAKE_CMD goto :eof

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" set "VSWHERE=%ProgramFiles%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "%VSWHERE%" (
    set "VS_INSTALL="
    for /f "usebackq delims=" %%v in (`"%VSWHERE%" -latest -products * -property installationPath`) do set "VS_INSTALL=%%v"
    if exist "%VS_INSTALL%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" (
        set "CMAKE_CMD=%VS_INSTALL%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
    )
)
goto :eof
