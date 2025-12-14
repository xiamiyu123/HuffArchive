#!/bin/bash
set -e

BUILD_DIR="build_release"
DIST_DIR="dist"
EXEC_NAME="Huffman"
TAR_NAME="Huffman-Linux.tar.gz"

# Clean
rm -rf $BUILD_DIR $DIST_DIR $TAR_NAME

# Configure
echo "Configuring CMake..."
cmake -B $BUILD_DIR -DCMAKE_BUILD_TYPE=Release

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
