@echo off
echo Quick build and run for RandPlay...

:: Build the project
call ..\build\build_cmake_auto.bat %1
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    exit /b 1
)

echo.
echo Starting application...

:: Wait a moment for any file locks to clear
timeout /t 1 /nobreak >nul

:: Run the application
call run.bat
