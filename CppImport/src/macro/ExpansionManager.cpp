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

#include "ExpansionManager.h"

#include "MacroImportHelper.h"
#include "StaticStuff.h"

namespace CppImport {

ExpansionManager::ExpansionManager(ClangHelper* clang, AstMapping* astMapping, DefinitionManager* definitionManager,
											  LexicalHelper* lexicalHelper)
	: clang_(clang), astMapping_(astMapping), definitionManager_(definitionManager), lexicalHelper_(lexicalHelper) {}

void ExpansionManager::addMacroExpansion(clang::SourceRange sourceRange, const clang::MacroDirective* macroDirective,
													  const clang::MacroArgs* macroArguments)
{
	if (definitionManager_->isPartialEnd(macroDirective))
	{
		/*
		 * if the to be registered expansion's definition is a partial end macro then we are not going to generate a
		 * meta definition for this expansion.
		 * instead we remember that we are currently not in an xMacro body.
		 */
		currentXMacroParent = nullptr;
		return;
	}

	// build new macro expansion entry from the provided information
	auto entry = new MacroExpansion();
	entry->range = sourceRange;
	entry->definition = macroDirective;
	entry->parent = expansion(sourceRange.getBegin());
	if (entry->parent) entry->parent->children.append(entry);

	// handle xMacro data members
	if (definitionManager_->isPartialBegin(macroDirective) && !currentXMacroParent)
		/*
		 * if the definition of this expansion is a partial begin macro we remember that we are now in a xMacro body.
		 * we check whether we are not already in a xMacro body because we want to remember the .h part (the first one) of
		 * the xMacro begins (the potential .cpp part could overwrite the stored information otherwise).
		 */
		currentXMacroParent = entry;
	else if (currentXMacroParent && !entry->parent)
	{
		/*
		 * if we are in a xMacro body and the current expansion is not a top level expansion then this expansion is a
		 * xMacro child of the stored xMacro expansion.
		 */
		entry->xMacroParent = currentXMacroParent;
		currentXMacroParent->xMacroChildren.append(entry);
	}

	entry->metaCall =
			new OOModel::MetaCallExpression(definitionManager_->definitionName(entry->definition));

	if (!macroDirective->getMacroInfo()->isObjectLike()) // only function like macros have braces in their signature to parse
	{
		// extract everything in parentheses of the expansion signature using a regular expression
		QRegularExpression regex ("\\((.*)\\)", QRegularExpression::DotMatchesEverythingOption);
		auto argumentsString = lexicalHelper_->unexpandedSpelling(sourceRange);
		auto match = regex.match(argumentsString);
		auto arguments = match.captured(1).split(",");

		// by default initialize meta call arguments to be reference expressions with the raw spelling at this expansion
		for (auto i = 0; i < clang_->argumentNames(entry->definition).size(); i++)
		{
			auto actualArg = macroArguments->getUnexpArgument((unsigned int)i);
			entry->metaCall->arguments()->append(new OOModel::ReferenceExpression(arguments[i]));
			entry->argumentLocs.append(actualArg->getLocation());
		}
	}

	expansions_.append(entry);
}

void ExpansionManager::clear()
{
	expansionCache_.clear();
	expansions_.clear();
}

QVector<MacroExpansion*> ExpansionManager::topLevelExpansions()
{
	QVector<MacroExpansion*> result;
	for (auto expansion : expansions_)
		if (!expansion->parent)
			result.append(expansion);

	return result;
}


MacroExpansion*ExpansionManager::expansion(clang::SourceLocation loc)
{
	MacroExpansion* expansion = immediateExpansion(loc);
	MacroExpansion* last = expansion;

	if (expansion)
	{
		do
		{
			last = expansion;
			loc = clang_->sourceManager()->getImmediateExpansionRange(loc).first;
			expansion = immediateExpansion(loc);
		} while (expansion && expansion->isChildOf(last));
	}

	return last;
}

MacroExpansion*ExpansionManager::immediateExpansion(clang::SourceLocation loc)
{
	auto expansion = clang_->immediateMacroLocation(loc);
	for (auto i = 0; i < expansions_.size(); i++)
		if (expansions_[i]->range.getBegin() == expansion) return expansions_[i];

	/*
	 * if we have not found an immediate expansion for loc then we try again using the found immediateExpansionLoc.
	 * this can happen in case of token concatenation or stringifycation where the first expansion location would point
	 * to the location of the concatenated token or stringifycation result.
	 */
	expansion = clang_->immediateMacroLocation(expansion);
	for (auto i = 0; i < expansions_.size(); i++)
		if (expansions_[i]->range.getBegin() == expansion) return expansions_[i];

	return nullptr;
}


QSet<MacroExpansion*> ExpansionManager::expansion(Model::Node* node)
{
	Q_ASSERT(node);

	if (!expansionCache_.contains(node))
	{
		expansionCache_[node] = {};

		if (auto n = astMapping_->closestParentWithAstMapping(node))
			if (astMapping_->contains(n))
				for (auto range : astMapping_->get(n))
				{
					auto exp = expansion(range.getBegin());
					if (exp)	expansionCache_[node].insert(exp);
				}
	}

	return expansionCache_[node];
}


QVector<Model::Node*> ExpansionManager::tLExpansionTLNodes(MacroExpansion* expansion)
{
	Q_ASSERT(expansion);
	Q_ASSERT(!expansion->parent); // ensure expansion is a top level expansion

	QVector<Model::Node*> allTLExpansionNodes;
	for (auto node : astMapping_->nodes())
		for (auto range : astMapping_->get(node))
			// for all mapped nodes check whether any of their ranges expand to the top level macro range
			if (clang_->sourceManager()->getExpansionLoc(range.getBegin()) == expansion->range.getBegin())
			{
				allTLExpansionNodes.append(node);
				break;
			}

	QVector<Model::Node*> result = StaticStuff::topLevelNodes(allTLExpansionNodes);
	StaticStuff::orderNodes(result);
	return result;
}


QVector<Model::Node*> ExpansionManager::nTLExpansionTLNodes(MacroExpansion* exp)
{
	Q_ASSERT(exp);

	QVector<Model::Node*> allNTLExpansionNodes;
	for (auto node : astMapping_->nodes())
		if (expansion(node).contains(exp))
			allNTLExpansionNodes.append(node);

	QVector<Model::Node*> result = StaticStuff::topLevelNodes(allNTLExpansionNodes);
	StaticStuff::orderNodes(result);
	return result;
}

}
