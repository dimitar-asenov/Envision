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
				for (auto softDependency : softDependencies)
					if (result.contains(softDependency))
						if (unit->node() == softDependency || unit->node()->isAncestorOf(softDependency))
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

Export::SourceFragment* CodeComposite::partFragment(CodeUnitPart* (CodeUnit::*part) ())
{
	Q_ASSERT(!units().empty());

	QSet<CodeComposite*> compositeDependencies;
	for (auto unit : units())
		for (CodeUnitPart* dependency : (unit->*part)()->dependencies())
			compositeDependencies.insert(dependency->parent()->composite());

	if ((units().first()->*part)() == units().first()->headerPart())
	{
		if (auto api = ExportHelpers::apiInclude(units().first()->node())) compositeDependencies.insert(api);
		compositeDependencies.remove(this);
	}

	auto composite = new Export::CompositeFragment{units().first()->node()};
	if (!compositeDependencies.empty())
	{
		for (auto compositeDependency : compositeDependencies)
			if (((units().first()->*part)() != units().first()->headerPart() || headerPartExtension() == ".cpp") &&
				 compositeDependency->isTemplateImplementationSeparateFile() && !isTemplateImplementationSeparateFile())
				*composite << "#include \"" + relativePath(compositeDependency) + ".hpp\"\n";
			else if (compositeDependency->name().endsWith("_api") ||
						!compositeDependency->units().first()->hasNoHeaderPart())
				*composite << "#include \"" + relativePath(compositeDependency) + ".h\"\n";

		*composite << "\n";
	}

	Export::CompositeFragment* unitsComposite = nullptr;
	if (!units().isEmpty())
	{
		unitsComposite = new Export::CompositeFragment{units().first()->node(), "spacedSections"};

		OOModel::Module* currentNamespace{};
		auto currentNamespaceFragment = unitsComposite;
		for (auto unit : units())
		{
			auto codeUnitPart = (unit->*part)();
			if (codeUnitPart->isSourceFragmentEmpty()) continue;

			auto softDependenciesReduced = reduceSoftDependencies(compositeDependencies, codeUnitPart->softDependencies());
			if (!softDependenciesReduced.empty())
			{
				if ((units().first()->*part)() == units().first()->headerPart())
					for (auto i = 0; i < units().indexOf(unit); i++)
						softDependenciesReduced.remove(units().at(i)->node());
				else
					for (auto codeUnit : units())
						softDependenciesReduced.subtract(codeUnit->headerPart()->softDependencies());

				for (auto softDependency : softDependenciesReduced)
				{
					if (auto classs = DCast<OOModel::Class>(softDependency))
					{
						if (classs == codeUnitPart->parent()->node()) continue;

						auto neededNamespace = ExportHelpers::parentNamespaceModule(classs);
						if (neededNamespace != currentNamespace)
						{
							currentNamespaceFragment = addNamespaceFragment(unitsComposite, neededNamespace);
							currentNamespace = neededNamespace;
						}

						if (!classs->typeArguments()->isEmpty())
							*currentNamespaceFragment << ElementVisitor{neededNamespace}
																  .visitTemplateArguments(classs->typeArguments());

						auto softDependencyComposite = currentNamespaceFragment->append(
																											new Export::CompositeFragment{classs});
						if (OOModel::Class::ConstructKind::Class == classs->constructKind())
							*softDependencyComposite << "class ";
						else if (OOModel::Class::ConstructKind::Struct == classs->constructKind())
							*softDependencyComposite << "struct ";
						else if (OOModel::Class::ConstructKind::Enum == classs->constructKind())
							*softDependencyComposite << "enum ";
						else Q_ASSERT(false);
						*softDependencyComposite << softDependency->symbolName() + ";";
					}
				}
			}

			auto neededNamespace = DCast<OOModel::ExplicitTemplateInstantiation>(unit->node()) ? nullptr :
					ExportHelpers::parentNamespaceModule(unit->node());
			if (neededNamespace != currentNamespace)
			{
				currentNamespaceFragment = addNamespaceFragment(unitsComposite, neededNamespace);
				currentNamespace = neededNamespace;
			}

			*currentNamespaceFragment << codeUnitPart->fragment();
		}
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
