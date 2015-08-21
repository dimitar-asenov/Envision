#!/bin/bash

jitPath=~/Envision/JavaImportTool/bin/
jit="java -cp "${jitPath}:/opt/eclipse/plugins/*:../lib/*" javaImportTool.Main"

gumtreePath=~/gumtree/gumtree-all/target/
gumtree="java -cp ${gumtreePath}gumtree.jar com.github.gumtreediff.client.Run -c Clients.experimental true envdmp"

idpatcher=~/Envision/misc/idPatcher.py

envision=~/Envision/DebugBuild/Envision
testdir=~/Envision/FilePersistence/test/persisted/version-control/TestMerge_WorkflowTest/

workDir=~/VC_Eval/
projectName=SyntheticExample
fileName=ConversionException.java

# Set up traditional Git repo with java code
cd $workDir
find ! -name "*.tar" -exec rm -rf {} +
tar -xf "${projectName}.tar"

# Checkout relevant versions and copy them to temporary files
mkdir "${projectName}_master"
cp "${projectName}/${fileName}" "${projectName}_master/"
(
	cd $projectName
	git checkout dev
)
mkdir "${projectName}_dev"
cp "${projectName}/${fileName}" "${projectName}_dev/"
(
	cd $projectName
	git checkout "HEAD~1"
)
mkdir "${projectName}_base"
cp "${projectName}/${fileName}" "${projectName}_base/"

# Import the files into Envision
mkdir master
mkdir dev
mkdir base
$jit TestMerge "${projectName}_master/" master
$jit TestMerge "${projectName}_dev/" dev
$jit TestMerge "${projectName}_base/" base

# Generate patch files
$gumtree base/TestMerge/TestMerge master/TestMerge/TestMerge
$gumtree base/TestMerge/TestMerge dev/TestMerge/TestMerge

# Apply patches
$idpatcher master/TestMerge/TestMerge
$idpatcher dev/TestMerge/TestMerge

# Move to Envision test directory
cp base/TestMerge/TestMerge "${testdir}master_a_(base)_TestMerge"
cp master/TestMerge/TestMerge "${testdir}master_b_(master)_TestMerge"
cp dev/TestMerge/TestMerge "${testdir}dev_a_master_a_(dev)_TestMerge"

# Run test
#$envision -graphicssystem raster --test FilePersistence:WorkflowTest

# Export to Java code
