#!/bin/bash

# This script takes as the single argument a directory previously populated by the dump_repo.sh script.
# It merges the java files using Envision's merge algorithm and compares the result to the definitive developer-merged verison.
# If the versions are different, an entry specifying the revision and file is written to merges/issues_env.
# The JavaImportTool, Gumtree and patch_ids.py are used to import and match the developer-merged version to the envision-merged version.

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

JavaImportTool="$SCRIPT_DIR/../JavaImportToolBin/JavaImportTool/bin/JavaImportTool"

gumtree="$SCRIPT_DIR/../gumtree_bin/gumtree-2.1.0-SNAPSHOT/bin/gumtree -c Clients.experimental true -c match.gt.minh 1 -c match.bu.sim 0.5 envdmp -g envision -m gumtree"

idpatcher=$SCRIPT_DIR/patch_ids.py

rm "${1}/merges/issues_env"

merges="${1}/merges/*"
for m in $merges; do
	#echo Revision $m
	for fdir in ${m}/*; do
		#echo File $fdir
		if [ -d "${fdir}" ]; then
			(
				cd $fdir
				if [ -f base.java ] && [ -f dev.java ] && [ -f master.java ]; then
					
					$SCRIPT_DIR/import_and_merge.sh base.java master.java dev.java
					mkdir devMerged
					$JavaImportTool TestMerge devMerged.java devMerged -force-single-pu
					cp /tmp/EnvisionVC/TestMerge/TestMerge envMerged
					$gumtree envMerged devMerged/TestMerge/TestMerge
					$idpatcher devMerged/TestMerge/TestMerge
					
					diff devMerged/TestMerge/TestMerge envMerged > diff_dev_env
					if [ -s diff_dev_env ]; then
						echo "${m##*/}/${fdir##*/}" >> ../../issues_env
					fi
					rm diff_dev_env
				fi
			)
		fi
	done
done

sort "${1}/merges/issues_env" > "${1}/merges/issues_env2"
mv "${1}/merges/issues_env2" "${1}/merges/issues_env"
