/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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

#include "DeclarativeTest.h"

#include "../src/items/Symbol.h"
#include "../src/items/Text.h"
#include "../src/items/VExtendable.h"
#include "ModelBase/src/test_nodes/BinaryNode.h"
#include "../src/declarative/GridLayoutElement.h"
#include "../src/declarative/AnchorLayoutElement.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(DeclarativeTest, "item")

DeclarativeTest::DeclarativeTest(Item* parent, TestNodes::BinaryNode* node) :
		DeclarativeItem<DeclarativeTest>(parent), testNode_{node}
{
	setPurpose(0);
}

void DeclarativeTest::initializeForms()
{
	Element* testItemElement = item<Symbol, I>(&I::testItem_, [](I*){return itemStyles().get();});
	Element* testItem2Element = item<Text, I>(&I::testItem2_, [](I*){return Text::itemStyles().get();});
	Element* testItem3Element = item<Text, I>(&I::testItem3_, [](I*){return Text::itemStyles().get();});
	Element* testItem4Element = item<Text, I>(&I::testItem4_, [](I*){return Text::itemStyles().get();});
	Element* testNodeItemGeneralElement = item<I>(&I::testNodeItemGeneral_, [](I* v){return v->testNode_;});
	Element* testNodeItemElement = item<VExtendable,I>(&I::testNodeItem_, [](I* v){return v->testNode_;},
																		[](I*){return VExtendable::itemStyles().get();});

	// Test 0: VisualizationItemWrapperElement
	addForm(testItemElement);

	// Test 1: NodeItemWrapperElement
	addForm(testNodeItemGeneralElement);

	// Test 2: NodeWithVisualizationItemWrapperElement
	addForm(testNodeItemElement);

	// Test 3: GridLayoutElement
	addForm((new GridLayoutElement())
				->setVerticalAlignment(LayoutStyle::Alignment::Center)
				->setHorizontalAlignment(LayoutStyle::Alignment::Right)
				->put(0, 0, testItemElement)
				->put(0, 1, testNodeItemElement)
				->put(1, 1, (new GridLayoutElement())
									->put(0, 0, testItem2Element)
									->put(1, 1, testItem3Element))
				->put(2, 0, testItem4Element));

	// Test 4: GridLayoutElement with merged cells
	addForm((new GridLayoutElement())
				->setVerticalAlignment(LayoutStyle::Alignment::Center)
				->setHorizontalAlignment(LayoutStyle::Alignment::Center)
				->setColumnStretchFactor(1, 1)
				->setRowStretchFactors(1)
				->put(0, 0, (new GridLayoutElement())
									->setHorizontalSpacing(10)
									->put(0, 0, testItemElement)
									->put(1, 0, testItem2Element)
									->put(2, 0, testItem3Element))
				->setCellSpanning(2, 1)
				->put(2, 0, testNodeItemElement)
				->setCellSpanning(1, 2)
				->put(0, 1, testItem4Element)
				);

	// Test 5: Size dependencies inside the grid & alignment
	addForm((new GridLayoutElement())
				->setVerticalAlignment(LayoutStyle::Alignment::Center)
				->put(0, 0, (new GridLayoutElement())
									->setHorizontalSpacing(300)
									->put(0, 0, testItemElement)
									->put(1, 0, testItem2Element)))
				->put(0, 1, (new GridLayoutElement())
									->setColumnStretchFactor(1, 1)
									->put(0, 0, testNodeItemElement)
									->put(2, 0, testItem3Element)
									->setCellVerticalAlignment(LayoutStyle::Alignment:: Center))
				->put(1, 0, testItem4Element)
				->setCellSpanning(1, 2);

	// Test 6: Anchor layout
	addForm((new AnchorLayoutElement())
				->put(AnchorLayoutElement::PlaceEdge::BottomOf, testItemElement, 20,
						AnchorLayoutElement::FromEdge::Above, testNodeItemElement)
				->put(AnchorLayoutElement::PlaceEdge::LeftOf, testItemElement,
						AnchorLayoutElement::AtEdge::AtLeftOf, testNodeItemElement)
				->put(AnchorLayoutElement::PlaceEdge::VCenterOf, testItem2Element,
						AnchorLayoutElement::AtEdge::AtCenterOf, testItemElement)
				->put(AnchorLayoutElement::PlaceEdge::LeftOf, testItem2Element, 20,
						AnchorLayoutElement::FromEdge::FromRightOf, testItemElement)
				->put(AnchorLayoutElement::PlaceEdge::HCenterOf, testItem3Element,
						AnchorLayoutElement::AtEdge::AtCenterOf, testItem2Element)
				->put(AnchorLayoutElement::PlaceEdge::TopOf, testItem3Element, 20,
						AnchorLayoutElement::FromEdge::Below, testNodeItemElement)
				->put(AnchorLayoutElement::PlaceEdge::TopOf, testItem4Element, 0.33, testItem3Element)
				->put(AnchorLayoutElement::PlaceEdge::RightOf, testItem4Element, 0.1, testNodeItemElement)
				);

	// Test 7: Anchor Layout with stretching Elements, vertically
	Element* stretchableGrid1 = (new GridLayoutElement)
										->setColumnStretchFactors(1)
										->put(0, 0, testItemElement)
										->setCellHorizontalAlignment(LayoutStyle::Alignment::Left)
										->put(1, 0, testItem2Element)
										->setCellHorizontalAlignment(LayoutStyle::Alignment::Center)
										->put(2, 0, testItem3Element)
										->setCellHorizontalAlignment(LayoutStyle::Alignment::Right);
	Element* stretchableGrid2 = (new GridLayoutElement)
										->setColumnStretchFactors(1)
										->put(0, 0, testItem4Element)
										->setCellHorizontalAlignment(LayoutStyle::Alignment::Left)
										->put(1, 0, testNodeItemElement)
										->setCellHorizontalAlignment(LayoutStyle::Alignment::Right);
	addForm((new AnchorLayoutElement())
				->put(AnchorLayoutElement::PlaceEdge::RightOf, stretchableGrid1,
						AnchorLayoutElement::AtEdge::AtRightOf, stretchableGrid2)
				->put(AnchorLayoutElement::PlaceEdge::LeftOf, stretchableGrid1,
						AnchorLayoutElement::AtEdge::AtLeftOf, stretchableGrid2)
				->put(AnchorLayoutElement::PlaceEdge::TopOf, stretchableGrid1, 20,
						AnchorLayoutElement::FromEdge::Below, stretchableGrid2));

	// Test 8: Anchor Layout with stretching Elements, horizontally
	Element* stretchableGrid3 = (new GridLayoutElement)
										->setRowStretchFactors(1)
										->put(0, 0, testItemElement)
										->setCellVerticalAlignment(LayoutStyle::Alignment::Top)
										->put(0, 1, testItem2Element)
										->setCellVerticalAlignment(LayoutStyle::Alignment::Center)
										->put(0, 2, testItem3Element)
										->setCellVerticalAlignment(LayoutStyle::Alignment::Bottom);
	Element* stretchableGrid4 = (new GridLayoutElement)
										->setRowStretchFactors(1)
										->put(0, 0, testItem4Element)
										->setCellVerticalAlignment(LayoutStyle::Alignment::Top)
										->put(0, 1, testNodeItemElement)
										->setCellVerticalAlignment(LayoutStyle::Alignment::Bottom);
	addForm((new AnchorLayoutElement())
				->put(AnchorLayoutElement::PlaceEdge::TopOf, stretchableGrid3,
						AnchorLayoutElement::AtEdge::AtTopOf, stretchableGrid4)
				->put(AnchorLayoutElement::PlaceEdge::BottomOf, stretchableGrid3,
						AnchorLayoutElement::AtEdge::AtBottomOf, stretchableGrid4)
				->put(AnchorLayoutElement::PlaceEdge::LeftOf, stretchableGrid3, 20,
						AnchorLayoutElement::FromEdge::FromRightOf, stretchableGrid4));
}

int DeclarativeTest::determineForm()
{
	return 7;
}

void DeclarativeTest::determineChildren()
{
	BaseItemType::determineChildren();
	if (testItem2_) testItem2_->setText("second");
	if (testItem3_) testItem3_->setText("third");
	if (testItem4_) testItem4_->setText("fourth");
}

} /* namespace Visualization */
