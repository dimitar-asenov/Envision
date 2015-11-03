# Removes all BinaryOperation nodes from the input which are not Multiplications
#
# Test to execute: $<"<ast -s=g -t=BinaryOp*>" | "<filterMultiplications>">$

for ts in inputs:
    tuples = ts.tuples("ast")
    for tuple in tuples:
        if type(tuple.ast) is BinaryOperation:
            if tuple.ast.op != BinaryOperation.OperatorTypes.TIMES:
                ts.remove(tuple)
    results.append(ts)