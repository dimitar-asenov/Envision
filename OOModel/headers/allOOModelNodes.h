/***********************************************************************************************************************
 * allOOModelNodes.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef ALLOOMODELNODES_H_
#define ALLOOMODELNODES_H_

#include "top_level/Project.h"
#include "top_level/Library.h"
#include "top_level/Module.h"
#include "top_level/Class.h"
#include "top_level/Method.h"
#include "top_level/Field.h"

#include "statements/Statement.h"
#include "statements/Block.h"
#include "statements/BreakStatement.h"
#include "statements/ContinueStatement.h"
#include "statements/ForEachStatement.h"
#include "statements/IfStatement.h"
#include "statements/LoopStatement.h"
#include "statements/ReturnStatement.h"
#include "statements/SwitchCase.h"
#include "statements/SwitchStatement.h"
#include "statements/VariableDeclaration.h"
#include "statements/MethodCallStatement.h"
#include "statements/AssignmentStatement.h"

#include "expressions/Expression.h"
#include "expressions/IntegerLiteral.h"
#include "expressions/FloatLiteral.h"
#include "expressions/StringLiteral.h"
#include "expressions/BooleanLiteral.h"
#include "expressions/CharacterLiteral.h"
#include "expressions/NullLiteral.h"
#include "expressions/ThisExpression.h"
#include "expressions/VariableAccess.h"
#include "expressions/NewExpression.h"
#include "expressions/MethodCallExpression.h"
#include "expressions/UnaryOperation.h"
#include "expressions/BinaryOperation.h"
#include "expressions/CastExpression.h"
#include "expressions/ReferenceExpression.h"
#include "expressions/ArrayInitializer.h"

#include "types/Type.h"
#include "types/PrimitiveType.h"
#include "types/NamedType.h"
#include "types/ArrayType.h"

#include "elements/Visibility.h"
#include "elements/Static.h"
#include "elements/StatementItem.h"
#include "elements/FormalArgument.h"
#include "elements/FormalResult.h"

#endif /* ALLOOMODELNODES_H_ */
