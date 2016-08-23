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

#include "InteractionBasePlugin.h"

#include "handlers/GenericHandler.h"
#include "handlers/HText.h"
#include "handlers/HList.h"
#include "handlers/HComposite.h"
#include "handlers/HActionPrompt.h"
#include "handlers/HSceneHandlerItem.h"
#include "handlers/HPositionLayout.h"
#include "handlers/HRootItem.h"
#include "handlers/HInfoNode.h"
#include "handlers/HViewItem.h"
#include "handlers/HWebBrowserItem.h"
#include "handlers/HArrowHandler.h"

#include "vis/TextAndDescription.h"
#include "vis/VViewSwitcherEntry.h"
#include "vis/Menu.h"
#include "actions/ActionPrompt.h"
#include "input_actions/ActionRegistry.h"

#include "events/DetectMainSceneActivated.h"

#include "prompt/CommandMode.h"
#include "prompt/Prompt.h"
#include "prompt/PromptShell.h"
#include "prompt/HPromptShell.h"

#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/items/SceneHandlerItem.h"
#include "VisualizationBase/src/items/VComposite.h"
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
#include "VisualizationBase/src/items/VInfoNode.h"
#include "VisualizationBase/src/icons/SVGIcon.h"
#include "VisualizationBase/src/layouts/SequentialLayout.h"
#include "VisualizationBase/src/layouts/PositionLayout.h"
#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/items/ViewItem.h"
#include "VisualizationBase/src/items/WebBrowserItem.h"
#include "VisualizationBase/src/overlays/MessageOverlay.h"
#include "VisualizationBase/src/overlays/ArrowOverlay.h"

#include "SelfTest/src/TestManager.h"
#include "Logger/src/Log.h"

using namespace Logger;

namespace Interaction {

Log& InteractionBasePlugin::log()
{
	static auto log = Logger::Log::getLogger("interactionbase");
	return *log;
}

bool InteractionBasePlugin::initialize(Core::EnvisionManager& envisionManager)
{
	// Register prompt modes
	Prompt::registerMode<CommandMode>(Prompt::defaultModeName());
	PromptShell::setDefaultClassHandler(HPromptShell::instance());
	Prompt::registerPromptShowShortcut(Qt::Key_F, [](Visualization::Item* target){Prompt::show(target, "find ");});
	// Register prompt scale toggle
	ActionRegistry::instance()->registerInputHandler("GenericHandler.TogglePromptScale", Prompt::toggleScale);

	Visualization::Item::setDefaultClassHandler(GenericHandler::instance());
	Visualization::TextRenderer::setDefaultClassHandler(HText::instance());
	Visualization::SceneHandlerItem::setDefaultClassHandler(HSceneHandlerItem::instance());
	Visualization::VComposite::setDefaultClassHandler(HComposite::instance());
	Visualization::VList::setDefaultClassHandler(HList::instance());
	Visualization::RootItem::setDefaultClassHandler(HRootItem::instance());
	Visualization::PositionLayout::setDefaultClassHandler(HPositionLayout::instance());
	Visualization::VInfoNode::setDefaultClassHandler(HInfoNode::instance());
	Visualization::ViewItem::setDefaultClassHandler(HViewItem::instance());
	Visualization::WebBrowserItem::setDefaultClassHandler(HWebBrowserItem::instance());
	Visualization::MessageOverlay::setDefaultClassHandler(HMovableItem::instance());
	Visualization::ArrowOverlay::setDefaultClassHandler(HArrowHandler::instance());
	ActionPrompt::setDefaultClassHandler(HActionPrompt::instance());

	// We use to show the prompt. It can only be shown once the Scene is activated.
	if (!envisionManager.areSelfTestsPending())
	{
		auto mainScene = Visualization::VisualizationManager::instance().mainScene();
		mainScene->installEventFilter(new DetectMainSceneActivated{});
	}

	return true;
}

void InteractionBasePlugin::unload()
{
}

void InteractionBasePlugin::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<InteractionBasePlugin>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<InteractionBasePlugin>::runTest(testid).printResultStatistics();
}

}
