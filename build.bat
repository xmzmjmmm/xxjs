@echo off
setlocal
cd /d %~dp0
chcp 65001 >nul
echo   星星加速exe构建
echo.

where g++ >nul 2>nul
if %errorlevel% neq 0 (
    echo [错误] 未找到 g++ 编译器！
    pause
    exit /b 1
)

echo [进度] 正在编译 星星加速.cpp...
g++ -std=c++17 "星星加速.cpp" -o "patcher.exe" -lshlwapi -lshell32 -lole32 -lgdiplus -mwindows -municode

if %errorlevel% equ 0 (
    if exist "星星加速.exe" del "星星加速.exe"
    ren "patcher.exe" "星星加速.exe"
    echo.
    echo [成功] 构建完成: 星星加速.exe
) else (
    echo.
    echo [失败] 编译过程中出现错误，请检查源码。
)

echo.
echo ========================================
pause
