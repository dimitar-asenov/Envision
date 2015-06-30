from AstApi import *

print(Method.MethodKind.values)

#Create a method from python inserting is not yet possible as we first need the class exposed to this script.
#m = Method('foo')

for method in methods:
    print(method.name)
    print(method.methodKind)
    if method.name == "bubbleSort":
        #res = method.results
        for arg in method.arguments:
            print(arg.name)