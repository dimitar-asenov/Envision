# script $<"<changes>" | "<countChanges>" | "<heatmap>">$

# assume single input
input = inputs[0]
commits = input.take("commit")
nodeCounts = {}
for commit in commits:
    sha1 = commit.commit
    for change in input.take(sha1):
        key = getattr(change, sha1)
        nodeCounts[key] = nodeCounts.get(key, 0) + 1

result = TupleSet()
for node, count in nodeCounts.items():
    result.add(Tuple([NamedProperty("count", count), NamedProperty("ast", node)]))

results = [result]