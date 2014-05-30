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

#pragma once

#include "../visualizationbase_api.h"

#include "VisualizationGroup.h"

namespace Visualization {

class VISUALIZATIONBASE_API VisualizationGroupsManager
{
	public:
		VisualizationGroupsManager();

		/**
		 * Clears all managed visualization groups.
		 */
		void clear();


		/**
		 * Returns a visualization group for the exact matching type id, purpose and semantic zoom level.
		 *
		 * Returns nullptr if no visualization matches the criteria.
		 */
		VisualizationGroup* getExactMatch(int typeId, int purpose, int semanticZoomLevel);

		/**
		 * Returns a list of visualization groups with matching type id.
		 */
		QVector<VisualizationGroup*> getByTypeId(int typeId);

		/**
		 * Returns a list of visualization groups with matching type id and purpose.
		 */
		QVector<VisualizationGroup*> getByTypeIdAndPurpose(int typeId, int purpose);

		/**
		 * Returns a list of visualization groups with matching type id and semantic zoom level.
		 */
		QVector<VisualizationGroup*> getByTypeIdAndSemanticZoomLevel(int typeId, int semanticZoomLevel);


		/**
		 * Adds a visualization to the managed pool.
		 */
		void addVisualization(int nodeTypeId, int purpose, int semanticZoomLevel,
									 VisualizationGroup::ItemConstructor visualization, int itemTypeId);

		/**
		 * Adds a visualization group to the managed pool.
		 */
		void addGroup(int nodeTypeId, int purpose, int semanticZoomLevel, VisualizationGroup* group);

		/**
		 * Returns whether there exists a visualization for the provided parameters.
		 */
		bool hasVisualization(int nodeTypeId, int purpose, int semanticZoomLevel);

	private:
		QVector<QVector<QVector<VisualizationGroup*>>> groupsTypePurposeSz_;
		QVector<QVector<QVector<VisualizationGroup*>>> groupsTypeSzPurpose_;

		void prepareAddingVisualizationGroupOrVisualization(int nodeTypeId, int purpose, int semanticZoomLevel);
};

} /* namespace Visualization */
