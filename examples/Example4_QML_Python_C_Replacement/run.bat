@echo off
taskkill /F /IM dense.exe 2>nul
..\..\dense.exe main.dense
pause
