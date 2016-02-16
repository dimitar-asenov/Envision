import html

class HtmlPrintable(object):
    def printTo(self):
        pass

class Table(HtmlPrintable):
    header = []
    rows = []
    def __init__(self, tupleList):
        for tupleEntry in tupleList:
            rowEntry = []
            for valueName, value in tupleEntry.items():
                rowEntry.append(value)
            self.rows.append(rowEntry)
    
    def printTo(self):
        htmlTable = '<table>'
        for row in self.rows:
            htmlTable +='\n\t<tr>'
            for rowValue in row:
                htmlTable += '\n\t\t<td>{}</td>'.format(html.escape(str(rowValue)))
            htmlTable += '\n\t</tr>'
        htmlTable += '\n</table>'
        return htmlTable


tupleDict = {}
for tuple in Query.input.tuples():
    key = tuple.tupleTag()
    if not key in tupleDict:
        tupleDict[key] = []
    valueDict = {}
    for i in range(0, tuple.size()):
        valueDict[tuple[i].name] = tuple[i].value
    tupleDict[key].append(valueDict)

Query.result = Query.input

htmlString = '<!DOCTYPE html><html><body>'
for tupleName, tupleList in tupleDict.items():
    htmlString += Table(tupleList).printTo()

htmlString += '</body></html>'

# Write html to file
with open('foo.html', 'w') as htmlFile:
    htmlFile.write(htmlString)

t = Tuple([('html', 'foo.html')])
Query.result.add(t)

