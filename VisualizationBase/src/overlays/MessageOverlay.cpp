/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "MessageOverlay.h"

#include "../items/StaticStyle.h"
#include "../items/Text.h"
#include "../items/Static.h"
#include "../items/TextStyle.h"
#include "../declarative/DeclarativeItem.hpp"

namespace Visualization {

DEFINE_ITEM_COMMON(MessageOverlay, "item")

MessageOverlay::MessageOverlay(Item* associatedItem, const StyleType* style) : Super{{associatedItem}, style}
{
	setAcceptedMouseButtons(Qt::AllButtons);

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

MessageOverlay::MessageOverlay(Item* associatedItem, SyncFunction syncFunction, const StyleType* style)
	: MessageOverlay{associatedItem, style}
{
	 syncFunction_ = syncFunction;
}

void MessageOverlay::determineChildren()
{
	QString message;
	if (syncFunction_) message = syncFunction_(this);
	Super::determineChildren();
	Q_ASSERT(message_);
	message_->setText(message);
}

void MessageOverlay::updateGeometry(int availableWidth, int availableHeight)
{
	Super::updateGeometry(availableWidth, availableHeight);
	// TODO: if there are multiple messages for the same node it migth be better to place it different.
	// It seems good to have it below, having it on the right side seems weird.
	if (!positionSet_)
	{
		setPos(associatedItem()->mapToScene(0, associatedItem()->heightInLocal()));
		positionSet_ = true;
	}
}

void MessageOverlay::initializeForms()
{
	auto header = (new GridLayoutFormElement{})
			->setSpacing(3)->setColumnStretchFactor(1, 1)
			->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;})
			->put(0, 0, item<Static>(&I::messageIcon_, &StyleType::messageIcon))
			->put(1, 0, item<Text>(&I::message_, &StyleType::message))
			->put(2, 0, item<Static>(&I::closeIcon_, &StyleType::closeIcon));

	auto container = (new GridLayoutFormElement{})
			->put(0, 0, header)
			->put(0, 1, item<Item>(&I::content_));

	addForm(container);
}

}
