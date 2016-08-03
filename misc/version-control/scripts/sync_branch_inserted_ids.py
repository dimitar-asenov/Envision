#!/usr/bin/python3

# This script is meant to synchronize the IDs of elements that are inserted by both branches, but look like they should be identical.
# It takes two arguments: the names of the branchA and branchB files that need to be synced.
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

# Compare the files
transformations = difflib.SequenceMatcher(None, a=branchAList, b=branchBList, autojunk=False).get_opcodes()

# Get replacements of equal size
replacements = (x for x in transformations if x[0] == 'replace' and (x[2]-x[1] == x[4]-x[3]))

# Regex to parse Envision files
subRegex = re.compile("^(\t*)(\S+ \S+ \{)(\S+)(\} \{)(\S+)(\}.*)$")

# Returns a set of ids that are in an Envision file
def idsInLines( lines ):
	result = set()
	for line in lines:
		match = subRegex.match(line)
		assert match
		result.update({match.group(3)})
	return result

idsInB = idsInLines( branchBList )

# The result will be stored here
finalMapping = {}

# Returns an ID mapping to convert subListB to subListA, but only if that conversion is safe
def mappingIfStructureIsIdenticalAndNew( subListA, subListB ):
	
	if len(subListA) != len(subListB):
		return {}
	
	result = {}
	currentRootIndentSize = -1
	for lineA, lineB in zip(subListA, subListB):
		matchA = subRegex.match(lineA)
		matchB = subRegex.match(lineB)
		assert matchA
		assert matchB
		
		# Only allow identical lines.
		if matchA.group(1) != matchB.group(1) or matchA.group(2) != matchB.group(2) or  matchA.group(6) != matchB.group(6):
			return {}
			
		# Only allow identical IDs, or IDs unsused in B
		differentId = matchA.group(3) != matchB.group(3)
		if differentId and ( matchA.group(3) in idsInB ):
			return {}
		
		currentIndentSize = len(matchA.group(1))
		if currentRootIndentSize < 0 or currentIndentSize < currentRootIndentSize:
			currentRootIndentSize = currentIndentSize
		
		# If this line is at the level of the current root, only allow this to work if
		# the parent node is identical, or will be identical
		# Otherwise we might end up inserting the same element into two different parents
		differentParentId = matchA.group(5) != matchB.group(5)
		if currentIndentSize == currentRootIndentSize and differentParentId and ((matchB.group(5) not in finalMapping) or finalMapping[matchB.group(5)] != matchA.group(5)):
			return {}
		
		
		# If all checks passed, add this value to the dictionary
		# It will only be returned if the structures are identical
		if differentId:
			result[matchB.group(3)] = matchA.group(3)
	
	return result

# Get mappings of identical subLists
for x in replacements:
	finalMapping.update(mappingIfStructureIsIdenticalAndNew( branchAList[x[1]:x[2]],  branchBList[x[3]:x[4]]))

# Output the final mappings
for key, value in finalMapping.items():
	print(key + ' -> ' + value)
