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

#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/items/Item.h"
#include "VisualizationBase/src/overlays/ArrowOverlay.h"

#include "HighlightOverlay.h"
#include "../dataformat/TupleSet.h"

namespace InformationScripting {

const QString DefaultVisualizer::HIGHLIGHT_OVERLAY_GROUP = {"default graph highlight"};
const QString DefaultVisualizer::ARROW_OVERLAY_GROUP = {"default arrow"};

DefaultVisualizer& DefaultVisualizer::instance()
{
	static DefaultVisualizer instance;
	return instance;
}

void DefaultVisualizer::visualize(const TupleSet& ts)
{
	// First remove all existing overlays:
	for (auto scene : Visualization::Scene::allScenes())
	{
		scene->removeOverlayGroup(HIGHLIGHT_OVERLAY_GROUP);
		scene->removeOverlayGroup(ARROW_OVERLAY_GROUP);
	}
	// Now draw the new stuff
	auto calls = ts.tuples("calls");
	if (!calls.empty())
	{
		for (auto t : calls)
		{
			auto allAsts = t.valuesOfType<Model::Node*>();
			if (allAsts.size() > 1)
			{
				Model::Node* fromNode = allAsts[0];
				Model::Node* toNode = allAsts[1];

				auto fromVisualization = Visualization::Item::nodeItemsMap().find(fromNode);
				Q_ASSERT(fromVisualization != Visualization::Item::nodeItemsMap().end());
				auto toVisualization = Visualization::Item::nodeItemsMap().find(toNode);
				Q_ASSERT(toVisualization != Visualization::Item::nodeItemsMap().end());
				auto fromVisualizationItem = *fromVisualization;

				auto overlay = new Visualization::ArrowOverlay(
				fromVisualizationItem, *toVisualization, Visualization::ArrowOverlay::itemStyles().get());
				fromVisualizationItem->addOverlay(overlay, ARROW_OVERLAY_GROUP);
			}
		}
	}
	else
	{
		QHash<Model::Node*, QString> colors;

		auto astTuples = ts.tuples("ast");
		auto colorTuples = ts.tuples("color");

		for (const auto& colorTuple : colorTuples)
		{
			auto it = colorTuple.find("ast");
			if (it != colorTuple.end())
			{
				QString color = colorTuple["color"];
				colors[it->second] = color;
			}
		}

		// Set default color for non covered ast nodes.
		for (const auto& astTuple : astTuples)
		{
			auto& color = colors[astTuple["ast"]];
			if (color.isNull()) color = "red";
		}


		for (auto it = colors.begin(); it != colors.end(); ++it)
		{
			Model::Node* node = it.key();
			Q_ASSERT(node);
			auto nodeVisualizationIt = Visualization::Item::nodeItemsMap().find(node);
			if (nodeVisualizationIt == Visualization::Item::nodeItemsMap().end())
				qWarning() << "no visualization for" << node->typeName();
			while (nodeVisualizationIt != Visualization::Item::nodeItemsMap().end() && nodeVisualizationIt.key() == node)
			{
				auto item = *nodeVisualizationIt++;
				auto overlay = new HighlightOverlay(item);
				setColor(overlay, it.value());
				item->addOverlay(overlay, HIGHLIGHT_OVERLAY_GROUP);
			}
		}
	}
}

void DefaultVisualizer::setColor(HighlightOverlay* overlay, QColor color)
{
	color.setAlpha(DEFAULT_ALPHA_);
	overlay->setColor(color);
}


} /* namespace InformationScripting */
