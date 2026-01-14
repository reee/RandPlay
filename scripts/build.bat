@echo off
setlocal EnableDelayedExpansion

REM ============================================================
REM RandPlay Build Script
REM ============================================================

REM Set project paths
set "SCRIPT_DIR=%~dp0"
set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"
pushd "%SCRIPT_DIR%\.."
set "PROJECT_ROOT=%CD%"
popd

set "BUILD_DIR=%PROJECT_ROOT%\build"
set "BIN_DIR=%BUILD_DIR%\bin"

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
echo %COLOR_CYAN%RandPlay - Build Script%COLOR_RESET%
echo %COLOR_CYAN%=============================================================%COLOR_RESET%
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
    echo %COLOR_RED%[ERROR] cmake not found. Please ensure it is installed.%COLOR_RESET%
    exit /b 1
)

REM Ensure directories exist
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if not exist "%BIN_DIR%" mkdir "%BIN_DIR%"

REM Generate build files
echo %COLOR_WHITE%[1] Generating build files%COLOR_RESET%
cmake -B "%BUILD_DIR%" -A x64
if %errorlevel% neq 0 (
    echo %COLOR_RED%[ERROR] Failed to generate build files%COLOR_RESET%
    exit /b 1
)

REM Build project
echo %COLOR_WHITE%[2] Building project [%BUILD_CONFIG%] - %BUILD_TARGET%%COLOR_RESET%
if "%BUILD_TARGET%"=="all" (
    cmake --build "%BUILD_DIR%" --config %BUILD_CONFIG%
) else (
    cmake --build "%BUILD_DIR%" --config %BUILD_CONFIG% --target %BUILD_TARGET%
)

if %errorlevel% neq 0 (
    echo %COLOR_RED%[ERROR] Build failed%COLOR_RESET%
    exit /b 1
)

echo %COLOR_GREEN%[OK] Build completed%COLOR_RESET%

REM Display output info
if exist "%BIN_DIR%\%BUILD_CONFIG%\RandPlay_EN.exe" (
    for %%f in ("%BIN_DIR%\%BUILD_CONFIG%\RandPlay_EN.exe") do (
        echo %COLOR_BLUE%[INFO] RandPlay_EN.exe - %%~zf bytes%COLOR_RESET%
    )
)

if exist "%BIN_DIR%\%BUILD_CONFIG%\RandPlay_ZH_CN.exe" (
    for %%f in ("%BIN_DIR%\%BUILD_CONFIG%\RandPlay_ZH_CN.exe") do (
        echo %COLOR_BLUE%[INFO] RandPlay_ZH_CN.exe - %%~zf bytes%COLOR_RESET%
    )
)

REM Run application
if "%RUN_AFTER%"=="true" (
    echo %COLOR_WHITE%[3] Running application%COLOR_RESET%
    start "" "%BIN_DIR%\%BUILD_CONFIG%\RandPlay_EN.exe"
)

echo.
echo %COLOR_GREEN%[OK] Done%COLOR_RESET%
