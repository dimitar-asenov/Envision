results = [];

from DataApi import *

color = "red"
if (len(args)):
    color = args[0]

for ts in inputs:
    tuples = ts.tuples("ast")
    for tuple in tuples:
        ts.remove(tuple)
        tuple.add(NamedProperty("color", color))
        ts.add(tuple)
    results.append(ts)