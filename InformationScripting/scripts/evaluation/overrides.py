if type(Query.target) is Method:
    allMethods = Query.methods(['-global'], [])[0]
    for astTuple in allMethods.tuples('ast'):
        if astTuple.ast.overrides(Query.target):
            Query.result.add(astTuple)
else:
    raise Exception('Finding overrides of a methods only works on a method.')
    
