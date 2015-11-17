#Example: methods|examples/renameMethodsToFoo

for tuple in Query.input.tuples("ast"):
    method = tuple.ast
    method.beginModification("rename")
    method.name = "Foo"
    method.endModification()