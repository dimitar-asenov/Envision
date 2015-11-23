# Example:
# methods|color blue

import argparse

parser = argparse.ArgumentParser()
parser.add_argument('color', default='red', nargs='?')
parser.add_argument('--tag', '-t', default='ast', nargs='?')
parsedArgs = parser.parse_args(Query.args)

tag = parsedArgs.tag
color = parsedArgs.color

for tuple in Query.input.take(tag):
    t = Tuple([NamedProperty("color", color), NamedProperty("ast", tuple.ast)])
    Query.result.add(t)