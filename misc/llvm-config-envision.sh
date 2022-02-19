#!/bin/bash

SUPPORTED_VERSION="13"
COMMAND_TO_USE=""

#This checks whether a particular llvm-config command exists in the system and if it works with the supported version
check() {
	if [ "$COMMAND_TO_USE" == "" ]; then
		if hash $1 2>/dev/null; then
			if [ `$1 --version | grep "^$SUPPORTED_VERSION" -c` == "1" ]; then
				COMMAND_TO_USE=$1
			fi
		fi
	fi
}

# This is the list of llvm-config commands that we'll try
check "llvm-config"
check "llvm-config-$SUPPORTED_VERSION"

# Fail if not found
if [ "$COMMAND_TO_USE" == "" ]; then
	>&2 echo "Could not find compatible llvm-config"
	exit 1
fi

$COMMAND_TO_USE "$@"
