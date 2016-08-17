#!/bin/bash

# This script takes 3 java files as input: base master dev + an optional flag '-quick-match'
#
# It creates a repo in /tmp/EnvisionVC/TestMerge and merges the files using Envision.
# It does the same using the Git merge in /tmp/EnvisionVC/Git.
# If run manually, this script is intended for the files in Envision/FilePersistence/test/persisted/version-control/manual/
# The script is also used in the diff_envision_dev.sh script.
#
# If the '-quick-match' flag is used, then instead of GumTree, a simple text-comparison is used to match IDs from different versions.

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ENVISION_ROOT="$( cd "$SCRIPT_DIR/../../.." && pwd )"

JavaImportTool="$SCRIPT_DIR/../JavaImportToolBin/JavaImportTool/bin/JavaImportTool"

gumtree="$SCRIPT_DIR/../gumtree_bin/gumtree-2.1.0-SNAPSHOT/bin/gumtree -c Clients.experimental true -c match.gt.minh 1 -c match.bu.sim 0.5 envdmp -g envision -m gumtree"
quick_match="$SCRIPT_DIR/quick-match.py"
QUICK_MATCH_ARG=$4

idpatcher=$SCRIPT_DIR/patch_ids.py
idsync=$SCRIPT_DIR/sync_branch_inserted_ids.py
repoScript=$ENVISION_ROOT/FilePersistence/test/persisted/version-control/create-test-git-repo.py

testdir="/tmp/EnvisionVC"
scriptReadyFile="/tmp/EnvisionVC/scriptReady"
envisionReadyFile="/tmp/EnvisionVC/envisionReadyFile"
base="${testdir}/base"
master="${testdir}/master"
dev="${testdir}/dev"

envRepoSrc="${testdir}/envSrc"
gitRepoSrc="${testdir}/gitSrc"
envRepo="${testdir}/TestMerge"
gitRepo="${testdir}/Git"

# $1 is base
# $2 is master
# $3 is dev
# $4 is the optional -quick-match

# Import the files into Envision
rm -rf $testdir
mkdir $testdir
mkdir $base
mkdir $master
mkdir $dev
cp $1 $base
cp $2 $master
cp $3 $dev

mkdir $gitRepoSrc
cp $1 "${gitRepoSrc}/master_a_(base)_TestMerge"
cp $2 "${gitRepoSrc}/master_b_(master)_TestMerge"
cp $3 "${gitRepoSrc}/dev_a_master_a_(dev)_TestMerge"
mkdir $envRepoSrc

MERGE_AND_FILE_DIR="$( pwd )"

cd $testdir

function waitFor () {
	while kill -0 $1 2> /dev/null; do sleep 0.05; done;
}

echo "-------------------- Importing into Envision --------------------"
# Import Java to Envision
$JavaImportTool TestMerge base base -force-single-pu -no-size-estimation &
baseImportPID=$!

# Move to Envision test directory
function copyBase ()
{
	waitFor $baseImportPID
	cp base/TestMerge/TestMerge "${envRepoSrc}/master_a_(base)_TestMerge"
}
copyBase &
copyBasePID=$!

$JavaImportTool TestMerge master master -force-single-pu -no-size-estimation &
masterImportPID=$!

function matchMaster ()
{
	waitFor $baseImportPID
	waitFor $masterImportPID
	
	echo "-------------------- Matching Master to Base --------------------"
	if [ "$QUICK_MATCH_ARG" == "-quick-match" ]; then
		$quick_match base/TestMerge/TestMerge master/TestMerge/TestMerge > master/TestMerge/TestMerge.idpatch
	else
		$gumtree base/TestMerge/TestMerge master/TestMerge/TestMerge
	fi
	
	echo "-------------------- Patching Master IDs --------------------"
	$idpatcher master/TestMerge/TestMerge
}
matchMaster &
matchMasterPID=$!

# In some cases both the master and dev branch of a file are identical, but they are both different to base
# In such cases create matching ids and skip unnecessary computation
DEV_MASTER_DIFF=$(diff dev/dev.java master/master.java)

function processDevBranch ()
{
	if [[ $DEV_MASTER_DIFF  ]]; then
		$JavaImportTool TestMerge dev dev -force-single-pu -no-size-estimation
		
		waitFor $baseImportPID
		
		echo "-------------------- Matching Dev to Base --------------------"
		if [ "$QUICK_MATCH_ARG" == "-quick-match" ]; then
			$quick_match base/TestMerge/TestMerge dev/TestMerge/TestMerge > dev/TestMerge/TestMerge.idpatch
		else
			$gumtree base/TestMerge/TestMerge dev/TestMerge/TestMerge
		fi
	
		echo "-------------------- Patching Dev IDs --------------------"
		$idpatcher dev/TestMerge/TestMerge
		
		waitFor $matchMasterPID
		echo "-------------------- Syncing branch IDs --------------------"
		# Additionally match some newly introduced IDs
		# Match dev to master
		$idsync base/TestMerge/TestMerge master/TestMerge/TestMerge dev/TestMerge/TestMerge > dev/TestMerge/TestMerge.idpatch.sync
		mv -f dev/TestMerge/TestMerge.idpatch.sync dev/TestMerge/TestMerge.idpatch
		$idpatcher dev/TestMerge/TestMerge
		# Match master to dev
		$idsync base/TestMerge/TestMerge dev/TestMerge/TestMerge master/TestMerge/TestMerge > master/TestMerge/TestMerge.idpatch.sync
		mv -f master/TestMerge/TestMerge.idpatch.sync master/TestMerge/TestMerge.idpatch
		$idpatcher master/TestMerge/TestMerge
		
		# Copy
		cp master/TestMerge/TestMerge "${envRepoSrc}/master_b_(master)_TestMerge"
		cp dev/TestMerge/TestMerge "${envRepoSrc}/dev_a_master_a_(dev)_TestMerge"
	else
		waitFor $matchMasterPID
		cp master/TestMerge/TestMerge "${envRepoSrc}/dev_a_master_a_(dev)_TestMerge"
		cp master/TestMerge/TestMerge "${envRepoSrc}/master_b_(master)_TestMerge"
	fi
}
processDevBranch &
processDevBranchPID=$!

waitFor $copyBasePID
waitFor $processDevBranchPID

echo "-------------------- Creating repositories --------------------"
$repoScript $envRepoSrc $envRepo &> /dev/null

# Move Java files to Git test directory

$repoScript $gitRepoSrc $gitRepo &> /dev/null
(
cd $gitRepo
git merge dev -m "merge dev"
)

echo "-------------------- Merging in Envision --------------------"
rm -rf $envisionReadyFile
touch $scriptReadyFile
while [ ! -f $envisionReadyFile ] ;
do
	sleep 0.1
done

echo "-------------------- Copying merged files and info --------------------"

# Copy envision repo sources and summary
cp -rf $envRepoSrc $MERGE_AND_FILE_DIR/.

declare -a filesToCopy=("direct_conflicts" "remaining_changes" "soft_conflicts")
for f in "${filesToCopy[@]}"
do
	if [ -f $f ] ; then
		cp -f $f $MERGE_AND_FILE_DIR/.
		rm $f
	else
		rm -f $MERGE_AND_FILE_DIR/$f
	fi
done

# Cleanup of unnecessary files
rm -rf $envRepoSrc
rm -rf $gitRepoSrc
