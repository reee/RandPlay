@echo off
setlocal EnableDelayedExpansion

echo ==============================================================
echo RandPlay - 多语言版本快速测试
echo ==============================================================
echo.

set "lang=%1"
if "%lang%"=="" (
    echo 请选择要测试的语言版本:
    echo   1. 英文版 (EN)
    echo   2. 中文版 (ZH_CN)
    echo.
    set /p choice="输入选择 (1-2): "
    
    if "!choice!"=="1" set "lang=EN"
    if "!choice!"=="2" set "lang=ZH_CN"
    
    if "!lang!"=="" (
        echo 无效选择，默认使用英文版
        set "lang=EN"
    )
)

echo.
echo 正在构建并运行 %lang% 版本...
echo.

:: 切换到项目根目录
cd /d "%~dp0..\.."

:: 构建指定版本
call scripts\build\build_multilang.bat Release
if %errorlevel% neq 0 (
    echo 构建失败！
    pause
    exit /b 1
)

echo.
echo 启动 RandPlay_%lang%.exe...

:: 等待文件解锁
timeout /t 1 /nobreak >nul

:: 运行程序
cd build\bin\Release
start RandPlay_%lang%.exe

echo.
echo 程序已启动，测试界面语言是否正确
echo.
pause
