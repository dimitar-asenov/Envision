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
#include "handlers/HProject.h"
#include "handlers/HClass.h"
#include "handlers/HMethod.h"
#include "handlers/HExpression.h"
#include "handlers/HFormalArgument.h"
#include "handlers/HStatement.h"

#include "string_components/UnaryOperatorStringComponents.h"
#include "string_components/BinaryOperatorStringComponents.h"
#include "string_components/EmptyExpressionStringComponents.h"
#include "string_components/ErrorExpressionStringComponents.h"
#include "string_components/CastExpressionStringComponents.h"
#include "string_components/CommaExpressionStringComponents.h"
#include "string_components/NewArrayStringComponents.h"
#include "string_components/InitializerStringComponents.h"
#include "string_components/BooleanLiteralStringComponents.h"
#include "string_components/IntegerLiteralStringComponents.h"
#include "string_components/StringLiteralStringComponents.h"
#include "string_components/NullLiteralStringComponents.h"
#include "string_components/ThisExpressionStringComponents.h"
#include "string_components/UnfinishedOperatorStringComponents.h"
#include "string_components/VariableAccessStringComponents.h"
#include "string_components/CallStringComponents.h"
#include "string_components/PrimitiveTypeStringComponents.h"
#include "string_components/ArrayTypeStringComponents.h"
#include "string_components/AssignmentStringComponents.h"

#include "string_providers/SequentialVisualizationStringProvider.h"
#include "string_providers/EmptyExpressionStringProvider.h"
#include "string_providers/SimpleLiteralStringProvider.h"
#include "string_providers/TextRendererStringProvider.h"
#include "string_providers/StaticStringProvider.h"
#include "string_providers/InitializerStringProvider.h"
#include "string_providers/CallStringProvider.h"

#include "OOVisualization/headers/allOOVisualizations.h"

#include "OOModel/headers/allOOModelNodes.h"

#include "InteractionBase/headers/handlers/GenericHandler.h"
#include "InteractionBase/headers/handlers/HText.h"

#include "VisualizationBase/headers/items/Static.h"
#include "VisualizationBase/headers/items/Symbol.h"
#include "VisualizationBase/headers/items/Text.h"
#include "VisualizationBase/headers/items/VText.h"

#include "ModelBase/headers/adapter/AdapterManager.h"

Q_EXPORT_PLUGIN2( oointeraction, OOInteraction::OOInteraction )

