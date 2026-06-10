@echo off
setlocal enabledelayedexpansion

echo ===============================
echo   UE 5.7 ONE CLICK REBUILD FIX
echo ===============================

cd /d %~dp0
cd ..

REM =========================
REM Detect .uproject
REM =========================
set UPROJECT=
for %%f in (*.uproject) do set UPROJECT=%%f

if "%UPROJECT%"=="" (
    echo [ERROR] No .uproject found
    pause
    exit /b 1
)

echo [INFO] Project: %UPROJECT%

set UPROJECT_PATH=%cd%\%UPROJECT%

REM =========================
REM UE PATH (your install)
REM =========================
set UE_PATH=C:\Program Files\UE\UE_5.7\

if not exist "%UE_PATH%" (
set UE_PATH=C:\Program Files\Epic Games\UE_5.7\
    
)

if not exist "%UE_PATH%" (
echo [ERROR] UE 5.7 not found at:
    echo %UE_PATH%
    pause
    exit /b 1
)

echo [INFO] UE Path: %UE_PATH%

REM =========================
REM CLEAN
REM =========================
echo.
echo [STEP] Cleaning project...

rmdir /s /q Binaries 2>nul
rmdir /s /q Intermediate 2>nul
rmdir /s /q .vs 2>nul
del /q *.sln 2>nul

REM =========================
REM PROJECT FILES VIA DOTNET UBT (UE 5.7 SAFE METHOD)
REM =========================
echo.
echo [STEP] Generating project files (dotnet UBT)...

set UBT_DLL=%UE_PATH%Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.dll

if not exist "%UBT_DLL%" (
    echo [ERROR] UnrealBuildTool.dll not found:
    echo %UBT_DLL%
    pause
    exit /b 1
)

dotnet "%UBT_DLL%" -projectfiles -project="%UPROJECT_PATH%" -game -engine -progress

if errorlevel 1 (
    echo [ERROR] Project file generation failed
    pause
    exit /b 1
)

REM =========================
REM BUILD
REM =========================
echo.
echo [STEP] Building project...

for %%A in ("%UPROJECT%") do set PROJECT_NAME=%%~nA

call "%UE_PATH%Engine\Build\BatchFiles\Build.bat" ^
%PROJECT_NAME%Editor Win64 Development ^
-Project="%UPROJECT_PATH%" ^
-WaitMutex

if errorlevel 1 (
    echo [ERROR] Build failed
    pause
    exit /b 1
)

echo.
echo ===============================
echo   BUILD SUCCESS
echo ===============================
pause
endlocal