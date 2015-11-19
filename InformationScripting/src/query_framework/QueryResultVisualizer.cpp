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

#include "QueryResultVisualizer.h"

#include "ModelBase/src/nodes/Node.h"

#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/items/Item.h"
#include "VisualizationBase/src/overlays/ArrowOverlay.h"

#include "HighlightOverlay.h"
#include "QueryRegistry.h"

namespace InformationScripting {

const QString QueryResultVisualizer::HIGHLIGHT_OVERLAY_GROUP = {"default graph highlight"};
const QString QueryResultVisualizer::ARROW_OVERLAY_GROUP = {"default arrow"};

const QStringList QueryResultVisualizer::INFO_ARGUMENT_NAMES{"i", "info"};

QueryResultVisualizer::QueryResultVisualizer(Model::Node* target, QStringList args)
	: LinearQuery{target}, arguments_{{
		{INFO_ARGUMENT_NAMES, "Name of info values to be printed", INFO_ARGUMENT_NAMES[1]}
	}, args}
{}

Optional<TupleSet> QueryResultVisualizer::executeLinear(TupleSet input)
{
	if (!input.isEmpty())
	{
		auto result = visualize(input);
		if (result.hasErrors())
			return {result.errors()[0]};
	}
	// A visualization is always a sink.
	// If there was an explicit visualization we don't want to have the implicit show anything.
	return TupleSet();
}

void QueryResultVisualizer::registerDefaultQueries()
{
	QueryRegistry::registerQuery<QueryResultVisualizer>("show");
	QueryRegistry::registerAlias("info", "show", [](QStringList& args) {args.prepend("-info");});
}

Optional<int> QueryResultVisualizer::visualize(const TupleSet& ts)
{
	cleanScene();
	showASTRelation(ts, "calls");

	auto colors = extractColors(ts);
	auto infoOptional = extractInfo(ts);
	if (infoOptional.hasErrors())
		return {infoOptional.errors()[0]};
	auto infos = infoOptional.value();

	auto astTuples = ts.tuples("ast");
	// Set default color for non covered ast nodes.
	for (const auto& astTuple : astTuples)
	{
		auto& color = colors[astTuple["ast"]];
		if (color.isNull()) color = "red";
	}

	for (auto it = infos.begin(); it != infos.end(); ++it)
	{
		auto& color = colors[it.key()];
		if (color.isNull()) color = "red";
	}

	for (auto it = colors.begin(); it != colors.end(); ++it)
	{
		Model::Node* node = it.key();
		Q_ASSERT(node);

		QString info = infos[node];

		auto nodeVisualizationIt = Visualization::Item::nodeItemsMap().find(node);
		if (nodeVisualizationIt == Visualization::Item::nodeItemsMap().end())
			qWarning() << "no visualization for" << node->typeName();
		while (nodeVisualizationIt != Visualization::Item::nodeItemsMap().end() && nodeVisualizationIt.key() == node)
		{
			auto item = *nodeVisualizationIt++;
			auto overlay = new HighlightOverlay(item);
			overlay->setText(info);
			setColor(overlay, it.value());
			item->addOverlay(overlay, HIGHLIGHT_OVERLAY_GROUP);
		}
	}
	return {1};
}

void QueryResultVisualizer::cleanScene()
{
	for (auto scene : Visualization::Scene::allScenes())
	{
		scene->removeOverlayGroup(HIGHLIGHT_OVERLAY_GROUP);
		scene->removeOverlayGroup(ARROW_OVERLAY_GROUP);
	}
}

void QueryResultVisualizer::showASTRelation(const TupleSet& ts, const QString& relationName)
{
	auto relationTuples = ts.tuples(relationName);
	for (auto t : relationTuples)
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

QHash<Model::Node*, QString> QueryResultVisualizer::extractColors(const TupleSet& ts)
{
	QHash<Model::Node*, QString> colors;
	for (const auto& colorTuple : ts.tuples("color"))
	{
		auto it = colorTuple.find("ast");
		if (it != colorTuple.end())
		{
			QString color = colorTuple["color"];
			colors[it->second] = color;
		}
	}
	return colors;
}

Optional<QHash<Model::Node*, QString>> QueryResultVisualizer::extractInfo(const TupleSet& ts)
{
	QHash<Model::Node*, QString> infos;

	std::vector<std::pair<QString, QString>> values;
	const QRegularExpression valueMatch("((\\w+)\\.)?(\\w+)");
	auto valueMatchIt = valueMatch.globalMatch(arguments_.argument(INFO_ARGUMENT_NAMES[1]));
	while (valueMatchIt.hasNext())
	{
		auto match = valueMatchIt.next();
		if (!match.hasMatch()) return {"Info values have to be of form: tag.value"};
		auto tag = match.captured(2);
		auto value = match.captured(3);

		values.push_back({tag, value});
	}
	if (values.empty()) return infos;
	const auto tag = values[0].first;
	bool allSame = std::all_of(values.begin(), values.end(), [&tag](const auto& values) {return values.first == tag;});
	if (!allSame) return {"Info values have to have same tag"};

	for (const auto& infoTuple : ts.tuples(tag))
	{
		auto allAsts = infoTuple.valuesOfType<Model::Node*>();
		if (allAsts.isEmpty() || allAsts.size() > 1) return {"Infotuple has to have a value which refers to an AST node"};
		auto astNode = allAsts[0];

		QStringList data;

		for (const auto& taggedVal : values)
		{
			auto valIt = infoTuple.find(taggedVal.second);
			if (valIt != infoTuple.end())
			{
				QString value = valIt->second;
				data << value;
			}
			else return {QString("info: tuple %1 has no entry named %2").arg(tag, taggedVal.second)};
		}
		infos[astNode] = data.join(", ");
	}
	return infos;
}

void QueryResultVisualizer::setColor(HighlightOverlay* overlay, QColor color)
{
	color.setAlpha(DEFAULT_ALPHA_);
	overlay->setColor(color);
}


} /* namespace InformationScripting */
