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

#include "ClassTypeExpression.h"
#include "../../types/ClassType.h"
#include "../../types/ErrorType.h"
#include "../../declarations/Class.h"

#include "ModelBase/src/nodes/TypedList.hpp"
template class Model::TypedList<OOModel::ClassTypeExpression>;

namespace OOModel {

DEFINE_COMPOSITE_EMPTY_CONSTRUCTORS(ClassTypeExpression)
DEFINE_COMPOSITE_TYPE_REGISTRATION_METHODS(ClassTypeExpression)

DEFINE_ATTRIBUTE(ClassTypeExpression, typeExpression, ReferenceExpression, false, false, true)

ClassTypeExpression::ClassTypeExpression(ReferenceExpression* ref)
: Super{nullptr, ClassTypeExpression::getMetaData()}
{
	if (ref) setTypeExpression(ref);
}

std::unique_ptr<Type> ClassTypeExpression::type()
{
	auto cl = DCast<Class> (typeExpression()->target());
	if (cl) return std::unique_ptr<Type>{new ClassType{cl, false}};
	else return std::unique_ptr<Type>{new ErrorType{"Unresolved class reference in a class type expression"}};
}

}
