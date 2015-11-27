for tuple in Query.input.tuples():
    print('{}: {{'.format(tuple.tupleTag()), end='')
    for i in range(0, tuple.size()):
        print('{{{}, {}}}'.format(tuple[i].name, tuple[i].value), end='')
    print('}')
Query.result = Query.input
