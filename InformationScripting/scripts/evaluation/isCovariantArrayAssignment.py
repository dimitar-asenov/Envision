for astTuple in Query.input.tuples('ast'):
    assignment = astTuple.ast
    leftType = assignment.left.type()
    rightType = assignment.right.type()
    if type(leftType) is ArrayType and type(rightType) is ArrayType:
        if not leftType.elementType().equals(rightType.elementType()):
            Query.result.add(astTuple)