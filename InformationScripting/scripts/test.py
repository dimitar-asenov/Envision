#example: classes|test

for tuple in Query.input.tuples("ast"):
    if type(tuple.ast) is Class:
        print(tuple.ast.constructKind)
        
Query.result = Query.input
