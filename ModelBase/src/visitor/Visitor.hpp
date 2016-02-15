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

#pragma once

#include "Visitor.h"
#include "../nodes/Node.h"
#include "../ModelException.h"

namespace Model {

template <typename ConcreteVisitor, typename Result>
Visitor<ConcreteVisitor, Result>::~Visitor()
{}

template <typename ConcreteVisitor, typename Result>
Result Visitor<ConcreteVisitor, Result>::visit(Node* n)
{
	for (auto id : n->hierarchyTypeIds())
	{
		auto f = findFunctionForId(id);
		if (f) return f(static_cast<ConcreteVisitor*>(this), n);
	}

	// No user specified function was found. Just visit all children.
	return visitChildren(n);
}
template <typename ConcreteVisitor, typename Result>
Result Visitor<ConcreteVisitor, Result>::visitChildren(Node* n)
{
	auto children = n->children();
	auto it = children.begin();
	if (!children.isEmpty())
		while (true)
		{
			if (it+1 == children.end()) return visit(*it);
			else visit(*it);
			++it;
		}

	// Return default value otherwise.
	return Result ();
}

template <typename ConcreteVisitor, typename Result>
typename Visitor<ConcreteVisitor, Result>::VisitFunctionInstance
	Visitor<ConcreteVisitor, Result>::findFunctionForId(int id)
{
	if (types().size() > id && types()[id])
		return types()[id];
	else
		return nullptr;
}


template <typename ConcreteVisitor, typename Result>
QVector<typename Visitor<ConcreteVisitor, Result>::VisitFunctionInstance>& Visitor<ConcreteVisitor, Result>::types()
{
	static QVector<VisitFunctionInstance> t;
	return t;
}

template <typename ConcreteVisitor, typename BaseVisitor>
typename BaseVisitor::VisitFunctionInstance
	ExtendedVisitor<ConcreteVisitor, BaseVisitor>::findFunctionForId(int id)
{
	if (types().size() > id && types()[id])
		return types()[id];
	else
		return BaseVisitor::findFunctionForId(id);
}

template <typename ConcreteVisitor, typename BaseVisitor>
QVector<typename BaseVisitor::VisitFunctionInstance>&
	ExtendedVisitor<ConcreteVisitor, BaseVisitor>::types()
{
	static QVector<typename BaseVisitor::VisitFunctionInstance> t;
	return t;
}

}
