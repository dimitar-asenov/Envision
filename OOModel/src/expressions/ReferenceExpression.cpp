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

#include "ReferenceExpression.h"
#include "VariableDeclarationExpression.h"
#include "../statements/ExpressionStatement.h"
#include "../declarations/Project.h"
#include "../declarations/Module.h"
#include "../declarations/Class.h"
#include "../declarations/Field.h"
#include "../declarations/TypeAlias.h"
#include "../declarations/NameImport.h"
#include "../declarations/Field.h"
#include "../declarations/MetaDefinition.h"
#include "../types/SymbolProviderType.h"
#include "../types/ClassType.h"
#include "../types/ErrorType.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
DEFINE_TYPED_LIST(OOModel::ReferenceExpression)

namespace OOModel {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(ReferenceExpression)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(ReferenceExpression)

REGISTER_ATTRIBUTE(ReferenceExpression, prefix, Expression, false, true, true)
REGISTER_ATTRIBUTE(ReferenceExpression, ref, OOReference, false, false, true)
REGISTER_ATTRIBUTE(ReferenceExpression, typeArguments, TypedListOfExpression, false, false, true)
REGISTER_ATTRIBUTE(ReferenceExpression, memKind, Integer, false, false, true)

ReferenceExpression::ReferenceExpression(const QString& name, Expression* prefix, MemberKind kind)
: Super(nullptr, ReferenceExpression::getMetaData())
{
	ref()->setName(name);
	setMemberKind(kind);
	if (prefix != nullptr) setPrefix(prefix);
}

Type* ReferenceExpression::type()
{
	auto resolvedTarget = ref()->target();

	if (!resolvedTarget) return new ErrorType{"Unresolved Reference"};

	if ( auto project = DCast<Project>( resolvedTarget ) )
		return new SymbolProviderType{project, false};
	else if ( auto module = DCast<Module>( resolvedTarget ) )
		return new SymbolProviderType{module, false};
	else if ( auto cl = DCast<Class>( resolvedTarget ) )
		return new ClassType{cl, false};
	else if ( auto method = DCast<Method>( resolvedTarget ) )
		return new SymbolProviderType{method, false};
	else if ( auto metaDefinition = DCast<MetaDefinition> (resolvedTarget ))
		return new SymbolProviderType{metaDefinition, false};
	else if ( auto alias = DCast<TypeAlias>( resolvedTarget ) )
		return new SymbolProviderType{alias, false};
	else if ( auto vdecl = DCast<VariableDeclarationExpression>( resolvedTarget ) )
	{
		auto t = vdecl->type();
		t->setValueType(true);
		return t;
	}
	else if ( auto vdecl = DCast<VariableDeclaration>( resolvedTarget ) )
	{
		auto t = vdecl->typeExpression()->type();
		t->setValueType(true);
		return t;
	}
	else if ( auto expSt = DCast<ExpressionStatement>( resolvedTarget ))
	{
		if ( auto vdecl = DCast<VariableDeclarationExpression>( expSt->expression() ) )
		{
			auto t = vdecl->type();
			t->setValueType(true);
			return t;
		}
		else return new ErrorType{"Unknown type for target of reference"};
	}
	else if ( auto field = DCast<Field>( resolvedTarget ) )
	{
		auto t = field->typeExpression()->type();
		t->setValueType(true);
		return t;
	}
	else if ( auto arg = DCast<FormalArgument>( resolvedTarget ) )
	{
		auto t = arg->typeExpression()->type();
		t->setValueType(true);
		return t;
	}
	else if ( auto res = DCast<FormalResult>( resolvedTarget ) )
	{
		auto t = res->typeExpression()->type();
		t->setValueType(true);
		return t;
	}
	else return new ErrorType{"Unknown type for target of reference"};
}

}
