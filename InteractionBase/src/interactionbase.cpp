/***********************************************************************************************************************
 * interactionbase.cpp
 *
 *  Created on: Jan 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "interactionbase.h"
#include "HText.h"

#include "SelfTest/headers/SelfTestSuite.h"

Q_EXPORT_PLUGIN2( interactionbase, Interaction::InteractionBase )

namespace Interaction {

bool InteractionBase::initialize(Envision::EnvisionManager&)
{
	Visualization::Text::setInteractionHandler(HText::instance());
	return true;
}

void InteractionBase::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<InteractionBase>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<InteractionBase>::runTest(testid).printResultStatistics();
}

}
