#!/usr/bin/python3
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
    print("Patching file " + fileName)
    with fileinput.input(files=(fileName), inplace=True, backup=".old") as fileToPatch:
        with open(fileName + patchFileSuffix) as patchFile:
            for patch in patchFile:
                parts = patch.rstrip("\n").split(", ")
                lineToPatch = parts[0]
                nodeId = parts[1]
                parentId = parts[2]
                while fileToPatch.filelineno() < int(lineToPatch) - 1:
                    print(fileToPatch.readline().rstrip("\n"))
                # Now the next line we read is the one to patch.
                oldLine = fileToPatch.readline()
                m = subRegex.match(oldLine)
                newLine = m.group(1) + nodeId + m.group(2) + parentId + m.group(3)
                print(newLine)
            while True: # Why the hell does python not have a .hasNext()???
                line = fileToPatch.readline()
                if not line: break;
                print(line) # Write the rest of the file

def main(argv):
    if len(argv) == 0:
        print("Please specify a file to patch.")
    else:
        for fileName in argv:
            patch(fileName)

if __name__ == "__main__":
    main(sys.argv[1:])
