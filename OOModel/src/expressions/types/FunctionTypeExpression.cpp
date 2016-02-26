/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

#include "FunctionTypeExpression.h"
#include "../../types/FunctionType.h"
#include "../../types/ErrorType.h"

#include "ModelBase/src/nodes/TypedList.hpp"
template class Model::TypedList<OOModel::FunctionTypeExpression>;

namespace OOModel {

DEFINE_COMPOSITE_EMPTY_CONSTRUCTORS(FunctionTypeExpression)
DEFINE_COMPOSITE_TYPE_REGISTRATION_METHODS(FunctionTypeExpression)

DEFINE_ATTRIBUTE(FunctionTypeExpression, arguments, TypedListOfExpression, false, false, true)
DEFINE_ATTRIBUTE(FunctionTypeExpression, results, TypedListOfExpression, false, false, true)

FunctionTypeExpression::FunctionTypeExpression(const QList<Expression*>& arguments, const QList<Expression*>& results)
: Super{nullptr, FunctionTypeExpression::getMetaData()}
{
	for (auto argument : arguments)
	{
		Q_ASSERT(argument);
		this->arguments()->append(argument);
	}
	for (auto result : results)
	{
		Q_ASSERT(result);
		this->results()->append(result);
	}
}

std::unique_ptr<Type> FunctionTypeExpression::type()
{
	// TODO: be a little more specific about what are allowed arguments and results.
	// E.g. return an error type if an argumnet is not a variable declaration and it is not
	// a type name.
	std::vector<std::unique_ptr<Type>> args;
	for (auto a : *arguments())
	{
		args.push_back(a->type());
		if (args.back()->isError())
			return std::unique_ptr<Type>{new ErrorType{"Invalid argument type for FunctionTypeExpression"}};
	}

	std::vector<std::unique_ptr<Type>> res;
	for (auto r : *results())
	{
		res.push_back(r->type());
		if (res.back()->isError())
			return std::unique_ptr<Type>{new ErrorType{"Invalid result type for FunctionTypeExpression"}};
	}

	return std::unique_ptr<Type>{new FunctionType{false, std::move(args), std::move(res)}};
}

}
