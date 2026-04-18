#!/usr/bin/env bash
set -euo pipefail

# --- Configuration ---
NUGET_SOURCE_NAME="nuget_godot_cache"
NUGET_LOCAL_DIR="nuget_godot_cache"
PRECISION="double"     # "single" or "double"
USE_LLVM="yes"         # "yes" or "no"
# --------------------

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

mkdir -p "$NUGET_LOCAL_DIR"

echo "==> Setting up local NuGet source..."
dotnet nuget remove source "$NUGET_SOURCE_NAME" >/dev/null 2>&1 || true
dotnet nuget add source "$SCRIPT_DIR/$NUGET_LOCAL_DIR" --name "$NUGET_SOURCE_NAME" >/dev/null

# Godot uses specific arch names. Normalize common Linux outputs.
UNAME_ARCH="$(uname -m)"
case "$UNAME_ARCH" in
  x86_64) ARCH="x86_64" ;;
  aarch64|arm64) ARCH="arm64" ;;
  *) echo "ERROR: Unsupported architecture: $UNAME_ARCH"; exit 1 ;;
esac

if [[ "$USE_LLVM" != "yes" && "$USE_LLVM" != "no" ]]; then
  echo "ERROR: USE_LLVM must be 'yes' or 'no' (got: $USE_LLVM)"
  exit 1
fi

echo "==> Building Godot editor for Linux (arch: $ARCH, precision: $PRECISION, use_llvm: $USE_LLVM)..."
scons -j"$(nproc)" \
  platform=linuxbsd \
  use_llvm="$USE_LLVM" \
  arch="$ARCH" \
  module_mono_enabled=yes \
  precision="$PRECISION"

# Compute the expected output name based on use_llvm.
LLVM_SUFFIX=""
if [[ "$USE_LLVM" == "yes" ]]; then
  LLVM_SUFFIX=".llvm"
fi

EDITOR_BINARY="bin/godot.linuxbsd.editor.${PRECISION}.${ARCH}${LLVM_SUFFIX}.mono"

# If naming differs due to branch/version, fall back to discovery.
if [[ ! -f "$EDITOR_BINARY" ]]; then
  echo "==> Editor binary not found at: $EDITOR_BINARY"
  echo "==> Trying to locate a matching Linux editor binary in ./bin ..."

  # Prefer exact precision/arch, accept either .mono or .llvm.mono, but avoid templates.
  EDITOR_BINARY="$(
    ls -1 \
      "bin/godot.linuxbsd.editor.${PRECISION}.${ARCH}.mono" \
      "bin/godot.linuxbsd.editor.${PRECISION}.${ARCH}.llvm.mono" \
      2>/dev/null \
    | head -n 1 || true
  )"

  # Last resort: broader pattern match (still anchored to editor + arch).
  if [[ -z "${EDITOR_BINARY:-}" || ! -f "$EDITOR_BINARY" ]]; then
    EDITOR_BINARY="$(ls -1 bin/godot.linuxbsd.editor.*."$ARCH".*.mono 2>/dev/null | head -n 1 || true)"
  fi
fi

if [[ -z "${EDITOR_BINARY:-}" || ! -f "$EDITOR_BINARY" ]]; then
  echo "ERROR: Could not find built editor binary in ./bin"
  ls -la bin || true
  exit 1
fi

echo "==> Generating C# glue using: $EDITOR_BINARY"
"$EDITOR_BINARY" --headless --generate-mono-glue modules/mono/glue

echo "==> Building C# assemblies and pushing to local NuGet source..."
python3 "./modules/mono/build_scripts/build_assemblies.py" \
  --godot-output-dir="./bin" \
  --push-nupkgs-local "$NUGET_SOURCE_NAME" \
  --precision="$PRECISION"

echo "==> Build complete."
