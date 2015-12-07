for astTuple in Query.input.tuples('ast'):
    if type(astTuple.ast) is Field:
        modifiers = astTuple.ast.modifiers
        nonFinalPublic = modifiers.isSet(Modifier.ModifierFlag.Public) and not modifiers.isSet(Modifier.ModifierFlag.Final)
        if not nonFinalPublic:
            Query.input.remove(astTuple)

Query.result = Query.input