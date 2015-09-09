#!/bin/bash

rm "${1}/merges/issues_git"

merges="${1}/merges/*"
for m in $merges; do
	#echo Revision $m
	for fdir in ${m}/*; do
		#echo File $fdir
		if [ -d "${fdir}" ]; then
			(
				cd $fdir
				if [ -f base.java ] && [ -f dev.java ] && [ -f master.java ]; then
					cp master.java gitMerged.java
					git merge-file -L master.java --quiet gitMerged.java base.java dev.java
					diff devMerged.java gitMerged.java > diff_dev_git
					if [ -s diff_dev_git ]; then
						echo "${m##*/}/${fdir##*/}" >> ../../issues_git
					fi
					echo "${m##*/}/${fdir##*/}" >> ../../all
					rm diff_dev_git
				fi
			)
		fi
	done
done

sort "${1}/merges/all" > "${1}/merges/all2"
mv "${1}/merges/all2" "${1}/merges/all"
sort "${1}/merges/issues_git" > "${1}/merges/issues_git2"
mv "${1}/merges/issues_git2" "${1}/merges/issues_git"