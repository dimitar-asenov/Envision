#script "<filterCasts>"

casts = Query.ast(["-t=CastExpression"] + args, [])
castTypeAttributes = Query.attribute(["-at=castType", "-s=of"], casts)
classUses = Query.uses(["-s=of", "-t=Class"], castTypeAttributes)

def hasTypeIdMethod( cl ):
    for method in cl.methods:
        if method.name == "typeIdStatic":
            return True
    return False

result = TupleSet()

for tuple in classUses[0].tuples("uses"):
    if hasTypeIdMethod(tuple.used):
        values = [NamedProperty("ast", tuple.user)]
        result.add(Tuple(values))

results = Query.toParent(["-t=CastExpression", "-addAs=node"], [result]) 
