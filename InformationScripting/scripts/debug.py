for ts in inputs:
    tuples = ts.tuples()
    for tuple in tuples:
        print('{}: {{'.format(tuple.tupleTag()), end="")
        for i in range(0, tuple.size()):
            print('{{{}, {}}}'.format(tuple[i].name, tuple[i].value), end="")
        print('}')
    results.append(ts)
