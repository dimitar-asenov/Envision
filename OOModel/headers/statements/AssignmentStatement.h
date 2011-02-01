/***********************************************************************************************************************
 * AssignmentStatement.h
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef ASSIGNMENTSTATEMENT_H_
#define ASSIGNMENTSTATEMENT_H_

#include "Statement.h"
#include "../expressions/Expression.h"

#include "ModelBase/headers/nodes/Integer.h"

namespace OOModel {

class OOMODEL_API AssignmentStatement: public Statement
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(AssignmentStatement)

	ATTRIBUTE(Expression, left, setLeft)
	ATTRIBUTE(Expression, right, setRight)
	PRIVATE_ATTRIBUTE_VALUE(Model::Integer, opr, setOpr, int);

	public:
		enum AssignmentTypes { ASSIGN, PLUS_ASSIGN, MINUS_ASSIGN, TIMES_ASSIGN, DIVIDE_ASSIGN, BIT_AND_ASSIGN, BIT_OR_ASSIGN, BIT_XOR_ASSIGN, REMAINDER_ASSIGN,
		    LEFT_SHIFT_ASSIGN, RIGHT_SHIFT_SIGNED_ASSIGN, RIGHT_SHIFT_UNSIGNED_ASSIGN};

		AssignmentTypes op() const;
		void setOp(const AssignmentTypes& oper);
};

inline AssignmentStatement::AssignmentTypes AssignmentStatement::op() const { return static_cast<AssignmentTypes> (opr()); }
inline void AssignmentStatement::setOp(const AssignmentTypes& oper) { setOpr(oper); }

}

#endif /* ASSIGNMENTSTATEMENT_H_ */
