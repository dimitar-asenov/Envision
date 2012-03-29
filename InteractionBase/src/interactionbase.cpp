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

#include "VisualizationBase/src/items/SceneHandlerItem.h"
#include "VisualizationBase/src/items/VExtendable.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/items/Text.h"
#include "VisualizationBase/src/items/Symbol.h"
#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/items/VInteger.h"
#include "VisualizationBase/src/items/VFloat.h"
#include "VisualizationBase/src/items/VCharacter.h"
#include "VisualizationBase/src/items/VBoolean.h"
#include "VisualizationBase/src/items/VReference.h"
#include "VisualizationBase/src/icons/SVGIcon.h"
#include "VisualizationBase/src/layouts/SequentialLayout.h"
#include "VisualizationBase/src/layouts/PanelLayout.h"
#include "VisualizationBase/src/layouts/PanelBorderLayout.h"
#include "VisualizationBase/src/layouts/PositionLayout.h"
#include "ModelBase/src/test_nodes/BinaryNode.h"

#include "SelfTest/src/SelfTestSuite.h"

using namespace Logger;

Q_EXPORT_PLUGIN2( interactionbase, Interaction::InteractionBase )

namespace Interaction {

Log* InteractionBase::logger = nullptr;

bool InteractionBase::initialize(Envision::EnvisionManager&)
{
	logger = Logger::Log::getLogger("interactionbase");
	Visualization::SceneHandlerItem::setInteractionHandler(HSceneHandlerItem::instance());
	Visualization::VExtendable::setInteractionHandler(HExtendable::instance());
	Visualization::VList::setInteractionHandler(HList::instance());
	Visualization::Text::setInteractionHandler(HText::instance());
	Visualization::Symbol::setInteractionHandler(HText::instance());
	Visualization::VText::setInteractionHandler(HText::instance());
	Visualization::VInteger::setInteractionHandler(HText::instance());
	Visualization::VFloat::setInteractionHandler(HText::instance());
	Visualization::VCharacter::setInteractionHandler(HText::instance());
	Visualization::VBoolean::setInteractionHandler(HText::instance());
	Visualization::VReference::setInteractionHandler(HText::instance());
	Visualization::SVGIcon::setInteractionHandler(GenericHandler::instance());
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
