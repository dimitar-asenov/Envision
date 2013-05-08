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

#include "interactionbase.h"

#include "handlers/GenericHandler.h"
#include "handlers/HText.h"
#include "handlers/HList.h"
#include "handlers/HExtendable.h"
#include "handlers/HCommandPrompt.h"
#include "handlers/HActionPrompt.h"
#include "handlers/HSceneHandlerItem.h"
#include "handlers/HPositionLayout.h"
#include "handlers/HRootItem.h"

#include "vis/CommandPrompt.h"
#include "vis/TextAndDescription.h"
#include "actions/ActionPrompt.h"

#include "events/DetectMainSceneActivated.h"

#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/items/SceneHandlerItem.h"
#include "VisualizationBase/src/items/VExtendable.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/items/Text.h"
#include "VisualizationBase/src/items/Symbol.h"
#include "VisualizationBase/src/items/Line.h"
#include "VisualizationBase/src/items/TestBox.h"
#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/items/VInteger.h"
#include "VisualizationBase/src/items/VFloat.h"
#include "VisualizationBase/src/items/VCharacter.h"
#include "VisualizationBase/src/items/VBoolean.h"
#include "VisualizationBase/src/items/VReference.h"
#include "VisualizationBase/src/items/RootItem.h"
#include "VisualizationBase/src/icons/SVGIcon.h"
#include "VisualizationBase/src/layouts/SequentialLayout.h"
#include "VisualizationBase/src/layouts/PanelLayout.h"
#include "VisualizationBase/src/layouts/PanelBorderLayout.h"
#include "VisualizationBase/src/layouts/PositionLayout.h"
#include "VisualizationBase/src/VisualizationManager.h"

#include "ModelBase/src/test_nodes/TestNodesInitializer.h"

#include "SelfTest/src/SelfTestSuite.h"

using namespace Logger;

Q_EXPORT_PLUGIN2( interactionbase, Interaction::InteractionBase )

namespace Interaction {

Log* InteractionBase::logger = nullptr;

Core::InitializationRegistry& itemTypeInitializationRegistry()
{
	static Core::InitializationRegistry r;
	return r;
}

bool InteractionBase::initialize(Core::EnvisionManager&)
{
	logger = Logger::Log::getLogger("interactionbase");
	Visualization::Item::setDefaultClassHandler(GenericHandler::instance());
	Visualization::TextRenderer::setDefaultClassHandler(HText::instance());
	Visualization::SceneHandlerItem::setDefaultClassHandler(HSceneHandlerItem::instance());
	Visualization::VExtendable::setDefaultClassHandler(HExtendable::instance());
	Visualization::VList::setDefaultClassHandler(HList::instance());
	Visualization::RootItem::setDefaultClassHandler(HRootItem::instance());
	Visualization::PositionLayout::setDefaultClassHandler(HPositionLayout::instance());
	CommandPrompt::setDefaultClassHandler(HCommandPrompt::instance());
	ActionPrompt::setDefaultClassHandler(HActionPrompt::instance());

	// We use to show the prompt. It can only be shown once the Scene is activated.
	auto mainScene = Visualization::VisualizationManager::instance().mainScene();
	mainScene->installEventFilter(new DetectMainSceneActivated());

	return true;
}

void InteractionBase::unload()
{
}

void InteractionBase::selfTest(QString testid)
{
	TestNodes::nodeTypeInitializationRegistry().initializeAll();

	if (testid.isEmpty()) SelfTest::TestManager<InteractionBase>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<InteractionBase>::runTest(testid).printResultStatistics();
}

Log* InteractionBase::log()
{
	return logger;
}

}
