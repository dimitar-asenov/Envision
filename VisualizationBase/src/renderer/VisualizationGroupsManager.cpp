/***********************************************************************************************************************
 **
 ** Copyright (c) 2014, 2016 ETH Zurich
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

#include "VisualizationGroupsManager.h"

namespace Visualization {

VisualizationGroupsManager::VisualizationGroupsManager()
{
}

void VisualizationGroupsManager::clear()
{
	for(auto groupsNodeType : groups_)
		for(auto groupsPurpose : groupsNodeType)
			for(auto g : groupsPurpose)
			SAFE_DELETE(g);

	groups_.clear();
}

VisualizationGroup *VisualizationGroupsManager::getExactMatch(int typeId, int purpose, int semanticZoomLevel)
{
	if (typeId < groups_.size() && purpose < groups_[typeId].size() &&
		 semanticZoomLevel < groups_[typeId][purpose].size() &&
		 groups_[typeId][purpose][semanticZoomLevel])
	{
		return groups_[typeId][purpose][semanticZoomLevel];
	}
	else
	{
		return nullptr;
	}
}

QVector<VisualizationGroup *> VisualizationGroupsManager::getByTypeId(int typeId)
{
	QVector<VisualizationGroup*> result = QVector<VisualizationGroup*>();

	if (typeId < groups_.size())
	{
		for (QVector<VisualizationGroup*> groups : groups_[typeId])
		{
			for (VisualizationGroup* group : groups)
			{
				if (group)
					result.append(group);
			}
		}
	}

	return result;
}

QVector<VisualizationGroup*> VisualizationGroupsManager::getByTypeIdAndPurpose(int typeId, int purpose)
{
	QVector<VisualizationGroup*> result = QVector<VisualizationGroup*>();

	if (typeId < groups_.size() && purpose < groups_[typeId].size())
	{
		for (VisualizationGroup* group : groups_[typeId][purpose])
		{
			if (group)
				result.append(group);
		}
	}

	return result;
}

QVector<VisualizationGroup *> VisualizationGroupsManager::getByTypeIdAndSemanticZoomLevel(int typeId,
																														int semanticZoomLevel)
{
	QVector<VisualizationGroup*> result = QVector<VisualizationGroup*>();

	if (typeId < groups_.size())
	{
		for (QVector<VisualizationGroup*> visualizationGroupsForThisPurpose : groups_[typeId])
		{
			if (semanticZoomLevel < visualizationGroupsForThisPurpose.size() &&
				 visualizationGroupsForThisPurpose[semanticZoomLevel])
			{
				result.append(visualizationGroupsForThisPurpose[semanticZoomLevel]);
			}
		}
	}

	return result;
}

void VisualizationGroupsManager::addVisualization(int nodeTypeId, int purpose, int semanticZoomLevel,
																  VisualizationGroup::ItemConstructor visualization)
{
	Q_ASSERT(nodeTypeId > 0);
	if (nodeTypeId >= groups_.size()) groups_.resize(nodeTypeId + 1);
	if (purpose >= groups_[nodeTypeId].size()) groups_[nodeTypeId].resize(purpose+1);
	if (semanticZoomLevel >= groups_[nodeTypeId][purpose].size())
		groups_[nodeTypeId][purpose].resize(semanticZoomLevel+1);

	if (!groups_[nodeTypeId][purpose][semanticZoomLevel])
	{
		groups_[nodeTypeId][purpose][semanticZoomLevel] = new VisualizationGroup();
	}

	groups_[nodeTypeId][purpose][semanticZoomLevel]->addVisualization(visualization);
}

void VisualizationGroupsManager::addGroup(int nodeTypeId, int purpose, int semanticZoomLevel, VisualizationGroup *group)
{
	if (nodeTypeId >= groups_.size()) groups_.resize(nodeTypeId + 1);
	if (purpose >= groups_[nodeTypeId].size()) groups_[nodeTypeId].resize(purpose+1);
	if (semanticZoomLevel >= groups_[nodeTypeId][purpose].size())
		groups_[nodeTypeId][purpose].resize(semanticZoomLevel+1);

	if (!groups_[nodeTypeId][purpose][semanticZoomLevel])
	{
		groups_[nodeTypeId][purpose][semanticZoomLevel] = new VisualizationGroup();
	}

	groups_[nodeTypeId][purpose][semanticZoomLevel]->addSubGroup(group);
}

bool VisualizationGroupsManager::hasVisualization(int nodeTypeId, int purpose, int semanticZoomLevel)
{
	Q_ASSERT(nodeTypeId > 0);
	if (nodeTypeId >= groups_.size()) return false;
	if (purpose >= groups_[nodeTypeId].size()) return false;
	if (semanticZoomLevel >= groups_[nodeTypeId][purpose].size()) return false;
	if (!groups_[nodeTypeId][purpose][semanticZoomLevel]) return false;
	return true;
	//TODO: One could also check whether there are any registered visualizations/subgroups.
}


} /* namespace Visualization */
