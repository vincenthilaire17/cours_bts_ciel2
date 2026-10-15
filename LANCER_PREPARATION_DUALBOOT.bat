@echo off
title BTS CIEL - Preparation Dual-Boot Windows 11
cd /d "%~dp0"
echo =====================================================================
echo  Lancement de la preparation automatique au Dual-Boot (100 Go)...
echo =====================================================================
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0preparer_dualboot_windows11.ps1"
pause
