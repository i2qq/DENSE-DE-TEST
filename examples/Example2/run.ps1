# Run Dense Example 2: Native Tkinter Unit Converter GUI
$ErrorActionPreference = "Stop"
Set-Location -Path $PSScriptRoot

# Clean up prior running instance if any per rule
Get-Process -Name "dense","python" -ErrorAction SilentlyContinue | Where-Object { $_.MainWindowTitle -like "*Unit Converter*" } | Stop-Process -Force -ErrorAction SilentlyContinue

Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "Running Dense Example 2: Native GUI (Zero .py Files)" -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor Cyan

& "..\..\dense.exe" main.dense

Write-Host "`nProcess finished with exit code: $LASTEXITCODE" -ForegroundColor Green
