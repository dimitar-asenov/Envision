results = [];

import AstApi

for ts in inputs:
    tuples = ts.tuples("ast")
    for tuple in tuples:
        asts = tuple.getAll("ast");
        if type(asts[0].value) is AstApi.Class:
            print(asts[0].value.constructKind)
    results.append(ts)
