#!/bin/bash
set -e

################################################################################
# Set some variables
################################################################################
THIS_SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
SOURCE_SORT_SCRIPT="$THIS_SCRIPT_DIR"/source-sort.py
CLANG_FORMAT_STYLE="$THIS_SCRIPT_DIR"/_clang-format

################################################################################
# Check input
################################################################################
if [ "$#" -ne 2 ]; then
    echo "Invalid number of arguments"
    echo ""
    echo "Usage: "
    echo "       compare-cpp-source-trees.sh originalSourceDir exportedSourceDir"
    echo ""
    echo "The sorted output files, suitable for comparison, will be in /tmp/envision_compare"
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
# Prepare an empty temp directory
################################################################################
TEMPDIR=/tmp/envision_compare
rm -rf $TEMPDIR
SORTED_ORIGINAL=$TEMPDIR/original
SORTED_EXPORTED=$TEMPDIR/exported
mkdir -p $SORTED_ORIGINAL
mkdir -p $SORTED_EXPORTED

################################################################################
# Copy necessary files
################################################################################
echo "Copying files"
cp $CLANG_FORMAT_STYLE $TEMPDIR/
cp -r $EXPORTED_SOURCE_DIR/* $SORTED_EXPORTED

# Only copy those original files for which there is an equivalent export file
find $SORTED_EXPORTED \( -name "*.h" -o -name "*.cpp" \) -exec bash -c 'mkdir -p `dirname ${0/$1/$2}`; cp ${0/$1/$3} ${0/$1/$2}' {} "$SORTED_EXPORTED" "$SORTED_ORIGINAL" "$ORIGINAL_SOURCE_DIR" \;

################################################################################
# Format everything uniformly. It's important to use the provided
# _clang-format style file as the next step relies on it.
################################################################################
echo "Running clang-format"
clang-format-3.8 -i -style=file `find $TEMPDIR \( -name "*.h" -o -name "*.cpp" \)`

################################################################################
# Sort "declarations" so that the files are comparable
################################################################################
echo "Sorting 'declarations'"
# The original sources are sorted without a 3rd argument: all includes will be kept
find $SORTED_ORIGINAL \( -name "*.h" -o -name "*.cpp" \) -exec bash -c 'python2 $0 $1 $2 &' $SOURCE_SORT_SCRIPT {} {} \;

# The exported sources are sorted providing the name of the original source as a third argument.
# Any includes which only appear in the exported source will be discarded.
# This is needed since we include all necessary dependencies directly, even if they are already included in other files.
find $SORTED_EXPORTED \( -name "*.h" -o -name "*.cpp" \) -exec bash -c 'python2 $0 $1 $1 ${1/$2/$3} &' $SOURCE_SORT_SCRIPT {} "$SORTED_EXPORTED" "$SORTED_ORIGINAL" \;
