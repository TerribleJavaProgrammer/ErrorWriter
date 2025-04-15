@echo off
setlocal

:: Get the current user's home directory and directories of interest
set "USER_DIR=C:\users\%USERNAME%"
set "DESKTOP_DIR=%USER_DIR%\Desktop"
set "DOCUMENTS_DIR=%USER_DIR%\Documents"
set "DOWNLOADS_DIR=%USER_DIR%\Downloads"

:: Check for MSYS2 by looking for gcc or mingw32-make in typical MSYS2 path
where gcc >nul 2>nul
if %errorlevel% neq 0 (
    echo MSYS2 not detected, checking default MSYS2 install path...
    if exist "C:\msys64\usr\bin\gcc.exe" (
        echo MSYS2 found at C:\msys64. Adding to PATH for this session.
        set "PATH=C:\msys64\usr\bin;%PATH%"
    ) else (
        echo Neither MSYS2 nor MinGW are installed. Would you like to install MSYS2? (y/n)
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
)

:: Check if nite exists in any of the directories (Desktop, Documents, Downloads)
if not exist "%DESKTOP_DIR%\nite" if not exist "%DOCUMENTS_DIR%\nite" if not exist "%DOWNLOADS_DIR%\nite" (
    echo Cloning the Nite repository into your user directories...
    git clone https://github.com/TerribleJavaProgrammer/nite.git "%DESKTOP_DIR%\nite"
) else (
    echo Nite repository already exists in one of your user directories. Skipping clone.
)

:: Check if NiteV1 exists in the Desktop directory
if exist "%DESKTOP_DIR%\nite\NiteV1" (
    set "NITE_DIR=%DESKTOP_DIR%\nite\NiteV1"
) 
:: Check if NiteV1 exists in the Documents directory
if exist "%DOCUMENTS_DIR%\nite\NiteV1" (
    set "NITE_DIR=%DOCUMENTS_DIR%\nite\NiteV1"
)
:: Check if NiteV1 exists in the Downloads directory
if exist "%DOWNLOADS_DIR%\nite\NiteV1" (
    set "NITE_DIR=%DOWNLOADS_DIR%\nite\NiteV1"
)

cd "%NITE_DIR%"
echo Compiling NiteV1...

:: Check for mingw32-make
where mingw32-make >nul 2>nul
if %errorlevel% neq 0 (
    echo 'mingw32-make' not found. Please ensure it's available in your MSYS2 or MinGW setup.
    pause
    exit /b
)

:: Compile using mingw32-make
mingw32-make

:: Absolute path of the executable
set "EXE_PATH=%cd%\NiteV1.exe"

:: Add to PATH (current session)
echo Adding NiteV1 to system PATH for this session...
set "PATH=%PATH%;%EXE_PATH%"

:: Check if NiteV1 is already in the PATH before permanently adding it
echo Checking if NiteV1 is already in the system PATH...
echo %PATH% | findstr /C:"%EXE_PATH%" >nul
if %errorlevel% neq 0 (
    echo Adding NiteV1 to system PATH permanently...
    setx PATH "%PATH%"
) else (
    echo NiteV1 is already in the PATH. Skipping permanent addition.
)

echo "Compilation complete. You can now run NiteV1 from any directory by typing: nite <filename>"
pause
endlocal
