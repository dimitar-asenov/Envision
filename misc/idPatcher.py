#!/usr/bin/python3
'''
Created on Aug 19, 2015

@author: Balz Guenat
'''

import re
import sys
import fileinput

patchFileSuffix = ".idpatch"
subRegex = re.compile("^(\t*\S+ \S+ \{)(\S+)(\} \{)(\S+)(\}.*)$")

def patch(fileName):
    print("Patching file " + fileName)
    patches = {} # Maps old IDs to new IDs
    for patch in open(fileName + patchFileSuffix):
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
