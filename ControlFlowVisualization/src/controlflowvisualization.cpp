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

#include "controlflowvisualization.h"
#include "SelfTest/src/SelfTestSuite.h"

#include "allCFVisualizations.h"

#include "OOModel/src/allOOModelNodes.h"

#include "VisualizationBase/src/Scene.h"

Q_EXPORT_PLUGIN2( controlflowvisualization, ControlFlowVisualization::ControlFlowVisualization )

using namespace OOModel;
using namespace Visualization;

namespace ControlFlowVisualization {

Core::InitializationRegistry& itemTypeInitializationRegistry()
{
	static Core::InitializationRegistry r;
	return r;
}

int ControlFlowVisualization::visualizationPurpose()
{
	static int purpose = Scene::defaultRenderer()->registerVisualizationPurpose("control_flow");
	return purpose;
}

bool ControlFlowVisualization::initialize(Core::EnvisionManager&)
{
	// Register controlflow purpose
	visualizationPurpose();

	// Register visualizations
	itemTypeInitializationRegistry().initializeAll();
	Scene::defaultRenderer()->registerVisualization(Method::typeIdStatic(), visualizationPurpose(),
			createVisualization<VMethodCF, Method>);
	Scene::defaultRenderer()->registerVisualization(Block::typeIdStatic(), visualizationPurpose(),
			createVisualization<VBlockCF, Block>);
	Scene::defaultRenderer()->registerVisualization(ReturnStatement::typeIdStatic(), visualizationPurpose(),
			createVisualization<VReturnStatementCF, ReturnStatement>);
	Scene::defaultRenderer()->registerVisualization(IfStatement::typeIdStatic(), visualizationPurpose(),
			createVisualization<VIfStatementCF, IfStatement>);
	Scene::defaultRenderer()->registerVisualization(LoopStatement::typeIdStatic(), visualizationPurpose(),
			createVisualization<VLoopStatementCF, LoopStatement>);
//	Scene::defaultRenderer()->registerVisualization(ForEachStatement::typeIdStatic(), visualizationPurpose(),
//			createVisualization<VForEachStatementCF, ForEachStatement>);
	Scene::defaultRenderer()->registerVisualization(BreakStatement::typeIdStatic(), visualizationPurpose(),
			createVisualization<VBreakStatementCF, BreakStatement>);
	Scene::defaultRenderer()->registerVisualization(ContinueStatement::typeIdStatic(), visualizationPurpose(),
			createVisualization<VContinueStatementCF, ContinueStatement>);

	// Register handlers
	// TODO: register custom handlers, possibly from OOInteraction Plugin

	return true;
}

void ControlFlowVisualization::unload()
{
}

void ControlFlowVisualization::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<ControlFlowVisualization>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<ControlFlowVisualization>::runTest(testid).printResultStatistics();
}

}
