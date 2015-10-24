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

#include "MacroExpansions.h"

#include "ClangHelpers.h"
#include "MacroDefinitions.h"
#include "MacroExpansion.h"
#include "EnvisionToClangMap.h"

#include "NodeHelpers.h"

#include "OOModel/src/expressions/MetaCallExpression.h"
#include "OOModel/src/expressions/ReferenceExpression.h"

namespace CppImport {

MacroExpansions::MacroExpansions(const ClangHelpers& clang, const EnvisionToClangMap& astMapping,
											  const MacroDefinitions& macroDefinitions)
	: clang_(clang), envisionToClangMap_(astMapping), macroDefinitions_(macroDefinitions) {}

void MacroExpansions::addMacroExpansion(clang::SourceRange sourceRange, const clang::MacroDirective* macroDirective,
													  const clang::MacroArgs* macroArguments)
{
	if (macroDefinitions_.isPartialEnd(macroDirective))
	{
		/*
		 * if the to be registered expansion's definition is a partial end macro then we are not going to generate a
		 * meta definition for this expansion.
		 * instead we remember that we are currently not in an xMacro body.
		 */
		currentXMacroParent = nullptr;
		return;
	}

	auto parent = expansion(sourceRange.getBegin());

	// if we are in a xMacro body and the current expansion is not a top level expansion then this expansion is a
	// xMacro child of the stored xMacro expansion.
	MacroExpansion* xMacroParent = currentXMacroParent && !parent ? currentXMacroParent : nullptr;

	auto entry = new MacroExpansion(sourceRange, macroDirective, parent, xMacroParent);
	if (parent) parent->children().append(entry);
	if (xMacroParent) xMacroParent->xMacroChildren().append(entry);

	if (!currentXMacroParent && macroDefinitions_.isPartialBegin(macroDirective))
		/*
		 * if the definition of this expansion is a partial begin macro we remember that we are now in a xMacro body.
		 * we check whether we are not already in a xMacro body because we want to remember the .h part (the first one) of
		 * the xMacro begins (the potential .cpp part could overwrite the stored information otherwise).
		 */
		currentXMacroParent = entry;

	entry->setMetaCall(new OOModel::MetaCallExpression(macroDefinitions_.definitionName(entry->definition())));

	// only function like macros have braces in their signature to parse
	if (!macroDirective->getMacroInfo()->isObjectLike())
	{
		// extract everything in parentheses of the expansion signature using a regular expression
		QRegularExpression regex ("\\((.*)\\)", QRegularExpression::DotMatchesEverythingOption);
		auto argumentsString = clang_.spelling(sourceRange);
		auto match = regex.match(argumentsString);
		auto arguments = match.captured(1).split(",");

		// by default initialize meta call arguments to be reference expressions with the raw spelling at this expansion
		for (auto i = 0; i < clang_.argumentNames(entry->definition()).size(); i++)
		{
			auto actualArg = macroArguments->getUnexpArgument((unsigned int)i);
			entry->metaCall()->arguments()->append(new OOModel::ReferenceExpression(arguments[i]));
			entry->argumentLocs().append(actualArg->getLocation());
		}

		/* alternative implementation:
		 * works on individual argument tokens and does not need a regular expression.
		 * more robust but depends on unexpandedSpelling.

		auto actualArgFirstToken = macroArguments->getUnexpArgument((unsigned int)i);

		auto actualArgLastToken = actualArgFirstToken;
		for (; actualArgLastToken->isNot(clang::tok::eof); ++actualArgLastToken);
		if (actualArgFirstToken != actualArgLastToken) --actualArgLastToken;

		auto unexpandedArgument = lexicalTransformation_.unexpandedSpelling(
					clang::SourceRange(actualArgFirstToken->getLocation(), actualArgLastToken->getLocation()));

		entry->metaCall->arguments()->append(new OOModel::ReferenceExpression(unexpandedArgument));
		entry->argumentLocs.append(actualArgFirstToken->getLocation());*/
	}

	expansions_.append(entry);
}

void MacroExpansions::clear()
{
	expansionCache_.clear();
	expansions_.clear();
}

QVector<MacroExpansion*> MacroExpansions::topLevelExpansions() const
{
	QVector<MacroExpansion*> result;
	for (auto expansion : expansions_)
		if (!expansion->parent())
			result.append(expansion);

	return result;
}


MacroExpansion* MacroExpansions::expansion(clang::SourceLocation loc) const
{
	MacroExpansion* expansion = immediateExpansion(loc);
	MacroExpansion* last = expansion;

	if (expansion)
	{
		do
		{
			last = expansion;
			loc = clang_.sourceManager()->getImmediateExpansionRange(loc).first;
			expansion = immediateExpansion(loc);
		} while (expansion && expansion->isChildOf(last));
	}

	return last;
}

MacroExpansion* MacroExpansions::immediateExpansion(clang::SourceLocation loc) const
{
	auto expansion = clang_.immediateMacroLocation(loc);
	for (auto macroExpansion : expansions_)
		if (macroExpansion->range().getBegin() == expansion) return macroExpansion;

	/*
	 * if we have not found an immediate expansion for loc then we try again using the found immediateExpansionLoc.
	 * this can happen in case of token concatenation or stringification where the first expansion location would point
	 * to the location of the concatenated token or stringification result.
	 */
	expansion = clang_.immediateMacroLocation(expansion);
	for (auto macroExpansion : expansions_)
		if (macroExpansion->range().getBegin() == expansion) return macroExpansion;

	return nullptr;
}


QSet<MacroExpansion*> MacroExpansions::expansions(Model::Node* node)
{
	Q_ASSERT(node);

	auto it = expansionCache_.find(node);
	if (it != expansionCache_.end()) return *it;

	QSet<MacroExpansion*> result;
	if (auto n = envisionToClangMap_.closestParentWithAstMapping(node))
		if (envisionToClangMap_.contains(n))
			for (auto range : envisionToClangMap_.get(n))
				if (auto exp = expansion(range.getBegin()))
					result.insert(exp);

	expansionCache_.insert(node, result);
	return result;
}


QVector<Model::Node*> MacroExpansions::topLevelNodes(MacroExpansion* expansion, NodeOriginType filter)
{
	Q_ASSERT(expansion);

	QVector<Model::Node*> allTLExpansionNodes;
	for (auto node : envisionToClangMap_.nodes())
		for (auto range : envisionToClangMap_.get(node))
		{
			bool isCandidate = filter == Transitive ?
						// return true if the node ultimately expands to the source location of the expansion either by
						// being directly generated by expansion or by one of its child expansions.
						clang_.sourceManager()->getExpansionLoc(range.getBegin()) == expansion->range().getBegin() :
						// only return true if the node is actually generated in expansion.
						expansions(node).contains(expansion);

			if (isCandidate)
			{
				allTLExpansionNodes.append(node);
				break;
			}
		}

	QVector<Model::Node*> result = NodeHelpers::topLevelNodes(allTLExpansionNodes);

	std::sort(result.begin(), result.end(),
			[](Model::Node* e1, Model::Node* e2)
	{
		if (e1 == e2) return false;

		Model::Node* thisParent = nullptr;
		Model::Node* otherParent = nullptr;
		auto commonAncestor = e1->lowestCommonAncestor(e2, &thisParent, &otherParent);
		Q_ASSERT(commonAncestor);

		if (auto list = DCast<Model::List>(commonAncestor))
			return list->indexOf(thisParent) < list->indexOf(otherParent);
		else if (auto common = DCast<Model::CompositeNode>(commonAncestor))
		{
			auto index1 = common->indexOf(thisParent);
			auto index2 = common->indexOf(otherParent);
			Q_ASSERT(index1.isValid() && index2.isValid());
			return index1.level() < index2.level() ||
					(index1.level() == index2.level() && index1.index() < index2.index());
		}
		else
			Q_ASSERT(false);
	});

	return result;
}

}
