package InformationScripting;

import Graph;

import InformationNode;
import InformationEdge;

class Graph {

InformationNode add(InformationNode node)
{
	assert (!nodeHashFunctions_.empty());

	InformationNode existingNode = findNode(node);

	if (existingNode && existingNode != node)
	{
		mergeNodes(existingNode, node);
	}
	else
	{
		nodes_[hashOf(node)] = node;
		existingNode = node;
	}
	return existingNode;
}

InformationEdge addEdge(InformationNode a, 
	InformationNode b, 
	QString name)
{
	// We only allow existing nodes:
	assert (a == findNode(a));
	assert (b == findNode(b));
	for (auto edge : edges_)
	{
		if ((edge.from() == a || edge.from() == b) && (edge.to() == b || edge.to() == a) && edge.name() == name)
		{
			auto existingEdge = edge;
			assert (!existingEdge.isDirected());
			existingEdge.incrementCount();
			return existingEdge;
		}
	}
	auto edge = new InformationEdge{a, b, name, Undirected};
	edges_.push_back(edge);
	return edge;
}

void remove(InformationNode node)
{
	assert (node);
	auto hash = hashOf(node);
	auto nodeIt = nodes_.find(hash);
	// If the node isn't in the graph we don't have to do anything:
	if (nodeIt == nodes_.end()) return;

	// It could be that node points to a different node with the same hash.
	// TODO in this case we might also want to merge the infos?
	auto ownedNode = node;
	if (ownedNode != nodeIt) ownedNode = nodeIt;

	for (auto edgeIt = edges_.begin(); edgeIt != edges_.end();)
	{
		auto edge = edgeIt;
		if (edge.from() == ownedNode || edge.to() == ownedNode)
		{
			edgeIt = edges_.erase(edgeIt);
		}
		else
			++edgeIt;
	}

	nodes_.erase(nodeIt);
}

void remove(QList<InformationNode> nodes)
{
	for (auto node : nodes) remove(node);
}

QList<InformationNode> nodes(NodeCondition holds) 
{
	if (!holds) return nodes_.values();

	QList<InformationNode> result;
	for (auto node : nodes_) if (holds(node)) result.push_back(node);
	return result;
}

QList<InformationEdge> edges(EdgeCondition holds) 
{
	if (!holds) return edges_;

	QList<InformationEdge> result;
	for (auto edge : edges_) if (holds(edge)) result.push_back(edge);
	return result;
}

size_t hashOf( InformationNode n) 
{
	for (auto hashFunction : nodeHashFunctions_)
	{
		auto result = hashFunction(n);
		if (result.second) return result.first;
	}
	// There should be at least one function registered for a certain node
	assert (false);
}

void findNode( InformationNode n)
{
	auto hash = hashOf(n);
	auto it = nodes_.find(hash);
	if (it != nodes_.end()) return it.value();
	return;
}

void mergeNodes(InformationNode into, InformationNode from)
{
	for (auto namedProperty : from)
	{
		QString name = namedProperty.first;
		if (!into.contains(name))
			into.insert(name, namedProperty.second);
		else
			assert ((from)[name] == namedProperty.second);
	}
}

} /* namespace InformationScripting */
