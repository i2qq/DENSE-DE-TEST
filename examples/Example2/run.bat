@echo off
setlocal
cd /d "%~dp0"
echo ============================================================
echo Running Dense Example 2: Native Tkinter GUI (Zero .py Files)
echo ============================================================
..\..\dense.exe main.dense
echo.
echo Process exit code: %ERRORLEVEL%
echo ============================================================
pause
