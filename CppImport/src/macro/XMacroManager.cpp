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

#include "XMacroManager.h"

#include "ExpansionManager.h"
#include "DefinitionManager.h"
#include "MetaDefinitionManager.h"

#include "StaticStuff.h"

namespace CppImport {

XMacroManager::XMacroManager(DefinitionManager* definitionManager, ExpansionManager* expansionManager,
									  MetaDefinitionManager* metaDefinitionManager)
	: definitionManager_(definitionManager), expansionManager_(expansionManager),
	  metaDefinitionManager_(metaDefinitionManager) {}

void XMacroManager::createMetaDef(QVector<Model::Node*> nodes, MacroExpansion* expansion, NodeMapping* mapping,
											 QVector<MacroArgumentInfo>& arguments)
{
	if (auto metaDef = metaDefinitionManager_->createMetaDef(expansion->definition))
	{
		auto metaDefParent = metaDefinitionManager_->metaDefinitionParent(expansion->definition);

		// check whether this expansion is not a potential partial begin macro specialization
		if (auto beginChild = partialBeginChild(expansion))
			handlePartialBeginSpecialization(metaDefParent, metaDef, expansion, beginChild);
		else
			metaDefinitionManager_->createMetaDefinitionBody(metaDef, nodes, expansion, mapping, arguments);

		metaDefParent->subDeclarations()->append(metaDef);
	}

	// qualify the meta call
	auto callee = DCast<OOModel::ReferenceExpression>(expansion->metaCall->callee());
	callee->setPrefix(definitionManager_->expansionQualifier(expansion->definition));
}

void XMacroManager::handlePartialBeginSpecialization(OOModel::Declaration* metaDefParent,
																	  OOModel::MetaDefinition* metaDef,
																	  MacroExpansion* expansion,
																	  MacroExpansion* beginChild)
{
	QVector<Model::Node*> statements = expansionManager_->nTLExpansionTLNodes(expansion);

	if (!statements.empty())
	{
		// create a new list containing all the additional statements defined in expansion (the specialization)
		auto list = new Model::List();
		for (auto stmt : statements) list->append(stmt->clone());

		auto childDef = metaDefinitionManager_->metaDefinition(beginChild->definition);
		Q_ASSERT(childDef);

		if (metaDefParent->name().endsWith("_CPP"))
		{
			QString cppSpecializationSpliceName = "cppSpecSplice";

			if (!StaticStuff::findDeclaration(childDef->arguments(), cppSpecializationSpliceName))
			{
				childDef->arguments()->append(new OOModel::FormalMetaArgument(cppSpecializationSpliceName));

				auto classContext = DCast<OOModel::Class>(childDef->context());
				Q_ASSERT(classContext);

				if (classContext->methods()->size() > 0)
					classContext->methods()->last()->items()->append(
								new OOModel::ExpressionStatement(new OOModel::ReferenceExpression(cppSpecializationSpliceName)));
			}
		}

		beginChild->metaCall->arguments()->append(list);
		specializations_.insert(definitionManager_->hash(expansion->definition), list);
	}

	metaDef->context()->metaCalls()->append(beginChild->metaCall);
}

void XMacroManager::applyPartialBeginSpecializationTransformation(MacroExpansion* hExpansion,
																						MacroExpansion* cppExpansion)
{
	auto hMetaDefinition = metaDefinitionManager_->metaDefinition(hExpansion->definition);

	// if the metaBindingInput was not yet added to this header xMacro begin meta definition then add it
	if (!StaticStuff::findDeclaration(hMetaDefinition->arguments(), "metaBindingInput"))
		hMetaDefinition->arguments()->append(new OOModel::FormalMetaArgument("metaBindingInput"));

	/*
	 * try to find a child meta call of this meta definition and modify it according to the specialization.
	 * we have to do it over the tree because the inner meta call might originate from an expansion of a
	 * translation unit prior to this one.
	 */
	auto specHash = definitionManager_->hash(cppExpansion->definition);

	auto it = specializations_.find(specHash);
	if (it != specializations_.end())
		for (auto i = 0; i < hMetaDefinition->context()->metaCalls()->size(); i++)
			if (auto metaCall = DCast<OOModel::MetaCallExpression>(hMetaDefinition->context()->metaCalls()->at(i)))
				if (auto callee = DCast<OOModel::ReferenceExpression>(metaCall->callee()))
					if (callee->name().startsWith("BEGIN_"))
						if (!specialized_.contains(metaCall))
						{
							// avoid multiple specialization transformations
							specialized_.insert(metaCall);

							// apply specialization transformations
							metaCall->arguments()->append((*it)->clone());
							metaCall->arguments()->append(new OOModel::ReferenceExpression("metaBindingInput"));

							return;
						}
}

void XMacroManager::handleXMacros()
{
	for (auto expansion : expansionManager_->expansions())
		if (!expansion->xMacroChildren.empty())
		{
			for (auto node : expansionManager_->tLExpansionTLNodes(expansion))
			{
				if (auto other = matchingXMacroExpansion(node))
				{
					applyPartialBeginSpecializationTransformation(expansion, other);

					StaticStuff::removeNode(other->metaCall, true);

					auto merged = new OOModel::MetaCallExpression();
					merged->setCallee(new OOModel::ReferenceExpression(
												definitionManager_->definitionName(expansion->definition),
												definitionManager_->expansionQualifier(expansion->definition)));

					for (auto i = 0; i < expansion->metaCall->arguments()->size(); i++)
						merged->arguments()->append(expansion->metaCall->arguments()->at(i)->clone());

					auto list = new Model::List();
					for (auto xMacroChild : expansion->xMacroChildren)
					{
						auto unbound = new OOModel::MetaCallExpression(
									definitionManager_->definitionName(xMacroChild->definition));
						for (auto i = 0; i < xMacroChild->metaCall->arguments()->size(); i++)
							unbound->arguments()->append(xMacroChild->metaCall->arguments()->at(i)->clone());

						list->append(unbound);
					}

					merged->arguments()->append(list);

					expansion->metaCall->parent()->replaceChild(expansion->metaCall, merged);

					auto metaDef = createXMacroMetaDef(expansion, other);

					for (auto i = 0; i < expansion->xMacroChildren.size(); i++)
					{
						auto xMacroChildH = expansion->xMacroChildren[i];
						auto xMacroChildCpp = other->xMacroChildren[i];

						auto unboundName = definitionManager_->definitionName(xMacroChildH->definition);

						auto binding1 = metaDef->metaBindings()->at(0);
						auto binding2 = metaDef->metaBindings()->at(1);

						if (StaticStuff::findDeclaration(binding1->mappings(), unboundName)) continue;

						auto mapping1 = new OOModel::MetaCallMapping(unboundName);
						mapping1->setValue(new OOModel::ReferenceExpression(
													 definitionManager_->definitionName(xMacroChildH->definition),
													 definitionManager_->expansionQualifier(xMacroChildH->definition)));
						binding1->mappings()->append(mapping1);

						auto mapping2 = new OOModel::MetaCallMapping(unboundName);
						mapping2->setValue(new OOModel::ReferenceExpression(
													 definitionManager_->definitionName(xMacroChildCpp->definition),
													 definitionManager_->expansionQualifier(xMacroChildCpp->definition)));
						binding2->mappings()->append(mapping2);
					}

					break;
				}
			}
		}
}

OOModel::MetaDefinition* XMacroManager::createXMacroMetaDef(MacroExpansion* hExpansion, MacroExpansion* cppExpansion)
{
	auto hBaseExpansion = basePartialBegin(hExpansion);
	auto cppBaseExpansion = basePartialBegin(cppExpansion);

	auto mergedMetaDef = xMacroMetaDefinition(hBaseExpansion->definition);
	if (!mergedMetaDef)
	{
		auto hBaseMetaDef = metaDefinitionManager_->metaDefinition(hBaseExpansion->definition);
		auto cppBaseMetaDef = metaDefinitionManager_->metaDefinition(cppBaseExpansion->definition);

		mergedMetaDef = hBaseMetaDef->clone();
		mergedMetaDef->setName(definitionManager_->definitionName(hBaseExpansion->definition));
		xMacroMetaDefinitions_.insert(definitionManager_->definitionName(hBaseExpansion->definition), mergedMetaDef);

		for (auto i = 0; i < cppBaseMetaDef->arguments()->size(); i++)
		{
			auto cppArg = cppBaseMetaDef->arguments()->at(i);

			if (!StaticStuff::findDeclaration(mergedMetaDef->arguments(), cppArg->name()))
				mergedMetaDef->arguments()->append(cppArg->clone());
		}

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
		mergedMetaDef->arguments()->append(new OOModel::FormalMetaArgument(metaBindingInputName));

		// add splices for the MetaBinding results
		mergedClass->metaCalls()->append(new OOModel::ReferenceExpression(declarationSpliceName));
		mergedClass->methods()->last()->items()->append(new OOModel::ExpressionStatement(
																			new OOModel::ReferenceExpression(statementSpliceName)));

		// MetaBinding for declarations splice
		auto declarationsSpliceMetaBinding = new OOModel::MetaBinding(declarationSpliceName);
		declarationsSpliceMetaBinding->setInput(new OOModel::ReferenceExpression(metaBindingInputName));
		mergedMetaDef->metaBindings()->append(declarationsSpliceMetaBinding);

		// MetaBinding for statements splice
		auto statementsSpliceMetaBinding = new OOModel::MetaBinding(statementSpliceName);
		statementsSpliceMetaBinding->setInput(new OOModel::ReferenceExpression(metaBindingInputName));
		mergedMetaDef->metaBindings()->append(statementsSpliceMetaBinding);

		// add the merged MetaDefinition to the tree
		hBaseMetaDef->parent()->replaceChild(hBaseMetaDef, mergedMetaDef);
	}

	StaticStuff::removeNode(metaDefinitionManager_->metaDefinition(cppExpansion->definition), true);
	StaticStuff::removeNode(metaDefinitionManager_->metaDefinition(cppBaseExpansion->definition), true);

	return mergedMetaDef;
}


MacroExpansion* XMacroManager::basePartialBegin(MacroExpansion* partialBeginExpansion)
{
	Q_ASSERT(definitionManager_->isPartialBegin(partialBeginExpansion->definition));

	for (auto child : partialBeginExpansion->children)
		if (definitionManager_->isPartialBegin(child->definition))
		 return basePartialBegin(child);

	return partialBeginExpansion;
}

void XMacroManager::mergeClasses(OOModel::Class* merged, OOModel::Class* mergee)
{
	for (auto i = 0; i < mergee->metaCalls()->size(); i++)
		merged->metaCalls()->append(mergee->metaCalls()->at(i)->clone());

	for (auto i = 0; i < merged->methods()->size(); i++)
		for (auto j = 0; j < mergee->methods()->size(); j++)
		{
			auto mergedMethod = merged->methods()->at(i);
			auto mergeeMethod = mergee->methods()->at(j);

			if (mergedMethod->name() == mergeeMethod->name())
			{
				for (auto k = 0; k < mergeeMethod->items()->size(); k++)
					mergedMethod->items()->append(mergeeMethod->items()->at(k)->clone());

				mergedMethod->memberInitializers()->clear();
				for (auto k = 0; k < mergeeMethod->memberInitializers()->size(); k++)
					mergedMethod->memberInitializers()->append(mergeeMethod->memberInitializers()->at(k)->clone());
			}
		}

}

MacroExpansion* XMacroManager::partialBeginChild(MacroExpansion* expansion)
{
	for (auto child : expansion->children)
		if (definitionManager_->isPartialBegin(child->definition))
			return child;

	return nullptr;
}

OOModel::MetaDefinition* XMacroManager::xMacroMetaDefinition(const clang::MacroDirective* md)
{
	QString h = definitionManager_->definitionName(md);

	auto it = xMacroMetaDefinitions_.find(h);

	return it != xMacroMetaDefinitions_.end() ? *it : nullptr;
}

MacroExpansion* XMacroManager::matchingXMacroExpansion(Model::Node* node)
{
	if (auto metaCall = DCast<OOModel::MetaCallExpression>(node))
		for (auto expansion : expansionManager_->expansions())
			if (!expansion->xMacroChildren.empty())
				if (expansion->metaCall == metaCall)
					return expansion;

	for (auto child : node->children())
		if (auto expansion = matchingXMacroExpansion(child))
			return expansion;

	return nullptr;
}

}
