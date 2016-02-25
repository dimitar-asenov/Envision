#!/usr/bin/python3

import fileinput
import re
import os.path

envisionRootDir = "/fast/Envision"

currentFile = "" # This is the file that needs its includes adjusted
currentFileRegex = re.compile(r'^(/.*) should (?:add|remove) these lines:$')
currentDir = ""

includeRegex = re.compile(r'^#include "(.+)".*$') # We purposefully do not touch lines that indicate removals: ^- #include.*$
endOfSourceFile = re.compile(r'^---$')

# Computes an absolute path for an include
# Note that IWYU suggests includes with broken paths so we need to search for them
def findFullPath(include):
	parentDir = currentDir
	while not os.path.isfile(parentDir + '/' + include):
		parentDir = os.path.dirname(parentDir)
		assert parentDir
		
	return parentDir + '/' + include

# Makes the include path as relative as possible
def fixPath(include):
	assert currentFile
	fullPath = findFullPath(include)
	
	# We get the common prefix and take its directory to avoid things like "Envision/Co"
	commonPrefix = os.path.dirname(os.path.commonprefix([currentDir, fullPath]))
	if commonPrefix == envisionRootDir:
		return fullPath[len(commonPrefix)+1:] # +1 to include the /
	else:
		return os.path.relpath(fullPath, currentDir)

for line in fileinput.input():
	line = line.strip()
	
	if endOfSourceFile.match(line):
		currentFile = ""
		print(line)
		continue
	
	match = currentFileRegex.match(line)
	if match:
		currentFile = match.group(1)
		assert os.path.isfile(currentFile)
		currentDir = os.path.dirname(currentFile)
		print(line)
	else:
		match = includeRegex.match(line)
		if match:
			print('#include "'+fixPath(match.group(1)) + '"')
		else:
			print(line)