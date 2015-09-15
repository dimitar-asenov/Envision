#!/usr/bin/python

import os
import glob
import re
from subprocess import call
from shutil import copyfile, rmtree
import argparse

argParser = argparse.ArgumentParser('Generate a git repository based on files')
argParser.add_argument('sourceDir')
argParser.add_argument('destDir')
args = argParser.parse_args()

fileNameRegEx = re.compile('^(?:' + args.sourceDir + '/)([a-zA-Z]+)_([a-zA-Z])_(?:([a-zA-Z]+)_([a-zA-Z])_)?(?:\(([^\)]+)\)_)?(.+)$')

#===============================================================================
# Function definitions

def commit(branch, tag, message):
	call(['git','-C',args.destDir,'add','-A'])
	call(['git','-C',args.destDir,'commit','-m', (message if message else concat(branch, tag))])
	call(['git','-C',args.destDir,'tag',concat(branch, tag)])

def compareFileNames(a, b):
	matchA = fileNameRegEx.match(a)
	matchB = fileNameRegEx.match(b)
	assert matchA
	assert matchB
	
	if matchA.group(1) == 'master' and matchB.group(1) != 'master':
		return -1
	if matchA.group(1) != 'master' and matchB.group(1) == 'master':
		return 1
	else:
		return cmp(a,b)

def concat(branch, tag):
	return branch + '-' + tag
	
#===============================================================================

files = glob.glob(args.sourceDir + "/*")
files.sort(compareFileNames)

#Initialize git repo
if os.path.exists(args.destDir + "/.git"):
	rmtree(args.destDir)
os.mkdir(args.destDir)
call(['git','init', args.destDir])

lastBranch = ''
lastTag = ''
lastMessage = ''

for fileName in files:
	
	match = fileNameRegEx.match(fileName)
	assert match, "bad match: %s" %fileName

	branch = match.group(1)
	tag = match.group(2)
	parentBranch = match.group(3)
	parentTag = match.group(4)
	commitMessage = match.group(5)
	fileToCommit = match.group(6)
	
	# Commit current state when starting the next state
	if (tag != lastTag and lastTag) or (branch != lastBranch and lastBranch):
		commit(lastBranch, lastTag, lastMessage)
		
		# switch to correct parent
		if parentBranch:
			call(['git','-C',args.destDir,'checkout', concat(parentBranch,parentTag) ])
			call(['git','-C',args.destDir,'checkout', '-b', branch ])
			
		# remove all exisitng files
		for fileToRemove in glob.glob(args.destDir + "/*"):
			if not fileToRemove.endswith('.git'):
				os.remove(fileToRemove)
	
	# copy files
	copyfile(fileName, args.destDir + '/' + fileToCommit)
	
	lastBranch = branch
	lastTag = tag
	lastMessage = commitMessage

# Make the last commit
commit(lastBranch, lastTag, lastMessage)

# Always leave in the master branch, regadless of how many branches are there
call(['git','-C',args.destDir,'checkout', 'master' ])