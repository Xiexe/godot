#!/usr/bin/env bash

# Exit immediately if a command exits with a non-zero status.
set -e

# --- Configuration ---
NUGET_SOURCE_NAME="nuget_godot_cache"
NUGET_LOCAL_DIR="nuget_godot_cache" # The local folder to store packages
PRECISION="double"
# --------------------

# Get the absolute path to the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Change to the script's directory to ensure all relative paths are correct
cd "$SCRIPT_DIR"

# 1. Add the local NuGet source
#    - We use $SCRIPT_DIR to replicate the functionality of Windows' %~dp0
#    - We first try to remove any existing source with the same name to avoid errors
echo "==> Setting up local NuGet source..."
dotnet nuget remove source "$NUGET_SOURCE_NAME" 2>/dev/null || true
dotnet nuget add source "$SCRIPT_DIR/$NUGET_LOCAL_DIR" --name "$NUGET_SOURCE_NAME"

# 2. Build the editor using SCons for macOS
#    - Detect the architecture (arm64 for Apple Silicon, x86_64 for Intel)
#    - Change platform from 'windows' to 'macos'
#    - Add 'generate_bundle=yes' to create a proper Godot.app package
ARCH=$(uname -m)
echo "==> Building Godot editor for macOS (arch: $ARCH)..."
scons platform=macos arch="$ARCH" module_mono_enabled=yes precision="$PRECISION" generate_bundle=yes

# 3. Generate the C# glue code
#    - The executable path is different on macOS (no .exe extension, platform/arch in name)
EDITOR_BINARY="bin/godot.macos.editor.$PRECISION.$ARCH.mono"
echo "==> Generating C# glue using: $EDITOR_BINARY"
if [[ ! -f "$EDITOR_BINARY" ]]; then
    echo "ERROR: Editor binary not found. SCons build may have failed."
    exit 1
fi
"$EDITOR_BINARY" --headless --generate-mono-glue modules/mono/glue

# 4. Build the C# assemblies and push them to our local NuGet source
#    - On macOS/Linux, it's best practice to explicitly call 'python3'
echo "==> Building C# assemblies and pushing to local NuGet source..."
python3 "./modules/mono/build_scripts/build_assemblies.py" \
    --godot-output-dir="./bin" \
    --push-nupkgs-local "$NUGET_SOURCE_NAME" \
    --precision="$PRECISION"

# 5. Pause for user to see the output (equivalent to Windows 'pause')
echo "==> Build complete."
read -p "Press Enter to continue..."