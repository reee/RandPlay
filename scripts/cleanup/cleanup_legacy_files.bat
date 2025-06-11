@echo off
setlocal EnableDelayedExpansion

echo ==============================================================
echo RandPlay - 清理遗留文件
echo ==============================================================
echo.
echo 此脚本将清理项目重构后不再需要的遗留文件：
echo - RandPlay.cpp (原始单文件代码)
echo.
echo 清理前将创建备份到 legacy_backup/ 目录
echo.

set /p confirm="确认执行清理操作? (y/N): "
if /i not "%confirm%"=="y" (
    echo 操作已取消。
    pause
    exit /b 0
)

echo.
echo 正在清理遗留文件...

:: 切换到项目根目录
cd /d "%~dp0..\.."

:: 创建备份目录
if not exist "legacy_backup" mkdir "legacy_backup"

:: 备份并删除原始文件
if exist "RandPlay.cpp" (
    echo 备份 RandPlay.cpp 到 legacy_backup/
    copy "RandPlay.cpp" "legacy_backup\" >nul
    if !errorlevel! equ 0 (
        echo 删除 RandPlay.cpp
        del "RandPlay.cpp"
        if !errorlevel! equ 0 (
            echo [✓] 成功删除 RandPlay.cpp
        ) else (
            echo [✗] 删除 RandPlay.cpp 失败
        )
    ) else (
        echo [✗] 备份 RandPlay.cpp 失败，跳过删除
    )
) else (
    echo [!] RandPlay.cpp 不存在，可能已被删除
)

echo.
echo 清理完成！
echo.
echo 备份文件位置: legacy_backup/
echo.
echo 如需恢复文件，请从备份目录复制。
echo 30天后可安全删除 legacy_backup/ 目录。
echo.
pause
