/***********************************************************************************************************************
 * interactionbase.cpp
 *
 *  Created on: Jan 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "interactionbase.h"

#include "handlers/GenericHandler.h"
#include "handlers/HText.h"
#include "handlers/HList.h"
#include "handlers/HExtendable.h"
#include "handlers/HCommandPrompt.h"
#include "handlers/HSceneHandlerItem.h"

#include "vis/CommandPrompt.h"

#include "VisualizationBase/headers/items/SceneHandlerItem.h"
#include "VisualizationBase/headers/items/VExtendable.h"
#include "VisualizationBase/headers/items/VList.h"
#include "VisualizationBase/headers/items/Text.h"
#include "VisualizationBase/headers/items/VText.h"
#include "ModelBase/headers/test_nodes/BinaryNode.h"

#include "SelfTest/headers/SelfTestSuite.h"

using namespace Logger;

Q_EXPORT_PLUGIN2( interactionbase, Interaction::InteractionBase )

namespace Interaction {

Log* InteractionBase::logger = NULL;

bool InteractionBase::initialize(Envision::EnvisionManager&)
{
	logger = Logger::Log::getLogger("interactionbase");
	Visualization::SceneHandlerItem::setInteractionHandler(HSceneHandlerItem::instance());
	Visualization::VExtendable::setInteractionHandler(HExtendable::instance());
	Visualization::VList::setInteractionHandler(HList::instance());
	Visualization::Text::setInteractionHandler(HText::instance());
	Visualization::VText::setInteractionHandler(HText::instance());
	CommandPrompt::setInteractionHandler(HCommandPrompt::instance());
	return true;
}

void InteractionBase::selfTest(QString testid)
{
	TestNodes::BinaryNode::init();

	if (testid.isEmpty()) SelfTest::TestManager<InteractionBase>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<InteractionBase>::runTest(testid).printResultStatistics();
}

Log* InteractionBase::log()
{
	return logger;
}

}
