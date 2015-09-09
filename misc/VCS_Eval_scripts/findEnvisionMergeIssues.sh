#!/bin/bash

jitPath=~/Envision/JavaImportTool/bin/
jit="java -cp "${jitPath}:/opt/eclipse/plugins/*:../lib/*" javaImportTool.Main"

gumtreePath=~/gumtree/gumtree-all/target
gumtree="java -cp ${gumtreePath}/gumtree.jar com.github.gumtreediff.client.Run -c Clients.experimental true -c match.gt.minh 1 -c match.bu.sim 0.5 envdmp -m gumtree"

idpatcher=~/Envision/misc/VCS_Eval_scripts/idPatcher.py

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
					
					~/Envision/misc/VCS_Eval_scripts/import_and_merge.sh base.java master.java dev.java
					mkdir devMerged
					$jit TestMerge devMerged.java devMerged
					cp /tmp/EnvisionVC/TestMerge/TestMerge envMerged.env
					$gumtree envMerged.env devMerged/TestMerge/TestMerge.env
					$idpatcher devMerged/TestMerge/TestMerge.env
					
					diff devMerged/TestMerge/TestMerge.env envMerged.env > diff_dev_env
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