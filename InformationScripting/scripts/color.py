# Example:
# methods|color blue

color = "red"
if (len(args)):
    color = args[0]

for ts in inputs:
    tuples = ts.take("ast")
    for tuple in tuples:
        t = Tuple([NamedProperty("color", color), NamedProperty("ast", tuple.ast)])
        ts.add(t)
    results.append(ts)