import os
import random
import json

def getNames(node):
	if node:
		names = getNames(node.parent)
		if node.symbolName():
			names.append(node.symbolName())
		return names
	else:
		return []
	
def getLabel(tuple):
	if isinstance(tuple.value, Node):
		return '.'.join(getNames(tuple.value)[-2:])
	return str(tuple.value)

def getId(tuple):
	if isinstance(tuple.value, Node):
		return '.'.join(getNames(tuple.value))
	
	return str(tuple.value)

nodeLabels = {}
nodes = []
edges = []

#Handle color tuples specially:
if len(Query.args) > 0 and Query.args[0] == 'useColor':
	for colorTuple in Query.input.take('color'):
		nodeId = getId(colorTuple[1])
		nodeLabels[nodeId] = nodeId
		nodes.append(json.dumps({'id': nodeId, 'label': nodeId, 'color': colorTuple.color}))

# Record all nodes and their labels
for tuple in Query.input.tuples():
	for i in range(0, tuple.size()):
		id = getId(tuple[i])
		if not id in nodeLabels:
			valueLabel = getLabel(tuple[i])
			nodeLabels[id] = valueLabel
			nodes.append(json.dumps({'id': id, 'label': valueLabel}))

# Record edges
for tuple in Query.input.tuples():
	if tuple.size() >= 2:
		edges.append(json.dumps({'from': getId(tuple[0]), 'to': getId(tuple[1]), 'arrows':'to'}))

nodesText = ','.join(nodes)
edgesText = ','.join(edges)

with open('scripts/vis-graph/vis-graph.html') as htmlFile:
	htmlText = htmlFile.read().replace('@path', os.path.abspath(".")).replace('@nodes', nodesText).replace('@edges', edgesText)
	t = Tuple([('html', htmlText)])
	Query.result.add(t)
