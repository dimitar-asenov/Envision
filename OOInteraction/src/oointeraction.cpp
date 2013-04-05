/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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

#include "oointeraction.h"
#include "SelfTest/src/SelfTestSuite.h"

#include "expression_editor/OOOperatorDescriptorList.h"
#include "handlers/HProject.h"
#include "handlers/HClass.h"
#include "handlers/HMethod.h"
#include "handlers/HExpression.h"
#include "handlers/HFormalArgument.h"
#include "handlers/HStatement.h"
#include "handlers/HLoop.h"
#include "handlers/HForEachStatement.h"
#include "handlers/HIfStatement.h"
#include "handlers/HReturnStatement.h"
#include "handlers/HKeywordStatement.h"

#include "commands/CCreateProject.h"
#include "commands/CCreateClass.h"
#include "commands/CSceneHandlerItemTest.h"

#include "string_components/UnaryOperatorStringComponents.h"
#include "string_components/BinaryOperatorStringComponents.h"
#include "string_components/EmptyExpressionStringComponents.h"
#include "string_components/ErrorExpressionStringComponents.h"
#include "string_components/CastExpressionStringComponents.h"
#include "string_components/CommaExpressionStringComponents.h"
#include "string_components/ConditionalExpressionStringComponents.h"
#include "string_components/NewArrayStringComponents.h"
#include "string_components/InitializerStringComponents.h"
#include "string_components/BooleanLiteralStringComponents.h"
#include "string_components/IntegerLiteralStringComponents.h"
#include "string_components/StringLiteralStringComponents.h"
#include "string_components/NullLiteralStringComponents.h"
#include "string_components/ThisExpressionStringComponents.h"
#include "string_components/UnfinishedOperatorStringComponents.h"
#include "string_components/ReferenceExpressionStringComponents.h"
#include "string_components/CallStringComponents.h"
#include "string_components/PrimitiveTypeStringComponents.h"
#include "string_components/ArrayTypeStringComponents.h"
#include "string_components/ClassTypeStringComponents.h"
#include "string_components/AssignmentStringComponents.h"
#include "string_components/VariableDeclarationStringComponents.h"
#include "string_components/ListStringComponents.h"
#include "string_components/ThrowStringComponents.h"
#include "string_components/LambdaStringComponents.h"

#include "string_offset_providers/SequentialVisualizationStringOffsetProvider.h"
#include "string_offset_providers/EmptyExpressionStringOffsetProvider.h"
#include "string_offset_providers/SimpleLiteralStringOffsetProvider.h"
#include "string_offset_providers/TextRendererStringOffsetProvider.h"
#include "string_offset_providers/StaticStringOffsetProvider.h"
#include "string_offset_providers/InitializerStringOffsetProvider.h"
#include "string_offset_providers/CallStringOffsetProvider.h"
#include "string_offset_providers/NewArrayStringOffsetProvider.h"
#include "string_offset_providers/ThrowStringOffsetProvider.h"
#include "string_offset_providers/CastStringOffsetProvider.h"
#include "string_offset_providers/VariableDeclarationStringOffsetProvider.h"
#include "string_offset_providers/ReferenceExpressionStringOffsetProvider.h"
#include "string_offset_providers/UnaryOperatorStringOffsetProvider.h"
#include "string_offset_providers/ClassTypeStringOffsetProvider.h"
#include "string_offset_providers/CompoundObjectStringOffsetProvider.h"
#include "string_offset_providers/KeywordMethodCallStringOffsetProvider.h"

#include "customization/CustomizationVisitor.h"

#include "OOVisualization/src/allOOVisualizations.h"

#include "OOModel/src/allOOModelNodes.h"


#include "InteractionBase/src/actions/Action.h"
#include "InteractionBase/src/handlers/GenericHandler.h"
#include "InteractionBase/src/handlers/HList.h"
#include "InteractionBase/src/handlers/HText.h"
#include "InteractionBase/src/handlers/HSceneHandlerItem.h"
#include "InteractionBase/src/events/SetCursorEvent.h"

#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/items/Symbol.h"
#include "VisualizationBase/src/items/Text.h"
#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/VisualizationManager.h"

#include "ModelBase/src/adapter/AdapterManager.h"

Q_EXPORT_PLUGIN2( oointeraction, OOInteraction::OOInteraction )

