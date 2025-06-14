@echo off
REM ================================================================
REM RandPlay 脚本工具库 - 公共函数和配置
REM ================================================================
REM 此文件包含所有脚本使用的公共函数和配置变量
REM 使用方法: call "%~dp0..\common\functions.bat"

if not defined RANDPLAY_FUNCTIONS_LOADED (
    set RANDPLAY_FUNCTIONS_LOADED=1
    
    REM ============================================================
    REM 项目配置
    REM ============================================================
    set "PROJECT_NAME=RandPlay"
    set "PROJECT_VERSION=1.0.0"
    set "DEFAULT_CONFIG=Release"
      REM 获取项目根目录
    set "PROJECT_ROOT=%~dp0.."
    
    REM 标准化路径
    for %%i in ("%PROJECT_ROOT%") do set "PROJECT_ROOT=%%~fi"
    
    REM 构建路径
    set "BUILD_DIR=%PROJECT_ROOT%\build"
    set "BIN_DIR=%BUILD_DIR%\bin"
    set "SRC_DIR=%PROJECT_ROOT%\src"
    set "SCRIPTS_DIR=%PROJECT_ROOT%\scripts"
    
    REM ============================================================
    REM 颜色定义 (Windows 10+)
    REM ============================================================
    set "COLOR_RESET=[0m"
    set "COLOR_RED=[91m"
    set "COLOR_GREEN=[92m"
    set "COLOR_YELLOW=[93m"
    set "COLOR_BLUE=[94m"
    set "COLOR_CYAN=[96m"
    set "COLOR_WHITE=[97m"
    
    REM ============================================================
    REM 工具函数
    REM ============================================================
    
    REM 显示带颜色的标题
    goto :skip_functions
    
    :print_header
        echo.
        echo %COLOR_CYAN%=============================================================%COLOR_RESET%
        echo %COLOR_CYAN%%PROJECT_NAME% - %~1%COLOR_RESET%
        echo %COLOR_CYAN%=============================================================%COLOR_RESET%
        echo.
        goto :eof
    
    :print_success
        echo %COLOR_GREEN%[✓] %~1%COLOR_RESET%
        goto :eof
    
    :print_error
        echo %COLOR_RED%[✗] %~1%COLOR_RESET%
        goto :eof
    
    :print_warning
        echo %COLOR_YELLOW%[!] %~1%COLOR_RESET%
        goto :eof
    
    :print_info
        echo %COLOR_BLUE%[i] %~1%COLOR_RESET%
        goto :eof
    
    :print_step
        echo %COLOR_WHITE%[%~1] %~2%COLOR_RESET%
        goto :eof
    
    REM 检查命令是否存在
    :check_command
        where %1 >nul 2>&1
        if %errorlevel% neq 0 (
            call :print_error "%1 未找到，请确保已正确安装"
            exit /b 1
        )
        goto :eof
    
    REM 确保目录存在
    :ensure_dir
        if not exist "%~1" (
            mkdir "%~1" 2>nul
            if %errorlevel% neq 0 (
                call :print_error "无法创建目录: %~1"
                exit /b 1
            )
        )
        goto :eof
    
    REM 安全删除目录
    :safe_rmdir
        if exist "%~1" (
            call :print_info "删除目录: %~1"
            rmdir /s /q "%~1" 2>nul
            if exist "%~1" (
                call :print_warning "目录删除失败，可能有文件被占用: %~1"
                timeout /t 2 /nobreak >nul
                rmdir /s /q "%~1" 2>nul
            )
        )
        goto :eof
    
    REM 检查文件是否存在
    :check_file
        if not exist "%~1" (
            call :print_error "文件不存在: %~1"
            exit /b 1
        )
        goto :eof
    
    REM 格式化文件大小
    :format_size
        set "size=%~1"
        if %size% lss 1024 (
            echo %size% B
        ) else if %size% lss 1048576 (
            set /a "kb=%size% / 1024"
            echo !kb! KB
        ) else (
            set /a "mb=%size% / 1048576"
            echo !mb! MB
        )
        goto :eof
    
    REM 获取当前时间戳
    :get_timestamp
        for /f "tokens=2 delims==" %%i in ('wmic OS Get localdatetime /value') do set "dt=%%i"
        set "timestamp=%dt:~0,4%-%dt:~4,2%-%dt:~6,2% %dt:~8,2%:%dt:~10,2%:%dt:~12,2%"
        goto :eof
    
    :skip_functions
)

REM 切换到项目根目录
cd /d "%PROJECT_ROOT%"
