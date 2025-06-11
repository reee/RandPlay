@echo off
setlocal EnableDelayedExpansion

REM 加载公共函数
call "%~dp0..\common\functions.bat"

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
)

echo.
echo [3/5] 构建英文版和中文版...
cmake --build build --config %config% --parallel
if %errorlevel% neq 0 (
    echo [✗] 构建失败
    pause
    exit /b 1
)

echo.
echo [4/5] 创建发布包...

:: 创建发布目录结构
mkdir "dist\RandPlay_%config%"
mkdir "dist\RandPlay_%config%\English"
mkdir "dist\RandPlay_%config%\Chinese"

:: 复制文件
copy "build\bin\%config%\RandPlay_EN.exe" "dist\RandPlay_%config%\English\"
copy "build\bin\%config%\RandPlay.ico" "dist\RandPlay_%config%\English\"
copy "build\bin\%config%\RandPlay_ZH_CN.exe" "dist\RandPlay_%config%\Chinese\"
copy "build\bin\%config%\RandPlay.ico" "dist\RandPlay_%config%\Chinese\"

:: 复制文档
copy "README.md" "dist\RandPlay_%config%\"
copy "LICENSE" "dist\RandPlay_%config%\"

:: 创建版本信息文件
echo RandPlay Multi-Language Build > "dist\RandPlay_%config%\VERSION.txt"
echo Build Date: %date% %time% >> "dist\RandPlay_%config%\VERSION.txt"
echo Configuration: %config% >> "dist\RandPlay_%config%\VERSION.txt"
echo. >> "dist\RandPlay_%config%\VERSION.txt"
echo Files: >> "dist\RandPlay_%config%\VERSION.txt"
echo   English\RandPlay_EN.exe - English Version >> "dist\RandPlay_%config%\VERSION.txt"
echo   Chinese\RandPlay_ZH_CN.exe - Chinese Version >> "dist\RandPlay_%config%\VERSION.txt"

echo.
echo [5/5] 生成使用说明...

:: 创建使用说明
(
echo RandPlay - Random File Player
echo =============================
echo.
echo This package contains two language versions:
echo.
echo ENGLISH VERSION:
echo   File: English\RandPlay_EN.exe
echo   Description: Full English interface
echo.
echo CHINESE VERSION:
echo   File: Chinese\RandPlay_ZH_CN.exe  
echo   Description: Simplified Chinese interface
echo.
echo USAGE:
echo   1. Choose your preferred language version
echo   2. Run the corresponding .exe file
echo   3. Go to File ^> Settings to configure directories
echo   4. Build index and enjoy random file playback
echo.
echo REQUIREMENTS:
echo   - Windows 10 or later
echo   - Administrative rights ^(for some directories^)
echo.
echo For more information, see README.md
) > "dist\RandPlay_%config%\HOW_TO_USE.txt"

echo.
echo ==============================================================
echo 构建完成！
echo ==============================================================
echo.
echo 发布包位置: dist\RandPlay_%config%\
echo.

:: 显示文件列表
echo 包含文件:
dir /s /b "dist\RandPlay_%config%" | findstr /v "^d:\Project\RandPlay\dist\RandPlay_%config%$"

echo.
echo 文件大小:
for /r "dist\RandPlay_%config%" %%f in (*.exe) do (
    echo   %%f - %%~zf 字节
)

echo.
echo 可以将 dist\RandPlay_%config% 文件夹打包分发
pause
