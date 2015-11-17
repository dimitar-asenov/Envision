# example: methods|typePrinter

for tuple in Query.input.tuples("ast"):
    print(type(tuple.ast))

Query.result = Query.input