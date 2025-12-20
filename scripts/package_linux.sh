#!/bin/bash
set -e

# Get the project root directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$( dirname "$SCRIPT_DIR" )"
cd "$PROJECT_ROOT"

echo "Working in: $PROJECT_ROOT"

BUILD_DIR="build_release"
DIST_DIR="dist"
EXEC_NAME="Huffman"
TAR_NAME="Huffman-Linux.tar.gz"
QT_PATH=$1

# Clean
rm -rf $BUILD_DIR $DIST_DIR $TAR_NAME

# Configure
echo "Configuring CMake..."
CMAKE_ARGS="-B $BUILD_DIR -DCMAKE_BUILD_TYPE=Release"
if [ ! -z "$QT_PATH" ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_PREFIX_PATH=$QT_PATH"
    echo "Using Qt Path: $QT_PATH"
fi

cmake $CMAKE_ARGS

# Build
echo "Building project..."
cmake --build $BUILD_DIR --config Release

# Prepare Dist
mkdir -p $DIST_DIR

# Copy Executable
cp "$BUILD_DIR/$EXEC_NAME" $DIST_DIR/

# Copy Resources
if [ -d "$BUILD_DIR/resource" ]; then
    cp -r "$BUILD_DIR/resource" $DIST_DIR/
fi

# Create Tarball
echo "Creating tarball..."
tar -czf $TAR_NAME -C $DIST_DIR .

echo "Packaging complete: $TAR_NAME"
