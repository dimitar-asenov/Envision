#!/bin/bash

# This script is used in dump_repo.sh. There should be no need to run it manually.
# The working directory should be the root of a git repository.
# It takes one argument; the SHA-1 of a merge-commit of the working directory repo.
# More information on what it does can be found in the dump_repo.sh script.

parents=($(git show --format=%P $1))
#echo Parent 0: ${parents[0]}
#echo Parent 1: ${parents[1]}
base=$(git merge-base ${parents[0]} ${parents[1]})
#echo Base    : $base

# stop if no base exists
if [ -z $base ]; then
	#echo No base; stop.
	exit 0
fi

# stop if not a real merge
if [ $base == ${parents[0]} ] || [ $base == ${parents[1]} ]; then
	#echo Not real merge; stop.
	exit 0
fi

# Compute the list of files that are modified by both branches.
changed=($(comm -12 <(git diff --name-only $base ${parents[0]} | sort) <(git diff --name-only $base ${parents[1]} | sort)))

# This is debug code
#echo Files changed by both branches:
#for f in ${changed[@]}; do
#	echo $f
#done

# stop if no files are changed by both branches.
if [ -z $changed ]; then
	#echo No concurrently changed files; stop.
	exit 0
fi

# create necessary directories
rootDir="../merges"
if [ ! -d $rootDir ]; then
	mkdir $rootDir
fi
revDir="${rootDir}/${1}"
mkdir $revDir
( # write the four relevant commit IDs to the commits file
echo base: $base
echo master: ${parents[0]}
echo dev: ${parents[1]}
echo merge: $1
) > "${revDir}/commits"

# dump each relevant version of each interesting java file
for f in ${changed[@]}; do
	if [[ $f == *.java ]]; then
		fileDir="${revDir}/${f##*/}"
		counter=1
		while [ -d "${fileDir}" ]; do
			((counter++))
			fileDir="${revDir}/${f##*/}-$counter"
		done
		mkdir $fileDir
		
		git show "${base}:${f}" > "${fileDir}/base.java"
		git show "${parents[0]}:${f}" > "${fileDir}/master.java"
		git show "${parents[1]}:${f}" > "${fileDir}/dev.java"
		git show "${1}:${f}" > "${fileDir}/devMerged.java"
	fi
done

exit 0
