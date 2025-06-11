@echo off
setlocal EnableDelayedExpansion

REM 加载公共函数
call "%~dp0..\common\functions.bat"

call :print_header "开发构建"

REM 参数解析
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

REM 检查必要工具
call :check_command cmake

REM 确保构建目录存在
call :ensure_dir "%BUILD_DIR%"
call :ensure_dir "%BIN_DIR%"

REM 生成构建文件
call :print_step "1" "生成构建文件"
cmake -B "%BUILD_DIR%" -A x64
if %errorlevel% neq 0 (
    call :print_error "生成构建文件失败"
    exit /b 1
)

REM 构建项目
call :print_step "2" "构建项目 [%BUILD_CONFIG%] - %BUILD_TARGET%"
if "%BUILD_TARGET%"=="all" (
    cmake --build "%BUILD_DIR%" --config %BUILD_CONFIG%
) else (
    cmake --build "%BUILD_DIR%" --config %BUILD_CONFIG% --target %BUILD_TARGET%
)

if %errorlevel% neq 0 (
    call :print_error "构建失败"
    exit /b 1
)

call :print_success "构建完成"

REM 显示构建结果
if exist "%BIN_DIR%\%BUILD_CONFIG%\RandPlay_EN.exe" (
    for %%f in ("%BIN_DIR%\%BUILD_CONFIG%\RandPlay_EN.exe") do (
        call :print_info "RandPlay_EN.exe - %%~zf 字节"
    )
)

if exist "%BIN_DIR%\%BUILD_CONFIG%\RandPlay_ZH_CN.exe" (
    for %%f in ("%BIN_DIR%\%BUILD_CONFIG%\RandPlay_ZH_CN.exe") do (
        call :print_info "RandPlay_ZH_CN.exe - %%~zf 字节"
    )
)

REM 运行程序
if "%RUN_AFTER%"=="true" (
    call :print_step "3" "启动程序"
    call "%SCRIPTS_DIR%\dev\run.bat"
)

echo.
call :print_success "任务完成"
