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
#include "Config.h"
#include "ExportHelpers.h"
#include "SpecialCases.h"
#include "visitors/DeclarationVisitor.h"
#include "visitors/ElementVisitor.h"
#include "visitors/CppPrintContext.h"

#include "Export/src/tree/CompositeFragment.h"
#include "OOModel/src/declarations/Project.h"
#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/expressions/MetaCallExpression.h"
#include "OOModel/src/declarations/NameImport.h"
#include "OOModel/src/declarations/ExplicitTemplateInstantiation.h"

namespace CppExport {

CodeComposite::CodeComposite(const QString& name) : name_{name}
{
	Q_ASSERT(!name.isEmpty());
}

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

	bool nothingInCommon = true;
	QStringList thisName = name().split("/");
	while (!thisName.empty() && !otherName.empty() && thisName.first() == otherName.first())
	{
		thisName.takeFirst();
		otherName.takeFirst();
		nothingInCommon = false;
	}

	if (nothingInCommon)
		return other->name();
	else
	{
		int backSteps = thisName.size() - 1;
		for (auto i = 0; i < backSteps; i++) otherName.prepend("..");
		return otherName.join("/");
	}
}

QSet<DependencyTarget> CodeComposite::reduceSoftDependencies(QSet<CodeComposite*> hardDependencies,
																			QSet<DependencyTarget> softDependencies)
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
				for (auto softDependency : softDependencies)
					if (result.contains(softDependency))
						if (unit->node() == softDependency.node_ || unit->node()->isAncestorOf(softDependency.node_))
							result.remove(softDependency);

			processed.insert(hardDependency);
		}
	}

	return result;
}

Export::CompositeFragment* CodeComposite::addNamespaceFragment(Export::CompositeFragment* parentFragment,
																					OOModel::Module* namespaceNode)
{
	if (!namespaceNode) return parentFragment;

	auto namespaceComposite = parentFragment->append(new Export::CompositeFragment{namespaceNode, "namespace"});
	auto namespaceBody = new Export::CompositeFragment{namespaceNode, "body"};
	*namespaceComposite << namespaceNode->name() << namespaceBody;
	return namespaceBody;
}

QList<CodeUnit*> CodeComposite::nonEmptyUnits(CodeUnitPart* (CodeUnit::*part) ())
{
	QList<CodeUnit*> result;
	for (auto unit : units())
		if (!(unit->*part)()->isSourceFragmentEmpty())
			result.append(unit);
	return result;
}

QSet<CodeComposite*> CodeComposite::calculateDependencies(CodeUnitPart* (CodeUnit::*part) ())
{
	QSet<CodeComposite*> result;
	if (isXMacroData()) return result;

	QList<CodeUnit*> unitsThisDependsOn{units()};
	unitsThisDependsOn << additionalDependencies();
	for (auto unit : unitsThisDependsOn)
		for (CodeUnitPart* dependency : (unit->*part)()->dependencies())
			result.insert(dependency->parent()->composite());

	if ((units().first()->*part)() == units().first()->headerPart())
	{
		if (auto api = ExportHelpers::apiInclude(units().first()->node())) result.insert(api);
		result.remove(this);
	}

	return result;
}

Export::CompositeFragment* CodeComposite::printHardDependencies(CodeUnitPart* (CodeUnit::*part) (),
																					 QSet<CodeComposite*> compositeDependencies)
{
	auto fragment = new Export::CompositeFragment{units().first()->node()};
	for (auto compositeDependency : compositeDependencies)
		if (((units().first()->*part)() != units().first()->headerPart() || headerPartExtension() == ".cpp") &&
			 compositeDependency->isTemplateImplementationSeparateFile() && !isTemplateImplementationSeparateFile())
			*fragment << "#include \"" + relativePath(compositeDependency) + ".hpp\"\n";
		else if (compositeDependency->name().endsWith("_api") ||
					!compositeDependency->units().first()->hasNoHeaderPart())
			*fragment << "#include \"" + relativePath(compositeDependency) + ".h\"\n";
	return fragment;
}

QSet<Model::Node*> CodeComposite::calculateIgnoredSoftDependencies(CodeUnitPart* (CodeUnit::*part) ())
{
	QSet<Model::Node*> result;
	if ((units().first()->*part)() == units().first()->sourcePart())
		for (auto unit : units())
			for (auto softDependency : unit->headerPart()->softDependencies())
				result.insert(softDependency.node_);
	return result;
}

