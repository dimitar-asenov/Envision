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

void ExpansionManager::addMacroExpansion(clang::SourceRange sr,
													  const clang::MacroDirective* md, const clang::MacroArgs* args)
{
	if (definitionManager_->isPartialEnd(md))
	{
		currentXMacroParent = nullptr;
		return;
	}

	auto entry = new MacroExpansion();
	entry->range = sr;
	entry->definition = md;
	entry->parent = expansion(sr.getBegin());
	if (entry->parent) entry->parent->children.append(entry);

	if (definitionManager_->isPartialBegin(md) && !currentXMacroParent)
		currentXMacroParent = entry;
	else if (currentXMacroParent && !entry->parent)
	{
		entry->xMacroParent = currentXMacroParent;
		currentXMacroParent->xMacroChildren.append(entry);
	}

	entry->metaCall =
			new OOModel::MetaCallExpression(definitionManager_->definitionName(entry->definition));

	if (!md->getMacroInfo()->isObjectLike())
	{
		QRegularExpression regex ("\\((.*)\\)", QRegularExpression::DotMatchesEverythingOption);
		auto argumentsString = lexicalHelper_->unexpandedSpelling(sr);
		auto match = regex.match(argumentsString);
		auto arguments = match.captured(1).split(",");

		for (auto i = 0; i < clang_->argumentNames(entry->definition).size(); i++)
		{
			auto actualArg = args->getUnexpArgument((unsigned int)i);
			entry->metaCall->arguments()->append(new OOModel::ReferenceExpression(arguments[i]));
			entry->argumentLocs.append(actualArg->getLocation());
		}
	}

	expansions_.append(entry);
}

QVector<MacroExpansion*> ExpansionManager::expansions() { return expansions_; }


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
	auto expansion = clang_->immediateMacroLoc(loc);
	for (auto i = 0; i < expansions_.size(); i++)
		if (expansions_[i]->range.getBegin() == expansion) return expansions_[i];

	expansion = clang_->immediateMacroLoc(expansion);
	for (auto i = 0; i < expansions_.size(); i++)
		if (expansions_[i]->range.getBegin() == expansion) return expansions_[i];

	return nullptr;
}


QSet<MacroExpansion*> ExpansionManager::expansion(Model::Node* node)
{
	if (!node) return {}; //TODO: necessary?

	//if (!expansionCache_.contains(node))
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
	Q_ASSERT(!expansion->parent); // ensure TLExpansion

	QVector<Model::Node*> allTLExpansionNodes;
	for (auto node : astMapping_->nodes())
		for (auto range : astMapping_->get(node))
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
