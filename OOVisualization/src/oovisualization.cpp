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
 * oovisualization.cpp
 *
 *  Created on: Feb 01, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "oovisualization.h"
#include "SelfTest/headers/SelfTestSuite.h"

#include "allOOVisualizations.h"

#include "OOModel/headers/allOOModelNodes.h"

#include "VisualizationBase/headers/Scene.h"
#include "VisualizationBase/headers/node_extensions/Position.h"

#include "InteractionBase/headers/handlers/GenericHandler.h"
#include "InteractionBase/headers/handlers/HText.h"

Q_EXPORT_PLUGIN2( oovisualization, OOVisualization::OOVisualization )

using namespace OOModel;
using namespace Visualization;

namespace OOVisualization {

bool OOVisualization::initialize(Envision::EnvisionManager&)
{
	// Register extensions
	Project::registerNewExtension<Position>();
	Module::registerNewExtension<Position>();
	Class::registerNewExtension<Position>();
	Method::registerNewExtension<Position>();

	// Register visualizations
	Scene::defaultRenderer()->registerVisualization(Project::typeIdStatic(), createVisualization<VProject, Project>);
	Scene::defaultRenderer()->registerVisualization(Library::typeIdStatic(), createVisualization<VLibrary, Library>);
	Scene::defaultRenderer()->registerVisualization(Module::typeIdStatic(), createVisualization<VModule, Module>);
	Scene::defaultRenderer()->registerVisualization(Class::typeIdStatic(), createVisualization<VClass, Class>);
	Scene::defaultRenderer()->registerVisualization(Method::typeIdStatic(), createVisualization<VMethod, Method>);
	Scene::defaultRenderer()->registerVisualization(Field::typeIdStatic(), createVisualization<VField, Field>);
	Scene::defaultRenderer()->registerVisualization(FormalArgument::typeIdStatic(), createVisualization<VFormalArgument, FormalArgument>);
	Scene::defaultRenderer()->registerVisualization(FormalResult::typeIdStatic(), createVisualization<VFormalResult, FormalResult>);
	Scene::defaultRenderer()->registerVisualization(ReferenceExpression::typeIdStatic(), createVisualization<VReferenceExpression, ReferenceExpression>);
	Scene::defaultRenderer()->registerVisualization(VariableAccess::typeIdStatic(), createVisualization<VVariableAccess, VariableAccess>);
	Scene::defaultRenderer()->registerVisualization(MethodCallExpression::typeIdStatic(), createVisualization<VMethodCallExpression, MethodCallExpression>);
	Scene::defaultRenderer()->registerVisualization(CastExpression::typeIdStatic(), createVisualization<VCastExpression, CastExpression>);
	Scene::defaultRenderer()->registerVisualization(NewExpression::typeIdStatic(), createVisualization<VNewExpression, NewExpression>);
	Scene::defaultRenderer()->registerVisualization(UnaryOperation::typeIdStatic(), createVisualization<VUnaryOperation, UnaryOperation>);
	Scene::defaultRenderer()->registerVisualization(BinaryOperation::typeIdStatic(), createVisualization<VBinaryOperation, BinaryOperation>);
	Scene::defaultRenderer()->registerVisualization(ArrayInitializer::typeIdStatic(), createVisualization<VArrayInitializer, ArrayInitializer>);
	Scene::defaultRenderer()->registerVisualization(EmptyExpression::typeIdStatic(), createVisualization<VEmptyExpression, EmptyExpression>);
	Scene::defaultRenderer()->registerVisualization(ErrorExpression::typeIdStatic(), createVisualization<VErrorExpression, ErrorExpression>);
	Scene::defaultRenderer()->registerVisualization(UnfinishedOperator::typeIdStatic(), createVisualization<VUnfinishedOperator, UnfinishedOperator>);
	Scene::defaultRenderer()->registerVisualization(ThisExpression::typeIdStatic(), createVisualization<VThisExpression, ThisExpression>);
	Scene::defaultRenderer()->registerVisualization(StringLiteral::typeIdStatic(), createVisualization<VStringLiteral, StringLiteral>);
	Scene::defaultRenderer()->registerVisualization(IntegerLiteral::typeIdStatic(), createVisualization<VIntegerLiteral, IntegerLiteral>);
	Scene::defaultRenderer()->registerVisualization(FloatLiteral::typeIdStatic(), createVisualization<VFloatLiteral, FloatLiteral>);
	Scene::defaultRenderer()->registerVisualization(CharacterLiteral::typeIdStatic(), createVisualization<VCharacterLiteral, CharacterLiteral>);
	Scene::defaultRenderer()->registerVisualization(BooleanLiteral::typeIdStatic(), createVisualization<VBooleanLiteral, BooleanLiteral>);
	Scene::defaultRenderer()->registerVisualization(NullLiteral::typeIdStatic(), createVisualization<VNullLiteral, NullLiteral>);
	Scene::defaultRenderer()->registerVisualization(MethodCallStatement::typeIdStatic(), createVisualization<VMethodCallStatement, MethodCallStatement>);
	Scene::defaultRenderer()->registerVisualization(VariableDeclaration::typeIdStatic(), createVisualization<VVariableDeclaration, VariableDeclaration>);
	Scene::defaultRenderer()->registerVisualization(AssignmentStatement::typeIdStatic(), createVisualization<VAssignmentStatement, AssignmentStatement>);
	Scene::defaultRenderer()->registerVisualization(Block::typeIdStatic(), createVisualization<VBlock, Block>);
	Scene::defaultRenderer()->registerVisualization(ReturnStatement::typeIdStatic(), createVisualization<VReturnStatement, ReturnStatement>);
	Scene::defaultRenderer()->registerVisualization(IfStatement::typeIdStatic(), createVisualization<VIfStatement, IfStatement>);
	Scene::defaultRenderer()->registerVisualization(LoopStatement::typeIdStatic(), createVisualization<VLoopStatement, LoopStatement>);
	Scene::defaultRenderer()->registerVisualization(ForEachStatement::typeIdStatic(), createVisualization<VForEachStatement, ForEachStatement>);
	Scene::defaultRenderer()->registerVisualization(BreakStatement::typeIdStatic(), createVisualization<VBreakStatement, BreakStatement>);
	Scene::defaultRenderer()->registerVisualization(ContinueStatement::typeIdStatic(), createVisualization<VContinueStatement, ContinueStatement>);
	Scene::defaultRenderer()->registerVisualization(NamedType::typeIdStatic(), createVisualization<VNamedType, NamedType>);
	Scene::defaultRenderer()->registerVisualization(PrimitiveType::typeIdStatic(), createVisualization<VPrimitiveType, PrimitiveType>);
	Scene::defaultRenderer()->registerVisualization(ArrayType::typeIdStatic(), createVisualization<VArrayType, ArrayType>);

	// Register handlers
	// TODO: move this to a better place i.e. OOInteraction Plugin
	// TODO: when you do that remove the dependency in the .plugin meta file
	VProject::setInteractionHandler(Interaction::GenericHandler::instance());
	VLibrary::setInteractionHandler(Interaction::GenericHandler::instance());
	VModule::setInteractionHandler(Interaction::GenericHandler::instance());
	VClass::setInteractionHandler(Interaction::GenericHandler::instance());
	VMethod::setInteractionHandler(Interaction::GenericHandler::instance());
	VField::setInteractionHandler(Interaction::GenericHandler::instance());
	VFormalArgument::setInteractionHandler(Interaction::GenericHandler::instance());
	VFormalResult::setInteractionHandler(Interaction::GenericHandler::instance());
	VReferenceExpression::setInteractionHandler(Interaction::GenericHandler::instance());
	VVariableAccess::setInteractionHandler(Interaction::GenericHandler::instance());
	VMethodCallExpression::setInteractionHandler(Interaction::GenericHandler::instance());
	VThisExpression::setInteractionHandler(Interaction::GenericHandler::instance());
	VCastExpression::setInteractionHandler(Interaction::GenericHandler::instance());
	VNewExpression::setInteractionHandler(Interaction::GenericHandler::instance());
	VUnaryOperation::setInteractionHandler(Interaction::GenericHandler::instance());
	VBinaryOperation::setInteractionHandler(Interaction::GenericHandler::instance());
	VArrayInitializer::setInteractionHandler(Interaction::GenericHandler::instance());
	VEmptyExpression::setInteractionHandler(Interaction::GenericHandler::instance());
	VErrorExpression::setInteractionHandler(Interaction::GenericHandler::instance());
	VUnfinishedOperator::setInteractionHandler(Interaction::GenericHandler::instance());
	VStringLiteral::setInteractionHandler(Interaction::GenericHandler::instance());
	VIntegerLiteral::setInteractionHandler(Interaction::GenericHandler::instance());
	VFloatLiteral::setInteractionHandler(Interaction::GenericHandler::instance());
	VCharacterLiteral::setInteractionHandler(Interaction::GenericHandler::instance());
	VBooleanLiteral::setInteractionHandler(Interaction::GenericHandler::instance());
	VNullLiteral::setInteractionHandler(Interaction::GenericHandler::instance());
	VMethodCallStatement::setInteractionHandler(Interaction::GenericHandler::instance());
	VVariableDeclaration::setInteractionHandler(Interaction::GenericHandler::instance());
	VAssignmentStatement::setInteractionHandler(Interaction::GenericHandler::instance());
	VIfStatement::setInteractionHandler(Interaction::GenericHandler::instance());
	VLoopStatement::setInteractionHandler(Interaction::GenericHandler::instance());
	VForEachStatement::setInteractionHandler(Interaction::GenericHandler::instance());
	VBreakStatement::setInteractionHandler(Interaction::GenericHandler::instance());
	VContinueStatement::setInteractionHandler(Interaction::GenericHandler::instance());
	VBlock::setInteractionHandler(Interaction::GenericHandler::instance());
	VReturnStatement::setInteractionHandler(Interaction::GenericHandler::instance());
	VPrimitiveType::setInteractionHandler(Interaction::GenericHandler::instance());
	VNamedType::setInteractionHandler(Interaction::GenericHandler::instance());
	VArrayType::setInteractionHandler(Interaction::GenericHandler::instance());

	return true;
}

void OOVisualization::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<OOVisualization>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<OOVisualization>::runTest(testid).printResultStatistics();
}

}
