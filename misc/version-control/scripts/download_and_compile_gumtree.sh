#!/bin/bash
set -u
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
GUM_TREE_DIR="$SCRIPT_DIR/../gumtree"
mkdir -p "$GUM_TREE_DIR"

git clone git@github.com:dimitar-asenov/gumtree.git "$GUM_TREE_DIR"

pushd "$GUM_TREE_DIR"
git checkout envision-samples
gradle build
popd
