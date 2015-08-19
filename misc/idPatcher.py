#!/usr/bin/python
'''
Created on Aug 19, 2015

@author: Balz Guenat
'''

import re
import sys
import fileinput

patchFileSuffix = ".idpatch"
subRegex = re.compile("^(\t*\S+ \S+ \{)\S+(\} \{)\S+(\}.*)$")

def patch(fileName):
    with fileinput.input(files=(fileName), inplace=True, backup=".old") as fileToPatch:
        with open(fileName + patchFileSuffix) as patchFile:
            curLine = 0
            for patch in patchFile:
                parts = patch.rstrip("\n").split(", ")
                lineToPatch = parts[0]
                nodeId = parts[1]
                parentId = parts[2]
                while curLine < int(lineToPatch):
                    oldLine = fileToPatch.readline()
                    curLine += 1
                m = subRegex.match(oldLine)
                newLine = m.group(1) + nodeId + m.group(2) + parentId + m.group(3)
                print(newLine)

def main(argv):
    if len(sys.argv) == 0:
        print("Please specify a file to patch.")
    else:
        for fileName in argv:
            patch(fileName)

if __name__ == "__main__":
    main(sys.argv[1:])
