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
#include "Scene.h"
#include "items/VText.h"
#include "items/VInteger.h"
#include "items/VCharacter.h"
#include "items/VBoolean.h"
#include "items/VFloat.h"
#include "items/VReference.h"
#include "items/VExtendable.h"
#include "items/VList.h"

#include "node_extensions/Position.h"

#include "ModelBase/headers/test_nodes/BinaryNode.h"
#include "SelfTest/headers/SelfTestSuite.h"

#include <QtGui/QDesktopWidget>
#include <QtGui/QApplication>

Q_EXPORT_PLUGIN2( visualizationbase, Visualization::VisualizationBase )

namespace Visualization
{

	bool VisualizationBase::initialize(Envision::EnvisionManager& manager)
	{
		VisualizationManager::init(&manager);
		Shape::registerShape<Box>();
		Shape::registerShape<Diamond>();

		// Register extensions
		Position::registerExtension();

		// Register visualizations
		Scene::defaultRenderer()->registerVisualization(Model::Text::typeIdStatic(), createVisualization<VText, Model::Text>);
		Scene::defaultRenderer()->registerVisualization(Model::Integer::typeIdStatic(), createVisualization<VInteger, Model::Integer>);
		Scene::defaultRenderer()->registerVisualization(Model::Float::typeIdStatic(), createVisualization<VFloat, Model::Float>);
		Scene::defaultRenderer()->registerVisualization(Model::Character::typeIdStatic(), createVisualization<VCharacter, Model::Character>);
		Scene::defaultRenderer()->registerVisualization(Model::Boolean::typeIdStatic(), createVisualization<VBoolean, Model::Boolean>);
		Scene::defaultRenderer()->registerVisualization(Model::Reference::typeIdStatic(), createVisualization<VReference, Model::Reference>);
		Scene::defaultRenderer()->registerVisualization(Model::ExtendableNode::typeIdStatic(), createVisualization<VExtendable, Model::ExtendableNode>);
		Scene::defaultRenderer()->registerVisualization(Model::List::typeIdStatic(), createVisualization<VList, Model::List>);

		// Enlarge and Center Main Window
		VisualizationManager::instance().getMainWindow()->resize(1200,700);
		QRect descktop( QApplication::desktop()->screenGeometry() );
		int leftPos = descktop.width()/2-VisualizationManager::instance().getMainWindow()->width()/2;
		int topPos = descktop.height()/2-VisualizationManager::instance().getMainWindow()->height()/2;
		VisualizationManager::instance().getMainWindow()->move(leftPos,topPos);

		return true;
	}

	void VisualizationBase::selfTest(QString)
	{
		TestNodes::BinaryNode::init();
		SelfTest::TestManager<VisualizationBase>::runAllTests().printResultStatistics();
	}

}
