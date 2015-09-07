#!/bin/bash

#rm -rf "/tmp/${2}"
mkdir "/tmp/${2}"
cd "/tmp/${2}"

if [ ! -d repo ]; then
	git clone $1 repo
fi
cd repo

#git rev-list --merges HEAD > /tmp/merges # debug code
git rev-list --merges HEAD | xargs -L 1 ~/Envision/misc/VCS_Eval_scripts/check_merge.sh
