/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#include "Field.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
DEFINE_TYPED_LIST(OOModel::Field)

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(Field)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(Field)

Field::Field(const QString& name, Expression* type)
: Super(nullptr, Field::getMetaData())
{
	setName(name);
	if (type) setTypeExpression(type);
}

Field::Field(const QString& name, Expression* type, Expression* initialValue)
: Super(nullptr, Field::getMetaData())
{
	setName(name);
	if (type) setTypeExpression(type);
	if (initialValue) setInitialValue(initialValue);
}

Field::Field(const QString& name, Expression* type, Visibility::VisibilityType vis)
: Super(nullptr, Field::getMetaData())
{
	setName(name);
	if (type) setTypeExpression(type);
	setVisibility(vis);
}

Field::Field
		(const QString& name, Expression* type, StorageSpecifier::StorageSpecifierTypes storage)
: Super(nullptr, Field::getMetaData())
{
	setName(name);
	if (type) setTypeExpression(type);
	setStorageSpecifier(storage);
}

Field::Field(const QString& name, Expression* type, Visibility::VisibilityType vis,
				 StorageSpecifier::StorageSpecifierTypes storage, Expression* initialValue)
: Super(nullptr, Field::getMetaData())
{
	setName(name);
	if (type) setTypeExpression(type);
	setVisibility(vis);
	setStorageSpecifier(storage);
	if (initialValue) setInitialValue(initialValue);
}

}
