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

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VSwitchStatement, "item")

VSwitchStatement::VSwitchStatement(Item* parent, NodeType* node, const StyleType* style) : Super(parent, node, style)
{}

void VSwitchStatement::initializeForms()
{
	auto header = (new GridLayoutFormElement())
			->setColumnStretchFactor(1, 1)->setVerticalAlignment(LayoutStyle::Alignment::Center)
			->setHorizontalSpacing(3)
			->put(0, 0, item<Static>(&I::icon_, [](I* v){return &v->style()->icon();}))
			->put(1, 0, item<NodeWrapper>(&I::condition_,	[](I* v){return v->node()->switchExpression();},
																			[](I* v){return &v->style()->condition();}));

	auto cases = item<VStatementItemList>(&I::body_,	[](I* v){return v->node()->body();},
														[](I* v){return &v->style()->body();});

	// We need something stretchable
	auto casesContainer = grid({{cases}})->setColumnStretchFactor(1, 1);

	auto shapeElement = new ShapeFormElement();

	addForm((new AnchorLayoutFormElement())
			->put(TheLeftOf, header, 10, FromLeftOf, casesContainer)
			->put(TheLeftOf, shapeElement, 2, FromLeftOf, header)
			->put(TheRightOf, header, AtRightOf, casesContainer)
			->put(TheRightOf, shapeElement, 2, FromRightOf, header)
			->put(TheBottomOf, header, 3, FromTopOf, casesContainer)
			->put(TheTopOf, shapeElement, AtCenterOf, header)
			->put(TheBottomOf, shapeElement, 2, FromBottomOf, casesContainer));
}

} /* namespace OOVisualization */
