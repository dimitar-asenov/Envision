for inTuple in Query.input.tuples('ast'):
    if type(inTuple.ast) is IfStatement:
        if inTuple.ast.thenBranch.size() == 0:
            t = Tuple([('message', 'Empty if block'), ('ast', inTuple.ast), ('type', 'warning')])
            Query.result.add(t)
            Query.result.add(inTuple)
