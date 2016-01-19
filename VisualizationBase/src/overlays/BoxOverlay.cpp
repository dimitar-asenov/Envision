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
#include "BoxOverlay.h"

#include "../items/StaticStyle.h"
#include "../items/Text.h"
#include "../items/Static.h"
#include "../items/TextStyle.h"
#include "../declarative/DeclarativeItemDef.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(BoxOverlay, "item")

BoxOverlay::BoxOverlay(Item* associatedItem, const StyleType* style) : Super{{associatedItem}, style}
{
	if (!style->closeIcon().clickHandler())
	{
		style->closeIcon().setClickHandler([](Static* staticParent)
		{
			// This indirection is needed since we can't destroy an item while we're in its even handler.
			staticParent->scene()->addPostEventAction(
						[staticParent](){	staticParent->scene()->removeOverlay(staticParent->parent());});
			return true;
		});
	}
}

BoxOverlay::BoxOverlay(Item* associatedItem, SyncFunction syncFunction, const StyleType* style)
	: BoxOverlay{associatedItem, style}
{
	 syncFunction_ = syncFunction;
}


void BoxOverlay::determineChildren()
{
	QString caption;
	if (syncFunction_) caption = syncFunction_(this);
	Super::determineChildren();
	Q_ASSERT(caption_);
	caption_->setText(caption);
}


void BoxOverlay::updateGeometry(int availableWidth, int availableHeight)
{
	Super::updateGeometry(availableWidth, availableHeight);

	if (associatedItem()->heightInLocal() < 100)
		setPos(associatedItem()->mapToScene(0, associatedItem()->heightInLocal()));
	else if (associatedItem()->widthInLocal() < 200)
		setPos(associatedItem()->mapToScene(associatedItem()->widthInLocal(), 0));
	else
		setPos( associatedItem()->mapToScene(10, 10) );
}

void BoxOverlay::initializeForms()
{
	auto header = (new GridLayoutFormElement{})
			->setSpacing(3)->setColumnStretchFactor(1, 1)
			->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;})
			->put(0, 0, item<Text>(&I::caption_, &StyleType::caption))
			->put(2, 0, item<Static>(&I::closeIcon_, &StyleType::closeIcon));

	auto container = (new GridLayoutFormElement{})
			->put(0, 0, header)
			->put(0, 1, item<Item>(&I::content_));

	addForm(container);
}

}
