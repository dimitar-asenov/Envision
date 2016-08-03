#!/usr/bin/python3

# This script is meant to synchronize the IDs of elements that are inserted by both branches, but look like they should be identical.
# It takes the following arguments in the following order:
#  - The names of the branchA and branchB files that need to be synced.
#    The script expects an ID-patch-file of the same name extended with ".idpatch" to exist in the same directory.  This is needed to know what elements from A are also in base.
#
# The script will output a patch file that can be used by patch_ids.py in order to patch branchB

import re
import difflib
import argparse

argParser = argparse.ArgumentParser('Create an id patch file based on similarities between two branches')
argParser.add_argument('branchAFilename')
argParser.add_argument('branchBFilename')
args = argParser.parse_args()

patchFileSuffix = ".idpatch"

# Read branch files
with open(args.branchAFilename) as f:
	branchAList = f.readlines()
with open(args.branchBFilename) as f:
	branchBList = f.readlines()
	
# Read patch files and extract only ids in base
with open(args.branchAFilename + patchFileSuffix) as f:
	idsInBase = {v.rstrip("\n").split(" -> ")[1] for v in  f.readlines()}
with open(args.branchBFilename + patchFileSuffix) as f:
	idsInBase.update( {v.rstrip("\n").split(" -> ")[1] for v in  f.readlines()} )
	
# Compare the files
transformations = difflib.SequenceMatcher(None, a=branchAList, b=branchBList, autojunk=False).get_opcodes()

# Get replacements of equal size
replacements = (x for x in transformations if x[0] == 'replace' and (x[2]-x[1] == x[4]-x[3]))

#========================= Helper functions =========================#
subRegex = re.compile("^(\t*\S+ \S+ \{)(\S+)(\} \{)(\S+)(\}.*)$")

def mappingIfStructureIsIdenticalAndNew( subListA, subListB ):
	
	if len(subListA) != len(subListB):
		return {}
	
	result = {}
	for lineA, lineB in zip(subListA, subListB):
		matchA = subRegex.match(lineA)
		matchB = subRegex.match(lineB)
		assert matchA
		assert matchB
		
		# Only allow identical lines.
		if matchA.group(1) != matchB.group(1) or matchA.group(5) != matchB.group(5):

			return {}
		
		# Only allow new or identical IDs
		differentId = matchA.group(2) != matchB.group(2)
		if differentId and ( matchA.group(2) in idsInBase or matchB.group(2) in idsInBase ):
			return {}
		
		# If all checks passed, add this value to the dictionary
		# It will only be returned if the structures are identical
		if differentId:
			result[matchB.group(2)] = matchA.group(2)
	
	return result
#========================= End helper functions =========================#

# Get mappings of identical subLists
finalMapping = {}
for x in replacements:
	finalMapping.update(mappingIfStructureIsIdenticalAndNew( branchAList[x[1]:x[2]],  branchBList[x[3]:x[4]]))

# Output the final mappings
for key, value in finalMapping.items():
	print(key + ' -> ' + value)
