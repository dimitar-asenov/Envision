for ts in inputs:
    tuples = ts.tuples("ast")
    for tuple in tuples:
        if type(tuple.ast) is Class:
            print(tuple.ast.constructKind)
    results.append(ts)
