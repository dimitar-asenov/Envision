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

#include "VReviewComment.h"

#include "VisualizationBase/src/declarative/DeclarativeItem.hpp"

#include "VisualizationBase/src/items/Item.h"
#include "VisualizationBase/src/items/EmptyItem.h"

namespace CodeReview
{

DEFINE_ITEM_COMMON(VReviewComment, "item")

VReviewComment::VReviewComment(Visualization::Item* parent, NodeType* node, const StyleType* style)
	: Super{parent, node, style}
{
	date_ = new Visualization::Text{this,  ""};
	username_ = new Visualization::Text{this, node->username()};
}

void VReviewComment::initializeForms()
{

	auto header = (new Visualization::GridLayoutFormElement{})
			->setColumnStretchFactor(1, 1)
			->setVerticalAlignment(Visualization::LayoutStyle::Alignment::Center)
			->setNoBoundaryCursors([](Item*){return true;})
			->setNoInnerCursors([](Item*){return true;})
			->setHorizontalSpacing(10)
			->setColumnHorizontalAlignment(3, Visualization::LayoutStyle::Alignment::Right)
			->put(0, 0, item<Visualization::Text>(&I::username_, &StyleType::username))
			->put(3, 0, item<Visualization::Text>(&I::date_, &StyleType::date));


	auto container = (new Visualization::GridLayoutFormElement{})
			->setColumnStretchFactor(0, 1)
			->setVerticalAlignment(Visualization::LayoutStyle::Alignment::Center)
			->setNoBoundaryCursors([](Item*){return true;})
			->setNoInnerCursors([](Item*){return true;})
			->setVerticalSpacing(5)
			->setBottomMargin(10)
			->setTopMargin(5)
			->put(0, 0, header)
			->put(0, 1, item<Comments::VComment>(&I::comment_, [](I* v)
				{return v->node()->commentNode();}, &StyleType::comment));

	addForm(container);
}

Visualization::Item::UpdateType VReviewComment::needsUpdate()
{
	return Visualization::Item::StandardUpdate;
}

void VReviewComment::determineChildren()
{
	Super::determineChildren();

	// TODO implement mechanism for time-based updates to avoid exploiting determineChildren and needsUpdate
	updateDateText();
}

void VReviewComment::updateDateText()
{
	auto commentDate = QDateTime::fromMSecsSinceEpoch(node()->date());
	auto currentDate = QDateTime::currentDateTime();
	auto secsToCurrentDate = commentDate.secsTo(currentDate);
	if (secsToCurrentDate < 60)
		date_->setText("Some seconds ago");
	else if (secsToCurrentDate >= 60 && secsToCurrentDate < 120)
	{
				date_->setText("1 minute ago");
	}
	else if (secsToCurrentDate < 3600)
	{
		int minutes = secsToCurrentDate / 60.0;
		date_->setText(QString::number(minutes) + " minutes ago");
	}
	else
		date_->setText(commentDate.toString("dd.MM.yyyy hh:mm"));
}

}
