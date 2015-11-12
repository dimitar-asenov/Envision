# profileSource scripts/carmodel.csv|heatmap

import csv

def findMethod(profileName):
    # This assumes no nested classes/namespaces, i.e. the format is should be:
    # ClassName.methodName(...)
    nameWithoutBracket = profileName.split('(')[0]
    astTuples = Query.ast(['-s=g', '-t=Method', '-n={}'.format(nameWithoutBracket)], inputs)[0].value()
    # Assume we only have one matching, return the first (set doesn't support indexing thus the loop workaround):
    for astTuple in astTuples.tuples("ast"):
        return astTuple.ast

result = TupleSet()

with open(args[0]) as csvfile:
    profileCSVReader = csv.reader(csvfile, delimiter=';', quotechar='"')
    next(profileCSVReader, None) # Ignore the header
    for row in profileCSVReader:
        m = findMethod(row[0])
        count = int(float(row[1]))
        values = [NamedProperty("count", count), NamedProperty("ast", m)]
        result.add(Tuple(values))
    
results.append(result)