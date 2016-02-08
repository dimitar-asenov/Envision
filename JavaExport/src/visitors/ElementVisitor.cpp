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

#include "ElementVisitor.h"
#include "DeclarationVisitor.h"
#include "ExpressionVisitor.h"
#include "StatementVisitor.h"

#include "OOModel/src/elements/FormalArgument.h"
#include "OOModel/src/elements/FormalResult.h"
#include "OOModel/src/elements/FormalTypeArgument.h"
#include "OOModel/src/elements/CatchClause.h"
#include "OOModel/src/elements/Enumerator.h"
#include "OOModel/src/elements/MemberInitializer.h"
#include "OOModel/src/expressions/types/PointerTypeExpression.h"
#include "OOModel/src/expressions/types/FunctionTypeExpression.h"

using namespace Export;
using namespace OOModel;

namespace JavaExport {

SourceFragment* ElementVisitor::visit(FormalArgument* argument)
{
	auto fragment = new CompositeFragment{argument};
	*fragment << expression(argument->typeExpression()) << " ";
	*fragment << argument->nameNode();
	return fragment;
}

SourceFragment* ElementVisitor::visit(FormalResult* result)
{
	auto fragment = new CompositeFragment{result};
	*fragment << "RESULT";
	return fragment;
}

SourceFragment* ElementVisitor::visit(FormalTypeArgument* typeArgument)
{
	auto fragment = new CompositeFragment{typeArgument};
	*fragment << typeArgument->nameNode();
	if (typeArgument->subTypeOfExpression())
		*fragment << " extends " << expression(typeArgument->subTypeOfExpression());
	if (typeArgument->superTypeOfExpression())
		*fragment << " super " << expression(typeArgument->superTypeOfExpression());

	notAllowed(typeArgument->specializationExpression());

	return fragment;
}

SourceFragment* ElementVisitor::visit(CatchClause* catchClause)
{
	auto fragment = new CompositeFragment{catchClause};

	required(catchClause, catchClause->exceptionToCatch(), "Exception type to catch");

	*fragment << "catch (";
	if (catchClause->exceptionToCatch()) *fragment << expression(catchClause->exceptionToCatch());
	*fragment << ")";
	*fragment << list(catchClause->body(), StatementVisitor{data()}, "body");

	return fragment;
}

SourceFragment* ElementVisitor::visit(Enumerator* enumerator)
{
	auto fragment = new CompositeFragment{enumerator};
	*fragment << enumerator->name();
	if (auto value = enumerator->value())
	{
		*fragment << " = " << expression(value);
	}
	return fragment;
}

SourceFragment* ElementVisitor::visit(MemberInitializer* memberInitializer)
{
	notAllowed(memberInitializer);
	return new TextFragment{memberInitializer};
}

} // namespace JavaExport
