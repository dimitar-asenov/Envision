/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2012 ETH Zurich
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

/*
 * CompoundObjectDescriptor.cpp
 *
 *  Created on: Dec 3, 2012
 *      Author: Dimitar Asenov
 */

#include "CompoundObjectDescriptor.h"
#include "OOModel/src/expressions/IntegerLiteral.h"
#include "OOModel/src/expressions/EmptyExpression.h"
#include "ModelBase/src/model/Model.h"

namespace OOInteraction {

CompoundObjectDescriptor::CompoundObjectDescriptor(const QString& name, const QString& signature,
		int num_operands, int precedence, Associativity associativity)
		: OOOperatorDescriptor(name, signature, num_operands, precedence, associativity)
{}

OOModel::Expression* CompoundObjectDescriptor::create(const QList<OOModel::Expression*>& operands)
{
	Q_ASSERT(operands.size() == 1);
	auto ilit = dynamic_cast<OOModel::IntegerLiteral*> (operands.first());
	Q_ASSERT(ilit);
	auto e = storedExpressions().value(ilit->value());
	SAFE_DELETE(ilit);
	Q_ASSERT(e);

	if (auto model = e->model())
	{
		auto beingModified = model->isBeingModified();
		auto oldModificationTarget = beingModified ? nullptr : model->modificationTarget();
		auto oldParent = e->parent();

		Q_ASSERT(e!=oldModificationTarget && !e->isAncestorOf(oldModificationTarget));

		if (!beingModified) oldParent->beginModification("extract expression");
		else model->changeModificationTarget(oldParent);
		auto replaceSuccessfull = oldParent->replaceChild(e, new OOModel::EmptyExpression());
		Q_ASSERT(replaceSuccessfull);

		if (!beingModified) oldParent->endModification();
		else model->changeModificationTarget(oldModificationTarget);
	}
	else
	{
		auto replaceSuccessfull = e->parent()->replaceChild(e, new OOModel::EmptyExpression());
		Q_ASSERT(replaceSuccessfull);
	}

	return e;
}

QMap<int,OOModel::Expression*>& CompoundObjectDescriptor::storedExpressions()
{
	static QMap<int,OOModel::Expression*> map;
	return map;
}

const QString& CompoundObjectDescriptor::compoundSignature()
{
	static const QString sig{"@@ expr @@"};
	return sig;
}

int CompoundObjectDescriptor::nextId()
{
	static int id = 0;
	return ++id;
}

const QString CompoundObjectDescriptor::storeExpression(OOModel::Expression* object)
{
	int id = nextId();
	storedExpressions().insert(id,object);

	auto str = compoundSignature();
	str.replace(" expr ", QString::number(id));

	return str;
}

void CompoundObjectDescriptor::cleanAllStoredExpressions()
{
	// We do not need to delete the original expressions. This is handled by the Undo system.
	storedExpressions().clear();
}

} /* namespace OOVisualization */
