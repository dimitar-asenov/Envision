import csv

def findMethod(profileName):
    # This assumes no nested classes/namespaces, i.e. the format should be:
    # ClassName.methodName(...)
    nameWithoutBracket = profileName.split('(')[0]
    astTuples = Query.methods(['-global', '-name', nameWithoutBracket], [])[0]
    # Assume we have exactly one matching:
    return astTuples.tuples('ast').pop().ast

with open(Query.args[0]) as csvfile:
    profileCSVReader = csv.reader(csvfile, delimiter=';', quotechar='"')
    next(profileCSVReader, None) # Ignore the header
    for row in profileCSVReader:
        if float(row[2]) > 10.0:
            m = findMethod(row[0])
            Query.result.add(Tuple([('ast', m)]))