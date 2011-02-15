/***********************************************************************************************************************
 * allOOVisualizations.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef ALLOOVISUALIZATIONS_H_
#define ALLOOVISUALIZATIONS_H_

#include "vis/top_level/VProject.h"
#include "vis/top_level/VLibrary.h"
#include "vis/top_level/VModule.h"
#include "vis/top_level/VClass.h"
#include "vis/top_level/VMethod.h"
#include "vis/elements/VField.h"
#include "vis/elements/VFormalArgument.h"
#include "vis/elements/VFormalResult.h"

#include "icons/ClassIcon.h"
#include "icons/MethodIcon.h"

#include "expressions/VVariableAccess.h"
#include "expressions/VMethodCallExpression.h"
#include "expressions/VReferenceExpression.h"
#include "expressions/VThisExpression.h"
#include "expressions/VCastExpression.h"
#include "expressions/VNewExpression.h"
#include "expressions/VUnaryOperation.h"
#include "expressions/VBinaryOperation.h"

#include "literals/VStringLiteral.h"
#include "literals/VIntegerLiteral.h"
#include "literals/VFloatLiteral.h"
#include "literals/VCharacterLiteral.h"
#include "literals/VBooleanLiteral.h"
#include "literals/VNullLiteral.h"

#include "statements/VMethodCallStatement.h"
#include "statements/VVariableDeclaration.h"
#include "statements/VAssignmentStatement.h"
#include "statements/VBlock.h"

#include "types/VNamedType.h"
#include "types/VPrimitiveType.h"

#endif /* ALLOOVISUALIZATIONS_H_ */
