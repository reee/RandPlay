@echo off
chcp 65001 >nul
setlocal EnableDelayedExpansion

REM ============================================================
REM RandPlay Development Menu
REM ============================================================
REM Interactive launcher for the build scripts.
REM ============================================================

call "%~dp0functions.bat"

:menu
cls
call :print_header "Development Menu"
echo   1  Quick build ^(Debug^)
echo   2  Build ^(Release^)
echo   3  Release package ^(clean + dist^)
echo   4  Clean build directory
echo   5  Run RandPlay
echo   6  Open build output folder
echo   0  Exit
echo.
set "choice="
set /p choice="Select: "

if "!choice!"=="1" ( call "%~dp0build.bat" debug      & goto :resume )
if "!choice!"=="2" ( call "%~dp0build.bat" release    & goto :resume )
if "!choice!"=="3" ( call "%~dp0build_release.bat"    & goto :resume )
if "!choice!"=="4" ( call :clean                      & goto :resume )
if "!choice!"=="5" ( call :run_app                    & goto :resume )
if "!choice!"=="6" ( call :open_output                & goto :resume )
if "!choice!"=="0" goto :quit
call :print_warning "Invalid choice"
goto :resume

:resume
echo.
pause
goto :menu

:quit
endlocal
exit /b 0

REM ================================================================
REM Helpers
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

:clean
    if exist "%BUILD_DIR%" (
        echo %COLOR_BLUE%[INFO] Removing directory: %BUILD_DIR%%COLOR_RESET%
        rmdir /s /q "%BUILD_DIR%" 2>nul
    )
    call :print_success "Build directory cleaned"
    goto :eof

:run_app
for %%c in (Debug Release) do (
    if exist "%BIN_DIR%\%%~c\RandPlay.exe" (
        start "" "%BIN_DIR%\%%~c\RandPlay.exe"
        call :print_success "Launched %%~c build"
        goto :eof
    )
)
call :print_error "RandPlay.exe not found - build first"
goto :eof

:open_output
for %%c in (Release Debug) do (
    if exist "%BIN_DIR%\%%~c\RandPlay.exe" (
        start "" "%BIN_DIR%\%%~c"
        goto :eof
    )
)
call :print_warning "No build output found - build first"
if exist "%BUILD_DIR%" start "" "%BUILD_DIR%"
goto :eof
