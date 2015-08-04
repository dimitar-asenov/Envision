from AstApi import *

print(Method.MethodKind.values)

#Create a method from python inserting is not yet possible as we first need the class exposed to this script.
#m = Method('foo')

for method in methods:
    print(method.ast.name)
    print(method.ast.methodKind)
    if method.ast.name == "bubbleSort":
        #res = method.results
        for arg in method.ast.arguments:
            print(arg.name)