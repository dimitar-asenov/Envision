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
#include "SuperExpression.h"
#include "../declarations/Class.h"
#include "../types/ClassType.h"
#include "../types/ErrorType.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
template class Model::TypedList<OOModel::SuperExpression>;

namespace OOModel {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(SuperExpression)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(SuperExpression)

Type* SuperExpression::type()
{
	auto p = parent();

	while (p)
	{
		auto cl = DCast<Class> (p);
		if (cl)
		{
			if (cl->baseClasses()->isEmpty())
			{
				if (auto base = cl->implicitBaseFromProject())
					return new ClassType{base, true};
				else
					return new ErrorType{"Invalid usage of 'super' expression. Class has no super class."};
			}
			else
			{
				if (auto base = Class::expressionToClass(cl->baseClasses()->first()))
					return new ClassType{base, true};
				else
					return new ErrorType{"Invalid usage of 'super' expression. Base class expression is incorrect."};
			}
		}
		p = p->parent();
	}

	return new ErrorType{"Invalid position for 'super' expression. Not within a class."};
}

}
