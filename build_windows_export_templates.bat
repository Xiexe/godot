@echo off
setlocal

set "ROOT=%~dp0"
set "OUTDIR=%ROOT%bin\template"

if not exist "%OUTDIR%" mkdir "%OUTDIR%"

py -m SCons platform=windows target=template_debug arch=x86_64 ^
    module_mono_enabled=yes precision=double ^
    debug_symbols=yes
if errorlevel 1 goto :fail

py -m SCons platform=windows target=template_release arch=x86_64 ^
    module_mono_enabled=yes precision=double ^
    debug_symbols=no
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
