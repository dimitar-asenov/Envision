#!/bin/bash

THIS_SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
TARGET="$THIS_SCRIPT_DIR"/../../compile_commands.json

################################################################################
# Check input
################################################################################
if [ "$#" -ne 1 ]; then
    echo "Invalid number of arguments"
    echo ""
    echo "Usage: "
    echo "       setup_compile_database.sh compile_commands.json"
    echo ""
    exit
fi

ORIGINAL_PATH=$1
cp $ORIGINAL_PATH $TARGET
sed -i 's/-O2 /-O2 -fparse-all-comments /g' $TARGET