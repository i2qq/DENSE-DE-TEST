Write-Host "==========================================================" -ForegroundColor Cyan
Write-Host "Running Dense Language QML, Python, and C Replacement Showcase" -ForegroundColor Cyan
Write-Host "==========================================================" -ForegroundColor Cyan

# Terminate existing active instances per process lifecycle rule
Stop-Process -Name dense -Force -ErrorAction SilentlyContinue

& "..\..\dense.exe" main.dense
