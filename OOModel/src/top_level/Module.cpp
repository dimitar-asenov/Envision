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
 * Module.cpp
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "top_level/Module.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(Module, Model::ExtendableNode)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(Module, Model::ExtendableNode)

REGISTER_ATTRIBUTE(Module, name, Text, false, false, true)
REGISTER_ATTRIBUTE(Module, modules, TypedListOfModule, false, false, true)
REGISTER_ATTRIBUTE(Module, classes, TypedListOfClass, false, false, true)

bool Module::definesSymbol() const
{
	return true;
}

const QString& Module::symbolName() const
{
	return name();
}

Model::Node* Module::navigateTo(Model::Node* source, QString path)
{
	QString symbol = extractFrontSymbol(path);
	Model::Node* found = nullptr;

	// Is the target symbol name the module's name
	if (isAncestorOf(source) && symbol == symbolName()) found = this;

	if (!found) found = modules()->findFirstSymbolDefinition(symbol);
	if (!found) found = classes()->findFirstSymbolDefinition(symbol);
	if (!found) return ExtendableNode::navigateTo(source, path);

	QString rest = extractSecondaryPath(path);
	if (!rest.isEmpty()) return found->navigateTo(this, rest);
	else return found;
}

}
