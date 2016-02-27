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

#include "AllMetaDefinitions.h"

#include "MacroDefinitions.h"
#include "MacroExpansion.h"
#include "MacroExpansions.h"

#include "NodeHelpers.h"
#include "../ClangHelpers.h"
#include "NodeToCloneMap.h"

#include "OOModel/src/allOOModelNodes.h"

namespace CppImport {

AllMetaDefinitions::AllMetaDefinitions(OOModel::Project* root, ClangHelpers& clangHelper,
									  const MacroDefinitions& macroDefinitions, MacroExpansions& macroExpansions)
	: root_{root}, clang_{clangHelper}, macroDefinitions_{macroDefinitions}, macroExpansions_{macroExpansions},
	  standardMetaDefinitions_{clangHelper, macroDefinitions, macroExpansions} {}

void AllMetaDefinitions::createMetaDef(QList<Model::Node*> nodes, MacroExpansion* expansion, NodeToCloneMap& mapping,
											 QList<MacroArgumentInfo>& arguments)
{
	if (auto metaDef = standardMetaDefinitions_.createMetaDef(expansion->definition()))
	{
		// check whether this expansion is not a potential partial begin macro specialization
		if (auto beginChild = partialBeginChild(expansion))
			handlePartialBeginSpecialization(metaDef, expansion, beginChild);
		else
			standardMetaDefinitions_.createMetaDefinitionBody(metaDef, nodes, expansion, mapping, arguments);

		clang_.insertDeclarationInFolder(metaDef, expansion->definition()->getMacroInfo()->getDefinitionLoc(),
													metaDefinitionParent(expansion->definition()));
	}
}

OOModel::Declaration* AllMetaDefinitions::metaDefinitionParent(const clang::MacroDirective* md)
{
	OOModel::Declaration* result = clang_.projectForLocation(md->getLocation());
	if (result == clang_.rootProject())
	{
		result = NodeHelpers::findDeclaration(root_->modules(), "ExternalMacro");

		if (!result)
		{
			result = new OOModel::Module{"ExternalMacro", OOModel::Module::ModuleKind::Folder};
			root_->modules()->append(result);
		}
	}

	Q_ASSERT(result);
	return result;
}

void AllMetaDefinitions::handlePartialBeginSpecialization(OOModel::MetaDefinition* metaDef, MacroExpansion* expansion,
																			 MacroExpansion* beginChild)
{
	QList<Model::Node*> statements = macroExpansions_.topLevelNodes(expansion, MacroExpansions::NodeOriginType::Direct);

	if (!statements.empty())
	{
		// create a new list containing all the additional statements defined in expansion (the specialization)
		auto list = new Model::List{};
		for (auto stmt : statements)
		{
			NodeToCloneMap childMapping;
			auto cloned = NodeHelpers::cloneWithMapping(stmt, childMapping);

			// the specialization statements were never part of a standard meta definition so the reference names inside
			// them were never unexpanded and we have to do it now.
			// (copied and adapted from StandardMetaDefinitions::createMetaDefinitionBody)
			QList<Model::Node*> workStack{cloned};
			while (!workStack.empty())
			{
				auto current = workStack.takeLast();
				if (auto referenceExpression = DCast<OOModel::ReferenceExpression>(current))
				{
					auto sourceRanges = clang_.envisionToClangMap().get(childMapping.original(referenceExpression));
					if (!sourceRanges.empty())
						referenceExpression->setName(clang_.unexpandedSpelling(sourceRanges.first().getBegin()));
				}
				workStack << current->children();
			}
			list->append(cloned);
		}

		auto childDef = standardMetaDefinitions_.metaDefinition(beginChild->definition());
		Q_ASSERT(childDef);

		QString cppSpecializationSpliceName = "cppSpecSplice";

		if (!NodeHelpers::findDeclaration(childDef->arguments(), cppSpecializationSpliceName))
		{
			childDef->arguments()->append(new OOModel::FormalMetaArgument{cppSpecializationSpliceName});

			auto classContext = DCast<OOModel::Class>(childDef->context());
			Q_ASSERT(classContext);

			if (classContext->methods()->size() > 0)
				classContext->methods()->last()->items()->append(
							new OOModel::ExpressionStatement{new OOModel::ReferenceExpression{cppSpecializationSpliceName}});
		}

		beginChild->metaCall()->arguments()->append(list);
		specializations_.insert(macroDefinitions_.signature(expansion->definition()), list);
	}

	metaDef->context()->metaCalls()->append(beginChild->metaCall());
}

void AllMetaDefinitions::applyPartialBeginSpecializationTransformation(MacroExpansion* hExpansion,
																						MacroExpansion* cppExpansion)
{
	auto hMetaDefinition = standardMetaDefinitions_.metaDefinition(hExpansion->definition());

	// if the metaBindingInput was not yet added to this header xMacro begin meta definition then add it
	if (!NodeHelpers::findDeclaration(hMetaDefinition->arguments(), "metaBindingInput"))
		hMetaDefinition->arguments()->append(new OOModel::FormalMetaArgument{"metaBindingInput"});

	/*
	 * try to find a child meta call of this meta definition and modify it according to the specialization.
	 * we have to do it over the tree because the inner meta call might originate from an expansion of a
	 * translation unit prior to this one.
	 */
	auto specHash = macroDefinitions_.signature(cppExpansion->definition());

	auto it = specializations_.find(specHash);
	if (it != specializations_.end())
		for (auto expr : *hMetaDefinition->context()->metaCalls())
			if (auto metaCall = DCast<OOModel::MetaCallExpression>(expr))
				if (auto callee = DCast<OOModel::ReferenceExpression>(metaCall->callee()))
					if (callee->name().startsWith("BEGIN_"))
						if (!specialized_.contains(metaCall))
						{
							// avoid multiple specialization transformations
							specialized_.insert(metaCall);

							// apply specialization transformations
							metaCall->arguments()->append((*it)->clone());
							metaCall->arguments()->append(new OOModel::ReferenceExpression{"metaBindingInput"});

							return;
						}
}

void AllMetaDefinitions::handleXMacros()
{
	for (auto expansion : macroExpansions_.expansions())
		if (!expansion->xMacroChildren().empty())
		{
			// case: expansion is a xMacro parent

			// find the .cpp counterpart
			for (auto node : macroExpansions_.topLevelNodes(expansion, MacroExpansions::NodeOriginType::Transitive))
				if (auto other = matchingXMacroExpansion(node))
				{
					// case: expansion is a .h xMacro parent and other is a .cpp xMacro parent

					applyPartialBeginSpecializationTransformation(expansion, other);

					// remove the .cpp xMacro meta call from the original tree
					NodeHelpers::removeNodeFromParent(other->metaCall(), true);

					// create the meta call that also contains unbound xMacro children
					auto merged = new OOModel::MetaCallExpression{};
					clang_.associateNodeWithPresumedFileLocation(merged, expansion->metaCall());
					merged->setCallee(new OOModel::ReferenceExpression{
												macroDefinitions_.definitionName(expansion->definition())});

					for (auto arg : *expansion->metaCall()->arguments())
						merged->arguments()->append(arg->clone());

					// create the unbound xMacro children list
					auto list = new Model::List{};
					for (auto xMacroChild : expansion->xMacroChildren())
					{
						auto unbound = new OOModel::MetaCallExpression{
									macroDefinitions_.definitionName(xMacroChild->definition())};
						for (auto arg : *xMacroChild->metaCall()->arguments())
							unbound->arguments()->append(arg->clone());

						list->append(unbound);
					}
					merged->arguments()->append(list);

					// replace the .h xMacro meta call with the newly created merged meta call
					expansion->metaCall()->parent()->replaceChild(expansion->metaCall(), merged);

					auto metaDef = createXMacroMetaDef(expansion, other);

					// check whether to insert meta bindings for all xMacro children
					for (auto i = 0; i < expansion->xMacroChildren().size(); i++)
					{
						auto xMacroChildH = expansion->xMacroChildren().at(i);
						auto xMacroChildCpp = other->xMacroChildren().at(i);

						auto unboundName = macroDefinitions_.definitionName(xMacroChildH->definition());

						auto binding1 = metaDef->metaBindings()->at(0);
						auto binding2 = metaDef->metaBindings()->at(1);

						// if the binding for this xMacro child already exists do nothing
						if (NodeHelpers::findDeclaration(binding1->mappings(), unboundName)) continue;

						// insert meta bindings for this xMacro child
						auto mapping1 = new OOModel::MetaCallMapping{unboundName};
						mapping1->setValue(new OOModel::ReferenceExpression{
													 macroDefinitions_.definitionName(xMacroChildH->definition())});
						binding1->mappings()->append(mapping1);

						auto mapping2 = new OOModel::MetaCallMapping{unboundName};
						mapping2->setValue(new OOModel::ReferenceExpression{
													 macroDefinitions_.definitionName(xMacroChildCpp->definition())});
						binding2->mappings()->append(mapping2);
					}

					break;
				}
		}
}

OOModel::MetaDefinition* AllMetaDefinitions::createXMacroMetaDef(MacroExpansion* hExpansion,
																					  MacroExpansion* cppExpansion)
{
	auto hBaseExpansion = basePartialBegin(hExpansion);
	auto cppBaseExpansion = basePartialBegin(cppExpansion);

	auto mergedMetaDef = xMacroMetaDefinition(hBaseExpansion->definition());
	if (!mergedMetaDef)
	{
		auto hBaseMetaDef = standardMetaDefinitions_.metaDefinition(hBaseExpansion->definition());
		auto cppBaseMetaDef = standardMetaDefinitions_.metaDefinition(cppBaseExpansion->definition());

		mergedMetaDef = hBaseMetaDef->clone();
		clang_.associateNodeWithPresumedFileLocation(mergedMetaDef, hBaseMetaDef);
		mergedMetaDef->setName(macroDefinitions_.definitionName(hBaseExpansion->definition()));
		xMacrometaDefinitions_.insert(macroDefinitions_.definitionName(hBaseExpansion->definition()), mergedMetaDef);

		for (auto cppArg : *cppBaseMetaDef->arguments())
			if (!NodeHelpers::findDeclaration(mergedMetaDef->arguments(), cppArg->name()))
				mergedMetaDef->arguments()->append(cppArg->clone());

		/* assumptions:
		 * - the context of hBaseMetaDef is a Module
		 * - the context module of hBaseMetaDef contains exactly one class
		 * - the context of cppBaseMetaDef is a Class
		 * - the merged MetaDefinition is correct if we merge those 2 classes
		 */
		auto mergedClass = DCast<OOModel::Class>(DCast<OOModel::Module>(mergedMetaDef->context())->classes()->first());
		auto cppBaseClass = DCast<OOModel::Class>(cppBaseMetaDef->context());

		mergeClasses(mergedClass, cppBaseClass);

		QString metaBindingInputName = "metaBindingInput";
		QString declarationSpliceName = "list1";
		QString statementSpliceName = "list2";

		// add an argument for the input to the MetaBindings
		mergedMetaDef->arguments()->append(new OOModel::FormalMetaArgument{metaBindingInputName});

		// add splices for the MetaBinding results
		mergedClass->metaCalls()->append(new OOModel::ReferenceExpression{declarationSpliceName});
		mergedClass->methods()->last()->items()->append(new OOModel::ExpressionStatement{
																			new OOModel::ReferenceExpression{statementSpliceName}});

		// MetaBinding for declarations splice
		auto declarationsSpliceMetaBinding = new OOModel::MetaBinding{declarationSpliceName};
		declarationsSpliceMetaBinding->setInput(new OOModel::ReferenceExpression{metaBindingInputName});
		mergedMetaDef->metaBindings()->append(declarationsSpliceMetaBinding);

		// MetaBinding for statements splice
		auto statementsSpliceMetaBinding = new OOModel::MetaBinding{statementSpliceName};
		statementsSpliceMetaBinding->setInput(new OOModel::ReferenceExpression{metaBindingInputName});
		mergedMetaDef->metaBindings()->append(statementsSpliceMetaBinding);

		// add the merged MetaDefinition to the tree
		hBaseMetaDef->parent()->replaceChild(hBaseMetaDef, mergedMetaDef);
	}

	NodeHelpers::removeNodeFromParent(standardMetaDefinitions_.metaDefinition(cppExpansion->definition()), true);
	NodeHelpers::removeNodeFromParent(standardMetaDefinitions_.metaDefinition(cppBaseExpansion->definition()), true);

	return mergedMetaDef;
}


MacroExpansion* AllMetaDefinitions::basePartialBegin(MacroExpansion* partialBeginExpansion)
{
	Q_ASSERT(macroDefinitions_.isPartialBegin(partialBeginExpansion->definition()));

	for (auto child : partialBeginExpansion->children())
		if (macroDefinitions_.isPartialBegin(child->definition()))
		 return basePartialBegin(child);

	return partialBeginExpansion;
}

void AllMetaDefinitions::mergeClasses(OOModel::Class* merged, OOModel::Class* mergee)
{
	for (auto metaCall : *mergee->metaCalls())
		merged->metaCalls()->append(metaCall->clone());

	for (auto mergedMethod : *merged->methods())
		for (auto mergeeMethod : *mergee->methods())
		{
			if (mergedMethod->name() == mergeeMethod->name())
			{
				for (auto k = 0; k < mergeeMethod->items()->size(); k++)
					mergedMethod->items()->append(mergeeMethod->items()->at(k)->clone());

				mergedMethod->memberInitializers()->clear();
				for (auto memberInitializer : *mergeeMethod->memberInitializers())
					mergedMethod->memberInitializers()->append(memberInitializer->clone());
			}
		}

}

MacroExpansion* AllMetaDefinitions::partialBeginChild(MacroExpansion* expansion)
{
	for (auto child : expansion->children())
		if (macroDefinitions_.isPartialBegin(child->definition()))
			return child;

	return nullptr;
}

OOModel::MetaDefinition* AllMetaDefinitions::xMacroMetaDefinition(const clang::MacroDirective* md)
{
	QString h = macroDefinitions_.definitionName(md);

	auto it = xMacrometaDefinitions_.find(h);

	return it != xMacrometaDefinitions_.end() ? *it : nullptr;
}

MacroExpansion* AllMetaDefinitions::matchingXMacroExpansion(Model::Node* node)
{
	if (auto metaCall = DCast<OOModel::MetaCallExpression>(node))
		for (auto expansion : macroExpansions_.expansions())
			if (!expansion->xMacroChildren().empty())
				if (expansion->metaCall() == metaCall)
					return expansion;

	for (auto child : node->children())
		if (auto expansion = matchingXMacroExpansion(child))
			return expansion;

	return nullptr;
}

}
