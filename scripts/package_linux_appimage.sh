#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
cd "$PROJECT_ROOT"

BUILD_DIR="build_release"
APPDIR="AppDir"
APP_NAME="Huffman"
APPIMAGE_NAME="Huffman-Linux-x86_64.AppImage"
TOOLS_DIR="${PROJECT_ROOT}/.tools"
LINUXDEPLOY="${TOOLS_DIR}/linuxdeploy-x86_64.AppImage"
LINUXDEPLOY_PLUGIN_QT="${TOOLS_DIR}/linuxdeploy-plugin-qt-x86_64.AppImage"

mkdir -p "$TOOLS_DIR"

QT_PATH="${1:-}"

echo "Working in: $PROJECT_ROOT"
rm -rf "$BUILD_DIR" "$APPDIR" "$APPIMAGE_NAME"

CMAKE_ARGS=(-S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release)
if [[ -n "$QT_PATH" ]]; then
    CMAKE_ARGS+=("-DCMAKE_PREFIX_PATH=$QT_PATH")
    echo "Using Qt Path: $QT_PATH"
fi

echo "Configuring CMake..."
cmake "${CMAKE_ARGS[@]}"

echo "Building project..."
cmake --build "$BUILD_DIR" --config Release

if [[ ! -f "$LINUXDEPLOY" ]]; then
    curl -L "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage" -o "$LINUXDEPLOY"
fi

if [[ ! -f "$LINUXDEPLOY_PLUGIN_QT" ]]; then
    curl -L "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage" -o "$LINUXDEPLOY_PLUGIN_QT"
fi

chmod +x "$LINUXDEPLOY" "$LINUXDEPLOY_PLUGIN_QT"

mkdir -p "$APPDIR/usr/bin"
mkdir -p "$APPDIR/usr/share/applications"
mkdir -p "$APPDIR/usr/share/icons/hicolor/scalable/apps"

cp "$BUILD_DIR/$APP_NAME" "$APPDIR/usr/bin/$APP_NAME"
cp "resource/huffman.desktop" "$APPDIR/usr/share/applications/huffman.desktop"
cp "resource/huffman.svg" "$APPDIR/usr/share/icons/hicolor/scalable/apps/huffman.svg"

if [[ -d "$BUILD_DIR/resource" ]]; then
    cp -R "$BUILD_DIR/resource" "$APPDIR/usr/bin/resource"
fi

export QMAKE="${QMAKE:-$(command -v qmake6 || command -v qmake || true)}"
if [[ -z "$QMAKE" ]]; then
    echo "qmake/qmake6 not found in PATH."
    exit 1
fi

export APPIMAGE_EXTRACT_AND_RUN=1
export OUTPUT="$APPIMAGE_NAME"

"$LINUXDEPLOY" \
    --appdir "$APPDIR" \
    --desktop-file "$APPDIR/usr/share/applications/huffman.desktop" \
    --icon-file "$APPDIR/usr/share/icons/hicolor/scalable/apps/huffman.svg" \
    --plugin qt \
    --output appimage

echo "Packaging complete: $APPIMAGE_NAME"
