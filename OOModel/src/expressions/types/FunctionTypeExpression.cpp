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

#include "ModelBase/src/nodes/TypedListDefinition.h"
template class Model::TypedList<OOModel::FunctionTypeExpression>;

namespace OOModel {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(FunctionTypeExpression)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(FunctionTypeExpression)

REGISTER_ATTRIBUTE(FunctionTypeExpression, arguments, TypedListOfExpression, false, false, true)
REGISTER_ATTRIBUTE(FunctionTypeExpression, results, TypedListOfExpression, false, false, true)

FunctionTypeExpression::FunctionTypeExpression(const QList<Expression*>& args,
		const QList<Expression*>& res)
: Super(nullptr, FunctionTypeExpression::getMetaData())
{
	for (auto a : args)
	{
		Q_ASSERT(a);
		arguments()->append(a);
	}
	for (auto r : res)
	{
		Q_ASSERT(r);
		results()->append(r);
	}
}

Type* FunctionTypeExpression::type()
{
	// TODO: be a little more specific about what are allowed arguments and results.
	// E.g. return an error type if an argumnet is not a variable declaration and it is not
	// a type name.
	QList<const Type*> args;
	for (auto a : *arguments())
	{
		args.append(a->type());
		if (args.last()->isError())
		{
			for (auto t : args) SAFE_DELETE(t);
			return new ErrorType{"Invalid argument type for FunctionTypeExpression"};
		}
	}

	QList<const Type*> res;
	for (auto r : *results())
	{
		res.append(r->type());
		if (res.last()->isError())
		{
			for (auto t : args) SAFE_DELETE(t);
			for (auto t : res) SAFE_DELETE(t);
			return new ErrorType{"Invalid result type for FunctionTypeExpression"};
		}
	}

	return new FunctionType{false, args, res};
}

}
