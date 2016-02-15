/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "ControlFlowVisualizationPlugin.h"
#include "SelfTest/src/TestManager.h"

#include "allCFVisualizations.h"

#include "OOModel/src/allOOModelNodes.h"

#include "VisualizationBase/src/Scene.h"

using namespace OOModel;
using namespace Visualization;

namespace ControlFlowVisualization {

int ControlFlowVisualizationPlugin::visualizationPurpose()
{
	static int purpose = Scene::defaultRenderer()->registerVisualizationPurpose("control_flow");
	return purpose;
}

bool ControlFlowVisualizationPlugin::initialize(Core::EnvisionManager&)
{
	// Register controlflow purpose
	visualizationPurpose();

	// Register visualizations
	Core::TypeRegistry::initializeNewTypes();
	Scene::defaultRenderer()->registerVisualization(StatementItemList::typeIdStatic(), visualizationPurpose(),
			createVisualization<VListCF, StatementItemList>, VListCF::typeIdStatic());
	Scene::defaultRenderer()->registerVisualization(Block::typeIdStatic(), visualizationPurpose(),
			createVisualization<VBlockCF, Block>, VBlockCF::typeIdStatic());
	Scene::defaultRenderer()->registerVisualization(ReturnStatement::typeIdStatic(), visualizationPurpose(),
			createVisualization<VReturnStatementCF, ReturnStatement>, VReturnStatementCF::typeIdStatic());
	Scene::defaultRenderer()->registerVisualization(IfStatement::typeIdStatic(), visualizationPurpose(),
			createVisualization<VIfStatementCF, IfStatement>, VIfStatementCF::typeIdStatic());
	Scene::defaultRenderer()->registerVisualization(LoopStatement::typeIdStatic(), visualizationPurpose(),
			createVisualization<VLoopStatementCF, LoopStatement>, VLoopStatementCF::typeIdStatic());
//	Scene::defaultRenderer()->registerVisualization(ForEachStatement::typeIdStatic(), visualizationPurpose(),
//			createVisualization<VForEachStatementCF, ForEachStatement>, VForEachStatementCF::typeIdStatic());
	Scene::defaultRenderer()->registerVisualization(BreakStatement::typeIdStatic(), visualizationPurpose(),
			createVisualization<VBreakStatementCF, BreakStatement>, VBreakStatementCF::typeIdStatic());
	Scene::defaultRenderer()->registerVisualization(ContinueStatement::typeIdStatic(), visualizationPurpose(),
			createVisualization<VContinueStatementCF, ContinueStatement>, VContinueStatementCF::typeIdStatic());

	// Register handlers
	// TODO: register custom handlers, possibly from OOInteraction Plugin

	return true;
}

void ControlFlowVisualizationPlugin::unload()
{
}

void ControlFlowVisualizationPlugin::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<ControlFlowVisualizationPlugin>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<ControlFlowVisualizationPlugin>::runTest(testid).printResultStatistics();
}

}
