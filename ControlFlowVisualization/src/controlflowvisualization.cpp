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
//	Scene::defaultRenderer()->registerVisualization(Block::typeIdStatic(), createVisualization<VBlockCF, Block>);
	Scene::defaultRenderer()->registerVisualization(ReturnStatement::typeIdStatic(), createVisualization<VReturnStatementCF, ReturnStatement>);
	Scene::defaultRenderer()->registerVisualization(IfStatement::typeIdStatic(), createVisualization<VIfStatementCF, IfStatement>);
	Scene::defaultRenderer()->registerVisualization(LoopStatement::typeIdStatic(), createVisualization<VLoopStatementCF, LoopStatement>);
//	Scene::defaultRenderer()->registerVisualization(ForEachStatement::typeIdStatic(), createVisualization<VForEachStatementCF, ForEachStatement>);
	Scene::defaultRenderer()->registerVisualization(BreakStatement::typeIdStatic(), createVisualization<VBreakStatementCF, BreakStatement>);
	Scene::defaultRenderer()->registerVisualization(ContinueStatement::typeIdStatic(), createVisualization<VContinueStatementCF, ContinueStatement>);

	// Register handlers
	// TODO: replace this with custom handlers, possibly from OOInteraction Plugin
	// TODO: when you do that fix dependencies in the .plugin meta file
	VMethodCF::setInteractionHandler(Interaction::GenericHandler::instance());
	VIfStatementCF::setInteractionHandler(Interaction::GenericHandler::instance());
	VLoopStatementCF::setInteractionHandler(Interaction::GenericHandler::instance());
//	VForEachStatementCF::setInteractionHandler(Interaction::GenericHandler::instance());
	VBreakStatementCF::setInteractionHandler(Interaction::GenericHandler::instance());
	VContinueStatementCF::setInteractionHandler(Interaction::GenericHandler::instance());
//	VBlockCF::setInteractionHandler(Interaction::GenericHandler::instance());
	VReturnStatementCF::setInteractionHandler(Interaction::GenericHandler::instance());

	return true;
}

void ControlFlowVisualization::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<ControlFlowVisualization>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<ControlFlowVisualization>::runTest(testid).printResultStatistics();
}

}
