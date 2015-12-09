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

#include "MacroImporter.h"

#include "NodeToCloneMap.h"
#include "MacroExpansion.h"
#include "MacroArgumentInfo.h"
#include "MacroArgumentLocation.h"
#include "PPCallback.h"

#include "NodeHelpers.h"

#include "OOModel/src/allOOModelNodes.h"

namespace CppImport {

void MacroImporter::clear()
{
	macroDefinitions_.clear();
	macroExpansions_.clear();
}

MacroImporter::MacroImporter(OOModel::Project* root, EnvisionToClangMap& envisionToClangMap,
									  ClangHelpers& clang)
	: root_(root), clang_{clang}, envisionToClangMap_(envisionToClangMap), macroDefinitions_(clang),
	  macroExpansions_(clang, envisionToClangMap, macroDefinitions_),
	  allMetaDefinitions_(root, clang, macroDefinitions_, macroExpansions_)
	  {}

void MacroImporter::endTranslationUnit()
{
	for (auto expansion : macroExpansions_.topLevelExpansions())
	{
		NodeToCloneMap mapping;
		QVector<Model::Node*> generatedNodes;
		QVector<MacroArgumentInfo> allArgs;
		for (auto node : macroExpansions_.topLevelNodes(expansion, MacroExpansions::NodeOriginType::Transitive))
		{
			auto generatedNode = NodeHelpers::cloneWithMapping(node, mapping);

			allArguments(generatedNode, allArgs, mapping);

			generatedNodes.append(generatedNode);
		}

		handleMacroExpansion(generatedNodes, expansion, mapping, allArgs);

		if (insertMetaCall(expansion))
		{
			OOModel::Declaration* context;

			if (generatedNodes.size() > 0)
				context = NodeHelpers::actualContext(mapping.original(generatedNodes.first()));
			else
				context = actualContext(expansion);

			if (!expansion->xMacroParent() && context)
			{
				if (!DCast<OOModel::Method>(context))
					context->metaCalls()->append(expansion->metaCall());
				else
				{
					if (auto replacementNode = expansion->replacementNode())
						finalizationMetaCalls.insert(replacementNode, expansion);
					else
						qDebug() << "no splice found for expansion"
									<< macroDefinitions_.definitionName(expansion->definition());
				}
			}
		}

		calculateFinalizationNodes(generatedNodes, mapping);

		insertArguments(allArgs);
	}

	// after all other transformations are complete call xMacro transformations
	allMetaDefinitions_.handleXMacros();

	clear();
}

void MacroImporter::insertArguments(QVector<MacroArgumentInfo>& allArguments)
{
	for (auto argument : allArguments)
	{
		if (argument.history_.empty()) continue;

		/*
		 * TODO: check whether this is still necessary. the intermediate argument location should be named correctly.
		 */
		for (auto i = 0; i < argument.history_.size() - 1; i++)
		{
			auto currentLoc = argument.history_[i];
			auto nextLoc = argument.history_[i + 1];

			auto currentArg = currentLoc.expansion_->metaCall()->arguments()->at(currentLoc.argumentNumber_);
			auto newArgValue = clang_.argumentNames(nextLoc.expansion_->definition()).at(nextLoc.argumentNumber_);
			auto newArg = new OOModel::ReferenceExpression(newArgValue);

			currentLoc.expansion_->metaCall()->arguments()->replaceChild(currentArg, newArg);
		}

		/*
		 * the last argument in the argument history is the logically original argument node and location.
		 * therefore we replace the node at the last argument's location with the stored original node.
		 */
		auto lastLoc = argument.history_.last();
		auto lastArg = lastLoc.expansion_->metaCall()->arguments()->at(lastLoc.argumentNumber_);

		if (auto currentArg = DCast<OOModel::ReferenceExpression>(lastArg))
		{
			auto newArg = argument.node_->clone();

			if (!currentArg->name().startsWith("#"))
				lastLoc.expansion_->metaCall()->arguments()->replaceChild(currentArg, newArg);
		}
	}
}

void MacroImporter::calculateFinalizationNodes(QVector<Model::Node*>& nodes, NodeToCloneMap& mapping)
{
	for (auto node : nodes)
	{
		Q_ASSERT(!node->parent());

		if (envisionToClangMap_.contains(mapping.original(node)))
		{
			// check whether this node only exists in generated code from macros
			bool nonMacroInstanceFound = false;
			for (auto range : envisionToClangMap_.get(mapping.original(node)))
				if (!clang_.isMacroRange(range))
				{
					nonMacroInstanceFound = true;
					break;
				}

			if (!nonMacroInstanceFound) finalizationNodes.append(mapping.original(node));
		}
	}
}

void MacroImporter::endEntireImport()
{
	// insert all top level meta calls
	for (auto it = finalizationMetaCalls.begin(); it != finalizationMetaCalls.end(); it++)
		if (DCast<OOModel::Statement>(it.key()))
			it.key()->parent()->replaceChild(it.key(), new OOModel::ExpressionStatement(it.value()->metaCall()));
		else if (DCast<OOModel::Expression>(it.key()))
			it.key()->parent()->replaceChild(it.key(), it.value()->metaCall());
		else if (DCast<OOModel::VariableDeclaration>(it.key()) &&
					DCast<OOModel::VariableDeclarationExpression>(it.key()->parent()))
			it.key()->parent()->parent()->replaceChild(it.key()->parent(), it.value()->metaCall());
		else
			qDebug() << "not inserted top level metacall" << it.key()->typeName();

	// remove all top level meta call generated nodes
	NodeHelpers::removeNodesFromParent(finalizationNodes);
}

void MacroImporter::startTranslationUnit(const clang::SourceManager* sourceManager,
															const clang::Preprocessor* preprocessor)
{
	clang_.setSourceManager(sourceManager);
	clang_.setPreprocessor(preprocessor);
	const_cast<clang::Preprocessor*>(preprocessor)->addPPCallbacks(std::make_unique<PPCallback>(macroDefinitions_,
																															  macroExpansions_));
}

void MacroImporter::handleMacroExpansion(QVector<Model::Node*> nodes, MacroExpansion* expansion,
													  NodeToCloneMap& mapping, QVector<MacroArgumentInfo>& arguments)
{
	// handle child macro expansions
	for (auto childExpansion : expansion->children())
	{
		auto tlNodes = macroExpansions_.topLevelNodes(childExpansion, MacroExpansions::NodeOriginType::Direct);
		handleMacroExpansion(mapping.clone(tlNodes), childExpansion, mapping, arguments);
	}

	// store the original of the first node generated from this expansion as placement information for the meta call
	if (nodes.size() > 0)
		expansion->setReplacementNode(mapping.original(nodes.first()));
	else
		for (auto childExpansion : expansion->children())
			if (childExpansion->replacementNode())
			{
				expansion->setReplacementNode(childExpansion->replacementNode());
				break;
			}

	allMetaDefinitions_.createMetaDef(nodes, expansion, mapping, arguments);
}

bool MacroImporter::insertMetaCall(MacroExpansion* expansion)
{
	auto presumedLocation = clang_.sourceManager()->getPresumedLoc(expansion->range().getBegin());

	QString hash = QDir(presumedLocation.getFilename()).absolutePath()
			+ "|" + macroDefinitions_.signature(expansion->definition())
			+ "|" + QString::number(presumedLocation.getLine())
			+ "|" + QString::number(presumedLocation.getColumn());

	if (!metaCalls_.contains(hash))
	{
		metaCalls_.insert(hash, expansion->metaCall());
		return true;
	}

	SAFE_DELETE(expansion->metaCall());
	expansion->setMetaCall(metaCalls_.value(hash));
	return false;
}

OOModel::Declaration* MacroImporter::actualContext(MacroExpansion* expansion)
{
	Q_ASSERT(!expansion->parent());

	// try to find a valid context where the context range contains the expansion range
	QVector<OOModel::Declaration*> candidates;
	for (auto it = envisionToClangMap_.begin(); it != envisionToClangMap_.end(); it++)
		if (lexicalTransformations_.contains(it.value(), expansion->range()))
			if (NodeHelpers::validContext(it.key()))
			{
				candidates.append(DCast<OOModel::Declaration>(it.key()));
				break;
			}

	// if we could not find a context return the root project
	if (candidates.empty()) return root_;

	auto result = candidates.first();

	for (auto candidate : candidates)
		if (result->isAncestorOf(candidate))
			result = candidate;

	return result;
}

QVector<MacroArgumentLocation> MacroImporter::argumentHistory(clang::SourceRange range)
{
	QVector<MacroArgumentLocation> result;

	if (clang_.sourceManager()->isMacroArgExpansion(range.getBegin()) &&
		 clang_.sourceManager()->isMacroArgExpansion(range.getEnd()))
	{
		QVector<clang::SourceLocation> spellingHistory;
		clang_.immediateSpellingHistory(range.getBegin(), spellingHistory);

		// find all expansions with arguments with location equal to an entry in spellingHistory
		for (auto argumentLoc : spellingHistory)
			for (auto expansion : macroExpansions_.expansions())
				for (auto i = 0; i < expansion->argumentLocs().size(); i++)
					if (expansion->argumentLocs().at(i) == argumentLoc)
						result.append({expansion, i});
	}

	return result;
}

QVector<MacroArgumentLocation> MacroImporter::argumentHistory(Model::Node* node)
{
	QVector<MacroArgumentLocation> result;
	if (envisionToClangMap_.contains(node))
			result = argumentHistory(envisionToClangMap_.get(node).first());
	return result;
}

void MacroImporter::allArguments(Model::Node* node, QVector<MacroArgumentInfo>& result, NodeToCloneMap& mapping)
{
	auto argLoc = argumentHistory(mapping.original(node));

	if (!argLoc.empty())
	{
		result.append({argLoc, node});
		return;
	}

	for (auto child : node->children())
		allArguments(child, result, mapping);
}

}
