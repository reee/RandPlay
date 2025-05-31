@echo off
echo Building RandPlay...

:: Compile resource file
echo Compiling resource file...
rc /fo RandPlay.res RandPlay.rc
if %ERRORLEVEL% neq 0 (
    echo Failed to compile resource file.
    echo This might be because the RC compiler is not in your PATH.
    echo.
    pause
    exit /b 1
)

:: Compile and link
echo Compiling and linking...
cl /EHsc /std:c++17 RandPlay.cpp RandPlay.res /link /SUBSYSTEM:WINDOWS user32.lib gdi32.lib comctl32.lib shell32.lib ole32.lib
if %ERRORLEVEL% neq 0 (
    echo Failed to compile and link.
    echo.
    pause
    exit /b 1
)

echo.
echo Build complete!
if exist RandPlay.exe (
    echo RandPlay.exe created successfully.
    echo You can now run RandPlay.exe to start the application.
) else (
    echo Failed to create RandPlay.exe.
)

pause
