results = [];

for g in inputs:
    nodes = g.nodes()
    for node in nodes:
        ast = node.ast;
        print(ast.constructKind)
    results.append(g)
