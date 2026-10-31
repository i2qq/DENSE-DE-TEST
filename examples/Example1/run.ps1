# Run Dense Example 1: Python Generator and Execution
$ErrorActionPreference = "Stop"
Set-Location -Path $PSScriptRoot

Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "Running Dense Example 1" -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor Cyan

& "..\..\dense.exe" main.dense

Write-Host "`nProcess finished with exit code: $LASTEXITCODE" -ForegroundColor Green
