# Removes all BinaryOperation nodes from the input which are not Multiplications
#
# Test to execute: ast -s=g -t=BinaryOp*|filterMultiplications

for tuple in Query.input.tuples("ast"):
    if type(tuple.ast) is BinaryOperation:
        if tuple.ast.op != BinaryOperation.OperatorTypes.TIMES:
            Query.input.remove(tuple)
Query.result = Query.input