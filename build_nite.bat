@echo off
setlocal

:: Check for MSYS2 or MinGW
where.exe gcc >nul 2>nul

if %errorlevel% neq 0 (
    echo Neither MSYS2 or MinGW are installed. Would you like to install MSYS2? (y/n)
    set /p response=
    if /i "%response%" == "y" (
        echo Installing MSYS2...
        curl -L https://repo.msys2.org/distrib/x86_64/msys2-x86_64-20210625.exe -o msys2-installer.exe
        start msys2-installer.exe
        pause
        exit /b
    ) else (
        echo Please install MSYS2 or MinGW manually and try again.
        exit /b
    )
)

:: Clone the repository if it doesn't exist
if not exisdt "nite" (
    git clone https://github.com/TerribleJavaProgrammer/nite.git
)

cd nite
echo Compiling Nite...

:: Run make command (make sure MinGW/MSYS2 is set up with GCC)
make

:: Get the absolute path of the executable
set EXE_PATH=%cd%\nite.exe

:: Add Nite to the PATH for the current session
echo Adding Nite to system PATH for this session...
set PATH = %PATH%;%EXE_PATH%

:: Permanently add Nite to the user's PATH
echo Permanently adding Nite to PATH...
setx Path "%PATH%"

echo Compilation complete. You can now run Nite from any directory by typing 'nite <filename>'.
pause
endlocal
