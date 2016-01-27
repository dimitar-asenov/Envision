/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "Declaration.h"

#include "ModelBase/src/nodes/TypedList.hpp"

template class Model::TypedList<OOModel::Declaration>;

namespace OOModel {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(Declaration)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(Declaration)

REGISTER_OONAME_SYMBOL_ATTRIBUTE(Declaration, UNSPECIFIED)
REGISTER_ATTRIBUTE(Declaration, modifiers, Modifier, false, false, true)
REGISTER_ATTRIBUTE(Declaration, annotations, StatementItemList, false, false, true)
REGISTER_ATTRIBUTE(Declaration, subDeclarations, TypedListOfDeclaration, false, false, true)
REGISTER_ATTRIBUTE(Declaration, metaCalls, TypedListOfExpression, false, false, true)

Declaration::Declaration(const QString& name)
: Super(nullptr, Declaration::getMetaData())
{
	setName(name);
}

Declaration::Declaration(const QString& name, Modifier::Modifiers modifiers)
: Super(nullptr, Declaration::getMetaData())
{
	setName(name);
	if (modifiers) this->modifiers()->set(modifiers);
}

}
