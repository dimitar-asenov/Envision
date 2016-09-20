#!/bin/bash

# This script takes as the single argument a directory previously populated by the dump_repo.sh script.
# It runs the default Git merge algorithm and compares the result to the definitive developer-merged verison.
# If the versions are different, an entry specifying the revision and file is written to merges/issues_git.
# Additionally, it writes an entry for all encountered files to merges/all which is helpful when using the comm command with these kind of files.

rm -rf "${1}/merges/all"
rm -rf "${1}/merges/issues_git"

merges="${1}/merges/*"
numFileMergesAttempted=0
for m in $merges; do
	#echo Revision $m
	for fdir in ${m}/*; do
		#echo File $fdir
		if [ -d "${fdir}" ] && [ -s "${fdir}/base.java" ] && [ -s "${fdir}/dev.java" ] && [ -s "${fdir}/master.java" ] && [ -s "${fdir}/devMerged.java" ]; then
			((numFileMergesAttempted++))
			
			(
				cd $fdir
				cp master.java gitMerged.java
				git merge-file -L master.java --quiet gitMerged.java base.java dev.java
				gitReturnValue=$?
				diff devMerged.java gitMerged.java > diff_dev_git
				if [ -s diff_dev_git ]; then
					if (( gitReturnValue > 0 )); then
						conflictString='conflict'
					else
						conflictString='no-conflict'
					fi
					echo "${m##*/}/${fdir##*/}" >> ../../issues_git
					echo "${m##*/}/${fdir##*/}" "$conflictString" >> ../../issues_git 
				fi
				echo "${m##*/}/${fdir##*/}" >> ../../all
				rm diff_dev_git
			)
		fi
	done
done

echo "Attempted to merge a total of $numFileMergesAttempted files using GIT"
#sort "${1}/merges/all" > "${1}/merges/all2"
#mv "${1}/merges/all2" "${1}/merges/all"
#sort "${1}/merges/issues_git" > "${1}/merges/issues_git2"
#mv "${1}/merges/issues_git2" "${1}/merges/issues_git"
