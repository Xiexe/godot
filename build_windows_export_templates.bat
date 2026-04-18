@echo off
setlocal

set "ROOT=%~dp0"
set "OUTDIR=%ROOT%bin\template"
set "TRACY_DIR=%ROOT%tracy"

if not exist "%OUTDIR%" mkdir "%OUTDIR%"

if not exist "%TRACY_DIR%\public\tracy\Tracy.hpp" (
    echo Tracy source not found at:
    echo   %TRACY_DIR%
    echo Expected file:
    echo   %TRACY_DIR%\public\tracy\Tracy.hpp
    goto :fail
)

rem Build templates with Tracy support
py -m SCons platform=windows target=template_debug arch=x86_64 ^
    module_mono_enabled=yes precision=double ^
    debug_symbols=yes ^
    profiler=tracy profiler_path=%TRACY_DIR%
if errorlevel 1 goto :fail

py -m SCons platform=windows target=template_release arch=x86_64 ^
    module_mono_enabled=yes precision=double
if errorlevel 1 goto :fail

rem Move + rename (Debug)
move /y "bin\godot.windows.template_debug.double.x86_64.mono.console.exe" "%OUTDIR%\windows_debug_x86_64_console.exe"
move /y "bin\godot.windows.template_debug.double.x86_64.mono.exe" "%OUTDIR%\windows_debug_x86_64.exe"

rem Move + rename (Release)
move /y "bin\godot.windows.template_release.double.x86_64.mono.console.exe" "%OUTDIR%\windows_release_x86_64_console.exe"
move /y "bin\godot.windows.template_release.double.x86_64.mono.exe" "%OUTDIR%\windows_release_x86_64.exe"

echo Done.
pause
exit /b 0

:fail
echo Build failed.
pause
exit /b 1