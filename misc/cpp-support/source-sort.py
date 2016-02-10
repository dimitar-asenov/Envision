#!/usr/bin/python2

import os
import argparse
import re
import sys

# Input filtering
initialComment = True
def isInitialComment(line):
	global initialComment
	initialComment = initialComment and (line.startswith("/***") or line.startswith(" **") or line.startswith("**"))
	return initialComment;

forwardDeclarationRegex = re.compile('^class \w+;$')
def isForwardDeclaration(line):
	global forwardDeclarationRegex
	return forwardDeclarationRegex.match(line);
	
def isInclude(line):
	return line.startswith("#include ");

includesToPreserve = []

def emptyLine(line):
	emptyLineRegex = re.compile('^\s*$')
	return emptyLineRegex.match(line)

def isExcluded(line):
	global correspondingSource
	return isInitialComment(line) or emptyLine(line) or (len(includesToPreserve) > 0 and isInclude(line) and not (line in includesToPreserve)) # or isForwardDeclaration(line)

# Argument parsers
argParser = argparse.ArgumentParser('Sort the declarations in a source file')
argParser.add_argument('inputFile')
argParser.add_argument('outputFile')
argParser.add_argument('correspondingOriginalSource', nargs='?')

args = argParser.parse_args()
sourceText = ''

# If an original source was provided record all of its includes.
# These will be preserved in the currently processed file
if (args.correspondingOriginalSource):
	with open(args.correspondingOriginalSource, 'r') as correspondingSource:
		for line in correspondingSource:
			if isInclude(line):
				includesToPreserve.append(line)

# Read entire file in a single string
with open(args.inputFile, 'r') as inputFile:
	for line in inputFile:
		if not isExcluded(line):
			if not sourceText.endswith('\n'):
				sourceText += '\n'
			while "\t " in line:
				line = line.replace("\t ", "\t")
			sourceText += line.lstrip(' ')


# Block class used to store and various fragments of the file
class Block:
	methodRegex = re.compile(r'.*\)(\s|const|override)*\s*(=\s*\w+)?(\s|\n)*{(\s|\n)*$', re.DOTALL)
	macroRegex = re.compile(r'(\s*[A-Z_]+\s*(\n|\(.*\)\n)|\#include.*)$', re.DOTALL)
	
	def __init__(self):
		self.prefix = ""
		self.suffix = ""
		self.children = []
		self.finalized = False
	
	def add(self, char):
		if self.finalized:
			self.suffix += char
		else:
			self.prefix += char
			
	def removeLast(self, char):
		if self.finalized:
			assert self.suffix.endswith(char)
			self.prefix = self.prefix[:-1]
		else:
			assert self.prefix.endswith(char)
			self.prefix = self.prefix[:-1]
	
	def deepen(self):
		assert not self.finalized
		self.children.append(Block())
		return self.children[-1]
	
	def finalize(self):
		assert not self.finalized
		self.finalized = True
		
	def debugPrint(self):
		if not self.prefix and not self.suffix and not self.children:
			return
		sys.stdout.write('>>>')
		sys.stdout.write(self.prefix)
		match = self.methodRegex.match(self.prefix)
		if match:
			sys.stdout.write('METHOD BODY')
		else:
			for c in self.children:
				assert c != self
				c.debugPrint()
			
		sys.stdout.write(self.suffix)
		sys.stdout.write('<<<')
		
	def sort(self):
		match = self.methodRegex.match(self.prefix)
		if not match:
			self.children.sort(key = lambda x : x.sortingString())
			for c in self.children:
				c.sort()
			
	def sortingString(self):
		return self.prefix
			
	def text(self):
		return self.prefix + (''.join(c.text() for c in self.children))  + self.suffix

# Store current nesting level in a stack
top = Block() # Here we will collect all blocks
stack = [top, top.deepen()]

inString = False
escaped = False
quote = "" #Either " or '

inComment = False
lineComment = False

prev = '' # previous character
current = '' # current character
prevPeer = None # previous peer node

currentLine = ''

# Loop over all the characters and build the tree of Block structures
for char in sourceText:
	if len(currentLine) > 0  and currentLine[-1] == '\n':
		currentLine = ''
	
	prev = current
	current = char
	stack[-1].add(char)
	currentLine += current
	
	if inString:
		assert char != '\n'
		if escaped:
			escaped = False
			continue
		if char == quote:
			inString = False
			continue
		if char == '\\':
			escaped = True
			continue
		continue
	
	if inComment:
		if lineComment and char == '\n':
			lineComment = False
			inComment = False
			continue
		if not lineComment and prev == '*' and char == '/':
			inComment = False
			continue
		continue
	
	assert not inString and not escaped and not inComment and not lineComment
	
	if char == '/' and prev == '/':
		inComment = True
		lineComment = True
		continue
	
	if char == '*' and prev == '/':
		inComment = True
		continue
	
	if char == '"' or char =='\'':
		inString = True
		quote = char
		continue
	
	if char == '{':
		stack.append( stack[-1].deepen() )
		continue
	
	isMacro = Block.macroRegex.match(currentLine) if char == '\n' else False
	if char == '}' or ( char ==';' and not prev == '}') or (char == '\n' and prev == ':') or (char == '\n' and isMacro):
		if char == '}':
			stack[-1].removeLast(char)
			stack[-2].finalize()
			stack[-2].add(char)
			stack = stack[:-1]
		else:
			stack[-1].finalize()
			
		prevPeer = stack[-1] # we need this to handle ; and : and isMacro
		stack = stack[:-1]
		stack.append( stack[-1].deepen())
		continue
	
	if prev == '}' and (char == ';' or char == '\n'):
		stack[-1].removeLast(char)
		prevPeer.add(char)
		continue
	
	if (prev == ';' or isMacro) and char == '\n':
		stack[-1].removeLast(char)
		prevPeer.add(char)
		continue
	
	if prev == '{' and char == '\n':
		stack[-1].removeLast(char)
		stack[-2].add(char)
		continue

#top.debugPrint() # Use this to debug things

top.sort()
with open(args.outputFile, 'w') as outputFile:
	outputFile.write(top.text())