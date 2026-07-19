@echo off
setlocal

set "ROOT=%~dp0"
set "NUGET_CACHE=%ROOT%nuget_godot_cache"
set "EDITOR_EXE=%ROOT%bin\godot.windows.editor.double.x86_64.mono.exe"

if not exist "%NUGET_CACHE%" mkdir "%NUGET_CACHE%"

dotnet nuget list source | findstr /C:"nuget_godot_cache" >nul
if errorlevel 1 (
    dotnet nuget add source "%NUGET_CACHE%" --name nuget_godot_cache
    if errorlevel 1 goto :fail
)

py -m SCons platform=windows target=editor arch=x86_64 ^
    module_mono_enabled=yes precision=double ^
    debug_symbols=yes

if errorlevel 1 goto :fail

if not exist "%EDITOR_EXE%" (
    echo Editor executable not found:
    echo   %EDITOR_EXE%
    goto :fail
)

"%EDITOR_EXE%" --headless --generate-mono-glue modules/mono/glue
if errorlevel 1 goto :fail

python "%ROOT%modules\mono\build_scripts\build_assemblies.py" ^
    --godot-output-dir="%ROOT%bin" ^
    --push-nupkgs-local "%NUGET_CACHE%" ^
    --precision=double ^
    --godot-platform=windows
if errorlevel 1 goto :fail

call "%ROOT%build_windows_export_templates.bat"
if errorlevel 1 goto :fail

echo Done.
pause
exit /b 0

:fail
echo Build failed.
pause
exit /b 1
