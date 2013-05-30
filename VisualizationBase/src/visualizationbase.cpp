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

#include "visualizationbase.h"

#include "VisualizationManager.h"
#include "shapes/Box.h"
#include "shapes/Diamond.h"
#include "shapes/Braces.h"
#include "shapes/SvgShape.h"
#include "Scene.h"
#include "items/VText.h"
#include "items/VInteger.h"
#include "items/VCharacter.h"
#include "items/VBoolean.h"
#include "items/VFloat.h"
#include "items/VReference.h"
#include "items/VComposite.h"
#include "items/VList.h"

#include "items/Static.h"
#include "items/Symbol.h"
#include "items/Line.h"
#include "items/TestBox.h"
#include "icons/SVGIcon.h"

#include "node_extensions/Position.h"
#include "nodes/TestBoxNode.h"

#include "ModelBase/src/test_nodes/TestNodesInitializer.h"
#include "SelfTest/src/SelfTestSuite.h"

Q_EXPORT_PLUGIN2( visualizationbase, Visualization::VisualizationBase )

namespace Visualization
{

Core::InitializationRegistry& nodeTypeInitializationRegistry()
{
	static Core::InitializationRegistry r;
	return r;
}

Core::InitializationRegistry& itemTypeInitializationRegistry()
{
	static Core::InitializationRegistry r;
	return r;
}

Core::InitializationRegistry& shapeTypeInitializationRegistry()
{
	static Core::InitializationRegistry r;
	return r;
}

bool VisualizationBase::initialize(Core::EnvisionManager& manager)
{
	VisualizationManager& vmi = VisualizationManager::instance();
	vmi.init(&manager);

	shapeTypeInitializationRegistry().initializeAll();

	// Register extensions and nodes
	Position::registerExtension();
	nodeTypeInitializationRegistry().initializeAll();

	// Register visualizations
	itemTypeInitializationRegistry().initializeAll();
	Scene::defaultRenderer()->registerVisualization(
			TestBoxNode::typeIdStatic(), createVisualization<TestBox, TestBoxNode>);

	// Register static visualizations
	Static::registerStaticItem<Symbol>();
	Static::registerStaticItem<SVGIcon>();
	Static::registerStaticItem<Line>();

	// Enlarge and Center Main Window

	vmi.getMainWindow()->resize(1180,700);
	QRect descktop( QApplication::desktop()->screenGeometry() );
	int leftPos = descktop.width()/2-VisualizationManager::instance().getMainWindow()->width()/2;
	int topPos = descktop.height()/2-VisualizationManager::instance().getMainWindow()->height()/2;
	vmi.getMainWindow()->move(leftPos,topPos);

	//VisualizationManager::instance().getMainWindow()->showFullScreen();

	return true;
}

void VisualizationBase::unload()
{
	VisualizationManager::instance().cleanup();
}

void VisualizationBase::selfTest(QString)
{
	TestNodes::nodeTypeInitializationRegistry().initializeAll();
	SelfTest::TestManager<VisualizationBase>::runAllTests().printResultStatistics();
}

}
