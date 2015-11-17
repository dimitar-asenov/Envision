# Example:
# ast -t=Method|heatmapDataGen|heatmap

import random

for tuple in Query.input.tuples("ast"):
    count = random.randrange(100)
    values = [NamedProperty("count", count), NamedProperty("ast", tuple.ast)]
    Query.input.add(Tuple(values))

Query.result = Query.input