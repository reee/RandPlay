@echo off
echo Starting RandPlay...

:: Check for Release build first
if exist "build\bin\Release\RandPlay.exe" (
    echo Running Release version...
    cd build\bin\Release
    RandPlay.exe
    goto :end
)

:: Check for Debug build
if exist "build\bin\Debug\RandPlay.exe" (
    echo Running Debug version...
    cd build\bin\Debug
    RandPlay.exe
    goto :end
)

:: Check for general build
if exist "build\bin\RandPlay.exe" (
    echo Running from build\bin...
    cd build\bin
    RandPlay.exe
    goto :end
)

echo RandPlay.exe not found!
echo Please build the project first using cmake_build.bat
pause

:end
