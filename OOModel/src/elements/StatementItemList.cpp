/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

/***********************************************************************************************************************
 * StatementItemList.cpp
 *
 *  Created on: Mar 23, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "elements/StatementItemList.h"

namespace OOModel {

NODE_DEFINE_EMPTY_CONSTRUCTORS(StatementItemList, Model::TypedList<StatementItem> )
NODE_DEFINE_TYPE_REGISTRATION_METHODS(StatementItemList)

Model::Node* StatementItemList::navigateTo(Model::Node* source, QString path)
{
	if (isAncestorOf(source))
	{
		QString symbol = extractFrontSymbol(path);
		Node* found = nullptr;

		// We should look for a statement above the source that declares a symbol.
		for(int i = indexOf(source); i >= 0; --i)
		{
			if ( at(i)->definesSymbol() && at(i)->symbolName() == symbol)
			{
				found = at(i);
				break;
			}
		}

		if (!found ) return Model::TypedList<StatementItem>::navigateTo(source, path);

		QString rest = extractSecondaryPath(path);
		if (!rest.isEmpty()) return found->navigateTo(this, rest);
		else return found;
	}
	else return nullptr;

}

QList<Model::Node*> StatementItemList::findSymbol(const QString& symbol, Model::Node* source, FindSymbolMode mode)
{
	QList<Model::Node*> symbols = findAllSymbolDefinitions(symbol, indexOfSubitem(source));

	return symbols.isEmpty() ? Node::findSymbol(symbol, source, mode) : symbols;
}

}
