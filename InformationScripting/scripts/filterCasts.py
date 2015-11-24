# filterCasts

casts = Query.ast(["-t=CastExpression"] + Query.args, [])
castTypeAttributes = Query.attribute(["-at=castType", "-s=of"], casts)
classUses = Query.uses(["-s=of", "-t=Class"], castTypeAttributes)

def hasTypeIdMethod( cl ):
    for method in cl.methods:
        if method.name == "typeIdStatic":
            return True
    return False

for tuple in classUses[0].tuples("uses"):
    if hasTypeIdMethod(tuple.used):
        values = [("ast", tuple.user)]
        Query.result.add(Tuple(values))

Query.result = Query.toParent(["-t=CastExpression", "-addAs=node"], [Query.result])[0]
