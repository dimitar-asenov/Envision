#!/bin/bash

# This script has one mandatory argument: a directory previously populated by the dump_repo.sh script and one optional flag '-quick-match'
#
# The script merges the java files using Envision's merge algorithm and compares the result to the definitive developer-merged verison.
# If the versions are different, an entry specifying the revision and file is written to merges/issues_env.
# The JavaImportTool, Gumtree and patch_ids.py are used to import and match the developer-merged version to the envision-merged version.
#
# If the '-quick-match' flag is used, then instead of GumTree, a simple text-comparison is used to match IDs from different versions.

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ENVISION_ROOT="$( cd "$SCRIPT_DIR/../../.." && pwd )"

JavaImportTool="$SCRIPT_DIR/../JavaImportToolBin/JavaImportTool/bin/JavaImportTool"

gumtree="$SCRIPT_DIR/../gumtree_bin/gumtree-2.1.0-SNAPSHOT/bin/gumtree -c Clients.experimental true -c match.gt.minh 1 -c match.bu.sim 0.5 envdmp -g envision -m gumtree"
quick_match="$SCRIPT_DIR/quick-match.py"
QUICK_MATCH_ARG=$2

idpatcher=$SCRIPT_DIR/patch_ids.py

scriptReadyFile="/tmp/EnvisionVC/scriptReady"
rm -rf $scriptReadyFile
rm -rf "/tmp/EnvisionVC"

rm -rf "${1}/merges/issues_env"

# Launch Envision in merge test mode
# syntax for tests: pluginName:testName1[>arg1,arg2,...][:testName2[>arg1,arg2,...]:...]
(
cd $ENVISION_ROOT/DebugBuild
#
./Envision -graphicssystem raster --test filepersistence:RunMerge &
)

function waitFor () {
	while kill -0 $1 2> /dev/null; do sleep 0.05; done;
}

merges=(${1}/merges/*)
counter=1
for m in "${merges[@]}"; do
	for fdir in ${m}/*; do
		echo "#################### Processing ($counter of ${#merges[@]}) $fdir ####################" 
		if [ -d "${fdir}" ]; then
			(
				cd $fdir
				if [ -f base.java ] && [ -f dev.java ] && [ -f master.java ] && [ -f devMerged.java ]; then
					
					$SCRIPT_DIR/import_and_merge.sh base.java master.java dev.java $QUICK_MATCH_ARG &
					importBranchesAndMergePID=$!
					
					rm -rf devMerged
					mkdir devMerged
					$JavaImportTool TestMerge devMerged.java devMerged -force-single-pu -no-size-estimation &
					importDevPID=$!
					
					waitFor $importBranchesAndMergePID
					waitFor $importDevPID
					
					cp /tmp/EnvisionVC/TestMerge/TestMerge envMerged
					
					if [ "$QUICK_MATCH_ARG" == "-quick-match" ]; then
						$quick_match envMerged devMerged/TestMerge/TestMerge > devMerged/TestMerge/TestMerge.idpatch
					else
						$gumtree envMerged devMerged/TestMerge/TestMerge
					fi
					$idpatcher devMerged/TestMerge/TestMerge
					
					AUTO_MANUAL_DIFF=$(diff devMerged/TestMerge/TestMerge envMerged)
					if [[ $AUTO_MANUAL_DIFF  ]]; then
						echo "${m##*/}/${fdir##*/}" >> ../../issues_env
					fi
				fi
			)
		fi
	done
	((counter++))
done

sort "${1}/merges/issues_env" > "${1}/merges/issues_env2"
mv "${1}/merges/issues_env2" "${1}/merges/issues_env"
