/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2012 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * oointeraction.cpp
 *
 *  Created on: Jan 12, 2012
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "oointeraction.h"
#include "SelfTest/headers/SelfTestSuite.h"

#include "expression_editor/OOOperatorDescriptorList.h"
#include "handlers/HOOExpression.h"

#include "OOVisualization/headers/allOOVisualizations.h"

#include "InteractionBase/headers/handlers/GenericHandler.h"
#include "InteractionBase/headers/handlers/HText.h"

Q_EXPORT_PLUGIN2( oointeraction, OOInteraction::OOInteraction )

namespace OOInteraction {

bool OOInteraction::initialize(Envision::EnvisionManager&)
{
	OOOperatorDescriptorList::initializeWithDefaultOperators();

	OOVisualization::VProject::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VLibrary::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VModule::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VClass::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VMethod::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VField::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VFormalArgument::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VFormalResult::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VReferenceExpression::setInteractionHandler(HOOExpression::instance());
	OOVisualization::VVariableAccess::setInteractionHandler(HOOExpression::instance());
	OOVisualization::VMethodCallExpression::setInteractionHandler(HOOExpression::instance());
	OOVisualization::VThisExpression::setInteractionHandler(HOOExpression::instance());
	OOVisualization::VCastExpression::setInteractionHandler(HOOExpression::instance());
	OOVisualization::VNewExpression::setInteractionHandler(HOOExpression::instance());
	OOVisualization::VUnaryOperation::setInteractionHandler(HOOExpression::instance());
	OOVisualization::VBinaryOperation::setInteractionHandler(HOOExpression::instance());
	OOVisualization::VArrayInitializer::setInteractionHandler(HOOExpression::instance());
	OOVisualization::VEmptyExpression::setInteractionHandler(HOOExpression::instance());
	OOVisualization::VErrorExpression::setInteractionHandler(HOOExpression::instance());
	OOVisualization::VUnfinishedOperator::setInteractionHandler(HOOExpression::instance());
	OOVisualization::VStringLiteral::setInteractionHandler(HOOExpression::instance());
	OOVisualization::VIntegerLiteral::setInteractionHandler(HOOExpression::instance());
	OOVisualization::VFloatLiteral::setInteractionHandler(HOOExpression::instance());
	OOVisualization::VCharacterLiteral::setInteractionHandler(HOOExpression::instance());
	OOVisualization::VBooleanLiteral::setInteractionHandler(HOOExpression::instance());
	OOVisualization::VNullLiteral::setInteractionHandler(HOOExpression::instance());
	OOVisualization::VMethodCallStatement::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VVariableDeclaration::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VAssignmentStatement::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VIfStatement::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VLoopStatement::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VForEachStatement::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VBreakStatement::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VContinueStatement::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VBlock::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VReturnStatement::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VPrimitiveType::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VNamedType::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VArrayType::setInteractionHandler(Interaction::GenericHandler::instance());

	return true;
}

void OOInteraction::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<OOInteraction>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<OOInteraction>::runTest(testid).printResultStatistics();
}

}