void CodeComposite::printRemainingSoftDependencies(QList<OOModel::Class*> remainingSoftDependencies,
																	Export::CompositeFragment* fragment)
{
	std::sort(remainingSoftDependencies.begin(), remainingSoftDependencies.end(),
			[](OOModel::Class* c1, OOModel::Class* c2)
	{
		auto n1 = ExportHelpers::parentNamespaceModule(c1);
		auto n2 = ExportHelpers::parentNamespaceModule(c2);
		if (!n1 && n2) return true;
		if (!n2) return false;
		Q_ASSERT(n1 && n2);
		return n1->name() < n2->name();
	});

	// print the remaining forward declarations
	OOModel::Module* currentNamespace = nullptr;
	auto currentNamespaceFragment = fragment;
	for (auto softDependency : remainingSoftDependencies)
	{
		auto neededNamespace = ExportHelpers::parentNamespaceModule(softDependency);
		if (neededNamespace != currentNamespace)
		{
			currentNamespaceFragment = addNamespaceFragment(fragment, neededNamespace);
			currentNamespace = neededNamespace;
		}
		*currentNamespaceFragment << printForwardDeclaration(softDependency);
	}
}

Export::CompositeFragment* CodeComposite::printForwardDeclaration(OOModel::Class* classs)
{
	auto fragment = new Export::CompositeFragment{classs};

	if (!classs->typeArguments()->isEmpty())
		*fragment << ElementVisitor{ExportHelpers::parentNamespaceModule(classs)}
						 .visitTemplateArguments(classs->typeArguments());

	if (OOModel::Class::ConstructKind::Class == classs->constructKind())
		*fragment << "class ";
	else if (OOModel::Class::ConstructKind::Struct == classs->constructKind())
		*fragment << "struct ";
	else if (OOModel::Class::ConstructKind::Enum == classs->constructKind())
		*fragment << "enum ";
	else if (OOModel::Class::ConstructKind::EnumClass == classs->constructKind())
		*fragment << "enum class ";
	else Q_ASSERT(false);
	*fragment << classs->symbolName() + ";";
	return fragment;
}

Export::SourceFragment* CodeComposite::partFragment(CodeUnitPart* (CodeUnit::*part) ())
{
	Q_ASSERT(!units().empty());

	auto units = nonEmptyUnits(part);
	if (units.empty()) return nullptr;

	// calculate hard dependencies
	auto compositeDependencies = calculateDependencies(part);

	// print hard dependencies
	auto composite = new Export::CompositeFragment{units.first()->node()};
	*composite << printHardDependencies(part, compositeDependencies);

	auto externalForwardDeclarations = new Export::CompositeFragment{units.first()->node(), "sections"};
	auto unitsComposite = new Export::CompositeFragment{units.first()->node(), "spacedSections"};
	*composite << externalForwardDeclarations << "\n\n" << unitsComposite;

	auto currentNamespaceFragment = unitsComposite;
	QList<OOModel::Module*> namespaces;
	QList<Export::CompositeFragment*> forwardDeclarationFragments;

	// print unit parts
	for (auto unit : units)
	{
		auto neededNamespace = DCast<OOModel::ExplicitTemplateInstantiation>(unit->node()) || isXMacroData() ?
					nullptr : ExportHelpers::parentNamespaceModule(unit->node());
		if (namespaces.empty() || neededNamespace != namespaces.last())
			currentNamespaceFragment = addNamespaceFragment(unitsComposite, neededNamespace);

		namespaces.append(neededNamespace);

		// insert soft dependency fragment
		forwardDeclarationFragments.append(
					currentNamespaceFragment->append(new Export::CompositeFragment{unit->node(), "sections"}));

		// print unit part
		*currentNamespaceFragment << (unit->*part)()->fragment();
	}

	// calculate forward declarations which should be ignored
	// ignore all forward declarations in the source part which also exist in the header part
	auto ignoredSoftDependencies = calculateIgnoredSoftDependencies(part);

	// calculate the latest soft dependency fragment a soft dependency has to be declared
	QHash<OOModel::Class*, int> softDependencyToIndexMap;
	for (auto unit : units)
		for (auto softDependency : reduceSoftDependencies(compositeDependencies,
																		  (unit->*part)()->softDependencies()))
			if (auto classs = DCast<OOModel::Class>(softDependency.node_))
			{
				if (classs != unit->node() && !ignoredSoftDependencies.contains(classs))
				{
					auto it = softDependencyToIndexMap.find(classs);
					if (it != softDependencyToIndexMap.end())
					{
						if (units.indexOf(unit) < it.value())
							it.value() = units.indexOf(unit);
					}
					else
						softDependencyToIndexMap.insert(classs, units.indexOf(unit));
				}
			}
			else if (softDependency.node_ == nullptr && !softDependency.name_.isEmpty())
				*externalForwardDeclarations << "struct " + softDependency.name_ + ";";

	// print forward declarations
	QList<OOModel::Class*> remainingSoftDependencies;
	for (auto it = softDependencyToIndexMap.begin(); it != softDependencyToIndexMap.end(); it++)
	{
		auto classs = it.key();
		auto highestPossibleIndex = it.value();

		// a forward declaration is unnecessary if there is a unit which declares the class the forward declaration
		// refers to before the forward declaration is needed the latest.
		bool forwardDeclarationUnnecessary = false;
		for (auto unit : units)
			if (unit->node() == classs)
				if (units.indexOf(unit) <= highestPossibleIndex)
				{
					forwardDeclarationUnnecessary = true;
					break;
				}
		if (forwardDeclarationUnnecessary) continue;

		// associate the forward declaration with a forward declaration fragment
		// the forward declaration fragment must be in the same namespace as the forward declaration needs to be and
		// the fragment must be before or at the latest possible place the forward declaration needs to be declared
		auto softDependencyNamespace = ExportHelpers::parentNamespaceModule(classs);
		for (int index = highestPossibleIndex; index >= 0; index--)
			if (softDependencyNamespace == namespaces.at(index))
			{
				auto bestIndex = index;
				while (bestIndex > 0 && softDependencyNamespace == namespaces.at(bestIndex - 1)) bestIndex--;
				*forwardDeclarationFragments.at(bestIndex) << printForwardDeclaration(classs);
				break;
			}
			else if (index == 0)
			{
				// in case we found no suitable forward declaration fragment we append it to a list we process later
				remainingSoftDependencies.append(classs);
			}
	}

	// all the remaining forward declarations must appear at the top of the composite fragment.
	// we group them by namespace in order to minimize the amount of namespace declarations we have to print.
	printRemainingSoftDependencies(remainingSoftDependencies, externalForwardDeclarations);

	return composite;
}

