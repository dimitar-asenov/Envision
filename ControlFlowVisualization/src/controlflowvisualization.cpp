/***********************************************************************************************************************
 * controlflowvisualization.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "controlflowvisualization.h"
#include "SelfTest/headers/SelfTestSuite.h"

#include "allCFVisualizations.h"
#include "OOModel/headers/allOOModelNodes.h"

#include "VisualizationBase/headers/Scene.h"

#include "InteractionBase/headers/handlers/GenericHandler.h"
#include "InteractionBase/headers/handlers/HText.h"

Q_EXPORT_PLUGIN2( controlflowvisualization, ControlFlowVisualization::ControlFlowVisualization )

using namespace OOModel;
using namespace Visualization;

namespace ControlFlowVisualization {

bool ControlFlowVisualization::initialize(Envision::EnvisionManager&)
{
	// Register visualizations
	Scene::defaultRenderer()->registerVisualization(Method::typeIdStatic(), createVisualization<VMethodCF, Method>);
//	Scene::defaultRenderer()->registerVisualization(Block::typeIdStatic(), createVisualization<VBlock, Block>);
//	Scene::defaultRenderer()->registerVisualization(ReturnStatement::typeIdStatic(), createVisualization<VReturnStatement, ReturnStatement>);
//	Scene::defaultRenderer()->registerVisualization(IfStatement::typeIdStatic(), createVisualization<VIfStatement, IfStatement>);
//	Scene::defaultRenderer()->registerVisualization(LoopStatement::typeIdStatic(), createVisualization<VLoopStatement, LoopStatement>);
//	Scene::defaultRenderer()->registerVisualization(ForEachStatement::typeIdStatic(), createVisualization<VForEachStatement, ForEachStatement>);
//	Scene::defaultRenderer()->registerVisualization(BreakStatement::typeIdStatic(), createVisualization<VBreakStatement, BreakStatement>);
//	Scene::defaultRenderer()->registerVisualization(ContinueStatement::typeIdStatic(), createVisualization<VContinueStatement, ContinueStatement>);

	// Register handlers
	// TODO: replace this with custom handlers, possibly from OOInteraction Plugin
	// TODO: when you do that fix dependencies in the .plugin meta file
	VMethodCF::setInteractionHandler(Interaction::GenericHandler::instance());
//	VIfStatement::setInteractionHandler(Interaction::GenericHandler::instance());
//	VLoopStatement::setInteractionHandler(Interaction::GenericHandler::instance());
//	VForEachStatement::setInteractionHandler(Interaction::GenericHandler::instance());
//	VBreakStatement::setInteractionHandler(Interaction::GenericHandler::instance());
//	VContinueStatement::setInteractionHandler(Interaction::GenericHandler::instance());
//	VBlock::setInteractionHandler(Interaction::GenericHandler::instance());
//	VReturnStatement::setInteractionHandler(Interaction::GenericHandler::instance());

	return true;
}

void ControlFlowVisualization::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<ControlFlowVisualization>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<ControlFlowVisualization>::runTest(testid).printResultStatistics();
}

}
