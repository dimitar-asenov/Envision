#!/usr/bin/python2

import os
import glob
import re

ENVISION_ROOT = os.path.dirname( os.path.dirname(os.path.realpath(__file__)))
DEFAULT_COLOR = {'red':0, 'green':0, 'blue':0, 'alpha':255}

files = glob.glob(ENVISION_ROOT + "/*/styles/*/*/*")

colorComponentRegex = re.compile('^\s*<(red|green|blue|alpha)>(\d+)</(red|green|blue|alpha)>\s*$')
endColorTag = re.compile('^\s*(</\w+>)\s*$')

for fileName in files:
	with open(fileName, 'r+') as file:
		oldLines = [line.rstrip('\n') for line in file]
		
		newLines = []
		foundAtLeastOneColor = False
		
		inColor = False
		color = DEFAULT_COLOR.copy()
		for line in oldLines:
			match = colorComponentRegex.match(line)
			if match:
				inColor = True
				foundAtLeastOneColor = True
				color[match.group(1)] = int(match.group(2))
			elif inColor:
				match = endColorTag.match(line)
				assert match
				
				hexColor = "%.2x%.2x%.2x" % (color['red'], color['green'], color['blue'])
				if color['alpha'] != 255:
					hexColor = "%.2x" % (color['alpha']) + hexColor
				newLines[-1] += '#' + hexColor + match.group(1)
				
				# Reset vars
				inColor = False
				color = DEFAULT_COLOR.copy()
			else:
				newLines.append(line)
					
		if foundAtLeastOneColor:
			print 'Converting file ' + fileName
			file.seek(0)
			file.writelines(["%s\n" % line  for line in newLines])
			file.truncate()