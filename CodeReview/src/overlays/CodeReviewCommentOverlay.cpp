/***********************************************************************************************************************
**
** Copyright (c) 2015 ETH Zurich
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

#include "CodeReviewCommentOverlay.h"

#include "VisualizationBase/src/declarative/GridLayoutFormElement.h"

#include "VisualizationBase/src/declarative/DeclarativeItem.hpp"

#include "VisualizationBase/src/items/Item.h"

#include "VisualizationBase/src/VisualizationManager.h"

#include "../handlers/HCodeReviewOverlay.h"

namespace CodeReview
{

DEFINE_ITEM_COMMON(CodeReviewCommentOverlay, "item")

CodeReviewCommentOverlay::CodeReviewCommentOverlay(Visualization::Item* associatedItem,
																	NodeReviews* nodeReviews, const StyleType* style) :
	Super{{associatedItem}, style}, nodeReviews_{nodeReviews}
{
	setAcceptedMouseButtons(Qt::AllButtons);
	setItemCategory(Visualization::Scene::MenuItemCategory);
	offsetItemLocal_ = QPoint{nodeReviews->offsetX(), nodeReviews->offsetY()};
}

void CodeReviewCommentOverlay::updateGeometry(int availableWidth, int availableHeight)
{
	Super::updateGeometry(availableWidth, availableHeight);
	setPos(associatedItem()->mapToScene(offsetItemLocal_));

	setScale(1.0/Visualization::VisualizationManager::instance().mainScene()->mainViewScalingFactor());
}

void CodeReviewCommentOverlay::initializeForms()
{
	auto container = (new Visualization::GridLayoutFormElement{})
			->setTopMargin(10)
			->put(0, 0, item(&I::nodeReviewsItem_, [](I* v) {return v->nodeReviews_;}));
	addForm(container);
}

void CodeReviewCommentOverlay::updateOffsetItemLocal(QPointF scenePos)
{
	offsetItemLocal_ = CodeReviewCommentOverlay::associatedItem()->mapFromScene(scenePos);
	nodeReviews_->beginModification("edit node");
	nodeReviews_->setOffsetX(offsetItemLocal_.x());
	nodeReviews_->setOffsetY(offsetItemLocal_.y());
	nodeReviews_->endModification();
}

}
