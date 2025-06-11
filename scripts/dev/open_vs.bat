@echo off
echo Opening RandPlay in Visual Studio...

:: Check if solution exists
if not exist "build\RandPlay.sln" (
    echo Solution file not found. Building project first...
    call build_cmake_auto.bat
    if %ERRORLEVEL% neq 0 (
        echo Failed to build project!
        pause
        exit /b 1
    )
)

:: Open Visual Studio solution
if exist "build\RandPlay.sln" (
    echo Opening Visual Studio...
    start "" "build\RandPlay.sln"
) else (
    echo Solution file still not found!
    pause
    exit /b 1
)

echo Visual Studio should now be opening...
