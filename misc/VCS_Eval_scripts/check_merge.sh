#!/bin/bash

# $1 is SHA-1 of a merge-commit, working directory should be the root of a git repository.

#parents=$(git show --format=%P $1 | tr " " "\n")
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

changed=($(comm -12 <(git diff --name-only $base ${parents[0]} | sort) <(git diff --name-only $base ${parents[1]} | sort)))

#echo Files changed by both branches:
#for f in ${changed[@]}; do
#	echo $f
#done

if [ -z $changed ]; then
	#echo No concurrently changed files; stop.
	exit 0
fi

rootDir="../merges"
#rm -rf $rootDir
if [ ! -d $rootDir ]; then
	mkdir $rootDir
fi
revDir="${rootDir}/${1}"
mkdir $revDir
(
echo base: $base
echo master: ${parents[0]}
echo dev: ${parents[1]}
echo merge: $1
) > "${revDir}/commits"

for f in ${changed[@]}; do
	if [[ $f == *.java ]]; then
		fileDir="${revDir}/${f##*/}"
		mkdir $fileDir
		git show "${base}:${f}" > "${fileDir}/base.java"
		git show "${parents[0]}:${f}" > "${fileDir}/master.java"
		git show "${parents[1]}:${f}" > "${fileDir}/dev.java"
		git show "${1}:${f}" > "${fileDir}/devMerged.java"
	fi
done

exit 0

# get both parents
# get base
# check if there are java files modified by both branches
# merge with git and envision
# compare git vs. envision vs. history