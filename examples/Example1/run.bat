@echo off
setlocal
cd /d "%~dp0"
echo ============================================================
echo Running Dense Example 1: Python Generator and Execution
echo ============================================================
..\..\dense.exe main.dense
echo.
echo Process exit code: %ERRORLEVEL%
echo ============================================================
pause
