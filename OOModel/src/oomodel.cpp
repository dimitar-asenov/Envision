/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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

/***********************************************************************************************************************
 * oomodel.cpp
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "oomodel.h"
#include "allOOModelNodes.h"

#include "SelfTest/src/SelfTestSuite.h"

Q_EXPORT_PLUGIN2( oomodel, OOModel::OOModel )

namespace OOModel {

bool OOModel::initialize(Envision::EnvisionManager&)
{
	// Initialize common nodes
	Visibility::registerNodeType();
	StorageSpecifier::registerNodeType();

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
	Model::TypedList<ExpressionStatement>::registerNodeType();

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
	Model::TypedList<ErrorExpression>::registerNodeType();
	Model::TypedList<UnfinishedOperator>::registerNodeType();
	Model::TypedList<EmptyExpression>::registerNodeType();
	Model::TypedList<CommaExpression>::registerNodeType();
	Model::TypedList<AssignmentExpression>::registerNodeType();
	Model::TypedList<ConditionalExpression>::registerNodeType();

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
	StatementItemList::registerNodeType();
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
	ExpressionStatement::init();

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
	EmptyExpression::init();
	ErrorExpression::init();
	UnfinishedOperator::init();
	CommaExpression::init();
	AssignmentExpression::init();
	ConditionalExpression::init();

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
