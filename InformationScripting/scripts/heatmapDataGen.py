# Example:
# ast -t=Method|heatmapDataGen|heatmap

import random

for tuple in Query.input.tuples("ast"):
    count = random.randrange(100)
    values = [("count", count), ("ast", tuple.ast)]
    Query.input.add(Tuple(values))

Query.result = Query.input