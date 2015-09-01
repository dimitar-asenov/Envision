results = [];


for ts in inputs:
    tuples = ts.tuples("ast")
    for tuple in tuples:
        asts = tuple.getAll("ast");
        print(asts[0].value.constructKind)
    results.append(ts)
