#!/usr/bin/python

import argparse
import os
import fnmatch

argParser = argparse.ArgumentParser('Generate a qt resource file from a directory')
argParser.add_argument('sourceDir')
argParser.add_argument('prefixForResource')
argParser.add_argument('destQrcFile', type=argparse.FileType('w'))

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

args.destQrcFile.writelines(["%s\n" % line  for line in qrcLines])

#colorComponentRegex = re.compile('^\s*<(red|green|blue|alpha)>(\d+)</(red|green|blue|alpha)>\s*$')
#endColorTag = re.compile('^\s*(</\w+>)\s*$')

#for fileName in files:
	#with open(fileName, 'r+') as file:
		#oldLines = [line.rstrip('\n') for line in file]
		
		#newLines = []
		#foundAtLeastOneColor = False
		
		#inColor = False
		#color = DEFAULT_COLOR.copy()
		#for line in oldLines:
			#match = colorComponentRegex.match(line)
			#if match:
				#inColor = True
				#foundAtLeastOneColor = True
				#color[match.group(1)] = int(match.group(2))
			#elif inColor:
				#match = endColorTag.match(line)
				#assert match
				
				#hexColor = "%.2x%.2x%.2x" % (color['red'], color['green'], color['blue'])
				#if color['alpha'] != 255:
					#hexColor = "%.2x" % (color['alpha']) + hexColor
				#newLines[-1] += '#' + hexColor + match.group(1)
				
				## Reset vars
				#inColor = False
				#color = DEFAULT_COLOR.copy()
			#else:
				#newLines.append(line)
					
		#if foundAtLeastOneColor:
			#print 'Converting file ' + fileName
			#file.seek(0)
			#file.writelines(["%s\n" % line  for line in newLines])
			#file.truncate()