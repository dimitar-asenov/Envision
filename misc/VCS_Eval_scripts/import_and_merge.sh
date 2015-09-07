#!/bin/bash

jitPath=~/Envision/JavaImportTool/bin/
jit="java -cp "${jitPath}:/opt/eclipse/plugins/*:../lib/*" javaImportTool.Main"

gumtreePath=~/gumtree/gumtree-all/target
gumtree="java -cp ${gumtreePath}/gumtree.jar com.github.gumtreediff.client.Run -c Clients.experimental true -c match.gt.minh 1 -c match.bu.sim 0.5 envdmp -m gumtree"

idpatcher=~/Envision/misc/VCS_Eval_scripts/idPatcher.py
repoScript=~/Envision/FilePersistence/test/persisted/version-control/create-test-git-repo.py
envision=~/Envision/DebugBuild/Envision

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

$jit TestMerge base base
$jit TestMerge master master
$jit TestMerge dev dev

# Generate patch files
$gumtree base/TestMerge/TestMerge.env master/TestMerge/TestMerge.env
$gumtree base/TestMerge/TestMerge.env dev/TestMerge/TestMerge.env

# Apply patches
$idpatcher master/TestMerge/TestMerge.env
$idpatcher dev/TestMerge/TestMerge.env

# Move to Envision test directory
mkdir $envRepoSrc
cp base/TestMerge/TestMerge.env "${envRepoSrc}/master_a_(base)_TestMerge"
cp master/TestMerge/TestMerge.env "${envRepoSrc}/master_b_(master)_TestMerge"
cp dev/TestMerge/TestMerge.env "${envRepoSrc}/dev_a_master_a_(dev)_TestMerge"

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
cd ~/Envision/DebugBuild
#
./Envision -graphicssystem raster --test filepersistence:RunMerge
)

# Export to Java code

# Cleanup of unnecessary files

rm -rf $envRepoSrc
rm -rf $gitRepoSrc
