@echo off
setlocal EnableDelayedExpansion

REM Load common functions
call "%~dp0functions_en.bat"

call :print_header "Development Build"

REM Parse arguments
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

REM Check required tools
call :check_command cmake

REM Ensure build directories exist
call :ensure_dir "%BUILD_DIR%"
call :ensure_dir "%BIN_DIR%"

REM Generate build files
call :print_step "1" "Generating build files"
cmake -B "%BUILD_DIR%" -A x64
if %errorlevel% neq 0 (
    call :print_error "Failed to generate build files"
    exit /b 1
)

REM Build project
call :print_step "2" "Building project [%BUILD_CONFIG%] - %BUILD_TARGET%"
if "%BUILD_TARGET%"=="all" (
    cmake --build "%BUILD_DIR%" --config %BUILD_CONFIG%
) else (
    cmake --build "%BUILD_DIR%" --config %BUILD_CONFIG% --target %BUILD_TARGET%
)

if %errorlevel% neq 0 (
    call :print_error "Build failed"
    exit /b 1
)

call :print_success "Build completed"

REM Display build results
if exist "%BIN_DIR%\%BUILD_CONFIG%\RandPlay_EN.exe" (
    for %%f in ("%BIN_DIR%\%BUILD_CONFIG%\RandPlay_EN.exe") do (
        call :print_info "RandPlay_EN.exe - %%~zf bytes"
    )
)

if exist "%BIN_DIR%\%BUILD_CONFIG%\RandPlay_ZH_CN.exe" (
    for %%f in ("%BIN_DIR%\%BUILD_CONFIG%\RandPlay_ZH_CN.exe") do (
        call :print_info "RandPlay_ZH_CN.exe - %%~zf bytes"
    )
)

REM Run program
if "%RUN_AFTER%"=="true" (
    call :print_step "3" "Starting program"
    if exist "%BIN_DIR%\%BUILD_CONFIG%\RandPlay_EN.exe" (
        start "" "%BIN_DIR%\%BUILD_CONFIG%\RandPlay_EN.exe"
    )
)

echo.
call :print_success "Task completed"