namespace OOInteraction {

bool OOInteraction::initialize(Core::EnvisionManager&)
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
	OOVisualization::VStatementItemList::setInteractionHandler(Interaction::HList::instance());
	OOVisualization::VReferenceExpression::setInteractionHandler(HExpression::instance());
	OOVisualization::VMethodCallExpression::setInteractionHandler(HExpression::instance());
	OOVisualization::VThisExpression::setInteractionHandler(HExpression::instance());
	OOVisualization::VCastExpression::setInteractionHandler(HExpression::instance());
	OOVisualization::VNewExpression::setInteractionHandler(HExpression::instance());
	OOVisualization::VThrowExpression::setInteractionHandler(HExpression::instance());
	OOVisualization::VLambdaExpression::setInteractionHandler(HExpression::instance());
	OOVisualization::VUnaryOperation::setInteractionHandler(HExpression::instance());
	OOVisualization::VBinaryOperation::setInteractionHandler(HExpression::instance());
	OOVisualization::VCommaExpression::setInteractionHandler(HExpression::instance());
	OOVisualization::VConditionalExpression::setInteractionHandler(HExpression::instance());
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
	OOVisualization::VIfStatement::setInteractionHandler(HIfStatement::instance());
	OOVisualization::VLoopStatement::setInteractionHandler(HLoop::instance());
	OOVisualization::VForEachStatement::setInteractionHandler(HForEachStatement::instance());
	OOVisualization::VBreakStatement::setInteractionHandler(HKeywordStatement::instance());
	OOVisualization::VContinueStatement::setInteractionHandler(HKeywordStatement::instance());
	OOVisualization::VBlock::setInteractionHandler(HStatement::instance());
	OOVisualization::VReturnStatement::setInteractionHandler(HReturnStatement::instance());
	OOVisualization::VTryCatchFinally::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VCatchClause::setInteractionHandler(Interaction::GenericHandler::instance());
	OOVisualization::VPrimitiveType::setInteractionHandler(HExpression::instance());
	OOVisualization::VClassType::setInteractionHandler(HExpression::instance());
	OOVisualization::VArrayType::setInteractionHandler(HExpression::instance());
	OOVisualization::VKeywordMethodCall::setInteractionHandler(HExpression::instance());

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
		<StringComponents, ConditionalExpressionStringComponents, OOModel::ConditionalExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, InitializerStringComponents, OOModel::ArrayInitializer>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, CallStringComponents, OOModel::MethodCallExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, NewArrayStringComponents, OOModel::NewExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, ThrowStringComponents, OOModel::ThrowExpression>();
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
		<StringComponents, ReferenceExpressionStringComponents, OOModel::ReferenceExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, UnfinishedOperatorStringComponents, OOModel::UnfinishedOperator>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, ErrorExpressionStringComponents, OOModel::ErrorExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, EmptyExpressionStringComponents, OOModel::EmptyExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, PrimitiveTypeStringComponents, OOModel::PrimitiveTypeExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, ArrayTypeStringComponents, OOModel::ArrayTypeExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, ClassTypeStringComponents, OOModel::ClassTypeExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, AssignmentStringComponents, OOModel::AssignmentExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, VariableDeclarationStringComponents, OOModel::VariableDeclaration>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, ListStringComponents, Model::TypedList<OOModel::Expression> >();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, ListStringComponents, Model::TypedList<OOModel::Statement> >();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringComponents, LambdaStringComponents, OOModel::LambdaExpression>();

