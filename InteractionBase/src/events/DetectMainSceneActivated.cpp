/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

#include "DetectMainSceneActivated.h"

#include "SetCursorEvent.h"
#include "ShowCommandPromptEvent.h"

#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/CustomSceneEvent.h"
#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/items/SceneHandlerItem.h"

namespace Interaction {

bool DetectMainSceneActivated::eventFilter(QObject* /*watched*/, QEvent* event)
{
	if (event->type() != QEvent::WindowActivate) return false;

	// We use this roundabout way to show the prompt. It can only be shown once the Scene is activated.

	auto mainScene = Visualization::VisualizationManager::instance().mainScene();
	QApplication::postEvent(mainScene, new SetCursorEvent(mainScene->sceneHandlerItem()));
	QApplication::postEvent(mainScene, new ShowCommandPromptEvent{mainScene});
	QApplication::postEvent(mainScene, new Visualization::CustomSceneEvent([=](){delete this;}));

	mainScene->removeEventFilter(this);
	return false;
}

}
