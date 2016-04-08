/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

#include "OOInteractionPlugin.h"
#include "SelfTest/src/TestManager.h"

#include "expression_editor/OOOperatorDescriptorList.h"
#include "handlers/HProject.h"
#include "handlers/HModule.h"
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
#include "handlers/HArrayInitializer.h"
#include "handlers/HStatementItemList.h"

#include "commands/CCreateProject.h"
#include "commands/CCreateModule.h"
#include "commands/CCreateClass.h"
#include "commands/CCreateMethod.h"
#include "commands/CSceneHandlerItemTest.h"
#include "commands/CDoxygen.h"
#include "commands/CAddCalleesToView.h"
#include "commands/CAddBaseClassesToView.h"
#include "commands/CAddCallersToView.h"
#include "commands/CAddSubClassesToView.h"
#include "commands/CInspectMethodInView.h"

#include "string_offset_providers/StringComponents.h"

#include "string_offset_providers/GridConstructors.h"
#include "string_offset_providers/GridBasedOffsetProvider.h"
#include "string_offset_providers/EmptyExpressionStringOffsetProvider.h"
#include "string_offset_providers/TextRendererStringOffsetProvider.h"
#include "string_offset_providers/StaticStringOffsetProvider.h"
#include "string_offset_providers/InitializerStringOffsetProvider.h"
#include "string_offset_providers/CompoundObjectStringOffsetProvider.h"

#include "customization/CustomizationVisitor.h"

#include "OOVisualization/src/allOOVisualizations.h"

#include "OOModel/src/allOOModelNodes.h"

#include "InteractionBase/src/handlers/GenericHandler.h"
#include "InteractionBase/src/handlers/HList.h"
#include "InteractionBase/src/handlers/HText.h"
#include "InteractionBase/src/handlers/HSceneHandlerItem.h"

#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/items/Symbol.h"
#include "VisualizationBase/src/items/Text.h"
#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/VisualizationManager.h"

#include "OOInteraction/src/DoxygenWholeTreeVisitor.h"
#include "OOInteraction/src/DoxygenCommentsOnlyVisitor.h"

#include "Core/src/AdapterManager.h"

#include "Logger/src/Log.h"

namespace OOInteraction {

Logger::Log& OOInteractionPlugin::log()
{
	static auto log = Logger::Log::getLogger("oointeraction");
	return *log;
}

bool OOInteractionPlugin::initialize(Core::EnvisionManager&)
{
	OOOperatorDescriptorList::initializeWithDefaultOperators();

	OOVisualization::VExpressionStaticData::setDefaultClassHandler(HExpression::instance());
	OOVisualization::VStatementItemStaticData::setDefaultClassHandler(HStatement::instance());
	OOVisualization::VProject::setDefaultClassHandler(HProject::instance());
	OOVisualization::VModule::setDefaultClassHandler(HModule::instance());
	OOVisualization::VClass::setDefaultClassHandler(HClass::instance());
	OOVisualization::VMethod::setDefaultClassHandler(HMethod::instance());
	OOVisualization::VFormalArgument::setDefaultClassHandler(HFormalArgument::instance());
	OOVisualization::VIfStatement::setDefaultClassHandler(HIfStatement::instance());
	OOVisualization::VLoopStatement::setDefaultClassHandler(HLoop::instance());
	OOVisualization::VForEachStatement::setDefaultClassHandler(HForEachStatement::instance());
	OOVisualization::VBreakStatement::setDefaultClassHandler(HKeywordStatement::instance());
	OOVisualization::VContinueStatement::setDefaultClassHandler(HKeywordStatement::instance());
	OOVisualization::VReturnStatement::setDefaultClassHandler(HReturnStatement::instance());
	OOVisualization::VArrayInitializer::setDefaultClassHandler(HArrayInitializer::instance());
	OOVisualization::VStatementItemList::setDefaultClassHandler(HStatementItemList::instance());

	//Enable the default grid offset string provider for all expressions:
	StringOffsetProvider::allowGridBasedProvider([](Visualization::Item* item)
		{ return DCast<OOModel::Expression>(item->node());});

	// Register string components that convert an expression to a string list representing its components
	StringComponents::initConversions();
	// For debugging purposes, also register a method that converts any node to a string
	Core::AdapterManager::registerDefaultAdapter<Model::NodeToDebugStringAdapter>(nodeToDebugString);

	// Register default string offset providers
	GridConstructors::initializeAll();
	Core::AdapterManager::registerDefaultAdapter<StringOffsetProvider>(StringOffsetProvider::defaultProvider);

	// Register custom string offset providers
	Core::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, InitializerStringOffsetProvider, OOVisualization::VArrayInitializer>();
	Core::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, EmptyExpressionStringOffsetProvider, OOVisualization::VEmptyExpression>();
	Core::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, TextRendererStringOffsetProvider, Visualization::Text>();
	Core::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, TextRendererStringOffsetProvider, Visualization::VText>();
	Core::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, TextRendererStringOffsetProvider, Visualization::Symbol>();
	Core::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, StaticStringOffsetProvider, Visualization::Static>();
	Core::AdapterManager::registerAdapterViaConstructor
		<StringOffsetProvider, CompoundObjectStringOffsetProvider, OOVisualization::VLambdaExpression>();
	Core::AdapterManager::registerAdapter<StringOffsetProvider, Visualization::NodeWrapper>(
		[](Visualization::NodeWrapper* wrapper){
			return Core::AdapterManager::adapt<StringOffsetProvider>(wrapper->wrappedItem());
	});

	Interaction::HSceneHandlerItem::instance()->addCommand(new CCreateProject{});
	Interaction::HSceneHandlerItem::instance()->addCommand(new CCreateModule{});
	Interaction::HSceneHandlerItem::instance()->addCommand(new CCreateClass{});
	Interaction::HSceneHandlerItem::instance()->addCommand(new CCreateMethod{});
	Interaction::HSceneHandlerItem::instance()->addCommand(new CSceneHandlerItemTest{});
	Interaction::HSceneHandlerItem::instance()->addCommand(new CDoxygen{});
	Interaction::HSceneHandlerItem::instance()->addCommand(new CAddCalleesToView{});
	Interaction::HSceneHandlerItem::instance()->addCommand(new CAddBaseClassesToView{});
	Interaction::HSceneHandlerItem::instance()->addCommand(new CAddCallersToView{});
	Interaction::HSceneHandlerItem::instance()->addCommand(new CAddSubClassesToView{});
	Interaction::HSceneHandlerItem::instance()->addCommand(new CInspectMethodInView{});

	// Initialize customization support
	auto customizationGroup = new Visualization::VisualizationGroup{};
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

	OOInteraction::DoxygenWholeTreeVisitor::init();
	OOInteraction::DoxygenCommentsOnlyVisitor::init();

	return true;
}

void OOInteractionPlugin::unload()
{
}

void OOInteractionPlugin::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<OOInteractionPlugin>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<OOInteractionPlugin>::runTest(testid).printResultStatistics();
}

Model::NodeToDebugStringAdapter* OOInteractionPlugin::nodeToDebugString(Model::Node* node)
{
	Q_ASSERT(node);

	auto ntds = new ::Model::NodeToDebugStringAdapter{};
	if (DCast<OOModel::Expression>(node))
		ntds->str = StringComponents::stringForNode(node);
	else
		ntds->str = "Can't convert node type to QString: " + node->typeName();

	return ntds;
}

}