	// Register string providers
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, SequentialVisualizationStringOffsetProvider, OOVisualization::VUnaryOperation>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, SequentialVisualizationStringOffsetProvider, OOVisualization::VBinaryOperation>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, CastStringOffsetProvider, OOVisualization::VCastExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, SequentialVisualizationStringOffsetProvider, OOVisualization::VCommaExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, SequentialVisualizationStringOffsetProvider, OOVisualization::VConditionalExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, NewArrayStringOffsetProvider, OOVisualization::VNewExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, ThrowStringOffsetProvider, OOVisualization::VThrowExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, SimpleLiteralStringOffsetProvider, OOVisualization::VBooleanLiteral>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, InitializerStringOffsetProvider, OOVisualization::VArrayInitializer>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, CallStringOffsetProvider, OOVisualization::VMethodCallExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, SimpleLiteralStringOffsetProvider, OOVisualization::VIntegerLiteral>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, SequentialVisualizationStringOffsetProvider, OOVisualization::VStringLiteral>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, SimpleLiteralStringOffsetProvider, OOVisualization::VNullLiteral>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, SimpleLiteralStringOffsetProvider, OOVisualization::VThisExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, UnaryOperatorStringOffsetProvider, OOVisualization::VUnaryOperation>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, ReferenceExpressionStringOffsetProvider, OOVisualization::VReferenceExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, SequentialVisualizationStringOffsetProvider, OOVisualization::VUnfinishedOperator>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, SequentialVisualizationStringOffsetProvider, OOVisualization::VErrorExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, EmptyExpressionStringOffsetProvider, OOVisualization::VEmptyExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, TextRendererStringOffsetProvider, Visualization::Text>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, TextRendererStringOffsetProvider, Visualization::VText>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, TextRendererStringOffsetProvider, Visualization::Symbol>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, StaticStringOffsetProvider, Visualization::Static>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, SimpleLiteralStringOffsetProvider, OOVisualization::VPrimitiveType>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, SequentialVisualizationStringOffsetProvider, OOVisualization::VArrayType>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, ClassTypeStringOffsetProvider, OOVisualization::VClassType>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, SequentialVisualizationStringOffsetProvider, OOVisualization::VAssignmentExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, VariableDeclarationStringOffsetProvider, OOVisualization::VVariableDeclaration>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, CompoundObjectStringOffsetProvider, OOVisualization::VLambdaExpression>();
	Model::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, KeywordMethodCallStringOffsetProvider, OOVisualization::VKeywordMethodCall>();

	Interaction::HSceneHandlerItem::instance()->addCommand(new CCreateProject());
	Interaction::HSceneHandlerItem::instance()->addCommand(new CCreateClass());
	Interaction::HSceneHandlerItem::instance()->addCommand(new CSceneHandlerItemTest());

	initializeActions();

	// Initialize customization support
	auto customizationGroup = new Visualization::VisualizationGroup();
	customizationGroup->setConditionFunction([=](Visualization::Item*, Model::Node* node) -> bool
	{
		auto call = static_cast<OOModel::MethodCallExpression*>(node);
		if (call->methodDefinition()) return true;
		return false;
	});
	Visualization::Scene::defaultRenderer()->registerGroup(
		OOModel::MethodCallExpression::typeIdStatic(), customizationGroup);
	CustomizationVisitor::init(customizationGroup);

	Visualization::VisualizationManager::instance().mainScene()->addRefreshActionFunction(
			CustomizationVisitor::onSceneRefresh);

	return true;
}

void OOInteraction::unload()
{
}

void OOInteraction::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<OOInteraction>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<OOInteraction>::runTest(testid).printResultStatistics();
}

void OOInteraction::initializeActions()
{
	Interaction::Action::add<OOModel::Method>(new Interaction::Action("r","Create result",
		Interaction::Action::ActionFunctionOnItem([](Visualization::Item* item){
			auto node = static_cast<OOModel::Method*>(item->node());
			if ( node->results()->size() == 0)
			{
				node->beginModification("add result");
				node->results()->append(new OOModel::FormalResult("", new OOModel::EmptyExpression()));
				node->endModification();
			}
			item->setUpdateNeededForChildItem(Visualization::Item::StandardUpdate, node->results()->first());
			item->scene()->addPostEventAction(new Interaction::SetCursorEvent(item, node->results()->first()));
	})));

	Interaction::Action::add<OOModel::Method>(new Interaction::Action("n","Create annotation",
		Interaction::Action::ActionFunctionOnItem([](Visualization::Item* item){
			auto node = static_cast<OOModel::Method*>(item->node());
			if ( node->annotations()->size() == 0)
			{
				node->beginModification("add annotation");
				node->annotations()->append(new OOModel::ExpressionStatement(new OOModel::EmptyExpression()));
				node->endModification();
			}
			item->setUpdateNeededForChildItem(Visualization::Item::StandardUpdate, node->annotations()->first());
			item->scene()->addPostEventAction(new Interaction::SetCursorEvent(item, node->annotations()->first()));
	})));
}

}
