astTuples = Query.input.tuples('ast')
firstClass = astTuples.pop().ast
    
for m in firstClass.methods:
    for otherClass in astTuples:
        for otherM in otherClass.ast.methods:
            if m.name == otherM.name:
                Query.result.add(Tuple([('ast', m)]))
                Query.result.add(Tuple([('ast', otherM)]))