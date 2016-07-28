#!/bin/bash
set -u
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
GUMTREE_SRC_DIR="$SCRIPT_DIR/../gumtree_src"
GUMTREE_BIN_DIR="$SCRIPT_DIR/../gumtree_bin"

JAVAIMPORTTOOL_BIN_DIR="$SCRIPT_DIR/../JavaImportToolBin"
JAVAIMPORTTOOL_SRC_DIR="$SCRIPT_DIR/../../../JavaImportTool"

if [ ! -d "$GUMTREE_SRC_DIR" ]; then
	echo "=== No GumTree detected ... cloning ==="

	mkdir -p "$GUMTREE_SRC_DIR"
	git clone git@github.com:dimitar-asenov/gumtree.git "$GUMTREE_SRC_DIR"

	pushd "$GUMTREE_SRC_DIR"
	git checkout envision-samples
	popd
fi

echo "=== Building GumTree ==="
pushd "$GUMTREE_SRC_DIR"

./gradlew build

rm -rf "$GUMTREE_BIN_DIR"
mkdir -p "$GUMTREE_BIN_DIR"
tar -xf $GUMTREE_SRC_DIR/dist/build/distributions/gumtree-2.1.0-SNAPSHOT.tar -C "$GUMTREE_BIN_DIR"

popd

echo "=== Building JavaImportTool ==="
pushd $JAVAIMPORTTOOL_SRC_DIR

gradle distTar

rm -rf "$JAVAIMPORTTOOL_BIN_DIR"
mkdir -p "$JAVAIMPORTTOOL_BIN_DIR"
tar -xf $JAVAIMPORTTOOL_SRC_DIR/build/distributions/JavaImportTool.tar -C "$JAVAIMPORTTOOL_BIN_DIR"

popd
