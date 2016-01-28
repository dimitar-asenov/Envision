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

#include "MethodCallExpression.h"
#include "ReferenceExpression.h"
#include "../declarations/Class.h"
#include "../declarations/Method.h"
#include "../types/PrimitiveType.h"
#include "../types/SymbolProviderType.h"
#include "../types/FunctionType.h"
#include "../types/ErrorType.h"

#include "ModelBase/src/nodes/TypedList.hpp"
template class Model::TypedList<OOModel::MethodCallExpression>;

namespace OOModel {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(MethodCallExpression)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(MethodCallExpression)

REGISTER_ATTRIBUTE(MethodCallExpression, callee, Expression, false, false, true)
REGISTER_ATTRIBUTE(MethodCallExpression, arguments, TypedListOfExpression, false, false, true)
REGISTER_ATTRIBUTE(MethodCallExpression, cKind, Integer, false, false, true)

MethodCallExpression::MethodCallExpression(const QString& name, Expression* referencePrefix)
: Super(nullptr, MethodCallExpression::getMetaData())
{
	setCallee(new ReferenceExpression{name, referencePrefix});
	setMethodCallKind(MethodCallKind::Call);
}

Method* MethodCallExpression::methodDefinition(std::unique_ptr<Type>& calleeType)
{
	Method* ret = nullptr;

	// TODO: handle other cases as well (e.g. FunctionType)
	calleeType = callee()->type();
	if (auto spt = dynamic_cast<SymbolProviderType*>(calleeType.get()))
	{
		if (auto m = DCast<Method>(spt->symbolProvider()))
			ret = m;
		else if (/*auto c =*/ DCast<Class>(spt->symbolProvider()))
			{/* TODO: Find the method that implements the () overload and return that*/}
	}

	return ret;
}

Method* MethodCallExpression::methodDefinition()
{
	std::unique_ptr<Type> dummy{};
	auto ret = methodDefinition(dummy);
	return ret;
}

std::unique_ptr<Type> MethodCallExpression::type()
{
	std::unique_ptr<Type> calleeType = nullptr;
	auto mdef = methodDefinition(calleeType);

	if (!mdef)
	{
		// This type does not point to a method. See if it points to a FunctionalType object
		std::unique_ptr<Type> ret;
		if (auto ft = dynamic_cast<FunctionType*>(calleeType.get()))
		{
			// TODO: handle multiple return values
			if (ft->results().empty()) ret = std::unique_ptr<Type>{new PrimitiveType{PrimitiveType::VOID, true}};
			else ret = std::unique_ptr<Type>{ft->results().front()->clone()};
		}

		if (ret) return ret;
		else return std::unique_ptr<Type>{new ErrorType{"Unresolved reference to a method"}};
	}


	if (mdef->results()->size() == 0)
		return std::unique_ptr<Type>{new PrimitiveType{PrimitiveType::VOID, true}};
	else
	{
		// TODO: handle multiple return values
		auto t = mdef->results()->first()->typeExpression()->type();
		t->setValueType(true);
		return t;
	}
}

}
