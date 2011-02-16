/***********************************************************************************************************************
 * BinaryOperation.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef BINARYOPERATION_H_
#define BINARYOPERATION_H_

#include "Expression.h"

#include "ModelBase/headers/nodes/Integer.h"

namespace OOModel {

class OOMODEL_API BinaryOperation: public Expression
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(BinaryOperation)

	ATTRIBUTE(Expression, left, setLeft)
	ATTRIBUTE(Expression, right, setRight)
	PRIVATE_ATTRIBUTE_VALUE(Model::Integer, opr, setOpr, int);

	public:
		enum OperatorTypes { TIMES, DIVIDE, REMAINDER, PLUS, MINUS, LEFT_SHIFT, RIGHT_SHIFT_SIGNED, RIGHT_SHIFT_UNSIGNED, LESS, GREATER,
			LESS_EQUALS, GREATER_EQUALS, EQUALS, NOT_EQUALS, XOR, AND, OR, CONDITIONAL_AND, CONDITIONAL_OR, ARRAY_INDEX };

		OperatorTypes op() const;
		void setOp(const OperatorTypes& oper);
};

inline BinaryOperation::OperatorTypes BinaryOperation::op() const { return static_cast<OperatorTypes> (opr()); }
inline void BinaryOperation::setOp(const OperatorTypes& oper) { setOpr(oper); }

}

#endif /* BINARYOPERATION_H_ */
