import html

class HtmlElement(object):
    def __init__(self, tag):
        self.tag = tag

    def html(self):
        htmlString = '<{}>'.format(self.tag)
        htmlString += self.printContent()
        htmlString += '</{}>'.format(self.tag)
        return htmlString
    
    def printContent(self):
        pass
    
class HtmlDocument(object):
    def __init__(self, body):
        self.body = body
        
    def html(self):
        htmlString = '<!DOCTYPE html><html>'
        htmlString += body.html()
        htmlString += '</html>'
        return htmlString
    
class HtmlBody(HtmlElement):
    def __init__(self):
        super(HtmlBody, self).__init__('body')
        self.elements = []
        
    def table(self, tableContent):
        self.elements.append(HtmlTable(tableContent))
        
    def printContent(self):
        htmlString = ''
        for element in self.elements:
            htmlString += element.html()
        return htmlString

class HtmlTable(HtmlElement):
    header = []
    rows = []
    def __init__(self, tupleList):
        super(HtmlTable,self).__init__('table')
        for tupleEntry in tupleList:
            rowEntry = []
            for valueName, value in tupleEntry.items():
                rowEntry.append(value)
            self.rows.append(rowEntry)
    
    def printContent(self):
        htmlTable = ''
        for row in self.rows:
            htmlTable +='\n\t<tr>'
            for rowValue in row:
                htmlTable += '\n\t\t<td>{}</td>'.format(html.escape(str(rowValue)))
            htmlTable += '\n\t</tr>'
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

body = HtmlBody()
for tupleName, tupleList in tupleDict.items():
    body.table(tupleList)

t = Tuple([('html', HtmlDocument(body).html())])
Query.result.add(t)

