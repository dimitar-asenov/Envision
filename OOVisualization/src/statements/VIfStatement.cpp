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

#include "VIfStatement.h"
#include "../elements/VStatementItemList.h"

#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/items/Line.h"
#include "VisualizationBase/src/declarative/DeclarativeItemDef.h"
#include "VisualizationBase/src/items/NodeWrapper.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VIfStatement, "item")

VIfStatement::VIfStatement(Item* parent, NodeType* node, const StyleType* style) : Super{parent, node, style}
{}

void VIfStatement::updateGeometry(int availableWidth, int availableHeight)
{
	int contentWidth = 0;
	if (thenBranch_) contentWidth += thenBranch_->widthInParent();
	if (elseBranch_) contentWidth += elseBranch_->widthInParent();
	if ( horizontal_ != (contentWidth <= style()->contentWidthSwitchTreshold()))
	{
		horizontal_ = !horizontal_;
		setUpdateNeeded(RepeatUpdate);
	}

	Super::updateGeometry(availableWidth, availableHeight);
}

int VIfStatement::determineForm()
{
	if (node()->elseBranch() && node()->elseBranch()->size() == 0) return 0;

	if (node()->elseBranch() && node()->elseBranch()->size() == 1 && DCast<IfStatement>(node()->elseBranch()->first()))
		return 3;
	return horizontal_ ? 1 : 2;
}

void VIfStatement::initializeForms()
{
	auto header = (new GridLayoutFormElement{})
			->setColumnStretchFactor(1, 1)->setVerticalAlignment(LayoutStyle::Alignment::Center)
			->setHorizontalSpacing(3)
			->put(0, 0, item<Static>(&I::icon_, [](I* v){
						if (v->node()->parent() && v->node()->parent()->parent() )
						{
							if (auto ifs = DCast<IfStatement>(v->node()->parent()->parent()))
								if ( ifs->elseBranch() == v->node()->parent())
									return &v->style()->elificon();
						}
						return &v->style()->icon();
					}))
			->put(1, 0, item<NodeWrapper>(&I::condition_, [](I* v){return v->node()->condition();},
																[](I* v){return &v->style()->condition();}));

	auto thenBranch = item<VStatementItemList>(&I::thenBranch_, [](I* v){return v->node()->thenBranch();},
																[](I* v){return &v->style()->thenBranch();});

	auto elseBranch = item<VStatementItemList>(&I::elseBranch_, [](I* v){return v->node()->elseBranch();},
																[](I* v){return &v->style()->elseBranch();});

	auto shapeElement = new ShapeFormElement{};

	// Form 0: no else branch
	auto contentElement = (new GridLayoutFormElement{})->setColumnStretchFactor(1, 1)->setRowStretchFactor(0, 1)
			->setHorizontalSpacing(5)
			->put(0, 0, thenBranch)
			->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;});

	addForm((new AnchorLayoutFormElement{})
			->put(TheTopOf, contentElement, 3, FromBottomOf, header)
			->put(TheTopOf, shapeElement, AtCenterOf, header)
			->put(TheLeftOf, shapeElement, AtLeftOf, header)
			->put(TheLeftOf, shapeElement, 10, FromLeftOf, contentElement)
			->put(TheRightOf, header, AtRightOf, contentElement)
			->put(TheRightOf, shapeElement, 3, FromRightOf, header)
			->put(TheBottomOf, shapeElement, 3, FromBottomOf, contentElement));

	// Form 1: then and else branch arranged horizontally
	auto elseVerticalLineElement = item<Line>(&I::elseLine_, [](I* v){return &v->style()->elseVerticalLine();});

	contentElement = (new GridLayoutFormElement{})->setColumnStretchFactor(1, 1)->setRowStretchFactor(0, 1)
			->setHorizontalSpacing(5)
			->put(0, 0, thenBranch)->put(1, 0, elseVerticalLineElement)->put(2, 0, elseBranch)
			->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;});

	addForm((new AnchorLayoutFormElement{})
			->put(TheTopOf, contentElement, 3, FromBottomOf, header)
			->put(TheTopOf, shapeElement, AtCenterOf, header)
			->put(TheLeftOf, shapeElement, AtLeftOf, header)
			->put(TheLeftOf, shapeElement, 10, FromLeftOf, contentElement)
			->put(TheRightOf, header, AtRightOf, contentElement)
			->put(TheRightOf, shapeElement, 3, FromRightOf, header)
			->put(TheBottomOf, shapeElement, 3, FromBottomOf, contentElement));

	// Form 2: then and else branch arranged vertically
	auto elseHorizontalLineElement = item<Line>(&I::elseLine_, [](I* v){return &v->style()->elseHorizontalLine();});

	contentElement = (new GridLayoutFormElement{})->setColumnStretchFactor(0, 1)->setRowStretchFactor(1, 1)
			->setVerticalSpacing(5)
			->put(0, 0, thenBranch)->put(0, 1, elseHorizontalLineElement)->put(0, 2, elseBranch)
			->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;});

	addForm((new AnchorLayoutFormElement{})
			->put(TheTopOf, contentElement, 3, FromBottomOf, header)
			->put(TheTopOf, shapeElement, AtCenterOf, header)
			->put(TheLeftOf, shapeElement, AtLeftOf, header)
			->put(TheLeftOf, shapeElement, 10, FromLeftOf, contentElement)
			->put(TheRightOf, header, AtRightOf, contentElement)
			->put(TheRightOf, shapeElement, 3, FromRightOf, header)
			->put(TheBottomOf, shapeElement, 3, FromBottomOf, contentElement));

	// Form 3: then branch and then a following if else statement
	contentElement = (new GridLayoutFormElement{})->setColumnStretchFactor(0, 1)->setRowStretchFactor(1, 1)
			->put(0, 0, thenBranch)
			->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;});

	auto elseIfBranch = item<VStatementItemList>(&I::elseBranch_, [](I* v){return v->node()->elseBranch();},
																[](I* v){return &v->style()->elseIfBranch();});

	auto elseContentElement = (new GridLayoutFormElement{})->setColumnStretchFactor(0, 1)->setRowStretchFactor(1, 1)
			->put(0, 0, elseIfBranch)
			->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;});

	addForm((new AnchorLayoutFormElement{})
			->put(TheTopOf, contentElement, 3, FromBottomOf, header)
			->put(TheTopOf, shapeElement, AtCenterOf, header)
			->put(TheLeftOf, shapeElement, AtLeftOf, header)
			->put(TheLeftOf, shapeElement, 10, FromLeftOf, contentElement)
			->put(TheRightOf, header, AtRightOf, contentElement)
			->put(TheRightOf, shapeElement, 10, FromRightOf, header)
			->put(TheBottomOf, shapeElement, AtBottomOf, elseContentElement)
			->put(TheTopOf, elseContentElement, 2, FromBottomOf, contentElement)
			->put(TheLeftOf, elseContentElement, AtLeftOf, shapeElement)
			->put(TheRightOf, elseContentElement, AtRightOf, shapeElement));
}

}
