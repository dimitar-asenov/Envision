/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2016 ETH Zurich
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

#include "HObjectPathCrumb.h"

#include "VisualizationBase/src/items/Item.h"
#include "VisualizationBase/src/cursor/LayoutCursor.h"
#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/declarative/DynamicGridFormElement.h"

#include "ModelBase/src/util/NameResolver.h"

#include "VisualizationBase/src/items/ViewItem.h"

#include "VersionControlUI/src/items/VDiffFrame.h"

#include "InteractionBase/src/prompt/Prompt.h"

namespace VersionControlUI {

HObjectPathCrumb* HObjectPathCrumb::instance()
{
	static HObjectPathCrumb h;
	return &h;
}

void HObjectPathCrumb::mousePressEvent(Visualization::Item *target, QGraphicsSceneMouseEvent* event)
{
	GenericHandler::mousePressEvent(target, event);
	crumbTarget_ = static_cast<ObjectPathCrumb*>(target);
}

void HObjectPathCrumb::mouseReleaseEvent(Visualization::Item* target, QGraphicsSceneMouseEvent* event)
{
	GenericHandler::mouseReleaseEvent(target, event);
	if (crumbTarget_)
	{
		VersionControlUI::VDiffFrame* vDiffFrame = nullptr;
		auto parent = crumbTarget_->parent();

		while (parent && !DCast<VDiffFrame>(parent))
			parent = parent->parent();

		vDiffFrame = DCast<VDiffFrame>(parent);

		auto currentView = crumbTarget_->scene()->currentViewItem();

		auto grid = dynamic_cast<Visualization::DynamicGridFormElement*>(currentView->currentForm());

		auto focusedIndex = grid->indexOf(currentView, vDiffFrame);

		Visualization::MajorMinorIndex indexToInsert;
		indexToInsert.major_ = focusedIndex.y();
		indexToInsert.minor_ = focusedIndex.x()+1;

		auto matches = Model::NameResolver::mostLikelyMatches(crumbTarget_->path()+crumbTarget_->name(), 1);
		if (matches.size() > 0)
			currentView->insertNode(matches[0].second, indexToInsert);
	}

	crumbTarget_ = nullptr;
}

}
