results = [];

import AstApi

for ts in inputs:
    tuples = ts.tuples("ast")
    for tuple in tuples:
        print(type(tuple.ast))
    results.append(ts) 
