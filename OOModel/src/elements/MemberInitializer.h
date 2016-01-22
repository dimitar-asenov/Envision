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

#pragma once

#include "../oomodel_api.h"

#include "../expressions/Expression.h"
#include "../expressions/ReferenceExpression.h"

namespace OOModel { class MemberInitializer; }
extern template class OOMODEL_API Model::TypedList<OOModel::MemberInitializer>;

namespace OOModel {
/**
 *	This class represent various forms of member initializers.
 * It may be a call to a super constructor then \a memberReference will denote
 * the callee of the super constructor with \a arguemnts.
 * In case of delegating constructors the \a memberRef shuold be a reference to owner class,
 * as the \a arguments will contain the arguments.
 * For simple member field initializers the \a memberReference will contain a reference to the field
 * and \a arguments are the constructor arguments
 */
class OOMODEL_API MemberInitializer : public Super<Model::CompositeNode>
{
	COMPOSITENODE_DECLARE_STANDARD_METHODS(MemberInitializer)

	ATTRIBUTE(Model::TypedList<Expression>, arguments, setArguments)
	ATTRIBUTE(Expression, memberReference, setMemberReference)

	public:
		MemberInitializer(Expression* memberRef, QList<Expression*> arguments);
};

}
