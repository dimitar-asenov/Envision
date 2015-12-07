for astTuple in Query.input.tuples('ast'):
    leftExpr = astTuple.ast.left
    if type(leftExpr) is ReferenceExpression and type(leftExpr.target()) is Field:
        field = leftExpr.target()
        if type(field.typeExpression) is ReferenceExpression:
            typeTarget = field.typeExpression.target()
            if type(typeTarget) is Class:
                if typeTarget.name == Query.args[0]:
                    Query.result.add(astTuple)
                else:
                    for aClass in typeTarget.allBaseClasses():
                        if aClass.name == Query.args[0]:
                            Query.result.add(astTuple)
            
