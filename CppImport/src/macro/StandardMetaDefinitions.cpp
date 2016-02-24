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

#include "StandardMetaDefinitions.h"
#include "../ClangHelpers.h"
#include "MacroDefinitions.h"
#include "MacroExpansion.h"
#include "MacroExpansions.h"
#include "MacroArgumentInfo.h"
#include "MacroArgumentLocation.h"
#include "NodeToCloneMap.h"
#include "NodeHelpers.h"
#include "../SpecialCases.h"
#include "../Comment.h"

#include "OOModel/src/allOOModelNodes.h"

namespace CppImport {

StandardMetaDefinitions::StandardMetaDefinitions(ClangHelpers& clang, const MacroDefinitions& definitionManager,
																 MacroExpansions& macroExpansions)
	: clang_{clang}, definitionManager_{definitionManager}, macroExpansions_{macroExpansions} {}

OOModel::MetaDefinition* StandardMetaDefinitions::createMetaDef(const clang::MacroDirective* md)
{
	if (metaDefinition(md)) return nullptr;

	auto metaDef = new OOModel::MetaDefinition{definitionManager_.definitionName(md)};
	standardMetaDefinitions_.insert(definitionManager_.signature(md), metaDef);

	// add formal arguments based on the expansion definition
	for (auto argName : clang_.argumentNames(md))
		metaDef->arguments()->append(new OOModel::FormalMetaArgument{argName});

	return metaDef;
}

OOModel::MetaDefinition* StandardMetaDefinitions::metaDefinition(const clang::MacroDirective* md)
{
	QString h = definitionManager_.signature(md);

	auto it = standardMetaDefinitions_.find(h);

	return it != standardMetaDefinitions_.end() ? *it : nullptr;
}

void StandardMetaDefinitions::createMetaDefinitionBody(OOModel::MetaDefinition* metaDef, QList<Model::Node*> nodes,
																		 MacroExpansion* expansion, NodeToCloneMap& mapping,
																		 QList<MacroArgumentInfo>& arguments)
{
	if (nodes.size() > 0)
	{
		// create a new context with type equal to the first node's context
		auto actualContext = NodeHelpers::actualContext(mapping.original(nodes.first()));
		metaDef->setContext(NodeHelpers::createContext(actualContext));

		// meta definition comment
		auto macroDefinitionPresumedLocation = clang_.sourceManager()->getPresumedLoc(
					expansion->definition()->getMacroInfo()->getDefinitionLoc());
		for (auto comment : clang_.comments())
			if (comment->fileName() == macroDefinitionPresumedLocation.getFilename() &&
				 comment->lineEnd() == macroDefinitionPresumedLocation.getLine() - 1)
				metaDef->setComment(new Comments::CommentNode{comment->text()});

		// clone and add nodes to the metaDef
		for (auto n : nodes)
		{
			NodeToCloneMap childMapping;
			auto cloned = NodeHelpers::cloneWithMapping(mapping.original(n), childMapping);

			// use unexpanded spelling for all references and names
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
				else if (auto nameText = DCast<Model::NameText>(current))
				{
					auto sourceRanges = clang_.envisionToClangMap().get(childMapping.original(nameText));

					/*
					 * In cases where the actual context is a class and the name of the context class is specified as a macro
					 * argument (in the example "className") we change the name of the context class to be equal to the macro
					 * argument instead of the generic "Context" caption.
					 *
					 * Example:
					 * #define DEFINE_TYPE_ID_BASE(className, nameExpression, templatePrefix)
					 * templatePrefix void className::initType() {}
					 */
					if (DCast<OOModel::Class>(metaDef->context()) && metaDef->context()->name() == "Context")
						for (auto sourceRange : sourceRanges)
						{
							auto expandedSourceRange = clang_.getUnexpandedRange(sourceRange);
							if (clang_.spelling(clang::SourceRange(
														  expandedSourceRange.getBegin().getLocWithOffset(-2),
														  expandedSourceRange.getBegin().getLocWithOffset(-1))) != "::")
								continue;

							for (auto argument : *metaDef->arguments())
							{
								auto prefixEnd = expandedSourceRange.getBegin().getLocWithOffset(-3);
								auto prefixSpelling = clang_.spelling(clang::SourceRange(
																					prefixEnd.getLocWithOffset(-argument->name().size() + 1),
																					prefixEnd));
								if (prefixSpelling == argument->name())
								{
									metaDef->context()->setName(argument->name());
									break;
								}
							}
						}

					if (!sourceRanges.empty())
						nameText->set(clang_.unexpandedSpelling(sourceRanges.first().getBegin()));
				}
				workStack << current->children();
			}

			insertChildMetaCalls(expansion, childMapping);
			if (removeUnownedNodes(cloned, expansion, childMapping)) continue;
			insertArgumentSplices(mapping, childMapping, arguments);

			// handle predefined meta definition: SET_OVERRIDE_FLAG
			SpecialCases::overrideFlag(metaDef, cloned);

			NodeHelpers::addNodeToDeclaration(cloned, metaDef->context());
		}
	}

