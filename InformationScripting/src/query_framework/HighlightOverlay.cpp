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

#include "HighlightOverlay.h"

#include "VisualizationBase/src/shapes/Shape.h"

#include "VisualizationBase/src/items/TextStyle.h"
#include "VisualizationBase/src/declarative/DeclarativeItemDef.h"
#include "VisualizationBase/src/items/EmptyItem.h"

using namespace Visualization;

namespace InformationScripting {

DEFINE_ITEM_COMMON(HighlightOverlay, "item")

HighlightOverlay::HighlightOverlay(Item* selectedItem, const StyleType* style)
	: Super{{selectedItem}, style}
{
	info_ = new Text{this};
	info_->setTextFormat(Qt::RichText);
}

void HighlightOverlay::initializeForms()
{
	auto backgroundElement = item<EmptyItem>(&I::background_, &StyleType::background);
	auto textItem = item<Text>(&I::info_, &StyleType::info);
	addForm(backgroundElement);

	addForm((new AnchorLayoutFormElement{})
			  ->put(TheVCenterOf, textItem, AtCenterOf, backgroundElement)
			  ->put(TheHCenterOf, textItem, AtCenterOf, backgroundElement));
}

int HighlightOverlay::determineForm()
{
	return info_->text().size() > 0 ? 1 : 0;
}

void HighlightOverlay::updateGeometry(int availableWidth, int availableHeight)
{
	background_->setAcceptedMouseButtons(Qt::NoButton);
	background_->setCustomSize(associatedItem()->widthInScene(), associatedItem()->heightInScene());
	Super::updateGeometry(availableWidth, availableHeight);
	if (hasShape())
	{
		auto pos = QPointF{(qreal) getShape()->contentLeft(), (qreal) getShape()->contentTop()};
		setPos(associatedItem()->mapToScene(0, 0) - pos);
	}
}

}
