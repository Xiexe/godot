#!/usr/bin/env bash
set -euo pipefail

NUGET_SOURCE_NAME="nuget_godot_cache"
NUGET_LOCAL_DIR="nuget_godot_cache"
PRECISION="double"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

mkdir -p "$NUGET_LOCAL_DIR" "bin/template"

echo "==> Setting up local NuGet source..."
dotnet nuget remove source "$NUGET_SOURCE_NAME" >/dev/null 2>&1 || true
dotnet nuget add source "$SCRIPT_DIR/$NUGET_LOCAL_DIR" --name "$NUGET_SOURCE_NAME" >/dev/null

case "$(uname -m)" in
    x86_64) ARCH="x86_64" ;;
    arm64|aarch64) ARCH="arm64" ;;
    *) echo "ERROR: Unsupported architecture: $(uname -m)"; exit 1 ;;
esac

SCONS=(
    python3 -m SCons
    platform=macos
    module_mono_enabled=yes
    precision="$PRECISION"
)

echo "==> Building Godot editor for macOS (arch: $ARCH, precision: $PRECISION)..."
"${SCONS[@]}" target=editor arch="$ARCH" debug_symbols=yes

PRECISION_SUFFIX=""
if [[ "$PRECISION" == "double" ]]; then
    PRECISION_SUFFIX=".double"
fi

EDITOR_BINARY="bin/godot.macos.editor${PRECISION_SUFFIX}.${ARCH}.mono"
if [[ ! -f "$EDITOR_BINARY" ]]; then
    echo "ERROR: Editor binary not found: $EDITOR_BINARY"
    exit 1
fi

echo "==> Generating C# glue..."
"$EDITOR_BINARY" --headless --generate-mono-glue "$SCRIPT_DIR/modules/mono/glue"

echo "==> Building C# assemblies..."
python3 "$SCRIPT_DIR/modules/mono/build_scripts/build_assemblies.py" \
    --godot-output-dir="$SCRIPT_DIR/bin" \
    --push-nupkgs-local "$SCRIPT_DIR/$NUGET_LOCAL_DIR" \
    --precision="$PRECISION" \
    --godot-platform=macos

echo "==> Assembling the macOS editor bundle..."
"${SCONS[@]}" target=editor arch="$ARCH" debug_symbols=yes generate_bundle=yes

echo "==> Building universal macOS export templates..."
"${SCONS[@]}" target=template_debug arch=x86_64 debug_symbols=yes
"${SCONS[@]}" target=template_release arch=x86_64 debug_symbols=no
"${SCONS[@]}" target=template_debug arch=arm64 debug_symbols=yes
"${SCONS[@]}" target=template_release arch=arm64 debug_symbols=no generate_bundle=yes

TEMPLATE_ZIP="bin/godot_macos_mono.zip"
if [[ "$PRECISION" == "double" ]]; then
    TEMPLATE_ZIP="bin/godot_macos_double_mono.zip"
fi
if [[ ! -f "$TEMPLATE_ZIP" ]]; then
    echo "ERROR: macOS export-template archive not found: $TEMPLATE_ZIP"
    exit 1
fi

mv -f "$TEMPLATE_ZIP" "bin/template/macos.zip"

echo "==> Build complete."
