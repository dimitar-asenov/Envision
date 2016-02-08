/***********************************************************************************************************************
 **
 ** Copyright (c) 2015 ETH Zurich
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

#include "NameResolver.h"

#include "SymbolMatcher.h"
#include "../model/TreeManager.h"
#include "../model/AllTreeManagers.h"

namespace Model {

QList<QPair<QString, Node*>> NameResolver::mostLikelyMatches(const QString& nodeName, int matchLimit,
																				 Node* root, IsSuggestable suggestable)
{
	QList<QPair<QString, Node*>> matches;
	auto parts = nodeName.split(".");
	QString pattern{"*"};
	for (auto part : parts) pattern += part + '*';
	auto matcher = SymbolMatcher{new QRegExp{pattern, Qt::CaseInsensitive, QRegExp::WildcardUnix}};

	if (root) matches.append(findAllMatches(matcher, "", root, suggestable));
	else
		for (TreeManager* manager : AllTreeManagers::instance().loadedManagers())
			matches.append(findAllMatches(matcher, "", manager->root(), suggestable));

	//Shorter names usually have less parts to the fully qualified name -> suggest them first
	std::sort(matches.begin(), matches.end(), [](QPair<QString, Node*> first, QPair<QString, Node*> second)
										{ return first.first.length() < second.first.length(); });
	//Limit the number of suggestions
	matches = matches.mid(0, matchLimit);
	return matches;
}

QList<QPair<QString, Node*>> NameResolver::findAllMatches(const SymbolMatcher& matcher, QString nameSoFar,
																			 Node* root, IsSuggestable suggestable)
{
	QList<QPair<QString, Node*>> result;

	//If it doesn't define a symbol, just pass it on
	if (!root->definesSymbol())
		for (auto child : root->children())
			result.append(findAllMatches(matcher, nameSoFar, child, suggestable));
	else if (suggestable(root->symbolType()) && root->symbolName().size() > 0)
	{
		//If it defines a symbol, check if the name matches with our SymbolMatcher

		auto newNameSoFar = nameSoFar + "." + root->symbolName();
		for (auto child : root->children())
			result.append(findAllMatches(matcher, newNameSoFar, child, suggestable));
		if (matcher.matches(newNameSoFar))
		{
			//Get rid of initial "."
			result.append(QPair<QString, Node*>(newNameSoFar.mid(1), root));
		}
	}
	return result;
}

bool NameResolver::isSuggestable(Node::SymbolTypes symbolType)
{
	return symbolType == Node::METHOD || symbolType == Node::CONTAINER;
}

}
