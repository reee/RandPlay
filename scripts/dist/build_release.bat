@echo off
setlocal EnableDelayedExpansion

echo ==============================================================
echo RandPlay - 完整构建和打包
echo ==============================================================
echo.

set "config=%1"
if "%config%"=="" set "config=Release"

echo 构建配置: %config%
echo.

:: 切换到项目根目录
cd /d "%~dp0..\.."

:: 清理并构建
echo [1/5] 清理之前的构建...
if exist build rmdir /s /q build
if exist dist rmdir /s /q dist
mkdir build
mkdir dist

echo.
echo [2/5] 生成构建文件...
cmake -B build -A x64
if %errorlevel% neq 0 (
    echo [✗] 生成构建文件失败
    pause
    exit /b 1
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
