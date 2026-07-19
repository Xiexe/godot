#!/usr/bin/env bash
set -euo pipefail

NUGET_SOURCE_NAME="nuget_godot_cache"
NUGET_LOCAL_DIR="nuget_godot_cache"
PRECISION="double"
USE_LLVM="yes"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

mkdir -p "$NUGET_LOCAL_DIR" "bin/template"

echo "==> Setting up local NuGet source..."
dotnet nuget remove source "$NUGET_SOURCE_NAME" >/dev/null 2>&1 || true
dotnet nuget add source "$SCRIPT_DIR/$NUGET_LOCAL_DIR" --name "$NUGET_SOURCE_NAME" >/dev/null

case "$(uname -m)" in
    x86_64) ARCH="x86_64" ;;
    aarch64|arm64) ARCH="arm64" ;;
    *) echo "ERROR: Unsupported architecture: $(uname -m)"; exit 1 ;;
esac

if [[ "$USE_LLVM" != "yes" && "$USE_LLVM" != "no" ]]; then
    echo "ERROR: USE_LLVM must be 'yes' or 'no' (got: $USE_LLVM)"
    exit 1
fi

SCONS=(
    python3 -m SCons
    -j"$(nproc)"
    platform=linuxbsd
    arch="$ARCH"
    use_llvm="$USE_LLVM"
    module_mono_enabled=yes
    precision="$PRECISION"
)

echo "==> Building Godot editor for Linux (arch: $ARCH, precision: $PRECISION, use_llvm: $USE_LLVM)..."
"${SCONS[@]}" target=editor debug_symbols=yes

LLVM_SUFFIX=""
if [[ "$USE_LLVM" == "yes" ]]; then
    LLVM_SUFFIX=".llvm"
fi

PRECISION_SUFFIX=""
if [[ "$PRECISION" == "double" ]]; then
    PRECISION_SUFFIX=".double"
fi

EDITOR_BINARY="bin/godot.linuxbsd.editor${PRECISION_SUFFIX}.${ARCH}${LLVM_SUFFIX}.mono"
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
    --godot-platform=linuxbsd

echo "==> Building Linux export templates..."
"${SCONS[@]}" target=template_debug debug_symbols=yes
"${SCONS[@]}" target=template_release debug_symbols=no

DEBUG_TEMPLATE="bin/godot.linuxbsd.template_debug${PRECISION_SUFFIX}.${ARCH}${LLVM_SUFFIX}.mono"
RELEASE_TEMPLATE="bin/godot.linuxbsd.template_release${PRECISION_SUFFIX}.${ARCH}${LLVM_SUFFIX}.mono"

if [[ ! -f "$DEBUG_TEMPLATE" || ! -f "$RELEASE_TEMPLATE" ]]; then
    echo "ERROR: One or more Linux export-template binaries were not generated."
    exit 1
fi

mv -f "$DEBUG_TEMPLATE" "bin/template/linux_debug.${ARCH}"
mv -f "$RELEASE_TEMPLATE" "bin/template/linux_release.${ARCH}"

echo "==> Build complete."
