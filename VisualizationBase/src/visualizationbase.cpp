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
 * visualizationbase.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

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
#include "items/VExtendable.h"
#include "items/VList.h"

#include "items/Static.h"
#include "items/Symbol.h"
#include "icons/SVGIcon.h"

#include "node_extensions/Position.h"

#include "ModelBase/src/test_nodes/BinaryNode.h"
#include "SelfTest/src/SelfTestSuite.h"

Q_EXPORT_PLUGIN2( visualizationbase, Visualization::VisualizationBase )

namespace Visualization
{

bool VisualizationBase::initialize(Core::EnvisionManager& manager)
{
	VisualizationManager::init(&manager);
	Shape::registerShape<Box>();
	Shape::registerShape<Diamond>();
	Shape::registerShape<Braces>();
	Shape::registerShape<SvgShape>();

	// Register extensions
	Position::registerExtension();

	// Register visualizations
	Scene::defaultRenderer()->registerVisualization(
			Model::Text::typeIdStatic(), createVisualization<VText, Model::Text>);
	Scene::defaultRenderer()->registerVisualization(
			Model::Integer::typeIdStatic(), createVisualization<VInteger, Model::Integer>);
	Scene::defaultRenderer()->registerVisualization(
			Model::Float::typeIdStatic(), createVisualization<VFloat, Model::Float>);
	Scene::defaultRenderer()->registerVisualization(
			Model::Character::typeIdStatic(), createVisualization<VCharacter, Model::Character>);
	Scene::defaultRenderer()->registerVisualization(
			Model::Boolean::typeIdStatic(), createVisualization<VBoolean, Model::Boolean>);
	Scene::defaultRenderer()->registerVisualization(
			Model::Reference::typeIdStatic(), createVisualization<VReference, Model::Reference>);
	Scene::defaultRenderer()->registerVisualization(
			Model::ExtendableNode::typeIdStatic(), createVisualization<VExtendable, Model::ExtendableNode>);
	Scene::defaultRenderer()->registerVisualization(
			Model::List::typeIdStatic(), createVisualization<VList, Model::List>);

	// Register static visualizations
	Static::registerStaticItem<Symbol>();
	Static::registerStaticItem<SVGIcon>();

	// Enlarge and Center Main Window

	VisualizationManager::instance().getMainWindow()->resize(1180,700);
	QRect descktop( QApplication::desktop()->screenGeometry() );
	int leftPos = descktop.width()/2-VisualizationManager::instance().getMainWindow()->width()/2;
	int topPos = descktop.height()/2-VisualizationManager::instance().getMainWindow()->height()/2;
	VisualizationManager::instance().getMainWindow()->move(leftPos,topPos);

	//VisualizationManager::instance().getMainWindow()->showFullScreen();

	return true;
}

void VisualizationBase::unload()
{
}

void VisualizationBase::selfTest(QString)
{
	TestNodes::BinaryNode::init();
	SelfTest::TestManager<VisualizationBase>::runAllTests().printResultStatistics();
}

}
