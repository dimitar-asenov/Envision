classUses = Query.executeQuery('ast -t=CastExpression | attribute -at=castType | definitions -t=Class', [])

def hasTypeIdMethod( cl ):
    for method in cl.methods:
        if method.name == 'typeIdStatic':
            return True
    return False

for definitionTuple in classUses[0].tuples('definition'):
    if hasTypeIdMethod(definitionTuple.definition):
        values = [('ast', definitionTuple.reference)]
        Query.result.add(Tuple(values))

Query.result = Query.toParent(['-t=CastExpression'], [Query.result])[0]