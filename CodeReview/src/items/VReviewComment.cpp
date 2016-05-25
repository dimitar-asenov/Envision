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
}

void VReviewComment::initializeForms()
{
	auto headerContent = (new Visualization::GridLayoutFormElement{})
		->setColumnStretchFactor(0, 1)
		->setVerticalAlignment(Visualization::LayoutStyle::Alignment::Center)
		->setNoBoundaryCursors([](Item*){return true;})
		->setNoInnerCursors([](Item*){return true;})
		->put(0, 0, item(&I::date_, [](I* v){return v->node()->date();}));

	auto headerBackground = item<Visualization::EmptyItem>(&I::headerBackground_, [](I* v)
	{ return &v->style()->headerBackground();});

	auto completeHeader = grid({{(new Visualization::AnchorLayoutFormElement{})
			->put(TheLeftOf, headerBackground, 2, FromLeftOf, headerContent)
			->put(TheTopOf, headerBackground, 2, FromTopOf, headerContent)
			->put(TheBottomOf, headerBackground, 2, FromBottomOf, headerContent)
			->put(TheRightOf, headerBackground, 2, FromRightOf, headerContent)}})
			->setColumnStretchFactor(0, 1);

	auto shapeElement = new Visualization::ShapeFormElement{};

	auto attr = (new Visualization::GridLayoutFormElement{})
							 ->setColumnStretchFactor(0, 1)
							 ->setVerticalAlignment(Visualization::LayoutStyle::Alignment::Center)
							 ->setNoBoundaryCursors([](Item*){return true;})
							 ->setNoInnerCursors([](Item*){return true;})
							 ->put(0, 0, item(&I::comment_, [](I* v){return v->node()->commentNode();}));

	addForm((new Visualization::AnchorLayoutFormElement{})
		->put(TheLeftOf, completeHeader, 10, FromLeftOf, attr)
		->put(TheTopOf, attr, AtBottomOf, completeHeader)
		->put(TheRightOf, attr, AtRightOf, completeHeader)
		->put(TheTopOf, shapeElement, AtCenterOf, completeHeader)
		->put(TheLeftOf, shapeElement, AtLeftOf, completeHeader)
		->put(TheBottomOf, shapeElement, 10, FromBottomOf, attr)
		->put(TheRightOf, shapeElement, 10, FromRightOf, completeHeader));
}

}
