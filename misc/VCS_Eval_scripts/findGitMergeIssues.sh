#!/bin/bash

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
						echo "${m##*/}_${fdir##*/}" >> ../../issues
					fi
					rm diff_dev_git
				fi
			)
		fi
	done
done