	// add all child expansion meta calls that are not yet added anywhere else as declaration meta calls
	for (auto childExpansion : expansion->children())
		if (!childExpansion->metaCall()->parent())
			metaDef->context()->metaCalls()->append(childExpansion->metaCall());
}

void StandardMetaDefinitions::insertChildMetaCalls(MacroExpansion* expansion, NodeToCloneMap& childMapping)
{
	for (auto childExpansion : expansion->children())
	{
		// do not handle xMacro children here
		if (childExpansion->xMacroParent()) continue;

		// retrieve the node that the child meta call should replace
		if (auto replacementNode = childExpansion->replacementNode())
			// replacementNode is an original node therefore we need to get to the cloned domain first
			// clonedReplacementNode represents the cloned version of replacementNode
			if (auto clonedReplacementNode = childMapping.clone(replacementNode))
			{
				if (clonedReplacementNode->parent())
				{
					if (DCast<OOModel::VariableDeclaration>(clonedReplacementNode))
					{
						if (clonedReplacementNode->parent()->parent())
							clonedReplacementNode->parent()->parent()
									->replaceChild(clonedReplacementNode->parent(), childExpansion->metaCall());
						else
							qDebug() << "not inserted metacall to"
										<< definitionManager_.definitionName(childExpansion->definition())
										<< "in" << definitionManager_.definitionName(expansion->definition());
					}
					else if (DCast<OOModel::Expression>(clonedReplacementNode))
						clonedReplacementNode->parent()->replaceChild(clonedReplacementNode, childExpansion->metaCall());
					else if (DCast<OOModel::Statement>(clonedReplacementNode))
						clonedReplacementNode->parent()->replaceChild(clonedReplacementNode,
																			new OOModel::ExpressionStatement{childExpansion->metaCall()});
					else if (DCast<OOModel::TypeAlias>(clonedReplacementNode))
						clonedReplacementNode->firstAncestorOfType<OOModel::Declaration>()->metaCalls()->append(
									childExpansion->metaCall());
					else
						qDebug() << "not inserted metacall to"
									<< definitionManager_.definitionName(childExpansion->definition())
									<< "in" << definitionManager_.definitionName(expansion->definition());
				}
				else
					qDebug() << "not inserted metacall to"
								<< definitionManager_.definitionName(childExpansion->definition())
								<< "in" << definitionManager_.definitionName(expansion->definition());
			}
	}
}

void StandardMetaDefinitions::childrenUnownedByExpansion(Model::Node* node, MacroExpansion* expansion,
																			NodeToCloneMap& mapping, QList<Model::Node*>& result)
{
	Q_ASSERT(expansion);

	// do not remove child meta calls
	if (DCast<OOModel::MetaCallExpression>(node)) return;

	if (auto original = mapping.original(node))
	{
		if (macroExpansions_.expansions(original).contains(expansion) ||
			 original->typeName() == "TypedListOfExpression" ||
			 original->typeName() == "FormalResult" ||
			 original->typeName() == "FormalArgument")
		{
			for (auto child : node->children())
				childrenUnownedByExpansion(child, expansion, mapping, result);

			return;
		}


		result.append(node);
	}
}

bool StandardMetaDefinitions::removeUnownedNodes(Model::Node* cloned, MacroExpansion* expansion,
																 NodeToCloneMap& mapping)
{
	QList<Model::Node*> unownedNodes;
	childrenUnownedByExpansion(cloned, expansion, mapping, unownedNodes);

	// if the unowned nodes contain the node itself then the node should not even be added to the meta definition
	if (unownedNodes.contains(cloned)) return true;

	NodeHelpers::removeNodesFromParent(NodeHelpers::topLevelNodes(unownedNodes));

	return false;
}

void StandardMetaDefinitions::insertArgumentSplices(NodeToCloneMap& mapping, NodeToCloneMap& childMapping,
																	 QList<MacroArgumentInfo>& arguments)
{
	for (auto argument : arguments)
	{
		// map the argument node to the corresponding node in childMapping
		auto original = mapping.original(argument.node_);

		if (auto child = childMapping.clone(original))
		{
			// the first entry of the spelling history is where the splice for this argument should be
			auto spliceLoc = argument.history_.first();

			// the splice name is equal to the formal argument name where the argument is coming from
			auto argName = clang_.argumentNames(spliceLoc.expansion_->definition()).at(spliceLoc.argumentNumber_);
			if (!argument.nameOnly_)
			{
				Model::Node* newNode = new OOModel::ReferenceExpression{argName};

				// in case the node to replace is not an expression we have to wrap the splice
				if (DCast<OOModel::FormalResult>(child))
					newNode = new OOModel::FormalResult{QString{}, DCast<OOModel::Expression>(newNode)};
				else if (DCast<OOModel::Statement>(child))
					newNode = new OOModel::ExpressionStatement{DCast<OOModel::Expression>(newNode)};

				// insert the splice into the tree
				if (child->parent()) child->parent()->replaceChild(child, newNode);
				childMapping.replaceClone(child, newNode);
			}
			else
				DCast<OOModel::ReferenceExpression>(child)->setName(argName);
		}
	}
}

}
