#!/bin/bash

# This scripts takes two arguments. The first argument is the url of a git repository. The second argument names the folder to be created in /tmp/ which will hold the cloned repo and dumped merges.
# For each merge commit and java file, the base, merged and both branch versions of the file are dumped into a folder named after the file. These folders are further grouped by merge commit ID.
# The dumped files represent non-trivial single-file merge situations.
# The so produced dumps can then be used by the diff_envision_dev.sh and diff_git_dev.sh scripts to evaluate and compare the different merge algorithms.

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

mkdir "/tmp/${2}"
cd "/tmp/${2}"

if [ ! -d repo ]; then
	git clone $1 repo
fi
rm -rf merges
cd repo

git rev-list --merges HEAD | xargs -L 1 "$SCRIPT_DIR"/dump_merge.sh
