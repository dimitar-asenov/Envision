/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2012 ETH Zurich
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

/*
 * VisualizationGroup.cpp
 *
 *  Created on: Apr 13, 2012
 *      Author: Dimitar Asenov
 */

#include "VisualizationGroup.h"

namespace Visualization {

VisualizationGroup::VisualizationGroup()
{
	// TODO Auto-generated constructor stub

}

VisualizationGroup::~VisualizationGroup()
{
	for(auto g : subGroups_)
		SAFE_DELETE(g);

	subGroups_.clear();
	visualizations_.clear();
}

bool VisualizationGroup::matchesContext(Item*, Model::Node*)
{
	return true;
}

QList<QPair<VisualizationSuitabilityScore, VisualizationGroup::ItemConstructor> >
VisualizationGroup::visualizationsForContext(Item* parent, Model::Node* node)
{
	QList<QPair<VisualizationSuitabilityScore, ItemConstructor> > result;
	if (matchesContext(parent, node))
	{
		for(auto v : visualizations_)
			result << qMakePair(VisualizationSuitabilityScore(), v);

		for(auto sg : subGroups_)
			result << sg->visualizationsForContext(parent, node);
	}

	return result;
}

bool operator< (const QPair<VisualizationSuitabilityScore, VisualizationGroup::ItemConstructor>& left,
		const QPair<VisualizationSuitabilityScore, VisualizationGroup::ItemConstructor>& right)
{
	return left.first < right.first;
}

} /* namespace Visualization */
