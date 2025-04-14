@echo off
setlocal

:: Check for required tools
where gcc >nul 2>nul
if %errorlevel% neq 0 (
    echo Neither MSYS2 nor MinGW are installed. Would you like to install MSYS2? (y/n)
    set /p response=
    if /i "%response%" == "y" (
        where curl >nul 2>nul
        if %errorlevel% neq 0 (
            echo curl not found. Please install it manually.
            exit /b
        )
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

where git >nul 2>nul
if %errorlevel% neq 0 (
    echo Git is not installed or not in PATH.
    exit /b
)

:: Clone repo if it doesn't exist
if not exist "nite" (
    git clone https://github.com/TerribleJavaProgrammer/nite.git
)

pushd nite
echo Compiling Nite...

where make >nul 2>nul
if %errorlevel% neq 0 (
    echo 'make' not found. Please ensure it's available.
    pause
    popd
    exit /b
)

make

set EXE_PATH=%cd%\nite.exe

echo Adding Nite to session PATH...
set PATH=%PATH%;%cd%

echo.
echo If you want to run Nite from any terminal session, add this to your PATH manually:
echo %cd%
echo.

popd
pause
endlocal
