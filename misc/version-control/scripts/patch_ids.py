#!/usr/bin/python3

# This script takes as its single argument an Envision file. The script expects an ID-patch-file of the same name extended with ".idpatch" to exist in the same directory. Optionally there can be a file extended ".idpatch.manual" that contains manual patches overriding the automatic ones.
# The original input file will be stored with the ".old" extension.

import re
import sys
import fileinput
from os.path import isfile

patchFileSuffix = ".idpatch"
manualFileSuffix = ".idpatch.manual"

subRegex = re.compile("^(\t*\S+ \S+ \{)(\S+)(\} \{)(\S+)(\}.*)$")

def patch(fileName):
    print("Patching file " + fileName)
    patches = {} # Maps old IDs to new IDs
    for patch in open(fileName + patchFileSuffix):
        parts = patch.rstrip("\n").split(" -> ")
        patches[parts[0]] = parts[1];
    # manual patches override automatic ones
    if isfile(fileName + manualFileSuffix):
        for patch in open(fileName + manualFileSuffix):
            parts = patch.rstrip("\n").split(" -> ")
            patches[parts[0]] = parts[1];
    
    for oldLine in fileinput.input(files=(fileName), inplace=True, backup=".old"):
        m = subRegex.match(oldLine)
        start = m.group(1)
        oldId = m.group(2)
        middle = m.group(3)
        oldParentId = m.group(4)
        end = m.group(5)
        if oldId in patches:
            newId = patches[oldId]
        else:
            newId = oldId
        if oldParentId in patches:
            newParentId = patches[oldParentId]
        else:
            newParentId = oldParentId
        
        newLine = start + newId + middle + newParentId + end
        print(newLine)

def main(argv):
    if len(argv) == 0:
        print("Please specify a file to patch.")
    else:
        for fileName in argv:
            patch(fileName)

if __name__ == "__main__":
    main(sys.argv[1:])
