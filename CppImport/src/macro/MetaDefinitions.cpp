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

#include "MetaDefinitions.h"

#include "MacroExpansion.h"
#include "MacroArgumentInfo.h"
#include "MacroDefinitions.h"
#include "MacroExpansions.h"
#include "LexicalHelper.h"
#include "StaticStuff.h"
#include "XMacroManager.h"

namespace CppImport {

MetaDefinitions::MetaDefinitions(const ClangHelper& clang, const MacroDefinitions& definitionManager,
															MacroExpansions& MacroExpansions, const LexicalHelper& lexicalHelper)
	: clang_(clang), definitionManager_(definitionManager), MacroExpansions_(MacroExpansions),
	  lexicalHelper_(lexicalHelper) {}

OOModel::MetaDefinition* MetaDefinitions::createMetaDef(const clang::MacroDirective* md)
{
	if (metaDefinition(md)) return nullptr;

	auto metaDef = new OOModel::MetaDefinition(definitionManager_.definitionName(md));
	metaDefinitions_.insert(definitionManager_.hash(md), metaDef);

	// add formal arguments based on the expansion definition
	for (auto argName : clang_.argumentNames(md))
		metaDef->arguments()->append(new OOModel::FormalMetaArgument(argName));

	return metaDef;
}

OOModel::MetaDefinition* MetaDefinitions::metaDefinition(const clang::MacroDirective* md)
{
	QString h = definitionManager_.hash(md);

	auto it = metaDefinitions_.find(h);

	return it != metaDefinitions_.end() ? *it : nullptr;
}

void MetaDefinitions::createMetaDefinitionBody(OOModel::MetaDefinition* metaDef, QVector<Model::Node*> nodes,
																	  MacroExpansion* expansion, NodeMapping* mapping,
																	  QVector<MacroArgumentInfo>& arguments)
{
	if (nodes.size() > 0)
	{
		// create a new context with type equal to the first node's context
		auto actualContext = StaticStuff::actualContext(mapping->original(nodes.first()));
		metaDef->setContext(StaticStuff::createContext(actualContext));

		// clone and add nodes to the metaDef
		for (auto n : nodes)
		{
			NodeMapping childMapping;
			auto cloned = StaticStuff::cloneWithMapping(mapping->original(n), &childMapping);
			lexicalHelper_.applyLexicalTransformations(cloned, &childMapping, clang_.argumentNames(expansion->definition));

			insertChildMetaCalls(expansion, &childMapping);
			if (removeUnownedNodes(cloned, expansion, &childMapping)) continue;
			insertArgumentSplices(mapping, &childMapping, arguments);

			StaticStuff::addNodeToDeclaration(cloned, metaDef->context());
		}
	}

	// add all child expansion meta calls that are not yet added anywhere else as declaration meta calls
	for (auto childExpansion : expansion->children)
		if (!childExpansion->metaCall->parent())
			metaDef->context()->metaCalls()->append(childExpansion->metaCall);
}

void MetaDefinitions::insertChildMetaCalls(MacroExpansion* expansion, NodeMapping* childMapping)
{
	for (auto childExpansion : expansion->children)
	{
		// do not handle xMacro children here
		if (childExpansion->xMacroParent) continue;

		// retrieve the node that the child meta call should replace
		if (auto replacementNode = childExpansion->replacementNode_)
			// replacementNode is an original node therefore we need to get to the cloned domain first
			// clonedReplacementNode represents the cloned version of replacementNode
			if (auto clonedReplacementNode = childMapping->clone(replacementNode))
				if (!DCast<OOModel::Declaration>(clonedReplacementNode))
				{
					if (clonedReplacementNode->parent())
						clonedReplacementNode->parent()->replaceChild(clonedReplacementNode, childExpansion->metaCall);
					else
						qDebug() << "not inserted metacall" << clonedReplacementNode->typeName();
				}
	}
}

void MetaDefinitions::childrenUnownedByExpansion(Model::Node* node, MacroExpansion* expansion,
																			NodeMapping* mapping, QVector<Model::Node*>* result)
{
	Q_ASSERT(expansion);

	// do not remove child meta calls
	if (DCast<OOModel::MetaCallExpression>(node)) return;

	if (auto original = mapping->original(node))
		if (MacroExpansions_.expansion(original).contains(expansion))
		{
			for (auto child : node->children())
				childrenUnownedByExpansion(child, expansion, mapping, result);

			return;
		}

	result->append(node);
}

bool MetaDefinitions::removeUnownedNodes(Model::Node* cloned, MacroExpansion* expansion, NodeMapping* mapping)
{
	QVector<Model::Node*> unownedNodes;
	childrenUnownedByExpansion(cloned, expansion, mapping, &unownedNodes);

	// if the unowned nodes contain the node itself then the node should not even be added to the meta definition
	if (unownedNodes.contains(cloned)) return true;

	StaticStuff::removeNodes(StaticStuff::topLevelNodes(unownedNodes));

	return false;
}

void MetaDefinitions::insertArgumentSplices(NodeMapping* mapping, NodeMapping* childMapping,
																  QVector<MacroArgumentInfo>& arguments)
{
	for (auto argument : arguments)
	{
		// map the argument node to the corresponding node in childMapping
		auto original = mapping->original(argument.node_);

		if (auto child = childMapping->clone(original))
		{
			// the first entry of the spelling history is where the splice for this argument should be
			auto spliceLoc = argument.history_.first();

			// the splice name is equal to the formal argument name where the argument is coming from
			auto argName = clang_.argumentNames(spliceLoc.expansion_->definition).at(spliceLoc.argumentNumber_);
			auto newNode = new OOModel::ReferenceExpression(argName);

			// insert the splice into the tree
			if (child->parent()) child->parent()->replaceChild(child, newNode);
			childMapping->replaceClone(child, newNode);
		}
	}
}

}
