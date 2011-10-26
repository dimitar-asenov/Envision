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
 * Method.cpp
 *
 *  Created on: Jan 28, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "top_level/Method.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(Method, Model::ExtendableNode)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(Method, Model::ExtendableNode)

REGISTER_ATTRIBUTE(Method, name, Text, false, false, true)
REGISTER_ATTRIBUTE(Method, items, StatementItemList, false, false, true)
REGISTER_ATTRIBUTE(Method, arguments, TypedListOfFormalArgument, false, false, true)
REGISTER_ATTRIBUTE(Method, results, TypedListOfFormalResult, false, false, true)
REGISTER_ATTRIBUTE(Method, visibility, Visibility, false, false, true)
REGISTER_ATTRIBUTE(Method, storageSpecifier, StorageSpecifier, false, false, true)

bool Method::definesSymbol() const
{
	return true;
}

const QString& Method::symbolName() const
{
	return name();
}

Model::Node* Method::navigateTo(Model::Node* source, QString path)
{
	if (isAncestorOf(source))
	{
		QString symbol = extractFrontSymbol(path);
		Model::Node* found = NULL;

		// Is the target symbol name the method's name
		if (symbol == symbolName()) found = this;

		if ( !found && source == items() )
		{
			if (!found) found = arguments()->findFirstSymbolDefinition(symbol);
			if (!found) found = results()->findFirstSymbolDefinition(symbol);
		}

		if (!found) return ExtendableNode::navigateTo(source, path);

		QString rest = extractSecondaryPath(path);
		if (!rest.isEmpty()) return found->navigateTo(this, rest);
		else return found;
	}
	else return NULL;
}

}
