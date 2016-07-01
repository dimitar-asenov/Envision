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

#include "VTryCatchFinally.h"
#include "../elements/VStatementItemList.h"

#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/declarative/DeclarativeItem.hpp"
#include "VisualizationBase/src/declarative/BorderFormElement.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

DEFINE_ITEM_COMMON(VTryCatchFinally, "item")

VTryCatchFinally::VTryCatchFinally(Item* parent, NodeType* node, const StyleType* style) : Super{parent, node, style}{}


int VTryCatchFinally::determineForm()
{
	return node()->finallyBody()->isEmpty() ? 0 : 1;
}

void VTryCatchFinally::initializeForms()
{
	auto tryIcon = grid({{item<Static>(&I::tryIcon_, [](I* v){return &v->style()->tryIcon();})}})
		->setColumnStretchFactor(1, 1);

	auto tryBody = grid({{item(&I::tryBody_, [](I* v){return v->node()->tryBody();})}})
			->setColumnStretchFactor(1, 1);
	auto catchClauses = item<VList>(&I::catchClauses_, [](I* v){return v->node()->catchClauses();},
			[](I* v){return &v->style()->catchClauses();});

	auto shapeElement = new ShapeFormElement{};

	addForm((new AnchorLayoutFormElement{})
			  ->put(TheTopOf, tryBody, 3, FromBottomOf, tryIcon)
			  ->put(TheTopOf, shapeElement, AtCenterOf, tryIcon)
			  ->put(TheLeftOf, shapeElement, -10, FromLeftOf, tryIcon)
			  ->put(TheLeftOf, shapeElement, 5, FromLeftOf, tryBody)
			  ->put(TheRightOf, tryIcon, AtRightOf, tryBody)
			  ->put(TheRightOf, shapeElement, 3, FromRightOf, tryIcon)
			  ->put(TheRightOf, shapeElement, 3, FromRightOf, tryBody)
			  ->put(TheLeftOf, catchClauses, AtLeftOf, tryIcon)
			  ->put(TheRightOf, catchClauses, AtRightOf, shapeElement)
			  ->put(TheTopOf, catchClauses, 3, FromBottomOf, tryBody)
			  ->put(TheBottomOf, shapeElement, 3, FromBottomOf, catchClauses));

	auto finallyIcon = grid({{item<Static>(&I::finallyIcon_, [](I* v){return &v->style()->finallyIcon();})}})
		->setColumnStretchFactor(1, 1);
	auto finallyBody = grid({{item(&I::finallyBody_, [](I* v){return v->node()->finallyBody();})}})
		->setColumnStretchFactor(1, 1);
	auto finallyOutline = item<EmptyItem>(&I::finallyOutline_, [](I* v){return &v->style()->finallyOutline();});


	addForm((new AnchorLayoutFormElement{})
	  ->put(TheTopOf, tryBody, 3, FromBottomOf, tryIcon)
	  ->put(TheTopOf, shapeElement, AtCenterOf, tryIcon)
	  ->put(TheLeftOf, shapeElement, -10, FromLeftOf, tryIcon)
	  ->put(TheLeftOf, shapeElement, 5, FromLeftOf, tryBody)
	  ->put(TheRightOf, tryIcon, AtRightOf, tryBody)
	  ->put(TheRightOf, shapeElement, 3, FromRightOf, tryIcon)
	  ->put(TheRightOf, shapeElement, 3, FromRightOf, tryBody)
	  ->put(TheLeftOf, catchClauses, AtLeftOf, tryIcon)
	  ->put(TheRightOf, catchClauses, AtRightOf, shapeElement)
	  ->put(TheTopOf, catchClauses, 3, FromBottomOf, tryBody)

	  ->put(TheTopOf, finallyIcon, 3, FromBottomOf, catchClauses)
	  ->put(TheLeftOf, finallyIcon, AtLeftOf, tryIcon)
	  ->put(TheRightOf, finallyOutline, AtRightOf, shapeElement)
	  ->put(TheBottomOf, finallyOutline, 3, FromBottomOf, shapeElement)

	  ->put(TheTopOf, finallyBody, 3, FromBottomOf, finallyIcon)
	  ->put(TheTopOf, finallyOutline, AtCenterOf, finallyIcon)
	  ->put(TheLeftOf, finallyOutline, -10, FromLeftOf, finallyIcon)
	  ->put(TheLeftOf, finallyOutline, 5, FromLeftOf, finallyBody)
	  ->put(TheRightOf, finallyIcon, AtRightOf, finallyBody)
	  ->put(TheRightOf, finallyOutline, 3, FromRightOf, finallyIcon)
	  ->put(TheBottomOf, finallyOutline, 3, FromBottomOf, finallyBody)
	  ->put(TheRightOf, finallyOutline, 3, FromRightOf, finallyBody));
}

}
