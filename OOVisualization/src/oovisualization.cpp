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

#include "oovisualization.h"
#include "SelfTest/src/SelfTestSuite.h"

#include "allOOVisualizations.h"

#include "OOModel/src/allOOModelNodes.h"

#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/node_extensions/Position.h"

Q_EXPORT_PLUGIN2( oovisualization, OOVisualization::OOVisualization )

using namespace OOModel;
using namespace Visualization;

namespace OOVisualization {

bool OOVisualization::initialize(Core::EnvisionManager&)
{
	// Register extensions
	Project::registerNewExtension<Position>();
	Module::registerNewExtension<Position>();
	Class::registerNewExtension<Position>();
	Method::registerNewExtension<Position>();

	// Register visualizations
	Scene::defaultRenderer()->registerVisualization(Project::typeIdStatic(), createVisualization<VProject, Project>);
	Scene::defaultRenderer()->registerVisualization(Module::typeIdStatic(), createVisualization<VModule, Module>);
	Scene::defaultRenderer()->registerVisualization(Class::typeIdStatic(), createVisualization<VClass, Class>);
	Scene::defaultRenderer()->registerVisualization(Method::typeIdStatic(), createVisualization<VMethod, Method>);
	Scene::defaultRenderer()->registerVisualization(Field::typeIdStatic(), createVisualization<VField, Field>);
	Scene::defaultRenderer()->registerVisualization(Enumerator::typeIdStatic(),
			createVisualization<VEnumerator, Enumerator>);
	Scene::defaultRenderer()->registerVisualization(FormalArgument::typeIdStatic(),
			createVisualization<VFormalArgument, FormalArgument>);
	Scene::defaultRenderer()->registerVisualization(FormalTypeArgument::typeIdStatic(),
			createVisualization<VFormalTypeArgument, FormalTypeArgument>);
	Scene::defaultRenderer()->registerVisualization(FormalResult::typeIdStatic(),
			createVisualization<VFormalResult, FormalResult>);
	Scene::defaultRenderer()->registerVisualization(StatementItemList::typeIdStatic(),
			createVisualization<VStatementItemList, StatementItemList>);
	Scene::defaultRenderer()->registerVisualization(ReferenceExpression::typeIdStatic(),
			createVisualization<VReferenceExpression, ReferenceExpression>);
	Scene::defaultRenderer()->registerVisualization(MethodCallExpression::typeIdStatic(),
			createVisualization<VMethodCallExpression, MethodCallExpression>);
	Scene::defaultRenderer()->registerVisualization(CastExpression::typeIdStatic(),
			createVisualization<VCastExpression, CastExpression>);
	Scene::defaultRenderer()->registerVisualization(NewExpression::typeIdStatic(),
			createVisualization<VNewExpression, NewExpression>);
	Scene::defaultRenderer()->registerVisualization(UnaryOperation::typeIdStatic(),
			createVisualization<VUnaryOperation, UnaryOperation>);
	Scene::defaultRenderer()->registerVisualization(BinaryOperation::typeIdStatic(),
			createVisualization<VBinaryOperation, BinaryOperation>);
	Scene::defaultRenderer()->registerVisualization(CommaExpression::typeIdStatic(),
			createVisualization<VCommaExpression, CommaExpression>);
	Scene::defaultRenderer()->registerVisualization(ArrayInitializer::typeIdStatic(),
			createVisualization<VArrayInitializer, ArrayInitializer>);
	Scene::defaultRenderer()->registerVisualization(EmptyExpression::typeIdStatic(),
			createVisualization<VEmptyExpression, EmptyExpression>);
	Scene::defaultRenderer()->registerVisualization(ErrorExpression::typeIdStatic(),
			createVisualization<VErrorExpression, ErrorExpression>);
	Scene::defaultRenderer()->registerVisualization(UnfinishedOperator::typeIdStatic(),
			createVisualization<VUnfinishedOperator, UnfinishedOperator>);
	Scene::defaultRenderer()->registerVisualization(ThisExpression::typeIdStatic(),
			createVisualization<VThisExpression, ThisExpression>);
	Scene::defaultRenderer()->registerVisualization(StringLiteral::typeIdStatic(),
			createVisualization<VStringLiteral, StringLiteral>);
	Scene::defaultRenderer()->registerVisualization(IntegerLiteral::typeIdStatic(),
			createVisualization<VIntegerLiteral, IntegerLiteral>);
	Scene::defaultRenderer()->registerVisualization(FloatLiteral::typeIdStatic(),
			createVisualization<VFloatLiteral, FloatLiteral>);
	Scene::defaultRenderer()->registerVisualization(CharacterLiteral::typeIdStatic(),
			createVisualization<VCharacterLiteral, CharacterLiteral>);
	Scene::defaultRenderer()->registerVisualization(BooleanLiteral::typeIdStatic(),
			createVisualization<VBooleanLiteral, BooleanLiteral>);
	Scene::defaultRenderer()->registerVisualization(NullLiteral::typeIdStatic(),
			createVisualization<VNullLiteral, NullLiteral>);
	Scene::defaultRenderer()->registerVisualization(VariableDeclaration::typeIdStatic(),
			createVisualization<VVariableDeclaration, VariableDeclaration>);
	Scene::defaultRenderer()->registerVisualization(AssignmentExpression::typeIdStatic(),
			createVisualization<VAssignmentExpression, AssignmentExpression>);
	Scene::defaultRenderer()->registerVisualization(ConditionalExpression::typeIdStatic(),
			createVisualization<VConditionalExpression, ConditionalExpression>);
	Scene::defaultRenderer()->registerVisualization(ThrowExpression::typeIdStatic(),
			createVisualization<VThrowExpression, ThrowExpression>);
	Scene::defaultRenderer()->registerVisualization(LambdaExpression::typeIdStatic(),
					createVisualization<VLambdaExpression, LambdaExpression>);

	Scene::defaultRenderer()->registerVisualization(Block::typeIdStatic(),
			createVisualization<VBlock, Block>);
	Scene::defaultRenderer()->registerVisualization(ReturnStatement::typeIdStatic(),
			createVisualization<VReturnStatement, ReturnStatement>);
	Scene::defaultRenderer()->registerVisualization(IfStatement::typeIdStatic(),
			createVisualization<VIfStatement, IfStatement>);
	Scene::defaultRenderer()->registerVisualization(LoopStatement::typeIdStatic(),
			createVisualization<VLoopStatement, LoopStatement>);
	Scene::defaultRenderer()->registerVisualization(ForEachStatement::typeIdStatic(),
			createVisualization<VForEachStatement, ForEachStatement>);
	Scene::defaultRenderer()->registerVisualization(BreakStatement::typeIdStatic(),
			createVisualization<VBreakStatement, BreakStatement>);
	Scene::defaultRenderer()->registerVisualization(ContinueStatement::typeIdStatic(),
			createVisualization<VContinueStatement, ContinueStatement>);
	Scene::defaultRenderer()->registerVisualization(ExpressionStatement::typeIdStatic(),
			createVisualization<VExpressionStatement, ExpressionStatement>);
	Scene::defaultRenderer()->registerVisualization(TryCatchFinallyStatement::typeIdStatic(),
			createVisualization<VTryCatchFinally, TryCatchFinallyStatement>);
	Scene::defaultRenderer()->registerVisualization(CatchClause::typeIdStatic(),
			createVisualization<VCatchClause, CatchClause>);

	Scene::defaultRenderer()->registerVisualization(ClassTypeExpression::typeIdStatic(),
			createVisualization<VClassType, ClassTypeExpression>);
	Scene::defaultRenderer()->registerVisualization(PrimitiveTypeExpression::typeIdStatic(),
			createVisualization<VPrimitiveType, PrimitiveTypeExpression>);
	Scene::defaultRenderer()->registerVisualization(ArrayTypeExpression::typeIdStatic(),
			createVisualization<VArrayType, ArrayTypeExpression>);

	return true;
}

void OOVisualization::unload()
{
}

void OOVisualization::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<OOVisualization>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<OOVisualization>::runTest(testid).printResultStatistics();
}

}
