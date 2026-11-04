@echo off
setlocal
set "ROOT=%~dp0..\.."
set "CLI=%ONEROM_CLI%"
if not defined CLI for %%I in (onerom.exe) do set "CLI=%%~$PATH:I"

if not defined CLI echo OneROM CLI was not found on PATH. Set ONEROM_CLI to its full path. & exit /b 1
if not exist "%CLI%" echo OneROM CLI was not found: "%CLI%" & exit /b 1
"%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe" -NoProfile -ExecutionPolicy Bypass -Command "& '%~dp0Build-Control-V8.1-Clean.ps1' -RepoRoot '%ROOT%' -Cli '%CLI%'"
set "RESULT=%ERRORLEVEL%"
if not "%RESULT%"=="0" echo Control image composition failed with exit code %RESULT%.
if "%RESULT%"=="0" echo Control image composition completed successfully.
exit /b %RESULT%
