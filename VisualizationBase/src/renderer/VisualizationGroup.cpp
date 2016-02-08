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

#include "VisualizationGroup.h"

namespace Visualization {

VisualizationGroup::VisualizationGroup() : scorePoints_{0}
{
}

VisualizationGroup::VisualizationGroup(ConditionFunction condition, int scorePoints)
	: contextCondition_{condition}, scorePoints_{scorePoints}
{
}

VisualizationGroup::~VisualizationGroup()
{
	clear();
}

bool VisualizationGroup::matchesContext(Item* parent, Model::Node* node)
{
	if (contextCondition_) return contextCondition_(parent, node);
	else return true;
}

QList<QPair<VisualizationSuitabilityScore, QPair<int, VisualizationGroup::ItemConstructor>>>
VisualizationGroup::visualizationsForContext(Item* parent, Model::Node* node)
{
	QList<QPair<VisualizationSuitabilityScore, QPair<int, ItemConstructor>>> result;
	if (matchesContext(parent, node))
	{
		for (int i = 0; i<visualizations_.size(); ++i)
			result << qMakePair(VisualizationSuitabilityScore{scorePoints_},
					qMakePair(itemTypeIds_[i], visualizations_[i]));

		for (auto sg : subGroups_)
		{
			for (auto v : sg->visualizationsForContext(parent, node) )
			{
				v.first.score += scorePoints_;
				result << v;
			}
		}
	}

	return result;
}

bool operator< (const QPair<VisualizationSuitabilityScore, QPair<int, VisualizationGroup::ItemConstructor>>& left,
		const QPair<VisualizationSuitabilityScore, QPair<int, VisualizationGroup::ItemConstructor>>& right)
{
	return left.first < right.first;
}

void VisualizationGroup::addVisualization(VisualizationGroup::ItemConstructor visualization, int itemTypeId)
{
	visualizations_ << visualization;
	itemTypeIds_ << itemTypeId;
}

void VisualizationGroup::addVisualization(VisualizationGroup::ItemConstructor visualization, int itemTypeId,
		ConditionFunction condition)
{
	auto g = new VisualizationGroup{condition};
	g->addVisualization(visualization, itemTypeId);
	addSubGroup(g);
}

}
