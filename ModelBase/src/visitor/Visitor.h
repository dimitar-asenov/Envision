/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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
 * Visitor.h
 *
 *  Created on: Aug 27, 2012
 *      Author: Dimitar Asenov
 */

#ifndef ModelBase_VISITOR_H_
#define ModelBase_VISITOR_H_

#include "../modelbase_api.h"

namespace Model {

class Node;


// VISITOR /////////////////////////////////////////////////////////////////////////////////////////


template <class ConcreteVisitor, class Result = void>
class Visitor {
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


// EXTENDED VISITOR ////////////////////////////////////////////////////////////////////////////////


template <class ConcreteVisitor, class BaseVisitor>
class ExtendedVisitor : public BaseVisitor {
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

} /* namespace Model */
#endif /* ModelBase_VISITOR_H_ */
