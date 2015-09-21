#$<"<ast -s=g -t=CastExpression>" | "<uses -s=of -t=Class>" | "<filterCasts>">$
import AstApi
from DataApi import Tuple, NamedProperty

def checkClass( cl ):
    for method in cl.methods:
        if method.name == "typeIdStatic":
            return True
    return False

results = [];

for ts in inputs:
    tuples = ts.tuples("uses")
    for tuple in tuples:
        if not checkClass(tuple.used):
            removeTuple = Tuple([NamedProperty("ast", tuple.user)])
            ts.remove(removeTuple)
    results.append(ts) 
