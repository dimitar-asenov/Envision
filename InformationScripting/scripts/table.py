import html

from xml.etree import ElementTree
from xml.etree.ElementTree import Element, SubElement
from xml.dom import minidom

def getNames(node):
    if node:
        names = getNames(node.parent)
        if node.symbolName():
            names.append(node.symbolName())
        return names
    else:
        return []

def getLabel(node):
    if isinstance(node, Node):
        return '.'.join(getNames(node)[-2:])
    return str(node)

class HtmlTable(Element):
    def __init__(self, tupleList):
        Element.__init__(self, 'table')
        # Add header
        if len(tupleList):
            header = Element('tr')
            for valueName, value in tupleList[0].items():
                headerEntry = Element('th')
                headerEntry.text = valueName
                header.append(headerEntry)
            self.append(header)
        # Add values
        for tupleEntry in tupleList:
            row = Element('tr')
            for valueName, value in tupleEntry.items():
                rowEntry = Element('td')
                rowEntry.text = getLabel(value)
                row.append(rowEntry)
            self.append(row)

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

body = Element('body')
for tupleName, tupleList in tupleDict.items():
    paragraph = Element('p')
    title = Element('h1')
    title.text = tupleName
    paragraph.append(title)
    paragraph.append(HtmlTable(tupleList))
    body.append(paragraph)

htmlRoot = Element('html')
htmlRoot.append(body)

style = Element('style')
style.text = """
h1 {margin-bottom: 0; padding-bottom: 0;}
*{font-family:sans-serif;}
th, td {
    border: 1px white;
    background-color:#efefef;
}
th {
    color: #fce5cd;
    background-color: #3d78d6;
}
"""
htmlRoot.append(style)

htmlString = ElementTree.tostring(htmlRoot, 'utf-8', 'html').decode('utf-8')

print(htmlString)

t = Tuple([('html', htmlString)])
Query.result.add(t)

