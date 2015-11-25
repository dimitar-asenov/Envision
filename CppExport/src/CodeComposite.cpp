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

#include "CodeComposite.h"

#include "Export/src/tree/CompositeFragment.h"

namespace CppExport {

CodeComposite::CodeComposite(const QString& name) : name_(name) {}

void CodeComposite::addUnit(CodeUnit* unit)
{
	units_.append(unit);
	// assert every unit belongs to only one composite
	Q_ASSERT(!unit->composite());
	unit->setComposite(this);
}

Export::SourceFragment* CodeComposite::partFragment(const QList<CodeUnit*>& codeUnits,
																	 CodeUnitPart* (CodeUnit::*part) ())
{
	Q_ASSERT(!codeUnits.empty());

	auto composite = new Export::CompositeFragment(codeUnits.first()->node());
	for (auto unit : codeUnits)
	{
		for (CodeUnitPart* dep : (unit->*part)()->dependencies())
		{
			*composite << "\n";
			*composite << "#include \"" + dep->parent()->name() + ".h\"";
		}
		*composite << "\n";

		composite->append((unit->*part)()->sourceFragment());
	}

	return composite;
}

QList<CodeUnit*> CodeComposite::sortUnits()
{
	if (units().size() <= 1) return units();

	QHash<CodeUnitPart*, QSet<CodeUnitPart*>> headerPartDependencies;
	for (auto unit : units()) headerPartDependencies.insert(unit->headerPart(), unit->headerPart()->dependencies());

	QList<CodeUnit*> result;
	for (auto headerPart : topologicalSort(headerPartDependencies)) result.append(headerPart->parent());
	return result;
}

template <class T>
QList<T*> CodeComposite::topologicalSort(QHash<T*, QSet<T*>> dependsOn)
{
	// calculate a list of elements with no dependencies.
	// calculate a map that maps from an element to all elements that depend on it.
	QList<T*> noPendingDependencies;
	QHash<T*, QSet<T*>> neededFor;
	for (auto it = dependsOn.begin(); it != dependsOn.end(); it++)
		if (it.value().empty())
			// this element depends on no other elements
			noPendingDependencies.append(it.key());
		else
			// for every other element this element depends on add it to the neededFor map for said other element
			for (auto dependency : it.value())
				neededFor[dependency].insert(it.key());

	QList<T*> result;
	while (!noPendingDependencies.empty())
	{
		// take any item form the list of item with no more dependencies and add it to the result
		auto n = noPendingDependencies.takeFirst();
		result.append(n);

		// check if we are neededFor another node
		auto it = neededFor.find(n);
		if (it == neededFor.end()) continue;

		// for every node we are neededFor
		for (auto m : *it)
		{
			// find the nodes the node we are needed for dependsOn
			auto dIt = dependsOn.find(m);
			Q_ASSERT(dIt != dependsOn.end());

			// remove us from its dependencies
			dIt->remove(n);

			// if this node has no more dependencies add it to the list of items with no more dependencies
			if (dIt->size() == 0)
				noPendingDependencies.append(m);
		}
	}

	// test graph for cycles
	for (auto dependencies : dependsOn.values())
		Q_ASSERT(dependencies.empty());

	return result;
}

}
