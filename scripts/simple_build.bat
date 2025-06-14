@echo off
echo Building RandPlay...

REM Set project root
set "PROJECT_ROOT=%~dp0.."
set "BUILD_DIR=%PROJECT_ROOT%\build"

REM Change to project directory
cd /d "%PROJECT_ROOT%"

REM Check if cmake exists
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: cmake not found
    pause
    exit /b 1
)

REM Create build directory if it doesn't exist
if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

REM Generate build files
echo [1/2] Generating build files...
cmake -B "%BUILD_DIR%" -A x64
if %errorlevel% neq 0 (
    echo ERROR: Failed to generate build files
    pause
    exit /b 1
)

REM Build project
echo [2/2] Building project...
cmake --build "%BUILD_DIR%" --config Release
if %errorlevel% neq 0 (
    echo ERROR: Build failed
    pause
    exit /b 1
)

echo.
echo Build completed successfully!

REM Show results
if exist "%BUILD_DIR%\bin\Release\RandPlay_EN.exe" (
    echo - RandPlay_EN.exe created
)
if exist "%BUILD_DIR%\bin\Release\RandPlay_ZH_CN.exe" (
    echo - RandPlay_ZH_CN.exe created
)

pause
