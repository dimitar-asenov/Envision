/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#include "statements/VIfStatement.h"
#include "../elements/VStatementItemList.h"

//#include "VisualizationBase/src/layouts/PanelBorderLayout.h"
//#include "VisualizationBase/src/layouts/SequentialLayout.h"
#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/declarative/GridLayoutElement.h"
#include "VisualizationBase/src/declarative/AnchorLayoutElement.h"
#include "VisualizationBase/src/declarative/ShapeElement.h"
#include "VisualizationBase/src/items/NodeWrapper.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VIfStatement, "item")

VIfStatement::VIfStatement(Item* parent, NodeType* node, const StyleType* style) : Super(parent, node, style)
{}

void VIfStatement::updateGeometry(int availableWidth, int availableHeight)
{
	int contentWidth = 0;
	if (thenBranch_) contentWidth += thenBranch_->width();
	if (elseBranch_) contentWidth += elseBranch_->width();
	if( horizontal_ != (contentWidth <= style()->contentWidthSwitchTreshold()))
	{
		horizontal_ = !horizontal_;
		setUpdateNeeded(RepeatUpdate);
	}

	Super::updateGeometry(availableWidth, availableHeight);
}

void VIfStatement::initializeForms()
{
	auto header = (new GridLayoutElement())
			->setColumnStretchFactor(1, 1)->setVerticalAlignment(LayoutStyle::Alignment::Center)
			->setHorizontalSpacing(3)
			->put(0, 0, item<Static, I>(&I::icon_, [](I* v){return &v->style()->icon();}))
			->put(1, 0, item<NodeWrapper, I>(&I::condition_, [](I* v){return v->node()->condition();},
																[](I* v){return &v->style()->condition();}));

	auto thenBranch = item<VStatementItemList, I>(&I::thenBranch_, [](I* v){return v->node()->thenBranch();},
																[](I* v){return &v->style()->thenBranch();});

	auto elseBranch = item<VStatementItemList, I>(&I::elseBranch_, [](I* v){return v->node()->elseBranch();},
																[](I* v){return &v->style()->elseBranch();});

	auto shapeElement = new ShapeElement();

	// Form 0: then and else branch arranged horizontally
	auto contentElement = (new GridLayoutElement())->setColumnStretchFactor(1, 1)->setRowStretchFactor(0, 1)
			->put(0, 0, thenBranch)->put(1, 0, elseBranch);

	addForm((new AnchorLayoutElement())
			->put(TheLeftOf, header, AtLeftOf, contentElement)
			->put(TheLeftOf, shapeElement, 2, FromLeftOf, contentElement)
			->put(TheRightOf, header, AtRightOf, contentElement)
			->put(TheRightOf, shapeElement, 2, FromRightOf, contentElement)
			->put(TheBottomOf, header, 3, FromTopOf, contentElement)
			->put(TheTopOf, shapeElement, AtCenterOf, header)
			->put(TheBottomOf, shapeElement, 2, FromBottomOf, contentElement));

	// Form 1: then and else branch arranged vertically
	contentElement = (new GridLayoutElement())->setColumnStretchFactor(0, 1)->setRowStretchFactor(1, 1)
			->put(0, 0, thenBranch)->put(0, 1, elseBranch);

	addForm((new AnchorLayoutElement())
			->put(TheLeftOf, header, AtLeftOf, contentElement)
			->put(TheLeftOf, shapeElement, 2, FromLeftOf, contentElement)
			->put(TheRightOf, header, AtRightOf, contentElement)
			->put(TheRightOf, shapeElement, 2, FromRightOf, contentElement)
			->put(TheBottomOf, header, 3, FromTopOf, contentElement)
			->put(TheTopOf, shapeElement, AtCenterOf, header)
			->put(TheBottomOf, shapeElement, 2, FromBottomOf, contentElement));
}

int VIfStatement::determineForm()
{
	if(horizontal_) return 0;
	else return 1;
}

}
