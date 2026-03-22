#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
cd "$PROJECT_ROOT"

BUILD_DIR="build_release"
APP_NAME="Huffman"
APP_BUNDLE="${BUILD_DIR}/${APP_NAME}.app"
DMG_NAME="Huffman-macOS.dmg"
QT_PATH="${1:-}"

echo "Working in: $PROJECT_ROOT"
rm -rf "$BUILD_DIR" "$DMG_NAME"

CMAKE_ARGS=(-S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release)
if [[ -n "$QT_PATH" ]]; then
    CMAKE_ARGS+=("-DCMAKE_PREFIX_PATH=$QT_PATH")
    echo "Using Qt Path: $QT_PATH"
fi

echo "Configuring CMake..."
cmake "${CMAKE_ARGS[@]}"

echo "Building project..."
cmake --build "$BUILD_DIR" --config Release

if [[ ! -d "$APP_BUNDLE" ]]; then
    echo "Expected app bundle not found: $APP_BUNDLE"
    exit 1
fi

MACDEPLOYQT="${MACDEPLOYQT:-$(command -v macdeployqt || true)}"
if [[ -z "$MACDEPLOYQT" ]]; then
    echo "macdeployqt not found in PATH."
    exit 1
fi

"$MACDEPLOYQT" "$APP_BUNDLE" -verbose=2 -dmg

if [[ -f "${BUILD_DIR}/${APP_NAME}.dmg" ]]; then
    mv "${BUILD_DIR}/${APP_NAME}.dmg" "$DMG_NAME"
elif [[ -f "${APP_NAME}.dmg" ]]; then
    mv "${APP_NAME}.dmg" "$DMG_NAME"
else
    echo "macdeployqt completed but no DMG was produced."
    exit 1
fi

echo "Packaging complete: $DMG_NAME"
