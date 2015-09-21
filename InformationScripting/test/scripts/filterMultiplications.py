# Removes all BinaryOperation nodes from the input which are not Multiplications
#
# Test to execute: $<"<ast -s=g -t=BinaryOp*>" | "<filterMultiplications>">$

results = [];

import AstApi

for ts in inputs:
    tuples = ts.tuples("ast")
    for tuple in tuples:
        if type(tuple.ast) is AstApi.BinaryOperation:
            if tuple.ast.op != AstApi.BinaryOperation.OperatorTypes.TIMES:
                ts.remove(tuple)
    results.append(ts)