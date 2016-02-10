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

	// The grid is needed to enable strectching
	auto elseIcon = grid({{item<Static>(&I::elseIcon_, &StyleType::elseIcon)}})
			->setColumnStretchFactor(1, 1)
			->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;});

	// The grid is needed to enable strectching
	auto thenBranch = grid({{item<VStatementItemList>(&I::thenBranch_, [](I* v){return v->node()->thenBranch();},
																[](I* v){return &v->style()->thenBranch();})}})
			->setColumnStretchFactor(1, 1)->setRowStretchFactor(0, 1)->setHorizontalSpacing(5)
			->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;});

	// The grid is needed to enable strectching
	auto elseBranch = grid({{item<VStatementItemList>(&I::elseBranch_, [](I* v){return v->node()->elseBranch();},
																[](I* v){return &v->style()->elseBranch();})}})
			->setColumnStretchFactor(1, 1)->setRowStretchFactor(0, 1)->setHorizontalSpacing(5)
			->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;});

	auto shapeElement = new ShapeFormElement{};

	// Form 0: no else branch
	addForm((new AnchorLayoutFormElement{})
			->put(TheTopOf, thenBranch, 3, FromBottomOf, header)
			->put(TheTopOf, shapeElement, AtCenterOf, header)
			->put(TheLeftOf, shapeElement, -10, FromLeftOf, header)
			->put(TheLeftOf, shapeElement, 5, FromLeftOf, thenBranch)
			->put(TheRightOf, header, AtRightOf, thenBranch)
			->put(TheRightOf, shapeElement, 3, FromRightOf, header)
			->put(TheBottomOf, shapeElement, 3, FromBottomOf, thenBranch));

	// Form 1: then and else branch arranged horizontally
	auto elseVerticalLineElement = item<Line>(&I::elseLine_, [](I* v){return &v->style()->elseVerticalLine();});

	addForm((new AnchorLayoutFormElement{})
			->put(TheVCenterOf, elseIcon, AtVCenterOf, header)
			->put(TheTopOf, thenBranch, 3, FromBottomOf, header)
			->put(TheTopOf, elseBranch, 3, FromBottomOf, header)
			->put(TheTopOf, shapeElement, AtCenterOf, header)
			->put(TheLeftOf, shapeElement, -10, FromLeftOf, header)
			->put(TheLeftOf, shapeElement, 5, FromLeftOf, thenBranch)
			->put(TheLeftOf, elseBranch, 5, FromHCenterOf, elseIcon)
			->put(TheLeftOf, elseIcon, 5, FromRightOf, header)
			->put(TheHCenterOf, elseIcon, 5, FromRightOf, thenBranch)
			->put(TheRightOf, shapeElement, 3, FromRightOf, elseBranch)
			->put(TheBottomOf, shapeElement, 3, FromBottomOf, thenBranch)
			->put(TheBottomOf, shapeElement, 3, FromBottomOf, elseBranch)

			->put(TheHCenterOf, elseVerticalLineElement, AtHCenterOf, elseIcon)
			->put(TheTopOf, elseVerticalLineElement, AtBottomOf, elseIcon)
			->put(TheBottomOf, elseVerticalLineElement, AtBottomOf, shapeElement));

	// Form 2: then and else branch arranged vertically
	auto elseHorizontalLineElement = item<Line>(&I::elseLine_, [](I* v){return &v->style()->elseHorizontalLine();});
	addForm((new AnchorLayoutFormElement{})
			  ->put(TheTopOf, thenBranch, 3, FromBottomOf, header)
			  ->put(TheLeftOf, elseIcon, AtLeftOf, header)
			  ->put(TheLeftOf, elseBranch, AtLeftOf, thenBranch)
			  ->put(TheTopOf, shapeElement, AtCenterOf, header)
			  ->put(TheLeftOf, shapeElement, -10, FromLeftOf, header)
			  ->put(TheLeftOf, shapeElement, 5, FromLeftOf, thenBranch)
			  ->put(TheTopOf, elseIcon, 3, FromBottomOf, thenBranch)
			  ->put(TheRightOf, shapeElement, 3, FromRightOf, thenBranch)
			  ->put(TheRightOf, shapeElement, 3, FromRightOf, elseBranch)
			  ->put(TheTopOf, elseBranch, 3, FromBottomOf, elseIcon)
			  ->put(TheBottomOf, shapeElement, 3, FromBottomOf, elseBranch)

			  ->put(TheVCenterOf, elseHorizontalLineElement, AtVCenterOf, elseIcon)
			  ->put(TheLeftOf, elseHorizontalLineElement, AtRightOf, elseIcon)
			  ->put(TheRightOf, elseHorizontalLineElement, AtRightOf, shapeElement));

	// Form 3: then branch and then a following if else statement
	auto elseIfBranch = item<VStatementItemList>(&I::elseBranch_, [](I* v){return v->node()->elseBranch();},
																[](I* v){return &v->style()->elseIfBranch();});

	auto elseContentElement = (new GridLayoutFormElement{})->setColumnStretchFactor(0, 1)->setRowStretchFactor(1, 1)
			->put(0, 0, elseIfBranch)
			->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;});

	addForm((new AnchorLayoutFormElement{})
			->put(TheTopOf, thenBranch, 3, FromBottomOf, header)
			->put(TheTopOf, shapeElement, AtCenterOf, header)
			->put(TheLeftOf, shapeElement, -10, FromLeftOf, header)
			->put(TheLeftOf, shapeElement, 5, FromLeftOf, thenBranch)
			->put(TheRightOf, header, AtRightOf, thenBranch)
			->put(TheRightOf, shapeElement, 3, FromRightOf, header)
			->put(TheBottomOf, shapeElement, AtBottomOf, elseContentElement)
			->put(TheTopOf, elseContentElement, 2, FromBottomOf, thenBranch)
			->put(TheLeftOf, elseContentElement, AtLeftOf, header)
			->put(TheRightOf, elseContentElement, AtRightOf, shapeElement));
}

}
