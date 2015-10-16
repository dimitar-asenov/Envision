#script $<"<replaceCasts>" | "<modify>">$
#script $<"<replaceCasts>" | "<debug>">$

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
        ## Get parent of type CastExpression
        castTuples = TupleSet([Tuple([NamedProperty("ast", tuple.user)])])
        castTuples = Query.toParent(["-t=CastExpression", "-addAs=node"], [castTuples])[0]
        for castTuple in castTuples.take("ast"):
            cast = castTuple.ast
            castTo = tuple.used.name
            newExpr = 'DCast<{}>()'.format(castTo)
            result.add(Tuple([NamedProperty("replace", cast), NamedProperty("with", newExpr), NamedProperty("keepAsarguments", "expr")]))

results.append(result)