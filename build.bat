@echo off
rem 一键编译并运行图书馆程序（MinGW gcc）
setlocal
chcp 65001 >nul
cd /d "%~dp0"

gcc -std=c11 -Wall -Wextra -Wpedantic -Wshadow -Wformat=2 main.c book.c user.c loan.c utils.c -o library.exe -lwinmm
if errorlevel 1 (
    echo.
    echo [错误] 编译失败，请检查上方错误信息。
    pause
    exit /b 1
)

echo.
echo [成功] 编译完成，正在启动程序...
echo.
library.exe

echo.
echo 程序已退出。
pause
