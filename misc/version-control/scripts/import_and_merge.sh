#!/bin/bash

# This script takes 3 java files as input: base master dev
# It creates a repo in /tmp/EnvisionVC/TestMerge and merges the files using Envision.
# It does the same using the Git merge in /tmp/EnvisionVC/Git.
# If run manually, this script is intended for the files in Envision/FilePersistence/test/persisted/version-control/manual/
# The script is also used in the diff_envision_dev.sh script.

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ENVISION_ROOT="$( cd "$SCRIPT_DIR/../../.." && pwd )"

JavaImportTool="$SCRIPT_DIR/../JavaImportToolBin/JavaImportTool/bin/JavaImportTool"

gumtree="$SCRIPT_DIR/../gumtree_bin/gumtree-2.1.0-SNAPSHOT/bin/gumtree -c Clients.experimental true -c match.gt.minh 1 -c match.bu.sim 0.5 envdmp -g envision -m gumtree"

idpatcher=$SCRIPT_DIR/patch_ids.py
repoScript=$ENVISION_ROOT/FilePersistence/test/persisted/version-control/create-test-git-repo.py

testdir="/tmp/EnvisionVC"
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

cd $testdir

$JavaImportTool TestMerge base base -force-single-pu
$JavaImportTool TestMerge master master -force-single-pu
$JavaImportTool TestMerge dev dev -force-single-pu

# Generate patch files
$gumtree base/TestMerge/TestMerge master/TestMerge/TestMerge
$gumtree base/TestMerge/TestMerge dev/TestMerge/TestMerge

# Apply patches
$idpatcher master/TestMerge/TestMerge
$idpatcher dev/TestMerge/TestMerge

# Move to Envision test directory
mkdir $envRepoSrc
cp base/TestMerge/TestMerge "${envRepoSrc}/master_a_(base)_TestMerge"
cp master/TestMerge/TestMerge "${envRepoSrc}/master_b_(master)_TestMerge"
cp dev/TestMerge/TestMerge "${envRepoSrc}/dev_a_master_a_(dev)_TestMerge"

$repoScript $envRepoSrc $envRepo

# Move Java filesto Git test directory

$repoScript $gitRepoSrc $gitRepo
(
cd $gitRepo
git merge dev -m "merge dev"
)

# Run test
# syntax for tests: pluginName:testName1[>arg1,arg2,...][:testName2[>arg1,arg2,...]:...]
(
cd $ENVISION_ROOT/DebugBuild
#
./Envision -graphicssystem raster --test filepersistence:RunMerge
)

# Export to Java code

# Cleanup of unnecessary files

rm -rf $envRepoSrc
rm -rf $gitRepoSrc
