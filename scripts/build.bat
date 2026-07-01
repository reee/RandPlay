@echo off
setlocal EnableDelayedExpansion

REM ============================================================
REM RandPlay Build Script
REM ============================================================
REM Builds RandPlay (one EXE + two resource DLLs) for x64.
REM
REM Usage:
REM   scripts\build.bat              Release build
REM   scripts\build.bat debug        Debug build
REM   scripts\build.bat run          Build + launch RandPlay.exe
REM   scripts\build.bat debug run    Combine flags
REM ============================================================

call "%~dp0functions.bat"

REM --- output helpers ---
call :print_header "Build Script"

REM --- parse arguments ---
set "BUILD_CONFIG=%DEFAULT_CONFIG%"
set "RUN_AFTER=false"
:parse_args
if "%~1"=="" goto :args_done
if /i "%~1"=="debug"   ( set "BUILD_CONFIG=Debug"   & shift & goto :parse_args )
if /i "%~1"=="release" ( set "BUILD_CONFIG=Release" & shift & goto :parse_args )
if /i "%~1"=="run"     ( set "RUN_AFTER=true"       & shift & goto :parse_args )
call :print_warning "Unknown argument: %~1"
shift & goto :parse_args
:args_done

REM --- locate CMake ---
if not defined CMAKE_CMD (
    call :print_error "cmake not found. Install CMake or Visual Studio 2019+ (Build Tools) and re-run."
    call :print_info  "Download: https://cmake.org/download/"
    exit /b 1
)
call :print_info "Using CMake: %CMAKE_CMD%"

REM --- ensure output dirs ---
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%" 2>nul
if not exist "%BIN_DIR%"  mkdir "%BIN_DIR%"  2>nul

REM --- generate build files ---
call :print_step 1 "Generating build files (x64)"
"%CMAKE_CMD%" -B "%BUILD_DIR%" -A x64 -S "%PROJECT_ROOT%"
if errorlevel 1 (
    call :print_error "Failed to generate build files"
    exit /b 1
)

REM --- build all targets (EXE + en/zh_cn resource DLLs) ---
call :print_step 2 "Building project [%BUILD_CONFIG%]"
"%CMAKE_CMD%" --build "%BUILD_DIR%" --config %BUILD_CONFIG%
if errorlevel 1 (
    call :print_error "Build failed"
    exit /b 1
)
call :print_success "Build completed"

REM --- report artifacts ---
set "OUT_DIR=%BIN_DIR%\%BUILD_CONFIG%"
for %%f in ("%OUT_DIR%\RandPlay.exe" "%OUT_DIR%\RandPlay_en.dll" "%OUT_DIR%\RandPlay_zh_cn.dll") do (
    if exist "%%~f" (
        call :format_size "%%~zf"
        call :print_info "%%~nxf - !_HUMAN_SIZE!"
    ) else (
        call :print_warning "Expected output missing: %%~nxf"
    )
)

REM --- optionally launch ---
if "%RUN_AFTER%"=="true" (
    if exist "%OUT_DIR%\RandPlay.exe" (
        call :print_step 3 "Running RandPlay.exe"
        start "" "%OUT_DIR%\RandPlay.exe"
    ) else (
        call :print_error "RandPlay.exe not found, cannot run"
        exit /b 1
    )
)

echo.
call :print_success "Done"
endlocal
exit /b 0

REM ================================================================
REM Helpers (same-file labels)
REM ================================================================
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

REM format_size <bytes>  ->  sets _HUMAN_SIZE
:format_size
    set "_bytes=%~1"
    set "_HUMAN_SIZE=%_bytes% B"
    if %_bytes% geq 1048576 goto :fmt_mb
    if %_bytes% geq 1024 goto :fmt_kb
    goto :eof
:fmt_mb
    set /a "_mb=%_bytes% / 1048576"
    set "_HUMAN_SIZE=!_mb! MB"
    goto :eof
:fmt_kb
    set /a "_kb=%_bytes% / 1024"
    set "_HUMAN_SIZE=!_kb! KB"
    goto :eof
