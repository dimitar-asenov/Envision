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
#include "../src/items/TestBox.h"
#include "../src/items/VExtendable.h"
#include "ModelBase/src/test_nodes/BinaryNode.h"
#include "../src/declarative/GridLayoutElement.h"
#include "../src/declarative/AnchorLayoutElement.h"
#include "../src/declarative/SequentialLayoutElement.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(DeclarativeTest, "item")

DeclarativeTest::DeclarativeTest(Item* parent, TestNodes::BinaryNode* node, Model::Node* first, Model::Node* second,
		Model::Node* third, Model::Node* fourth) :
		DeclarativeItem<DeclarativeTest>(parent, itemStyles().get()), testNode_{node}, firstNode_{first},
		secondNode_{second}, thirdNode_{third}, fourthNode_{fourth}
{
	setPurpose(0);
}

void DeclarativeTest::initializeForms()
{
	Element* testItemElement = item<Symbol, I>(&I::testItem_, [](I*){return itemStyles().get();});
	Element* testNodeItemGeneralElement = item<I>(&I::testNodeItemGeneral_, [](I* v){return v->testNode_;});
	Element* testNodeItemElement = item<VExtendable,I>(&I::testNodeItem_, [](I* v){return v->testNode_;},
																		[](I*){return VExtendable::itemStyles().get();});
	Element* firstElement = item<I>(&I::firstItem_, [](I* v){return v->firstNode_;});
	Element* secondElement = item<I>(&I::secondItem_, [](I* v){return v->secondNode_;});
	Element* thirdElement = item<I>(&I::thirdItem_, [](I* v){return v->thirdNode_;});
	Element* fourthElement = item<I>(&I::fourthItem_, [](I* v){return v->fourthNode_;});

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
				->put(0, 0, firstElement)
				->put(0, 1, testNodeItemElement)
				->put(1, 1, (new GridLayoutElement())
									->put(0, 0, secondElement)
									->put(1, 1, thirdElement))
				->put(2, 0, fourthElement));

	// Test 4: GridLayoutElement with merged cells
	addForm((new GridLayoutElement())
				->setVerticalAlignment(LayoutStyle::Alignment::Center)
				->setHorizontalAlignment(LayoutStyle::Alignment::Center)
				->setColumnStretchFactor(1, 1)
				->setRowStretchFactors(1)
				->put(0, 0, (new GridLayoutElement())
									->setHorizontalSpacing(10)
									->put(0, 0, firstElement)
									->put(1, 0, secondElement)
									->put(2, 0, thirdElement))
				->setCellSpanning(2, 1)
				->put(2, 0, testNodeItemElement)
				->setCellSpanning(1, 2)
				->put(0, 1, fourthElement)
				);

	// Test 5: Size dependencies inside the grid & alignment
	addForm((new GridLayoutElement())
				->setVerticalAlignment(LayoutStyle::Alignment::Center)
				->put(0, 0, (new GridLayoutElement())
									->setHorizontalSpacing(300)
									->put(0, 0, firstElement)
									->put(1, 0, secondElement)))
				->put(0, 1, (new GridLayoutElement())
									->setColumnStretchFactor(1, 1)
									->put(0, 0, testNodeItemElement)
									->put(2, 0, thirdElement)
									->setCellVerticalAlignment(LayoutStyle::Alignment:: Center))
				->put(1, 0, fourthElement)
				->setCellSpanning(1, 2);

	// Test 6: Anchor layout
	addForm((new AnchorLayoutElement())
				->put(AnchorLayoutElement::PlaceEdge::BottomOf, firstElement, 20,
						AnchorLayoutElement::FromEdge::Above, testNodeItemElement)
				->put(AnchorLayoutElement::PlaceEdge::LeftOf, firstElement,
						AnchorLayoutElement::AtEdge::AtLeftOf, testNodeItemElement)
				->put(AnchorLayoutElement::PlaceEdge::VCenterOf, secondElement,
						AnchorLayoutElement::AtEdge::AtCenterOf, firstElement)
				->put(AnchorLayoutElement::PlaceEdge::LeftOf, secondElement, 20,
						AnchorLayoutElement::FromEdge::FromRightOf, firstElement)
				->put(AnchorLayoutElement::PlaceEdge::HCenterOf, thirdElement,
						AnchorLayoutElement::AtEdge::AtCenterOf, secondElement)
				->put(AnchorLayoutElement::PlaceEdge::TopOf, thirdElement, 20,
						AnchorLayoutElement::FromEdge::Below, testNodeItemElement)
				->put(AnchorLayoutElement::PlaceEdge::TopOf, fourthElement, 0.33, thirdElement)
				->put(AnchorLayoutElement::PlaceEdge::RightOf, fourthElement, 0.1, testNodeItemElement)
				);

	// Test 7: Anchor Layout with stretching Elements, in order specification
	Element* verticalGrid1 = (new GridLayoutElement)
										->setRowStretchFactors(1)
										->put(0, 0, firstElement)
										->setCellVerticalAlignment(LayoutStyle::Alignment::Top)
										->put(0, 1, secondElement)
										->setCellVerticalAlignment(LayoutStyle::Alignment::Center)
										->put(0, 2, thirdElement)
										->setCellVerticalAlignment(LayoutStyle::Alignment::Bottom);
	Element* verticalGrid2 = (new GridLayoutElement)
										->setRowStretchFactors(1)
										->put(0, 0, fourthElement)
										->setCellVerticalAlignment(LayoutStyle::Alignment::Top)
										->put(0, 1, testNodeItemElement)
										->setCellVerticalAlignment(LayoutStyle::Alignment::Bottom);
	addForm((new AnchorLayoutElement())
				->put(AnchorLayoutElement::PlaceEdge::TopOf, verticalGrid1,
						AnchorLayoutElement::AtEdge::AtTopOf, verticalGrid2)
				->put(AnchorLayoutElement::PlaceEdge::BottomOf, verticalGrid1,
						AnchorLayoutElement::AtEdge::AtBottomOf, verticalGrid2)
				->put(AnchorLayoutElement::PlaceEdge::LeftOf, verticalGrid1, 20,
						AnchorLayoutElement::FromEdge::FromRightOf, verticalGrid2));

	// Test 8: Anchor Layout out of order specification with more elements, circular dependencies
	addForm((new AnchorLayoutElement())
				// arrange them on top of each other
				->put(AnchorLayoutElement::PlaceEdge::TopOf, firstElement, 20,
						AnchorLayoutElement::FromEdge::Below, secondElement)
				->put(AnchorLayoutElement::PlaceEdge::TopOf, secondElement, 20,
						AnchorLayoutElement::FromEdge::Below, thirdElement)
				// right edges elements 1 and 2
				->put(AnchorLayoutElement::PlaceEdge::RightOf, firstElement,
						AnchorLayoutElement::AtEdge::AtRightOf, secondElement)
				// left edges elements 3 and 2
				->put(AnchorLayoutElement::PlaceEdge::LeftOf, thirdElement,
						AnchorLayoutElement::AtEdge::AtLeftOf, secondElement)
				// left edges elements 1 and 3
				->put(AnchorLayoutElement::PlaceEdge::LeftOf, firstElement,
						AnchorLayoutElement::AtEdge::AtLeftOf, thirdElement)
				// right edges elements 1 and 3
				->put(AnchorLayoutElement::PlaceEdge::RightOf, firstElement,
						AnchorLayoutElement::AtEdge::AtRightOf, thirdElement));

	// Test 9: Test sequential layout
	Element* horizontalSequential = (new SequentialLayoutElement())
			->setListOfItems([](){return QList<Item*>{new TestBox("a"), new TestBox("b"), new TestBox("cde")};});
	Element* verticalSequential = (new SequentialLayoutElement())
			->setListOfNodes([](){return QList<Model::Node*>{new TestBoxNode("dfsdfs", true),
																				new TestBoxNode("sfsdfsdf")};})
			->setVertical();

	addForm((new AnchorLayoutElement())
			->put(AnchorLayoutElement::PlaceEdge::LeftOf, horizontalSequential, 20,
					AnchorLayoutElement::FromEdge::FromRightOf, verticalSequential));
}

int DeclarativeTest::determineForm()
{
	return 9;
}

} /* namespace Visualization */
