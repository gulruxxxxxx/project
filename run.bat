@echo off
rem Windows: builds ALL source files and starts the program.
cd /d "%~dp0"
g++ -std=c++17 -Wall main.cpp helpers.cpp queue.cpp storage.cpp analytics.cpp dashboard.cpp menus.cpp demo.cpp -o queueless.exe
if errorlevel 1 (
  echo.
  echo BUILD FAILED - read the error above.
  pause
  exit /b 1
)
queueless.exe
pause
