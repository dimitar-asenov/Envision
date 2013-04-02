/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

#include "oomodel.h"
#include "allOOModelNodes.h"

#include "SelfTest/src/SelfTestSuite.h"

Q_EXPORT_PLUGIN2( oomodel, OOModel::OOModel )

namespace OOModel {

bool OOModel::initialize(Core::EnvisionManager&)
{
	// Initialize common nodes
	Visibility::registerNodeType();
	StorageSpecifier::registerNodeType();
	OOReference::registerNodeType();

	// Register TypedList instantiations
	Model::TypedList<Project>::init();
	Model::TypedList<Library>::init();
	Model::TypedList<Module>::init();
	Model::TypedList<Class>::init();
	Model::TypedList<Method>::init();
	Model::TypedList<StatementItem>::init();
	Model::TypedList<FormalArgument>::init();
	Model::TypedList<FormalResult>::init();
	Model::TypedList<FormalTypeArgument>::init();
	Model::TypedList<Field>::init();
	Model::TypedList<OOReference>::init();
	Model::TypedList<CatchClause>::init();

	Model::TypedList<Statement>::init();
	Model::TypedList<Block>::init();
	Model::TypedList<BreakStatement>::init();
	Model::TypedList<ContinueStatement>::init();
	Model::TypedList<ForEachStatement>::init();
	Model::TypedList<IfStatement>::init();
	Model::TypedList<LoopStatement>::init();
	Model::TypedList<ReturnStatement>::init();
	Model::TypedList<SwitchCase>::init();
	Model::TypedList<SwitchStatement>::init();
	Model::TypedList<VariableDeclaration>::init();
	Model::TypedList<ExpressionStatement>::init();
	Model::TypedList<TryCatchFinallyStatement>::init();

	Model::TypedList<Expression>::init();
	Model::TypedList<IntegerLiteral>::init();
	Model::TypedList<FloatLiteral>::init();
	Model::TypedList<BooleanLiteral>::init();
	Model::TypedList<StringLiteral>::init();
	Model::TypedList<CharacterLiteral>::init();
	Model::TypedList<NullLiteral>::init();
	Model::TypedList<ThisExpression>::init();
	Model::TypedList<NewExpression>::init();
	Model::TypedList<MethodCallExpression>::init();
	Model::TypedList<UnaryOperation>::init();
	Model::TypedList<BinaryOperation>::init();
	Model::TypedList<CastExpression>::init();
	Model::TypedList<ReferenceExpression>::init();
	Model::TypedList<ArrayInitializer>::init();
	Model::TypedList<ErrorExpression>::init();
	Model::TypedList<UnfinishedOperator>::init();
	Model::TypedList<EmptyExpression>::init();
	Model::TypedList<CommaExpression>::init();
	Model::TypedList<AssignmentExpression>::init();
	Model::TypedList<ConditionalExpression>::init();
	Model::TypedList<ThrowExpression>::init();
	Model::TypedList<LambdaExpression>::init();

	Model::TypedList<TypeExpression>::init();
	Model::TypedList<PrimitiveTypeExpression>::init();
	Model::TypedList<ClassTypeExpression>::init();
	Model::TypedList<ArrayTypeExpression>::init();

	// Initialize OO Node types
	Project::init();
	Library::init();
	Module::init();
	Class::init();
	Method::init();
	StatementItem::init();
	StatementItemList::init();
	FormalArgument::init();
	FormalResult::init();
	FormalTypeArgument::init();
	Field::init();
	CatchClause::init();

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
	ExpressionStatement::init();
	TryCatchFinallyStatement::init();

	Expression::init();
	IntegerLiteral::init();
	FloatLiteral::init();
	BooleanLiteral::init();
	StringLiteral::init();
	CharacterLiteral::init();
	NullLiteral::init();
	ThisExpression::init();
	NewExpression::init();
	MethodCallExpression::init();
	UnaryOperation::init();
	BinaryOperation::init();
	CastExpression::init();
	ReferenceExpression::init();
	ArrayInitializer::init();
	EmptyExpression::init();
	ErrorExpression::init();
	UnfinishedOperator::init();
	CommaExpression::init();
	AssignmentExpression::init();
	ConditionalExpression::init();
	ThrowExpression::init();
	LambdaExpression::init();

	TypeExpression::init();
	PrimitiveTypeExpression::init();
	ClassTypeExpression::init();
	ArrayTypeExpression::init();

	// Register default element creation functions for some lists
	Model::TypedList<StatementItem>::setDefaultElementCreationFunction([]() -> StatementItem* {
		return new ExpressionStatement(new EmptyExpression()); } );
	Model::TypedList<FormalArgument>::setDefaultElementCreationFunction([]() -> FormalArgument* {
		auto arg = new FormalArgument();
		arg->setTypeExpression(new EmptyExpression());
		return arg;} );
	Model::TypedList<FormalResult>::setDefaultElementCreationFunction([]() -> FormalResult* {
		auto res = new FormalResult();
		res->setTypeExpression(new EmptyExpression());
		return res;} );
	Model::TypedList<FormalTypeArgument>::setDefaultElementCreationFunction([]() -> FormalTypeArgument* {
		return new FormalTypeArgument();} );
	Model::TypedList<Field>::setDefaultElementCreationFunction([]() -> Field* {
		auto field = new Field();
		field->setTypeExpression(new EmptyExpression());
		return field;} );
	Model::TypedList<Expression>::setDefaultElementCreationFunction([]() -> Expression* {
		return new EmptyExpression();} );

	return true;
}

void OOModel::unload()
{
}

void OOModel::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<OOModel>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<OOModel>::runTest(testid).printResultStatistics();
}

}
