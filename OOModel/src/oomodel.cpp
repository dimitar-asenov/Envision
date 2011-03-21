/***********************************************************************************************************************
 * oomodel.cpp
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "oomodel.h"
#include "allOOModelNodes.h"

#include "SelfTest/headers/SelfTestSuite.h"

Q_EXPORT_PLUGIN2( oomodel, OOModel::OOModel )

namespace OOModel {

bool OOModel::initialize(Envision::EnvisionManager&)
{
	// Initialize common nodes
	Visibility::registerNodeType();
	Static::registerNodeType();

	// Register TypedList instantiations
	Model::TypedList<Project>::registerNodeType();
	Model::TypedList<Library>::registerNodeType();
	Model::TypedList<Module>::registerNodeType();
	Model::TypedList<Class>::registerNodeType();
	Model::TypedList<Method>::registerNodeType();
	Model::TypedList<StatementItem>::registerNodeType();
	Model::TypedList<FormalArgument>::registerNodeType();
	Model::TypedList<FormalResult>::registerNodeType();
	Model::TypedList<Field>::registerNodeType();

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
	Model::TypedList<AssignmentStatement>::registerNodeType();

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
	Model::TypedList<ArrayInitializer>::registerNodeType();

	Model::TypedList<Type>::registerNodeType();
	Model::TypedList<PrimitiveType>::registerNodeType();
	Model::TypedList<NamedType>::registerNodeType();
	Model::TypedList<ArrayType>::registerNodeType();

	// Initialize OO Node types
	Project::init();
	Library::init();
	Module::init();
	Class::init();
	Method::init();
	StatementItem::init();
	FormalArgument::init();
	FormalResult::init();
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
	AssignmentStatement::init();

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
	ArrayInitializer::init();

	Type::init();
	PrimitiveType::init();
	NamedType::init();
	ArrayType::init();

	return true;
}

void OOModel::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<OOModel>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<OOModel>::runTest(testid).printResultStatistics();
}

}
