@echo off
setlocal
cd /d "%~dp0"
echo ============================================================
echo Running Dense Example 3: Wayland / Labwc Desktop Environment
echo ============================================================
..\..\dense.exe main.dense
echo.
echo Process exit code: %ERRORLEVEL%
echo ============================================================
pause
