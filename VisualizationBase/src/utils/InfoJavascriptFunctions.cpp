/***********************************************************************************************************************
**
** Copyright (c) 2015 ETH Zurich
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

#include "InfoJavascriptFunctions.h"

#include "ModelBase/src/util/NameResolver.h"
#include "ModelBase/src/nodes/Node.h"
#include "VisualizationManager.h"
#include "Scene.h"
#include "items/ViewItem.h"
#include "views/MainView.h"

namespace Visualization {

InfoJavascriptFunctions* InfoJavascriptFunctions::instance()
{
	static InfoJavascriptFunctions instance;
	return &instance;
}

void InfoJavascriptFunctions::jumpToObject(QString fullName)
{
	auto nodes = Model::NameResolver::mostLikelyMatches(fullName, 1);
	if (nodes.size() == 0) return;

	auto node = nodes.first().second;
	auto scene = VisualizationManager::instance().mainScene();
	auto vis = scene->currentViewItem()->findVisualizationOf(node);
	if (!vis) return;

	//Center view on the item
	for (auto v : scene->views())
		if (auto mainView = dynamic_cast<Visualization::MainView*>(v))
		{
			mainView->centerOn(vis->scenePos());
			return;
		}
}

}
