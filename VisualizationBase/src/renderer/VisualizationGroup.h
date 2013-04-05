/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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
#include "VisualizationSuitabilityScore.h"

namespace Model {
	class Node;
}

namespace Visualization {

class Item;

class VISUALIZATIONBASE_API VisualizationGroup {

	public:
		typedef std::function<Item* (Item* parent, Model::Node* node)> ItemConstructor;
		typedef std::function<bool (Item* parent, Model::Node* node)> ConditionFunction;

		VisualizationGroup();
		VisualizationGroup(ConditionFunction condition, int scorePoints = 1);
		virtual ~VisualizationGroup();

		void addVisualization(VisualizationGroup::ItemConstructor visualization);
		void addVisualization(VisualizationGroup::ItemConstructor visualization, ConditionFunction condition);
		void addSubGroup(VisualizationGroup* group);
		void clear();

		void setConditionFunction(ConditionFunction condition, int scorePoints = 1);

		virtual bool matchesContext(Item* parent, Model::Node* node);

		virtual QList<QPair<VisualizationSuitabilityScore, ItemConstructor> > visualizationsForContext(Item* parent,
				Model::Node* node);

	private:
		QVector<VisualizationGroup*> subGroups_;
		QVector<ItemConstructor> visualizations_;

		ConditionFunction contextCondition_;
		int scorePoints_;
};

inline void VisualizationGroup::addVisualization(VisualizationGroup::ItemConstructor visualization)
{ visualizations_ << visualization; }
inline void VisualizationGroup::addSubGroup(VisualizationGroup* group) { subGroups_ << group; }
inline void VisualizationGroup::clear() {visualizations_.clear(); subGroups_.clear();}
inline void VisualizationGroup::setConditionFunction(ConditionFunction condition, int scorePoints)
{ contextCondition_ = condition; scorePoints_ = scorePoints;}

bool operator< (const QPair<VisualizationSuitabilityScore, VisualizationGroup::ItemConstructor>& left,
		const QPair<VisualizationSuitabilityScore, VisualizationGroup::ItemConstructor>& right);

} /* namespace Visualization */
