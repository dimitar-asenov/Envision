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

#include "handlers/HReviewableItem.h"

#include "CodeReviewPlugin.h"
#include "SelfTest/src/TestManager.h"
#include "Logger/src/Log.h"

#include "overlays/CodeReviewCommentOverlay.h"

#include "handlers/HCodeReviewOverlay.h"

#include "VersionControlUI/src/items/VDiffFrame.h"

#include "commands/CCodeReview.h"

#include "OOVisualization/src/declarations/VClass.h"

#include "InteractionBase/src/handlers/HSceneHandlerItem.h"

namespace CodeReview {

Logger::Log& CodeReviewPlugin::log()
{
	static auto log = Logger::Log::getLogger("PLUGIN_NAME_LOWER");
	return *log;
}

bool CodeReviewPlugin::initialize(Core::EnvisionManager&)
{
	VersionControlUI::VDiffFrame::setDefaultClassHandler(HReviewableItem::instance());
	CodeReviewCommentOverlay::setDefaultClassHandler(HCodeReviewOverlay::instance());

	Interaction::HSceneHandlerItem::instance()->addCommand(new CCodeReview{});

	return true;
}

void CodeReviewPlugin::unload()
{
}

void CodeReviewPlugin::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<CodeReviewPlugin>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<CodeReviewPlugin>::runTest(testid).printResultStatistics();
}

}
