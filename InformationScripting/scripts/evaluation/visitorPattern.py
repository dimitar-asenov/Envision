if not type(Query.target) is Class:
    raise Exception('Needs to be executed on a class')

visitorName = Query.target.name

def addArgument(argType, argName, toMethod):
    argument = Node.createNewNode('FormalArgument', None)
    argument.typeExpression = AstModification.buildExpression(argType)
    argument.name = argName
    toMethod.arguments.append(argument)

def createVoidMethod(name):
    m = Node.createNewNode('Method', None)
    m.name = name
    returnValue = Node.createNewNode('FormalResult', None)
    returnValue.typeExpression = AstModification.buildExpression('void')
    m.results.append(returnValue)
    return m

def addStatementFromString(expression, toMethod):
    nodeExpr = AstModification.buildExpression(expression)
    printStmt = Node.createNewNode('ExpressionStatement', None)
    printStmt.expression = nodeExpr
    toMethod.items.append(printStmt)

for astTuple in Query.input.tuples('ast'):
    if astTuple.ast.name == visitorName:
        continue
    
    # First create the accept method in the current class
    acceptMethod = createVoidMethod('accept')
    addArgument(visitorName, 'visitor', acceptMethod)
    addStatementFromString('visitor.visit(this)', acceptMethod)
    
    astTuple.ast.beginModification('adding method')
    astTuple.ast.methods.append(acceptMethod)
    astTuple.ast.endModification()
    
    # Now create the visit stub in the visitor class
    visitMethod = createVoidMethod('visit')
    addArgument(astTuple.ast.name, 'node', visitMethod)
    addStatementFromString('System.out.println("Visiting {}")'.format(astTuple.ast.name), visitMethod)
    
    Query.target.beginModification('adding method')
    Query.target.methods.append(visitMethod)
    Query.target.endModification()