namespace OOInteraction {

bool OOInteraction::initialize(Envision::EnvisionManager&)
{
	OOOperatorDescriptorList::initializeWithDefaultOperators();

	OOVisualization::VProject::setInteractionHandler(HProject::instance());
	OOVisualization::VLibrary::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VModule::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VClass::setInteractionHandler(HClass::instance());
	OOVisualization::VMethod::setInteractionHandler(HMethod::instance());
	OOVisualization::VField::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VFormalArgument::setInteractionHandler(HFormalArgument::instance());
	OOVisualization::VFormalResult::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VReferenceExpression::setInteractionHandler(HExpression::instance());
	OOVisualization::VVariableAccess::setInteractionHandler(HExpression::instance());
	OOVisualization::VMethodCallExpression::setInteractionHandler(HExpression::instance());
	OOVisualization::VThisExpression::setInteractionHandler(HExpression::instance());
	OOVisualization::VCastExpression::setInteractionHandler(HExpression::instance());
	OOVisualization::VNewExpression::setInteractionHandler(HExpression::instance());
	OOVisualization::VUnaryOperation::setInteractionHandler(HExpression::instance());
	OOVisualization::VBinaryOperation::setInteractionHandler(HExpression::instance());
	OOVisualization::VCommaExpression::setInteractionHandler(HExpression::instance());
	OOVisualization::VArrayInitializer::setInteractionHandler(HExpression::instance());
	OOVisualization::VEmptyExpression::setInteractionHandler(HExpression::instance());
	OOVisualization::VErrorExpression::setInteractionHandler(HExpression::instance());
	OOVisualization::VUnfinishedOperator::setInteractionHandler(HExpression::instance());
	OOVisualization::VStringLiteral::setInteractionHandler(HExpression::instance());
	OOVisualization::VIntegerLiteral::setInteractionHandler(HExpression::instance());
	OOVisualization::VFloatLiteral::setInteractionHandler(HExpression::instance());
	OOVisualization::VCharacterLiteral::setInteractionHandler(HExpression::instance());
	OOVisualization::VBooleanLiteral::setInteractionHandler(HExpression::instance());
	OOVisualization::VNullLiteral::setInteractionHandler(HExpression::instance());
	OOVisualization::VVariableDeclaration::setInteractionHandler(HExpression::instance());
	OOVisualization::VAssignmentExpression::setInteractionHandler(HExpression::instance());
	OOVisualization::VExpressionStatement::setInteractionHandler(HStatement::instance());
	OOVisualization::VIfStatement::setInteractionHandler(HStatement::instance());
	OOVisualization::VLoopStatement::setInteractionHandler(HStatement::instance());
	OOVisualization::VForEachStatement::setInteractionHandler(HStatement::instance());
	OOVisualization::VBreakStatement::setInteractionHandler(HStatement::instance());
	OOVisualization::VContinueStatement::setInteractionHandler(HStatement::instance());
	OOVisualization::VBlock::setInteractionHandler(HStatement::instance());
	OOVisualization::VReturnStatement::setInteractionHandler(HStatement::instance());
	OOVisualization::VPrimitiveType::setInteractionHandler(HExpression::instance());
	OOVisualization::VNamedType::setInteractionHandler(HExpression::instance());
	OOVisualization::VArrayType::setInteractionHandler(HExpression::instance());

	// Register string components that convert an expression to a string list representing its components
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, UnaryOperatorStringComponents, OOModel::UnaryOperation>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, BinaryOperatorStringComponents, OOModel::BinaryOperation>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, CastExpressionStringComponents, OOModel::CastExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, CommaExpressionStringComponents, OOModel::CommaExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, InitializerStringComponents, OOModel::ArrayInitializer>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, CallStringComponents, OOModel::MethodCallExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, NewArrayStringComponents, OOModel::NewExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, BooleanLiteralStringComponents, OOModel::BooleanLiteral>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, IntegerLiteralStringComponents, OOModel::IntegerLiteral>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, StringLiteralStringComponents, OOModel::StringLiteral>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, NullLiteralStringComponents, OOModel::NullLiteral>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, ThisExpressionStringComponents, OOModel::ThisExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, VariableAccessStringComponents, OOModel::VariableAccess>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, UnfinishedOperatorStringComponents, OOModel::UnfinishedOperator>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, ErrorExpressionStringComponents, OOModel::ErrorExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, EmptyExpressionStringComponents, OOModel::EmptyExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, PrimitiveTypeStringComponents, OOModel::PrimitiveType>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, ArrayTypeStringComponents, OOModel::ArrayType>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, AssignmentStringComponents, OOModel::AssignmentExpression>();

	// Register string providers
	Model::AdapterManager::registerAdapterViaConstructor
		<StringProvider, SequentialVisualizationStringProvider, OOVisualization::VUnaryOperation>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringProvider, SequentialVisualizationStringProvider, OOVisualization::VBinaryOperation>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringProvider, SequentialVisualizationStringProvider, OOVisualization::VCastExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringProvider, SequentialVisualizationStringProvider, OOVisualization::VCommaExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringProvider, SimpleLiteralStringProvider, OOVisualization::VBooleanLiteral>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringProvider, InitializerStringProvider, OOVisualization::VArrayInitializer>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringProvider, CallStringProvider, OOVisualization::VMethodCallExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringProvider, SimpleLiteralStringProvider, OOVisualization::VIntegerLiteral>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringProvider, SequentialVisualizationStringProvider, OOVisualization::VStringLiteral>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringProvider, SimpleLiteralStringProvider, OOVisualization::VNullLiteral>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringProvider, SimpleLiteralStringProvider, OOVisualization::VThisExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringProvider, SequentialVisualizationStringProvider, OOVisualization::VVariableAccess>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringProvider, SequentialVisualizationStringProvider, OOVisualization::VUnfinishedOperator>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringProvider, SequentialVisualizationStringProvider, OOVisualization::VErrorExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringProvider, EmptyExpressionStringProvider, OOVisualization::VEmptyExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringProvider, TextRendererStringProvider, Visualization::Text>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringProvider, TextRendererStringProvider, Visualization::VText>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringProvider, TextRendererStringProvider, Visualization::Symbol>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringProvider, StaticStringProvider, Visualization::Static>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringProvider, SimpleLiteralStringProvider, OOVisualization::VPrimitiveType>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringProvider, SequentialVisualizationStringProvider, OOVisualization::VArrayType>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringProvider, SequentialVisualizationStringProvider, OOVisualization::VAssignmentExpression>();

	return true;
}

void OOInteraction::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<OOInteraction>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<OOInteraction>::runTest(testid).printResultStatistics();
}

}
