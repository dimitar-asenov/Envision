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

#include "../modelbase_api.h"

#if defined(ModelBase_VisitorDefinition)
	#include "../nodes/Node.h"
	#include "../ModelException.h"
	#define VISITOR_EXPORT Q_DECL_EXPORT
#else
	#define VISITOR_EXPORT
	namespace Model { class Node; }
#endif

namespace Model {

/* *********************************************************************************************************************
 * Visitor declaration
 **********************************************************************************************************************/
template <class ConcreteVisitor, class Result = void>
class VISITOR_EXPORT Visitor {
public:
	virtual ~Visitor();

	typedef Result VisitorResultType;
	typedef ConcreteVisitor BaseVisitorType;
	typedef std::function<Result (ConcreteVisitor*, Node*)> VisitFunctionInstance;

	template <class NodeType>
	static void addType(std::function<Result (ConcreteVisitor*, NodeType*)> f)
	{
		if (types().size() <= NodeType::typeIdStatic()) types().resize(NodeType::typeIdStatic()+1);
		types()[NodeType::typeIdStatic()] = [=](ConcreteVisitor* vis, Node* x) -> Result
		{ return f(vis, static_cast<NodeType*>(x)); };
	}

	Result visit(Node* n);
	virtual Result visitChildren(Node* n);

protected:
	virtual VisitFunctionInstance findFunctionForId(int id);

private:
	static QVector<VisitFunctionInstance>& types();
};


/* *********************************************************************************************************************
 * Extended Visitor declaration
 **********************************************************************************************************************/
template <class ConcreteVisitor, class BaseVisitor>
class VISITOR_EXPORT ExtendedVisitor : public BaseVisitor {
	public:
		template <class NodeType>
		static void addType(std::function<typename BaseVisitor::VisitorResultType (ConcreteVisitor*, NodeType*)> f)
		{
			if (types().size() <= NodeType::typeIdStatic()) types().resize(NodeType::typeIdStatic()+1);
			types()[NodeType::typeIdStatic()] = [=](typename BaseVisitor::BaseVisitorType* vis, Node* x)
					-> typename BaseVisitor::VisitorResultType
			{ return f(static_cast<ConcreteVisitor*>(vis), static_cast<NodeType*>(x)); };
		}

	protected:
		virtual typename BaseVisitor::VisitFunctionInstance findFunctionForId(int id) override;

	private:
		static QVector<typename BaseVisitor::VisitFunctionInstance>& types();
};


#if defined(ModelBase_VisitorDefinition)
/* *********************************************************************************************************************
 * Visitor implementation
 **********************************************************************************************************************/
template <class ConcreteVisitor, class Result>
Visitor<ConcreteVisitor, Result>::~Visitor()
{}

template <class ConcreteVisitor, class Result>
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
template <class ConcreteVisitor, class Result>
Result Visitor<ConcreteVisitor, Result>::visitChildren(Node* n)
{
	auto children = n->children();
	auto it = children.begin();
	if (!children.isEmpty())
	{
		while (true)
		{
			if (it+1 == children.end()) return visit(*it);
			else visit(*it);
			++it;
		}
	}

	// Return default value otherwise.
	return Result ();
}

template <class ConcreteVisitor, class Result>
typename Visitor<ConcreteVisitor, Result>::VisitFunctionInstance
	Visitor<ConcreteVisitor, Result>::findFunctionForId(int id)
{
	if (types().size() > id && types()[id])
		return types()[id];
	else
		return nullptr;
}


template <class ConcreteVisitor, class Result>
QVector<typename Visitor<ConcreteVisitor, Result>::VisitFunctionInstance>& Visitor<ConcreteVisitor, Result>::types()
{
	static QVector<VisitFunctionInstance> t;
	return t;
}


/* *********************************************************************************************************************
 * Extended Visitor implementation
 **********************************************************************************************************************/
template <class ConcreteVisitor, class BaseVisitor>
typename BaseVisitor::VisitFunctionInstance
	ExtendedVisitor<ConcreteVisitor, BaseVisitor>::findFunctionForId(int id)
{
	if (types().size() > id && types()[id])
		return types()[id];
	else
		return BaseVisitor::findFunctionForId(id);
}

template <class ConcreteVisitor, class BaseVisitor>
QVector<typename BaseVisitor::VisitFunctionInstance>&
	ExtendedVisitor<ConcreteVisitor, BaseVisitor>::types()
{
	static QVector<typename BaseVisitor::VisitFunctionInstance> t;
	return t;
}
#endif

} /* namespace Model */
