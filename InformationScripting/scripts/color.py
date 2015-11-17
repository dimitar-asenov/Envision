# Example:
# methods|color blue

color = "red"
if (len(Query.args)):
    color = Query.args[0]

for tuple in Query.input.take("ast"):
    t = Tuple([NamedProperty("color", color), NamedProperty("ast", tuple.ast)])
    Query.result.add(t)