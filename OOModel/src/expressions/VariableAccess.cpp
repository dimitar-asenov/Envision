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
 * VariableAccess.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VariableAccess.h"
#include "expressions/VariableDeclaration.h"
#include "top_level/Class.h"
#include "top_level/Field.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(VariableAccess, Expression)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(VariableAccess, Expression)

REGISTER_ATTRIBUTE(VariableAccess, ref, ReferenceExpression, false, false, true)

VariableAccess::VariableAccess(const QString& referenceString, Expression* prefix)
: Expression(nullptr, VariableAccess::getMetaData())
{
	ref()->ref()->setName(referenceString);
	if (prefix != nullptr) ref()->setPrefix(prefix);
}

Class* VariableAccess::classDefinition()
{
	Model::Node* var = ref()->target();

	Field* f = dynamic_cast<Field*> (var);
	if (f) return f->typeExpression()->classDefinition();

	VariableDeclaration* vd = dynamic_cast<VariableDeclaration*> (var);
	if (vd) return vd->varType()->classDefinition();

	return nullptr;
}

}
