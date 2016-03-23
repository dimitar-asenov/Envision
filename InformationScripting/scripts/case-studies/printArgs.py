for t in Query.input.tuples('ast'):
	m = t.ast
	if type(m) is Method:
		call = 'System.out.println("calling ' + m.name + ': "'
		for a in m.arguments:
			call += ' + "' + a.name + '=" + ' + a.name
		call += ')'
		
		m.beginModification('add print statement')
		nodeExpr = AstModification.buildExpression(call)
		printStmt = Node.createNewNode('ExpressionStatement', None)
		printStmt.expression = nodeExpr
		m.items.prepend(printStmt)
		m.endModification()