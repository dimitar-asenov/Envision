if type(Query.target) is IfStatement:
	parent = Query.target.parent
	parent.beginModification('If to while')

	condition = Query.target.condition
	thenBranch = Query.target.thenBranch

	Query.target.replaceChild(condition, Node.createNewNode('Expression', None))
	Query.target.replaceChild(thenBranch, Node.createNewNode('StatementItemList', None))

	whileNode = Node.createNewNode('LoopStatement', None)
	parent.replaceChild(Query.target, whileNode)

	# The while needs to be attached to the tree before it can reuse the parts of the if
	whileNode.condition = condition
	whileNode.body = thenBranch

	parent.endModification()

else:
	raise Exception('ifToWhile command only works on if statements')
