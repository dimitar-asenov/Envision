#!/bin/bash
set -e

################################################################################
# Set some variables
################################################################################
THIS_SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ENVISION_SRC_DIR="$( dirname "$( dirname "${THIS_SCRIPT_DIR}")")"

################################################################################
# Run clang-tidy
#
# Change the command argument to run different tidy checks and fixes
#
################################################################################
echo "Running clang-tidy"
clang-tidy-3.8 -fix -checks=-*,google-readability-braces-around-statements `find $ENVISION_SRC_DIR \( -name "*.cpp" \)`
echo "Done"