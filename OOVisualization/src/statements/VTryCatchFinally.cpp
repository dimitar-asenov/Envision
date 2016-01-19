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
#include "VisualizationBase/src/declarative/DeclarativeItemDef.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VTryCatchFinally, "item")

VTryCatchFinally::VTryCatchFinally(Item* parent, NodeType* node, const StyleType* style) : Super{parent, node, style}{}


int VTryCatchFinally::determineForm()
{
	return node()->finallyBody()->isEmpty() ? 0 : 1;
}

void VTryCatchFinally::initializeForms()
{
	auto tryIcon = grid({{item<Static>(&I::tryIcon_, [](I* v){return &v->style()->tryIcon();})}})
		->setColumnStretchFactor(1, 1);

	auto tryBody = item(&I::tryBody_, [](I* v){return v->node()->tryBody();});
	auto catchClauses = item<VList>(&I::catchClauses_, [](I* v){return v->node()->catchClauses();},
			[](I* v){return &v->style()->catchClauses();});

	auto contentElement = grid({{tryBody}, {catchClauses}})
				->setColumnStretchFactor(1, 1);

	auto shapeElement = new ShapeFormElement{};

	addForm((new AnchorLayoutFormElement{})
			->put(TheLeftOf, tryIcon, 8, FromLeftOf, contentElement)
			->put(TheLeftOf, shapeElement, 10, FromLeftOf, contentElement)
			->put(TheRightOf, tryIcon, AtRightOf, contentElement)
			->put(TheRightOf, shapeElement, 10, FromRightOf, contentElement)
			->put(TheBottomOf, tryIcon, 3, FromTopOf, contentElement)
			->put(TheTopOf, shapeElement, AtCenterOf, tryIcon)
			->put(TheBottomOf, shapeElement, 2, FromBottomOf, contentElement));

	auto finallyIcon = grid({{item<Static>(&I::finallyIcon_, [](I* v){return &v->style()->finallyIcon();})}})
		->setColumnStretchFactor(1, 1);
	auto finallyBody = grid({{item(&I::finallyBody_, [](I* v){return v->node()->finallyBody();})}})
		->setColumnStretchFactor(1, 1);
	auto finallyOutline = item<EmptyItem>(&I::finallyOutline_, [](I* v){return &v->style()->finallyOutline();});

	auto finallyCombined = (new AnchorLayoutFormElement{})
		->put(TheLeftOf, finallyIcon, AtLeftOf, finallyBody)
		->put(TheLeftOf, finallyOutline, 2, FromLeftOf, finallyBody)
		->put(TheRightOf, finallyIcon, AtRightOf, finallyBody)
		->put(TheRightOf, finallyOutline, 2, FromRightOf, finallyBody)
		->put(TheBottomOf, finallyIcon, 3, FromTopOf, finallyBody)
		->put(TheTopOf, finallyOutline, AtCenterOf, finallyIcon)
		->put(TheBottomOf, finallyOutline, 2, FromBottomOf, finallyBody);

	contentElement = grid({{tryBody}, {catchClauses}, {finallyCombined}})
				->setColumnStretchFactor(1, 1)->setVerticalSpacing(3);

	addForm((new AnchorLayoutFormElement{})
			->put(TheLeftOf, tryIcon, 8, FromLeftOf, contentElement)
			->put(TheLeftOf, shapeElement, 10, FromLeftOf, contentElement)
			->put(TheRightOf, tryIcon, AtRightOf, contentElement)
			->put(TheRightOf, shapeElement, 10, FromRightOf, contentElement)
			->put(TheBottomOf, tryIcon, 3, FromTopOf, contentElement)
			->put(TheTopOf, shapeElement, AtCenterOf, tryIcon)
			->put(TheBottomOf, shapeElement, 2, FromBottomOf, contentElement));
}

}
