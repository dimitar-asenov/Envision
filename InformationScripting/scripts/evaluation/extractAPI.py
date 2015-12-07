def addToResultIfPublic(ast):
    if ast.modifiers.isSet(Modifier.ModifierFlag.Public):
        Query.result.add(Tuple([('ast', ast)]))

if type(Query.target) is Class:
    tClass = Query.target
    for method in tClass.methods:
        addToResultIfPublic(method)
    for field in tClass.fields:
        addToResultIfPublic(field)
    
else:
    raise Exception('extractAPI only works on classes')
