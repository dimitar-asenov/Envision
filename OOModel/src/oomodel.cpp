/***********************************************************************************************************************
 * oomodel.cpp
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "oomodel.h"
#include "SelfTest/headers/SelfTestSuite.h"

#include "common/Visibility.h"

#include "Project.h"
#include "Module.h"
#include "Class.h"
#include "Method.h"
#include "MethodItem.h"
#include "MethodArgument.h"

#include "statements/Statement.h"

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
#include "expressions/MethodCall.h"
#include "expressions/UnaryOperation.h"
#include "expressions/BinaryOperation.h"

#include "types/Type.h"
#include "types/PrimitiveType.h"
#include "types/NamedType.h"

Q_EXPORT_PLUGIN2( oomodel, OOModel::OOModel )

namespace OOModel {

bool OOModel::initialize(Envision::EnvisionManager&)
{
	// Initialize common nodes
	Visibility::registerNodeType();

	// Register TypedList instantiations
	Model::TypedList<Project>::registerNodeType();
	Model::TypedList<Module>::registerNodeType();
	Model::TypedList<Class>::registerNodeType();
	Model::TypedList<Method>::registerNodeType();
	Model::TypedList<MethodItem>::registerNodeType();
	Model::TypedList<MethodArgument>::registerNodeType();

	Model::TypedList<Statement>::registerNodeType();

	Model::TypedList<Expression>::registerNodeType();
	Model::TypedList<IntegerLiteral>::registerNodeType();
	Model::TypedList<FloatLiteral>::registerNodeType();
	Model::TypedList<BooleanLiteral>::registerNodeType();
	Model::TypedList<StringLiteral>::registerNodeType();
	Model::TypedList<CharacterLiteral>::registerNodeType();
	Model::TypedList<NullLiteral>::registerNodeType();
	Model::TypedList<ThisExpression>::registerNodeType();
	Model::TypedList<VariableAccess>::registerNodeType();
	Model::TypedList<NewExpression>::registerNodeType();
	Model::TypedList<MethodCall>::registerNodeType();
	Model::TypedList<UnaryOperation>::registerNodeType();
	Model::TypedList<BinaryOperation>::registerNodeType();

	Model::TypedList<Type>::registerNodeType();
	Model::TypedList<PrimitiveType>::registerNodeType();
	Model::TypedList<NamedType>::registerNodeType();

	// Initialize OO Node types
	Project::init();
	Module::init();
	Class::init();
	Method::init();
	MethodItem::init();
	MethodArgument::init();

	Statement::init();

	Expression::init();
	IntegerLiteral::init();
	FloatLiteral::init();
	BooleanLiteral::init();
	StringLiteral::init();
	CharacterLiteral::init();
	NullLiteral::init();
	ThisExpression::init();
	VariableAccess::init();
	NewExpression::init();
	MethodCall::init();
	UnaryOperation::init();
	BinaryOperation::init();

	Type::init();
	PrimitiveType::init();
	NamedType::init();

	return true;
}

void OOModel::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<OOModel>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<OOModel>::runTest(testid).printResultStatistics();
}

}
