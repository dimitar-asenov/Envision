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

#include "VSwitchStatement.h"
#include "../elements/VStatementItemList.h"

#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/declarative/DeclarativeItemDef.h"
#include "VisualizationBase/src/items/NodeWrapper.h"

#include "OOModel/src/statements/CaseStatement.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

DEFINE_ITEM_COMMON(VSwitchStatement, "item")

VSwitchStatement::VSwitchStatement(Item* parent, NodeType* node, const StyleType* style) : Super{parent, node, style}
{}

void VSwitchStatement::initializeForms()
{
	auto icon = item<Static>(&I::icon_, [](I* v){return &v->style()->icon();});

	auto condition = item<NodeWrapper>(&I::condition_,	[](I* v){return v->node()->switchExpression();},
			[](I* v){return &v->style()->condition();});

	// We need the grid for stretching
	auto header = grid({{(new AnchorLayoutFormElement{})
								->put(TheVCenterOf, icon, AtVCenterOf, condition)
								->put(TheHCenterOf, icon, AtLeftOf, condition)}})
			->setColumnStretchFactor(1, 1);

	auto cases = item<VStatementItemList>(&I::body_,	[](I* v){return v->node()->body();},
														[](I* v){return &v->style()->body();});

	auto shapeElement = new ShapeFormElement{};

	// Form 0: we already have at least one stretchable element
	addForm((new AnchorLayoutFormElement{})
			->put(TheLeftOf, header, AtLeftOf, cases)
			->put(TheLeftOf, shapeElement, -10, FromLeftOf, header)
			->put(TheRightOf, header, AtRightOf, cases)
			->put(TheRightOf, shapeElement, 2, FromRightOf, header)
			->put(TheBottomOf, header, 3, FromTopOf, cases)
			->put(TheTopOf, shapeElement, AtCenterOf, header)
			->put(TheBottomOf, shapeElement, 2, FromBottomOf, cases)
			->setBottomMargin(10));

	// Form 1: There might not be any stretchable elements
	// Make the contain auto stretchable
	auto casesContainer = grid({{cases}})
			->setColumnStretchFactor(1, 1)->setHorizontalSpacing(5)
			->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;});

	addForm((new AnchorLayoutFormElement{})
			->put(TheLeftOf, header, AtLeftOf, casesContainer)
			->put(TheLeftOf, shapeElement, -10, FromLeftOf, header)
			->put(TheRightOf, header, AtRightOf, casesContainer)
			->put(TheRightOf, shapeElement, 2, FromRightOf, header)
			->put(TheBottomOf, header, 3, FromTopOf, casesContainer)
			->put(TheTopOf, shapeElement, AtCenterOf, header)
			->put(TheBottomOf, shapeElement, 2, FromBottomOf, casesContainer)
			->setBottomMargin(10));
}

int VSwitchStatement::determineForm()
{
	if (node()->body() && node()->body()->size() > 0
		 && node()->body()->first()->typeId() == OOModel::CaseStatement::typeIdStatic())
	{
		// We know that the body will be stretchable
		return 0;
	}

	// The body might not be stretchable.
	return 1;
}

}
