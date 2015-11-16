# Example:
# ast -t=Method|heatmapDataGen|heatmap

import random

for ts in inputs:
    tuples = ts.tuples("ast")
    for tuple in tuples:
        count = random.randrange(100)
        values = [NamedProperty("count", count), NamedProperty("ast", tuple.ast)]
        ts.add(Tuple(values))
    results.append(ts)