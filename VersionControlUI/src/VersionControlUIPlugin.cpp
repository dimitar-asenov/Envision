/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2015 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

#include "DiffManager.h"
#include "items/ObjectPathCrumb.h"
#include "handlers/HObjectPathCrumb.h"
#include "commands/CClear.h"
#include "commands/CDiff.h"
#include "commands/CHistory.h"
#include "VersionControlUIPlugin.h"

#include "Logger/src/Log.h"

#include "SelfTest/src/TestManager.h"

#include "InteractionBase/src/handlers/HSceneHandlerItem.h"
#include "InteractionBase/src/input_actions/ActionRegistry.h"

namespace VersionControlUI {

Logger::Log& VersionControlUIPlugin::log()
{
	static auto log = Logger::Log::getLogger("PLUGIN_NAME_LOWER");
	return *log;
}

bool VersionControlUIPlugin::initialize(Core::EnvisionManager&)
{
	VersionControlUI::ObjectPathCrumb::setDefaultClassHandler(HObjectPathCrumb::instance());

	Interaction::ActionRegistry::instance()->registerInputHandler("GenericHandler.NameChangeFilter",
																					  DiffManager::toggleNameChangeHighlights);

	Interaction::HSceneHandlerItem::instance()->addCommand(new CClear{});
	Interaction::HSceneHandlerItem::instance()->addCommand(new CDiff{});
	Interaction::HSceneHandlerItem::instance()->addCommand(new CHistory{});

	return true;
}

void VersionControlUIPlugin::unload()
{
}

void VersionControlUIPlugin::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<VersionControlUIPlugin>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<VersionControlUIPlugin>::runTest(testid).printResultStatistics();
}

}
