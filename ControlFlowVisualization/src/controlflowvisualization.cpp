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
 * controlflowvisualization.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "controlflowvisualization.h"
#include "SelfTest/src/SelfTestSuite.h"

#include "allCFVisualizations.h"
#include "handlers/HControlFlowMethodSwitch.h"

#include "OOModel/src/allOOModelNodes.h"

#include "VisualizationBase/src/Scene.h"

#include "InteractionBase/src/handlers/GenericHandler.h"
#include "InteractionBase/src/handlers/HText.h"

Q_EXPORT_PLUGIN2( controlflowvisualization, ControlFlowVisualization::ControlFlowVisualization )

using namespace OOModel;
using namespace Visualization;

namespace ControlFlowVisualization {

bool ControlFlowVisualization::initialize(Envision::EnvisionManager&)
{
	// Register visualizations
	Scene::defaultRenderer()->registerVisualization(Method::typeIdStatic(), createVisualization<VControlFlowMethodSwitch, Method>);
	Scene::defaultRenderer()->registerVisualization(Block::typeIdStatic(), createVisualization<VBlockCF, Block>);
	Scene::defaultRenderer()->registerVisualization(ReturnStatement::typeIdStatic(), createVisualization<VReturnStatementCF, ReturnStatement>);
	Scene::defaultRenderer()->registerVisualization(IfStatement::typeIdStatic(), createVisualization<VIfStatementCF, IfStatement>);
	Scene::defaultRenderer()->registerVisualization(LoopStatement::typeIdStatic(), createVisualization<VLoopStatementCF, LoopStatement>);
//	Scene::defaultRenderer()->registerVisualization(ForEachStatement::typeIdStatic(), createVisualization<VForEachStatementCF, ForEachStatement>);
	Scene::defaultRenderer()->registerVisualization(BreakStatement::typeIdStatic(), createVisualization<VBreakStatementCF, BreakStatement>);
	Scene::defaultRenderer()->registerVisualization(ContinueStatement::typeIdStatic(), createVisualization<VContinueStatementCF, ContinueStatement>);

	// Register handlers
	// TODO: replace this with custom handlers, possibly from OOInteraction Plugin
	// TODO: when you do that fix dependencies in the .plugin meta file
	VListCF::setInteractionHandler(Interaction::GenericHandler::instance());
	VMethodCF::setInteractionHandler(Interaction::GenericHandler::instance());
	VControlFlowMethodSwitch::setInteractionHandler(HControlFlowMethodSwitch::instance());
	VIfStatementCF::setInteractionHandler(Interaction::GenericHandler::instance());
	VLoopStatementCF::setInteractionHandler(Interaction::GenericHandler::instance());
//	VForEachStatementCF::setInteractionHandler(Interaction::GenericHandler::instance());
	VBreakStatementCF::setInteractionHandler(Interaction::GenericHandler::instance());
	VContinueStatementCF::setInteractionHandler(Interaction::GenericHandler::instance());
	VBlockCF::setInteractionHandler(Interaction::GenericHandler::instance());
	VReturnStatementCF::setInteractionHandler(Interaction::GenericHandler::instance());

	return true;
}

void ControlFlowVisualization::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<ControlFlowVisualization>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<ControlFlowVisualization>::runTest(testid).printResultStatistics();
}

}
