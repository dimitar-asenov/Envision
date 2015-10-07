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

#include "Heatmap.h"

#include "../queries/QueryRegistry.h"

#include "ModelBase/src/nodes/Node.h"

namespace InformationScripting {

const QStringList Heatmap::VALUE_ATTRIBUTE_NAME_NAMES{"n", "name"};

TupleSet Heatmap::execute(TupleSet input)
{
	QString valueTag = argument(VALUE_ATTRIBUTE_NAME_NAMES[1]);
	if (valueTag.isEmpty()) valueTag = "count"; // Use count as default tag for heatmaps
	auto valueTuples = input.tuples(valueTag);

	if (valueTuples.size() > 0)
	{
		std::vector<int> values;
		values.reserve(valueTuples.size());
		for (const auto& tuple : valueTuples)
			values.push_back(tuple[valueTag]);
		auto minMaxIt = std::minmax_element(values.begin(), values.end());
		valueRange_ = {*minMaxIt.first, *minMaxIt.second};

		input.take("ast"); // remove ast as we add them colored.
		for (const auto& tuple : valueTuples)
		{
			int count = tuple[valueTag];
			Model::Node* astNode = tuple["ast"];
			// TODO change the  color to be a tuple {{color, colorValue}, {ast, node}}
			input.add({{"ast", astNode}, {"color", colorForValue(count).name()}});
		}
	}
	return input;
}

void Heatmap::registerDefaultQueries()
{
	QueryRegistry::instance().registerQueryConstructor("heatmap", [](Model::Node*, QStringList args) {
		return new Heatmap(args);
	});
}

Heatmap::Heatmap(QStringList args)
	: ScopedArgumentQuery{ nullptr, {
		{VALUE_ATTRIBUTE_NAME_NAMES, "Name of the attribute that contains the count value", VALUE_ATTRIBUTE_NAME_NAMES[1]}
		}, QStringList("Heatmap") + args}
{}

QColor Heatmap::colorForValue(int value)
{
	// First we need to calculate a index (i.e. factor) in the range. This we do with the following formula:
	// minVal + colorFactor * range == value, thus colorFactor == (value - minVal) / range.
	auto range = valueRange_.second - valueRange_.first;
	double colorFactor = (double (value - valueRange_.first)) / range;
	// The colorFactor can now be used to return a color based on the baseColor: (red + and green minus)
	int red = baseColor_.red();
	int green = baseColor_.green();
	int colorRange = 255 - red;
	colorRange += green;
	int colorAdd = colorRange * colorFactor;
	if (red + colorAdd > 255)
	{
		green -= (colorAdd - (255 - red));
		red = 255;

	}
	else
		red += colorAdd;
	return {red, green, 0};
}

} /* namespace InformationScripting */
