@echo off
setlocal EnableExtensions EnableDelayedExpansion

set "PATCH_ROOT=%~dp0"
if "%PATCH_ROOT:~-1%"=="\" set "PATCH_ROOT=%PATCH_ROOT:~0,-1%"

for %%I in ("%PATCH_ROOT%\..") do set "REPO_ROOT=%%~fI"

set "PATCH_LIST=%TEMP%\patch_list_%RANDOM%_%RANDOM%.txt"
set "CHECK_LOG=%TEMP%\patch_check_%RANDOM%_%RANDOM%.log"
set "APPLY_LOG=%TEMP%\patch_apply_%RANDOM%_%RANDOM%.log"

if not exist "%PATCH_ROOT%" (
    echo [ERROR] Patch directory not found: "%PATCH_ROOT%"
    pause
    exit /b 1
)

pushd "%REPO_ROOT%" >nul 2>&1

git rev-parse --is-inside-work-tree >nul 2>&1
if errorlevel 1 (
    echo [ERROR] This script must be run from inside a git repository.
    popd >nul 2>&1
    pause
    exit /b 1
)

dir /b /s /a-d "%PATCH_ROOT%\*.patch" > "%PATCH_LIST%"

if errorlevel 1 (
    echo [INFO] No patch files found under "%PATCH_ROOT%"
    del "%PATCH_LIST%" >nul 2>&1
    popd >nul 2>&1
    pause
    exit /b 0
)

set "TOTAL_COUNT=0"
set "APPLIED_COUNT=0"
set "SKIPPED_COUNT=0"

echo.
echo [INFO] Searching for patches under "%PATCH_ROOT%"
echo [INFO] Applying from repo root "%REPO_ROOT%"
echo.

for /f "usebackq delims=" %%F in ("%PATCH_LIST%") do (
    set /a TOTAL_COUNT+=1

    echo ==================================================
    echo [!TOTAL_COUNT!] Checking patch:
    echo %%F
    echo ==================================================

    git apply --check "%%F" > "%CHECK_LOG%" 2>&1
    if errorlevel 1 (
        set /a SKIPPED_COUNT+=1
        echo [SKIPPED] %%F
        echo [REASON]
        type "%CHECK_LOG%"
        echo.
    ) else (
        git apply --whitespace=nowarn "%%F" > "%APPLY_LOG%" 2>&1
        if errorlevel 1 (
            set /a SKIPPED_COUNT+=1
            echo [SKIPPED] %%F
            echo [REASON]
            type "%APPLY_LOG%"
            echo.
        ) else (
            set /a APPLIED_COUNT+=1
            echo [APPLIED] %%F
            echo.
        )
    )
)

del "%PATCH_LIST%" >nul 2>&1
del "%CHECK_LOG%" >nul 2>&1
del "%APPLY_LOG%" >nul 2>&1

echo ==================================================
echo [SUMMARY]
echo Total patches:   !TOTAL_COUNT!
echo Applied patches: !APPLIED_COUNT!
echo Skipped patches: !SKIPPED_COUNT!
echo ==================================================

popd >nul 2>&1
pause
exit /b 0