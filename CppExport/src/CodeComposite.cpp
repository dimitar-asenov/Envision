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
#include "OOModel/src/declarations/Class.h"

namespace CppExport {

CodeComposite::CodeComposite(const QString& name) : name_{name} {}

void CodeComposite::addUnit(CodeUnit* unit)
{
	units_.append(unit);
	// assert every unit belongs to only one composite
	Q_ASSERT(!unit->composite());
	unit->setComposite(this);
}

QString CodeComposite::relativePath(CodeComposite* other)
{
	QStringList otherName = other->name().split("/");

	if (name() == other->name()) return otherName.last();

	QStringList thisName = name().split("/");
	while (!thisName.empty() && !otherName.empty() && thisName.first() == otherName.first())
	{
		thisName.takeFirst();
		otherName.takeFirst();
	}

	int backSteps = thisName.size() - otherName.size();
	for (auto i = 0; i < backSteps; i++) otherName.prepend("..");
	return otherName.join("/");
}

QSet<Model::Node*> CodeComposite::reduceSoftDependencies(QSet<CodeComposite*> hardDependencies,
																			QSet<Model::Node*> softDependencies)
{
	auto result = softDependencies;
	auto workList = QList<CodeComposite*>::fromSet(hardDependencies);
	QSet<CodeComposite*> processed;

	while (!workList.empty())
	{
		auto hardDependency = workList.takeFirst();

		if (!processed.contains(hardDependency))
		{
			for (auto unit : hardDependency->units())
			{
				for (auto transitiveDependencyHeaderPart : unit->headerPart()->dependencies())
					workList.append(transitiveDependencyHeaderPart->parent()->composite());

				for (auto softDependency : softDependencies)
					if (result.contains(softDependency))
						if (unit->node() == softDependency || unit->node()->isAncestorOf(softDependency))
							result.remove(softDependency);
			}

			processed.insert(hardDependency);
		}
	}

	return result;
}

Export::SourceFragment* CodeComposite::partFragment(CodeUnitPart* (CodeUnit::*part) ())
{
	Q_ASSERT(!units().empty());

	QSet<CodeComposite*> compositeDependencies;
	for (auto unit : units())
		for (CodeUnitPart* dependency : (unit->*part)()->dependencies())
			compositeDependencies.insert(dependency->parent()->composite());

	if ((units().first()->*part)() == units().first()->headerPart())
	{
		compositeDependencies.insert(new CodeComposite("Core/src/core_api"));
		compositeDependencies.remove(this);
	}

	auto composite = new Export::CompositeFragment(units().first()->node());
	if (!compositeDependencies.empty())
	{
		for (auto compositeDependency : compositeDependencies)
				*composite << "#include \"" + relativePath(compositeDependency) + ".h\"\n";

		*composite << "\n";
	}

	Export::CompositeFragment* unitsComposite = nullptr;
	if (!units().isEmpty())
	{
		unitsComposite = new Export::CompositeFragment(units().first()->node(), "spacedSections");

		OOModel::Module* currentNamespace{};
		for (auto unit : units())
		{
			auto codeUnitPart = (unit->*part)();
			if (codeUnitPart->isSourceFragmentEmpty()) continue;

			auto neededNamespace = unit->node()->firstAncestorOfType<OOModel::Module>();
			if (neededNamespace != currentNamespace)
			{
				if (currentNamespace) *unitsComposite << "\n}\n\n";
				if (neededNamespace)
				{
					auto namespaceComposite = new Export::CompositeFragment(unitsComposite->node());
					*namespaceComposite << "namespace " << neededNamespace->symbolName() << " {\n\n";
					unitsComposite->append(namespaceComposite);
				}
				currentNamespace = neededNamespace;
			}

			auto softDependenciesReduced = reduceSoftDependencies(compositeDependencies, codeUnitPart->softDependencies());
			if (!softDependenciesReduced.empty())
			{
				auto softDependencyComposite = new Export::CompositeFragment(units().first()->node());
				for (auto softDependency : softDependenciesReduced)
				{
					if (auto classs = DCast<OOModel::Class>(softDependency))
					{
						if (OOModel::Class::ConstructKind::Class == classs->constructKind())
							*softDependencyComposite << "class ";
						else if (OOModel::Class::ConstructKind::Struct == classs->constructKind())
							*softDependencyComposite << "struct ";
						else if (OOModel::Class::ConstructKind::Enum == classs->constructKind())
							*softDependencyComposite << "enum ";
						else Q_ASSERT(false);

						*softDependencyComposite << softDependency->symbolName() + ";\n";
					}
				}

				*unitsComposite << softDependencyComposite;
			}

			unitsComposite->append(codeUnitPart->sourceFragment());
		}

		if (currentNamespace) *unitsComposite << "\n}";
	}

	if (unitsComposite && !unitsComposite->fragments().empty())
	{
		composite->append(unitsComposite);
		return composite;
	}
	SAFE_DELETE(unitsComposite);
	SAFE_DELETE(composite);
	return nullptr;
}

Export::SourceFragment* CodeComposite::addPragmaOnce(Export::SourceFragment* fragment)
{
	auto compositeFragment = new Export::CompositeFragment(fragment->node());
	*compositeFragment << "#pragma once\n\n" << fragment;
	return compositeFragment;
}

void CodeComposite::sortUnitsByHeaderPartDependencies()
{
	if (units().size() <= 1) return;

	QHash<CodeUnitPart*, QSet<CodeUnitPart*>> headerPartDependencies;
	for (auto unit : units()) headerPartDependencies.insert(unit->headerPart(), unit->headerPart()->dependencies());

	units_.clear();
	for (auto headerPart : topologicalSort(headerPartDependencies)) units_.append(headerPart->parent());
}

void CodeComposite::sortUnitsBySourcePartDependencies()
{
	if (units().size() <= 1) return;

	QHash<CodeUnitPart*, QSet<CodeUnitPart*>> sourcePartDependencies;
	for (auto unit : units())
	{
		sourcePartDependencies[unit->sourcePart()] = {};

		for (auto referenceNode : unit->sourcePart()->referenceNodes())
			if (auto target = referenceNode->target())
				for (auto otherUnit : units())
					if (otherUnit->sourcePart() != unit->sourcePart() &&
						 otherUnit->sourcePart()->nameNodes().contains(target))
							sourcePartDependencies[unit->sourcePart()].insert(otherUnit->sourcePart());
	}

	units_.clear();
	for (auto sourcePart : topologicalSort(sourcePartDependencies)) units_.append(sourcePart->parent());
}

void CodeComposite::fragments(Export::SourceFragment*& header, Export::SourceFragment*& source)
{
	sortUnitsByHeaderPartDependencies();
	header = headerFragment();
	sortUnitsBySourcePartDependencies();
	source = sourceFragment();
}

template <typename T>
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
		{
			// for every other element this element depends on add it to the neededFor map for said other element
			bool notNeededForAnything = true;
			for (auto dependency : it.value())
				if (dependsOn.contains(dependency))
				{
					neededFor[dependency].insert(it.key());
					notNeededForAnything = false;
				}
			if (notNeededForAnything) noPendingDependencies.append(it.key());
		}
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

			bool noPendingDependency = true;
			for (auto d : *dIt)
				if (dependsOn.contains(d))
				{
					noPendingDependency = false;
					break;
				}
			if (noPendingDependency) noPendingDependencies.append(m);
		}
	}

	// test graph for cycles
	for (auto dependencies : dependsOn.values())
		Q_ASSERT(dependencies.empty());

	return result;
}

}
