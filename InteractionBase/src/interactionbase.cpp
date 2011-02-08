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
#include "handlers/HPositionLayout.h"

#include "vis/CommandPrompt.h"

#include "VisualizationBase/headers/items/SceneHandlerItem.h"
#include "VisualizationBase/headers/items/VExtendable.h"
#include "VisualizationBase/headers/items/VList.h"
#include "VisualizationBase/headers/items/Text.h"
#include "VisualizationBase/headers/items/VText.h"
#include "VisualizationBase/headers/items/VInteger.h"
#include "VisualizationBase/headers/items/VReference.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/layouts/PanelLayout.h"
#include "VisualizationBase/headers/layouts/PanelBorderLayout.h"
#include "VisualizationBase/headers/layouts/PositionLayout.h"
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
	Visualization::VInteger::setInteractionHandler(HText::instance());
	Visualization::VReference::setInteractionHandler(HText::instance());
	Visualization::SequentialLayout::setInteractionHandler(GenericHandler::instance());
	Visualization::PanelLayout::setInteractionHandler(GenericHandler::instance());
	Visualization::PanelBorderLayout::setInteractionHandler(GenericHandler::instance());
	Visualization::PositionLayout::setInteractionHandler(HPositionLayout::instance());
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
