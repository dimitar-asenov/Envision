/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2016 ETH Zurich
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
#include "TypeArgumentBindings.h"
#include "../elements/FormalTypeArgument.h"
#include "../expressions/ReferenceExpression.h"
#include "../declarations/Class.h"
#include "../declarations/Method.h"
#include "../declarations/TypeAlias.h"
#include "../types/ErrorType.h"
#include "../types/Type.h"

namespace OOModel {

TypeArgumentBindings::TypeArgumentBindings(const TypeArgumentBindings& other)
{
	for (auto it = other.bindings_.cbegin(); it != other.bindings_.cend(); ++it)
		bindings_[it->first] = std::unique_ptr<OOModel::Type>{it->second->clone()};
}

TypeArgumentBindings::TypeArgumentBindings(const TypeArgumentBindings&& other)
	: bindings_{std::move(other.bindings_)}{}

void TypeArgumentBindings::insert(FormalTypeArgument* argument, std::unique_ptr<Type> type)
{
	bindings_[argument] = std::move(type);
}

std::unique_ptr<Type> TypeArgumentBindings::bindingFor(FormalTypeArgument* argument)
{
	auto it = bindings_.find(argument);
	if (it != bindings_.end())
		return std::unique_ptr<OOModel::Type>{it->second->clone()};

	// We do not have a specified binding for this argument, use a default one if present
	if (argument->defaultType())
		return argument->defaultType()->type();

	// Otherwise we can't resolve the type
	return std::unique_ptr<Type>{new ErrorType{"Unbound type argument"}};
}

void TypeArgumentBindings::insertFromReference(const ReferenceExpression* ref, Model::Node* genericNode)
{
	auto addTypeArguments = [this, ref](auto container) {
		int i = 0;
		for (auto typeArg : *container->typeArguments())
		{
			if (i < ref->typeArguments()->size())
			{
				insert(typeArg, ref->typeArguments()->at(i)->type(*this));
				++i;
			}
		}

	};

	if (auto aClass = DCast<Class>(genericNode))
		addTypeArguments(aClass, ref);
	else if (auto aMethod = DCast<Method>(genericNode))
		addTypeArguments(aMethod, ref);
	else if (auto aTypeAlias = DCast<TypeAlias>(genericNode))
		addTypeArguments(aTypeAlias, ref);
}

}
