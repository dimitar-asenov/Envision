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

#include "VisualizationGroupsManager.h"

namespace Visualization {

VisualizationGroupsManager::VisualizationGroupsManager()
{
}

void VisualizationGroupsManager::clear()
{
	for (auto groupsNodeType : groupsTypePurposeSz_)
		for (auto groupsPurpose : groupsNodeType)
			for (auto g : groupsPurpose)
			SAFE_DELETE(g);

	groupsTypePurposeSz_.clear();
	groupsTypeSzPurpose_.clear();
}

VisualizationGroup *VisualizationGroupsManager::getExactMatch(int typeId, int purpose, int semanticZoomLevel)
{
	if (typeId < groupsTypePurposeSz_.size() && purpose < groupsTypePurposeSz_[typeId].size() &&
		 semanticZoomLevel < groupsTypePurposeSz_[typeId][purpose].size())
		return groupsTypePurposeSz_[typeId][purpose][semanticZoomLevel];
	else return nullptr;
}

QVector<VisualizationGroup *> VisualizationGroupsManager::getByTypeId(int typeId)
{
	QVector<VisualizationGroup*> result = QVector<VisualizationGroup*>();

	if (typeId < groupsTypePurposeSz_.size())
		for (QVector<VisualizationGroup*> groups : groupsTypePurposeSz_[typeId])
			for (VisualizationGroup* group : groups)
				if (group) result.append(group);

	return result;
}

QVector<VisualizationGroup*> VisualizationGroupsManager::getByTypeIdAndPurpose(int typeId, int purpose)
{
	QVector<VisualizationGroup*> result = QVector<VisualizationGroup*>();

	if (typeId < groupsTypePurposeSz_.size() && purpose < groupsTypePurposeSz_[typeId].size())
		for (VisualizationGroup* group : groupsTypePurposeSz_[typeId][purpose])
			if (group) result.append(group);

	return result;
}

QVector<VisualizationGroup *> VisualizationGroupsManager::getByTypeIdAndSemanticZoomLevel(int typeId,
																														int semanticZoomLevel)
{
	QVector<VisualizationGroup*> result = QVector<VisualizationGroup*>();

	if (typeId < groupsTypeSzPurpose_.size() && semanticZoomLevel < groupsTypeSzPurpose_[typeId].size())
		for (VisualizationGroup* group : groupsTypeSzPurpose_[typeId][semanticZoomLevel])
			if (group) result.append(group);

	return result;
}

void VisualizationGroupsManager::addVisualization(int nodeTypeId, int purpose, int semanticZoomLevel,
																  VisualizationGroup::ItemConstructor visualization, int itemTypeId)
{
	Q_ASSERT(nodeTypeId > 0);
	prepareAddingVisualizationGroupOrVisualization(nodeTypeId, purpose, semanticZoomLevel);

	groupsTypePurposeSz_[nodeTypeId][purpose][semanticZoomLevel]->addVisualization(visualization, itemTypeId);
}

void VisualizationGroupsManager::addGroup(int nodeTypeId, int purpose, int semanticZoomLevel, VisualizationGroup *group)
{
	prepareAddingVisualizationGroupOrVisualization(nodeTypeId, purpose, semanticZoomLevel);

	groupsTypePurposeSz_[nodeTypeId][purpose][semanticZoomLevel]->addSubGroup(group);
}

void VisualizationGroupsManager::prepareAddingVisualizationGroupOrVisualization(int nodeTypeId, int purpose,
																											int semanticZoomLevel)
{
	if (nodeTypeId >= groupsTypePurposeSz_.size()) groupsTypePurposeSz_.resize(nodeTypeId + 1);
	if (purpose >= groupsTypePurposeSz_[nodeTypeId].size()) groupsTypePurposeSz_[nodeTypeId].resize(purpose+1);
	if (semanticZoomLevel >= groupsTypePurposeSz_[nodeTypeId][purpose].size())
		groupsTypePurposeSz_[nodeTypeId][purpose].resize(semanticZoomLevel+1);

	if (nodeTypeId >= groupsTypeSzPurpose_.size()) groupsTypeSzPurpose_.resize(nodeTypeId + 1);
	if (semanticZoomLevel >= groupsTypeSzPurpose_[nodeTypeId].size())
		groupsTypeSzPurpose_[nodeTypeId].resize(semanticZoomLevel+1);
	if (purpose >= groupsTypeSzPurpose_[nodeTypeId][semanticZoomLevel].size())
		groupsTypeSzPurpose_[nodeTypeId][semanticZoomLevel].resize(purpose+1);

	if (!groupsTypePurposeSz_[nodeTypeId][purpose][semanticZoomLevel])
	{
		auto newVisualizationGroup = new VisualizationGroup{};

		groupsTypePurposeSz_[nodeTypeId][purpose][semanticZoomLevel] = newVisualizationGroup;
		groupsTypeSzPurpose_[nodeTypeId][semanticZoomLevel][purpose] = newVisualizationGroup;
	}
}

bool VisualizationGroupsManager::hasVisualization(int nodeTypeId, int purpose, int semanticZoomLevel)
{
	Q_ASSERT(nodeTypeId > 0);
	if (nodeTypeId >= groupsTypePurposeSz_.size()) return false;
	if (purpose >= groupsTypePurposeSz_[nodeTypeId].size()) return false;
	if (semanticZoomLevel >= groupsTypePurposeSz_[nodeTypeId][purpose].size()) return false;
	return groupsTypePurposeSz_[nodeTypeId][purpose][semanticZoomLevel];
	//TODO: One could also check whether there are any registered visualizations/subgroups.
}

}
