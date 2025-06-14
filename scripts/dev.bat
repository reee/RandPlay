@echo off
setlocal EnableDelayedExpansion

REM 加载公共函数
call "%~dp0functions.bat"

call :print_header "开发工具"

REM 显示菜单
echo.
call :print_info "选择操作:"
echo.
echo   1. 快速构建 (Debug)
echo   2. 发布构建 (Release)
echo   3. 清理构建
echo   4. 运行英文版
echo   5. 运行中文版
echo   6. 查看构建状态
echo   0. 退出
echo.

set /p choice="请选择 (0-6): "

if "%choice%"=="1" goto :quick_build
if "%choice%"=="2" goto :release_build
if "%choice%"=="3" goto :clean_build
if "%choice%"=="4" goto :run_en
if "%choice%"=="5" goto :run_zh
if "%choice%"=="6" goto :build_status
if "%choice%"=="0" goto :end
goto :invalid_choice

:quick_build
call :print_step "1" "快速构建 (Debug)"
call "%SCRIPTS_DIR%\build.bat" debug
goto :end

:release_build
call :print_step "1" "发布构建"
call "%SCRIPTS_DIR%\build_release.bat"
goto :end

:clean_build
call :print_step "1" "清理构建环境"
call :safe_rmdir "%BUILD_DIR%"
call :print_success "构建环境已清理"
goto :end

:run_en
if exist "%BIN_DIR%\Release\RandPlay_EN.exe" (
    call :print_step "1" "启动英文版"
    start "" "%BIN_DIR%\Release\RandPlay_EN.exe"
) else if exist "%BIN_DIR%\Debug\RandPlay_EN.exe" (
    call :print_step "1" "启动英文版 (Debug)"
    start "" "%BIN_DIR%\Debug\RandPlay_EN.exe"
) else (
    call :print_error "英文版程序未找到，请先构建项目"
)
goto :end

:run_zh
if exist "%BIN_DIR%\Release\RandPlay_ZH_CN.exe" (
    call :print_step "1" "启动中文版"
    start "" "%BIN_DIR%\Release\RandPlay_ZH_CN.exe"
) else if exist "%BIN_DIR%\Debug\RandPlay_ZH_CN.exe" (
    call :print_step "1" "启动中文版 (Debug)"
    start "" "%BIN_DIR%\Debug\RandPlay_ZH_CN.exe"
) else (
    call :print_error "中文版程序未找到，请先构建项目"
)
goto :end

:build_status
call :print_step "1" "检查构建状态"
echo.

if exist "%BUILD_DIR%\CMakeCache.txt" (
    call :print_success "CMake 配置已生成"
) else (
    call :print_warning "CMake 配置未生成"
)

if exist "%BIN_DIR%\Debug\RandPlay_EN.exe" (
    for %%f in ("%BIN_DIR%\Debug\RandPlay_EN.exe") do (
        call :print_info "Debug版英文程序: %%~zf 字节"
    )
) else (
    call :print_warning "Debug版英文程序未构建"
)

if exist "%BIN_DIR%\Debug\RandPlay_ZH_CN.exe" (
    for %%f in ("%BIN_DIR%\Debug\RandPlay_ZH_CN.exe") do (
        call :print_info "Debug版中文程序: %%~zf 字节"
    )
) else (
    call :print_warning "Debug版中文程序未构建"
)

if exist "%BIN_DIR%\Release\RandPlay_EN.exe" (
    for %%f in ("%BIN_DIR%\Release\RandPlay_EN.exe") do (
        call :print_info "Release版英文程序: %%~zf 字节"
    )
) else (
    call :print_warning "Release版英文程序未构建"
)

if exist "%BIN_DIR%\Release\RandPlay_ZH_CN.exe" (
    for %%f in ("%BIN_DIR%\Release\RandPlay_ZH_CN.exe") do (
        call :print_info "Release版中文程序: %%~zf 字节"
    )
) else (
    call :print_warning "Release版中文程序未构建"
)

goto :end

:invalid_choice
call :print_error "无效选择，请重新运行脚本"
goto :end

:end
echo.
pause
