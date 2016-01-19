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

#include "VLoopStatement.h"

#include "../elements/VStatementItemList.h"

#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/declarative/DeclarativeItemDef.h"
#include "VisualizationBase/src/items/NodeWrapper.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VLoopStatement, "item")

VLoopStatement::VLoopStatement(Item* parent, NodeType* node, const StyleType* style) :
	Super{parent, node, style}
{}

void VLoopStatement::initializeForms()
{
	auto header = (new GridLayoutFormElement{})
					->setHorizontalSpacing(3)->setColumnStretchFactor(3, 1)
					->setVerticalAlignment(LayoutStyle::Alignment::Center)
					->put(0, 0, item<Static>(&I::icon_, [](I* v){return &v->style()->icon();}))
					->put(1, 0, item<NodeWrapper>(&I::initStep_, [](I* v){return v->node()->initStep();},
																					[](I* v){return &v->style()->initStep();}))
					->put(2, 0, item<NodeWrapper>(&I::condition_, [](I* v){return v->node()->condition();},
																					[](I* v){return &v->style()->condition();}))
					->put(3, 0, item<NodeWrapper>(&I::updateStep_, [](I* v){return v->node()->updateStep();},
																					[](I* v){return &v->style()->updateStep();}));

	auto body = (new GridLayoutFormElement{})
			->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;})
			->setColumnStretchFactor(0, 1)
			->put(0, 0, item<VStatementItemList>(&I::body_, [](I* v){return v->node()->body();},
							[](I* v){return &v->style()->body();}));

	auto shapeElement = new ShapeFormElement{};

	addForm((new AnchorLayoutFormElement{})
		->put(TheTopOf, body, 3, FromBottomOf, header)
		->put(TheTopOf, shapeElement, AtCenterOf, header)
		->put(TheLeftOf, shapeElement, AtLeftOf, header)
		->put(TheLeftOf, shapeElement, 10, FromLeftOf, body)
		->put(TheRightOf, header, AtRightOf, body)
		->put(TheRightOf, shapeElement, 10, FromRightOf, header)
		->put(TheBottomOf, shapeElement, 3, FromBottomOf, body));

	// Now add the second form for a DO loop
	auto icon = grid({{item<Static>(&I::icon_, [](I* v){return &v->style()->icon();})}})->setColumnStretchFactor(0, 1);
	auto footer = grid({{item<NodeWrapper>(&I::condition_, [](I* v){return v->node()->condition();},
			[](I* v){return &v->style()->condition();})}})->setColumnStretchFactor(0, 1);

	addForm((new AnchorLayoutFormElement{})
		->put(TheTopOf, body, 3, FromBottomOf, icon)
		->put(TheTopOf, shapeElement, AtCenterOf, icon)
		->put(TheLeftOf, shapeElement, AtLeftOf, icon)
		->put(TheLeftOf, shapeElement, 5, FromLeftOf, footer)
		->put(TheLeftOf, shapeElement, 10, FromLeftOf, body)
		->put(TheRightOf, icon, AtRightOf, body)
		->put(TheRightOf, footer, AtRightOf, body)
		->put(TheRightOf, shapeElement, 10, FromRightOf, icon)
		->put(TheTopOf, footer, 10, FromBottomOf, body)
		->put(TheBottomOf, shapeElement, AtCenterOf, footer));
}

int VLoopStatement::determineForm()
{
	return static_cast<int>(node()->loopKind());
}

}
