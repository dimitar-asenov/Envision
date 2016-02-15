/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
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

#include "DeclarationVisitor.h"
#include "ExpressionVisitor.h"
#include "StatementVisitor.h"
#include "ElementVisitor.h"
#include "CppPrintContext.h"

#include "OOModel/src/elements/FormalArgument.h"
#include "OOModel/src/elements/FormalMetaArgument.h"
#include "OOModel/src/elements/FormalResult.h"
#include "OOModel/src/elements/FormalTypeArgument.h"
#include "OOModel/src/elements/CatchClause.h"
#include "OOModel/src/elements/Enumerator.h"
#include "OOModel/src/elements/MemberInitializer.h"
#include "OOModel/src/expressions/types/PointerTypeExpression.h"
#include "OOModel/src/expressions/types/FunctionTypeExpression.h"
#include "OOModel/src/expressions/types/ArrayTypeExpression.h"
#include "OOModel/src/declarations/Method.h"
#include "OOModel/src/declarations/Class.h"

#include "Comments/src/nodes/CommentNode.h"

using namespace Export;
using namespace OOModel;

namespace CppExport {

SourceFragment* ElementVisitor::visit(FormalArgument* argument)
{
	auto fragment = new CompositeFragment{argument};

	auto pointerTypeExpression = DCast<PointerTypeExpression>(argument->typeExpression());
	if (pointerTypeExpression && DCast<FunctionTypeExpression>(pointerTypeExpression->typeExpression()))
		*fragment << ExpressionVisitor{data()}.visitFunctionPointer(pointerTypeExpression, argument->name());
	else
	{
		if (auto arrayTypeExpression = DCast<ArrayTypeExpression>(argument->typeExpression()))
			*fragment << expression(arrayTypeExpression->typeExpression());
		else
			*fragment << expression(argument->typeExpression());

		if (printContext().isClass() || argument->isUsedInParentMethod() ||
			 argument->firstAncestorOfType<OOModel::MetaDefinition>())
			*fragment << " " << argument->nameNode();

		if (DCast<ArrayTypeExpression>(argument->typeExpression()))
			*fragment << "[]";
	}

	if (printContext().hasOption(CppPrintContext::PrintDefaultArgumentValues) && argument->initialValue())
		*fragment << " = " << ExpressionVisitor{data()}.visit(argument->initialValue());

	return fragment;
}

SourceFragment* ElementVisitor::visit(FormalMetaArgument* argument)
{
	auto fragment = new CompositeFragment{argument};
	*fragment << argument->nameNode();
	return fragment;
}

SourceFragment* ElementVisitor::visit(FormalResult* result)
{
	auto fragment = new CompositeFragment{result};
	*fragment << expression(result->typeExpression());
	return fragment;
}

SourceFragment* ElementVisitor::visit(FormalTypeArgument* typeArgument)
{
	auto fragment = new CompositeFragment{typeArgument};
	*fragment << "typename " << typeArgument->nameNode();
	if (printContext().isClass() && typeArgument->defaultType())
		*fragment << " = " << expression(typeArgument->defaultType());
	return fragment;
}

SourceFragment* ElementVisitor::visit(CatchClause* catchClause)
{
	auto fragment = new CompositeFragment{catchClause};

	required(catchClause, catchClause->exceptionToCatch(), "Exception type to catch");

	*fragment << "catch (";
	if (catchClause->exceptionToCatch())
		*fragment << expression(catchClause->exceptionToCatch());
	else
		*fragment << "...";
	*fragment << ")";
	*fragment << list(catchClause->body(), StatementVisitor{data()}, "body");

	return fragment;
}

SourceFragment* ElementVisitor::visit(Enumerator* enumerator)
{
	auto fragment = new CompositeFragment{enumerator};

	auto commentNode = DCast<Comments::CommentNode>(enumerator->comment());
	if (commentNode && commentNode->lines()->size() > 1)
		*fragment << DeclarationVisitor::compositeNodeComments(enumerator, "declarationComment");

	*fragment << enumerator->name();
	if (auto value = enumerator->value())
		*fragment << " = " << expression(value);

	// TODO: do not print the comma here, use a list layout instead
	if (auto parentList = DCast<Model::List>(enumerator->parent()))
		if (parentList->last() != enumerator)
			*fragment << ",";

	if (commentNode && commentNode->lines()->size() == 1)
		*fragment << "/**< " + commentNode->lines()->first()->get() + "*/";

	return fragment;
}

SourceFragment* ElementVisitor::visit(MemberInitializer* memberInitializer)
{
	auto fragment = new CompositeFragment{memberInitializer};
	*fragment << expression(memberInitializer->memberReference())
				 << list(memberInitializer->arguments(), ExpressionVisitor{data()}, "initializerList");
	return fragment;
}

SourceFragment* ElementVisitor::visitTemplateArguments(Model::TypedList<FormalTypeArgument>* typeArguments)
{
	return list(typeArguments, this, "templateArgsList");
}

}
