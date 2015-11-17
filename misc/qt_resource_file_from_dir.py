#!/usr/bin/python2

import argparse
import os
import fnmatch

argParser = argparse.ArgumentParser('Generate a qt resource file from a directory')
argParser.add_argument('sourceDir')
argParser.add_argument('prefixForResource')
argParser.add_argument('destQrcFile')

args = argParser.parse_args()

qrcLines = []
qrcLines.append('<!DOCTYPE RCC><RCC version="1.0">')
qrcLines.append('	<qresource prefix="' + args.prefixForResource + '">')

fullSourceDirPath = os.path.realpath( args.sourceDir )

for root, dirnames, filenames in os.walk(args.sourceDir):
	for filename in filenames:
		fullFilePath = os.path.realpath( os.path.join(root, filename) )
		assert fullFilePath.startswith(fullSourceDirPath)
		qrcLines.append('		<file alias="' + fullFilePath[len(fullSourceDirPath):] + '">' + fullFilePath + '</file>')

qrcLines.append('	</qresource>')
qrcLines.append('</RCC>')

# Read the existing file, if any
with open(args.destQrcFile, 'a+') as f:
	oldLines = f.readlines()

# Check if file needs updating
newLines = ["%s\n" % line  for line in qrcLines]
if (oldLines == newLines):
	print "No new resources in directory " + fullSourceDirPath
else:
	with open(args.destQrcFile, 'w') as f:
		f.writelines(newLines)
	print "Generated resources file from directory " + fullSourceDirPath