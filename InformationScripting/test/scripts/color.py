# Example:
# $<"<ast -t=Method>" | "<color blue>">$

color = "red"
if (len(args)):
    color = args[0]

for ts in inputs:
    tuples = ts.take("ast")
    for tuple in tuples:
        tuple.add(NamedProperty("color", color))
        ts.add(tuple)
    results.append(ts)