@echo off
echo Installing RandPlay...

:: Check if release build exists
if not exist "build\bin\Release\RandPlay.exe" (
    echo Release build not found. Building now...
    call build_cmake_auto.bat release
    if %ERRORLEVEL% neq 0 (
        echo Build failed!
        pause
        exit /b 1
    )
)

:: Create install directory
set "INSTALL_DIR=%USERPROFILE%\Desktop\RandPlay"
if not exist "%INSTALL_DIR%" (
    echo Creating install directory: %INSTALL_DIR%
    mkdir "%INSTALL_DIR%"
)

:: Copy files
echo Copying application files...
copy "build\bin\Release\RandPlay.exe" "%INSTALL_DIR%\" >nul
copy "build\bin\Release\RandPlay.ico" "%INSTALL_DIR%\" >nul
copy "README.md" "%INSTALL_DIR%\" >nul
copy "LICENSE" "%INSTALL_DIR%\" >nul

:: Create uninstall script
echo Creating uninstall script...
echo @echo off > "%INSTALL_DIR%\uninstall.bat"
echo echo Uninstalling RandPlay... >> "%INSTALL_DIR%\uninstall.bat"
echo cd /d "%USERPROFILE%\Desktop" >> "%INSTALL_DIR%\uninstall.bat"
echo rmdir /s /q "RandPlay" >> "%INSTALL_DIR%\uninstall.bat"
echo echo RandPlay has been uninstalled. >> "%INSTALL_DIR%\uninstall.bat"
echo pause >> "%INSTALL_DIR%\uninstall.bat"

echo.
echo Installation complete!
echo RandPlay has been installed to: %INSTALL_DIR%
echo.
echo To run RandPlay: "%INSTALL_DIR%\RandPlay.exe"
echo To uninstall: "%INSTALL_DIR%\uninstall.bat"
echo.
pause
