@echo off
setlocal enabledelayedexpansion

echo Searching for ErrorWriter.exe under %USERPROFILE%...

set "found="

for /r "%USERPROFILE%" %%f in (ErrorWriter.exe) do (
    if exist "%%f" (
        set "found=%%f"
        goto :launch
    )
)

echo ErrorWriter.exe not found in your user directory.
goto :eof

:launch
echo Found: !found!
echo Launching ErrorWriter.exe with arguments: %*
start "" "!found!" %*
