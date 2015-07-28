/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
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

#include "DefaultVisualizer.h"

#include "ModelBase/src/nodes/Node.h"

#include "OOModel/src/declarations/Method.h"

#include "VisualizationBase/src/items/Item.h"
#include "VisualizationBase/src/overlays/SelectionOverlay.h"

#include "../graph/Graph.h"
#include "../graph/InformationNode.h"

namespace InformationScripting {

const QString DefaultVisualizer::HIGHLIGHT_OVERLAY_GROUP = {"default graph highlight"};

DefaultVisualizer DefaultVisualizer::instance()
{
	static DefaultVisualizer instance;
	return instance;
}

void DefaultVisualizer::visualize(Graph* g)
{
	auto condition = [](const InformationNode* node) {
		return node->contains("ast");
	};
	auto nodes = g->nodesForWhich(condition);

	for (auto informationNode : nodes)
	{
		// FIXME: this should be Model::Node* we first have to adapt the property converter for this.
		OOModel::Method* node = (*informationNode)["ast"];
		auto nodeVisualization = Visualization::Item::nodeItemsMap().find(node);
		Q_ASSERT(nodeVisualization != Visualization::Item::nodeItemsMap().end());
		auto item = *nodeVisualization;
		auto overlay = new Visualization::SelectionOverlay(
					item, Visualization::SelectionOverlay::itemStyles().get());
		item->addOverlay(overlay, HIGHLIGHT_OVERLAY_GROUP);
	}
}


} /* namespace InformationScripting */
