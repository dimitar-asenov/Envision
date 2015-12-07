for changeTuple in Query.input.tuples('change'):
    method = changeTuple.ast
    printExpr = AstModification.buildExpression('System.out.println("Executing method {}")'.format(method.name))
    printStmt = Node.createNewNode('ExpressionStatement', None)
    printStmt.expression = printExpr
    method.beginModification('Add print statement')
    method.items.prepend(printStmt)
    method.endModification()
