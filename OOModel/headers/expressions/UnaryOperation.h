/***********************************************************************************************************************
 * UnaryOperation.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef UNARYOPERATION_H_
#define UNARYOPERATION_H_

#include "Expression.h"

#include "ModelBase/headers/nodes/Integer.h"

namespace OOModel {

class OOMODEL_API UnaryOperation: public Expression
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(UnaryOperation)

	ATTRIBUTE(Expression, operand, setOperand)
	PRIVATE_ATTRIBUTE_VALUE(Model::Integer, opr, setOpr, int);

	public:
		enum OperatorTypes {INCREMENT, DECREMENT, PLUS, MINUS, NOT, COMPLEMENT};

		OperatorTypes op() const;
		void setOp(const OperatorTypes& oper);
};

inline UnaryOperation::OperatorTypes UnaryOperation::op() const { return static_cast<OperatorTypes> (opr()); }
inline void UnaryOperation::setOp(const OperatorTypes& oper) { setOpr(oper); }

}

#endif /* UNARYOPERATION_H_ */
