#!/usr/bin/python3

# This script is a quick and dirty alternative to gumtree. It synchronizes the IDs of elements in a branch tree to the base
# It takes two arguments: the names of the base and the branch files that need to be synced.
#
# The script will output a patch file that can be used by patch_ids.py in order to patch the branch file
#
# Unlike gumtree, this script only very superficially matches the structure of the files, most-notably ignoring almost all moves.
# This is somewhat OK, if we don't really care about preserving node identity, which we anyway don't have when importing from traditional source code.

##
from __future__ import print_function
import sys

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)
##

import re
import difflib
import argparse



argParser = argparse.ArgumentParser('Create an id patch file matching a branch file to a base')
argParser.add_argument('baseFilename')
argParser.add_argument('branchFilename')
args = argParser.parse_args()

def isInt( x ):
	try: 
		int(x)
		return True
	except ValueError:
		return False

# Regex to parse Envision files
subRegex = re.compile("^(\t*)(\S+)( \S+ \{)(\S+)(\} \{)(\S+)(\}.*)$")

def stripListLabelsAndIds(line):
	m = subRegex.match(line)
	if not m:
		eprint("BAD Line" + line)
	assert m
	
	if isInt( m.group(2) ):
		return m.group(1) +              m.group(3) + m.group(5) + m.group(7)
	else:
		return m.group(1) + m.group(2) + m.group(3) + m.group(5) + m.group(7)

# Read input files
with open(args.baseFilename) as f:
	baseList = f.readlines()
with open(args.branchFilename) as f:
	branchList = f.readlines()
	
# Get the matching blocks
matchingBlocks = difflib.SequenceMatcher(None, a=list(map(stripListLabelsAndIds, baseList)), b=list(map(stripListLabelsAndIds, branchList)), autojunk=False).get_matching_blocks()

# The result will be stored here
finalMapping = {}

# Iterate over all matching blocks and extract the mapping
for block in matchingBlocks:
	if block[2] > 0:
		for baseLine, branchLine in zip(baseList[block[0]:block[0]+block[2]], branchList[block[1]:block[1]+block[2]]):
			matchBase = subRegex.match(baseLine)
			matchBranch = subRegex.match(branchLine)
			assert matchBase
			assert matchBranch
			
			finalMapping[matchBranch.group(4)] = matchBase.group(4)

# Output the final mappings
for key, value in finalMapping.items():
	print(key + ' -> ' + value)
