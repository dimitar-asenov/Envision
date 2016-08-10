#!/usr/bin/python3

# This script loads two envision files and compares them structurally, ignoring the order of certain types of lists.
# The envision files' IDs must have already been matched since the IDs are used for sorting.
# The arguments are the two Envision files to compare
#
# If there is a difference the script will output some information about the first difference it finds. Otherwise there will be no output.

import re
import sys
import argparse

# Regex to parse Envision files
envisionLineRegex = re.compile("^(\t*)(\S+) (\S+) \{(\S+)\} \{(\S+)\}(.*)$")

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
			childIndex = 0
			for c in self.children:
				c.label = str(childIndex)
				childIndex += 1
	
	def isEqual(self, other):
		if ( self.tabs == other.tabs
			and self.label == other.label
			and self.type == other.type 
			and self.id == other.id
			and self.parentId == other.parentId
			and self.value == other.value
			and len(self.children) == len(other.children) ):
			
			# Everything matches so far, compare the children
			childrenEqual = True
			for selfChild, otherChild in zip(self.children, other.children):
				if not selfChild.isEqual(otherChild):
					childrenEqual = False
					break
			
			return childrenEqual
		print(self.id + "   " + other.id)
		return False
	
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
argParser.add_argument('filenameA')
argParser.add_argument('filenameB')
args = argParser.parse_args()

# Read input files
with open(args.filenameA) as f:
	linesA = f.readlines()
with open(args.filenameB) as f:
	linesB = f.readlines()

if len(linesA) != len(linesB):
	print('Different number of nodes')
	sys.exit()

treeA = Node.loadTreeFromLines(linesA)
treeB = Node.loadTreeFromLines(linesB)

treeA.sort()
treeB.sort()

if not treeA.isEqual(treeB):
	print('Nodes')
