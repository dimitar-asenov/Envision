#!/bin/bash
set -e

################################################################################
# Set some variables
################################################################################
THIS_SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
TRANSFER_SCRIPT="$THIS_SCRIPT_DIR"/transfer-dependencies.py

################################################################################
# Check input
################################################################################
if [ "$#" -ne 2 ]; then
    echo "Invalid number of arguments"
    echo ""
    echo "Usage: "
    echo "       transfer-all-dependencies.sh originalSourceDir exportedSourceDir"
    echo ""
    exit
fi

ORIGINAL_SOURCE_DIR=$1
EXPORTED_SOURCE_DIR=$2

# Check if directories exist
if [ ! -d "$ORIGINAL_SOURCE_DIR" ]; then
  echo "$ORIGINAL_SOURCE_DIR does not exist or is not a directory"
  exit 1
fi

if [ ! -d "$EXPORTED_SOURCE_DIR" ]; then
  echo "$EXPORTED_SOURCE_DIR does not exist or is not a directory"
  exit 1
fi

################################################################################
# Transfer dependencies
################################################################################
echo "Transferring dependencies"
find $EXPORTED_SOURCE_DIR \( -name "*.h" -o -name "*.cpp" -o -name "*.hpp" \) -exec bash -c 'python3 $0 ${1/$2/$3} $1 &' $TRANSFER_SCRIPT {} "$EXPORTED_SOURCE_DIR" "$ORIGINAL_SOURCE_DIR" \;