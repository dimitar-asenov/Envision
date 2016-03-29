for node in Query.input.tuples('ast'):
	if isinstance(node.ast, IfStatement):
		if node.ast.elseBranch.size() > 0:
			Query.result.add( node )