Export::SourceFragment* CodeComposite::addPragmaOnce(Export::SourceFragment* fragment)
{
	if (!fragment) return nullptr;

	if (auto classs = DCast<OOModel::Class>(fragment->node()))
		if (SpecialCases::isTestClass(classs)) return fragment;

	auto compositeFragment = new Export::CompositeFragment{fragment->node()};
	*compositeFragment << "#pragma once\n\n" << fragment;
	return compositeFragment;
}

void CodeComposite::sortUnits(CodeUnitPart* (CodeUnit::*part) (),
										std::function<QSet<CodeUnitPart*>(CodeUnitPart*)> dependencies)
{
	if (units().size() <= 1) return;

	QHash<CodeUnitPart*, QSet<CodeUnitPart*>> partDependencies;
	for (auto unit : units())
		partDependencies.insert((unit->*part)(), dependencies((unit->*part)()));

	units_.clear();
	std::function<CodeUnitPart*(QList<CodeUnitPart*>&, CodeUnitPart*)> selector = [](QList<CodeUnitPart*>& parts,
																												CodeUnitPart* previous)
	{
		CodeUnitPart* result = {};
		int resultPriority = 0;
		const int NAME_IMPORT_PRIORITY = 4;
		const int EXPLICIT_TEMPLATE_INSTANTIATION_PRIORITY = 3;
		const int META_CALL_EXPRESSION_PRIORITY = 2;
		const int SAME_NAMESPACE_AS_PREVIOUS_PRIORITY = 1;

		auto previousParentNamespaceModule = previous ? ExportHelpers::parentNamespaceModule(previous->parent()->node())
																	 : nullptr;

		QList<CodeUnitPart*> softIndependentParts;
		for (auto part : parts)
		{
			bool softIndependent = true;
			if (!DCast<OOModel::ExplicitTemplateInstantiation>(part->parent()->node()))
				for (auto other : parts)
					if (part != other)
						if (part->softDependencies().contains(other->parent()->node()))
						{
							softIndependent = false;
							break;
						}
			if (softIndependent) softIndependentParts.append(part);
		}
		if (softIndependentParts.empty()) softIndependentParts = parts;

		for (auto part : softIndependentParts)
			if (DCast<OOModel::NameImport>(part->parent()->node()) && resultPriority < NAME_IMPORT_PRIORITY)
			{
				result = part;
				resultPriority = NAME_IMPORT_PRIORITY;
			}
			else if (DCast<OOModel::ExplicitTemplateInstantiation>(part->parent()->node()) &&
						resultPriority < EXPLICIT_TEMPLATE_INSTANTIATION_PRIORITY)
			{
				result = part;
				resultPriority = EXPLICIT_TEMPLATE_INSTANTIATION_PRIORITY;
			}
			else if (DCast<OOModel::MetaCallExpression>(part->parent()->node()) &&
						resultPriority < META_CALL_EXPRESSION_PRIORITY)
			{
				result = part;
				resultPriority = META_CALL_EXPRESSION_PRIORITY;
			}
			else if (previousParentNamespaceModule &&
						previousParentNamespaceModule == ExportHelpers::parentNamespaceModule(part->parent()->node()))
			{
				result = part;
				resultPriority = SAME_NAMESPACE_AS_PREVIOUS_PRIORITY;
			}
		if (!result) result = softIndependentParts.first();

		parts.removeOne(result);
		return result;
	};
	for (auto part : ExportHelpers::topologicalSort(partDependencies, selector)) units_.append(part->parent());
}

void CodeComposite::fragments(Export::SourceFragment*& header, Export::SourceFragment*& source)
{
	sortUnits(&CodeUnit::headerPart, [](CodeUnitPart* p) { return p->dependencies(); });
	header = headerFragment();
	sortUnits(&CodeUnit::sourcePart, [this](CodeUnitPart* p) { return p->dependenciesWithinFile(units()); });
	source = sourceFragment();
}

}
