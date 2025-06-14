@echo off
REM ================================================================
REM RandPlay Script Utilities - Common Functions and Configuration
REM ================================================================
REM This file contains common functions and configuration variables used by all scripts

if not defined RANDPLAY_FUNCTIONS_LOADED (
    set RANDPLAY_FUNCTIONS_LOADED=1
    
    REM ============================================================
    REM Project Configuration
    REM ============================================================
    set "PROJECT_NAME=RandPlay"
    set "PROJECT_VERSION=1.0.0"
    set "DEFAULT_CONFIG=Release"
    
    REM Get project root directory
    set "PROJECT_ROOT=%~dp0.."
    
    REM Normalize path
    for %%i in ("%PROJECT_ROOT%") do set "PROJECT_ROOT=%%~fi"
    
    REM Build paths
    set "BUILD_DIR=%PROJECT_ROOT%\build"
    set "BIN_DIR=%BUILD_DIR%\bin"
    set "SRC_DIR=%PROJECT_ROOT%\src"
    set "SCRIPTS_DIR=%PROJECT_ROOT%\scripts"
    
    REM ============================================================
    REM Color definitions (Windows 10+)
    REM ============================================================
    set "COLOR_RESET=[0m"
    set "COLOR_RED=[91m"
    set "COLOR_GREEN=[92m"
    set "COLOR_YELLOW=[93m"
    set "COLOR_BLUE=[94m"
    set "COLOR_CYAN=[96m"
    set "COLOR_WHITE=[97m"
    
    REM ============================================================
    REM Utility Functions
    REM ============================================================
    
    REM Display colored header
    goto :skip_functions
    
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
    
    REM Check if command exists
    :check_command
        where %1 >nul 2>&1
        if %errorlevel% neq 0 (
            call :print_error "%1 not found, please ensure it is properly installed"
            exit /b 1
        )
        goto :eof
    
    REM Ensure directory exists
    :ensure_dir
        if not exist "%~1" (
            mkdir "%~1" 2>nul
            if %errorlevel% neq 0 (
                call :print_error "Cannot create directory: %~1"
                exit /b 1
            )
        )
        goto :eof
    
    REM Safely remove directory
    :safe_rmdir
        if exist "%~1" (
            call :print_info "Removing directory: %~1"
            rmdir /s /q "%~1" 2>nul
            if exist "%~1" (
                call :print_warning "Directory removal failed, files may be in use: %~1"
                timeout /t 2 /nobreak >nul
                rmdir /s /q "%~1" 2>nul
            )
        )
        goto :eof
    
    REM Check if file exists
    :check_file
        if not exist "%~1" (
            call :print_error "File does not exist: %~1"
            exit /b 1
        )
        goto :eof
    
    REM Format file size
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
    
    REM Get current timestamp
    :get_timestamp
        for /f "tokens=2 delims==" %%i in ('wmic OS Get localdatetime /value') do set "dt=%%i"
        set "timestamp=%dt:~0,4%-%dt:~4,2%-%dt:~6,2% %dt:~8,2%:%dt:~10,2%:%dt:~12,2%"
        goto :eof
    
    :skip_functions
)

REM Change to project root directory
cd /d "%PROJECT_ROOT%"
