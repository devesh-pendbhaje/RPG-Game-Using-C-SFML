@echo off
echo Compiling Text-Based RPG Game Engine...
g++ -std=c++17 main.cpp -o TextRPG.exe

if %errorlevel% neq 0 (
    echo Compilation failed!
    pause
    exit /b %errorlevel%
)

echo.
echo Done! Build successful.
echo.
echo Running TextRPG.exe...
TextRPG.exe
pause
