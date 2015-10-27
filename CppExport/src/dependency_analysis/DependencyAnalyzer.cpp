/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2015 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
 **    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 **      following disclaimer in the documentation and/or other materials provided with the distribution.
 **    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
 **      derived from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 ** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 ** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 ** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 ** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 **********************************************************************************************************************/

#include "DependencyAnalyzer.h"

#include "DependencyComposite.h"
#include "Config.h"

#include "OOModel/src/allOOModelNodes.h"

namespace CppExport {

QList<DependencyUnit*> DependencyAnalyzer::units(Model::Node* node)
{
	QList<DependencyUnit*> result;
	units(node, "", result);
	return result;
}

void DependencyAnalyzer::units(Model::Node* current, QString namespaceName, QList<DependencyUnit*>& result)
{
	if (auto ooModule = DCast<OOModel::Module>(current))
	{
		if (ooModule->classes()->size() > 0)
			namespaceName = ooModule->name();
		else
		{
			// macro file
			result.append(new DependencyUnit(namespaceName + "/" + ooModule->name(), current));
			return;
		}
	}
	else if (auto ooClass = DCast<OOModel::Class>(current))
	{
		result.append(new DependencyUnit(namespaceName + "/" + ooClass->name(), current));
		return;
	}

	for (auto child : current->children())
		units(child, namespaceName, result);
}

QList<DependencyComposite*> DependencyAnalyzer::mergeUnits(QList<DependencyUnit*>& units)
{
	QHash<QString, QString> mergeMap = Config::instance().dependencyUnitMergeMap();

	QHash<QString, DependencyComposite*> nameToCompositeMap;
	for (auto unit : units)
	{
		auto it = mergeMap.find(unit->name());
		auto compositeName = it != mergeMap.end() ? *it : unit->name();

		auto cIt = nameToCompositeMap.find(compositeName);
		if (cIt != nameToCompositeMap.end())
			// case A: the composite that unit is a part of already exists => merge
			(*cIt)->addUnit(unit);
		else
		{
			// case B: the composite that unit is a part of does not yet exist
			auto composite = new DependencyComposite(compositeName);
			composite->addUnit(unit);
			nameToCompositeMap.insert(composite->name(), composite);
		}
	}

	return nameToCompositeMap.values();
}

template <class T>
QList<T*> DependencyAnalyzer::topologicalSort(QHash<T*, QSet<T*>> graph)
{
	QHash<T*, QSet<T*>> reverseGraph;
	for (auto it = graph.begin(); it != graph.end(); it++)
		for (auto dependency : it.value())
			reverseGraph[dependency].insert(it.key());

	QList<T*> toBeProcessed;
	for (auto it = graph.begin(); it != graph.end(); it++)
		if (!reverseGraph.contains(it.key()))
			toBeProcessed.append(it.key());

	QList<T*> result;
	while (!toBeProcessed.empty())
	{
		auto n = toBeProcessed.takeFirst();
		result.append(n);

		for (auto m : graph.value(n))
		{
			auto it = reverseGraph.find(m);

			auto oldSize = (*it).size();
			(*it).remove(n);
			auto newSize = (*it).size();

			if (oldSize > newSize && newSize == 0)
				toBeProcessed.append(m);
		}
	}

	for (auto it = reverseGraph.begin(); it != reverseGraph.end(); it++)
		// test graph for cycles
		Q_ASSERT((*it).empty());

	return result;
}

}
