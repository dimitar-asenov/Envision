#!/usr/bin/python3

# This script loads two envision files and compares them structurally, ignoring the order of certain types of lists.
# The envision files' IDs must have already been matched since the IDs are used for sorting.
# The arguments are the two Envision files to compare. The first file must be the developer merged version and the second the envision merged version.
#
# If there is a difference the script will output some information about the first difference it finds. Otherwise it will output either EQUAL if the files are structurally identical
# or SUBSET if the developer merged file is a subset of the envision merged file.

import re
import sys
import argparse

# Regex to parse Envision files
envisionLineRegex = re.compile("^(\t*)(\S+) (\S+) \{(\S+)\} \{(\S+)\}(.*)$")

# Define some constants
EQUAL=0
SUBSET=1
DIFFERENT=2

# The Node class represents a single Envision node
class Node:
	def __init__(self, line):
		match = envisionLineRegex.match(line)
		assert match
		
		self.tabs = match.group(1)
		self.label = match.group(2)
		self.type = match.group(3)
		self.id = match.group(4)
		self.parentId = match.group(5)
		self.value = match.group(6)
		self.children = []
		
	def addChild(self, child):
		self.children.append(child)
		
	def shouldSortChildren(self):
		return self.type == 'TypedListOfMethod' or self.type == 'TypedListOfClass' or self.type == 'TypedListOfDeclaration'
	
	def sort(self):
		for c in self.children:
			c.sort()
		if self.shouldSortChildren():
			self.children.sort(key = lambda c: c.id)
			# Erase the label
			for c in self.children:
				c.label = ''
	
	def compare(self, other):
		if other is None:
			print(self.id + " No Match")
			return DIFFERENT
		
		if ( self.tabs == other.tabs
			and self.label == other.label
			and self.type == other.type 
			and self.id == other.id
			and self.parentId == other.parentId
			and self.value == other.value
			and len(self.children) <= len(other.children) ):
			
			# Everything matches so far, compare the children
			childrenEqual = EQUAL
			for selfChild in self.children:
				otherChild = other.child(selfChild.id)
				
				comparison = selfChild.compare(otherChild)
				if comparison == DIFFERENT:
					childrenEqual = DIFFERENT
					break
				
				if comparison == SUBSET:
					childrenEqual = SUBSET
			
			if childrenEqual == EQUAL and len(self.children) < len(other.children):
				childrenEqual = SUBSET
				
			return childrenEqual
		
		print(self.id + "   " + other.id)
		return DIFFERENT
	
	def child(self, id):
		for c in self.children:
			if c.id == id:
				return c
		return None
	
	def numNodes(self):
		result = 1
		for child in self.children:
			result += child.numNodes()
		return result
	
	@staticmethod
	def loadTreeFromLines(lines):
		root = Node(lines[0])
		
		nodeStack = [root]
		for line in lines[1:]:
			node = Node(line)
			
			assert len(node.tabs) <= len(nodeStack)
			if len(node.tabs) < len(nodeStack):
				nodeStack = nodeStack[0:len(node.tabs)]
			assert len(nodeStack) > 0
			
			nodeStack[-1].addChild(node)
			nodeStack.append(node)
			
		assert len(lines) == root.numNodes()
		return root
			
			
argParser = argparse.ArgumentParser('Compare two Envision files for equality, ignoring the order of some lists')
argParser.add_argument('devMergedFilename')
argParser.add_argument('envisionMergedFilename')
args = argParser.parse_args()

# Read input files
with open(args.devMergedFilename) as f:
	linesDev = f.readlines()
with open(args.envisionMergedFilename) as f:
	linesEnvision = f.readlines()

if len(linesDev) > len(linesEnvision):
	print('The developer version has more lines than the Envision-merged one.')
	print('DIFFERENT')
	sys.exit()

treeDev = Node.loadTreeFromLines(linesDev)
treeEnvision = Node.loadTreeFromLines(linesEnvision)

treeDev.sort()
treeEnvision.sort()

comparison = treeDev.compare(treeEnvision)
if comparison == EQUAL:
	print('EQUAL')
if comparison == SUBSET:
	print('SUBSET')
if comparison == DIFFERENT:
	print('DIFFERENT')
