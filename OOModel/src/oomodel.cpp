/***********************************************************************************************************************
 * oomodel.cpp
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "oomodel.h"
#include "SelfTest/headers/SelfTestSuite.h"

#include "common/Visibility.h"
#include "common/Static.h"

#include "Project.h"
#include "Module.h"
#include "Class.h"
#include "Method.h"
#include "MethodItem.h"
#include "FormalArgument.h"
#include "Field.h"
#include "OOReference.h"

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

#include "types/Type.h"
#include "types/PrimitiveType.h"
#include "types/NamedType.h"

Q_EXPORT_PLUGIN2( oomodel, OOModel::OOModel )

namespace OOModel {

bool OOModel::initialize(Envision::EnvisionManager&)
{
	// Initialize common nodes
	Visibility::registerNodeType();
	Static::registerNodeType();
	OOReference::registerNodeType();

	// Register TypedList instantiations
	Model::TypedList<Project>::registerNodeType();
	Model::TypedList<Module>::registerNodeType();
	Model::TypedList<Class>::registerNodeType();
	Model::TypedList<Method>::registerNodeType();
	Model::TypedList<MethodItem>::registerNodeType();
	Model::TypedList<FormalArgument>::registerNodeType();
	Model::TypedList<Field>::registerNodeType();
	Model::TypedList<OOReference>::registerNodeType();

	Model::TypedList<Statement>::registerNodeType();
	Model::TypedList<Block>::registerNodeType();
	Model::TypedList<BreakStatement>::registerNodeType();
	Model::TypedList<ContinueStatement>::registerNodeType();
	Model::TypedList<ForEachStatement>::registerNodeType();
	Model::TypedList<IfStatement>::registerNodeType();
	Model::TypedList<LoopStatement>::registerNodeType();
	Model::TypedList<ReturnStatement>::registerNodeType();
	Model::TypedList<SwitchCase>::registerNodeType();
	Model::TypedList<SwitchStatement>::registerNodeType();
	Model::TypedList<VariableDeclaration>::registerNodeType();
	Model::TypedList<MethodCallStatement>::registerNodeType();

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
	Model::TypedList<MethodCallExpression>::registerNodeType();
	Model::TypedList<UnaryOperation>::registerNodeType();
	Model::TypedList<BinaryOperation>::registerNodeType();
	Model::TypedList<CastExpression>::registerNodeType();
	Model::TypedList<ReferenceExpression>::registerNodeType();

	Model::TypedList<Type>::registerNodeType();
	Model::TypedList<PrimitiveType>::registerNodeType();
	Model::TypedList<NamedType>::registerNodeType();

	// Initialize OO Node types
	Project::init();
	Module::init();
	Class::init();
	Method::init();
	MethodItem::init();
	FormalArgument::init();
	Field::init();

	Statement::init();
	Block::init();
	BreakStatement::init();
	ContinueStatement::init();
	ForEachStatement::init();
	IfStatement::init();
	LoopStatement::init();
	ReturnStatement::init();
	SwitchCase::init();
	SwitchStatement::init();
	VariableDeclaration::init();
	MethodCallStatement::init();

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
	MethodCallExpression::init();
	UnaryOperation::init();
	BinaryOperation::init();
	CastExpression::init();
	ReferenceExpression::init();

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
