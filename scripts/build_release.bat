@echo off
setlocal EnableDelayedExpansion

REM 加载公共函数
call "%~dp0functions.bat"

call :print_header "发布构建"

call :print_info "准备发布版本构建和打包"

REM 完全清理
call :print_step "1" "清理构建环境"
call :safe_rmdir "%BUILD_DIR%"
call :safe_rmdir "%PROJECT_ROOT%\dist"
call :ensure_dir "%BUILD_DIR%"
call :ensure_dir "%PROJECT_ROOT%\dist"

REM 生成构建文件
call :print_step "2" "生成构建文件"
cmake -B "%BUILD_DIR%" -A x64
if %errorlevel% neq 0 (
    call :print_error "生成构建文件失败"
    pause
    exit /b 1
)

REM 构建Release版本
call :print_step "3" "构建Release版本"
cmake --build "%BUILD_DIR%" --config Release
if %errorlevel% neq 0 (
    call :print_error "构建失败"
    pause
    exit /b 1
)

REM 创建发布包
call :print_step "4" "创建发布包"
set "RELEASE_DIR=%PROJECT_ROOT%\dist\RandPlay_%PROJECT_VERSION%"
call :ensure_dir "%RELEASE_DIR%"

REM 复制可执行文件
if exist "%BIN_DIR%\Release\RandPlay_EN.exe" (
    copy "%BIN_DIR%\Release\RandPlay_EN.exe" "%RELEASE_DIR%\" >nul
    call :print_success "已复制英文版"
)

if exist "%BIN_DIR%\Release\RandPlay_ZH_CN.exe" (
    copy "%BIN_DIR%\Release\RandPlay_ZH_CN.exe" "%RELEASE_DIR%\" >nul
    call :print_success "已复制中文版"
)

REM 复制资源文件
if exist "%BIN_DIR%\Release\RandPlay.ico" (
    copy "%BIN_DIR%\Release\RandPlay.ico" "%RELEASE_DIR%\" >nul
)

if exist "%PROJECT_ROOT%\README.md" (
    copy "%PROJECT_ROOT%\README.md" "%RELEASE_DIR%\" >nul
)

if exist "%PROJECT_ROOT%\LICENSE" (
    copy "%PROJECT_ROOT%\LICENSE" "%RELEASE_DIR%\" >nul
)

REM 创建版本信息文件
call :print_info "生成版本信息文件"
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
echo   RandPlay_EN.exe - English Version
echo   RandPlay_ZH_CN.exe - Chinese Version
echo   RandPlay.ico - Application Icon
echo   README.md - Project Documentation
echo   LICENSE - License Information
echo.
echo System Requirements:
echo   - Windows 10 or later
echo   - .NET Framework 4.7.2 or later
echo.
echo For more information, visit: https://github.com/yourname/RandPlay
) > "%RELEASE_DIR%\VERSION.txt"

REM 创建使用说明文件
call :print_info "生成使用说明文件"
(
echo RandPlay - Random File Player
echo =============================
echo.
echo OVERVIEW:
echo   RandPlay is a random file player that helps you discover
echo   and play files from your collection in a random order.
echo.
echo LANGUAGE VERSIONS:
echo   This package contains two language versions:
echo.
echo   ENGLISH VERSION:
echo     File: RandPlay_EN.exe
echo     Description: Full English interface
echo.
echo   CHINESE VERSION:
echo     File: RandPlay_ZH_CN.exe
echo     Description: Simplified Chinese interface
echo.
echo GETTING STARTED:
echo   1. Choose your preferred language version
echo   2. Run the corresponding .exe file
echo   3. Go to File ^> Settings to configure directories
echo   4. Build the file index
echo   5. Enjoy random file playback
echo.
echo FEATURES:
echo   - Random file selection from configured directories
echo   - Multi-language support ^(English and Chinese^)
echo   - Customizable file filters
echo   - Fast file indexing
echo   - Portable application ^(no installation required^)
echo.
echo SYSTEM REQUIREMENTS:
echo   - Windows 10 or later
echo   - Administrative rights may be required for some directories
echo.
echo SUPPORT:
echo   For issues, feature requests, or documentation,
echo   please refer to the README.md file or visit our
echo   project repository.
echo.
echo Enjoy using RandPlay!
) > "%RELEASE_DIR%\HOW_TO_USE.txt"

call :print_step "5" "完成"

REM 显示结果
echo.
call :print_success "发布构建完成"
call :print_info "发布目录: %RELEASE_DIR%"

if exist "%RELEASE_DIR%\RandPlay_EN.exe" (
    for %%f in ("%RELEASE_DIR%\RandPlay_EN.exe") do (
        call :print_info "RandPlay_EN.exe - %%~zf 字节"
    )
)

if exist "%RELEASE_DIR%\RandPlay_ZH_CN.exe" (
    for %%f in ("%RELEASE_DIR%\RandPlay_ZH_CN.exe") do (
        call :print_info "RandPlay_ZH_CN.exe - %%~zf 字节"
    )
)

call :print_info "打开发布目录? (Y/N)"
set /p open_dir=">"
if /i "%open_dir%"=="y" (
    start "" "%RELEASE_DIR%"
)

echo.
pause
