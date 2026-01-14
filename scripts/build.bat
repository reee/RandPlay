@echo off
setlocal EnableDelayedExpansion

REM ============================================================
REM RandPlay Build Script
REM ============================================================

REM Enable ANSI color support for Windows 10+
set "ESC="
for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do (
    set "ESC=%%b"
)
set "ANSI_RESET=%ESC%[0m"
set "ANSI_CYAN=%ESC%[96m"
set "ANSI_GREEN=%ESC%[92m"
set "ANSI_RED=%ESC%[91m"
set "ANSI_WHITE=%ESC%[97m"
set "ANSI_BLUE=%ESC%[94m"

REM Set project paths
set "SCRIPT_DIR=%~dp0"
set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"
pushd "%SCRIPT_DIR%\.."
set "PROJECT_ROOT=%CD%"
popd

set "BUILD_DIR=%PROJECT_ROOT%\build"
set "BIN_DIR=%BUILD_DIR%\bin"

REM Print header
echo.
echo %ANSI_CYAN%=============================================================%ANSI_RESET%
echo %ANSI_CYAN%RandPlay - Build Script%ANSI_RESET%
echo %ANSI_CYAN%=============================================================%ANSI_RESET%
echo.

REM Parse arguments
set "BUILD_CONFIG=Release"
set "BUILD_TARGET=all"
set "RUN_AFTER=false"

:parse_args
if "%~1"=="" goto :args_done
if /i "%~1"=="debug" set "BUILD_CONFIG=Debug" & shift & goto :parse_args
if /i "%~1"=="release" set "BUILD_CONFIG=Release" & shift & goto :parse_args
if /i "%~1"=="en" set "BUILD_TARGET=RandPlay_EN" & shift & goto :parse_args
if /i "%~1"=="zh" set "BUILD_TARGET=RandPlay_ZH_CN" & shift & goto :parse_args
if /i "%~1"=="run" set "RUN_AFTER=true" & shift & goto :parse_args
shift & goto :parse_args
:args_done

REM Check if cmake is available
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo %ANSI_RED%[ERROR] cmake not found. Please ensure it is installed.%ANSI_RESET%
    exit /b 1
)

REM Ensure directories exist
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if not exist "%BIN_DIR%" mkdir "%BIN_DIR%"

REM Generate build files
echo %ANSI_WHITE%[1] Generating build files%ANSI_RESET%
cmake -B "%BUILD_DIR%" -A x64
if %errorlevel% neq 0 (
    echo %ANSI_RED%[ERROR] Failed to generate build files%ANSI_RESET%
    exit /b 1
)

REM Build project
echo %ANSI_WHITE%[2] Building project [%BUILD_CONFIG%] - %BUILD_TARGET%%ANSI_RESET%
if "%BUILD_TARGET%"=="all" (
    cmake --build "%BUILD_DIR%" --config %BUILD_CONFIG%
) else (
    cmake --build "%BUILD_DIR%" --config %BUILD_CONFIG% --target %BUILD_TARGET%
)

if %errorlevel% neq 0 (
    echo %ANSI_RED%[ERROR] Build failed%ANSI_RESET%
    exit /b 1
)

echo %ANSI_GREEN%[OK] Build completed%ANSI_RESET%

REM Display output info
if exist "%BIN_DIR%\%BUILD_CONFIG%\RandPlay_EN.exe" (
    for %%f in ("%BIN_DIR%\%BUILD_CONFIG%\RandPlay_EN.exe") do (
        echo %ANSI_BLUE%[INFO] RandPlay_EN.exe - %%~zf bytes%ANSI_RESET%
    )
)

if exist "%BIN_DIR%\%BUILD_CONFIG%\RandPlay_ZH_CN.exe" (
    for %%f in ("%BIN_DIR%\%BUILD_CONFIG%\RandPlay_ZH_CN.exe") do (
        echo %ANSI_BLUE%[INFO] RandPlay_ZH_CN.exe - %%~zf bytes%ANSI_RESET%
    )
)

REM Run application
if "%RUN_AFTER%"=="true" (
    echo %ANSI_WHITE%[3] Running application%ANSI_RESET%
    start "" "%BIN_DIR%\%BUILD_CONFIG%\RandPlay_EN.exe"
)

echo.
echo %ANSI_GREEN%[OK] Done%ANSI_RESET%
