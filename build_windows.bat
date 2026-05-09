@echo off
setlocal

where cmake >nul 2>nul
if errorlevel 1 (
  echo CMake est introuvable. Installez CMake puis reessayez.
  exit /b 1
)

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 exit /b 1

cmake --build build --config Release
if errorlevel 1 exit /b 1

echo Build termine. Binaire attendu: build\Release\app.exe ou build\app.exe
