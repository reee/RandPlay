@echo off
REM ================================================================
REM RandPlay Build Script Library - Common Functions
REM ================================================================
REM This file contains common functions and configuration
REM Usage: call "%~dp0functions.bat"

if not defined RANDPLAY_FUNCTIONS_LOADED (
    set "RANDPLAY_FUNCTIONS_LOADED=1"

    REM ============================================================
    REM Project Configuration
    REM ============================================================
    set "PROJECT_NAME=RandPlay"
    set "PROJECT_VERSION=1.0.0"
    set "DEFAULT_CONFIG=Release"

    REM Get project root directory (this file is in scripts/, go up one level)
    pushd "%~dp0.."
    set "PROJECT_ROOT=%CD%"
    popd

    REM Directory paths
    set "BUILD_DIR=%PROJECT_ROOT%\build"
    set "BIN_DIR=%BUILD_DIR%\bin"
    set "SRC_DIR=%PROJECT_ROOT%\src"
    set "SCRIPTS_DIR=%PROJECT_ROOT%\scripts"

    REM ============================================================
    REM Color Codes (Windows 10+)
    REM ============================================================
    set "COLOR_RESET=[0m"
    set "COLOR_RED=[91m"
    set "COLOR_GREEN=[92m"
    set "COLOR_YELLOW=[93m"
    set "COLOR_BLUE=[94m"
    set "COLOR_CYAN=[96m"
    set "COLOR_WHITE=[97m"

    REM Change to project root directory
    cd /d "%PROJECT_ROOT%"
)

REM Define functions outside the if block to avoid parsing issues

:print_header
    echo.
    echo %COLOR_CYAN%=============================================================%COLOR_RESET%
    echo %COLOR_CYAN%%PROJECT_NAME% - %~1%COLOR_RESET%
    echo %COLOR_CYAN%=============================================================%COLOR_RESET%
    echo.
    goto :eof

:print_success
    echo %COLOR_GREEN%[OK] %~1%COLOR_RESET%
    goto :eof

:print_error
    echo %COLOR_RED%[ERROR] %~1%COLOR_RESET%
    goto :eof

:print_warning
    echo %COLOR_YELLOW%[WARNING] %~1%COLOR_RESET%
    goto :eof

:print_info
    echo %COLOR_BLUE%[INFO] %~1%COLOR_RESET%
    goto :eof

:print_step
    echo %COLOR_WHITE%[%~1] %~2%COLOR_RESET%
    goto :eof

:check_command
    where %1 >nul 2>&1
    if %errorlevel% neq 0 (
        call :print_error "%1 not found. Please ensure it is installed."
        exit /b 1
    )
    goto :eof

:ensure_dir
    if not exist "%~1" (
        mkdir "%~1" 2>nul
        if %errorlevel% neq 0 (
            call :print_error "Cannot create directory: %~1"
            exit /b 1
        )
    )
    goto :eof

:safe_rmdir
    if exist "%~1" (
        call :print_info "Removing directory: %~1"
        rmdir /s /q "%~1" 2>nul
        if exist "%~1" (
            call :print_warning "Directory removal failed, retrying: %~1"
            timeout /t 2 /nobreak >nul
            rmdir /s /q "%~1" 2>nul
        )
    )
    goto :eof

:check_file
    if not exist "%~1" (
        call :print_error "File not found: %~1"
        exit /b 1
    )
    goto :eof

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

:get_timestamp
    for /f "tokens=2 delims==" %%i in ('wmic OS Get localdatetime /value') do set "dt=%%i"
    set "timestamp=%dt:~0,4%-%dt:~4,2%-%dt:~6,2% %dt:~8,2%:%dt:~10,2%:%dt:~12,2%"
    goto :eof
