# filterCasts

casts = Query.ast(["-t=CastExpression"] + args, [])[0].value()
castTypeAttributes = Query.attribute(["-at=castType", "-s=of"], [casts])[0].value()
classUses = Query.uses(["-s=of", "-t=Class"], [castTypeAttributes])[0].value()

def hasTypeIdMethod( cl ):
    for method in cl.methods:
        if method.name == "typeIdStatic":
            return True
    return False

result = TupleSet()

for tuple in classUses.tuples("uses"):
    if hasTypeIdMethod(tuple.used):
        values = [NamedProperty("ast", tuple.user)]
        result.add(Tuple(values))

results = Query.toParent(["-t=CastExpression", "-addAs=node"], [result]) 
