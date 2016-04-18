def packageOf(node):
	package = ''
	node = node.parent
	while node:
		if type(node) is Module:
			package = node.symbolName() + '.' + package
		node = node.parent
	return package

def dependsOnPackages(aClass):
	for decl in aClass.subDeclarations:
		if type(decl) is NameImport:
			package = ''
			name = decl.importedName
			while type(name) is ReferenceExpression:
				package = name.name + '.' + package
				name = name.prefix
			yield package

allPackages = set()
efferent = {}
afferent = {}

for tuple in Query.input.take('ast'):
	p = packageOf(tuple.ast)
	allPackages.add(p)
	deps = dependsOnPackages(tuple.ast)
	if deps:
		efferent[p] = 1 + (efferent[p] if p in efferent else 0)
		for dep in deps:
			afferent[dep] = 1 + (afferent[p] if p in afferent else 0)

for p in allPackages:
	e = efferent[p] if p in efferent else 0
	a = afferent[p] if p in afferent else 0
	i = str( e/(e+a) ) if e+a > 0 else 1
	t = Tuple([('package', p), ('instability', i)])
	Query.result.add(t)