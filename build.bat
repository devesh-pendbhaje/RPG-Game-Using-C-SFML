@echo off
echo Terminating any existing game.exe...
taskkill /f /im game.exe >nul 2>&1
echo Compiling the RPG Game (Modular Refactoring)...
g++ -std=c++17 src/*.cpp -Iinclude -IC:/SFML/include -LC:/SFML/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -o game.exe > build_log.txt 2>&1
if %errorlevel% neq 0 (
    echo Compilation failed! See build_log.txt for details.
    exit /b %errorlevel%
)
echo Done! Launching game.exe...
start game.exe
